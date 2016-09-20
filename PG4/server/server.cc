// Ben Harris

// This program manages the current game board and communicates with both clients.

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

Board board;

int main(int argc, char **argv){

  int sc = socket(AF_INET, SOCK_STREAM, 0);
  sockaddr_in sin;
  sin.sin_family = AF_INET;
  sin.sin_port = htons(12480);
  sin.sin_addr.s_addr = INADDR_ANY;

  int b = bind(sc, (sockaddr *)&sin, sizeof(sockaddr_in));
  if(b < 0){perror("bind"); exit(1);}

  listen(sc, 1);

  while(1){
    for(int i = 0; i < 7; i++)
      for(int j = 0; j < 6; j++)
        board[i][j] = '|';

    // cout << "Server started. Waiting for players..." << endl;

    int player1 = accept(sc, NULL, NULL);
    if(player1 < 0){perror("accept"); exit(1);}

    fdistream p1in(player1);
    fdostream p1out(player1);

    p1out << "Waiting for opponent to connect." << endl;
    // cout << "Red player connected." << endl;

    int player2 = accept(sc, NULL, NULL);
    if(player2 < 0){perror("accept"); exit(1);}

    fdistream p2in(player2);
    fdostream p2out(player2);

    p2out << "You are the blue player. Waiting for Red to make a move." << endl;
    // cout << "Blue player connected." << endl;

    //// BOTH CONNECTED NOW ////
    // red goes first
    // cout << "Both players connected. Game begin." << endl;

    int turn = RED_PLAYER;
    string curr_move;
    while(!win()){
      // cout << print_board() << endl; // debug to server screen

      // if(turn == RED_PLAYER) cout << "red";
      // else cout << "blue";
      // cout << "'s turn" << endl;

      if(turn == RED_PLAYER){
        p1out << "You are Red (\033[1;31mO\033[0m). Enter the number of the column where you'd like to place a playing piece." << endl;
        p1out << print_board() << endl;
        p1out << "> " << endl;
        getline(p1in, curr_move);
        if(place_piece(RED_PLAYER, atoi(curr_move.c_str()))){
          p1out << print_board() << endl;
          turn = BLUE_PLAYER;
        }else{ // invalid move
          p1out << "Invalid move. Cannot place piece." << endl;
        }

      }else{ // BLUE's turn
        p2out << "You are Blue (\033[1;34mO\033[0m). Enter the number of the column where you'd like to place a playing piece." << endl;
        p2out << print_board() << endl;
        p2out << "> " << endl;
        getline(p2in, curr_move);
        if(place_piece(BLUE_PLAYER, atoi(curr_move.c_str()))){
          p2out << print_board() << endl;
          turn = RED_PLAYER;
        }else{ // invalid move
          p2out << "Invalid move. Cannot place piece." << endl;
        }

      }
    }
    // game is won!
    if(turn == BLUE_PLAYER){
      p1out << "You win!" << endl;
      p2out << print_board() << endl;
      p2out << "You lose :(" << endl;

    }else{
      p1out << print_board() << endl;
      p1out << "You lose :(" << endl;
      p2out << "You win!" << endl;
    }

    p1out << "><" << endl;
    p2out << "><" << endl;


    close(player1);
    close(player2);
  }
  close(b);

  return 0;
}

bool win(){
  char player;
  int i2,j2, counter = 0;

  for(int i = 0; i < 7; i++){
    for(int j = 5; j >= 0; j--){
      player = get_board_at(i, j);
      if(player == '|') continue;
      counter = 1;
      i2 = i + 1; j2 = j;
      while(get_board_at(i2, j2) == player){ // check going E
        i2++; counter++;
        if(counter == 4) return true;
      }
      counter = 1;
      i2 = i; j2 = j + 1;
      while(get_board_at(i2, j2) == player){ // check going N
        j2++; counter++;
        if(counter == 4) return true;
      }
      counter = 1;
      i2 = i + 1; j2 = j + 1;
      while(get_board_at(i2, j2) == player){ // check going NE
        i2++; j2++; counter++;
        if(counter == 4) return true;
      }
      counter = 1;
      i2 = i - 1; j2 = j + 1;
      while(get_board_at(i2, j2) == player){ // check going NW
        i2--; j2++; counter++;
        if(counter == 4) return true;
      }
      counter = 1;
      i2 = i - 1; j2 = j;
      while(get_board_at(i2, j2) == player){ // check going W
        i2--; counter++;
        if(counter == 4) return true;
      }
      counter = 1;
      i2 = i - 1; j2 = j - 1;
      while(get_board_at(i2, j2) == player){ // check going SW
        i2--; j2--; counter++;
        if(counter == 4) return true;
      }
      counter = 1;
      i2 = i; j2 = j - 1;
      while(get_board_at(i2, j2) == player){ // check going S
        j2--; counter++;
        if(counter == 4) return true;
      }
      counter = 1;
      i2 = i + 1; j2 = j - 1;
      while(get_board_at(i2, j2) == player){ // check going SE
        i2++; j2--; counter++;
        if(counter == 4) return true;
      }
    }
  }
  return false;
}

bool place_piece(int player, int col){
  if(col == 0) return false;
  int i = col - 1;
  for(int j = 5; j >= 0; j--){
    if(get_board_at(i, j) != '|') continue;
    else{
      board[i][j] = player;
      return true;
    }
  }
  return false;
}

string print_board(){
  string ret = "\n1 2 3 4 5 6 7\n";
  for(int j = 0; j < 6; j++){
    for(int i = 0; i < 7; i++){
      if(get_board_at(i, j) == RED_PLAYER){
        ret += "\033[1;31m";
        ret += "O";
        ret += "\033[0m";
      }else if(get_board_at(i, j) == BLUE_PLAYER){
        ret += "\033[1;34m";
        ret += "O";
        ret += "\033[0m";
      }else{
        ret += get_board_at(i, j);
      }
      ret += " ";
    }
    ret += "\n";
  }
  return ret;
}

char get_board_at(int x, int y){
  if(x < 0 || y < 0 || x >= 7 || y >= 6) return '|';
  return board[x][y];
}
