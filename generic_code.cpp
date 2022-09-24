#include <optional>
#include <regex>
#include <string>
#include <vector>
#include <iostream>

using namespace std;

static const bool _DEBUG = false;
#define DEBUG(x) if (_DEBUG) { x; }
static int _DEBUG_DEPTH = 0;
#define DEBUGP(x)                                                       \
  if (_DEBUG) {                                                         \
    for (int i = 0; i < _DEBUG_DEPTH; i++) cout << "  ";                \
    x;                                                                  \
  }

#define MATCH(tok_name)                                            \
  if (TOK_##tok_name == tokens[localTokIndex].type) {              \
    DEBUGP(cout << "matched token: " << #tok_name << endl);        \
    localTokIndex++;                                               \
  } else {                                                         \
    DEBUGP(cout << "no match: " << #tok_name << endl);             \
    return 1;                                                      \
  }

#define PARSE(nt_name)                                                  \
  if (PARSE_##nt_name(tokens, localTokIndex) == 0) {                    \
    DEBUGP(cout << "parsed nt: " << #nt_name << endl);                  \
  } else {                                                              \
    DEBUGP(cout << "no match: " << #nt_name << endl);                   \
    return 1;                                                           \
  }

bool matchstr(string &s, string &line, int i);

typedef int TokenType;
class Token {
 public:
  const TokenType type;
  const string heart;
};

typedef optional<Token> (*KwMatchFunc)(int lineNum, string &line, int &i);
typedef int (*ParseFunc)(vector<Token>& tokens, int& tokIndex);


/* INSERT GENERATED CODE HERE */


// ===== GENERIC CODE ===== //
#include <fstream>
#include <iostream>
#include <stdexcept>

runtime_error lineErr(int lineNum, string msg, int i, string line) {
  auto errMsg = to_string(lineNum) + "at index " + to_string(i) + ": "
                + msg + ": \"" + line + "\"";
  return runtime_error(msg);
}

bool matchstr(string &s, string &line, int i) {
  // TODO: If you need to match multiline strings, this will need work.
  // NOTE: This does not account for escape characters (null, tab),
  //       but they shouldn't show up in a keyword.
  return line.compare(i, s.size(), s)==0;
}

void skipAnySpaces(string& line, int& i) {
  for (; i < line.size() && isspace(line[i]); i++);
}

vector<Token> lex(ifstream& f) {
  vector<Token> tokens;
  string line;
  int lineNum = 0;
  while (getline(f, line)) {
    lineNum++;
    int i = 0;
    while (i < line.size()) {
      skipAnySpaces(line, i);
      if (i == line.size()) break;

      for (auto kw_match_func : kw_match_funcs) {
        optional<Token> optToken = kw_match_func(lineNum, line, i);
        if (!optToken.has_value()) continue;
        // DEBUG(cout << "found kw token: " << optToken.value().heart << endl);
        tokens.push_back(optToken.value());
        goto foundMatch;
      }

      for (auto rx_match_func : rx_match_funcs) {
        optional<Token> optToken = rx_match_func(lineNum, line, i);
        if (!optToken.has_value()) continue;
        // DEBUG(cout << "found rg token: " << optToken.value().heart << endl);
        tokens.push_back(optToken.value());
        goto foundMatch;
      }

      throw lineErr(lineNum, "could not find token match", i, line);
   foundMatch:;
    }
  }

  return tokens;
}

void parse(vector<Token>& tokens) {
  int tokIndex = 0;
  if (PARSE_root(tokens, tokIndex) == 0) {
    cout << "succeeded at parsing!";
  } else {
    cout << "failed parsing :-(";
  }
}

int main(int argc, char** argv) {
  if (argc != 2) {
    printf("usage: %s SOURCE_FILE.c", argv[0]);
  }

  ifstream f((string(argv[1])));
  cout << "-- LEXING --" << endl;
  vector<Token> tokens = lex(f);
  for (auto tok : tokens) cout << " " << tok.heart;
  cout << endl;

  cout << endl;
  cout << "-- PARSING --" << endl;
  parse(tokens);
  return 0;
}
