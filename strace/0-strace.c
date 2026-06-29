#include <sys/ptrace.h>
#include <sys/types.h>
#include <sys/user.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

extern char **environ;

/**
 * trace_syscalls - trace and print syscall numbers
 * @child: pid of child process
 *
 * Return: 0 on success
 */
int trace_syscalls(pid_t child)
{
	int status;
	struct user_regs_struct regs;

	ptrace(PTRACE_SETOPTIONS, child, 0, PTRACE_O_TRACESYSGOOD);

	while (1)
	{
		ptrace(PTRACE_SYSCALL, child, 0, 0);
		waitpid(child, &status, 0);

		if (WIFEXITED(status))
			break;

		if (!(WSTOPSIG(status) & 0x80))
			continue;

		if (ptrace(PTRACE_GETREGS, child, 0, &regs) == -1)
			exit(1);

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
		execve(argv[1], &argv[1], environ);
		perror("execve");
		exit(1);
	}

	return (trace_syscalls(child));
}
