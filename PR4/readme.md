### Завдання 4.1
Скільки пам’яті може виділити malloc(3) за один виклик?
Параметр malloc(3) є цілим числом типу даних size_t, тому логічно максимальне число, яке можна передати як параметр malloc(3), — це максимальне значення size_t на платформі (sizeof(size_t)). У 64-бітній Linux size_t становить 8 байтів, тобто 8 * 8 = 64 біти. Відповідно, максимальний обсяг пам’яті, який може бути виділений за один виклик malloc(3), дорівнює 2^64. Спробуйте запустити код на x86_64 та x86. Чому теоретично максимальний обсяг складає 8 ексабайт, а не 16?

```C
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h> 

int main(){
    printf("Розмір size_t: %zu байтів %zu бітів\n",sizeof(size_t), sizeof(size_t) * 8);
        
    printf("Максимальний розмір size_t: %zu байтів\n",SIZE_MAX);

    printf("Спроба виділення...\n");

    void *p = malloc(SIZE_MAX);

    if(p== NULL){
        printf("Malloc повернув NULL\n");
    } else{
        printf("Успіх\n");
    }

    return 0;
}
```
```bash
illson@nobara-pc:~/Documents/ASPZ/PR4$ gcc t1.c -o t1_64 && ./t1_64
Розмір size_t: 8 байтів 64 бітів
Максимальний розмір size_t: 18446744073709551615 байтів
Спроба виділення...
Malloc повернув NULL

illson@nobara-pc:~/Documents/ASPZ/PR4$ gcc -m32 t1.c -o t1_32 && ./t1_32
Розмір size_t: 4 байтів 32 бітів
Максимальний розмір size_t: 4294967295 байтів
Спроба виділення...
Malloc повернув NULL
```
64-бітна програма не змогла виділити 16 ексабайтів, так і 32-х бітна не змогла 4 гігабайта. 
Теоретично Malloc може виділити лише половину з максимального віртуального простору через принципи розподілу пам'яті. 
У випадку 64-бітної архітектури 2^64 байтів діляться на половину, де старший біт (0/1) визначає адресу під простір користувача (у випадку 0) чи для простіру ядра (у випадку 1).     
Попри це, сучасні процесори не здатні реалізувати й 8 ексабайтів, тому віртуальна адресація пам'яті апаратно обмежена до 48 біт, 2^48 дорівнює 256 терайбатів, 
отже простору користувача належить лише 128 терабайтів, про що й можна дізнатись використавши  "cat /proc/self/maps".  
```bash
illson@nobara-pc:~$ cat /proc/self/maps
5636746bb000-5636746c0000 r-xp 00000000 00:21 761114                     /usr/bin/cat
5636746c0000-5636746c3000 r--p 00005000 00:21 761114                     /usr/bin/cat
5636746c3000-5636746c4000 r--p 00007000 00:21 761114                     /usr/bin/cat
5636746c4000-5636746c5000 rw-p 00008000 00:21 761114                     /usr/bin/cat
5636b19b0000-5636b19d1000 rw-p 00000000 00:00 0                          [heap]
7f8b1dc00000-7f8b2ba70000 r--p 00000000 00:21 1269958                    /usr/lib/locale/locale-archive
7f8b2bb4c000-7f8b2bb91000 rw-p 00000000 00:00 0 
7f8b2bb91000-7f8b2bd00000 r-xp 00000000 00:21 1269849                    /usr/lib64/libc.so.6
7f8b2bd00000-7f8b2bd76000 r--p 0016f000 00:21 1269849                    /usr/lib64/libc.so.6
7f8b2bd76000-7f8b2bd7a000 r--p 001e4000 00:21 1269849                    /usr/lib64/libc.so.6
7f8b2bd7a000-7f8b2bd7c000 rw-p 001e8000 00:21 1269849                    /usr/lib64/libc.so.6
7f8b2bd7c000-7f8b2bd86000 rw-p 00000000 00:00 0 
7f8b2bda9000-7f8b2bdad000 r--p 00000000 00:00 0                          [vvar]
7f8b2bdad000-7f8b2bdaf000 r--p 00000000 00:00 0                          [vvar_vclock]
7f8b2bdaf000-7f8b2bdb1000 r-xp 00000000 00:00 0                          [vdso]
7f8b2bdb1000-7f8b2bddb000 r-xp 00000000 00:21 1269846                    /usr/lib64/ld-linux-x86-64.so.2
7f8b2bddb000-7f8b2bde7000 r--p 0002a000 00:21 1269846                    /usr/lib64/ld-linux-x86-64.so.2
7f8b2bde7000-7f8b2bde9000 r--p 00035000 00:21 1269846                    /usr/lib64/ld-linux-x86-64.so.2
7f8b2bde9000-7f8b2bdea000 rw-p 00037000 00:21 1269846                    /usr/lib64/ld-linux-x86-64.so.2
7f8b2bdea000-7f8b2bdeb000 rw-p 00000000 00:00 0 
7fffec658000-7fffec67a000 rw-p 00000000 00:00 0                          [stack]
ffffffffff600000-ffffffffff601000 --xp 00000000 00:00 0                  [vsyscall]
```
* Стек є найвищою точкою в пам'яті користувача.
* Згідно з таблицею межа стеку згідно таблиці становить 0x7fffec67a000.
* Перевівши в десяткову систему це становить 140737159602176 байтів що в свою чергу становить 128 терабайт.  

