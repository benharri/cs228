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

// method definitions
typedef char Board[7][6];

const int RED_PLAYER = 'R';
const int BLUE_PLAYER = 'B';


int main(int argc, char **argv);
bool win();
bool place_piece(int player, int col);
string print_board();
char get_board_at(int x, int y);

#endif
