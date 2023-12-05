#include <bits/stdc++.h>

#define RED "\033[1;31m"
#define BLUE "\033[1;34m"
#define GREEN "\033[1;32m"
#define RESET "\033[0m"
#define DICTIONARY_NAME "pt_BR.dic"

using namespace std;

struct TrieNode {
    map<char, TrieNode*> children;
    bool isEndOfWord;

    TrieNode() : isEndOfWord(false) {}
};

struct Trie {
    TrieNode* root;

    Trie() : root(new TrieNode()) {}

    void insert(TrieNode* node, const string& word) {
        TrieNode* current = node;

        for (char c : word) {
            if (current->children.find(c) == current->children.end()) {
                current->children[c] = new TrieNode();
            }
            current = current->children[c];
        }

        current->isEndOfWord = true;
    }

    void insert(const string& word) {
        insert(root, word);
    }

    bool search(TrieNode* node, const string& word) {
        TrieNode* current = node;

        for (char c : word) {
            if (current->children.find(c) == current->children.end()) {
                return false;
            }
            current = current->children[c];
        }

        return current != nullptr && current->isEndOfWord;
    }

    bool search(const string& word) {
        return search(root, word);
    }
};

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

    int i = 0;
    int j = 0;

    while (i < textLength) {
        if (pattern[j] == text[i]) {
            j++;
            i++;
        }
        if (j == patternLength) {
            matches.insert(i - j);
            j = failure_function[j - 1];
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

int end_word(const string& text){
    for (int i = 0; i < text.length(); ++i) {
        if (ispunct(text[i]) || isspace(text[i])) {
            return i;
        }
    }

    return -1;
}

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

int levenshteinDistance(const string& s1, const string& s2) {
    int m = s1.length();
    int n = s2.length();


    vector<vector<int>> dp(m + 1, vector<int>(n + 1, 0));


    for (int i = 0; i <= m; ++i) {
        for (int j = 0; j <= n; ++j) {
            if (i == 0) {
                dp[i][j] = j;
            }
            else if (j == 0) {
                dp[i][j] = i;
            }
            else {
                dp[i][j] = 0;
            }
        }
    }


    for (int i = 1; i <= m; ++i) {
        for (int j = 1; j <= n; ++j) {
            int insertion = dp[i][j - 1] + 1;
            int deletion = dp[i - 1][j] + 1;
            int substitution = dp[i - 1][j - 1] + (s1[i - 1] == s2[j - 1] ? 0 : 1);
            dp[i][j] = min(min(insertion, deletion), substitution);
        }
    }


    return dp[m][n];
}

void dfs(TrieNode* node, const string& currentWord, const string& targetWord, int index, int distance, int maxDistance, set<pair<int, string>>& suggestions) {
    if (distance > maxDistance)
        return;
    int editDistance = levenshteinDistance(currentWord, targetWord);
    if (node->isEndOfWord && distance <= maxDistance && editDistance < 4) {
        suggestions.insert(make_pair(editDistance, currentWord));
    }

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
        return words; 
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

void split_string(const string& input, vector<string> &words_in_text, vector<string> &words_in_text_w_accent , char delimiter) {
    istringstream iss(input);
    string token;

    while (getline(iss, token, delimiter)) {
        words_in_text_w_accent.push_back(token);
        remove_accent(token);
        words_in_text.push_back(token);
    }

}

void readTextFile(string filename, vector<string>& words, vector<string> &words_w_accent, string& full_text) {
    ifstream file(filename);
    full_text = "";
    if (!file.is_open()) {
        cerr << "Error opening file: " << filename << endl;
        return;
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

void find_all_errors(vector<int> &wrong_words_in_array, const string &text, vector<string> &all_words_of_text, int index, set<int> &occurrences){
    if(index >= wrong_words_in_array.size())
        return;
    KMP(text,all_words_of_text[wrong_words_in_array[index]], occurrences);
    find_all_errors(wrong_words_in_array,text,all_words_of_text,index + 1, occurrences);
}

string removeColorCodes(const string& input) {
    regex color_regex("\x1B\\[[0-9;]*m");
    return regex_replace(input, color_regex, "");
}

void add_to_dictionary(string word, string dictionary){
    ofstream dic_file(dictionary, ios::app);
    if (!dic_file.is_open()) {
        std::cerr << "Error opening file for reading.\n";
        return ;
    }
    dic_file << word << endl;
}

void auto_correct(vector<set<pair<int, string>>>& suggestions, vector<string> &text_arr, vector<int> &wrong_pos_arr, string &corrected_text, string &white_corrected_text, Trie* dictionary){
    corrected_text = "";
    white_corrected_text = "";
    int ct=0;
    vector<string> words_not_found;
    for(auto x: wrong_pos_arr){
        if(!(suggestions[ct].empty())){
            auto firstElement = *(suggestions[ct].begin());
            text_arr[x] = isalpha(text_arr[x][text_arr[x].length() - 1]) ? (GREEN + firstElement.second + RESET) : text_arr[x].replace(0,text_arr[x].length()-1,(GREEN + firstElement.second + RESET));
        }else{
            words_not_found.push_back(text_arr[x]);
            text_arr[x] = isalpha(text_arr[x][text_arr[x].length() - 1]) ? (RED + text_arr[x] + RESET) : text_arr[x].replace(0,text_arr[x].length()-1,(RED + text_arr[x] + RESET));
        }
        ct++;
    }
    string cor = GREEN;
    for(auto x : text_arr){
        string plain_text = removeColorCodes(x);
        white_corrected_text =  white_corrected_text + (white_corrected_text != "" ? " " : "") + plain_text;
        corrected_text =  corrected_text + (corrected_text != "" ? " " : "") + x;
    }
    cout << "\n" << corrected_text << endl;
    if(!words_not_found.empty()){
        cout << "\nNo corrections found to: ";
        for(auto x : words_not_found){
            cout << "\"" << x << "\"" << "\n\n";
            cout << "Do you wanna add this word to the dictionary?\n\n1-Yes.\n2-No.\n";
            int choice;
            cin >> choice;
            if(choice==1){
                dictionary->insert(x);
                string dic_name = DICTIONARY_NAME;
                add_to_dictionary(x,dic_name);
            }
        }
    }
}


void show_suggetions(vector<set<pair<int, string>>>& suggestions, set<int> &wrong_words_text, string &text, Trie *dictionary, int maxDistance){
    string wrong_word;
    for(auto x: wrong_words_text){
        wrong_word = text.substr(x,end_word(text.substr(x)));
        suggestions.resize(suggestions.size() + 1);
        generateCorrections(wrong_word, dictionary, suggestions[suggestions.size()-1], maxDistance);
    }
}

void manual_corrections(vector<string> &text_arr, vector<int> &wrong_pos_arr, string &corrected_text, string &white_corrected_text, Trie* dictionary){
    corrected_text = "";
    white_corrected_text = "";
    string correction;
    for(auto x : wrong_pos_arr){
        cout << "\nInput the correction to " << RED << (isalpha(text_arr[x][text_arr[x].length() - 1]) ? text_arr[x] : text_arr[x].substr(0,text_arr[x].size()-1)  ) << RESET <<  ": ";
        cin >> correction;
        text_arr[x] = isalpha(text_arr[x][text_arr[x].length() - 1]) ? (GREEN + correction + RESET) : text_arr[x].replace(0,text_arr[x].length()-1,(GREEN + correction + RESET));
        if(!dictionary->search(correction)){
            cout << "Word " << BLUE << correction << RESET << " not found in the dictionary, do you want do add it?\n\n1 - Yes.\n2 - No.\n";
            int choice;
            cin >> choice;
            if(choice == 1){
                add_to_dictionary(correction,DICTIONARY_NAME);
                dictionary->insert(correction);
            }
        }
    }
    for(auto x : text_arr){
        string plain_text = removeColorCodes(x);
        white_corrected_text =  white_corrected_text + (white_corrected_text != "" ? " " : "") + plain_text;
        corrected_text =  corrected_text + (corrected_text != "" ? " " : "") + x;
    }
}


void printHeader(const string& title, int& maxWidth) {
    maxWidth = max(maxWidth, static_cast<int>(title.length()) + 4);
    int width = max(static_cast<int>(title.length()) + 4, maxWidth);

    if (title == "") {
        cout << '+' << string(width, '-') << '+' << endl;
        return;
    }
    cout << '+' << string(width, '-') << '+' << endl;
    cout << '|' << setw(width) << left << title << setw(width) << '|' << endl;
    cout << '+' << string(width, '-') << '+' << endl;
}

void printContent(const string& description, int maxWidth) {
    int descWidth = max(static_cast<int>(description.length()) + 2, maxWidth);
    cout << '|' << left << setw(descWidth) << description << '|' << std::endl;
}

void menu(){
    int maxWidth = 0;

    vector<string> content = { "(0) Exit program!","(1) Find word's occurences in the text?","(2) Correct the text?"};

    for (const auto& content : content)
        maxWidth = std::max(maxWidth, static_cast<int>(content.length()) + 4);
    printHeader("What do you want?", maxWidth);

    for (const auto& content : content)
        printContent(content, maxWidth);

    printHeader("", maxWidth);
}

void searchMenu(){
    int maxWidth = 0;

    vector<string> content = {"(1) Yes.", "(2) No."};

    for (const auto& content : content) 
        maxWidth = std::max(maxWidth, static_cast<int>(content.length()) + 4);
    printHeader("Wanna search another word?", maxWidth);

    for(auto x : content)
        printContent(x,maxWidth);

    printHeader("", maxWidth);
}

void addDictionaryMenu(){

    int maxWidth = 0;

    vector<string> content = {"(1) Yes.", "(2) No."};

    for (const auto& content : content) 
        maxWidth = std::max(maxWidth, static_cast<int>(content.length()) + 4);
    printHeader("Add word to dictionary?", maxWidth);

    for(auto x : content)
        printContent(x,maxWidth);

    printHeader("", maxWidth);

}

void suggestCorrectionsMenu(){
    int maxWidth = 0;

    vector<string> content = { "(1) Auto correct misspelled words?","(2) Correct words manually?","(0) Don't correct words." };

    for (const auto& content : content)
        maxWidth = std::max(maxWidth, static_cast<int>(content.length()) + 4);
    printHeader("What do you wanna do?", maxWidth);

    for (const auto& content : content)
        printContent(content, maxWidth);

    printHeader("", maxWidth);

}
void suggestSaveFile(){

    int maxWidth = 0;
    vector<string> content = {"(1) Yes.", "(2) No."};

    for (const auto& content : content) 
        maxWidth = std::max(maxWidth, static_cast<int>(content.length()) + 4);
    printHeader("Do you want to save the corrections in a text file?", maxWidth);

    for(auto x : content)
        printContent(x,maxWidth);

    printHeader("", maxWidth);
}


int main() {
    Trie* dictionary = new Trie();
    string dictionaryFile = DICTIONARY_NAME;
    vector<string> mockDictionary = readDictionaryFromFile(dictionaryFile);
    vector<string> words_in_text;
    vector<string> words_in_text_w_accent;
    set<int> wrong_words_text;
    vector<int> wrong_erros_pos_arr;
    vector<set<pair<int, string>>> suggestedCorrections;
    string file_text;
    string palavra_certona, after_corrections = "";
    string teste = "flores";
    int maxDistance = 3;

    if (!mockDictionary.empty()) {
        cout << "Dictionary loaded successfully. Words read: " << mockDictionary.size() << endl;
    }
    else {
        cout << "Empty dictionary or error reading the file." << endl;
    }
    for (const string& word : mockDictionary) {
        dictionary->insert(word);
    }

    string inputFile;
    cout << "Enter a text file: ";
    cin >> inputFile;
    readTextFile(inputFile,words_in_text,words_in_text_w_accent,file_text);

    system("clear");
    for(int choice;;){
        menu();
        cin >> choice;
        switch(choice){
            case 1:
                for(;;){
                    string word_to_find;
                    cout << file_text << endl;
                    cout << "\n\nWhat word you trying to find? " << endl;
                    cin >> word_to_find;
                    string colored_find_text = highlight_words(file_text,word_to_find,BLUE);
                    system("clear");
                    cout << colored_find_text << endl;
                    searchMenu();
                    cin >> choice;
                    if(choice == 1) continue;
                    else break;
                }
            break;
            case 2:
                for(;;){
                    wrong_erros_pos_arr =  showErros(words_in_text,dictionary);
                    if(after_corrections != ""){
                        suggestSaveFile();
                        cin >> choice;
                        if(choice == 1){
                            if(!wrong_erros_pos_arr.empty()){
                                cout << "\n\nYour text still have " << wrong_erros_pos_arr.size() << " error(s). Are you sure to continue?\n\n1-Yes.\n2-No.\n";
                                cin >> choice;
                            }
                            if(choice == 1 || wrong_erros_pos_arr.empty()){
                                cout << "\n\nInput the file name: ";
                                string file_name;
                                cin >> file_name;
                                ofstream file(file_name);
                                streambuf *original_cout = cout.rdbuf();
                                cout.rdbuf(file.rdbuf());
                                cout << after_corrections;
                                cout.rdbuf(original_cout);
                            }
                        }
                    }
                    find_all_errors(wrong_erros_pos_arr, file_text, words_in_text,0,wrong_words_text);
                    string colored_wrong_text = highlight_wrong_words(file_text,RED,wrong_words_text);
                    cout << endl << colored_wrong_text << endl;
                    cout << "\nyour text have: " << wrong_erros_pos_arr.size() << " misspelled words\n";
                    suggestCorrectionsMenu();
                    cin >> choice;
                    if(choice == 0){
                        after_corrections = "";
                        break;
                    } 
                    if (choice == 1){
                        show_suggetions(suggestedCorrections,wrong_words_text, (after_corrections == "" ? file_text : after_corrections) ,dictionary,maxDistance);
                        auto_correct(suggestedCorrections,words_in_text_w_accent,wrong_erros_pos_arr,palavra_certona, after_corrections, dictionary);
                        suggestedCorrections.clear();
                        words_in_text_w_accent.clear();
                        wrong_erros_pos_arr.clear();
                        wrong_words_text.clear();
                        words_in_text.clear();
                        split_string(after_corrections,words_in_text,words_in_text_w_accent,' ');
                    }else if(choice == 2){
                        show_suggetions(suggestedCorrections,wrong_words_text, (after_corrections == "" ? file_text : after_corrections) ,dictionary,maxDistance);
                        manual_corrections(words_in_text_w_accent,wrong_erros_pos_arr,palavra_certona, after_corrections, dictionary);
                        suggestedCorrections.clear();
                        words_in_text_w_accent.clear();
                        wrong_erros_pos_arr.clear();
                        wrong_words_text.clear();
                        words_in_text.clear();
                        split_string(after_corrections,words_in_text,words_in_text_w_accent,' ');
                    }
                    file_text = after_corrections;
                }
            break;
            case 0: return 0;
        }

    }
    
    return 0;
}
