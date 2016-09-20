// Ben Harris
// firstchar.cc asks for a string/word, reverses it, then forks
// child process execls ./lastchar with the reversed string/word
// parent process waits for ./lastchar and displays the return byte (which contains the last char of the string)

#include <iostream>
#include <cstdlib>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
using namespace std;

int main(int argc, char **argv){
  cout << "Enter a word: ";

  char word[256];
  cin.getline(word, 256);
  unsigned int input_length = strlen(word);
  char out[input_length + 1];

  unsigned int i, j;
  for(i = 0, j = input_length - 1; i < input_length; i++, j--) out[i] = word[j];
  out[input_length] = '\0';

  pid_t pid = fork();
  if(pid == 0){ // child
    execl("./lastchar", "lastchar", out, NULL);
  } else { // parent
    int status;
    wait(&status);
    cout << "First char: " << (char) WEXITSTATUS(status) << endl;
  }

  return 0;
}
