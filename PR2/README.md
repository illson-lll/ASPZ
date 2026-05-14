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

Сегмент BSS призначений лише для глобальних та статичних змінних, та він нічого не важить в пам'яті. 
Переносивши дані масиву в всередину функції, вони перестали бути у виконуваному файлі, і bss знову став замість 4032. 

