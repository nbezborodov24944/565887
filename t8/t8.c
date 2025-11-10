#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <sys/stat.h>

static void perror_exit(const char *msg) {
    perror(msg);
    exit(EXIT_FAILURE);
}

int main(int argc, char *argv[]) {
    const char *path;
    const char *editor = "vi";
    int fd;
    struct flock fl;
    int rc;

    if (argc < 2) {
        fprintf(stderr, "Usage: %s <file> [editor]\n", argv[0]);
        return 2;
    }
    path = argv[1];
    if (argc >= 3) editor = argv[2];

    /* Открываем файл (read/write — чтобы поставить эксклюзивную блокировку) */
    fd = open(path, O_RDWR);
    if (fd == -1) {
        /* если файл не существует — можно создать (по желанию) */
        if (errno == ENOENT) {
            fd = open(path, O_RDWR | O_CREAT, 0666);
            if (fd == -1) perror_exit("open (create)");
        } else {
            perror_exit("open");
        }
    }

    /* Настраиваем структуру блокировки: whole-file */
    memset(&fl, 0, sizeof(fl));
    fl.l_type = F_WRLCK;     /* эксклюзивная (write) блокировка */
    fl.l_whence = SEEK_SET;
    fl.l_start = 0;          /* с начала */
    fl.l_len = 0;            /* 0 = до конца файла (весь файл) */

    /* Пример 1: блокировать **блокирующе** (процесс будет ждать, если кто-то держит блок) */
    rc = fcntl(fd, F_SETLKW, &fl);
    if (rc == -1) {
        /* Если хотите неблокирующее поведение, используйте F_SETLK и проверяйте EACCES/EAGAIN */
        perror("fcntl(F_SETLKW) — установить блокировку");
        close(fd);
        return 3;
    }

    printf("Файл '%s' захвачен (fcntl F_WRLCK). Запускаю редактор: %s\n", path, editor);
    printf("Чтобы освободить блокировку — выйдите из редактора. PID=%d\n", getpid());

    /* Вызов редактора через system(). Пока редактор жив — блокировка удерживается. */
    {
        char cmdline[1024];
        /* Экранный редактор откроет файл по имени; используем system(3) по условию задания */
        snprintf(cmdline, sizeof(cmdline), "%s %s", editor, path);
        rc = system(cmdline);
        if (rc == -1) {
            perror("system");
        } else {
            printf("Редактор завершился, код возврата: %d\n", rc);
        }
    }

    /* Снимаем блокировку перед выходом (необязательно — close() снимет автоматически) */
    fl.l_type = F_UNLCK;
    if (fcntl(fd, F_SETLK, &fl) == -1) {
        perror("fcntl(F_SETLK, F_UNLCK)");
    } else {
        printf("Блокировка снята.\n");
    }

    close(fd);
    return 0;
}
