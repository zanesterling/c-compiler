#include "grammar.h"
#include <stdexcept>

// ===== class Production ===== //
void Production::prettyPrint() {
  for (auto minal : body) cout << " \"" << minal.heart << '"';
  if (code.length() > 0) cout << " " << code;
  cout << endl;
}


// ===== class Grammar ===== //
void parseTerminal(Grammar& grammar, string& line, size_t lineNum);
void parseProduction(Grammar& grammar, string& line, size_t& lineNum, ifstream& f);
bool isMinalNameChar(char c);
void expectString(string s,     int lineNum, string& line, int& i);
void expectChar  (char c,       int lineNum, string& line, int& i);
void expectOneof (string chars, int lineNum, string& line, int& i);
void skipSomeSpaces(int lineNum, string& line, int& i);
void skipAnySpaces(string& line, int& i);


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
    auto msg = "expected '"s + c + "' at index " + to_string(i);
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
  skipAnySpaces(line, i);
}

void skipAnySpaces(string &line, int &i) {
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

// TODO: Consider allowing multi-line code blocks.
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

pair<vector<Minal>, string> parseProdBody(string& line, size_t lineNum, int& i) {
  vector<Minal> body;
  string code;
  while (i < line.size()) {
    skipSomeSpaces(lineNum, line, i);
    if (i >= line.size()) break;
    if (line[i] == '{') {
      code = parseCodeBlock(line, lineNum, i);
      skipAnySpaces(line, i);
      if (i<line.size()) {
        throw lineErr(lineNum, "expected EOL after code block", line);
      }
      break;
    }
    if (!isMinalNameChar(line[i])) {
      throw lineErr(lineNum, "expected minal at index " + to_string(i), line);
    }

    // Parse a minal.
    int minalStart = i;
    for (; i < line.size() && isMinalNameChar(line[i]); i++);
    auto heart = line.substr(minalStart, i - minalStart);
    body.emplace_back(Minal::getKind(heart), heart);
  }
  return pair(body, code);
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
    auto [body, code] = parseProdBody(line, lineNum, i);
    skipAnySpaces(line, i);
    grammar.productions[head].push_back(Production{head, body, code});
    return;
  }

  // MULTI-LINE PRODUCTION
  vector<Production> productions;
  while (true) {
    DEBUG(cout << "multi line production" << endl);
    lineNum++;
    if (!getline(f, line)) {
      throw lineErr(lineNum, "encountered EOF inside production", line);
    }
    i=0;
    skipAnySpaces(line, i);

    if (line[i] == ';') { // END OF PRODUCTION
      i++;
      if (i<line.size()) {
        throw lineErr(lineNum, "expected EOL at index " + to_string(i), line);
      }
      break;
    } else if (line[i] == '|') { // NEW BODY
      i++;
      auto [body, code] = parseProdBody(line, lineNum, i);
      productions.push_back(Production{head, body, code});
    } else {
      throw lineErr(lineNum, "expected '|' or ';' at index " + to_string(i), line);
    }
  }
  grammar.productions.emplace(head, productions);
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
  for (auto [_, ps] : productions) {
    for (auto production : ps) {
      DEBUG(production.prettyPrint());
      for (auto minal : production.body) {
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

void Grammar::generate(ofstream& f) {
  ifstream genericCodeFile("generic_code.cpp");
  if (!genericCodeFile.is_open()) {
    throw runtime_error("could not open generic_code.cpp");
  }

  string line;
  const string stub = "/* INSERT GENERATED CODE HERE */";
  while (getline(genericCodeFile, line) && line.compare(stub)!=0) {
    f << line << endl;
  }

  f << "// ===== LEXING  CODE ===== //" << endl;
  int tokn = 0;
  for (auto [name, _] : keywords) {
    f << "int TOK_" << name << " = " << tokn++ << ";" << endl;
  }
  for (auto [name, _] : tokens) {
    f << "int TOK_" << name << " = " << tokn++ << ";" << endl;
  }
  f << endl;

  for (auto [name, kw] : keywords) {
    f << endl;
    f << "// %" << name << " \"" << kw << "\"" << endl;
    f << "optional<Token> LEX_" << name
      << " (int lineNum, string& line, int& i) {" << endl;
    f << "  string kw = \"" << kw << "\";" << endl;
    f << "  if (matchstr(kw, line, i)) {" << endl;
    f << "    i += kw.length();" << endl;
    f << "    return optional(Token{TOK_" << name << ", kw});" << endl;
    f << "  }" << endl;
    f << "  return optional<Token>();" << endl;
    f << "}" << endl;
  }
  f << endl;
  f << "vector<KwMatchFunc> kw_match_funcs {" << endl;
  for (auto [name, _] : keywords) {
    f << "  LEX_" << name << "," << endl;
  }
  f << "};" << endl;
  f << endl;

  for (auto [name, rgText] : tokens) {
    f << endl;
    f << "// %" << name << " " << rgText << endl;
    f << "optional<Token> LEX_" << name
      << " (int lineNum, string& line, int& i) {" << endl;
    // TODO: Need to escape this regex text.
    f << "  regex reg(\"" + rgText + "\");" << endl;
    f << "  cmatch m;" << endl;
    f << "  if (!regex_search(line.c_str()+i, m, reg, regex_constants::match_continuous)) {" << endl;
    f << "    return optional<Token>();" << endl;
    f << "  }" << endl;
    f << "  i += m.length();" << endl;
    f << "  return optional(Token{TOK_" << name << ", m[0]});" << endl;
    f << "}" << endl;
  }
  f << "vector<KwMatchFunc> rx_match_funcs {" << endl;
  for (auto [name, _] : tokens) {
    f << "  LEX_" << name << "," << endl;
  }
  f << "};" << endl;
  f << endl << endl;

  f << "// ===== PARSING CODE ===== //" << endl;

  for (auto [head, _] : productions) {
    f << "int PARSE_" << head << "(vector<Token>& tokens, int& tokIndex);" << endl;
  }
  f << endl;

  for (auto [head, ps] : productions) {
    for (int i=0; i<ps.size(); i++) {
      f << endl;
      auto p = ps[i];
      f << "// " << p.head << " ->";
      for (auto minal : p.body) f << " " << minal.heart;
      f << endl;

      f << "int PARSE_" << p.head << "_" << i
        << " (vector<Token>& tokens, int& tokIndex) {" << endl;
      f << "  int localTokIndex = tokIndex;" << endl;
      for (auto minal : p.body) {
        if (minal.kind == terminal) {
          f << "  MATCH(" << minal.heart << ");" << endl;
        } else if (minal.kind == nonterminal) {
          f << "  PARSE(" << minal.heart << ");" << endl;
        }
      }
      if (p.code.length() > 0) {
        f << "  " << p.code << endl;
      }
      f << "  tokIndex = localTokIndex;" << endl;
      f << "  return 0;" << endl;
      f << "}" << endl;
    }

    f << endl;
    f << "int PARSE_" << head << " (vector<Token>& tokens, int& tokIndex) {" << endl;
    f << "  DEBUGP(cout << \"parsing " << head << "\" << endl);" << endl;
    f << "  _DEBUG_DEPTH++;" << endl;
    f << "  int res;" << endl;
    for (int i = 0; i < ps.size(); i++) {
      f << "  if (PARSE_" << head << "_" << i << "(tokens, tokIndex) == 0) {" << endl;
      f << "    _DEBUG_DEPTH--;" << endl;
      f << "    return 0;" << endl;
      f << "  }" << endl;
    }
    f << "  _DEBUG_DEPTH--;" << endl;
    f << "  return 1;" << endl;
    f << "}" << endl;
  }

  f << endl;
  f << "ParseFunc PARSE_root = "
    << "PARSE_" << this->startNonterminal << ";" << endl;

  for (string s; getline(genericCodeFile, s);) {
    f << s << endl;
  }
}
