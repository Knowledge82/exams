#include <unistd.h>
#include <sys/wait.h>
#include <string.h>

static int	err_msg(char *s, char *arg)
{
	while (*s)
		write(2, s++, 1);
	if (arg)
	{
		write(2, " ", 1);
		while (*arg)
			write(2, arg++, 1);
	}
	write(2, "\n", 1);
	return (1);
}

static int	cd(char **argv, int args_num)
{
	if (args_num != 2)
		return err_msg("error: cd: bad arguments", NULL);
	else if (chdir(argv[1]) == -1)
		return err_msg("error: cd: cannot change directory to", argv[1]);
	return (0);
}

static int	exec(char **argv, char **envp, int i)
{
	int	status;
	int	fd[2];
	int	pipes = (argv[i] && !strcmp(argv[i], "|"));

	if (pipes && pipe(fd) == -1)
		return (err_msg("error: fatal", NULL));
	int	pid = fork();
	if (!pid)
	{
		argv[i] = 0;
		if (pipes && (dup2(fd[1], 1) == -1 || close(fd[1]) == -1 || close(fd[0]) == -1))
			return (err_msg("error: fatal", NULL));
		execve(*argv, argv, envp);
		return (err_msg("error: cannot execute", *argv));
	}
	waitpid(pid, &status, 0);
	if (pipes && (dup2(fd[0], 0) == -1 || close(fd[0]) == -1 || close(fd[1]) == -1))
		return (err_msg("error: fatal", NULL));
	return WIFEXITED(status) && WEXITSTATUS(status);
}

int	main(int argc, char **argv, char **envp)
{
	(void)argc;
	int	status = 0;

	while (*argv && *(argv + 1))
	{
		argv++;
		int	i = 0;
		while (argv[i] && strcmp(argv[i], "|") && strcmp(argv[i], ";"))
			i++;
		if (strcmp(*argv, "cd") == 0)
			status = cd(argv, i);
		else if (i)
			status = exec(argv, envp, i);
		argv = argv + i;
	}
	return (status);
}
