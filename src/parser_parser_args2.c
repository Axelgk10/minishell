/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser_parser_args2.c                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: axgimene <axgimene@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/08 16:57:41 by axgimene          #+#    #+#             */
/*   Updated: 2025/11/03 16:45:10 by axgimene         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

t_cmd	*create_command(void)
{
	t_cmd	*cmd;

	cmd = malloc(sizeof(t_cmd));
	if (!cmd)
		return (NULL);
	cmd->as = NULL;
	cmd->av = NULL;
	cmd->in_fd = -1;
	cmd->out_fd = -1;
	cmd->pipe[0] = -1;
	cmd->pipe[1] = -1;
	cmd->is_builtin = 0;
	cmd->next = NULL;
	return (cmd);
}

void	copy_old_args(t_cmd *cmd, char **new_args, int count)
{
	int	i;

	i = 0;
	if (cmd->as != NULL)
	{
		while (i < count)
		{
			new_args[i] = cmd->as[i];
			i++;
		}
		free(cmd->as);
	}
}

void	add_arg_to_command(t_cmd *cmd, char *arg)
{
	int		count;
	char	**new_args;
	char	*dup_arg;

	if (!cmd || !arg)
		return ;
	count = 0;
	if (cmd->as != NULL)
		while (cmd->as[count])
			count++;
	new_args = malloc(sizeof(char *) * (count + 2));
	if (!new_args)
		return ;
	dup_arg = ft_strdup(arg);
	if (!dup_arg)
	{
		free(new_args);
		return ;
	}
	copy_old_args(cmd, new_args, count);
	new_args[count] = dup_arg;
	new_args[count + 1] = NULL;
	cmd->as = new_args;
}
