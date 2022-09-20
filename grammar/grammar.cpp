#include "grammar.h"

void parseTerminal(Grammar& grammar, string& line, size_t lineNum);
void parseProduction(Grammar& grammar, string& line, size_t lineNum);
bool isTerminalNameChar(char c);


runtime_error lineErr(int lineNum, string msg, string line) {
  return runtime_error(to_string(lineNum)+": " + msg+": \"" + line + "\"");
}

Grammar Grammar::fromFile(string filename) {
  ifstream f(filename);
  if (!f.is_open()) {
    throw runtime_error("error opening file: " + filename);
  }

  Grammar grammar;
  string line;
  size_t lineNum = 0;
  while (getline(f, line)) {
    if (line.size() == 0) continue;
    if (line.compare(0, 2, "//") == 0) continue;
    if (line[0] == '%') { // Terminal declaration.
      parseTerminal(grammar, line, lineNum);
    } else if (line.find("->") != string::npos) { // Production.
      parseProduction(grammar, line, lineNum);
    } else if (line[0] == '!') { // Set root nonterminal.
      if (line.size() == 1) {
        throw lineErr(lineNum, "empty start nonterminal", line);
      }
      if (grammar.startNonterminal != "") {
        throw lineErr(lineNum, "start nonterminal already set", line);
      }
      for (int i=1; i<line.size(); i++) {
        if (!isalpha(line[i]) || !islower(line[i])) {
          throw lineErr(lineNum, "invalid start nonterminal", line);
        }
      }
      auto s = line.substr(1);
      if (grammar.nonterminals.count(s) == 0) {
        throw lineErr(lineNum, "nonterminal not recognized", line);
      }
      grammar.startNonterminal = s;
    } else {
      throw lineErr(lineNum, "bad line layout", line);
    }
    lineNum++;
  }

  return grammar;
}

bool isTerminalNameChar(char c) {
  return c == '_' || isalpha(c);
}

void parseTerminal(Grammar& grammar, string& line, size_t lineNum) {
  // TODO: Switch to codegen later for kw and regex tokens?
  auto i = 1;
  for (; i<line.size() && isTerminalNameChar(line[i]); i++);
  if (i==1) {
    throw lineErr(lineNum, "expected terminal name", line);
  }
  auto name = line.substr(1, i-1);

  if (i >= line.size() || !isspace(line[i])) {
    throw lineErr(lineNum, "expected terminal at index " + to_string(i), line);
  }
  for (; i<line.size() && isspace(line[i]); i++);

  if (name[0] == '_') { // ALIAS
    grammar.aliases[name] = line.substr(i + 1);
  } else if (line[i] == '"') { // KEYWORD
    auto kwStart = i+1;
    auto kwEnd = line.find('"', kwStart);
    if (kwEnd == string::npos) {
      throw lineErr(lineNum, "unclosed quote for keyword", line);
    }
    grammar.keywords[name] = line.substr(kwStart, kwEnd-kwStart);
  } else { // REGEX
    // TODO: substitute aliases
    auto regex = line.substr(i);
    grammar.tokens[name] = regex;
  }
}

void parseProduction(Grammar& grammar, string& line, size_t lineNum) {
  int i = 0;
  for (; i < line.size() && isalpha(line[i]); i++);
  auto headEnd = i;

  if (line.compare(i, 3, " ->") != 0) {
    throw lineErr(lineNum, "expected \" ->\" at index " + to_string(i), line);
  }
  i += 3;

  vector<Minal> body;
  while (i+1 < line.size()) {
    if (line[i]!=' ' || !isalpha(line[i+1])) {
      throw lineErr(lineNum, "expected minal at index " + to_string(i+1), line);
    }
    int minalStart = i+1;
    i++;
    for (; i<line.size() && (line[i]=='_' || isalpha(line[i])); i++);
    auto kind = isupper(line[minalStart])
                ? MinalKind::terminal : MinalKind::nonterminal;
    body.push_back(Minal(kind, line.substr(minalStart, i-minalStart)));
  }

  auto head = line.substr(0, headEnd);
  grammar.productions.push_back(Production{head, body});
  grammar.nonterminals.insert(head);
}

bool Grammar::validate(bool debug) {
  if (debug) {
    cout << "LEX:" << endl;
    cout << "\taliases: " << aliases.size() << endl;
    cout << "\tkeywords: " << keywords.size() << endl;
    cout << "\ttokens: " << tokens.size() << endl;
    cout << "GRAMMAR:" << endl;
    cout << "\tnonterminals: " << nonterminals.size() << endl;
    cout << "\tproductions: " << productions.size() << endl;
    cout << "\tstartNonterminal: " << startNonterminal << endl;
    cout << endl;
  }
  for (auto production : productions) {
    if (debug) {
      cout << production.head << " ->";
      for (auto minal : production.body)
        cout << " \"" << minal.heart << '"';
      cout << endl;
    }
    for (auto minal : production.body) {
      if (minal.kind == MinalKind::terminal) {
        if (debug) cout << "terminal:    " << minal.heart << endl;
        if (keywords.count(minal.heart) == 0 &&
            tokens.count(minal.heart) == 0) return false;
      } else {
        if (debug) cout << "nonterminal: " << minal.heart << endl;
        if (nonterminals.count(minal.heart) == 0) return false;
      }
    }
  }
  return true;
}

vector<LexedToken> Grammar::lex(string filename) {
  ifstream f(filename);
  if (!f.is_open()) {
    throw runtime_error("error opening file: " + filename);
  }

  vector<LexedToken> result;
  string line;
  int lineNum = 0;
  while (getline(f, line)) {
    int i = 0;
    lineNum++;
    while (i<line.size()) {
      for (; i<line.size() && isspace(line[i]); i++);
      if (i==line.size()) break;

      for (auto [name, kw] : keywords) {
        if (i+kw.length() > line.size()) continue;
        if (line.compare(i, kw.length(), kw) != 0) continue;
        result.push_back(LexedToken{name, kw});
        i += kw.length();
        goto foundToken;
      }

      for (auto [name, rgText] : tokens) {
        regex reg(rgText);
        cmatch m;
        if (!regex_search(line.c_str()+i, m, reg, regex_constants::match_continuous)) {
          continue;
        }
        result.push_back(LexedToken{name, m[0]});
        i += m.length();
        goto foundToken;
      }

      throw lineErr(lineNum, "no match at index " + to_string(i), line);
   foundToken:;
    }
  }
  return result;
}

bool Grammar::parse(const vector<LexedToken>& tokens) {
  return true;
}
