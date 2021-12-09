#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char *argv[]) {

    if(argc != 2) {
        printf("ERROR: Expected 1 argument\n");
        return 0;
    }

    char *str = argv[1];
    int a = 11/6;


    printf("%d \n", a);
    return 0;

}
