// Ben Harris
// forkandsquare.cc forks itself:
// the parent sends a user-inputted integer to the child process through a pipe and waits for something to be returned on a different pipe
// the child reads from one pipe, squares the value and writes it to the other pipe.

#include <iostream>
#include <cstdlib>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fdstream/fdstream.h>
using namespace std;

int send[2];
int ret[2];

void child(){
  // the child process reads information from the read end of one pipe,
  // parses it as an int, squares it, and writes that to the other pipe.

  close(send[1]); // close write end of send
  close(ret[0]); // close read end of ret

  fdistream send_in (send[0]);
  string s;
  getline(send_in, s);
  send_in.close();

  int h = atoi(s.c_str());
  h *= h;

  fdostream ret_out (ret[1]);
  ret_out << h << endl;
  ret_out.close();
}

void parent(){
  // the parent process asks the user for an integer, writes that to a pipe,
  // waits for the child to die, and prints what it finds on the read end of the other pipe.

  close(send[0]); // close read end of send
  close(ret[1]); // close write end of ret

  cout << "Enter an integer to square: " << endl;
  string s;
  getline(cin, s);
  fdostream send_out (send[1]);
  send_out << s << endl; // pass inpt to child process through a pipe....
  send_out.close();

  int status;
  wait(&status);

  fdistream ret_in (ret[0]);
  getline(ret_in, s);
  ret_in.close();

  cout << "result: " << s << endl;
}

int main(int argc, char **argv){
  int send_result = pipe(send);
  int ret_result = pipe(ret);
  if(send_result == -1 || ret_result == -1) exit(1);

  pid_t pid = fork();
  if(pid < 0) exit(1); // error check
  else if(pid == 0) child();
  else parent();

  return 0;
}
