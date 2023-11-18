#if FaroToolTest
#include <gtest/gtest.h>
#include "Commandline.hpp"
#include "FileTree.hpp"

TEST(FileTree, MissingModule)
{
    FileTree tree(nullptr);
    EXPECT_NO_FATAL_FAILURE(tree.Parse());
}
#endif
