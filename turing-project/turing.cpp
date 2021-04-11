#include <iostream>
#include <assert.h>
#include <getopt.h>
#include <cstring>
#include <fstream>
#include <string>
#include <set>
#include <regex>

#define Log(format, ...) \
  if (verbose) \
    printf( format , ## __VA_ARGS__)

#define Assert(cond, ...) \
  if (!(cond)) { \
    fprintf(stderr, "syntax error\n"); \
    if (verbose) { \
      fprintf(stderr, __VA_ARGS__); \
      fprintf(stderr, "\n"); } \
    exit(EXIT_FAILURE); \
  }

using namespace std;

static char *tmfile;
static char *input_string;
static bool verbose = false;
regex cm_re(","); // comma
regex ws_re("\\s"); // whitespace

class TF {
public:
  string old_state, old_symbols, new_symbols, directions, new_state;
  TF(string ost, string osym, string nsym, string dir, string nst): old_state(ost), old_symbols(osym), new_symbols(nsym), directions(dir), new_state(nst){}
};

string q0;      // start state
string B;       // blank symbol
uint32_t N;     // number of tapes
set<string> Q, S, G, F;
vector<TF> Delta; // Transition function
string state;
string syms;

class Tape {
public:
  string s;
  int head;
  int begin;
  Tape(string str): s(str), head(0), begin(0){}
  Tape(): s(B), head(0), begin(0){}

  void move(char sym, char dir) {
    s[head] = sym;
    switch (dir) {
      case 'l':
        if (head == 0) {
          s.insert(0, B);
          begin--;
        } else head--;
        break;
      case 'r':
        head++;
        if (head == s.length()) s += B;
        break;
      case '*':
        break;
    }
  }
};

void SetStates(string str) {
  if (str.length())
  Assert(str.length() >= 6, "%s", str.c_str());
  smatch m;
  string text;
  if (regex_search(str, m, regex("\\{(.+?)\\}"))) text = m[1];
  switch (str[1]) {
    case 'N':
      Assert(regex_match(str, regex("#N = \\d+")), "%s", str.c_str());
      N = stoi(str.substr(5)); break;
    case 'B':
      Assert(regex_match(str, regex("#B = _")), "Blank symbol should be '_'");
      B = str.substr(5); break;
    case 'q':
      Assert(regex_match(str, regex("#q0 = \\w+")), "%s", str.c_str());
      q0 = str.substr(6);
      state = q0;
      break;
    case 'Q':
      Assert(regex_match(str, regex("#Q = \\{\\w+(,\\w+)*\\}")), "%s", str.c_str());
      Q = set<string>(sregex_token_iterator(text.begin(), text.end(), cm_re, -1), sregex_token_iterator());
      break;
    case 'S':
      Assert(!regex_search(text, regex("[\\x20\\x3b\\x7b\\x7d\\x2a\\x5f]")), "%s", text.c_str());
      Assert(regex_match(str, regex("#S = \\{[\\x20-\\x7e](,[\\x20-\\x7e])*\\}")), "%s", str.c_str());
      S = set<string>(sregex_token_iterator(text.begin(), text.end(), cm_re, -1), sregex_token_iterator());
      break;
    case 'G':
      Assert(!regex_search(text, regex("[\\x20\\x3b\\x7b\\x7d\\x2a]")), "%s", str.c_str());
      Assert(regex_match(str, regex("#G = \\{[\\x20-\\x7e](,[\\x20-\\x7e])*\\}")), "%s", str.c_str());
      G = set<string>(sregex_token_iterator(text.begin(), text.end(), cm_re, -1), sregex_token_iterator());
      break;
    case 'F':
      Assert(regex_match(str, regex("#F = \\{\\w+(,\\w+)*\\}")), "%s", str.c_str());
      F = set<string>(sregex_token_iterator(text.begin(), text.end(), cm_re, -1), sregex_token_iterator());
      break;
    default:
      Assert(0, "%s", str.c_str()); // Unknown case
  }
}

void SetTransitionFunction(string str) {
  vector<string> v(sregex_token_iterator(str.begin(), str.end(), ws_re, -1), sregex_token_iterator());
  Assert(v.size() == 5, "%s", str.c_str());
  Assert(Q.count(v[0]) != 0 && Q.count(v[4]) != 0, "%s", str.c_str());
  Delta.push_back(TF(v[0], v[1], v[2], v[3], v[4]));
}

static struct option long_options[] = {
  {"verbose", no_argument, NULL, 'v'},
  {"help", no_argument, NULL, 'h'},
  {NULL, 0, NULL, 0}
};

static void parse_args(int argc, char *argv[]) {
  int o = 0;
  bool parse_failure = false;
  while ((o = getopt_long(argc, argv, "vh", long_options, NULL)) != -1) {
    switch (o) {
      case 'v':
        verbose = true;
        break;
      case 'h':
        printf("usage: turing [-v|--verbose] [-h|--help] <tm> <input>\n");
        exit(0);
        break;
      default:
        parse_failure = true;
        break;
    }
  }
  if (optind + 1 >= argc) parse_failure = true;
  if (parse_failure) {
    fprintf(stderr, "usage: turing [-v|--verbose] [-h|--help] <tm> <input>\n");
    exit(EXIT_FAILURE);
  }
  tmfile = argv[optind];
  input_string = argv[optind+1];
}

static void parser() {
  ifstream inFile;
  inFile.open(tmfile);
  string str;
  while (!inFile.eof()) {
    getline(inFile, str);
    if (str.empty() || str[0] == ';') continue;
    size_t found = str.find(';'); // delete comments
    if (found!=string::npos) { str = str.substr(0, found); }
    str.erase(str.find_last_not_of(" ") + 1); // trim string

    if (str[0] == '#') 
      SetStates(str);
    else 
      SetTransitionFunction(str);
  }
  inFile.close();
}

void simulate(string input) {
  for (int i = 0; i < input.length(); i++) {
    if (S.count(input.substr(i, 1)) == 0) {
      if (!verbose) fprintf(stderr, "illegal input\n");
      else {
        fprintf(stderr, "Input: %s\n", input_string);
        fprintf(stderr, "==================== ERR ====================\n");
        fprintf(stderr, "error: '%c' was not declared in the set of input symbols\n", input[i]);
        fprintf(stderr, "Input: %s\n", input_string);
        fprintf(stderr, "%*s\n", 8+i, "^");
        fprintf(stderr, "==================== END ====================\n");
      }
      exit(EXIT_FAILURE);
    }
  }
  Log("Input: %s\n", input_string);
  Log("==================== RUN ====================\n");
  if (input.length() == 0) input = "_";
  Tape tapes[N] = { Tape(input) };
  for (auto tp : tapes) {
    syms += tp.s[tp.head];
  }
  int step = 0;
  bool halt = false;
  while (!halt) {
    halt = true;
    Log("%-8s: %d\n", "Step", step);
    for (int i = 0; i < N; i++) {
      Log("Index%-3d: ", i);
      int shift = tapes[i].begin;
      int head = tapes[i].head;
      int start = tapes[i].s.find_first_not_of(B);
      int end = tapes[i].s.find_last_not_of(B);
      if (start == string::npos) { // tape contains only of blank symbol
        start = head;
        end = head;
      }
      for (int ind = min(start, head); ind <= max(end, head); ind++) {
        Log("%d ", abs(shift + ind));
      }
      Log("\n");
      Log("Tape%-4d: ", i);
      for (int ind = min(start, head); ind <= max(end, head); ind++) {
        Log("%-*c", int(to_string(abs(shift + ind)).length())+1, tapes[i].s[ind]);
      }
      Log("\n");
      Log("Head%-4d: ", i);
      for (int ind = min(start, head); ind <= max(end, head); ind++) {
        if (ind == head) {
          Log("%-*s", int(to_string(abs(shift + ind)).length())+1, "^");
        } else {
          Log("%-*s", int(to_string(abs(shift + ind)).length())+1, "");
        }
      }
      Log("\n");
    }
    Log("%-8s: %s\n---------------------------------------------\n", "State", state.c_str());
    if (F.count(state) != 0) break;

    for (auto tf : Delta) {
      if (tf.old_state == state && tf.old_symbols == syms) {
        state = tf.new_state;
        for (int i = 0; i < N; i++) {
          tapes[i].move(tf.new_symbols[i], tf.directions[i]);
          syms.clear();
          for (auto tp : tapes) {
            syms += tp.s[tp.head];
          }
        }
        
        halt = false;
        break;
      }
    }
    step ++;
  }
  
  Log("Result: ");
  int start = tapes[0].s.find_first_not_of(B);
  int end = tapes[0].s.find_last_not_of(B);
  if (start != string::npos) {
    for (int i = start; i <= end; i++) {
      printf("%c", tapes[0].s[i]);
    }
    printf("\n");
  }
  Log("==================== END ====================\n");
}

int main(int argc, char *argv[]) {
  parse_args(argc, argv);
  parser();

  simulate(input_string);
}