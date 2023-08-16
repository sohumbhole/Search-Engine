// Project 2
// @author Sohum Bhole, sbhol2

#pragma once

#include <iostream>
#include <set>
#include <map>
#include <cctype>
#include <cstring>
#include <string>
#include <sstream>
#include <fstream>

using namespace std;

// cleanToken helper function. Converts any inputted string to lowercase.
string toLower(string input) {
    string output = "";
    for (int i = 0; i < input.length(); i++) {
        output += tolower(input.at(i));
    }

    return output;
}

bool checkHasLetter(string input) {
    for (char c: input) {
        if (isalpha(c)) {
            return true;
        }
    }
    return false;
}

// Input: string to be trimmed
// Output: same input string without leading or trailing whitespace. Empty string otherwise
// Checks each character from outside in till both indexes are on the first instance of a non-
// punctuation character from the left and right and substrings that.
string cleanToken(string s) {
    int leadingIndex = 0;
    int trailingIndex = s.length() -1;
    string output = "";
    bool stop = false;
    while (!stop && (ispunct(s.at(leadingIndex)) || ispunct(s.at(trailingIndex)))) {
        stop = leadingIndex == trailingIndex;
        if (ispunct(s.at(leadingIndex))) {
            leadingIndex++;
        }
        else if(ispunct(s.at(trailingIndex))) {
            trailingIndex--;
        }
        else {
            stop = true;
        }
    }

    output = toLower(s.substr(leadingIndex, (trailingIndex - leadingIndex + 1)));
    bool hasLetter = checkHasLetter(output);
    if (output.length() > 0 && hasLetter) {
        if (output.length() == 1) {
            return isalpha(output.at(0)) ? output : "";
        }
        else {
            return output;
        }
    }
    return "";

}

set<string> gatherTokens(string line) {
    set<string> tokens;
    string word;
    stringstream ss(line);
    while (ss >> word) {
        word = cleanToken(word);
        if (word.length() > 0) {
            tokens.insert(word);
        }
    }

    return tokens;
}


int buildIndex(string filename, map<string, set<string>>& index) {
    int lineNum = 0;
    string line, value;
    ifstream inFS;
    inFS.open(filename);
    if (!inFS.is_open()) {
        return 0;
    }

    while(getline(inFS,line)){
        value = line;
        getline(inFS,line);
        set<string> tokens = gatherTokens(line);
        for (string s : tokens){
            if (index.count(s)){
                index.at(s).insert(value);
            } else{
                set<string> URL;
                URL.insert(value);
                index.emplace(s,URL);
                URL.clear();
            }
        }
        tokens.clear();
        lineNum++;
    }

    inFS.close();
    return lineNum;
}

set<string> findMatches (set<string> &urlMatches, set<string> words){
    set<string> newSet;
    for (string token : words){
        if(urlMatches.count(token)){
            newSet.insert(token);
        }
    }
    urlMatches = newSet;

    return urlMatches;
}

set<string> addUrls (set<string> &urlMatches, set<string> words){
    urlMatches.insert(words.begin(),words.end());

    return urlMatches;
}

set<string> differenceUrls (set<string> &URLmatches, set<string> words){
    for (string s : words){
        URLmatches.erase(s);
    }
    return URLmatches;
}


// CREATIVE COMPONENT
// Adds urls only of search keyword starts with inputted letter
set<string> findSearchMatchesStartingLetter(map<string, set<string>>& index, string sentence, char letter) {
    set<string> urlMatches;
    set<string> words;
    string word;
    sentence = toLower(sentence);
    stringstream ss(sentence);
    ss >> word;
    urlMatches = index.at(cleanToken(word));
    while (ss >> word){
        words = index.at(cleanToken(word));
        if (word.at(0) == '+'){
            if (tolower(word.at(1)) == letter) {
                findMatches(urlMatches, words);
            }
        }
        else if (word.at(0) == '-'){
            if (tolower(word.at(1)) == letter) {
                differenceUrls(urlMatches, words);
            }
        }
        else{
            if ((tolower(word.at(0)) == letter)) {
                addUrls(urlMatches, words);
            }
        }
        words.clear();
    }

    return urlMatches;
}


set<string> findSearchMatches(map<string, set<string>>& index, string sentence) {
    set<string> urlMatches;
    set<string> words;
    string word;
    sentence = toLower(sentence);
    stringstream ss(sentence);
    ss >> word;
    urlMatches = index.at(cleanToken(word));
    while (ss >> word){
        words = index.at(cleanToken(word));
        if (word.at(0) == '+'){
            findMatches(urlMatches, words);
        }
        else if (word.at(0) == '-'){
            differenceUrls(urlMatches, words);
        }
        else{
            addUrls(urlMatches, words);
        }
        words.clear();
    }

    return urlMatches;
}

// TODO: Add a function header comment here to explain the
// behavior of the function and how you implemented this behavior
void searchEngine(string filename) {
    map<string, set<string>> index;
    set<string> searchMatches;
    string enter;
    int indexSize = buildIndex(filename, index);

    cout << "Stand by while building index..." << endl;
    cout << "Indexed " << indexSize << " pages containing " << index.size()
         << " unique terms" << endl;
    cout << endl << "Enter query sentence (press enter to quit): ";

    getline(cin, enter);

    while (enter.length() != 0) {
        searchMatches = findSearchMatches(index, enter); // returns a set, you need to return all of them
        cout << "Found " << searchMatches.size() << " matches pages" << endl;
        for (string link: searchMatches) {
            cout << link << endl;
        }

        cout << endl << "Enter query sentence (press enter to quit): ";
        getline(cin, enter);
        if (enter.length() == 0) {
            break;
        }
    }
    cout << "Thank you for searching!";
}
