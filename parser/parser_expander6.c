/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser_expander6.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gguardam <gguardam@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/29 15:17:13 by axgimene          #+#    #+#             */
/*   Updated: 2025/12/05 14:48:44 by gguardam         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

static char	*search_in_env_array(char **env, char *var_name, int len)
{
	int	i;

	i = 0;
	while (env[i])
	{
		if (ft_strncmp(env[i], var_name, len) == 0 && env[i][len] == '=')
			return (ft_strdup(env[i] + len + 1));
		i++;
	}
	return (NULL);
}

static char	*get_env_value(t_shell *shell, char *var_name)
{
	int		len;
	char	*result;

	if (!shell || !var_name)
		return (ft_strdup(""));
	if (ft_strncmp(var_name, "?", 1) == 0)
		return (ft_itoa(shell->exit_status));
	len = ft_strlen(var_name);
	result = search_in_env_array(shell->env, var_name, len);
	if (result)
		return (result);
	if (shell->local_vars)
	{
		result = search_in_env_array(shell->local_vars, var_name, len);
		if (result)
			return (result);
	}
	return (ft_strdup(""));
}

static char	*extract_variable_name(char *str, int *i)
{
	int	start;

	start = *i;
	if (str[*i] == '?')
	{
		(*i)++;
		return (ft_strdup("?"));
	}
	while (str[*i] && (ft_isalnum(str[*i]) || str[*i] == '_'))
		(*i)++;
	return (ft_substr(str, start, *i - start));
}

int	is_dollar_terminator(char c)
{
	return (c == ' ' || c == '\'' || c == '"' || c == '$' || c == '=');
}

char	*expand_dollar(t_shell *shell, char *str, int *i)
{
	char	*var_name;
	char	*var_value;

	(*i)++;
	if (!str[*i] || is_dollar_terminator(str[*i]))
		return (ft_strdup("$"));
	var_name = extract_variable_name(str, i);
	if (!var_name)
		return (ft_strdup(""));
	var_value = get_env_value(shell, var_name);
	free(var_name);
	if (!var_value)
		return (ft_strdup(""));
	return (var_value);
}
