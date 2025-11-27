#include "../minishell.h"

void	fd_checker(t_shell *shell)
{
	if (shell->commands->out_fd != STDOUT_FILENO && shell->commands->out_fd > 0)
	{
		dup2(shell->commands->out_fd, STDOUT_FILENO);
		close(shell->commands->out_fd);  // ✅ Cierra DESPUÉS de dup2
	}
	if (shell->commands->in_fd != STDIN_FILENO && shell->commands->in_fd > 0)
	{
		dup2(shell->commands->in_fd, STDIN_FILENO);
		close(shell->commands->in_fd);  // ✅ Cierra DESPUÉS de dup2
	}
}

void	status_wait(pid_t pid, int status)
{
	waitpid(pid, &status, 0);
	if (WIFEXITED(status))
		g_exit_status = WEXITSTATUS(status);
	else if (WIFSIGNALED(status))
		g_exit_status = 128 + WTERMSIG(status);
	else
		g_exit_status = 1;
}

char	*find_binary(char *command, char **paths)
{
	char	*tmp_path;
	char	*full_path;
	int		i;

	if (!command || !paths)
		return (NULL);
	i = 0;
	while (paths[i])
	{
		tmp_path = ft_strjoin(paths[i], "/");
		if (!tmp_path)
			return (NULL);
		full_path = ft_strjoin(tmp_path, command);
		free(tmp_path);
		if (!full_path)
			return (NULL);
		if (access(full_path, X_OK) == 0)
			return (full_path);
		free(full_path);
		i++;
	}
	return (NULL);
}