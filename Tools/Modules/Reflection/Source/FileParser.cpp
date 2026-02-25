#include "FileParser.hpp"
#include <tree_sitter/api.h>
#include "Utility.hpp"

FileReflector fileReflector;

struct ReflectHeader
{
    std::vector<std::string> arguments;
};

struct ReflectionInfo
{
    const std::string& file;
    std::vector<::ReflectedEntry*>& entries;
};

extern "C" const TSLanguage* tree_sitter_cpp(void);

struct FileReflector::Implementation
{
    TSParser* parser = nullptr;

    bool reflect(const std::string& file, std::vector<::ReflectedEntry*>& entries);

    bool reflect_node(ReflectionInfo& info, TSNode& node);
    bool find_reflection_header(ReflectionInfo& info, TSNode& node, ReflectHeader& header);
    ReflectedEntry* reflect_type(ReflectionInfo& info, TSNode& node, const ReflectHeader& header);

    bool expect_child_node(const TSNode& parent, const char* nodeType, TSNode& node) const;
    std::string get_node_text(const TSNode& node, const std::string& fileBuffer) const;
};

FileReflector::FileReflector() : impl(new Implementation)
{
    impl->parser = ts_parser_new();
    if (!ts_parser_set_language(impl->parser, tree_sitter_cpp()))
    {
        Utility::PrintLine("Failed to setup parser");
    }
}

FileReflector::~FileReflector()
{
    ts_parser_delete(impl->parser);

    delete impl;
    impl = nullptr;
}

bool FileReflector::reflect(const std::string& file, std::vector<::ReflectedEntry*>& entries)
{
    return impl->reflect(file, entries);
}

bool FileReflector::Implementation::reflect(const std::string& file, std::vector<::ReflectedEntry*>& entries)
{
    if (file.find("Reflect") == std::string::npos) return true;

    TSTree* tree = ts_parser_parse_string(parser, nullptr, file.c_str(), static_cast<uint32_t>(file.length()));

    TSNode root = ts_tree_root_node(tree);

    ReflectionInfo info = {
        file,
	    entries
    };

    bool result = reflect_node(info, root);

    ts_tree_delete(tree);

    return result;
}

bool FileReflector::Implementation::reflect_node(ReflectionInfo& info, TSNode& node)
{
    uint32_t child_count = ts_node_named_child_count(node);
    bool has_header = false;
    ReflectHeader header;

    for (uint32_t i = 0; i < child_count; i++) {
        TSNode child = ts_node_named_child(node, i);
        if (has_header)
        {
            ReflectedEntry* reflected = reflect_type(info, child, header);
            if (reflected == nullptr)
            {
                Utility::PrintLine("Failed to reflect"); //TODO Log file location
                return false;
            }
            else
            {
                info.entries.push_back(reflected);
            }
        }

        header = {};
        has_header = find_reflection_header(info, child, header);

        reflect_node(info, child);
    }

    return true;
}

bool FileReflector::Implementation::find_reflection_header(ReflectionInfo& info, TSNode& node, ReflectHeader& header)
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
                if (get_node_text(id, info.file) == "Reflect")
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
                if (get_node_text(id, info.file) == "Reflect")
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
            if (get_node_text(id, info.file) == "Reflect")
            {
                header = {}; //TODO Parse flags
                return true;
            }
        }
    }

    return false;
}

ReflectedEntry* FileReflector::Implementation::reflect_type(ReflectionInfo& info, TSNode& node, const ReflectHeader&)
{
    //TODO Handle arguments from header
    //TODO Handle parent relationship (class/struct members)

    std::string nodeType = ts_node_type(node);
    if (nodeType == "class_specifier")
    {
        TSNode id;
        if (expect_child_node(node, "type_identifier", id))
        {
            ReflectedClass* entry = new ReflectedClass();
            entry->name = get_node_text(id, info.file);
            return entry;
        }
    }
    else if (nodeType == "field_declaration")
    {
        TSNode id;
        if (expect_child_node(node, "field_identifier", id))
        {
            ReflectedVariable* entry = new ReflectedVariable();
            entry->name = get_node_text(id, info.file);
            return entry;
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
                ReflectedFunction* entry = new ReflectedFunction();
                entry->name = get_node_text(id, info.file);
                return entry;
            }
        }
    }

    return nullptr;
}

bool FileReflector::Implementation::expect_child_node(const TSNode& parent, const char* nodeType, TSNode& node) const
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

std::string FileReflector::Implementation::get_node_text(const TSNode& node, const std::string& fileBuffer) const
{
    uint32_t textStart = ts_node_start_byte(node);
    uint32_t textEnd = ts_node_end_byte(node);
    return fileBuffer.substr(textStart, textEnd - textStart);
}
