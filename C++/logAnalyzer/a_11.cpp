/*
for  gcc version 4.9.2 
g++ -Wall -std=c++11
*/
#include <fstream>
#include <iostream>
#include <string>
#include <map>
using namespace std;

/*data type
*/
struct UPCUL_107
{
  int bbUeRef;
  int crnti;
  int prbListStart;
  int prbListEnd;
  int qm;
  int tbs; //bits
};

struct SE_IUA
{
  int bbUeRef;
  int crnti;
  int iuaStartSb;
  int nrOfIuaPrbs;
  int iuaModulation;
  int iuaTbs; //bytes
};

/*global variable
*/
ifstream fs_in;
ofstream fs_out;

//return xvalue(expiring value)
string scan_item(string &line, string pat, string ending = " ")
{
  string::size_type n1, n2;
  n1 = line.find(pat);
  if (n1 == string::npos)
  {
    return "";
  }
  n2 = line.find(ending, n1);
  if (n2 == string::npos)
  {
    return "";
  }
  return line.substr(n1 + pat.length(), n2 - n1 - pat.length());
}

int get_trace_info(string &line, UPCUL_107 &trace)
{
  trace.bbUeRef = stoi(scan_item(line, "bbUeRef="), 0, 16);
  trace.crnti = stoi(scan_item(line, "crnti="));
  trace.qm = stoi(scan_item(line, "qm="));
  trace.tbs = stoi(scan_item(line, "tbs="));
  trace.prbListStart = stoi(scan_item(line, "prbListStart="));
  trace.prbListEnd = stoi(scan_item(line, "prbListEnd="));
  return 0;
}
int get_se_info(string &str_se_info, SE_IUA &se)
{
  se.bbUeRef = stoi(scan_item(str_se_info, "bbUeRef", ","));
  se.crnti = stoi(scan_item(str_se_info, "rnti", ","));
  se.iuaStartSb = stoi(scan_item(str_se_info, "iuaStartSb", ","));
  se.nrOfIuaPrbs = stoi(scan_item(str_se_info, "nrOfIuaPrbs", ","));
  se.iuaModulation = stoi(scan_item(str_se_info, "iuaModulation", ","));
  se.iuaTbs = stoi(scan_item(str_se_info, "iuaTbs", ","));
  return 0;
}

bool trace_matched(SE_IUA &se, UPCUL_107 &trace)
{
  int qm[] = {2, 4, 6, 8, 0}; //UPCCOMMON_modulationE
  return ((trace.crnti == se.crnti) &&
          (trace.prbListStart == se.iuaStartSb) &&
          (trace.prbListEnd - trace.prbListStart + 1 == se.nrOfIuaPrbs) &&
          (trace.tbs == se.iuaTbs * 8) &&
          (trace.qm == qm[se.iuaModulation]));
}

/*
comapre the trace with SE
return 0 if matched ;
return >0 if preceeding trace missing 
else <0
*/
inline int compare_trace(string &str_se_info, map<int, string> &map_trace)
{
  SE_IUA se;
  get_se_info(str_se_info, se);

  UPCUL_107 trace;
  string str_trace;
  auto search = map_trace.find(se.bbUeRef);
  if (search != map_trace.end())
  {
    str_trace = map_trace[se.bbUeRef];
    fs_out << str_trace << str_se_info;
    get_trace_info(str_trace, trace);
    if (trace_matched(se, trace))
      return 0;
    else
      return -1;
  }
  else
  {
    return 1;
  }
}

//return true if the requried se type met
bool read_se(string &str_se_info, int &lineno)
{
  string line;
  bool b_se_found = false;
  string pat_se_type = "iuaImplicitTxSeData";
  string pat_se_end = "}";

  while (getline(fs_in, line))
  {
    lineno++;
    str_se_info += line + "\n";
    if (line.find(pat_se_type) < string::npos)
    {
      b_se_found = true;
    }
    else if (line.find(pat_se_end) < string::npos)
    {
      break;
    }
  }
  return b_se_found;
}

int checklog(long &count)
{
  string pat_trace = "<!UPCUL.107!>";
  int lineno = 0; //without long long, the old gcc can't match stoi
  map<int, string> map_trace;
  int status = 0;

  for (string line; (status >= 0) && getline(fs_in, line);)
  {
    ++lineno;
    if (line.find(pat_trace) < string::npos) //trace line met
    {
      int bbUeRef = stoi(scan_item(line, "bbUeRef="), 0, 16);
      map_trace[bbUeRef] = to_string(lineno) + ":" + line + "\n";
    }
    else if (line.find("seSchedInfoList {") < string::npos) //line such as "bbUeRef 16777216" met
    {
      string str_se_info = to_string(lineno) + ":" + line + "\n";
      if (read_se(str_se_info, lineno))
      {
        count++;
        status = compare_trace(str_se_info, map_trace);
      }
    }
    else
    {
      /* do nothing*/
    }
  }
  map_trace.clear();
  return status;
}

int main(int argc, char *argv[])
{
  if (argc < 2)
  {
    cerr << "Usage:" << argv[0] << " [FILE] \n"
         << "used for EBB IUA implicit grant log verification" << endl
         << "developed  by Xu YangChun" << endl;
    exit(-1);
  }
  fs_in.open(argv[1]);
  if (!fs_in.good())
  {
    cerr << argv[1] << " open error!" << endl;
    exit(-2);
  }
  string fn("result.txt");
  fs_out.open(fn, ios::trunc);
  if (!fs_out.good())
  {
    cerr << fn << " open error!" << endl;
    exit(-3);
  }
  long count = 0;
  if (checklog(count) < 0)
    cout << endl
         << count << " se verified,last one not matched " << endl;
  else
    cout << endl
         << count << " se verified,all matched" << endl;
  cout << "please double check via " << fn << endl;
  fs_in.close();
  fs_out.close();
  return 0;
}
