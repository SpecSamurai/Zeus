#include <core/dsa/BinarySearchTree.hpp>

#include "gtest/gtest.h"

TEST(BinarySearchTreeTest, Contains)
{
    Zeus::BinarySearchTree<int> node(10);
    node.Insert(0);
    node.Insert(2);
    node.Insert(3);
    node.Insert(11);
    node.Insert(5);
    node.Insert(9);

    EXPECT_FALSE(node.Contains(1));
    EXPECT_TRUE(node.Contains(9));
    EXPECT_TRUE(node.Contains(11));
}

TEST(BinarySearchTreeTest, DFS)
{
    Zeus::BinarySearchTree<int> node(10);
    node.Insert(0);
    node.Insert(2);
    node.Insert(3);
    node.Insert(11);
    node.Insert(5);
    node.Insert(9);

    std::cerr << "\nIn order: ";
    node.PrintInOrder();

    std::cerr << "\nPre order: ";
    node.PrintPreOrder();

    std::cerr << "\nPost order: ";
    node.PrintPostOrder();
}

TEST(BinarySearchTreeTest, BFS)
{
    Zeus::BinarySearchTree<int> node(10);
    node.Insert(0);
    node.Insert(-1);
    node.Insert(3);
    node.Insert(12);
    node.Insert(5);
    node.Insert(11);

    node.PrintBFS();
}
