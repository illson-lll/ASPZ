### Практична 3
Для роботи з середовищом, я забуілдив власний dockerfile з GCC та NANO для зручності
```
FROM ubuntu:latest

RUN apt-get update && apt-get install -y \
    gcc \
    nano \
    libc6-dev \
    && rm -rf /var/lib/apt/lists/*
WORKDIR /PR3
```
```bash
illson@nobara-pc:~/Documents/ASPZ/PR3$ docker build -t pr3 .
[+] Building 14.1s (7/7) FINISHED                                                                    docker:default
 => [internal] load build definition from dockerfile                                                           0.0s
 => => transferring dockerfile: 188B                                                                           0.0s
 => [internal] load metadata for docker.io/library/ubuntu:latest                                               0.0s
 => [internal] load .dockerignore                                                                              0.0s
 => => transferring context: 2B                                                                                0.0s
 => CACHED [1/3] FROM docker.io/library/ubuntu:latest@sha256:f3d28607ddd78734bb7f71f117f3c6706c666b8b76cbff7c  0.0s
 => => resolve docker.io/library/ubuntu:latest@sha256:f3d28607ddd78734bb7f71f117f3c6706c666b8b76cbff7c9ff6e57  0.0s
 => [2/3] RUN apt-get update && apt-get install -y     gcc     nano     libc6-dev     && rm -rf /var/lib/apt/  6.8s
 => [3/3] WORKDIR /PR3                                                                                         0.1s 
 => exporting to image                                                                                         7.1s 
 => => exporting layers                                                                                        5.7s 
 => => exporting manifest sha256:3c69b8b514391362699e9a3cad357fe394520c8e6102be6ecae004b7425fb644              0.0s 
 => => exporting config sha256:026f23b2bb1243a89228f25ee60bab27b6b2bbee9e9f29d3914c0cea9d780bd9                0.0s 
 => => exporting attestation manifest sha256:cc035e56356ae8012be31eb057f06c0ec70f685bed6d42ea780526f091b6e60f  0.0s 
 => => exporting manifest list sha256:3ab69536d4f77598e19576c29765d586ee256aabe099ce8a9aab9e6df8165bb1         0.0s
 => => naming to docker.io/library/pr3:latest                                                                  0.0s
 => => unpacking to docker.io/library/pr3:latest  
```

### Завдання 3.1
Запустіть Docker-контейнер і поекспериментуйте з максимальним лімітом ресурсів відкритих файлів. Для цього виконайте команди у вказаному порядку:
```Bash
illson@nobara-pc:~/Documents/ASPZ/PR3$ docker run -it --rm ubuntu bash
Unable to find image 'ubuntu:latest' locally
latest: Pulling from library/ubuntu
1c24335ddd46: Pull complete 
6f5c5aa4e145: Pull complete 
9bcf140d7f0f: Download complete 
Digest: sha256:f3d28607ddd78734bb7f71f117f3c6706c666b8b76cbff7c9ff6e5718d46ff64
Status: Downloaded newer image for ubuntu:latest
root@dbef9e55f133:/#  ulimit -n
1024
root@dbef9e55f133:/#  ulimit -aS | grep "open files"
open files                          (-n) 1024
root@dbef9e55f133:/#  ulimit -aH | grep "open files"
open files                          (-n) 1048576
root@dbef9e55f133:/# ulimit -n 3000
root@dbef9e55f133:/#  ulimit -aS | grep "open files"
open files                          (-n) 3000
root@dbef9e55f133:/#  ulimit -aH | grep "open files"
open files                          (-n) 3000
root@dbef9e55f133:/# ulimit -n 3001
bash: ulimit: open files: cannot modify limit: Operation not permitted
root@dbef9e55f133:/#  ulimit -n 2000
root@dbef9e55f133:/# ulimit -n
2000
root@dbef9e55f133:/#  ulimit -aS | grep "open files"
open files                          (-n) 2000
root@dbef9e55f133:/#  ulimit -aH | grep "open files"
open files                          (-n) 2000
root@dbef9e55f133:/# ulimit -n 3000
bash: ulimit: open files: cannot modify limit: Operation not permitted
root@dbef9e55f133:/# 
```
Софт ліміт не може перевищувати  хард ліміт.

