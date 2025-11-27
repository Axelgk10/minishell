#include "../minishell.h"

static int	execute_builtin_cd_pwd_exit(t_shell *shell, t_cmd *cmd)
{
	if (!ft_strcmp(cmd->av[0], "cd"))
	{
		shell->exit_status = change_directory(cmd->av[1]);
		if (shell->exit_status == 0)
			update_envs(shell);
		return (0);
	}
	if (!ft_strcmp(cmd->av[0], "pwd"))
		return (ft_pwd(cmd));
	if (!ft_strcmp(cmd->av[0], "exit"))
	{
		manage_exit(shell);
		return (0);
	}
	return (-1);
}

static int	execute_builtin_env_echo_export(t_shell *shell, t_cmd *cmd)
{
	if (!ft_strcmp(cmd->av[0], "env"))  // ✅ SIN la condición got_path
		return (ft_env(shell));
	if (!ft_strcmp(cmd->av[0], "echo"))
		return (ft_echo(cmd));
	if (!ft_strcmp(cmd->av[0], "export"))
		return (export_variables(shell));
	if (!ft_strcmp(cmd->av[0], "unset"))
		return (unset_variables(shell));
	if (ft_strchr(cmd->av[0], '='))
		return (set_local_var(shell));
	return (-1);
}

static int	execute_builtin_in_pipeline(t_shell *shell, t_cmd *cmd)
{
	int	result;

	if (!shell || !cmd || !cmd->av || !cmd->av[0])
		return (1);
	result = execute_builtin_cd_pwd_exit(shell, cmd);
	if (result != -1)
		return (result);
	result = execute_builtin_env_echo_export(shell, cmd);
	if (result != -1)
		return (result);
	return (write_error_message(STDERR_FILENO, cmd->av[0], "", "command not found"));
}

static void	setup_child_input(int prev_pipe_out, t_cmd *cmd)
{
	if (prev_pipe_out > 2)
	{
		dup2(prev_pipe_out, STDIN_FILENO);
		close(prev_pipe_out);
	}
	else if (cmd->in_fd != STDIN_FILENO && cmd->in_fd > 2)
	{
		dup2(cmd->in_fd, STDIN_FILENO);
		close(cmd->in_fd);
	}
}

static void	setup_child_output(t_cmd *cmd, int *pipe_fd)
{
	if (cmd->next)
	{
		dup2(pipe_fd[1], STDOUT_FILENO);
		close(pipe_fd[1]);
		close(pipe_fd[0]);
	}
	else if (cmd->out_fd != STDOUT_FILENO && cmd->out_fd > 2)
	{
		dup2(cmd->out_fd, STDOUT_FILENO);
		close(cmd->out_fd);
	}
}

static int	execute_child_process(t_shell *shell, t_cmd *cmd, int prev_pipe_out, int *pipe_fd)
{
	char	**path_env;
	char	*bin_path;

	setup_child_input(prev_pipe_out, cmd);
	setup_child_output(cmd, pipe_fd);
	if (cmd->is_builtin)
		exit(execute_builtin_in_pipeline(shell, cmd));
	path_env = get_path_values(shell->env, "PATH");
	bin_path = find_binary(cmd->av[0], path_env);
	if (!bin_path)
	{
		// ✅ Valida si PATH existe
		if (!got_path(shell))
		{
			write_error_message(STDERR_FILENO, cmd->av[0], "", "No such file or directory");
		}
		else
		{
			write_error_message(STDERR_FILENO, cmd->av[0], "", "command not found");
		}
		// ✅ Libera path_env
		int i = 0;
		while(path_env && path_env[i])
			free(path_env[i++]);
		free(path_env);
		exit(127);
	}
	if (execve(bin_path, cmd->av, shell->env) == -1)
	{
		free(bin_path);
		int i = 0;
		while(path_env && path_env[i])
			free(path_env[i++]);
		free(path_env);
		error_executing(2, shell->env, cmd->av);
	}
	exit(127);
}

static int	count_commands(t_cmd *commands)
{
	t_cmd	*current;
	int		count;

	current = commands;
	count = 0;
	while (current)
	{
		count++;
		current = current->next;
	}
	return (count);
}

static void	handle_parent_cleanup(int *prev_pipe_out, t_cmd *current, int *pipe_fd)
{
	if (*prev_pipe_out > 2)
		close(*prev_pipe_out);
	if (current->next)
	{
		close(pipe_fd[1]);
		*prev_pipe_out = pipe_fd[0];
	}
}

static int	fork_and_execute(t_shell *shell, t_cmd *current, int prev_pipe_out, int *pipe_fd)
{
	pid_t	pid;

	pid = fork();
	if (pid == -1)
	{
		perror("fork");
		return (-1);
	}
	if (pid == 0)
		execute_child_process(shell, current, prev_pipe_out, pipe_fd);
	return (pid);
}

static void	cleanup_pipeline(pid_t *pids)
{
	if (pids)
		free(pids);
}

static int	wait_all_processes(pid_t *pids, int count, t_shell *shell)
{
	int	i;
	int	status;
	int	last_status;

	i = 0;
	last_status = 0;
	while (i < count)
	{
		waitpid(pids[i], &status, 0);
		if (i == count - 1)
			last_status = status;
		i++;
	}
	if (WIFEXITED(last_status))
		shell->exit_status = WEXITSTATUS(last_status);
	else if (WIFSIGNALED(last_status))
		shell->exit_status = 128 + WTERMSIG(last_status);
	cleanup_pipeline(pids);
	return (shell->exit_status);
}

int	execute_pipeline(t_shell *shell, t_cmd *commands)
{
	t_cmd	*current;
	pid_t	*pids;
	int		count;
	int		i;
	int		pipe_fd[2];
	int		prev_pipe_out;

	if (!shell || !commands)
		return (1);
	count = count_commands(commands);
	pids = malloc(sizeof(pid_t) * count);
	if (!pids)
		return (1);
	current = commands;
	i = 0;
	prev_pipe_out = -1;
	while (current)
	{
		if (current->next && pipe(pipe_fd) == -1)
		{
			perror("pipe");
			cleanup_pipeline(pids);
			return (1);
		}
		pids[i] = fork_and_execute(shell, current, prev_pipe_out, pipe_fd);
		if (pids[i] == -1)
		{
			if (prev_pipe_out > 2)
				close(prev_pipe_out);
			if (current->next)
			{
				close(pipe_fd[0]);
				close(pipe_fd[1]);
			}
			cleanup_pipeline(pids);
			return (1);
		}
		handle_parent_cleanup(&prev_pipe_out, current, pipe_fd);
		current = current->next;
		i++;
	}
	return (wait_all_processes(pids, count, shell));
}
