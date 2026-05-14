### Завдання 2.1
Напишіть програму для визначення моменту, коли time_t
закінчиться.Дослідіть, які зміни відбуваються в залежності від 32- та
64-бітної архітектури. Дослідіть сегменти виконуваного файлу.
```C
#include <stdio.h>
#include <time.h>
#include <limits.h>

int main() {
    printf("Розмір time_t: %zu біт або %zu байт\n", sizeof(time_t) * 8, sizeof(time_t));
    time_t max_time;
    if (sizeof(time_t) == 4) {
        max_time = (time_t)INT_MAX;
    } else {
        max_time = (time_t)LLONG_MAX;
    }
    printf("Максимальний час: %lld\n", (long long)max_time);
    printf("Дата закінчення: %s", ctime(&max_time));
    max_time += 1;
    printf("Після переповнення: %s", ctime(&max_time));
    return 0;
}
```
Використовуючи стандартний gcc компілятор програма виводить:
```bash
illson@nobara-pc:~/Documents/ASPZ/PR2$ gcc -Wall main.c & ./a.out
[1] 17977
Розмір time_t: 64 біт або 8 байт
Максимальний час: 9223372036854775807
Дата закінчення: (null)Після переповнення: (null)
```
А з 32 бітною бібліотекою glibc-devel.i686:
```bash
illson@nobara-pc:~/Documents/ASPZ/PR2$ gcc -m32 main.c && ./a.out
Розмір time_t: 32 біт або 4 байт
Максимальний час: 2147483647
Дата закінчення: Tue Jan 19 05:14:07 2038
Після переповнення: Fri Dec 13 22:47:56 1901
illson@nobara-pc:~/Documents/ASPZ/PR2$ 
```
Отже, 64-бітний розмір в time_t в двічі більший, тому в 32-біт виникає проблема 2038, де закінчується ліміт time_t та всі годинники котрі працюють на 32 бітній архітектури переведуться на 1901 рік.

### Завдання 2.2 
1. Скомпілюйте програму "hello world", запустіть ls -l для
виконуваного файлу, щоб отримати його загальний розмір, і
запустіть size, щоб отримати розміри сегментів всередині нього.
```bash
illson@nobara-pc:~/Documents/ASPZ/PR2$ ls -l a.out
-rwxr-xr-x 1 illson illson 12520 тра 14 22:16 a.out
illson@nobara-pc:~/Documents/ASPZ/PR2$ size a.out
   text    data     bss     dec     hex filename
   1021     532       4    1557     615 a.out
```
2. Додайте оголошення глобального масиву із 1000 int,
перекомпілюйте й повторіть вимірювання. Зверніть увагу на
відмінності.
```bash
illson@nobara-pc:~/Documents/ASPZ/PR2$ gcc -Wall main.c  
\main.c: In function ‘main’:
main.c:4:9: warning: unused variable ‘a’ [-Wunused-variable]
    4 |     int a[1000];
      |         ^
illson@nobara-pc:~/Documents/ASPZ/PR2$ ls -l a.out
-rwxr-xr-x 1 illson illson 12336 тра 14 22:21 a.out
illson@nobara-pc:~/Documents/ASPZ/PR2$ size a.out
   text    data     bss     dec     hex filename
    841     460       4    1305     519 a.out
```
3. Тепер додайте початкове значення в оголошення масиву
(пам’ятайте, що C не змушує вас вказувати значення для кожного
елемента масиву в ініціалізаторі). Це перемістить масив із сегмента
BSS у сегмент даних. Повторіть вимірювання. Зверніть увагу на
різницю.

```bash
illson@nobara-pc:~/Documents/ASPZ/PR2$ ls -l a.out
-rwxr-xr-x 1 illson illson 12544 тра 14 22:19 a.out
illson@nobara-pc:~/Documents/ASPZ/PR2$ size a.out
   text    data     bss     dec     hex filename
   1031     532    4032    5595    15db a.out
```
4. Тепер додайте оголошення великого масиву в локальну функцію.
Оголосіть другий великий локальний масив з ініціалізатором.
Повторіть вимірювання. Дані розташовуються всередині функцій,
залишаючись у виконуваному файлі? Яка різниця, якщо масив
ініціалізований чи ні?
```bash
illson@nobara-pc:~/Documents/ASPZ/PR2$ ls -l a.out
-rwxr-xr-x 1 illson illson 12560 тра 14 22:24 a.out
illson@nobara-pc:~/Documents/ASPZ/PR2$ size a.out
   text    data     bss     dec     hex filename
   1137     532       4    1673     689 a.out
```
5. Які зміни відбуваються з розмірами файлів і сегментів, якщо ви
компілюєте для налагодження? Для максимальної оптимізації?

Сегмент BSS призначений лише для глобальних та статичних змінних, та він нічого не важить в пам'яті. Саме тому варто не заповнювати дані нулями без необхідності. 
Переносивши дані масиву в всередину функції, вони перестали бути у виконуваному файлі, і значення bss знову стало 4 замість 4032. 

### Завдання 2.3
Скомпілюйте й запустіть тестову програму, щоб визначити приблизне
розташування стека у вашій системі:
```С
    #include "stdio.h"
    int main() {
    int i;
    printf("The stack top is near %p\n", &i);
    return 0;
}
```
Знайдіть розташування сегментів даних і тексту, а також купи всередині
сегмента даних, оголосіть змінні, які будуть поміщені в ці сегменти, і
виведіть їхні адреси.

