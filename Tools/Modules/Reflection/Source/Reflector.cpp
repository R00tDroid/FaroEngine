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
        return false;
    }

    TSTree* tree = ts_parser_parse_string(
        tsParser,
        nullptr,
        fileBuffer.c_str(),
        static_cast<uint32_t>(fileBuffer.length())
    );

    TSNode root_node = ts_tree_root_node(tree);

    const char* query_string = R"(
        (class_specifier 
            name: (type_identifier) @class_name
            body: (field_declaration_list
                (field_declaration 
                    declarator: (field_identifier) @member_name)
            )
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

    TSQueryCursor* cursor = ts_query_cursor_new();
    ts_query_cursor_exec(cursor, query, root_node);

    TSQueryMatch match;
    while (ts_query_cursor_next_match(cursor, &match)) {
        for (uint16_t i = 0; i < match.capture_count; i++) {
            TSQueryCapture capture = match.captures[i];

            uint32_t name_len;
            std::string capture_name = ts_query_capture_name_for_id(query, capture.index, &name_len);

            uint32_t start = ts_node_start_byte(capture.node);
            uint32_t end = ts_node_end_byte(capture.node);
            std::string text = fileBuffer.substr(start, end - start);

            Utility::PrintLineD(capture_name + ": " + text);
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
