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
