#include "grammar.h"

// ===== class Production ===== //
void Production::prettyPrint() {
  assert(bodies.size() != 0);
  if (bodies.size() == 1) {
    cout << head << " ->";
    for (auto minal : bodies[0]) cout << " \"" << minal.heart << '"';
    cout << endl;
  } else {
    cout << head << " ->" << endl;
    for (auto body : bodies) {
      cout << "\t| ";
      for (auto minal : body)
        cout << " \"" << minal.heart << '"';
      cout << endl;
    }
    cout << "\t;" << endl;
  }
}


// ===== class Grammar ===== //
void parseTerminal(Grammar& grammar, string& line, size_t lineNum);
void parseProduction(Grammar& grammar, string& line, size_t& lineNum, ifstream& f);
bool isMinalNameChar(char c);
void expectString(string s,     int lineNum, string& line, int& i);
void expectChar  (char c,       int lineNum, string& line, int& i);
void expectOneof (string chars, int lineNum, string& line, int& i);
void skipSomeSpaces(int lineNum, string& line, int& i);
void skipAnySpaces(int lineNum, string& line, int& i);


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
    lineNum++; // This is fine: lineNum should start at 1.
    if (line.size() == 0) continue;
    if (line.compare(0, 2, "//") == 0) continue;
    if (line[0] == '%') { // Terminal declaration.
      parseTerminal(grammar, line, lineNum);
    } else if (line.find("->") != string::npos) { // Production.
      parseProduction(grammar, line, lineNum, f);
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
      if (grammar.productions.count(s) == 0) {
        throw lineErr(lineNum, "nonterminal not recognized", line);
      }
      grammar.startNonterminal = s;
    } else {
      throw lineErr(lineNum, "bad line layout", line);
    }
  }

  return grammar;
}

bool isMinalNameChar(char c) {
  return c == '_' || isalpha(c);
}

void expectString(string s, int lineNum, string &line, int &i) {
  if (line.compare(i, s.length(), s) != 0) {
    throw lineErr(lineNum, "expected \"" + s + "\" at index " + to_string(i),
                  line);
  }
  i += s.length();
}

void expectChar(char c, int lineNum, string &line, int &i) {
  if (i >= line.size() || line[i] != c) {
    auto msg = "expected '" + to_string(c) + "' at index " + to_string(i);
    throw lineErr(lineNum, msg, line);
  }
  i++;
}

void expectOneof(string s, int lineNum, string &line, int &i) {
  if (i >= line.size() || s.find(line[i]) == string::npos) {
    string msg = "expected one of [";
    for (int j=0; j<s.size(); j++) {
      if (j>0) msg += ", ";
      msg += "'" + to_string(s[j]) + "'";
    }
    msg += "] at index " + to_string(i);
    throw lineErr(lineNum, msg, line);
  }
  i++;
}

void skipSomeSpaces(int lineNum, string &line, int &i) {
  expectChar(' ', lineNum, line, i);
  skipAnySpaces(lineNum, line, i);
}

void skipAnySpaces(int lineNum, string &line, int &i) {
  for (; i<line.size() && isspace(line[i]); i++);
}

