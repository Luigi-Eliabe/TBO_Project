#include <iostream>
#include <string>
#include <map>
#include <vector>
#include <queue>
#include <fstream>

class TrieNode {
public:
    std::map<char, TrieNode*> children;
    bool isEndOfWord;

    TrieNode() {
        isEndOfWord = false;
    }
};

class Trie {
public:
    TrieNode* root;

    Trie() {
        root = new TrieNode();
    }

    void insert(const std::string& word) {
        TrieNode* current = root;

        for (char c : word) {
            if (current->children.find(c) == current->children.end()) {
                current->children[c] = new TrieNode();
            }
            current = current->children[c];
        }

        current->isEndOfWord = true;
    }

    bool search(const std::string& word) {
        TrieNode* current = root;

        for (char c : word) {
            if (current->children.find(c) == current->children.end()) {
                return false;
            }
            current = current->children[c];
        }

        return current != nullptr && current->isEndOfWord;
    }
};

void dfs(TrieNode* node, const std::string& currentWord, const std::string& targetWord, int index, int distance, int maxDistance, std::vector<std::string>& suggestions) {
    if (index == targetWord.length()) {
        if (node->isEndOfWord && distance <= maxDistance) {
            suggestions.push_back(currentWord);
        }
        return;
    }

    char currentChar = targetWord[index];
    for (const auto& child : node->children) {
        char childChar = child.first;
        TrieNode* nextNode = child.second;

        int newDistance = distance + (currentChar == childChar ? 0 : 1);

        if (newDistance <= maxDistance) {
            dfs(nextNode, currentWord + childChar, targetWord, index + 1, newDistance, maxDistance, suggestions);
        }
    }
}

void generateCorrections(const std::string& word, Trie* dictionary, std::vector<std::string>& suggestions, int maxDistance) {
    dfs(dictionary->root, "", word, 0, 0, maxDistance, suggestions);
}

std::vector<std::string> readDictionaryFromFile(const std::string& filename) {
    std::vector<std::string> words;

    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Error opening file: " << filename << std::endl;
        return words; // Return empty vector if file couldn't be opened
    }

    std::string word;
    while (file >> word) {
        words.push_back(word);
    }

    file.close();
    return words;
}

int main() {
    Trie* dictionary = new Trie();

    // Mock dictionary
    // std::vector<std::string> mockDictionary = {"apple", "banana", "banena", "banina", "banine", "orange", "grape", "peach", "pear"};
    std::string dictionaryFile = "pt_BR.dic"; // Replace with your ".dic" file path

    std::vector<std::string> mockDictionary = readDictionaryFromFile(dictionaryFile);

    if (!mockDictionary.empty()) {
        std::cout << "Dictionary loaded successfully. Words read: " << mockDictionary.size() << std::endl;
        // You can use 'dictionary' vector containing words read from the file here
    } else {
        std::cout << "Empty dictionary or error reading the file." << std::endl;
    }
    for (const std::string& word : mockDictionary) {
        dictionary->insert(word);
    }

    std::string inputWord;
    std::cout << "Enter a word: ";
    std::cin >> inputWord;

    int maxDistance = 2; // Maximum edit distance for suggestions

    if (dictionary->search(inputWord)) {
        std::cout << "The word is valid.\n";
    } else {
        std::vector<std::string> suggestedCorrections;
        generateCorrections(inputWord, dictionary, suggestedCorrections, maxDistance);

        if (!suggestedCorrections.empty()) {
            std::cout << "Possible corrections:\n";
            for (const std::string& correction : suggestedCorrections) {
                std::cout << correction << "\n";
            }
        } else {
            std::cout << "No suggestions found.\n";
        }
    }

    return 0;
}