### Завдання 3.2
У Docker-контейнері встановіть утиліту perf(1). Поекспериментуйте з досягненням процесом встановленого ліміту.
```bash
root@dbef9e55f133:/# exec 3>/tmp/file3; exec 4>/tmp/file4; exec 5>/tmp/file5; exec 6>/tmp/file6; exec 7>/tmp/file7; exec 8>/tmp/file8; exec 9>/tmp/file9
root@dbef9e55f133:/# ulimit -n 10
root@dbef9e55f133:/# exec 10>/tmp/file10
bash: /tmp/file10: Too many open files
root@dbef9e55f133:/# perf list
bash: start_pipeline: pgrp pipe: Too many open files
perf: error while loading shared libraries: libm.so.6: cannot open shared object file: Error 24
root@dbef9e55f133:/# 
```
При спробі відкрити 10-й файл bash заблокував операцію, perf list призвела до повного краху програми, та виникли помилки з завантаженням бібліотек.
Занадто низьке значення ліміту робить роботу контейнера неможливою, оскільки блокує базові системні виклики

### Завдання 3.3
Напишіть програму, що імітує кидання шестигранного кубика. Імітуйте кидки, результати записуйте у файл, для якого попередньо встановлено обмеження на його максимальний розмір (max file size). Коректно обробіть ситуацію перевищення ліміту.

```C
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <errno.h>
#include <string.h>

int main() {
    FILE *f = fopen("output.txt", "w");
    if (f == NULL) {
        return 1;
    }
    srand(time(NULL));
    int count = 0;

    while (1) {
        int dice = (rand() % 6) + 1;

        int check = fprintf(f, "%d: %d\n", count, dice);

        if (check < 0) {
            printf("Write error %s",strerror(errno));
            break;
        }
        if (fflush(f) != 0) {
            printf("\nFlush error\n");
            break;
        }
        count++;
    }

    fclose(f);
    return 0;
}
```
Вивід:
```bash
illson@nobara-pc:~/Documents/ASPZ/PR3$ docker run -it --rm --ulimit fsize=1000:1000 pr3
root@a905f5647c57:/PR3# touch dice.c
root@a905f5647c57:/PR3# nano dice
root@e6279fe86d31:/PR3# gcc -Wall dice.c    
root@e6279fe86d31:/PR3# ulimit -f 10
root@e6279fe86d31:/PR3# ./a.out 
File size limit exceeded   (core dumped) ./a.out
root@e6279fe86d31:/PR3# ls -l output.txt 
-rw-r--r-- 1 root root 10240 May 14 22:31 output.txt
root@e6279fe86d31:/PR3#
```
### Завдання 3.4
Напишіть програму, що імітує лотерею, вибираючи 7 різних цілих чисел у діапазоні від 1 до 49 і ще 6 з 36. Встановіть обмеження на час ЦП (max CPU time) і генеруйте результати вибору чисел (7 із 49, 6 із 36).
```C
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <signal.h>
#include <unistd.h>

void handle_sigxcpu(int sig) {
    fprintf(stderr,"Killed\n"), sig);
    exit(0);
}

void print_lottery(int count, int max_val) {
    printf("%d (1-%d): ", count, max_val);
    for (int i = 0; i < count; i++) {
        printf("%d ", (rand() % max_val) + 1);
    }
    printf("\n");
}

int main() {
    signal(SIGXCPU, handle_sigxcpu);
    srand(time(NULL));
    unsigned long iteration = 0;

    while (1) {
        iteration++;
        print_lottery(7, 49);
        print_lottery(6, 36);
        printf("Success: %lu.\n", iteration);
    }
    return 0;
}
```
Вивід:
```bash
Success: 156978.
7 (1-49): 28 13 41 37 42 14 11 
6 (1-36): 19 11 22 18 28 13 
Success: 156979.
7 (1-49): 16 4 26 29 37 43 41 
6 (1-36): 7 6 20 25 25 33 
Success: 156980.
7 (1-49): 2 34 17 48 27 49 16 
6 (1-36): 21 10 15 35 25 13 
Success: 156981.
7 (1-49): 17 10 9 47 24 24 1 
6 (1-36): 21 18 32 11 30 18 
Success: 156982.
7 (1-49): 47 16 43 5 21 1 38 
6 (1-36): 35 32 14 5 8 14 
Success: 156983.
7 (1-49): 34 42 28 36 17 44 1 
6 (1-36): 8 31 29 5 35 14 
Success: 156984.
7 (1-49): 13 39 27 16 47 24 37 
6 (1-36): 30 2 13 1 1 12 
Success: 156985.
7 (1-49): 10 36 37 33 29 26 26 
6 (1-36): 35 10 14 15 16 21 
Success: 156986.
7 (1-49): 3 41 33 5 20 1 48 
Killed
```
Використовуючи ulimit -t 1, програма встигла провести 156986 ітерацій. 

