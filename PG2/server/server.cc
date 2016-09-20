// Ben Harris

// Receives input from client, runs specified program, sends output back down the socket

#include <iostream>
#include <fstream>
#include <cstdlib>
#include <string>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <fdstream/fdstream.h>
#include <fcntl.h>
#include "server.h"
using namespace std;

int main(int argc, char **argv){

  int sc = socket(AF_INET, SOCK_STREAM, 0);
  sockaddr_in sin;
  sin.sin_family = AF_INET;
  sin.sin_port = htons(12346);
  sin.sin_addr.s_addr = INADDR_ANY;

  int b = bind(sc, (sockaddr *)&sin, sizeof(sockaddr_in));
  if(b < 0) {perror("bind"), exit(1);}

  listen(sc, 1);

  int s = accept(sc, NULL, NULL);
  if(s < 0) {perror("accept"); exit(1);}

  fdistream sock_in(s);
  fdostream sock_out(s);

  string exe;
  char c;

  while(1){
    getline(sock_in, exe);
    if(sock_in.eof()) break;
    cout << exe << endl;
    // get the input file from the socket and decode it
    // input file terminates at \0\1
    ofstream tmp ("input_file.txt");
    while(sock_in.get(c)){
      if(c == '\0'){
        sock_in.get(c);
        if(c == '\1'){ // end the input file
          cout << "---end of input file---" << endl;
          break;
        }else tmp << c;
      }else tmp << c;
    }
    tmp.close();

    pid_t pid = fork();
    if(pid < 0) exit(1);
    if(pid > 0){ // parent
      int status;
      wait(&status);

      cout << exe << " done. sending results back down the socket." << endl;

      ifstream tm ("temp.tmp");
      while(tm.get(c)){
        if(c == '\0') sock_out << '\0' << '\0';
        else sock_out << c;
      }
      tm.close();
      sock_out << '\0' << '\1';
      sock_out.flush();

    }else{ // child. program executed here

      int fd = open("input_file.txt", O_RDONLY);
      cout << "child process: about to execute " << exe << endl;
      dup2(fd, 0);
      close(fd);
      fd = open("temp.tmp", O_TRUNC | O_CREAT | O_WRONLY, 0700);
      dup2(fd, 1);
      close(fd);
      execl(exe.c_str(), exe.c_str(), NULL);

    }

  }

  return 0;
}
