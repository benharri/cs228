// Ben Harris

// This program runs from the command line.
// usage:
// ./pg3 email@nmu.edu test.mp3

// The file will be sent to the specified email
// as an attachment using mail.nmu.edu

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
#include "pg3.h"
using namespace std;

char b64ch(char ch){
  static const char * b64 = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
  if(ch > 63) return '=';
  return b64[(int)ch];
}

int main(int argc, char **argv){
  if (argc != 3){
    cout << "Usage:" << endl << "\t./pg3 email mp3path" << endl << endl;
    exit(1);
  }
  cout << "email to: " << argv[1] << endl;
  cout << "file: " << argv[2] << endl;

  struct hostent *h;
  sethostent(1);
  h = gethostbyname("semail.nmu.edu");
  endhostent();

  int s = socket(AF_INET, SOCK_STREAM, 0);
  sockaddr_in sin;
  sin.sin_family = AF_INET;
  sin.sin_port = htons(25);
  sin.sin_addr = (*(in_addr *) h->h_addr_list[0]);
  int c = connect(s, (sockaddr *)&sin, sizeof(sockaddr_in));
  if(c < 0) {perror("connect"); exit(1);}

  fdostream out_sock (s);
  fdistream in_sock (s);

  string line;
  getline(in_sock, line);

  out_sock << "HELO semail.nmu.edu" << endl;
  getline(in_sock, line);
  out_sock << "MAIL FROM: apoe@nmu.edu" << endl;
  getline(in_sock, line);
  out_sock << "RCPT TO: " << argv[1] << endl;
  getline(in_sock, line);
  out_sock << "DATA" << endl;
  getline(in_sock, line);

  out_sock << "From: " << argv[1] << endl;
  out_sock << "To: " << argv[1] << endl;
  out_sock << "Subject: PG3 mail" << endl;

  out_sock << "MIME-Version: 1.0" << endl;
  out_sock << "Content-Type: multipart/mixed; boundary=\"DRUMPFWALL\"" << endl;
  out_sock << "--DRUMPFWALL" << endl;
  out_sock << "Content-Type: text/plain" << endl;

  out_sock << endl << "Happy birthday! Here's an mp3!" << endl;

  out_sock << "--DRUMPFWALL" << endl;
  out_sock << "Content-Disposition: attachment; filename="<< argv[2] << endl;
  out_sock << "Content-Type: audio/mpeg3" << endl;
  out_sock << "Content-Transfer-Encoding: base64" << endl;

  ifstream mp3_in (argv[2], ios::binary);
  streampos filesize = mp3_in.tellg();
  mp3_in.seekg(0, ios::end);
  unsigned int fsz = mp3_in.tellg() - filesize;
  mp3_in.seekg(0, ios::beg);

  unsigned char b3[3];
  int i = 0, j = 0;
  string b64;

  while(fsz--){
    // read three bytes
    b3[i++] = mp3_in.get();
    // bitshift the chars
    if(i == 3){
      //                 11111100 >> 2 => 00111111
      b64 += b64ch((b3[0] & 0xFC) >> 2);
      //       00000011 << 4 => 00110000 +   11110000 >> 4 => 00001111
      b64 += b64ch(((b3[0] & 0x03) << 4) + ((b3[1] & 0xF0) >> 4));
      //       00001111 << 2 => 00111100 +   11000000 >> 6 => 00000011
      b64 += b64ch(((b3[1] & 0x0F) << 2) + ((b3[2] & 0xC0) >> 6));
      //                   00111111
      b64 += b64ch(b3[2] & 0x3F);

      i = 0;
    }
  }

  if(i){ // if there are remaining bytes (i is set to 0 when %3 = 0)
    for(j = i; j < 3; j++) b3[j] = '\0';
    //                         11111100 >> 2 => 00111111
    b64 += b64ch((b3[0] & 0xFC) >> 2);
    //                         00000011 << 4 => 00110000 +   11110000 >> 4 => 00001111
    b64 += b64ch(((b3[0] & 0x03) << 4) + ((b3[1] & 0xF0) >> 4));
    //                         00001111 << 2 => 00111100 +   11000000 >> 6 => 00000011
    b64 += b64ch(((b3[1] & 0x0F) << 2) + ((b3[2] & 0xC0) >> 6));
    //                         00111111
    b64 += b64ch(b3[2] & 0x3F);

    while(i++ < 3) b64 += b64ch(64); // pad with =
  }

  for(unsigned int q = 0; q < b64.length(); q++){
    out_sock << b64[q];
    // insert endl every 80 chars
    if(q % 80 == 79) out_sock << endl;
  }

  out_sock << endl << "--DRUMPFWALL--" << endl;
  out_sock << "." << endl;
  getline(in_sock, line);

  out_sock.close();
  cout << "message sent" << endl;

  return 0;
}
