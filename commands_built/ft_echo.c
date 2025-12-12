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

static int	is_valid_n_flag(char *arg)
{
	int	j;

	if (arg[0] != '-' || arg[1] != 'n')
		return (0);
	j = 1;
	while (arg[j] == 'n')
		j++;
	if (arg[j] == '\0')
		return (1);
	return (0);
}

static int	echo_newline(t_cmd *cmd)
{
	int	i;
	int	newline;

	i = 1;
	newline = 1;
	while (cmd->av[i])
	{
		if (is_valid_n_flag(cmd->av[i]))
		{
			newline = 0;
			i++;
			continue ;
		}
		break ;
	}
	return (newline);
}

int	ft_echo(t_cmd *cmd)
{
	int	i;
	int	newline;

	newline = echo_newline(cmd);
	i = 1;
	while (cmd->av[i])
	{
		if (is_valid_n_flag(cmd->av[i]))
		{
			i++;
			continue ;
		}
		break ;
	}
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
