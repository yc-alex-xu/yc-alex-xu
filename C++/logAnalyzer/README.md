
### background
There was a project about new log for troubleshooting, which verifier need compare with existed logs for correctness. 

The log file size is 3~10G bytes, so I considered using the C++/STL to do the log analyze instead of Python.
### tool for log comparing
* [verify the IUA type log ](a_11.cpp)
* [verify the NewTx type ](c_11.cpp)

Vector/Map/Regex were used at firstly, later only map was used.

Regex was abandoned because it run slower than using string::find() for pattern matching as tested.

Vector was used to store a list of info appear before info needed appear, but it is found needless when running the verification tools, so was abandoned too.

The key of Map is "bbUeRef", id of scheduling entity will be compared.

### tool for log statistics
During the full load test, lots message were lost. It is quite difficult for tester to do statistics then to find out why message was lost just using Linux commands. so I wrote the tool for tester.

* [statistics of message and its status](se_stastics.cpp)

#### spinning indicator
Since the log file is quite big, so it need about 30~60s to complete. so a spinning indication was designed, to tell the tester that the program is progressing.
It need start a sperate thread for it:

```C++
this_thread::sleep_for(40ms); // 25 frames/second
```

### something learned
#### regex lib issue
The program used the Regex lib firstly, and it work fine under g++ 4.9x/g++8.0, but not work under gcc version 4.8.5/clang version 6.0.1. And it is a known issue.

But regex lib can make code simpler.

* [regex example](regex.cpp)

#### valgrind to check memory leak
http://valgrind.org

the framework work fine, not forget to disable optimize when compilation.

```bash
g++ -g -std=c++11 -o logA a.cpp
valgrind --tool=memcheck --leak-check=full ./logA BB.dec
```

