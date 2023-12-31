#include <bits/stdc++.h>

#define RED "\033[1;31m" //code to RED color
#define BLUE "\033[1;34m" //code to BLUE color
#define GREEN "\033[1;32m" //code to GREEN color
#define RESET "\033[0m" //code to RESET the color
#define DICTIONARY_NAME "pt_BR.dic" //Name of the dictionary file to use

using namespace std;

//Struct Trie Node
struct TrieNode {
    map<char, TrieNode*> children;
    bool isEndOfWord;

    TrieNode() : isEndOfWord(false) {}

    ~TrieNode() {
        for (auto& child : children) {
            delete child.second;
        }
    }
};

struct Trie {
    TrieNode* root;

    Trie() : root(new TrieNode()) {}

    ~Trie() {
        delete root;
    }

    //Insert trie node
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

    //Search for a node
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

//Calculate the Longest Prefix Suffix used in the KMP
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

//Text = string to find patterns | pattern = pattern | matches = set that maps all pattern positions in string
void KMP(const string& text, const string& pattern, set<int>& matches) {
    int text_length = text.length();
    int pattern_length = pattern.length();
    vector<int> failure_function = compute_failure_function(pattern);

    int i = 0;
    int j = 0;

    while (i < text_length) {
        if (pattern[j] == text[i]) {
            j++;
            i++;
        }
        if (j == pattern_length) {
            matches.insert(i - j);
            j = failure_function[j - 1];
        }
        else if (i < text_length && pattern[j] != text[i]) {
            if (j != 0) {
                j = failure_function[j - 1];
            }
            else {
                i++;
            }
        }
    }
}

//Auxiliar function to find the end of a word
int end_word(const string& text) {
    for (int i = 0; i < text.length(); ++i) {
        if (ispunct(text[i]) || isspace(text[i])) {
            return i;
        }
    }

    return -1;
}

//Function to highlight words searching them in the text using KMP
string highlight_words(string& text, string& word, string color_code) {
    string colored_text = text;
    string reset_color = RESET;
    string color = color_code;
    set<int> occurrences;
    KMP(text, word, occurrences);
    int offset = 0; //used to format colored word
    for (auto x : occurrences) {
        if (!isalpha(text[x - 1])) { //Just to ensure that the word is in the beginning 
            colored_text.insert(x + offset, color);
            offset += color.length();
            colored_text.insert(x + end_word(text.substr(x)) + offset, reset_color);
            offset += reset_color.length();
        }
    }
    return colored_text;
}

//Function to highlight errors to search words that alrady have mapped
string highlight_wrong_words(const string& text, string color_code, set<int>& occurrences) {
    string colored_text = text;
    string reset_color = RESET;
    string color = color_code;
    int offset = 0; //used to calculate the distance to insert color
    for (auto x : occurrences) {
        if (!isalpha(text[x - 1])) { //Just to ensure that the word is in the beginning
            colored_text.insert(x + offset, color);
            offset += color.length();
            colored_text.insert(x + end_word(text.substr(x)) + offset, reset_color);
            offset += reset_color.length();
        }
    }
    return colored_text;
}

//Helper function to calculate distances between words, to see if the result is satisfier or not
int levenshtein_distance(const string& s1, const string& s2) {
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

//DFS used to navigate through the Trie to find close matching word for the misspelled one
//PARAMETERS
//node = Trie dictionary
//current_word = the word that is forming through the Trie
//target_word = word that is used to be compared
//index = used to compare char to char between the atual node of the Trie and the target_word
//distance = distance in the current iteraction
//max_distance = max distance allowed to navigate through the Trie
//suggestions = set that contain the distance between 2 words using levenshtein and the suggest wright word
void dfs(TrieNode* node, const string& current_word, const string& target_word, int index, int distance, int max_distance, set<pair<int, string>>& suggestions) {
    if (distance > max_distance) //Case base to stop DFS
        return;
    int edit_distance = levenshtein_distance(current_word, target_word);
    if (node->isEndOfWord && distance <= max_distance && edit_distance < 4) { //If the word is in the range of misspelled chars and the distance between the two words is closed put it in the set of suggestions
        suggestions.insert(make_pair(edit_distance, current_word));
    }

    if (target_word[index] == target_word[index + 1] && index == 0)
        index++;
    char currentChar = target_word[index];
    for (const auto& child : node->children) {
        char childChar = child.first;
        TrieNode* nextNode = child.second;

        int newDistance = distance + (currentChar == childChar ? 0 : 1);

        if (newDistance <= max_distance) { //verify if the distance is in the range allowed to continue searching
            dfs(nextNode, current_word + childChar, target_word, (index == target_word.length() - 1 ? 0 : index + 1), newDistance, max_distance, suggestions);
        }
    }
}

//function especified to call the DFS
//word = word to find possible corrections
//dicitonary = dictionary Trie
//suggestions = set that contain the distance between 2 words using levenshtein and the suggest wright word
//max_distance = max distance allowed to navigate through the Trie
void generate_corrections(const string& word, Trie* dictionary, set<pair<int, string>>& suggestions, int max_distance) {
    dfs(dictionary->root, "", word, 0, 0, max_distance, suggestions);
}

//Read the dictorary to a file, and put each word in a vector
vector<string> read_dictionary_from_file(const string& filename) {
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

//Cleans the punctation from the text
void remove_accent(string& word) {
    if (word[word.length() - 1] == ',' || word[word.length() - 1] == '.') {
        word.resize(word.length() - 1);
    }
}

//Splits the string into many word in a vector
//PARAMETERS
//input = text string to dismount
//words_in_text = array of words that will not have "," and "." to make the operations easier
//words_in_text_w_accent = array of words to recreate the original text
//delimiter = char to navigate through the string text
void split_string(const string& input, vector<string>& words_in_text, vector<string>& words_in_text_w_accent, char delimiter) {
    istringstream iss(input);
    string token;

    while (getline(iss, token, delimiter)) {
        words_in_text_w_accent.push_back(token);
        remove_accent(token);
        words_in_text.push_back(token);
    }

}

//function to read files and create a string based on the text and 2 vectors of string: 1 vector containing all words of the text without "." and "," | 1 vector containing all words with the pontuations to rebuild the string with colored words
//PARAMETERS
//filename = text file 
//words = array of words that will not have "," and "." to make the operations easier
//words_w_accent = array of words to recreate the original text
//full_text = string that will me created by the text file
void read_text_file(string filename, vector<string>& words, vector<string>& words_w_accent, string& full_text) {
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

//Find and return all indexes of words not listed in the dictionary
vector<int> show_erros(vector<string>& text, Trie*& dictionary) {
    vector<int> wrong_words;
    for (int i = 0; i < text.size(); i++) {
        transform(text[i].begin(), text[i].end(), text[i].begin(), [](char c) { //
            return std::tolower(c);                                             // used to put the word interated in the for in the lowercase for search in the trie
            });                                                                 //
        if (!dictionary->search(text[i]))
            wrong_words.push_back(i);
    }
    return wrong_words;
}

//Function that find all occurences of misspelleds words in the text using KMP anvoid
//PARAMETERS
//wrong_words_in_array = vector that map all positions of wrong words in the ARRAY of words that represents the text
//text = the actual text string
//all_words_of_text = array of all words in the text
//index = auxiliar variable to stop the recursion
//occurrences = set that map all misspelled words in the TEXT STRING
void find_all_errors(vector<int>& wrong_words_in_array, const string& text, vector<string>& all_words_of_text, int index, set<int>& occurrences) {
    if (index >= wrong_words_in_array.size())
        return;
    KMP(text, all_words_of_text[wrong_words_in_array[index]], occurrences);
    find_all_errors(wrong_words_in_array, text, all_words_of_text, index + 1, occurrences);
}

//Helper function to remove colors in a string
string remove_color_codes(const string& input) {
    regex color_regex("\x1B\\[[0-9;]*m");
    return regex_replace(input, color_regex, "");
}

//Function to add word in the dictionary file 
void add_to_dictionary(string word, string dictionary) {
    ofstream dic_file(dictionary, ios::app);
    if (!dic_file.is_open()) {
        std::cerr << "Error opening file for reading.\n";
        return;
    }
    dic_file << word << endl;
}

//Switch all matching words with  its counterpart in the misspelled string
//PARAMETERS
//suggestions = set with the suggested corrections
//text_arr = array containig all words of the text
//wrong_pos_arr = vector that map all misspelled words int the ARRAY of words
//corrected_text = variable where the text fully corrected will be saved colored
//white_corrected_text = variable where the text fully corrected will be saved but in white
//dictionary = dictionary trie
void auto_correct(vector<set<pair<int, string>>>& suggestions, vector<string>& text_arr, vector<int>& wrong_pos_arr, string& corrected_text, string& white_corrected_text, Trie* dictionary) {
    corrected_text = "";
    white_corrected_text = "";
    int ct = 0;
    vector<string> words_not_found;
    for (auto x : wrong_pos_arr) {
        if (!(suggestions[ct].empty())) {
            auto firstElement = *(suggestions[ct].begin());
            //verify if the word is in the end to just color the word, to not color the "," or "."
            text_arr[x] = isalpha(text_arr[x][text_arr[x].length() - 1]) ? (GREEN + firstElement.second + RESET) : text_arr[x].replace(0, text_arr[x].length() - 1, (GREEN + firstElement.second + RESET));
        }
        else {
            words_not_found.push_back(text_arr[x]);
            text_arr[x] = isalpha(text_arr[x][text_arr[x].length() - 1]) ? (RED + text_arr[x] + RESET) : text_arr[x].replace(0, text_arr[x].length() - 1, (RED + text_arr[x] + RESET));
        }
        ct++;
    }
    string cor = GREEN;
    for (auto x : text_arr) {
        string plain_text = remove_color_codes(x);
        //Extra verification to not input some " " in the begin of the text string
        white_corrected_text = white_corrected_text + (white_corrected_text != "" ? " " : "") + plain_text;
        corrected_text = corrected_text + (corrected_text != "" ? " " : "") + x;
    }
    cout << "\n" << corrected_text << endl;
    if (!words_not_found.empty()) {
        cout << "\nNo corrections found to: ";
        for (auto x : words_not_found) {
            cout << "\"" << x << "\"" << "\n\n";
            cout << "Do you wanna add this word to the dictionary?\n\n1-Yes.\n2-No.\n";
            int choice;
            cin >> choice;
            if (choice == 1) {
                dictionary->insert(x);
                string dic_name = DICTIONARY_NAME;
                add_to_dictionary(x, dic_name);
            }
        }
    }
}

//Receive a misspelled word and find all matching existing word with it
//PARAETERS
//suggestions = set with the suggested corrections
//wrong_words_text = set that map all misspelled words in the TEXT STRING
//text = the text to be navigated
//dictionary = dctionary Trie
//max_distance = max_distance to be used in the DFS
void show_suggetions(vector<set<pair<int, string>>>& suggestions, set<int>& wrong_words_text, string& text, Trie* dictionary, int max_distance) {
    string wrong_word;
    for (auto x : wrong_words_text) {
        wrong_word = text.substr(x, end_word(text.substr(x)));
        suggestions.resize(suggestions.size() + 1);
        generate_corrections(wrong_word, dictionary, suggestions[suggestions.size() - 1], max_distance);
    }
}


//Function that do manual corrections in the text
//PARAMETERS
//text_arr = array containg all words in the text string
//wrong_pos_arr = set that map all misspelled words in the ARRAY of words
//corrected_text = variable where the text fully corrected will be saved colored
//white_corrected_text = variable where the text fully corrected will be saved but in white
//dictionary = dictionary Trie
void manual_corrections(vector<string>& text_arr, vector<int>& wrong_pos_arr, string& corrected_text, string& white_corrected_text, Trie* dictionary) {
    corrected_text = "";
    white_corrected_text = "";
    string correction;
    for (auto x : wrong_pos_arr) {
        cout << "\nInput the correction to " << RED << (isalpha(text_arr[x][text_arr[x].length() - 1]) ? text_arr[x] : text_arr[x].substr(0, text_arr[x].size() - 1)) << RESET << ": ";
        cin >> correction;
        text_arr[x] = isalpha(text_arr[x][text_arr[x].length() - 1]) ? (GREEN + correction + RESET) : text_arr[x].replace(0, text_arr[x].length() - 1, (GREEN + correction + RESET));
        if (!dictionary->search(correction)) { //Here the input of the corrected word to is called when the word ins't in the dictionary
            cout << "Word " << BLUE << correction << RESET << " not found in the dictionary, do you want do add it?\n\n1 - Yes.\n2 - No.\n";
            int choice;
            cin >> choice;
            if (choice == 1) {
                add_to_dictionary(correction, DICTIONARY_NAME);
                dictionary->insert(correction);
            }
        }
    }
    for (auto x : text_arr) {
        string plain_text = remove_color_codes(x);
        white_corrected_text = white_corrected_text + (white_corrected_text != "" ? " " : "") + plain_text;
        corrected_text = corrected_text + (corrected_text != "" ? " " : "") + x;
    }
}

//FUNCTION TO FORMAT PRINT THE TABLE IN THE TERMINAL MENU
void print_header(const string& title, int& max_width) {
    max_width = max(max_width, static_cast<int>(title.length()) + 4);
    int width = max(static_cast<int>(title.length()) + 4, max_width);

    if (title == "") {
        cout << '+' << string(width, '-') << '+' << endl;
        return;
    }
    cout << '+' << string(width, '-') << '+' << endl;
    cout << '|' << setw(width) << left << title << setw(width) << '|' << endl;
    cout << '+' << string(width, '-') << '+' << endl;
}

//FUNCTION TO FORMAT PRINT THE TABLE IN THE TERMINAL MENU
void print_content(const string& description, int max_width) {
    int descWidth = max(static_cast<int>(description.length()) + 2, max_width);
    cout << '|' << left << setw(descWidth) << description << '|' << std::endl;
}

//FUNCTION TO FORMAT PRINT THE TABLE IN THE TERMINAL MENU
void menu() {
    int max_width = 0;

    vector<string> content = { "(0) Exit program!","(1) Find word's occurences in the text?","(2) Correct the text?" };

    for (const auto& content : content)
        max_width = std::max(max_width, static_cast<int>(content.length()) + 4);
    print_header("What do you want?", max_width);

    for (const auto& content : content)
        print_content(content, max_width);

    print_header("", max_width);
}

//FUNCTION TO FORMAT PRINT THE TABLE IN THE TERMINAL MENU
void search_menu() {
    int max_width = 0;

    vector<string> content = { "(1) Yes.", "(2) No." };

    for (const auto& content : content)
        max_width = std::max(max_width, static_cast<int>(content.length()) + 4);
    print_header("Wanna search another word?", max_width);

    for (auto x : content)
        print_content(x, max_width);

    print_header("", max_width);
}

//FUNCTION TO FORMAT PRINT THE TABLE IN THE TERMINAL MENU
void suggest_correction_menu() {
    int max_width = 0;

    vector<string> content = { "(1) Auto correct misspelled words?","(2) Correct words manually?","(0) Don't correct words." };

    for (const auto& content : content)
        max_width = std::max(max_width, static_cast<int>(content.length()) + 4);
    print_header("What do you wanna do?", max_width);

    for (const auto& content : content)
        print_content(content, max_width);

    print_header("", max_width);

}

//FUNCTION TO FORMAT PRINT THE TABLE IN THE TERMINAL MENU
void suggest_save_file() {

    int max_width = 0;
    vector<string> content = { "(1) Yes.", "(2) No." };

    for (const auto& content : content)
        max_width = std::max(max_width, static_cast<int>(content.length()) + 4);
    print_header("Do you want to save the corrections in a text file?", max_width);

    for (auto x : content)
        print_content(x, max_width);

    print_header("", max_width);
}


int main() {
    Trie* dictionary = new Trie();
    string dictionary_name = DICTIONARY_NAME;
    vector<string> dictionary_file = read_dictionary_from_file(dictionary_name);
    vector<string> words_in_text;
    vector<string> words_in_text_w_accent;
    set<int> wrong_words_text;
    vector<int> wrong_erros_pos_arr;
    vector<set<pair<int, string>>> suggested_corrections;
    string file_text;
    string wright_word, after_corrections = "";
    string teste = "flores";
    int max_distance = 3;

    cout << "Loading dictionary!" << endl;

    if (!dictionary_file.empty()) {
        cout << "Dictionary loaded successfully. Words read: " << dictionary_file.size() << endl;
        for (const string& word : dictionary_file) {
            dictionary->insert(word);
        }
    }
    else {
        cout << "Empty dictionary or error reading the file." << endl;
    }

    string inputFile;
    cout << "Enter a text file: ";
    cin >> inputFile;
    read_text_file(inputFile, words_in_text, words_in_text_w_accent, file_text);

    [[maybe_unused]] int result = system("clear");
    for (int choice;;) {
        menu();
        cin >> choice;
        switch (choice) {
        case 1:
            for (;;) {
                string word_to_find;
                cout << file_text << endl;
                cout << "\n\nWhat word you trying to find? " << endl;
                cin >> word_to_find;
                string colored_find_text = highlight_words(file_text, word_to_find, BLUE);
                [[maybe_unused]] int result = system("clear");
                cout << colored_find_text << endl;
                search_menu();
                cin >> choice;
                if (choice == 1) continue;
                else break;
            }
            break;
        case 2:
            for (;;) {
                wrong_erros_pos_arr = show_erros(words_in_text, dictionary);
                if (after_corrections != "") {
                    suggest_save_file();
                    cin >> choice;
                    if (choice == 1) {
                        if (!wrong_erros_pos_arr.empty()) {
                            cout << "\n\nYour text still have " << wrong_erros_pos_arr.size() << " error(s). Are you sure to continue?\n\n1-Yes.\n2-No.\n";
                            cin >> choice;
                        }
                        if (choice == 1 || wrong_erros_pos_arr.empty()) {
                            cout << "\n\nInput the file name: ";
                            string file_name;
                            cin >> file_name;
                            ofstream file(file_name);
                            streambuf* original_cout = cout.rdbuf();
                            cout.rdbuf(file.rdbuf());
                            cout << after_corrections;
                            cout.rdbuf(original_cout);
                        }
                    }
                }
                find_all_errors(wrong_erros_pos_arr, file_text, words_in_text, 0, wrong_words_text);
                string colored_wrong_text = highlight_wrong_words(file_text, RED, wrong_words_text);
                cout << endl << colored_wrong_text << endl;
                cout << "\nyour text have: " << wrong_erros_pos_arr.size() << " misspelled words\n";
                suggest_correction_menu();
                cin >> choice;
                if (choice == 0) {
                    after_corrections = "";
                    break;
                }
                if (choice == 1) {
                    show_suggetions(suggested_corrections, wrong_words_text, (after_corrections == "" ? file_text : after_corrections), dictionary, max_distance); //this function is called to find the wrong words in the text
                    auto_correct(suggested_corrections, words_in_text_w_accent, wrong_erros_pos_arr, wright_word, after_corrections, dictionary);
                    suggested_corrections.clear();
                    words_in_text_w_accent.clear();
                    wrong_erros_pos_arr.clear();
                    wrong_words_text.clear();
                    words_in_text.clear();
                    split_string(after_corrections, words_in_text, words_in_text_w_accent, ' '); //generate the text string already corrected
                }
                else if (choice == 2) {
                    show_suggetions(suggested_corrections, wrong_words_text, (after_corrections == "" ? file_text : after_corrections), dictionary, max_distance); //this function is called to find the wrong words in the text
                    manual_corrections(words_in_text_w_accent, wrong_erros_pos_arr, wright_word, after_corrections, dictionary);
                    suggested_corrections.clear();
                    words_in_text_w_accent.clear();
                    wrong_erros_pos_arr.clear();
                    wrong_words_text.clear();
                    words_in_text.clear();
                    split_string(after_corrections, words_in_text, words_in_text_w_accent, ' '); //generate the text string already corrected
                }
                file_text = after_corrections; //rewrite the text string with corrections
            }
            break;
        case 0:
            dictionary->~Trie(); //free memory
            return 0;
        }

    }
    dictionary->~Trie();
    return 0;
}
