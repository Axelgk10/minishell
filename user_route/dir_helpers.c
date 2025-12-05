/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   dir_helpers.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gguardam <gguardam@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/05 15:00:00 by gguardam          #+#    #+#             */
/*   Updated: 2025/12/05 15:05:31 by gguardam         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

char	*get_env_value_from_shell(t_shell *shell, char *var_name)
{
	int		i;
	size_t	len;

	if (!shell || !shell->env || !var_name)
		return (NULL);
	len = ft_strlen(var_name);
	i = 0;
	while (shell->env[i])
	{
		if (ft_strncmp(shell->env[i], var_name, len) == 0
			&& shell->env[i][len] == '=')
			return (shell->env[i] + len + 1);
		i++;
	}
	return (NULL);
}
