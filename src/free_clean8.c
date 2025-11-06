/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   free_clean8.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: axgimene <axgimene@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/31 17:06:33 by axgimene          #+#    #+#             */
/*   Updated: 2025/11/06 18:27:06 by axgimene         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

void	free_tokens(t_token **tokens)
{
	t_token	*current;
	t_token	*temp;

	if (!tokens || !*tokens)
		return ;
	current = *tokens;
	while (current)
	{
		temp = current->next;
		if (current->value)
			free(current->value);
		free(current);
		current = temp;
	}
	*tokens = NULL;
}
static void free_command_args_and_fds(t_cmd *cmd)
{
	int	i;
	
	if (!cmd)
		return ;
	if (cmd->av)
	{
		i = 0;
		while (cmd->av[i])
			free(cmd->av[i++]);
		free(cmd->av);
	}
	if (cmd->in_fd > 2)
			close(cmd->in_fd);
		if (cmd->out_fd > 2)
			close(cmd->out_fd);
		if (cmd->pipe[0] > 2)
			close(cmd->pipe[0]);
		if (cmd->pipe[1] > 2)
			close(cmd->pipe[1]);
}


void	free_commands(t_cmd **commands)
{
	t_cmd	*current;
	t_cmd	*temp;

	if (!commands || !*commands)
		return ;
	current = *commands;
	while (current)
	{
		temp =current->next;
		free_command_args_and_fds(current);
		free(current);
		current = temp;
	}
	*commands = NULL;
}

void	cleanup_command_fds(t_cmd *cmd)
{
	if (cmd->in_fd > 2)
	{
		close(cmd->in_fd);
		cmd->in_fd = STDIN_FILENO;
	}
	if (cmd->out_fd > 2)
	{
		close(cmd->out_fd);
		cmd->out_fd = STDOUT_FILENO;
	}
}
