#pragma once

#include <string>
#include <unordered_map>
#include <vector>

namespace Zeus
{
class Trie
{
public:
    Trie() : data{ 0 }, children{ std::unordered_map<char, Trie*>() }
    {
    }

    int data;
    std::unordered_map<char, Trie*> children;

    static Trie* build(std::vector<std::string> words)
    {
        Trie* root = new Trie();
        Trie* current{ nullptr };

        for (auto word : words)
        {
            current = root;
            for (char c : word)
            {
                if (current->children.find(c) == current->children.end())
                {
                    current->children[c] = new Trie();
                }
                current = current->children[c];
            }
        }

        return root;
    }
};
}
