#include <iostream>
#include <unordered_map>
#include <vector>
using namespace std;

class TrieNode {
public:
    unordered_map<char, TrieNode*> children;
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

    void insert(string word) {
        TrieNode* node = root;
        for (char c : word) {
            if (node->children.find(c) == node->children.end()) {
                node->children[c] = new TrieNode();
            }
            node = node->children[c];
        }
        node->isEndOfWord = true;
    }

    bool search(string word) {
        TrieNode* node = root;
        for (char c : word) {
            if (node->children.find(c) == node->children.end()) {
                return false;
            }
            node = node->children[c];
        }
        return node != nullptr && node->isEndOfWord;
    }

    void suggestionsUtil(TrieNode* node, string currentWord, vector<string>& suggestions) {
        if (node->isEndOfWord) {
            suggestions.push_back(currentWord);
        }

        for (auto& child : node->children) {
            suggestionsUtil(child.second, currentWord + child.first, suggestions);
        }
    }

    vector<string> suggestCorrections(string word) {
        vector<string> suggestions;
        TrieNode* node = root;

        for (char c : word) {
            if (node->children.find(c) == node->children.end()) {
                return suggestions;
            }
            node = node->children[c];
        }

        suggestionsUtil(node, word, suggestions);
        return suggestions;
    }

    string correctWord(string word) {
        if (search(word)) {
            return word;
        }

        vector<string> suggestions = suggestCorrections(word);
        if (!suggestions.empty()) {
            return suggestions[0]; // Return first suggestion as the correction
        } else {
            return ""; // If no suggestions found, return an empty string
        }
    }
};

vector<int> computeLPSArray(const string& pattern) {
    int patternLength = pattern.length();
    vector<int> lps(patternLength, 0);
    int len = 0; // Length of the previous longest prefix suffix

    int i = 1;
    while (i < patternLength) {
        if (pattern[i] == pattern[len]) {
            len++;
            lps[i] = len;
            i++;
        } else {
            if (len != 0) {
                len = lps[len - 1];
            } else {
                lps[i] = 0;
                i++;
            }
        }
    }
    return lps;
}

vector<int> KMP(const string& text, const string& pattern) {
    vector<int> lps = computeLPSArray(pattern);
    vector<int> matches;
    int textLength = text.length();
    int patternLength = pattern.length();
    int i = 0; // Index for text[]
    int j = 0; // Index for pattern[]

    while (i < textLength) {
        if (pattern[j] == text[i]) {
            j++;
            i++;
        }
        if (j == patternLength) {
            matches.push_back(i - j);
            j = lps[j - 1];
        } else if (i < textLength && pattern[j] != text[i]) {
            if (j != 0) {
                j = lps[j - 1];
            } else {
                i++;
            }
        }
    }
    return matches;
}

int main() {
    vector<string> wordList = {"apple", "banana", "application", "app", "bat", "ball"};

    Trie trie;
    for (const string& word : wordList) {
        trie.insert(word);
    }

    string wordToCheck = "appl";
    cin >> wordToCheck;
    string correctedWord = trie.correctWord(wordToCheck);
    if (!correctedWord.empty()) {
        cout << "Did you mean: " << correctedWord << endl;
    } else {
        cout << "No suggestions found." << endl;
    }

    string texto = "UIUUFSDUBFKDBVJHBKVKABVHJFBVHJBHJFBVHBVBUABVUFFRBARUNEBVHRABJHEBHJABVHJABVHJDBADUFCNDUFJHA";
    string palavra = "UF";

    for( auto x : KMP(texto,palavra)){
        cout << " " << x;
    }

    return 0;
}
