#include <sys/ptrace.h>
#include <sys/user.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

/**
 * main - Entry point for strace_0 with precise entry/exit tracking
 * @argc: Argument count
 * @argv: Argument vector
 *
 * Return: 0 on success, 1 on failure
 */
int main(int argc, char *argv[])
{
    pid_t child;
    int status;
    struct user_regs_struct regs;
    int is_entry = 1;

    if (argc < 2)
    {
        fprintf(stderr, "Usage: %s command [args...]\n", argv[0]);
        return (1);
    }

    child = fork();
    if (child < 0)
    {
        perror("fork");
        return (1);
    }

    if (child == 0)
    {
        /* Child process setup */
        ptrace(PTRACE_TRACEME, 0, NULL, NULL);
        kill(getpid(), SIGSTOP);
        execve(argv[1], argv + 1, NULL);
        perror("execve");
        exit(1);
    }
    else
    {
        /* Parent process waiting for initial SIGSTOP */
        waitpid(child, &status, 0);

        /* Set options to distinguish syscall traps explicitly */
        ptrace(PTRACE_SETOPTIONS, child, NULL, PTRACE_O_TRACESYSGOOD);
        ptrace(PTRACE_SYSCALL, child, NULL, NULL);

        while (1)
        {
            waitpid(child, &status, 0);
            if (WIFEXITED(status) || WIFSIGNALED(status))
                break;

            /* Check if the trap was specifically a syscall trap (SIGTRAP | 0x80) */
            if (WIFSTOPPED(status) && WSTOPSIG(status) == (SIGTRAP | 0x80))
            {
                if (is_entry)
                {
                    if (ptrace(PTRACE_GETREGS, child, NULL, &regs) == 0)
                    {
                        printf("%llu\n", regs.orig_rax);
                        fflush(stdout);
                    }
                    is_entry = 0;
                }
                else
                {
                    is_entry = 1;
                }
            }
            /* Reset entry flag if an unexpected execve boundary disruption occurs */
            else if (WIFSTOPPED(status) && WSTOPSIG(status) == SIGTRAP)
            {
                is_entry = 1;
            }

            ptrace(PTRACE_SYSCALL, child, NULL, NULL);
        }
    }

    return (0);
}
