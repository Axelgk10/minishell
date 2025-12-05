/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser_builtins2.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gguardam <gguardam@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/03 13:45:14 by axgimene          #+#    #+#             */
/*   Updated: 2025/12/05 14:41:16 by gguardam         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

static void	init_builtins_array(char **builtins)
{
	builtins[0] = "echo";
	builtins[1] = "cd";
	builtins[2] = "pwd";
	builtins[3] = "export";
	builtins[4] = "unset";
	builtins[5] = "env";
	builtins[6] = "exit";
	builtins[7] = NULL;
}

void	set_builtin_flag(t_cmd *cmd)
{
	char	*builtins[8];
	int		i;

	if (!cmd->av || !cmd->av[0])
		return ;
	i = 0;
	init_builtins_array(builtins);
	while (builtins[i])
	{
		if (ft_strncmp(cmd->av[0], builtins[i], ft_strlen(builtins[i])) == 0)
		{
			cmd->is_builtin = 1;
			return ;
		}
		else if (ft_strchr(cmd->av[0], '='))
		{
			cmd->is_builtin = 1;
			return ;
		}
		i++;
	}
	cmd->is_builtin = 0;
}