Отже максимальну кількість пам'яті що може виділити маллок, практично лише 128 терабайт що цілком достатьно в реаліях стандартних 64-бітних систем. 

Джерела:
https://www.kernel.org/doc/Documentation/x86/x86_64/mm.txt
https://www.bottomupcs.com/ch06s02.html

### Завдання 4.2
Що станеться, якщо передати malloc(3) від’ємний аргумент? Напишіть тестовий випадок, який обчислює кількість виділених байтів за формулою num = xa * xb. Що буде, якщо num оголошене як цілочисельна змінна зі знаком, а результат множення призведе до переповнення? Як себе поведе malloc(3)? Запустіть програму на x86_64 і x86.
```C
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h> 

int main(){
    printf("Розмір size_t: %zu байтів %zu бітів\n",sizeof(size_t), sizeof(size_t) * 8);
        
    int xa = 5000000;
    int xb = 5000000;

    int num = xa * xb; 

    printf("Значення xa: %d\n", xa);
    printf("Значення xb: %d\n", xb);
    printf("Результат xa * xb (num): %d\n", num);

    printf("size_t num: %zu байтів\n", (size_t)num);

    printf("Спроба виділення пам'яті...\n");
    void *p = malloc(num);

    if (p == NULL) {
        printf("Malloc повернув NULL\n");
    } else {
        printf("Успіх!");
        free(p); 
    }
    return 0;
}
```
```bash
illson@nobara-pc:~/Documents/ASPZ/PR4$ gcc t1.c -o t1_64 && ./t1_64
Розмір size_t: 8 байтів 64 бітів
Значення xa: 5000000
Значення xb: 5000000
Результат xa * xb (num): -1004630016
size_t num: 18446744072704921600 байтів
Спроба виділення пам'яті...
Malloc повернув NULL
```
Через вихід за межі int. Відбулось знакове переповнення. Останній біт котрий відповідав за знак почав містити 1, отже число стало від'ємним, також арифметика почала заново заповнювати біти з початку (справа). 
size_t є безнаковим, отже не може бути від'ємним, тому фізичний розмір num все ще є плюсовим. 
* В unsigned якщо від нуля відняти одиницю, він перетворюється на плюсове та максимальне число

В size_t це число розміром 64 біта, отже при мінусовому значенні, size_t починає важити 16 ексабайт. Тому malloc повернув нуль, бо згідно з висновком минулого завдання максимальний користувацький простір складає значно меньше.

### Завдання 4.3 
Чи є помилки у такому коді?
```C
void *ptr = NULL;
while (<some-condition-is-true>) {
    if (!ptr)
        ptr = malloc(n);
    [... <використання 'ptr'> ...]
    free(ptr);
}
```
Напишіть тестовий випадок, який продемонструє проблему та правильний варіант коду.

