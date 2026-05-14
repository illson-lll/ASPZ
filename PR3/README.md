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
root@e6279fe86d31:/PR3# 
```
