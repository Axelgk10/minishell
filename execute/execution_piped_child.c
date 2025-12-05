/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   execution_piped_child.c                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: axgimene <axgimene@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/04 18:00:00 by axgimene          #+#    #+#             */
/*   Updated: 2025/12/04 18:00:00 by axgimene         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

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

static void	handle_exec_error(char *bin_path, char **path_env, pid_t *pids)
{
	perror("execve");
	free(bin_path);
	cleanup_path_env(path_env);
	free(pids);
	exit(126);
}

static void	handle_no_binary(t_shell *shell, t_cmd *cmd,
	char **path_env, pid_t *pids)
{
	if (!got_path(shell))
		write_error_message(STDERR_FILENO, cmd->av[0],
			"", "No such file or directory");
	else
		write_error_message(STDERR_FILENO, cmd->av[0],
			"", "command not found");
	cleanup_path_env(path_env);
	free(pids);
	exit(127);
}

void	execute_child_process(t_shell *shell, t_cmd *cmd, t_pipe_data *data)
{
	char	**path_env;
	char	*bin_path;

	setup_child_input(data->prev_pipe_out, cmd);
	setup_child_output(cmd, data->pipe_fd);
	if (!cmd->av || !cmd->av[0] || cmd->av[0][0] == '\0')
	{
		write_error_message(STDERR_FILENO, "", "", "command not found");
		free(data->pids);
		exit(127);
	}
	if (cmd->is_builtin)
	{
		free(data->pids);
		exit(execute_builtin_in_pipeline(shell, cmd));
	}
	path_env = get_path_values(shell->env, "PATH");
	bin_path = find_binary(cmd->av[0], path_env);
	if (!bin_path)
		handle_no_binary(shell, cmd, path_env, data->pids);
	if (execve(bin_path, cmd->av, shell->env) == -1)
		handle_exec_error(bin_path, path_env, data->pids);
	exit(127);
}
