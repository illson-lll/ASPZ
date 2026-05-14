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

