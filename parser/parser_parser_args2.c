/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser_parser_args2.c                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: axgimene <axgimene@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/08 16:57:41 by axgimene          #+#    #+#             */
/*   Updated: 2025/11/27 17:25:06 by axgimene         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

t_cmd	*create_command(void)
{
	t_cmd	*cmd;

	cmd = malloc(sizeof(t_cmd));
	if (!cmd)
		return (NULL);
	cmd->av = NULL;
	cmd->ac = 0;  // ✅ Inicializa ac
	cmd->in_fd = STDIN_FILENO;
	cmd->out_fd = STDOUT_FILENO;
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
	if (cmd->av != NULL)
	{
		while (i < count)
		{
			new_args[i] = cmd->av[i];
			i++;
		}
		free(cmd->av);
	}
}

void	add_arg_to_command(t_cmd *cmd, char *arg)
{
    char	*new_arg;

    if (!cmd || !arg)
        return ;
    new_arg = ft_strdup(arg);
    if (!new_arg)
        return ;
    cmd->av = realloc(cmd->av, sizeof(char *) * (cmd->ac + 2));
    if (!cmd->av)
    {
        free(new_arg);
        return ;
    }
    cmd->av[cmd->ac] = new_arg;
    cmd->ac++;
    cmd->av[cmd->ac] = NULL;
}