На перший ітерації, if(!ptr) спрацьовує, malloc виділяє пам'ять і виконується free(ptr). Free звільюнє пам'ять, однак ptr не стає NULL і все ще зберігає стару недійсну адресу.
```C
#include <stdio.h>
#include <stdlib.h>

int main() {
    int *ptr = NULL;
    int с = 0;

    while (с < 10) {
        с++;
        printf("\nітерація %d:\n", с);
        printf("Значення ptr: %p\n", (void*)ptr);

        if (!ptr) {
            printf("!ptr...\n");
            ptr = (int *)malloc(sizeof(int));
            if (ptr == NULL) return 1;
            *ptr = 67; 
        } else {
            printf("else\n");
        }

        printf("Значення ptr = %d\n", *ptr);

        printf("free(ptr)...\n");
        free(ptr);
    }

    return 0;
}
```
```bash
illson@nobara-pc:~/Documents/ASPZ/PR4$ gcc t1.c -o t1_64 && ./t1_64

ітерація 1:
Значення ptr: (nil)
!ptr...
Значення ptr = 67
free(ptr)...

ітерація 2:
Значення ptr: 0x22569720
else
Значення ptr = 140649
free(ptr)...
free(): double free detected in tcache 2
Aborted                    (core dumped) ./t1_64
```
Для виправлення помилки достатньо в кінці циклу прописати ptr = NULL;
```bash
illson@nobara-pc:~/Documents/ASPZ/PR4$ gcc t1.c -o t1_64 && ./t1_64

ітерація 1:
Значення ptr: (nil)
!ptr...
Значення ptr = 67
free(ptr)...

ітерація 2:
Значення ptr: (nil)
!ptr...
Значення ptr = 67
free(ptr)...

ітерація 3:
Значення ptr: (nil)
!ptr...
Значення ptr = 67
free(ptr)...

...
```
### Завдання 4.4
Що станеться, якщо realloc(3) не зможе виділити пам’ять? Напишіть тестовий випадок, що демонструє цей сценарій.
```C
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main() {
    size_t initial_size = 5 * sizeof(int);
    int *ptr = (int *)malloc(initial_size);

    if (ptr == NULL) {
        return 1;
    }

    for (int i = 0; i < 5; i++) {
        ptr[i] = (i + 1) * 10;
    }

    printf("Перший елемент: %d\n", ptr[0]);

    size_t impossible_size = (size_t)1024 * 1024 * 1024 * 1024 * 1024; 
    printf("realloc на %zu байт...\n", impossible_size);

    int *tmp = (int *)realloc(ptr, impossible_size);

    if (tmp == NULL) {
        printf("\nrealloc повернув NULL.\n");
        
        printf("ptr[0]: %d\n", ptr[0]);
        printf("ptr[4]: %d\n", ptr[4]);

        free(ptr);
        ptr = NULL;
    } else {
        ptr = tmp;
        printf("Успіх!\n");
        free(ptr);
    }

    return 0;
}
```
```BASH
illson@nobara-pc:~/Documents/ASPZ/PR4$ gcc t1.c -o t1_64 && ./t1_64
Перший елемент: 10
realloc на 1125899906842624 байт...

realloc повернув NULL.
ptr[0]: 10
ptr[4]: 50
```
через використання тимчасового поінтера tmp, хоч і realloc повернув NULL основний ptr не постраждав і пам'ять зберіглась. 
## Завдання 4.5 
Якщо realloc(3) викликати з NULL або розміром 0, що станеться? Напишіть тестовий випадок.
```C
#include <stdio.h>
#include <stdlib.h>

int main() {
    int *ptr = (int *)realloc(NULL, sizeof(int));
    printf("realloc виділив адресу: %p\n", (void*)ptr);

    void *tmp = realloc(ptr, 0);
    printf("realloc(ptr, 0): %p\n", tmp);

    return 0;
}
```
```BASH
Стару пам'ять успішно звільнено вручну.
illson@nobara-pc:~/Documents/ASPZ/PR4$ gcc t1.c -o t1_64 && ./t1_64
realloc виділив адресу: 0xcb00310
realloc(ptr, 0): (nil)
```
В першому виклику realloc, він спрацював так само як malloc виділивши пам'ять незважаючи на відстутність даних. 
В другому він звільнив пам'ять спрацювавши так само як free і зніс поінтер.

## Завдання 4.6
```C
#include <stdio.h>
#include <stdlib.h>

struct sbar {
    int id;
    char data[64];
};

int main() {
    struct sbar *ptr, *newptr;

    ptr = calloc(1000, sizeof(struct sbar));
    if (ptr == NULL) return 1;

    newptr = reallocarray(ptr, 500, sizeof(struct sbar));
    
    if (newptr == NULL) {
        free(ptr); 
        return 1;
    }
    ptr = newptr;

    free(ptr);
    return 0;
}
```
```BASH
illson@nobara-pc:~/Documents/ASPZ/PR4$ ltrace ./t1_old
calloc(1000, 68)                            = 0x34c08010
realloc(0x34c08010, 34000)                  = 0x34c08010
+++ exited (status 0) +++
illson@nobara-pc:~/Documents/ASPZ/PR4$ ltrace ./t1_new
calloc(1000, 68)                            = 0x97fa010
reallocarray(0x97fa010, 500, 68, 0x97fa010) = 0x97fa010
free(0x97fa010)                             = <void>
+++ exited (status 0) +++
```
ltrace демонструє що reallocarray(3) приймає розмір та кількість елементів як два окремих параметри, що дозволяє безпечно перевіряти переповнення на рівні бібліотеки glibc на відміну від стандартного realloc(3).
