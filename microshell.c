#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>

typedef struct s_cmd
{
	char	**args;
	int		is_pipe;
	struct s_cmd	*next;
}	t_cmd;

static int	err_msg(char *msg, char *arg)
{
	int	i = 0;

	while (msg && msg[i])
	{
		write(2, &msg[i], 1);
		i++;
	}
	if (arg)
	{
		write(2, " ", 1);
		i = 0;
		while (arg[i])
		{	
			write(2, &arg[i], 1);
			i++;
		}
	}
	write(2, "\n", 1);
	return (1);
}

static int	cd(char **args, int num_args)
{
	if (num_args != 2)
		return err_msg("error: cd: bad arguments", NULL);
	else if (chdir(args[1]) == -1)
		return err_msg("error: cd: cannot change directory to", args[1]);
	return (0);
}

static int	count_args(char **args)
{
	int	i = 0;

	while (args[i] && strcmp(args[i], "|") != 0 && strcmp(args[i], ";") != 0)
		i++;
	return (i);
}

static char	**copy_args(char **argv, int count)
{
	int	i = 0;
	char	**args = malloc(sizeof(char *) * (count + 1));

	if (!args)
		return (NULL);
	while (i < count)
	{
		args[i] = argv[i];
		i++;
	}
	args[i] = NULL;
	return (args);
}

static void	free_commands(t_cmd *cmd)
{
	t_cmd	*temp;

	while (cmd)
	{
		temp = cmd;
		free(cmd->args);
		cmd = cmd->next;
		free(temp);
	}
}

static t_cmd	*parse_commands(char **argv)
{
	int	i = 0;
	int	count = 0;
	t_cmd	*head = NULL;
	t_cmd	*tail = NULL;
	t_cmd	*new;

	while (argv[i])
	{
		if (strcmp(argv[i], ";") == 0)
		{
			i++;
			continue;
		}
		new = malloc(sizeof(t_cmd));
		if (!new)
			return (NULL);
		count = count_args(&argv[i]);
		new->args = copy_args(&argv[i], count);
		new->is_pipe = (argv[i + count] && strcmp(argv[i + count], "|") == 0);
		new->next = NULL;
		if (!head)
			head = new;
		else
			tail->next = new;
		tail = new;
		i = i + count;
		if (argv[i] && (strcmp(argv[i], "|") == 0 || strcmp(argv[i], ";") == 0))
			i++;
	}
	return (head);
}

static int	exec(t_cmd *cmd, char **envp)
{
	int fd[2];
	int	prev_pipe = -1;
	int	status = 0;
	pid_t	pid;

	while (cmd)
	{
		if (cmd->args[0] && strcmp(cmd->args[0], "cd") == 0)
		{
			status = cd(cmd->args, count_args(cmd->args));
			cmd = cmd->next;
			continue;
		}
		if (cmd->is_pipe && pipe(fd) == -1)
			return err_msg("error: fatal", NULL);
		pid = fork();
		if (pid == -1)
			return err_msg("error: fatal", NULL);
		if (pid == 0)
		{
			if (prev_pipe != -1)
			{
				dup2(prev_pipe, STDIN_FILENO);
				close(prev_pipe);
			}
			if (cmd->is_pipe)
			{
				dup2(fd[1], STDOUT_FILENO);
				close(fd[0]);
				close(fd[1]);
			}
			execve(cmd->args[0], cmd->args, envp);
			err_msg("error: cannot execute", cmd->args[0]);
			exit (1);
		}
		else
		{
			waitpid(pid, &status, 0);
			if (prev_pipe != -1)
				close(prev_pipe);
			if (cmd->is_pipe)
			{
				close(fd[1]);
				prev_pipe = fd[0];
			}
			else
				prev_pipe = -1;
		}
		cmd = cmd->next;
	}
	return (status);
}

int	main(int argc, char **argv, char **envp)
{
	int	status;
	t_cmd	*cmd_list;

	(void)argc;
	cmd_list = parse_commands(&argv[1]);
	status = exec(cmd_list, envp);
	free_commands(cmd_list);
	return (status);	
}
