# Завдання 1.2 
Дослідіть, які бібліотеки доступні у вашій системі.
1. Використовуйте команду ldd, щоб перевірити, які бібліотеки використовуються вашими програмами (наприклад, ls або gcc).
2. Знайдіть бібліотеку з math-функціями, визначте її точне місце розташування (find /usr/lib -name "*math*").
3. Перевірте символи бібліотеки за допомогою nm або objdump. Наприклад:
 nm -D /usr/lib/libm.so | grep erf
4. Проаналізуйте залежності бібліотек за допомогою ldd або objdump -p.
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