Збільшіть розмір стека, викликавши функцію й оголосивши кілька
великих локальних масивів. Яка зараз адреса вершини стека?

```bash
illson@nobara-pc:~/Documents/ASPZ/PR2$ ./a.out
Text near: 0x4011f0
Data near: 0x403028
BSS near: 0x403030
Heap near:     0x118eb310
The stack top is near    0x7fff53a986a4
Stack after array near:  0x7fff53a9869c
```
Тексту є найнижча адреса у вашому списку; Знаходиться трохи вище за текст; BSS іде після даних; Та heap розташована найвище від статичних данних;
Початкова вершина стека 0x7fff53a986a4
Адреса після масиву: 0x7fff53a9869c
Отже стек росте вниз.


### Завдання 2.4
Порівняння GDB та gstack 
```C
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>

#define MSG "\nIn function %20s; &localvar = %p\n"

static void bar_is_now_closed(void) {
    int localvar = 5;
    printf(MSG, __FUNCTION__, &localvar);
    printf("\n Now blocking on pause()...\n");
    pause();
}

static void bar(void) {
    int localvar = 5;
    printf(MSG, __FUNCTION__, &localvar);
    bar_is_now_closed();
}

static void foo(void) {
    int localvar = 5;
    printf(MSG, __FUNCTION__, &localvar);
    bar();
}

int main(int argc, char **argv) {
    int localvar = 5;
    printf(MSG, __FUNCTION__, &localvar);
    
    foo();
    
    return EXIT_SUCCESS;
}
```
Вивід:
```bash
illson@nobara-pc:~/Documents/ASPZ/PR2$ ./a.out

In function                 main; &localvar = 0x7ffd90dfe4bc

In function                  foo; &localvar = 0x7ffd90dfe48c

In function                  bar; &localvar = 0x7ffd90dfe46c

In function    bar_is_now_closed; &localvar = 0x7ffd90dfe44c

 Now blocking on pause()...
```
Стек іде в низ.
```bash
illson@nobara-pc:~/Documents/ASPZ/PR2$ gstack 25253
Thread 1 (Thread 0x7fa5c6dd4740 (LWP 25253) "a.out"):
#0  0x00007fa5c6e45bbe in __internal_syscall_cancel () from /lib64/libc.so.6
#1  0x00007fa5c6e45be4 in __syscall_cancel () from /lib64/libc.so.6
#2  0x00007fa5c6ea2421 in pause () from /lib64/libc.so.6
#3  0x00000000004004bf in bar_is_now_closed ()
#4  0x00000000004004f1 in bar ()
#5  0x0000000000400523 in foo ()
#6  0x000000000040055c in main ()
illson@nobara-pc:~/Documents/ASPZ/PR2$ sudo gdb -q -p 25253
Attaching to process 25253
Reading symbols from /home/illson/Documents/ASPZ/PR2/a.out...
(No debugging symbols found in /home/illson/Documents/ASPZ/PR2/a.out)
Reading symbols from /lib64/libc.so.6...
(No debugging symbols found in /lib64/libc.so.6)
Reading symbols from /lib64/ld-linux-x86-64.so.2...
[Thread debugging using libthread_db enabled]
Using host libthread_db library "/lib64/libthread_db.so.1".
0x00007fa5c6e45bbe in __internal_syscall_cancel () from /lib64/libc.so.6
Missing rpms, try: dnf --enablerepo='*debug*' install glibc-debuginfo-2.42-12.fc43.x86_64
(gdb) bt
#0  0x00007fa5c6e45bbe in __internal_syscall_cancel () from /lib64/libc.so.6
#1  0x00007fa5c6e45be4 in __syscall_cancel () from /lib64/libc.so.6
#2  0x00007fa5c6ea2421 in pause () from /lib64/libc.so.6
#3  0x00000000004004bf in bar_is_now_closed ()
#4  0x00000000004004f1 in bar ()
#5  0x0000000000400523 in foo ()
#6  0x000000000040055c in main ()
(gdb)
```
Аналіз від gstack та gdb показав ідентичний ланцюжок викликів: програма заблокована на системному виклику pause(), який був викликаний послідовно через main -> foo -> bar -> bar_is_now_closed. Це демонструє структуру стека, де кожен виклик створює новий кадр, причому main залишається в основі стека, а поточна виконувана функція знаходиться на його вершині.
### Завдання 2.5
Відомо, що при виклику процедур і поверненні з них процесор
використовує стек.Чи можна в такій схемі обійтися без лічильника команд
(IP), використовуючи замість нього вершину стека? Обґрунтуйте свою
відповідь та наведіть приклади.

Відповідь:
Використання стека замість IP є неефективним, оскільки стек не пристосований для нелінійного керування програмою. У такій моделі реалізація циклів вимагала б безкінечного дублювання коду, бо виконані інструкції просто зникали б зі стека. Крім того, відсутність механізму довільного доступу, який є у IP, унеможливлює швидкі переходи та складні умовні розгалуження, обмежуючи логіку програми лише строгою послідовністю дій.
