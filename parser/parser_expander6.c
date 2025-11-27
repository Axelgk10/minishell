/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser_expander6.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gguardam <gguardam@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/29 15:17:13 by axgimene          #+#    #+#             */
/*   Updated: 2025/11/26 11:36:04 by gguardam         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

static char	*get_env_value(t_shell *shell, char *var_name)
{
	int	i;
	int	len;

	if (!shell || !var_name)
		return (ft_strdup(""));
	if (ft_strncmp(var_name, "?", 1) == 0)
		return (ft_itoa(shell->exit_status));
	i = 0;
	len = ft_strlen(var_name);
	while (shell->env[i])
	{
		if (ft_strncmp(shell->env[i], var_name, len) == 0
			&& shell->env[i][len] == '=')
			return (ft_strdup(shell->env[i] + len + 1));
		i++;
	}
	i = 0;
	while (shell->local_vars[i])
	{
		if (ft_strncmp(shell->local_vars[i], var_name, len) == 0
			&& shell->local_vars[i][len] == '=')
			return (ft_strdup(shell->local_vars[i] + len + 1));
		i++;
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

char	*handle_single_quotes(char *str, int *i)
{
	int		start;
	char	*quoted_content;

	++(*i);
	start = *i;
	while (str[*i] && str[*i] != '\'')
		(*i)++;
	quoted_content = ft_substr(str, start, *i - start);
	if (!quoted_content)
		return (NULL);
	if (str[*i] == '\'')
		(*i)++;
	return (quoted_content);
}
