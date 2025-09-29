#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>

int	picoshell(char ***cmds)
{
	int	fd[2];
	int	in_fd = -1;
	int i = 0;
	pid_t	pid;
  
	if (!cmds || !cmds[0])
		return (1);

	while (cmds[i])
	{
		if (pipe(fd) == -1)
		{
			if (in_fd != -1)
				close(in_fd);
			return (1);
		}
		pid = fork();

		if (pid < 0)
		{
			close(fd[0]);
			close(fd[1]);
			if (in_fd != -1)
				close(in_fd);
			return (1);
		}
		if (pid == 0)
		{
			close(fd[0]);
			if (in_fd != -1)
			{
				if (dup2(in_fd, STDIN_FILENO) == -1)
					exit(1);
				close(in_fd);
			}
			if (cmds[i + 1])
			{
				if (dup2(fd[1], STDOUT_FILENO) == -1)
					exit(1);
			}
			close(fd[1]);
			execvp(cmds[i][0], cmds[i]);
			exit(127);
		}
		else
		{
			close(fd[1]);
			if (in_fd != -1)
				close(in_fd);
			in_fd = fd[0];
		}
		i++;
	}
	close(fd[0]);
	while (wait(NULL) > 0)
		;
	return 0;
}
