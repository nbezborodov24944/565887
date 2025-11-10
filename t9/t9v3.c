/* v1_no_wait_notify.c — модифицированный вариант без wait
 * Родитель не ждёт, но дочерний сам сообщает о своём завершении.
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>

int main(void) {
    setvbuf(stdout, NULL, _IONBF, 0); // без буфера

    pid_t pid = fork();
    if (pid < 0) {
        perror("fork");
        return 1;
    }

    if (pid == 0) {
        // Дочерний: через shell запустим cat, потом echo
        execlp("sh", "sh", "-c", "cat /etc/services; echo \"[child] Дочерний завершился ✅\"", (char *)NULL);
        perror("execlp");
        _exit(127);
    }

    // Родитель печатает свои строки
    printf("[parent] Начинаю печатать строки...\n");
    for (int i = 1; i <= 5; ++i) {
        printf("[parent] строка %d\n", i);
        usleep(200000);
    }

    printf("[parent] Родитель завершает работу (не ждёт дочернего)\n");
    return 0;
}
