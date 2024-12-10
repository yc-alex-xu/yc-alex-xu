# background

to check whether the change introduce the new klocwork issue

# compile

```bash
$ icc kw_cmp.cpp -std=c++14 -o kw_cmp
```
# design
It is difficult to check whether there is a new issue if the  code block is moved to the other files.  So here don't take the filename + issue desc as key, only taking issue desc as a key, if its occurrence increased, there is  new issues generated. 

Surely, it can't identify such situation:  the same kind of issue increase/decrease at the same time, e.g.  a issue is fixed in one code block, but generated in the other code block.



# usage

```
$ ./kw_cmp kwreport.txt kwreport.txt 
 NO new issue!
$ ./kw_cmp kwreport.txt kwreport2.txt 
'nInitFlag' might be used uninitialized in this function.       +2
```