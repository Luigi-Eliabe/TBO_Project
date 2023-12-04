#include <bits/stdc++.h>

#define RED "\033[1;31m"
#define BLUE "\033[1;34m"
#define GREEN "\033[1;32m"
#define RESET "\033[0m"

using namespace std;

vector<int> compute_failure_function(const string& pattern) {
    int m = pattern.length();
    vector<int> lps(m, 0);
    int j = 0;

    for (int i = 1; i < m; ) {
        if (pattern[i] == pattern[j]) {
            lps[i] = j + 1;
            ++j;
            ++i;
        }
        else {
            if (j != 0) {
                j = lps[j - 1];
            }
            else {
                lps[i] = 0;
                ++i;
            }
        }
    }

    return lps;
}

void KMP(const string& text, const string& pattern, set<int>& matches) {
    int textLength = text.length();
    int patternLength = pattern.length();
    vector<int> failure_function = compute_failure_function(pattern);

    int i = 0; // Index for text[]
    int j = 0; // Index for pattern[]

    while (i < textLength) {
        if (pattern[j] == text[i]) {
            j++;
            i++;
        }
        if (j == patternLength) {
            matches.insert(i - j);
            j = failure_function[j - 1];  // Update j to continue searching for the next occurrence
        }
        else if (i < textLength && pattern[j] != text[i]) {
            if (j != 0) {
                j = failure_function[j - 1];
            }
            else {
                i++;
            }
        }
    }
}

int partition(vector<pair<string, int>>& x, int start, int end) {
    int pivot = x[end].second;
    int i = start - 1;

    for (int j = start; j < end; j++) {
        if (x[j].second < pivot) {
            i += 1;
            swap(x[i], x[j]);
        }
    }
    if (pivot < x[i + 1].second) {
        swap(x[i + 1], x[end]);
    }

    return i + 1;
}

void quickSort(vector<pair<string, int>>& x, int left, int right) {
    if (left < right) {
        int p = partition(x, left, right);
        quickSort(x, left, p - 1);
        quickSort(x, p + 1, right);
    }
}



int end_word(const string& text){
    for (int i = 0; i < text.length(); ++i) {
        if (ispunct(text[i]) || isspace(text[i])) {
            return i;
        }
    }

    return -1;
}

//red = "\033[1;31m"
//blue = "\033[1;34m"
//green = "\033[1;32m"
string highlight_words(string& text, string& word, string color_code) {
    string colored_text = text;
    string reset_color = RESET;
    string color = color_code;
    set<int> occurrences;
    KMP(text, word, occurrences);
    int offset = 0;
    for (auto x : occurrences) {
        if(!isalpha(text[x-1])){
            colored_text.insert(x + offset, color);
            offset += color.length();
            colored_text.insert(x + end_word(text.substr(x)) + offset, reset_color);
            offset += reset_color.length();
        }
    }
    return colored_text;
}

//overload para caso já tenha as occorencias
//red = "\033[1;31m"
//blue = "\033[1;34m"
//green = "\033[1;32m"
string highlight_wrong_words(const string& text, string color_code, set<int>& occurrences) {
    string colored_text = text;
    string reset_color = RESET;
    string color = color_code;
    int offset = 0;
    for (auto x : occurrences) {
        if(!isalpha(text[x-1])){
            colored_text.insert(x + offset, color);
            offset += color.length();
            colored_text.insert(x + end_word(text.substr(x)) + offset, reset_color);
            offset += reset_color.length();
        }
    }
    return colored_text;
}

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

void dfs(TrieNode* node, const string& currentWord, const string& targetWord, int index, int distance, int maxDistance, set<pair<int, string>>& suggestions) {
    // if (index == targetWord.length()) {
    if (distance > maxDistance)
        return;
    int editDistance = levenshteinDistance(currentWord, targetWord);
    if (node->isEndOfWord && distance <= maxDistance && editDistance < 4) {
        suggestions.insert(make_pair(editDistance, currentWord));
    }
    // }

    if(targetWord[index] == targetWord[index+1] && index == 0)
        index++;
    char currentChar = targetWord[index];
    for (const auto& child : node->children) {
        char childChar = child.first;
        TrieNode* nextNode = child.second;

        int newDistance = distance + (currentChar == childChar ? 0 : 1);

        if (newDistance <= maxDistance) {
            dfs(nextNode, currentWord + childChar, targetWord, (index == targetWord.length() - 1 ? 0 : index + 1), newDistance, maxDistance, suggestions);
        }
    }
}