### Завдання 3.5
Напишіть програму для копіювання одного іменованого файлу в інший. Імена файлів передаються у вигляді аргументів.
Програма має:
перевіряти, чи передано два аргументи, інакше виводити "Program need two arguments";
перевіряти доступність першого файлу для читання, інакше виводити "Cannot open file .... for reading";
перевіряти доступність другого файлу для запису, інакше виводити "Cannot open file .... for writing";
обробляти ситуацію перевищення обмеження на розмір файлу.

```C
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

#define BUFFER_SIZE 8192

int main(int argc, char *argv[]){
    if(argc !=3){
        fprintf(stderr, "Program need two arguments\n");
        return 1;
    }

    char *src_path = argv[1];
    char *dest_path = argv[2];

    FILE *src_f, *dest_f;
    src_f = fopen(src_path,"rb");
    if(src_f==NULL){
        printf("Cannot open file %s for reading.\n",src_path);
        return 1;
    }

    dest_f = fopen(argv[2], "wb");
    if (dest_f == NULL) {
        printf("Cannot open file %s for writing.\n", argv[2]);
        fclose(dest_f);
        return 1;
    }

    char buffer[BUFFER_SIZE];
    ssize_t bytes_read, bytes_written;

    while ((bytes_read = fread(buffer, 1, BUFFER_SIZE, src_f)) > 0) {
        bytes_written = fwrite(buffer, 1, bytes_read, dest_f);
        if (bytes_written < bytes_read) {
            if (errno == EFBIG) {
                fprintf(stderr, "Error: File size limit exceeded\n");
            } else {
                fprintf(stderr, "Error during writing to %s\n", dest_path);
            }
            fclose(src_f);
            fclose(dest_f);
            return 1;
        }
    }
    fclose(src_f);
    fclose(dest_f);
    return 0;
}
```
Вивід
```bash
root@ec81070f7518:/PR3# ./fcopy
Program need two arguments
root@ec81070f7518:/PR3# echo "Hello" > input.txt
root@ec81070f7518:/PR3# ./fcopy input.txt output.txt
root@ec81070f7518:/PR3# cat output.txt
Hello
root@ec81070f7518:/PR3# ./fcopy input2.txt output.txt
Cannot open file input2.txt for reading.
root@215c11034d29:/PR3# dd if=/dev/urandom of=big_file.txt bs=1k count=100
100+0 records in
100+0 records out
102400 bytes (102 kB, 100 KiB) copied, 0.000841781 s, 102 MB/s
root@215c11034d29:/PR3# ulimit -f 10
root@215c11034d29:/PR3# ./fcopy big_file.txt output.txt
File size limit exceeded   (core dumped) ./fcopy big_file.txt output.txt
root@215c11034d29:/PR3# 
```
### Завдання 3.6
Напишіть програму, що демонструє використання обмеження (max stack segment size). Підказка: рекурсивна програма активно використовує стек.
```C
#include <stdio.h>
#include <stdlib.h>

void recursive_function(int depth) {
    char stack_buffer[10240];

    stack_buffer[0] = (char)(depth % 255);

    if (depth % 10 == 0) {
        printf("depth: %d\n", depth);
    }

    recursive_function(depth + 1);
}

int main() {
    recursive_function(1);
    return 0;
}
```
```bash
root@f0e6014580e8:/PR3# ulimit -s 8192
root@f0e6014580e8:/PR3# ./a.out 
...
depth: 750
depth: 760
depth: 770
depth: 780
depth: 790
depth: 800
depth: 810
Segmentation fault         (core dumped) ./a.out

root@b88a5c361ecd:/PR3# ulimit -s 100
root@b88a5c361ecd:/PR3# ./a.out  
Segmentation fault         (core dumped) ./a.out

root@b88a5c361ecd:/PR3# 
```
