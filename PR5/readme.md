### Завдання 5.2
Створити приклад, де переповнення цілочисельного розміру призводить до виділення занадто малого блоку і подальшого тихого пошкодження heap-метаданих.
```C
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
        printf("Заповнення адреси %p\n", (void*)&   element[i]);
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
```
Число С = 67108865. Це число яке утворилось з 32-бітного розміру (int) поділене на 64 та з "додаванням лишньої" одинички.
* ( 32^2 / 64 ) + 1;

Розмір структури el = рівно 64 байти. Отже підраховуючи total_size, 64 на трохи надлишкове числ, переповнючи ліміт unsigned int на 65 одиниць. 
Таким чином total_size стає дуже маленьким через перезаписування перших бітів.
'''console
illson@fedora:~/Документи/ASPZ/PZ5$ gcc -Wall t.c && ./a.out 
t.c: У функції «overflow»:
t.c:27:11: попередження: Невикористана змінна «a» [-Wunused-variable]
   27 |     void *a = malloc(5);
      |           ^
Введене count: 67108865
total_size: 64 байт
Заповнення адреси 0x3e582720
Заповнення адреси 0x3e582760
Заповнення адреси 0x3e5827a0
Заповнення адреси 0x3e5827e0
Заповнення адреси 0x3e582820
malloc(): corrupted top size
Перервано         (збережено знімок оперативної пам’яті)./a.out
```
