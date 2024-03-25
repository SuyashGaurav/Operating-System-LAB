#include<stdlib.h>
#include<stdio.h>
#include <unistd.h>

int main(int argv, char* argc[]){
    if(argv < 2) printf("Enter correct arguments\n"), exit(1);
    long long int n = atol(argc[argv-1]);
    n = n*n;
    printf("Square: Current process id: %d, Current result: %lld\n", getpid(), n);

    char* arg_list[argv];
    arg_list[argv-1] = NULL;
    char str_n[20];
    sprintf(str_n, "%lld", n);
    arg_list[argv-2] = str_n;
    for(int i=1; i<argv-1; i++){
        arg_list[i-1] = argc[i];
    }
    execvp(arg_list[0], arg_list);
    return 0;
}