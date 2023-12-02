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

int levenshteinDistance(const string& s1, const string& s2) {
    int m = s1.length();
    int n = s2.length();

    // Create a matrix to store the distances
    vector<vector<int>> dp(m + 1, vector<int>(n + 1, 0));

    // Initialize the matrix
    for (int i = 0; i <= m; ++i) {
        for (int j = 0; j <= n; ++j) {
            if (i == 0) {
                dp[i][j] = j;
            }
            else if (j == 0) {
                dp[i][j] = i;
            }
            else {
                dp[i][j] = 0; // Placeholder for distance calculation
            }
        }
    }

    // Fill the matrix
    for (int i = 1; i <= m; ++i) {
        for (int j = 1; j <= n; ++j) {
            int insertion = dp[i][j - 1] + 1;
            int deletion = dp[i - 1][j] + 1;
            int substitution = dp[i - 1][j - 1] + (s1[i - 1] == s2[j - 1] ? 0 : 1);
            dp[i][j] = min(min(insertion, deletion), substitution);
        }
    }

    // The bottom-right cell contains the Levenshtein distance
    return dp[m][n];
}

void dfs(TrieNode* node, const string& currentWord, const string& targetWord, int index, int distance, int maxDistance, vector<pair<string, int>>& suggestions) {
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


void generateCorrections(const string& word, Trie* dictionary, vector<pair<string,int>>& suggestions, int maxDistance) {
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
        vector<pair<string,int>> suggestedCorrections;
        generateCorrections(inputWord, dictionary, suggestedCorrections, maxDistance);

        if (!suggestedCorrections.empty()) {
            cout << "Possible corrections:\n";
            for (auto correction : suggestedCorrections) {
                cout << correction.first << " " << correction.second << "\n";
            }
        }
        else {
            cout << "No suggestions found.\n";
        }
    }

    return 0;
}
