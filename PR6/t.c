#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char data[64];
} el;

void overflow(unsigned int count){
    unsigned int total_size = count * sizeof(el);

    printf("Введене count: %d\n",count);
    printf("total_size: %u байт\n", total_size);

    el *element = (el*) malloc(total_size);

    if(element == NULL){
        printf("null");
        return;
    }

    for(unsigned int i = 0; i<5;i++){
        printf("Заповнення адреси %p\n", (void*)&element[i]);
        memset(&element[i],'A',sizeof(el));
    }

    void *a = malloc(5);

    free(element);
}

int main(){
    unsigned int c = 67108865;
    overflow(c);
    return 0;
}
