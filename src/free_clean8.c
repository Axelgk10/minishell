/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   free_clean8.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: axgimene <axgimene@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/31 17:06:33 by axgimene          #+#    #+#             */
/*   Updated: 2025/11/11 16:30:11 by axgimene         ###   ########.fr       */
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

void	free_commands(t_cmd **commands)
{
	t_cmd	*current;
	t_cmd	*temp;
	int		i;

	if (!commands || !*commands)
		return ;
	current = *commands;
	while (current)
	{
		temp = current->next;
		if (current->av)
		{
			i = 0;
			while (current->av[i])
			{
				free(current->av[i]);
				i++;
			}
			free(current->av);
		}
		free(current);
		current = temp;
	}
	*commands = NULL;
}

void	cleanup_command_fds(t_cmd *cmd)
{
	if (!cmd)
		return ;
	if (cmd->in_fd > 2)
	{
		close(cmd->in_fd);
		cmd->in_fd = -1;
	}
	if (cmd->out_fd > 2)
	{
		close(cmd->out_fd);
		cmd->out_fd = -1;
	}
	if (cmd->pipe[0] > 2)
	{
		close(cmd->pipe[0]);
		cmd->pipe[0] = -1;
	}
	if (cmd->pipe[1] > 2)
	{
		close(cmd->pipe[1]);
		cmd->pipe[1] = -1;
	}
}
