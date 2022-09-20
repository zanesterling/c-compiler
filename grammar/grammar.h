#pragma once

#include <fstream>
#include <iostream>
#include <map>
#include <regex>
#include <set>
#include <stdexcept>
#include <vector>

using namespace std;

class LexedToken {
 public:
  const string name;
  const string contents;
};

enum MinalKind {
  terminal,
  nonterminal
};
class Minal {
public:
  const MinalKind kind;
  const std::string heart;

  Minal(MinalKind kind, std::string heart) : kind(kind), heart(heart) {}
};

class Production {
public:
  std::string head;
  vector<Minal> body;
};

class Grammar {
public:
  // TODO: Should these be const to avoid bad access?
  // Maps keyed by token name, contents are matching info.
  map<string, string> aliases;
  map<string, string> keywords;
  map<string, string> tokens; // Regex tokens.

  set<string> nonterminals;
  vector<Production> productions;
  string startNonterminal;

  Grammar() {}

  static Grammar fromFile(string filename);

  bool validate(bool debug);
  vector<LexedToken> lex(string filename);
  bool parse(const vector<LexedToken>& tokens);
};
