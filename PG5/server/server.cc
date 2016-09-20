// Ben Harris

// Multiplayer command-line shooter server

#include <iostream>
#include <fstream>
#include <sstream>
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
#include <poll.h>
#include <fcntl.h>
#include "server.h"
using namespace std;

ppos pos[26];
Board board;
struct pollfd fds[26];

int main(int argc, char **argv){
  for(int r = 1; r < 26; r++) fds[r].fd = -1;
  fds[0].fd = socket(AF_INET, SOCK_STREAM, 0);
  sockaddr_in sin;
  sin.sin_family = AF_INET;
  sin.sin_port = htons(12421);
  sin.sin_addr.s_addr = INADDR_ANY;
  int b = bind(fds[0].fd, (sockaddr *)&sin, sizeof(sockaddr_in));
  if(b < 0){perror("bind"); exit(1);}
  fds[0].events = POLLIN;
  listen(fds[0].fd, 26);
  string line;

  while(1){
    if(poll(fds, 26, -1) < 0){ perror("poll"); exit(1);}
    if(fds[0].revents & POLLIN){
      int f = 1;
      for(; f <= 25; f++) if(fds[f].fd == -1) break;
      new_player(f);
    }
    for(int i = 1; i <= 25; i++){
      if(fds[i].revents & POLLIN && fds[i].fd != -1){
        if(getline(pos[i].read, line)){
          do_line(line, i);
        }else{
          ostringstream msg;
          msg << "Player " << i << "'s client disconnected";
          kill_player(i);
          alert(msg.str());
        }
      	update();
      }
    }
  }
  return 0;
}

// METHODS  ////////////////////////////////////////////////////////////////////
void do_line(string line, int player){
  if(line == "MOVE"){ pos[player].move_forward(); }
  else if(line == "LEFT"){ pos[player].turn_left(); }
  else if(line == "RIGHT"){ pos[player].turn_right(); }
  else if(line == "FIRE"){ shoot(player); }
  else if(line == "SCAN"){ scan(player);}
  else if(line == "EXIT"){
    kill_player(player);
    ostringstream msg;
    msg << "Player " << player << " threw in the towel";
    alert(msg.str());
  }
  else if(line == "HELP" || line == "help"){
    pos[player].write << "Commands are: MOVE, LEFT, RIGHT, SCAN, FIRE, and EXIT. Kill other players. Join and leave at any time." << endl;
  }
  else{ pos[player].write << "invalid command. type HELP for more info." << endl; }
  // cout << "Player " << player << " at (" << pos[player].row << ", " << pos[player].col << ") facing " << pos[player].dr << "dr and " << pos[player].dc << "dc" <<endl;
}

void new_player(int player){
  if(player > 25) return;
  srand(time(NULL));
  ostringstream msg; // need ostringstream to concatenate int with strings
  msg << "Player " << player << " joined the game";
  alert(msg.str());
  fds[player].fd = accept(fds[0].fd, NULL, NULL);
  int i = rand()%10, j = rand()%10;
  while(board[i][j]){ i = rand()%10; j = rand()%10;}
  pos[player].row = i;
  pos[player].col = j;
  pos[player].dr = rand()%3 - 1;
  pos[player].dc = pos[player].dr == 0? rand()%2? 1: -1: 0;
  fds[player].events = POLLIN;
  fds[player].revents = 0;
  pos[player].read.clear();
  pos[player].read.attach(fds[player].fd);
  pos[player].write.clear();
  pos[player].write.attach(fds[player].fd);
  pos[player].write << "You are player " << player << endl;
  update();
}

void kill_player(int player){
  pos[player].read.close();
  pos[player].write.close();
  fds[player].events = 0;
  fds[player].revents = 0;
  fds[player].fd = -1;
}

void alert(string msg){
  for(int i = 1; i <= 25; i++) if(fds[i].fd != -1) pos[i].write << msg << endl;
}

void shoot(int player){
  update();
  int curr;
  for(int i = 1; i <=5; i++){
    curr = board[(pos[player].row + i*pos[player].dr + 10) % 10][(pos[player].col + i*pos[player].dc + 10) % 10];
    if(curr == 0) continue;
    else{
      pos[player].write << "You shot player " << curr << endl;
      pos[curr].write << "You were shot by player " << player << endl;
      ostringstream msg;
      msg << "Player " << player << " shot " << curr;
      kill_player(curr);
      alert(msg.str());
      break;
    }
  }
  update();
}

void update(){
  for(int i = 0 ; i < 10; i++) for(int j = 0; j < 10; j++) board[i][j] = 0;
  for(int i = 1; i <= 25; i++){
    if(fds[i].fd == -1) continue;
    if(!board[pos[i].row][pos[i].col]){
      board[pos[i].row][pos[i].col] = i;
    }else{
      int other_player = board[pos[i].row][pos[i].col];
      ostringstream msg;
      msg << "Players " << i << " and " << other_player << " died in a collision";
      pos[i].write << "You died in a collision with player " << other_player << endl;
      kill_player(i);
      pos[other_player].write << "You died in a collision with player " << i << endl;
      kill_player(other_player);
      alert(msg.str());
    }
  }
  // for(int i = 0 ; i < 10; i++){ // debug
  //   for(int j = 0; j < 10; j++)
  //     cout << board[i][j] << " ";
  //   cout << endl;
  // }
}

void scan(int player){
  update();
  pos[player].write << "printscan" << endl;
  for(int i = 5; i >= 1; i--){
    for(int j = 2; j >= -2; j--)
      pos[player].write << board[(pos[player].row - j*pos[player].dc + i*pos[player].dr + 10)%10][(pos[player].col + i*pos[player].dc + j*pos[player].dr + 10)%10] << " ";
    pos[player].write << "\n_ _ _ _ _\n";
  }
  pos[player].write.flush();
}
