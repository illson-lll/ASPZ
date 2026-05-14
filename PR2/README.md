### Завдання 2.1
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
