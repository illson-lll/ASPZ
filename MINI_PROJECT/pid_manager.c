#define _XOPEN_SOURCE 700

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>
#include <errno.h>

#define MAX_PROCESSES 10

typedef struct
{
    pid_t pid;
    char name[50];
    int is_active;
} proccess;

proccess proc_list[MAX_PROCESSES];

void handle_sigchld(int sig)
{
    int status;
    pid_t pid;

    while ((pid = waitpid(-1, &status, WNOHANG)) > 0)
    {
        for (int i = 0; i < MAX_PROCESSES; i++)
        {
            if (proc_list[i].is_active && proc_list[i].pid == pid)
            {
                proc_list[i].is_active = 0;
                printf("[PID: %d] Процес %s завершився.\n", proc_list[i].pid, proc_list[i].name);
                fflush(stdout);
                break;
            }
        }
    }
}

void create_procces(const char *name, int duration)
{
    int index = -1;
    for (int i = 0; i < MAX_PROCESSES; i++)
    {
        if (!proc_list[i].is_active)
        {
            index = i;
            break;
        }
    }
    if (index == -1)
    {
        printf("Помилка: ліміт процесів.\n");
        return;
    }
    pid_t cpid = fork();

    if (cpid < 0)
    {
        printf("Помилка: fork().\n");
        return;
    }
    else if (cpid == 0)
    {
        printf("\n[PID %d] Запущено: %s на %d сек.\n", getpid(), name, duration);
        sleep(duration);
        exit(0);
    }
    else
    {
        proc_list[index].pid = cpid;
        strncpy(proc_list[index].name, name, sizeof(proc_list[index].name) - 1);
        proc_list[index].is_active = 1;
    }
}

void list_proccesses()
{
    printf("Активні процеси:\n");
    int c = 0;
    for (int i = 0; i < MAX_PROCESSES; i++)
    {
        if (proc_list[i].is_active)
        {
            c++;
            printf("[Процес %d] PID: %d | Назва: %s\n", i + 1, proc_list[i].pid, proc_list[i].name);
        }
    }
    if (c == 0)
    {
        printf("Фонові процеси відсутні.\n");
    }
    printf("\n");
}
void kill_procces(int pid_target)
{
    int index = -1;
    for (int i = 0; i < MAX_PROCESSES; i++)
    {
        if (proc_list[i].is_active && proc_list[i].pid == pid_target)
        {
            index = i;
            break;
        }
    }
    if (index == -1)
    {
        printf("Помилка: процесу не існує.\n");
    }
    else
    {
        if (kill(pid_target, SIGKILL) == 0)
        {
            printf("[Процес %d] знищенно сигналом.\n", pid_target);
        }
        else
        {
            printf("Помилка: kill()");
        }
    }
}

int main(int argc, char const *argv[])
{
    struct sigaction sa;
    sa.sa_flags = SA_RESTART;
    sa.sa_handler = &handle_sigchld;

    if (sigaction(SIGCHLD, &sa, NULL) == -1)
    {
        perror("Помилка: sigaction");
        exit(1);
    }

    char ch;
    char name[50];
    int duration;
    int pid;
    while (1)
    {
        printf("\nМенеджер процесів\n");
        printf("(1) Запустити процес\n");
        printf("(2) Показати активні процеси\n");
        printf("(3) Вбити процес\n");
        printf("(q) Вийти з програми\n");

        scanf(" %c", &ch);
        int clear;
        while ((clear = getchar()) != '\n' && clear != EOF)
            ;
        switch (ch)
        {
        case '1':
            printf("Введіть назву процесу: ");
            scanf("%s", name);
            printf("Введіть тривалість роботи (в секундах): ");
            scanf("%d", &duration);
            create_procces(name, duration);
            break;
        case '2':
            list_proccesses();
            break;
        case '3':
            printf("Введіть PID для знищення процесу: ");

            if (scanf("%d", &pid) == 1)
            {
                kill_procces(pid);
            }
            else
            {
                printf("-\n");
            }
            break;
        case 'q':
        case 'Q':
            printf("\nЗавершення роботи...\n");
            int active_proc_c = 0;

            for (int i = 0; i < MAX_PROCESSES; i++)
            {
                if (proc_list[i].is_active)
                    active_proc_c++;
            }
            if (active_proc_c != 0)
            {
                printf("Очікується завершення %d процесів...\n", active_proc_c);
                for (int i = 0; i < MAX_PROCESSES; i++)
                {
                    if (proc_list[i].is_active)
                    {
                        kill_procces(proc_list[i].pid);
                    }
                }
                return 0;
            }
            else
            {
                return 0;
            }

            break;
        default:
            printf("-");
        }
    }
    return 0;
}