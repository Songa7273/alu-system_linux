#include <sys/ptrace.h>
#include <sys/types.h>
#include <sys/user.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

/**
 * trace_syscalls - trace and print syscall numbers
 * @child: pid of traced process
 *
 * Return: 0 on success
 */
int trace_syscalls(pid_t child)
{
	int status;
	struct user_regs_struct regs;

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

		printf("%lld\n", (long long)regs.orig_rax);
	}

	return (0);
}

/**
 * main - executes and traces a command
 * @argc: argument count
 * @argv: argument vector
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
		ptrace(PTRACE_TRACEME, 0, NULL, NULL);
		execvp(argv[1], &argv[1]);
		perror("execvp");
		exit(EXIT_FAILURE);
	}

	return (trace_syscalls(child));
}
