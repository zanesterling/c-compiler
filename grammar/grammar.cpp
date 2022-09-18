#include "grammar.h"

void parseTerminal(Grammar& grammar, string& line, size_t lineNum);
void parseProduction(Grammar& grammar, string& line, size_t lineNum);


runtime_error lineErr(int lineNum, string msg, string line) {
  return runtime_error(to_string(lineNum)+": " + msg+": \"" + line + "\"");
}

Grammar Grammar::parse(string filename) {
  ifstream f(filename);
  if (!f.is_open()) {
    throw runtime_error("error opening file: " + filename);
  }

  Grammar grammar;
  string line;
  size_t lineNum = 0;
  while (getline(f, line)) {
    if (line.size() == 0) continue;
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

void parseTerminal(Grammar& grammar, string& line, size_t lineNum) {
  // TODO: Switch to codegen later for kw and regex tokens?
  if (isalpha(line[1]) && isupper(line[1])) {
    // Alias definition.
    size_t alias_end = 1;
    for (; alias_end < line.size() && !isspace(line[alias_end]); alias_end++);
    string alias = line.substr(1, alias_end-1);

    size_t regex_start = alias_end;
    for (; regex_start < line.size() && isspace(line[regex_start]); regex_start++);
    string regex = line.substr(regex_start);

    grammar.aliases[alias] = regex;
  } else if (line[1] == '"') {
    // Keyword definition.
    auto kw_end = line.find('"', 2);
    if (kw_end == string::npos) {
      throw lineErr(lineNum, "unclosed quote for keyword", line);
    }
    auto kw_def = line.substr(2, kw_end-2);

    auto i=kw_end+2;
    for (; i<line.size() && isalpha(line[i]); i++);
    if (i < line.size()) {
      throw lineErr(lineNum, "expected line to end at index " + to_string(i), line);
    }
    auto name = line.substr(kw_end+2, i-(kw_end+1));
    grammar.keywords[name] = kw_def;
  } else {
    // Regex token definition.
    size_t regex_end = 1;
    for (; regex_end < line.size() && !isspace(line[regex_end]); regex_end++);
    auto regex = line.substr(1, regex_end-1);

    auto i = regex_end+1;
    for (; i < line.size() && isalpha(line[i]); i++);
    if (i < line.size()) {
      throw lineErr(lineNum, "expected line to end at index " + to_string(i),
                    line);
    }
    auto name = line.substr(regex_end+1, i-(regex_end+1));
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
    if (line[i] != ' ' || !isalpha(line[i+1])) {
      throw lineErr(lineNum, "expected minal at index " + to_string(i+1), line);
    }
    int minalStart = i+1;
    i++;
    for (; i < line.size() && isalpha(line[i]); i++);
    auto kind = isupper(line[minalStart])
                ? MinalKind::terminal : MinalKind::nonterminal;
    body.push_back(Minal(kind, line.substr(minalStart, i-minalStart)));
  }

  auto head = line.substr(0, headEnd);
  grammar.productions.push_back(Production{head, body});
  grammar.nonterminals.insert(head);
}

bool Grammar::validate() {
  cout << "LEX:" << endl;
  cout << "\taliases: " << aliases.size() << endl;
  cout << "\tkeywords: " << keywords.size() << endl;
  cout << "\ttokens: " << tokens.size() << endl;
  cout << "GRAMMAR:" << endl;
  cout << "\tnonterminals: " << nonterminals.size() << endl;
  cout << "\tproductions: " << productions.size() << endl;
  cout << "\tstartNonterminal: " << startNonterminal << endl;
  cout << endl;
  for (auto production : productions) {
    cout << production.head << " ->";
    for (auto minal : production.body) cout << " \"" << minal.heart << '"';
    cout << endl;
    for (auto minal : production.body) {
      if (minal.kind == MinalKind::terminal) {
        cout << "terminal:    " << minal.heart << endl;
        if (keywords.count(minal.heart) == 0 &&
            tokens.count(minal.heart) == 0) return false;
      } else {
        cout << "nonterminal: " << minal.heart << endl;
        if (nonterminals.count(minal.heart) == 0) return false;
      }
    }
  }
  return true;
}
