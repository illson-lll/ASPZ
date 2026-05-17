### Завдання 6.2
Створити програму з навмисним пошкодженням метаданих купи та порівняти діагностику помилки в glibc, AddressSanitizer і Valgrind з поясненням відмінностей.
```C
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char data[64];
} el;

void overflow(unsigned int count){
    unsigned int total_size = count * sizeof(el);

    printf("Введене count: %d\n",count);
    printf("total_size: %u байт\n", total_size);

    el *element = (el*) malloc(total_size);

    if(element == NULL){
        printf("null");
        return;
    }

    for(unsigned int i = 0; i<5;i++){
        printf("Заповнення адреси %p\n", (void*)&element[i]);
        memset(&element[i],'A',sizeof(el));
    }

    void *a = malloc(5);

    free(element);
}

int main(){
    unsigned int c = 67108865;
    overflow(c);
    return 0;
}

```
glibC:
```console 
illson@nobara-pc:~/Documents/ASPZ/PR6$ gcc t.c && ./a.out
Введене count: 67108865
total_size: 64 байт
Заповнення адреси 0x38992720
Заповнення адреси 0x38992760
Заповнення адреси 0x389927a0
Заповнення адреси 0x389927e0
Заповнення адреси 0x38992820
malloc(): corrupted top size
Aborted                    (core dumped) ./a.out
illson@nobara-pc:~/Documents/ASPZ/PR6$ 
```
valgrind:
```console
illson@nobara-pc:~/Documents/ASPZ/PR6$ valgrind --leak-check=full ./a.out
==8328== Memcheck, a memory error detector
==8328== Copyright (C) 2002-2024, and GNU GPL'd, by Julian Seward et al.
==8328== Using Valgrind-3.26.0 and LibVEX; rerun with -h for copyright info
==8328== Command: ./a.out
==8328== 
Введене count: 67108865
total_size: 64 байт
Заповнення адреси 0x4a76480
Заповнення адреси 0x4a764c0
==8328== Invalid write of size 8
==8328==    at 0x484F184: memset (vg_replace_strmem.c:1391)
==8328==    by 0x400547: overflow (in /home/illson/Documents/ASPZ/PR6/a.out)
==8328==    by 0x400586: main (in /home/illson/Documents/ASPZ/PR6/a.out)
==8328==  Address 0x4a764c0 is 0 bytes after a block of size 64 alloc'd
==8328==    at 0x4840B26: malloc (vg_replace_malloc.c:447)
==8328==    by 0x4004DC: overflow (in /home/illson/Documents/ASPZ/PR6/a.out)
==8328==    by 0x400586: main (in /home/illson/Documents/ASPZ/PR6/a.out)
==8328== 
==8328== Invalid write of size 8
==8328==    at 0x484F18B: memset (vg_replace_strmem.c:1391)
==8328==    by 0x400547: overflow (in /home/illson/Documents/ASPZ/PR6/a.out)
==8328==    by 0x400586: main (in /home/illson/Documents/ASPZ/PR6/a.out)
==8328==  Address 0x4a764d0 is 16 bytes after a block of size 64 alloc'd
==8328==    at 0x4840B26: malloc (vg_replace_malloc.c:447)
==8328==    by 0x4004DC: overflow (in /home/illson/Documents/ASPZ/PR6/a.out)
==8328==    by 0x400586: main (in /home/illson/Documents/ASPZ/PR6/a.out)
==8328== 
Заповнення адреси 0x4a76500
Заповнення адреси 0x4a76540
Заповнення адреси 0x4a76580
--8328-- VALGRIND INTERNAL ERROR: Valgrind received a signal 11 (SIGSEGV) - exiting
--8328-- si_code=128;  Faulting address: 0x0;  sp: 0x1002ca5d00

valgrind: the 'impossible' happened:
   Killed by fatal signal

host stacktrace:
==8328==    at 0x5805974F: get_bszB_as_is (m_mallocfree.c:302)
==8328==    by 0x5805974F: get_bszB (m_mallocfree.c:314)
==8328==    by 0x5805974F: vgPlain_arena_malloc (m_mallocfree.c:1819)
==8328==    by 0x58007523: vgPlain_cli_malloc (replacemalloc_core.c:106)
==8328==    by 0x58007523: vgPlain_cli_malloc (replacemalloc_core.c:101)
==8328==    by 0x58007523: vgMemCheck_new_block (mc_malloc_wrappers.c:374)
==8328==    by 0x580075C2: vgMemCheck_malloc (mc_malloc_wrappers.c:409)
==8328==    by 0x580075C2: vgMemCheck_malloc (mc_malloc_wrappers.c:404)
==8328==    by 0x580B48A8: do_client_request (scheduler.c:2018)
==8328==    by 0x580B48A8: vgPlain_scheduler (scheduler.c:1573)
==8328==    by 0x58111C67: thread_wrapper (syswrap-linux.c:102)
==8328==    by 0x58111C67: run_a_thread_NORETURN.lto_priv.0 (syswrap-linux.c:154)

sched status:
  running_tid=1

Thread 1: status = VgTs_Runnable (lwpid 8328)
==8328==    at 0x4840B26: malloc (vg_replace_malloc.c:447)
==8328==    by 0x40055B: overflow (in /home/illson/Documents/ASPZ/PR6/a.out)
==8328==    by 0x400586: main (in /home/illson/Documents/ASPZ/PR6/a.out)
client stack range: [0x1FFEFFD000 0x1FFF000FFF] client SP: 0x1FFEFFF630
valgrind stack range: [0x1002BA6000 0x1002CA5FFF] top usage: 14224 of 1048576


Note: see also the FAQ in the source distribution.
It contains workarounds to several common problems.
In particular, if Valgrind aborted or crashed after
identifying problems in your program, there's a good chance
that fixing those problems will prevent Valgrind aborting or
crashing, especially if it happened in m_mallocfree.c.

If that doesn't help, please report this bug to: www.valgrind.org

In the bug report, send all the above text, the valgrind
version, and what OS and version you are using.  Thanks.
```
AddressSanitizer:
```console
illson@nobara-pc:~/Documents/ASPZ/PR6$ gcc -fsanitize=address -g t.c -o prog_asan
illson@nobara-pc:~/Documents/ASPZ/PR6$ ./prog_asan
Введене count: 67108865
total_size: 64 байт
Заповнення адреси 0x7bd055fe0080
Заповнення адреси 0x7bd055fe00c0
=================================================================
==8675==ERROR: AddressSanitizer: heap-buffer-overflow on address 0x7bd055fe00c0 at pc 0x7f70574e39ea bp 0x7ffc45871b60 sp 0x7ffc45871330
WRITE of size 64 at 0x7bd055fe00c0 thread T0
    #0 0x7f70574e39e9 in memset (/lib64/libasan.so.8+0xe39e9) (BuildId: 25975f766867e9e604dc5a71a8befeaed3301942)
    #1 0x000000400587 in overflow /home/illson/Documents/ASPZ/PR6/t.c:24
    #2 0x0000004005c6 in main /home/illson/Documents/ASPZ/PR6/t.c:34
    #3 0x7f70572105b4 in __libc_start_call_main (/lib64/libc.so.6+0x35b4) (BuildId: abd2a4d896fed122b3f7da571746f193feeb86a1)
    #4 0x7f7057210667 in __libc_start_main@@GLIBC_2.34 (/lib64/libc.so.6+0x3667) (BuildId: abd2a4d896fed122b3f7da571746f193feeb86a1)
    #5 0x000000400414 in _start (/home/illson/Documents/ASPZ/PR6/prog_asan+0x400414) (BuildId: 195a94b5aa174c1965137cada40aa5439998136c)

0x7bd055fe00c0 is located 0 bytes after 64-byte region [0x7bd055fe0080,0x7bd055fe00c0)
allocated by thread T0 here:
    #0 0x7f70574e6f2b in malloc (/lib64/libasan.so.8+0xe6f2b) (BuildId: 25975f766867e9e604dc5a71a8befeaed3301942)
    #1 0x00000040051c in overflow /home/illson/Documents/ASPZ/PR6/t.c:15
    #2 0x0000004005c6 in main /home/illson/Documents/ASPZ/PR6/t.c:34
    #3 0x7f70572105b4 in __libc_start_call_main (/lib64/libc.so.6+0x35b4) (BuildId: abd2a4d896fed122b3f7da571746f193feeb86a1)
    #4 0x7f7057210667 in __libc_start_main@@GLIBC_2.34 (/lib64/libc.so.6+0x3667) (BuildId: abd2a4d896fed122b3f7da571746f193feeb86a1)
    #5 0x000000400414 in _start (/home/illson/Documents/ASPZ/PR6/prog_asan+0x400414) (BuildId: 195a94b5aa174c1965137cada40aa5439998136c)

SUMMARY: AddressSanitizer: heap-buffer-overflow /home/illson/Documents/ASPZ/PR6/t.c:24 in overflow
Shadow bytes around the buggy address:
  0x7bd055fdfe00: 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
  0x7bd055fdfe80: 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
  0x7bd055fdff00: 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
  0x7bd055fdff80: 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
  0x7bd055fe0000: fa fa fa fa 00 00 00 00 00 00 05 fa fa fa fa fa
=>0x7bd055fe0080: 00 00 00 00 00 00 00 00[fa]fa fa fa fa fa fa fa
  0x7bd055fe0100: fa fa fa fa fa fa fa fa fa fa fa fa fa fa fa fa
  0x7bd055fe0180: fa fa fa fa fa fa fa fa fa fa fa fa fa fa fa fa
  0x7bd055fe0200: fa fa fa fa fa fa fa fa fa fa fa fa fa fa fa fa
  0x7bd055fe0280: fa fa fa fa fa fa fa fa fa fa fa fa fa fa fa fa
  0x7bd055fe0300: fa fa fa fa fa fa fa fa fa fa fa fa fa fa fa fa
Shadow byte legend (one shadow byte represents 8 application bytes):
  Addressable:           00
  Partially addressable: 01 02 03 04 05 06 07 
  Heap left redzone:       fa
  Freed heap region:       fd
  Stack left redzone:      f1
  Stack mid redzone:       f2
  Stack right redzone:     f3
  Stack after return:      f5
  Stack use after scope:   f8
  Global redzone:          f9
  Global init order:       f6
  Poisoned by user:        f7
  Container overflow:      fc
  Array cookie:            ac
  Intra object redzone:    bb
  ASan internal:           fe
  Left alloca redzone:     ca
  Right alloca redzone:    cb
==8675==ABORTING
```
