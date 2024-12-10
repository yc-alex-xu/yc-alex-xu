/*
Author: Alex Xu
*/
#include <fstream>
#include <iostream>
#include <map>
#include <string>
using namespace std;

map<string, int> map_baseline;

void parse_file(ifstream &fs, bool cmp) {
  for (string line; getline(fs, line);) {
    if (line.find("(Local) /") < string::npos) {
      getline(fs, line);
      auto it = map_baseline.find(line);
      if (it == map_baseline.end())
        map_baseline[line] = 1;
      else if (cmp)
        map_baseline[line]--;
      else
        map_baseline[line]++;
    }
  }
}

void build_baseline(ifstream &fs) { parse_file(fs, false); }
void cmp_baseline(ifstream &fs) { parse_file(fs, true); }

int main(int argc, char *argv[]) {
  if (argc < 3) {
    cerr << "Usage:compare the kw report with baseline\n"
         << argv[0] << " <baseline file> <new file>" << endl;
    exit(-1);
  }
  ifstream fs;
  fs.open(argv[1]);
  if (!fs.good()) {
    cerr << argv[1] << " open error!" << endl;
    exit(-2);
  }
  build_baseline(fs);
  fs.close();
  fs.open(argv[2]);
  if (!fs.good()) {
    cerr << argv[2] << " open error!" << endl;
    exit(-3);
  }
  cmp_baseline(fs);
  fs.close();
  int count = 0;
  for (auto &it : map_baseline)
    if (it.second < 0) {
      cout << it.first << "\t+" << -it.second << endl;
      count++;
    }
  if (count == 0)
    cout << " NO new issue!" << endl;
  return 0;
}