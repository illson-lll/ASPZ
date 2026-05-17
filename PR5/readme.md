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
```
Число С = 67108865. Це число яке утворилось з 32-бітного розміру (int) поділене на 64 та з "додаванням лишньої" одинички.
* ( 32^2 / 64 ) + 1;

Розмір структури el = рівно 64 байти. Отже підраховуючи total_size, 64 на трохи надлишкове числ, переповнючи ліміт unsigned int на 65 одиниць. 
Таким чином total_size стає дуже маленьким через перезаписування перших бітів, а конкретно 64 байти.
```console
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
В циклі заповнення видно що між 5 адресами різниця рівно 0x040 (64 байти). 
Однак починаючи з 2 індекса іде переповнення, memset хоч і виставляє пам'ять він не дивиться на розміри виділення. 
При спробі виділити нову пам'ять через malloc для змінної a, виникає помилка corrupted top size. 

valgrin:
```console
==23491== Memcheck, a memory error detector
==23491== Copyright (C) 2002-2026, and GNU GPL'd, by Julian Seward et al.
==23491== Using Valgrind-3.27.0 and LibVEX; rerun with -h for copyright info
==23491== Command: ./a.out
==23491== 
Введене count: 67108865
total_size: 64 байт
Заповнення адреси 0x4a71480
Заповнення адреси 0x4a714c0
==23491== Invalid write of size 8
==23491==    at 0x484F1C4: memset (vg_replace_strmem.c:1399)
==23491==    by 0x400547: overflow (in /home/illson/Документи/ASPZ/PZ5/a.out)
==23491==    by 0x400586: main (in /home/illson/Документи/ASPZ/PZ5/a.out)
==23491==  Address 0x4a714c0 is 0 bytes after a block of size 64 alloc'd
==23491==    at 0x4840AE6: malloc (vg_replace_malloc.c:447)
==23491==    by 0x4004DC: overflow (in /home/illson/Документи/ASPZ/PZ5/a.out)
==23491==    by 0x400586: main (in /home/illson/Документи/ASPZ/PZ5/a.out)
==23491== 
==23491== Invalid write of size 8
==23491==    at 0x484F1CB: memset (vg_replace_strmem.c:1399)
==23491==    by 0x400547: overflow (in /home/illson/Документи/ASPZ/PZ5/a.out)
==23491==    by 0x400586: main (in /home/illson/Документи/ASPZ/PZ5/a.out)
==23491==  Address 0x4a714d0 is 16 bytes after a block of size 64 alloc'd
==23491==    at 0x4840AE6: malloc (vg_replace_malloc.c:447)
==23491==    by 0x4004DC: overflow (in /home/illson/Документи/ASPZ/PZ5/a.out)
==23491==    by 0x400586: main (in /home/illson/Документи/ASPZ/PZ5/a.out)
==23491== 
Заповнення адреси 0x4a71500
Заповнення адреси 0x4a71540
Заповнення адреси 0x4a71580
--23491-- VALGRIND INTERNAL ERROR: Valgrind received a signal 11 (SIGSEGV) - exiting
--23491-- si_code=128;  Faulting address: 0x0;  sp: 0x1002ca5d00

valgrind: the 'impossible' happened:
   Killed by fatal signal
```
Valgrind показав помилку SIGSEGV, що свідчить про неконтрольований запис в heap.
