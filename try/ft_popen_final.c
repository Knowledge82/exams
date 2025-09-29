#include <unistd.h>
#include <stdlib.h>

int	ft_popen(const char *file, const char *argv[], int type)
{
	int	fd[2];
	pid_t	pid;

	if ((type != 'r' && type != 'w') || !file || !argv || !argv[0])
		return (-1);
	
	if (pipe(fd) == -1)
		return (-1);

	pid = fork();
	if (pid < 0)// err
	{
		close(fd[0]);
		close(fd[1]);
		return (-1);
	}
	if (pid == 0)// child
	{
		if (type == 'r')
		{
			close(fd[0]);
			if (dup2(fd[1], STDOUT_FILENO) == -1)
				exit(-1);
			close(fd[1]);
		}
		if (type == 'w')
		{
			close(fd[1]);
			if (dup2(fd[0], STDIN_FILENO) == -1)
				exit(-1);
			close(fd[0]);
		}
		execvp(file, argv);
		exit(127);
	}
	else//parent
	{
		if (type == 'r')
		{
			close(fd[1]);
			return (fd[0]);
		}
		if (type == 'w')
		{
			close(fd[0]);
			return (fd[1]);
		}
	}
}
