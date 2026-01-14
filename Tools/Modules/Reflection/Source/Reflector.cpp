#include "Reflector.hpp"
#include <filesystem>
#include <fstream>
#include "ModuleInfo.hpp"
#include <string>
#include <string_view>
#include <tree_sitter/api.h>

extern "C" const TSLanguage* tree_sitter_cpp(void);

bool Reflector::generateFileReflection(const ModuleManifest* moduleManifest, unsigned int fileIndex)
{
    std::filesystem::path file = moduleManifest->sourceFile(fileIndex);
    std::ifstream fileStream(file);

    fileStream.seekg(0, std::ios::end);
    size_t size = fileStream.tellg();
    std::string fileBuffer(size, ' ');
    fileStream.seekg(0);
    fileStream.read(fileBuffer.data(), size);

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

    TSNode root_node = ts_tree_root_node(tree);

    char* tree_string = ts_node_string(root_node);
    //Utility::PrintLine(file.filename().string() + ": " + tree_string);
    free(tree_string);

    const char* query_string = R"(
    (
        (expression_statement 
            (call_expression 
                (identifier) @macro_name 
                (argument_list) @flags)
        ) 
        
        . 

        [
            (class_specifier name: (_) @class_name)
            (field_declaration _ (_) @field_name)
            (field_declaration _ (function_declarator (_) @field_name))
            (declaration _ (_) @field_name)
            (declaration _ (function_declarator (_) @field_name))
        ]

        ; Predicate to filter for "Reflect"
        (#eq? @macro_name "Reflect")
    )
)";

    uint32_t error_offset;
    TSQueryError error_type;

    TSQuery* query = ts_query_new(
        tree_sitter_cpp(),
        query_string,
        static_cast<uint32_t>(strlen(query_string)),
        &error_offset,
        &error_type
    );

    if (query == nullptr)
    {
        std::string error_context = std::string(query_string).substr(error_offset, 20);

        Utility::PrintLine("Failed to setup parsing query: " + std::to_string(error_type) + ", " + error_context);
        return false;
    }

    TSQueryCursor* cursor = ts_query_cursor_new();
    ts_query_cursor_exec(cursor, query, root_node);

    TSQueryMatch match;
    while (ts_query_cursor_next_match(cursor, &match)) {
        std::string current_flags = "";

        for (uint16_t i = 0; i < match.capture_count; i++) {
            TSQueryCapture capture = match.captures[i];
            uint32_t name_len;
            std::string capture_name = ts_query_capture_name_for_id(query, capture.index, &name_len);

            uint32_t start = ts_node_start_byte(capture.node);
            uint32_t end = ts_node_end_byte(capture.node);
            std::string text = fileBuffer.substr(start, end - start);

            if (capture_name == "flags") {
                current_flags = text.substr(1, text.length() - 2);
            }
            else if (capture_name == "class_name") {
                Utility::PrintLineD("Found Reflected Class: " + text + " with flags: [" + current_flags + "]");
            }
            else if (capture_name == "field_name") {
                Utility::PrintLineD("Found Reflected Field: " + text + " with flags: [" + current_flags + "]");
            }
        }
    }

    ts_query_cursor_delete(cursor);
    ts_query_delete(query);
    ts_tree_delete(tree);
    ts_parser_delete(tsParser);

    return true;
}

bool Reflector::generateModuleReflection(const ModuleManifest*)
{
    return true;
}
