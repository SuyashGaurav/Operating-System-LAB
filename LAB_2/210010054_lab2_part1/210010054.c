#include<stdio.h>
#include <stdlib.h>
#include<unistd.h>
#include <sys/wait.h>
int main() {
	char* hello = "Hello World";
	for(int i=0; i<11; i++){
		fork() > 0 ? wait(NULL), exit(0) : printf("%c %d\n", hello[i], getpid()), sleep(rand()%4+1);
	}
}