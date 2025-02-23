#include "include/search.h"

#include <algorithm>
#include <fstream>
#include <iostream>
#include <map>
#include <set>
#include <sstream>
#include <string>
#include <vector>

using namespace std;

string cleanToken(const string& token) {
  // empty input
  if (token.empty()) {
    return "";
  }

  bool atLeastOne = false;
  for (char c : token) {
    if (isalpha(c)) {
      atLeastOne = true;
      break;
    }
  }
  // return nothing if it has no letters
  if (!atLeastOne) {
    return "";
  }

  // Find first and last non-punctuation characters
  size_t start = 0;
  size_t end = token.size() - 1;
  // start from beginning to remove non letter leading elements
  while (start < token.size() && ispunct(token[start])) {
    start++;
  }
  // start from end until the last letter in the token
  while (end > start && ispunct(token[end])) {
    end--;
  }

  // Extract the trimmed substring
  string clean = token.substr(start, end - start + 1);

  // Convert to lowercase
  for (char& c : clean) {
    c = tolower(c);
  }

  return clean;
}

set<string> gatherTokens(const string& text) {
  set<string> setTokens;
  istringstream stream(text);  // grab words out like cin
  string token;

  while (stream >> token) {
    string clean = cleanToken(token);
    if (!clean.empty()) {
      setTokens.insert(clean);
    }
  }

  return setTokens;
}

int buildIndex(const string& filename, map<string, set<string>>& index) {
  ifstream file(filename);
  if (!file.is_open()) {
    return 0;  // Return 0 if not found
  }

  string url;
  string text;
  int pairs = 0;

  while (getline(file, url) && getline(file, text)) {
    set<string> tokens = gatherTokens(text);

    for (string token : tokens) {
      index[token].insert(url);
    }

    pairs++;
  }

  file.close();
  return pairs;  // Return the number of url and text pairs
}

set<string> findQueryMatches(const map<string, set<string>>& index,
                             const string& sentence) {
  istringstream stream(sentence);
  string token;
  set<string> resultSet;
  bool isFirstTerm = true;  // first term doesn't follow the modifier rule

  while (stream >> token) {
    char modifier = ' ';
    if (token[0] == '+' || token[0] == '-') {
      modifier = token[0];
      token = token.substr(1);
    }

    string cleanedToken = cleanToken(token);

    set<string> matchingPages;
    if (index.count(cleanedToken)) {
      matchingPages = index.at(cleanedToken);
    }

    if (isFirstTerm) {
      resultSet = matchingPages;
    } else if (modifier == '+') {
      set<string> tempResult;
      set_intersection(resultSet.begin(), resultSet.end(),
                       matchingPages.begin(), matchingPages.end(),
                       inserter(tempResult, tempResult.begin()));
      resultSet = tempResult;
    } else if (modifier == '-') {
      set<string> tempResult;
      set_difference(resultSet.begin(), resultSet.end(), matchingPages.begin(),
                     matchingPages.end(),
                     inserter(tempResult, tempResult.begin()));
      resultSet = tempResult;
    } else {
      set<string> tempResult;
      set_union(resultSet.begin(), resultSet.end(), matchingPages.begin(),
                matchingPages.end(), inserter(tempResult, tempResult.begin()));
      resultSet = tempResult;
    }
    isFirstTerm = false;
  }

  return resultSet;
}

void searchEngine(const string& filename) {
  map<string, set<string>> index;

  int pairs = buildIndex(filename, index);

  if (pairs == 0) {
    cout << "Invalid filename." << endl;
  }

  cout << "Stand by while building index..." << endl;

  cout << "Indexed " << pairs << " pages containing " << index.size()
       << " unique terms" << endl;

  while (true) {
    cout << "Enter query sentence (press enter to quit): ";
    string query;
    getline(cin, query);

    if (query.empty()) {
      cout << "Thank you for searching!";
      break;
    }

    set<string> results = findQueryMatches(index, query);

    cout << "Found " << results.size() << " matching pages" << endl;

    for (const string& url : results) {
      cout << url << endl;
    }
  }
}
