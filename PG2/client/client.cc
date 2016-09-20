// Ben Harris

// Allows user to run a specified program on a remote client
// Uses a socket to send input and receive output

#include <iostream>
#include <cstdlib>
#include <string>
#include <fstream>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <fdstream/fdstream.h>
#include <fcntl.h>
#include "client.h"
using namespace std;

int main(int argc, char **argv){

  if(argc != 2){
    cout << "Please add the remote hostname as an argument." << endl;
    exit(1);
  }
  // set up socket, connect to hostname
  struct hostent *h;
  sethostent(1);
  h = gethostbyname(argv[1]);
  endhostent();

  int s = socket(AF_INET, SOCK_STREAM, 0);
  sockaddr_in sin;
  sin.sin_family = AF_INET;
  sin.sin_port = htons(12346);
  sin.sin_addr = (*(in_addr *) h->h_addr_list[0]);
  int c = connect(s, (sockaddr *)&sin, sizeof(sockaddr_in));
  if(c < 0) {perror("connect"); exit(1);}

  fdistream sock_in (s);
  fdostream sock_out (s);

  string exe, in_path, out_path;
  char ch;

  while(1){ // keep running programs until user says N
    cout << "Program to run: ";
    getline(cin, exe);
    cout << exe << " will be run " <<  endl;
    sock_out << exe << endl;

    cout << "Input filepath: ";
    getline(cin, in_path);
    fstream in_file (in_path.c_str());

    cout << "Output filepath: ";
    getline(cin, out_path);
    ofstream out_file (out_path.c_str());

    // read and encode the input file
    // every \0 is replaced with \0\0
    // after file is read completely, send \0\1 to signal eof without closing socket
    // send input file now
    while(in_file.get(ch)){
      if(ch == '\0') sock_out << '\0' << '\0';
      else sock_out << ch;
    }
    in_file.close();
    sock_out << '\0' << '\1';
    sock_out.flush();

    // server should execute exe on in_file now

    // wait for response
    while(sock_in.get(ch)){
      if(ch == '\0'){
        sock_in.get(ch);
        if(ch == '\1') break;
        else out_file << ch;
      }else out_file << ch;
    }
    out_file.close();

    // ask if want to run another program
    cout << "Would you like to run another program? [Y/N] ";
    string cont;
    getline(cin, cont);
    if(cont == "n" || cont == "N") break;
    else cout << "Ready to run another program." << endl;

  }
  sock_out.close();
  return 0;
}


