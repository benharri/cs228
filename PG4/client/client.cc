// Ben Harris

// Connect 4 client
// Prints what it gets from the socket
// Sends a move from stdin after validating it

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

  struct hostent *h;
  sethostent(1);
  h = gethostbyname(argv[1]);
  endhostent();

  int s = socket(AF_INET, SOCK_STREAM, 0);
  sockaddr_in sin;
  sin.sin_family = AF_INET;
  sin.sin_port = htons(12480);
  sin.sin_addr = (*(in_addr *) h->h_addr_list[0]);
  int c = connect(s, (sockaddr *)&sin, sizeof(sockaddr_in));
  if(c < 0) {perror("connect"); exit(1);}

  fdistream sock_in (s);
  fdostream sock_out (s);

  string curr;
  int currval = 0;

  for(;;){
    while(getline(sock_in, curr)){
      if(sock_in.eof()) break;
      if(curr == "><") break;
      if(curr == "> "){
        cout << ">> ";
        break;
      }else cout << curr << endl;
    }
    if(curr == "><") break;
    getline(cin, curr);
    currval = atoi(curr.c_str());
    if(currval < 1 || currval > 7){
      cout << "Not a valid move." << endl;
      sock_out << 0 << endl;
    }else{
      sock_out << curr << endl;
      cout << "Waiting for other player to move." << endl;
    }
  }

  sock_out.close();
  return 0;
}
