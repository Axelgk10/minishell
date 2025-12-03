/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_env.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gguardam <gguardam@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/10 18:35:24 by gguardam          #+#    #+#             */
/*   Updated: 2025/12/03 12:17:24 by gguardam         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

int	ft_env(t_shell *shell)
{
	int	i;

	i = 0;
	while (shell->env[i])
	{
		if (ft_strchr(shell->env[i], '='))
		{
			ft_putstr_fd(shell->env[i], shell->commands->out_fd);
			ft_putchar_fd('\n' ,shell->commands->out_fd);
		}
		i++;
	}
	return (0);
}
