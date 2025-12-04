/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   execution_piped.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: axgimene <axgimene@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/10 18:38:00 by gguardam          #+#    #+#             */
/*   Updated: 2025/12/04 18:00:00 by axgimene         ###   ########.fr       */
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

int	execute_pipeline(t_shell *shell, t_cmd *commands)
{
	t_cmd	*current;
	pid_t	*pids;
	int		count;
	int		i;
	int		pipe_fd[2];
	int		prev_pipe_out;
	pid_t	pid;

	if (!shell || !commands || !has_valid_commands(commands))
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
			free(pids);
			return (1);
		}
		pid = fork();
		if (pid == -1)
		{
			perror("fork");
			if (prev_pipe_out > 2)
				close(prev_pipe_out);
			if (current->next)
			{
				close(pipe_fd[0]);
				close(pipe_fd[1]);
			}
			free(pids);
			return (1);
		}
		if (pid == 0)
			execute_child_process(shell, current, prev_pipe_out,
				pipe_fd, pids);
		pids[i] = pid;
		if (prev_pipe_out > 2)
			close(prev_pipe_out);
		if (current->next)
		{
			close(pipe_fd[1]);
			prev_pipe_out = pipe_fd[0];
		}
		current = current->next;
		i++;
	}
	return (wait_all_processes(pids, count, shell));
}
