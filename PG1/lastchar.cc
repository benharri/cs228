// Ben Harris
// lastchar.cc takes one command line argument
// returns the last char of that string

#include <iostream>
#include <cstdlib>
#include <cstring>
using namespace std;

int main(int argc, char **argv){
  string ret = argv[1];
  char c = argv[1][strlen(argv[1]) - 1];
  return c;
}