void generateCorrections(const string& word, Trie* dictionary, set<pair<int, string>>& suggestions, int maxDistance) {
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

void remove_accent(string& word) {
    if (word[word.length() - 1] == ',' || word[word.length() - 1] == '.') {
        word.resize(word.length() - 1);
    }
}

void readTextFile(string filename, vector<string>& words, vector<string> &words_w_accent, string& full_text) {
    ifstream file(filename);
    full_text = "";
    if (!file.is_open()) {
        cerr << "Error opening file: " << filename << endl;
        return; // Return empty vector if file couldn't be opened
    }
    string actual_word;
    while (getline(file, actual_word, ' ')) {
        full_text = full_text + (full_text != "" ? " " : "") + actual_word;
        words_w_accent.push_back(actual_word);
        remove_accent(actual_word);
        words.push_back(actual_word);
    }
    file.close();
}


//Função que mostra palavras erradas no texto
vector<int> showErros(vector<string>& text, Trie*& dictionary) {
    vector<int> wrong_words;
    for (int i = 0; i < text.size(); i++) {
        transform(text[i].begin(), text[i].end(), text[i].begin(), [](char c) {
            return std::tolower(c);
            });
        if (!dictionary->search(text[i]))
            wrong_words.push_back(i);
    }
    return wrong_words;
}

//returns all position of wrong words in the text
void find_all_errors(vector<int> &wrong_words_in_array, string &text, vector<string> &all_words_of_text, int index, set<int> &occurrences){
    if(index >= wrong_words_in_array.size())
        return;
    KMP(text,all_words_of_text[wrong_words_in_array[index]], occurrences);
    find_all_errors(wrong_words_in_array,text,all_words_of_text,index + 1, occurrences);
}


void auto_correct(vector<set<pair<int, string>>>& suggestions, vector<string> &text_arr, vector<int> &wrong_pos_arr, string &corrected_text){
    int ct=0;
    vector<string> words_not_found;
    for(auto x: wrong_pos_arr){
        if(!(suggestions[ct].empty())){
            auto firstElement = *(suggestions[ct].begin());
            text_arr[x] = isalpha(text_arr[x][text_arr[x].length() - 1]) ? (GREEN + firstElement.second + RESET) : text_arr[x].replace(0,text_arr[x].length()-1,(GREEN + firstElement.second + RESET));
            // text.insert(x,firstElement.second);
        }else{
            words_not_found.push_back(text_arr[x]);
            text_arr[x] = isalpha(text_arr[x][text_arr[x].length() - 1]) ? (RED + text_arr[x] + RESET) : text_arr[x].replace(0,text_arr[x].length()-1,(RED + text_arr[x] + RESET));
        }
        ct++;
    }
    for(auto x : text_arr){
        corrected_text =  corrected_text + (corrected_text != "" ? " " : "") + x;
    }
    cout << endl << corrected_text << endl;
    if(!words_not_found.empty()){
        cout << "\nNo corrections found to: ";
        for(auto x : words_not_found){
            cout << "\"" << x << "\"" << " ";
        }
        cout << endl;
    }
}


void show_suggetions(vector<set<pair<int, string>>>& suggestions, set<int> &wrong_words_text, string &text, Trie *dictionary, int maxDistance){
    string wrong_word;
    for(auto x: wrong_words_text){
        wrong_word = text.substr(x,end_word(text.substr(x)));
        suggestions.resize(suggestions.size() + 1);
        generateCorrections(wrong_word, dictionary, suggestions[suggestions.size()-1], maxDistance);
        cout << "Correções para " << wrong_word << endl;
        int ct = 0;
        for(auto y: suggestions[suggestions.size()-1]){
            if(y.first <= 3 && ct < 3)
                cout << y.second << " " << y.first << endl;
            ct++;
        }
    }
}

// string random_corrections(string &text,vector<string> &wrong_words, set<pair<int, string>>& suggestions){

void menu(){
    cout << "+------------------------------------------+" << endl;
    cout << "| What do you want?                        |" << endl;
    cout << "|------------------------------------------|" << endl;
    cout << "| (0) Exit program!                        |" << endl;
    cout << "|------------------------------------------|" << endl;
    cout << "| (1) Find word's occurences in the text?  |" << endl;
    cout << "|------------------------------------------|" << endl;
    cout << "| (2) Correct the text?                    |" << endl;
    cout << "+------------------------------------------+" << endl;
}

void searchMenu(){
    cout << "+------------------------------------------+" << endl;
    cout << "| Wanna search another word?               |" << endl;
    cout << "|------------------------------------------|" << endl;
    cout << "| (1) Yes.                                 |" << endl;
    cout << "|------------------------------------------|" << endl;
    cout << "| (2) No.                                  |" << endl;
    cout << "+------------------------------------------+" << endl;
}

void addDictionaryMenu(){
    cout << "+------------------------------------------+" << endl;
    cout << "| Add word to dictionary?                  |" << endl;
    cout << "|------------------------------------------|" << endl;
    cout << "| (1) Yes.                                 |" << endl;
    cout << "|------------------------------------------|" << endl;
    cout << "| (2) No.                                  |" << endl;
    cout << "+------------------------------------------+" << endl;
}

void suggestCorrectionsMenu(){
    cout << "+------------------------------------------+" << endl;
    cout << "| What do you wanna do?                    |" << endl;
    cout << "|------------------------------------------|" << endl;
    cout << "| (1) Auto correct misspelled words?       |" << endl;
    cout << "|------------------------------------------|" << endl;
    cout << "| (2) Correct words manually?              |" << endl;
    cout << "|------------------------------------------|" << endl;
    cout << "| (0) Don't correct words.                 |" << endl;
    cout << "+------------------------------------------+" << endl;
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

    transform(inputWord.begin(), inputWord.end(), inputWord.begin(), [](char c) {
        return std::tolower(c);
        });

    int maxDistance = 2; // Maximum edit distance for suggestions

    if (dictionary->search(inputWord)) {
        cout << "The word is valid.\n";
    }
    else {
        vector<pair<string, int>> suggestedCorrections;
        generateCorrections(inputWord, dictionary, suggestedCorrections, maxDistance);

        quickSort(suggestedCorrections, 0, suggestedCorrections.size() - 1);

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
