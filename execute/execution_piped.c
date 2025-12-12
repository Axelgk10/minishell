/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   execution_piped.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: axgimene <axgimene@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/10 18:38:00 by gguardam          #+#    #+#             */
/*   Updated: 2025/12/05 17:28:12 by axgimene         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

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
	free(pids);
	return (shell->exit_status);
}

static int	has_valid_commands(t_cmd *commands)
{
	t_cmd	*current;

	current = commands;
	while (current)
	{
		if (!current->av || !current->av[0] || current->av[0][0] == '\0')
			return (0);
		current = current->next;
	}
	return (1);
}

static int	handle_pipe_fork(t_shell *shell, t_cmd *current, t_pipe_data *data)
{
	pid_t	pid;

	if (current->next && pipe(data->pipe_fd) == -1)
		return (perror("pipe"), free(data->pids), 1);
	pid = fork();
	if (pid == -1)
	{
		perror("fork");
		if (data->prev_pipe_out > 2)
			close(data->prev_pipe_out);
		if (current->next)
		{
			close(data->pipe_fd[0]);
			close(data->pipe_fd[1]);
		}
		free(data->pids);
		return (1);
	}
	if (pid == 0)
		execute_child_process(shell, current, data);
	data->pids[data->i] = pid;
	return (0);
}

int	execute_pipeline(t_shell *shell, t_cmd *commands)
{
	t_cmd		*current;
	t_pipe_data	data;

	if (!shell || !commands || !has_valid_commands(commands))
		return (1);
	data.pids = malloc(sizeof(pid_t) * count_commands(commands));
	if (!data.pids)
		return (1);
	current = commands;
	data.i = 0;
	data.prev_pipe_out = -1;
	while (current)
	{
		if (handle_pipe_fork(shell, current, &data))
			return (1);
		if (data.prev_pipe_out > 2)
			close(data.prev_pipe_out);
		if (current->next && (close(data.pipe_fd[1]), 1))
			data.prev_pipe_out = data.pipe_fd[0];
		current = current->next;
		data.i++;
	}
	return (wait_all_processes(data.pids, data.i, shell));
}
