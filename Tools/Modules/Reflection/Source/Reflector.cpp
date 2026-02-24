#include "Reflector.hpp"
#include <filesystem>
#include <fstream>
#include "ModuleInfo.hpp"
#include <string>
#include <string_view>
#include <tree_sitter/api.h>

extern "C" const TSLanguage* tree_sitter_cpp(void);

struct ReflectHeader
{
    std::vector<std::string> arguments;
};

bool expect_child_node(const TSNode& parent, const char* nodeType, TSNode& node)
{
    uint32_t child_count = ts_node_named_child_count(parent);
    for (uint32_t i = 0; i < child_count; i++) {
        TSNode child = ts_node_named_child(parent, i);
        std::string childNodeType = ts_node_type(child);
        if (childNodeType == nodeType)
        {
            node = child;
            return true;
        }
    }

    return false;
}

std::string node_text(const TSNode& node, const std::string& fileBuffer)
{
    uint32_t textStart = ts_node_start_byte(node);
    uint32_t textEnd = ts_node_end_byte(node);
    return fileBuffer.substr(textStart, textEnd - textStart);
}

bool parse_reflect_header(TSNode& node, ReflectHeader& header, const std::string& fileBuffer)
{
    header = {};

    std::string nodeType = ts_node_type(node);
    if (nodeType == "expression_statement")
    {
        TSNode expression;
        if (expect_child_node(node, "call_expression", expression))
        {
            TSNode id;
            if (expect_child_node(expression, "identifier", id))
            {
                if (node_text(id, fileBuffer) == "Reflect")
                {
                    header = {}; //TODO Parse flags
                    return true;
                }
            }
        }
    }
    else if (nodeType == "declaration")
    {
        TSNode declaration;
        if (expect_child_node(node, "function_declarator", declaration))
        {
            TSNode id;
            if (expect_child_node(declaration, "identifier", id))
            {
                if (node_text(id, fileBuffer) == "Reflect")
                {
                    header = {};
                    return true;
                }
            }
        }
    }
    else if (nodeType == "field_declaration")
    {
        TSNode id;
        if (expect_child_node(node, "type_identifier", id))
        {
            if (node_text(id, fileBuffer) == "Reflect")
            {
                header = {}; //TODO Parse flags
                return true;
            }
        }
    }

    return false;
}

struct ReflectedEntry
{
    std::string name = "";
};

bool reflect_type(TSNode& node, ReflectedEntry& entry, const std::string& fileBuffer)
{
    entry = {};

    std::string nodeType = ts_node_type(node);
    if (nodeType == "class_specifier")
    {
        TSNode id;
        if (expect_child_node(node, "type_identifier", id))
        {
            entry.name = node_text(id, fileBuffer);
            return true;
        }
    }
    else if (nodeType == "field_declaration")
    {
        TSNode id;
        if (expect_child_node(node, "field_identifier", id))
        {
            entry.name = node_text(id, fileBuffer);
            return true;
        }
    }
    else if (nodeType == "function_definition")
    {
        TSNode declaration;
        if (expect_child_node(node, "function_declarator", declaration))
        {
            TSNode id;
            if (expect_child_node(declaration, "field_identifier", id))
            {
                entry.name = node_text(id, fileBuffer);
                return true;
            }
        }
    }

    return false;
}

void print_node(TSNode& node, unsigned int indentation, std::string& fileBuffer)
{
    TSPoint start = ts_node_start_point(node);
    
    std::string nodeType = ts_node_type(node);
    std::string text = {};
    
    if (nodeType.find("identifier") != std::string::npos) {
        uint32_t textStart = ts_node_start_byte(node);
        uint32_t textEnd = ts_node_end_byte(node);
        text = fileBuffer.substr(textStart, textEnd - textStart);
    }

    Utility::PrintLineD(std::string(indentation * 2, ' ') + " -" + nodeType + " (" + std::to_string(start.row) + ", " + std::to_string(start.column) + ") " + text);

    uint32_t child_count = ts_node_named_child_count(node);
    bool reflecting = false;
    ReflectHeader header;

    for (uint32_t i = 0; i < child_count; i++) {
        TSNode child = ts_node_named_child(node, i);

        print_node(child, indentation + 1, fileBuffer);

        if (reflecting)
        {
            ReflectedEntry entry;
            if (reflect_type(node, entry, fileBuffer))
            {
                Utility::PrintLineD("Reflected " + entry.name);
            }
            else
            {
                Utility::PrintLine("Failed to reflect");
            }
        }

        header = {};
        reflecting = parse_reflect_header(child, header, fileBuffer);

        if (reflecting)
        {
            TSPoint reflectMarker = ts_node_start_point(node);
	        Utility::PrintLineD("Reflecting " + std::to_string(reflectMarker.row) + ", " + std::to_string(reflectMarker.column));
            //TODO Save reflected type
        }
    }
}

bool Reflector::generateFileReflection(const ModuleManifest* moduleManifest, unsigned int fileIndex)
{
    std::filesystem::path file = moduleManifest->sourceFile(fileIndex);
    std::ifstream fileStream(file);

    fileStream.seekg(0, std::ios::end);
    size_t size = fileStream.tellg();
    std::string fileBuffer(size, ' ');
    fileStream.seekg(0);
    fileStream.read(fileBuffer.data(), size);

    //TODO What to do when nothing is reflect-able?
    if (fileBuffer.find("Reflect") == std::string::npos) return true;

    TSParser* tsParser = ts_parser_new();
    if (!ts_parser_set_language(tsParser, tree_sitter_cpp()))
    {
        Utility::PrintLine("Failed to setup parser");
        return false;
    }

    TSTree* tree = ts_parser_parse_string(
        tsParser,
        nullptr,
        fileBuffer.c_str(),
        static_cast<uint32_t>(fileBuffer.length())
    );

    TSNode root = ts_tree_root_node(tree);
    print_node(root, 0, fileBuffer);

    ts_tree_delete(tree);
    ts_parser_delete(tsParser);

    return true;
}

bool Reflector::generateModuleReflection(const ModuleManifest*)
{
    return true;
}
