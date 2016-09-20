// Ben Harris

#ifndef _SERVER_
#define _SERVER_

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
using namespace std;

typedef int Board[10][10];

struct ppos{
  int row, col, dr, dc;
  fdistream read;
  fdostream write;

  void move_forward(){
    row = (row + dr +10) % 10;
    col = (col + dc +10) % 10;
  }
  void turn_left(){
    int tmp = dr;
    dr = -dc;
    dc = tmp;
  }
  void turn_right(){
    int tmp = dr;
    dr = dc;
    dc = -tmp;
  }
};

int main(int argc, char **argv);
void kill_player(int player);
void do_line (string line, int player);
void ready(int player);
void new_player(int player);
void alert(string msg);
void shoot(int player);
void update();
void scan(int player);

#endif
