// Ben Harris

// Shooter game client

#include <iostream>
#include <string>
#include <cstdlib>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <fdstream/fdstream.h>
#include <poll.h>
using namespace std;

int main (int argc, char **argv) {
  char *hostaddr;
  if(argc == 1) hostaddr = (char*) "euclid.nmu.edu";
  else hostaddr = argv[1];

  struct pollfd pollinfo[2];
  struct hostent *h;
  sethostent (1);
  h = gethostbyname (hostaddr);
  endhostent ();
  int s = socket (AF_INET,SOCK_STREAM,0);
  sockaddr_in sin;
  sin.sin_family = AF_INET;
  sin.sin_port = htons (12421);
  sin.sin_addr = (*(in_addr *)h->h_addr_list[0]);
  int c = connect (s,(sockaddr *)&sin,sizeof(sockaddr_in));
  if (c < 0) {perror ("connect");exit(1);}

  fdistream sinp(s);
  fdostream sout(s);
  pollinfo[0].fd = 0; //stdin
  pollinfo[1].fd = s; //socket
  pollinfo[0].events = POLLIN;
  pollinfo[1].events = POLLIN;
  string line;

  while (true) {
    if (poll (pollinfo, 2, -1) < 0) {perror ("poll"); exit(1);}
    if (pollinfo[0].revents & POLLIN) { //standard input
      if(getline (cin,line)) sout << line << endl;
      else break;
    }
    if (pollinfo[1].revents & POLLIN) { //socket
      if(getline (sinp,line)){
        if(line == "printscan"){
          for(int i = 0; i < 10; i++){
            getline(sinp, line);
            cout << line << endl;
          }
        }else cout << line << endl;
      }
      else{
        cout << "connection lost/closed" << endl;
        break;
      }
    }
  }
  sout.close();
  return 0;
}
