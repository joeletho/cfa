# Character Frequency Analyzer

#### This program serves two purposes: 1) obtaining the number or occurrences of ASCII characters in a given file, and 2) obtain the rank of ASCII characters in given file.

<img align="left" width="200" height="343" src="https://github.com/joeletho/cfa/blob/main/assets/images/cfa_count.png">
<img align="center" width="200" height="343" src="https://github.com/joeletho/cfa/blob/main/assets/images/cfa_rank.png">

#### To begin, clone the repo and move into the directory:
```bash
$ git clone --recursive "https://github.com/joeletho/cfa"
$ cd cfa
```
#### Using CMake:
```bash
$ cmake --build .
```

#### Using Clang:
```bash
$ clang++ -Wall -std=c++17 main.cpp -o cfa
```
###### _*Note: This program has only been compiled with Clag and tested on Windows 10._
###### _**Note: C++ stdlib 17 must be used._

#### To run:
```bash
$ ./cfa
```
#### Pass arguments to obtain different display options:
```bash
$ ./cfa -count
$ ./cfa -rank
$ ./cfa -test
```
#### Using the `-test` argument provides a testing environment where the text input source, display and character parsing options, and sort methods can be selected explicitly. The `-count` and `-rank` arguments provide only a single role, in which a given file is analyzed and displays results in alphabetical order. This could be expanded to allow for additional arguments specifying the analysis criteria.
