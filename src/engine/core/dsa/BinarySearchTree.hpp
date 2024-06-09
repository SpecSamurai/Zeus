#pragma once

#include <iostream>
#include <queue>

namespace Zeus
{
template <typename TValue>
class BinarySearchTree
{
public:
    BinarySearchTree(
        TValue value,
        BinarySearchTree* left = nullptr,
        BinarySearchTree* right = nullptr)
        : m_left{ left },
          m_right{ right },
          m_value{ value }
    {
    }

    BinarySearchTree* m_left;
    BinarySearchTree* m_right;
    TValue m_value;

    void InsertRecursive(TValue value)
    {
        if (value <= m_value)
        {
            if (m_left == nullptr)
            {
                m_left = new BinarySearchTree(value);
            }
            else
            {
                m_left->InsertRecursive(value);
            }
        }
        else
        {
            if (m_right == nullptr)
            {
                m_right = new BinarySearchTree(value);
            }
            else
            {
                m_right->InsertRecursive(value);
            }
        }
    }

    void Insert(TValue value)
    {
        BinarySearchTree* node{ this };

        while (node != nullptr)
        {
            if (value <= node->m_value)
            {
                if (node->m_left != nullptr)
                    node = node->m_left;
                else
                {
                    node->m_left = new BinarySearchTree(value);
                    break;
                }
            }
            else
            {
                if (node->m_right != nullptr)
                    node = node->m_right;
                else
                {
                    node->m_right = new BinarySearchTree(value);
                    break;
                }
            }
        }
    }

    bool Contains(TValue value)
    {
        if (value == m_value)
        {
            return true;
        }
        else if (value < m_value)
        {
            if (m_left == nullptr)
                return false;
            else
                return m_left->Contains(value);
        }
        else
        {
            if (m_right == nullptr)
                return false;
            else
                return m_right->Contains(value);
        }
    }

    BinarySearchTree<TValue>* Delete(
        BinarySearchTree<TValue>* root,
        TValue value)
    {
        if (root)
        {
            if (value < root->m_value)
                root->m_left = Delete(root->m_left, value);
            else if (value > root->m_value)
                root->m_right = Delete(root->m_right, value);
            else
            {
                if (!root->m_left && !root->m_right)
                {
                    delete root;
                    return nullptr;
                }
                else if (root->m_left != nullptr ^ root->m_right != nullptr)
                {
                    auto temp = root->m_left ? root->m_left : root->m_right;
                    delete root;
                    return temp;
                }
                else
                {
                    // Fin min on the right side
                    auto min = root->m_right;
                    while (min->m_left)
                        min = min->m_left;

                    root->m_value = min->m_value;
                    root->m_right = Delete(root->m_right, min->m_value);
                }
            }
        }
        return root;
    }

    void PrintInOrder()
    {
        if (m_left != nullptr)
            m_left->PrintInOrder();

        std::cerr << m_value << " ";

        if (m_right != nullptr)
            m_right->PrintInOrder();
    }

    void PrintPreOrder()
    {
        std::cerr << m_value << " ";

        if (m_left != nullptr)
            m_left->PrintInOrder();

        if (m_right != nullptr)
            m_right->PrintInOrder();
    }

    void PrintPostOrder()
    {
        if (m_left != nullptr)
            m_left->PrintInOrder();

        if (m_right != nullptr)
            m_right->PrintInOrder();

        std::cerr << m_value << " ";
    }

    void PrintBFS()
    {
        std::queue<BinarySearchTree<TValue>*> queue;
        queue.push(this);

        std::size_t size{ 0 };

        while (!queue.empty())
        {
            size = queue.size();

            while (size--)
            {
                auto front = queue.front();
                queue.pop();

                std::cerr << front->m_value << " ";

                if (front->m_left != nullptr)
                    queue.push(front->m_left);

                if (front->m_right != nullptr)
                    queue.push(front->m_right);
            }

            std::cerr << std::endl;
        }
    }
};
}
