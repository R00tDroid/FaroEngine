#include "Reflector.hpp"
#include <filesystem>
#include <fstream>
#include "ModuleInfo.hpp"
#include <string>
#include <string_view>
#include <tree_sitter/api.h>

extern "C" const TSLanguage* tree_sitter_cpp(void);

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
    for (uint32_t i = 0; i < child_count; i++) {
        TSNode child = ts_node_named_child(node, i);
        print_node(child, indentation + 1, fileBuffer);
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
