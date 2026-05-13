# Завдання 1.2 
Дослідіть, які бібліотеки доступні у вашій системі.
1. Використовуйте команду ldd, щоб перевірити, які бібліотеки використовуються вашими програмами (наприклад, ls або gcc).
2. Знайдіть бібліотеку з math-функціями, визначте її точне місце розташування (find /usr/lib -name "*math*").
3. Перевірте символи бібліотеки за допомогою nm або objdump. Наприклад:
 nm -D /usr/lib/libm.so | grep erf
4. Проаналізуйте залежності біблFіотек за допомогою ldd або objdump -p.
5. Напишіть скрипт на Bash, який аналізує всі бібліотеки у /usr/lib, витягує з них символи та шукає певні функції (наприклад, sin, cos, exp).

### 1. Аналіз конкретної програми
Використувуючи ldd можна дізнатись які саме бібліотеки потрібні програми для запуску. 

```bash
illson@nobara-pc:/usr/bin$ ldd 7z
        linux-vdso.so.1 (0x00007f39e7fa6000)
        libstdc++.so.6 => /lib64/libstdc++.so.6 (0x00007f39e7c00000)
        libm.so.6 => /lib64/libm.so.6 (0x00007f39e7e86000)
        libgcc_s.so.1 => /lib64/libgcc_s.so.1 (0x00007f39e7bd4000)
        libc.so.6 => /lib64/libc.so.6 (0x00007f39e79e1000)
        /lib64/ld-linux-x86-64.so.2 (0x00007f39e7fa8000)
illson@nobara-pc:/usr/bin$
```
Архіватор 7z використовує linux-vdso заради створення для себе простору та системних викликів, бібліотеки мови С для підтримки його роботи в тому числі libm.so.6 (математична бібліотека) для складних алгоритмів стиснення.

### 2. Пошук бібліотеки 
Зазвичай в дистрибутивах від rad hat більшість пов'язаних бібліотек лежать в lib64.
```bash
illson@nobara-pc:/usr/bin$ find /usr/lib64 -name "*libm*"
/usr/lib64/libglibmm-2.4.so.1
/usr/lib64/libglibmm_generate_extra_defs-2.4.so.1
/usr/lib64/libmad.so.0
/usr/lib64/libmagic.so.1
/usr/lib64/libmalcontent-0.so.0
/usr/lib64/libmaliit-glib.so.2
/usr/lib64/libmaliit-plugins.so.2
/usr/lib64/libmana.so.1
/usr/lib64/libmarkdown.so.2
/usr/lib64/libmaxminddb.so.0
/usr/lib64/libmbedcrypto.so
...
```
/usr/lib64/libm.so.6
### 3. Пошук функції 
```bash
illson@nobara-pc:/usr/lib64$  nm -D libm.so.6 | grep erf
000000000004a460 W erf@@GLIBC_2.2.5
000000000004a9e0 W erfc@@GLIBC_2.2.5
0000000000060420 W erfcf@@GLIBC_2.2.5
00000000000231e0 W erfcf128@@GLIBC_2.26
0000000000060420 W erfcf32@@GLIBC_2.27
000000000004a9e0 W erfcf32x@@GLIBC_2.27
000000000004a9e0 W erfcf64@@GLIBC_2.27
0000000000035cf0 W erfcf64x@@GLIBC_2.27
0000000000035cf0 W erfcl@@GLIBC_2.2.5
0000000000060220 W erff@@GLIBC_2.2.5
00000000000240e0 W erff128@@GLIBC_2.26
0000000000060220 W erff32@@GLIBC_2.27
000000000004a460 W erff32x@@GLIBC_2.27
000000000004a460 W erff64@@GLIBC_2.27
00000000000358c0 W erff64x@@GLIBC_2.27
00000000000358c0 W erfl@@GLIBC_2.2.5
```
Виводиться адреса, та всі види помилкової функції, з різними типами та формулами.

### 4. Аналіз залежностей 
```bash
illson@nobara-pc:/usr/lib64$ objdump -p libm.so.6 | grep NEEDED
  NEEDED               libc.so.6
  NEEDED               ld-linux-x86-64.so.2
illson@nobara-pc:/usr/lib64$ 
```
libm використовує базову бібліотеку libc для мат. операцій, та лінкувальник бібілотек ld. 

### 5. Скрипт 
finder.sh
```bash
#!/bin/bash

DIR="/usr/lib64"

echo "Введіть функцію: "
read FUNC

echo "Пошук ($FUNC) у $DIR ..."

for file in "$DIR"/*.so*; do
        nm -D "$file" 2>/dev/null | grep -w "$FUNC" && echo "| $file"
done
```
```bash
Введіть функцію: 
cos
Пошук (cos) у /usr/lib64 ...
                 U cos@GLIBC_2.2.5
| /usr/lib64/lib2geom.so.1.4.0
                 U cos@GLIBC_2.2.5
| /usr/lib64/liba52.so.0
                 U cos@GLIBC_2.2.5
| /usr/lib64/liba52.so.0.0.0
                 U cos@GLIBC_2.2.5
| /usr/lib64/libadwaita-1.so.0
                 U cos@GLIBC_2.2.5
| /usr/lib64/libAlembic.so.1.8
                 U cos@GLIBC_2.2.5
| /usr/lib64/libAlembic.so.1.8.10
                 U cos@GLIBC_2.2.5
| /usr/lib64/libaom.so.3
                 U cos@GLIBC_2.2.5
| /usr/lib64/libaom.so.3.13.1
                 U cos@GLIBC_2.2.5
| /usr/lib64/libarrow.so.2000
                 U cos@GLIBC_2.2.5
| /usr/lib64/libarrow.so.2000.0.0
                 U cos@GLIBC_2.2.5
| /usr/lib64/libassimp.so.6
```





