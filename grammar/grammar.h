#pragma once

#include <fstream>
#include <iostream>
#include <map>
#include <optional>
#include <regex>
#include <set>
#include <stdexcept>
#include <vector>

#include "debug.h"

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

  static MinalKind getKind(string heart) {
    return isupper(heart[0]) ? terminal : nonterminal;
  }
};

class Production {
public:
  const string head;
  const vector<Minal> body;
  const string code;

  Production& operator=(const Production&) = delete;

  void prettyPrint();
};

class Grammar {
public:
  // TODO: Should these be const to avoid bad access?
  // Maps keyed by token name, contents are matching info.
  map<string, string> aliases;
  map<string, string> keywords;
  map<string, string> tokens; // Regex tokens.

  map<string, vector<Production>> productions;
  string startNonterminal;

  string preparseCodeBlock;

  Grammar() {}

  static Grammar fromFile(string filename);

  bool validate();
  void generate(ofstream& f);
};
