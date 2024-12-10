/*
for  gcc version 4.9.2 
valid for not ulca situation
g++ -Wall (-DDEBUG) -static -std=c++11 -o logB
*/

#include <fstream>
#include <iostream>
#include <string>
#include <map>

using namespace std;

/*
data type
 */

using MAP = map<int, string>;

struct UPCUL_211 //after SE
{
    unsigned long bbUeRef;
    int pathThroughTfs;
    int pathThroughTfs2;
    int csiDecisionBitmap;
    int tbs;
    int isUlMuMimoCandidate;
    int muMimoPairedSeType;
    int servCellIndex;
    int spsActivationPending;
    int hasPucchTransmission;
};

struct UPCUL_123 //before SE
{
    unsigned long bbUeRef;
    int srCause;
    int srType;
    int isSrReceived;
};

struct UPCUL_170 //before SE, higher order than UPCUL_123
{
    unsigned long bbUeRef;
    int isSrReceived;
};

struct SE_newTx
{
    unsigned long bbUeRef;
    int pathThroughTfs;
    int pathThroughTfs2;
    int csiDecisionBitmap;
    int tbs;
    int isUlMuMimoCandidate;
    int muMimoPairedSeType;
    int servCellIndex;
    int spsActivationPending;
    int hasPucchTransmission;
    int isSrReceived;
    int srCause;
    int srType;
};

/*
global variable
 */
ifstream fs_in;
ofstream fs_out;

/*
internal func
 */
bool startsWith(string &s, const string &sub)
{
    return s.find(sub) == 0 ? true : false;
}

bool endsWith(string &s, const string &sub)
{
    return s.rfind(sub) == (s.length() - sub.length()) ? true : false;
}

//return xvalue(expiring value), not a copy
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

int get_trace_info(string &line, UPCUL_211 &trace)
{
    trace.bbUeRef = stoul(scan_item(line, "bbUeRef="), 0, 0);
    trace.servCellIndex = stoi(scan_item(line, "servCellIndex="));
    trace.isUlMuMimoCandidate = stoi(scan_item(line, "isUlMuMimoCandidate="));
    trace.muMimoPairedSeType = stoi(scan_item(line, "muMimoPairedSeType="));
    trace.tbs = stoi(scan_item(line, "tbs="));
    trace.pathThroughTfs = stoi(scan_item(line, "pathThroughTfs="), 0, 0);
    trace.pathThroughTfs2 = stoi(scan_item(line, "pathThroughTfs2="), 0, 0);
    trace.csiDecisionBitmap = stoi(scan_item(line, "csiDecisionBitmap="), 0, 0);
    trace.spsActivationPending = stoi(scan_item(line, "spsActivationPending="));
    trace.hasPucchTransmission = stoi(scan_item(line, "hasPucchTransmission="));
    return 0;
}

int get_trace_info(string &line, UPCUL_123 &trace)
{
    trace.bbUeRef = stoul(scan_item(line, "bbUeRef="), 0, 0);
    trace.isSrReceived = stoi(scan_item(line, "isSrReceived="));
    trace.srCause = stoi(scan_item(line, "srCause="));
    trace.srType = stoi(scan_item(line, "srType="));
    return 0;
}

int get_trace_info(string &line, UPCUL_170 &trace)
{
    trace.bbUeRef = stoul(scan_item(line, "bbUeRef="), 0, 0);
    trace.isSrReceived = stoi(scan_item(line, "isSrReceived="));
    return 0;
}

int get_se_info(string &line, SE_newTx &se)
{
    se.bbUeRef = stoul(scan_item(line, "bbUeRef:", ","));
    se.servCellIndex = stoi(scan_item(line, "servCellIndex:",",")); 
    se.isUlMuMimoCandidate = stoi(scan_item(line, "isUlMuMimoCandidate:",","));
    se.muMimoPairedSeType = stoi(scan_item(line, "muMimoPairedSeType:",","));
    se.tbs = stoi(scan_item(line, "tbs:",","));
    se.pathThroughTfs = stoi(scan_item(line, "pathThroughTfs:", ","));
    se.pathThroughTfs2 = stoi(scan_item(line, "pathThroughTfs2:", ","));
    se.csiDecisionBitmap = stoi(scan_item(line, "csiDecisionBitmap:", ","));
    se.spsActivationPending = stoi(scan_item(line, "spsActivationPending:", ","));
    se.hasPucchTransmission = stoi(scan_item(line, "hasPucchTransmission:", ","));
 
    se.isSrReceived = stoi(scan_item(line, "isSrReceived:",","));
    se.srCause = stoi(scan_item(line, "srCause:",","));
    se.srType = stoi(scan_item(line, "srType:",","));
    return 0;
}

