#include <bits/stdc++.h>

using namespace std;
class TrieNode {
public:
    map<char, TrieNode*> children;
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

    void insert(const string& word) {
        TrieNode* current = root;

        for (char c : word) {
            if (current->children.find(c) == current->children.end()) {
                current->children[c] = new TrieNode();
            }
            current = current->children[c];
        }

        current->isEndOfWord = true;
    }

    bool search(const string& word) {
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
void dfs(TrieNode* node, const string& currentWord, const string& targetWord, int index, int distance, int maxDistance, vector<string>& suggestions) {
    // if (index == targetWord.length()) {
    if (distance > maxDistance)
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


void generateCorrections(const string& word, Trie* dictionary, vector<string>& suggestions, int maxDistance) {
    dfs(dictionary->root, "", word, 0, 0, maxDistance, suggestions);
}

vector<string> readDictionaryFromFile(const string& filename) {
    vector<string> words;

    ifstream file(filename);
    if (!file.is_open()) {
        cerr << "Error opening file: " << filename << endl;
        return words; // Return empty vector if file couldn't be opened
    }

    string word;
    while (file >> word) {
        words.push_back(word);
    }

    file.close();
    return words;
}

int main() {
    Trie* dictionary = new Trie();

    // Mock dictionary
    // vector<string> mockDictionary = {"apple", "banana", "banena", "banina", "banine", "orange", "grape", "peach", "pear"};
    string dictionaryFile = "pt_BR.dic"; // Replace with your ".dic" file path

    vector<string> mockDictionary = readDictionaryFromFile(dictionaryFile);

    if (!mockDictionary.empty()) {
        cout << "Dictionary loaded successfully. Words read: " << mockDictionary.size() << endl;
        // You can use 'dictionary' vector containing words read from the file here
    }
    else {
        cout << "Empty dictionary or error reading the file." << endl;
    }
    for (const string& word : mockDictionary) {
        dictionary->insert(word);
    }

    string inputWord;
    cout << "Enter a word: ";
    cin >> inputWord;

    int maxDistance = 2; // Maximum edit distance for suggestions

    if (dictionary->search(inputWord)) {
        cout << "The word is valid.\n";
    }
    else {
        vector<string> suggestedCorrections;
        generateCorrections(inputWord, dictionary, suggestedCorrections, maxDistance);

        if (!suggestedCorrections.empty()) {
            cout << "Possible corrections:\n";
            for (const string& correction : suggestedCorrections) {
                cout << correction << "\n";
            }
        }
        else {
            cout << "No suggestions found.\n";
        }
    }

    return 0;
}
