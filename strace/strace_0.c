#include <sys/ptrace.h>
#include <sys/types.h>
#include <sys/user.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

extern char **environ;

/**
 * trace_syscalls - trace child syscalls
 * @child: child pid
 *
 * Return: 0
 */
int trace_syscalls(pid_t child)
{
	int status;
	int entering;
	struct user_regs_struct regs;

	entering = 1;
	waitpid(child, &status, 0);

	while (1)
	{
		if (ptrace(PTRACE_SYSCALL, child, NULL, NULL) == -1)
			break;

		waitpid(child, &status, 0);

		if (WIFEXITED(status))
			break;

		if (ptrace(PTRACE_GETREGS, child, NULL, &regs) == -1)
			break;

		if (entering)
			printf("%lld\n", (long long)regs.orig_rax);

		entering = !entering;
	}

	return (0);
}

/**
 * main - execute and trace a command
 * @argc: number of arguments
 * @argv: command line arguments
 *
 * Return: 0 on success, 1 on failure
 */
int main(int argc, char *argv[])
{
	pid_t child;

	if (argc < 2)
	{
		fprintf(stderr, "Usage: %s command [args...]\n", argv[0]);
		return (1);
	}

	child = fork();

	if (child == -1)
	{
		perror("fork");
		return (1);
	}

	if (child == 0)
	{
		if (ptrace(PTRACE_TRACEME, 0, NULL, NULL) == -1)
		{
			perror("ptrace");
			exit(EXIT_FAILURE);
		}

		execve(argv[1], &argv[1], environ);
		perror("execve");
		exit(EXIT_FAILURE);
	}

	return (trace_syscalls(child));
}
