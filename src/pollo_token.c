/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pollo_token.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: axgimene <axgimene@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/06 12:52:22 by axgimene          #+#    #+#             */
/*   Updated: 2025/11/11 16:52:39 by axgimene         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

char	*extract_word(char *input, int *i)
{
	int		start;
	int		len;
	char	quote;

	start = *i;
	len = 0;
	quote = 0;
	while (input[*i] && (!is_metachar(input[*i]) || quote))
	{
		handle_quotes_in_token(input, i, &quote);
		if (!input[*i])
			break ;
		len++;
		(*i)++;
	}
	return (ft_substr(input, start, len));
}
