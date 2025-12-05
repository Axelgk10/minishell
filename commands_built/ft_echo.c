/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_echo.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: axgimene <axgimene@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/10 18:35:13 by gguardam          #+#    #+#             */
/*   Updated: 2025/12/05 15:33:55 by axgimene         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

int	ft_echo(t_cmd *cmd)
{
	int	i;
	int	newline;
	int	j;

	i = 0;
	newline = 1;
	while (cmd->av[++i] && cmd->av[i][0] == '-' && cmd->av[i][1] == 'n')
	{
		j = 0;
		while (cmd->av[i][++j] == 'n')
			;
		if (cmd->av[i][j] != '\0')
			break ;
		newline = 0;
	}
	while (cmd->av[i])
	{
		ft_putstr_fd(cmd->av[i], cmd->out_fd);
		if (cmd->av[i + 1])
			ft_putchar_fd(' ', cmd->out_fd);
		i++;
	}
	return (newline && (ft_putchar_fd('\n', cmd->out_fd), 0), 0);
}