bool trace_matched(SE_newTx *se, UPCUL_123 *trace_123, 
    UPCUL_170 *trace_170,UPCUL_211 *trace_211)
{
    if(trace_170 != nullptr
       && trace_170->isSrReceived != se->isSrReceived)
    {
        return false;
    } 

    if (!(
     trace_211->csiDecisionBitmap == se->csiDecisionBitmap
     && trace_211->hasPucchTransmission == se->hasPucchTransmission
     && trace_211->pathThroughTfs == se->pathThroughTfs
     && trace_211->servCellIndex == se->servCellIndex
     && trace_211->spsActivationPending == se->spsActivationPending
     && trace_211->tbs == se->tbs))
    {
        return false;
    }

    if ((trace_211->isUlMuMimoCandidate)
     && (trace_211->muMimoPairedSeType != se->muMimoPairedSeType))
    {
        return false;
    }
       
    if (!(
        trace_123->srCause == se->srCause 
        && trace_123->isSrReceived == se->isSrReceived 
        && trace_123->srType == se->srType))
    {
        return false;
    }
    return true;
}

/*
return false if not found
 */
template <typename T>
bool get_trace(unsigned long bbUeRef,MAP &map_trace,T &trace)
{
    string str_trace;
    auto iter = map_trace.find(bbUeRef);
    if (iter != map_trace.end())
    {
        str_trace = map_trace[bbUeRef];
        fs_out << str_trace << endl;
    }
    else
    {
        return false;
    }
    get_trace_info(str_trace, trace);
    if (map_trace.size() > 1000)
    {
        map_trace.clear();
    }
    else
    {
        map_trace.erase(iter);
    }
    return true;

}

/*
comapre the trace with golden
if matched return 0;
else if preceeding trace missing return >0
else <0
*/
int compare_trace(string &str_se_info, MAP &map_123,MAP &map_170,
 UPCUL_211 &trace_211)
{
    SE_newTx se;
    get_se_info(str_se_info, se);

    UPCUL_123 trace_123;
    if (!get_trace(se.bbUeRef,map_123,trace_123))
    {
        return 1;
    }
    UPCUL_170 trace_170;
    if (!get_trace(se.bbUeRef,map_170,trace_170))
    {
        return 2;
    }
    fs_out << str_se_info << endl;

    if (trace_matched(&se, &trace_123, &trace_170, &trace_211))
        return 0;
    else
        return -1;
}

void update_progress(int lineno)
{
    static int count = 1;
    if (lineno > count * 100000)
    {
        if (count == 1)
            cerr << endl
                 << "progress:\t"; //cerr is not buffered
        cerr << "#";
        count++;
    }
}

/*
return true if the requried se type met
 */
bool read_se(string &str_se_info, unsigned long &lineno, unsigned long &bbUeRef)
{
    string line;
    bool b_se_found = false;
    while (getline(fs_in, line))
    {
        lineno++;
        str_se_info += line + "\n";
        if (endsWith(line,"newTxSeData {") )//need verify
        {
            b_se_found = true;
        }
        else if (line.find("bbUeRef:") < string::npos)
        {
            bbUeRef = stoul(scan_item(line, "bbUeRef:", ","));
        }
        else if (endsWith(line,"}"))
        {
            break;
        }
    }
    return b_se_found;
}

int checklog(long &count)
{
    unsigned long lineno = 0; //without long long, the old gcc can't match stoi
    int status = 0;
    unsigned long bbUeRef_se = 0;
    bool b_se_found = false;
    string str_se_info;
    MAP map_123,map_170;
    const int MAX_SE = 50;

    for (string line; (status >= 0) && count < MAX_SE && getline(fs_in, line);)
    {
        ++lineno;
        update_progress(lineno);
        if (line.find("<!UPCUL.211!>") < string::npos)
        {
            unsigned long bbUeRef = stoul(scan_item(line, "bbUeRef="), 0, 0);
            if (b_se_found && bbUeRef == bbUeRef_se)
            {
                UPCUL_211 trace_211;
                get_trace_info(line, trace_211);
                count++;
                status = compare_trace(str_se_info, map_123,map_170, trace_211);
                fs_out <<lineno<<":"<<line <<endl;
            }
        }
        else if (line.find("<!UPCUL.170!>") < string::npos)
        {
            unsigned long bbUeRef = stoul(scan_item(line, "bbUeRef="), 0, 0);
            map_170[bbUeRef] = to_string(lineno) + ":" + line + "\n";
        }
        else if (line.find("<!UPCUL.123!>") < string::npos)
        {
            unsigned long bbUeRef = stoul(scan_item(line, "bbUeRef="), 0, 0);
            map_123[bbUeRef] = to_string(lineno) + ":" + line + "\n";
        }
        else if (line.find("seSchedInfoList {") < string::npos)
        {
            str_se_info = to_string(lineno) + ":" + line + "\n";
            b_se_found = read_se(str_se_info, lineno, bbUeRef_se);
        }
        else
        {
            /* do nothing*/
        }
    }
    map_123.clear();
    map_170.clear();
    return status;
}

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        cerr << "Usage:" << argv[0] << " [FILE] \nused for EBB log verification \ndeveloped  by Alex" << endl;
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