// TODO: Switch to codegen for kw and regex tokens?
void parseTerminal(Grammar &grammar, string &line, size_t lineNum) {
  auto i = 1;
  for (; i<line.size() && isMinalNameChar(line[i]); i++);
  if (i==1) {
    throw lineErr(lineNum, "expected terminal name", line);
  }
  auto name = line.substr(1, i-1);

  skipSomeSpaces(lineNum, line, i);

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

optional<string> parseProdMinal(string& line, size_t lineNum, int& i) {
  skipAnySpaces(lineNum, line, i);
  if (i>=line.size() || !isMinalNameChar(line[i])) {
    return optional<string>();
  }
  int minalStart = i;
  for (; i<line.size() && isMinalNameChar(line[i]); i++);
  return optional(line.substr(minalStart, i-minalStart));
}

// TODO: Consider allowing multi-line blocks.
// TODO: Use this in parseProduction / parseProdBody. Must consider...
string parseCodeBlock(string &line, size_t &lineNum, int &i) {
  expectChar('{', lineNum, line, i);
  string s = "{";
  int curlyDepth = 1;
  bool inString = false;
  for (; i < line.size() && curlyDepth > 0; i++) {
    char c = line[i];
    if (!inString) {
      if (c == '{')
        curlyDepth++;
      else if (c == '}')
        curlyDepth--;
    }
    if (c == '"' && line[i-1] != '\\')
      inString = !inString;
    s.push_back(c);
  }
  if (i < line.size()) {
    throw lineErr(
        lineNum,
        "expected line to end after code block at index " + to_string(i), line);
  }
  return s;
}

vector<Minal> parseProdBody(string& line, size_t lineNum, int&i) {
  vector<Minal> body;
  while (i < line.size()) {
    skipSomeSpaces(lineNum, line, i);
    auto minalOpt = parseProdMinal(line, lineNum, i);
    if (minalOpt.has_value()) {
      auto heart = minalOpt.value();
      body.emplace_back(Minal::getKind(heart), heart);
    } else if (i < line.size()) {
      throw lineErr(lineNum, "expected minal at index " + to_string(i), line);
    }
  }
  return body;
}

void parseProduction(Grammar& grammar, string& line, size_t& lineNum, ifstream& f) {
  int i = 0;
  for (; i < line.size() && isalpha(line[i]); i++);
  auto head = line.substr(0, i);

  if (grammar.productions.count(head) != 0) {
    throw lineErr(lineNum, "redefined nonterminal: \"" + head + "\"", line);
  }
  expectString(" ->", lineNum, line, i);

  if (i < line.size()) { // SINGLE LINE PRODUCTION
    DEBUG(cout << "single line production" << endl);
    vector<Minal> body = parseProdBody(line, lineNum, i);
    string code = "";
    grammar.productions.emplace(head, Production{head, {body}, code});
    return;
  }

  // MULTI-LINE PRODUCTION
  vector<vector<Minal>> bodies;
  string code;
  vector<Minal> body;
  while (true) {
    DEBUG(cout << "multi line production" << endl);
    lineNum++;
    if (!getline(f, line)) {
      throw lineErr(lineNum, "encountered EOF inside production", line);
    }
    i=0;
    skipAnySpaces(lineNum, line, i);

    if (line[i] == ';') { // END OF PRODUCTION
      // TODO: Check if there's a code block.
      break;
    } else if (line[i] == '|') { // NEW BODY
      i++;
      bodies.push_back(parseProdBody(line, lineNum, i));
    } else {
      throw lineErr(lineNum, "expected '|' or ';' at index " + to_string(i), line);
    }
  }
  grammar.productions.emplace(head, Production{head, bodies, code});
}

bool Grammar::validate() {
  DEBUG({
    cout << "LEX:" << endl;
    cout << "\taliases: " << aliases.size() << endl;
    cout << "\tkeywords: " << keywords.size() << endl;
    cout << "\ttokens: " << tokens.size() << endl;
    cout << "GRAMMAR:" << endl;
    cout << "\tproductions: " << productions.size() << endl;
    cout << "\tstartNonterminal: " << startNonterminal << endl;
    cout << endl;
    });
  for (auto [head, production] : productions) {
    DEBUG(production.prettyPrint());
    for (auto body : production.bodies) {
      for (auto minal : body) {
        if (minal.kind == MinalKind::terminal) {
          DEBUG(cout << "terminal:    " << minal.heart << endl);
          if (keywords.count(minal.heart) == 0 &&
              tokens.count(minal.heart) == 0)
            return false;
        } else {
          DEBUG(cout << "nonterminal: " << minal.heart << endl);
          if (productions.count(minal.heart) == 0)
            return false;
        }
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
    lineNum++;
    int i = 0;
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

// TODO: Implement.
bool Grammar::parse(const vector<LexedToken>& tokens) {
  return true;
}
