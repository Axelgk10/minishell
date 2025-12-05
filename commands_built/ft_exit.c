/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_exit.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: axgimene <axgimene@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/10 18:35:35 by gguardam          #+#    #+#             */
/*   Updated: 2025/12/05 15:36:21 by axgimene         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

static int	is_numeric(char *str)
{
	int	i;

	i = 0;
	if (str[0] == '-' || str[0] == '+')
		i = 1;
	while (str[i])
	{
		if (!ft_isdigit(str[i]))
			return (0);
		i++;
	}
	return (1);
}

void	manage_exit(t_shell *shell)
{
	int	exit_code;

	printf("exit\n");
	exit_code = 0;
	if (shell->commands->av[1])
	{
		if (!is_numeric(shell->commands->av[1]))
		{
			write_error_message(STDERR_FILENO, "exit",
				shell->commands->av[1], "numeric argument required");
			exit_code = 2;
		}
		else
			exit_code = ft_atoi(shell->commands->av[1]);
	}
	if (shell->tokens && (free_tokens(&shell->tokens), 1))
		shell->tokens = NULL;
	if (shell->commands && (free_commands(&shell->commands), 1))
		shell->commands = NULL;
	if (shell->prompt && (free(shell->prompt), 1))
		shell->prompt = NULL;
	cleanup_shell(shell);
	rl_clear_history();
	exit(exit_code);
}
