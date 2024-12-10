/*
for regex test 
wsl/redhat 6: g++ 4.9+
$g++ -g -Wall -std=c++11 regex.cpp

or 
centos 7/clang 6.0
$clang++ -g -std=c++11 -stdlib=libc++ -lc++abi regex.cpp
*/
#include <fstream>
#include <iostream>
#include <regex>
#include <string>

using namespace std;

void find_all_pair(ofstream &out, string &input)
{
  regex pat{R"(\s(\w+)=(\w+))"};
  for (sregex_iterator p(input.begin(), input.end(), pat); p != sregex_iterator{}; ++p)
  {
    out << (*p)[0] << "\t\t" << (*p)[1] << "/" << (*p)[2] << endl;
  }
}

int log_filter(ifstream &in, ofstream &out)
{
  regex pat{R"(<!UPCUL.107!> cellId=\d*, (bbUeRef=(\w+)))"};

  int lineno = 0;
  string line;
  while (getline(in, line))
  {
    ++lineno;
    smatch matches; // matched strings go here
    if (regex_search(line, matches, pat))
    {
      out << lineno << ": " << matches[0] << '\n'; // the complete match
      if (1 < matches.size() && matches[1].matched)
        out << matches[1] << endl; // submatch
      if (2 < matches.size() && matches[2].matched)
        out << matches[2] << endl; // submatch
      find_all_pair(out, line);
    }
  }
  return 0;
}

int main(int argc, char *argv[])
{
  if (argc < 2)
  {
    std::cout << "Usage:" << argv[0] << " [FILE] \n";
    exit(-1);
  }
  ifstream in(argv[1]);
  if (!in)
  {
    cerr << argv[1] << " open error!" << endl;
  }

  string fn("result.txt");
  ofstream out(fn);
  if (!out)
  {
    cerr << fn << " open error!" << endl;
  }
  log_filter(in, out);
  cout << "you can double check in  " << fn << endl;
  in.close();
  out.close();
  return 0;
}
