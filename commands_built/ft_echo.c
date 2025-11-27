/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_echo.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: axgimene <axgimene@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/10 18:35:13 by gguardam          #+#    #+#             */
/*   Updated: 2025/11/17 18:45:17 by axgimene         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

static int	echo_newline(t_cmd *cmd)
{
	int	i;
	int	newline;
	int	j;

	i = 1;
	newline = 1;
	while (cmd->av[i])
	{
		if (cmd->av[i][0] == '-' && cmd->av[i][1] == 'n')
		{
			j = 1;
			while (cmd->av[i][j] == 'n')
				j++;
			if (cmd->av[i][j] == '\0')
			{
				newline = 0;
				i++;
				continue;
			}
		}
		break;
	}
	return (newline);
}

int	ft_echo(t_cmd *cmd)
{
	int	i;
	int	newline;

	i = 1;
	if (cmd->av[i][0] == '-' && cmd->av[i][1] == 'n')
		i++;
	newline = echo_newline(cmd);
	while (cmd->av[i])
	{
		ft_putstr_fd(cmd->av[i], cmd->out_fd);
		if (cmd->av[i + 1])
			ft_putchar_fd(' ', cmd->out_fd);
		i++;
	}
	if (newline)
		ft_putchar_fd('\n', cmd->out_fd);
	return (0);
}
