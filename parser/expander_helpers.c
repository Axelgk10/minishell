/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expander_helpers.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: axgimene <axgimene@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/03 14:00:00 by axgimene          #+#    #+#             */
/*   Updated: 2025/12/03 13:35:01 by axgimene         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

char	*join_and_free(char *result, char *part)
{
	char	*new_result;

	new_result = ft_strjoin(result, part);
	free(part);
	if (!new_result)
	{
		free(result);
		return (NULL);
	}
	free(result);
	return (new_result);
}

void	print_redir_error(t_token *filename_token)
{
	ft_putstr_fd("minishell: syntax error near unexpected token `", 2);
	if (filename_token)
		ft_putstr_fd(filename_token->value, 2);
	else
		ft_putstr_fd("newline", 2);
	ft_putstr_fd("'\n", 2);
}
