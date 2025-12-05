/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main_quotes.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: axgimene <axgimene@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/04 18:00:00 by axgimene          #+#    #+#             */
/*   Updated: 2025/12/04 18:00:00 by axgimene         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	check_unclosed_quotes(char *input)
{
	int	i;
	int	single_count;
	int	double_count;

	i = -1;
	single_count = 0;
	double_count = 0;
	while (input[++i])
	{
		if (input[i] == '\'' && (i == 0 || input[i - 1] != '\\'))
			single_count++;
		else if (input[i] == '"' && (i == 0 || input[i - 1] != '\\'))
			double_count++;
	}
	if (single_count % 2 != 0)
		return (write(STDERR_FILENO,
				"minishell: unclosed single quote\n", 33), 1);
	if (double_count % 2 != 0)
		return (write(STDERR_FILENO,
				"minishell: unclosed double quote\n", 33), 1);
	return (0);
}
