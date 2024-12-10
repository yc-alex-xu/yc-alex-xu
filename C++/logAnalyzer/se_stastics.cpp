/*
g++ -Wall -std=c++14  -lpthread 

if c++14 not supported, 
$ g++ -Wall -std=c++1y se_statics.cpp -lpthread 
and change the 
    40ms
to
    std::chrono::microseconds t(40);
*/
#include <fstream>
#include <iostream>
#include <string>
#include <map>
#include <chrono>
#include <thread>
using namespace std;

/* data type
*/
using MAP_INT_INT = map<int, int>;

/* global variable
*/
bool bRunning = true;

int spin()
{
    int i = 0;
    const char chars[] = "|/-\\";
    cerr << "running: ";
    for (;;)
    {
        if (!bRunning)
        {
            cerr << "\bDone!\n";
            break;
        }
        cerr << "\b" << chars[i++ % 4];
        this_thread::sleep_for(40ms); // 25 frames/second
    }
    return 0;
}

template <typename T1, typename T2>
ostream &operator<<(ostream &os, const map<T1, T2> &m)
{
    for (const auto &v : m)
        os << "\t" << v.first << "\t" << v.second << endl;
    return os;
}

string scan_item(string &line, string pat) //return xvalue(expiring value)
{
    auto n1 = line.find(pat);
    if (n1 == string::npos)
        return "";                         //can't find the pattern
    return line.substr(n1 + pat.length()); //till end of line
}

void checklog(ifstream &fs_in)
{
    unsigned long lineno = 0;
    bool b_UpcDlMacCeFiUlValidationInfoInd = false;
    MAP_INT_INT count_UpcDlMacCeFiUlValidationInfoInd;
    MAP_INT_INT count_validation_passed;
    MAP_INT_INT count_validation_failed;
    bool b_UpcDlMacCeFiUlSchedInfoInd = false;
    MAP_INT_INT count_UpcDlMacCeFiUlSchedInfoInd;
    MAP_INT_INT count_newTxSeData;
    MAP_INT_INT count_nonAdaptReTxSeData;
    MAP_INT_INT count_adaptReTxSeData;
    int cellId;

    thread t{spin};
    for (string line; getline(fs_in, line);)
    {
        ++lineno;
        if (line.find("UpcDlMacCeFiUlValidationInfoInd") < string::npos)
        {
            b_UpcDlMacCeFiUlValidationInfoInd = true;
        }
        else if (line.find("UpcDlMacCeFiUlSchedInfoInd") < string::npos)
        {
            b_UpcDlMacCeFiUlSchedInfoInd = true;
        }
        else if (line.find("cellId:") < string::npos)
        {
            cellId = stoi(scan_item(line, "cellId:"));
            if (b_UpcDlMacCeFiUlValidationInfoInd)
            {
                count_UpcDlMacCeFiUlValidationInfoInd[cellId]++;
                b_UpcDlMacCeFiUlValidationInfoInd = false;
            }
            else if (b_UpcDlMacCeFiUlSchedInfoInd)
            {
                count_UpcDlMacCeFiUlSchedInfoInd[cellId]++;
                b_UpcDlMacCeFiUlSchedInfoInd = false;
            }
            else
            {
                bRunning = false;
                t.join();
                cerr << "lineno\t" << lineno << endl
                     << "cellId: appeared in unexpected place" << endl;
                exit(-3);
            }
        }
        else if (line.find("newTxSeData") < string::npos)
        {
            count_newTxSeData[cellId]++;
        }
        else if (line.find("nonAdaptReTxSeData {") < string::npos)
        {
            count_nonAdaptReTxSeData[cellId]++;
        }
        else if (line.find("adaptReTxSeData") < string::npos)
        {
            count_adaptReTxSeData[cellId]++;
        }
        else if (line.find("validResultBitmap:") < string::npos)
        {
            if (stoi(scan_item(line, "validResultBitmap:")) >= 256)
                count_validation_passed[cellId]++;
            else
                count_validation_failed[cellId]++;
        }
        else
        {
            /* do nothing*/
        }
    } //for

    bRunning = false;
    t.join();
    cout << "\nUpcDlMacCeFiUlSchedInfoInd\n"
         << count_UpcDlMacCeFiUlSchedInfoInd
         << "\tnewTxSeData\n"
         << count_newTxSeData
         << "\tnonAdaptReTxSeData\n"
         << count_nonAdaptReTxSeData
         << "\tadaptReTxSeData\n"
         << count_adaptReTxSeData
         << "\nUpcDlMacCeFiUlValidationInfoInd\t\n"
         << count_UpcDlMacCeFiUlValidationInfoInd
         << "\tpassed\n"
         << count_validation_passed
         << "\tfailed\n"
         << count_validation_failed;
}

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        cerr << "Usage:" << argv[0] << " [dec file]\n"
             << "used for statistics for Signal UpcDlMacCeFiUlValidationInfoInd & UpcDlMacCeFiUlSchedInfoInd only\n"
             << "ver 1.1 \ndeveloped  by Xu YangChun\n";
        return -1;
    }
    ifstream fs_in(argv[1]);
    if (!fs_in)
    {
        cerr << argv[1] << " open error!" << endl;
        exit(-2);
    }
    checklog(fs_in);
    fs_in.close();
    return 0;
}
