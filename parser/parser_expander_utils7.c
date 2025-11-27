/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser_expander_utils7.c                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: axgimene <axgimene@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/30 12:00:50 by axgimene          #+#    #+#             */
/*   Updated: 2025/11/27 17:28:47 by axgimene         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

static char	*handle_regular_char(char *str, int *i)
{
    char	*part;

    part = ft_substr(str, *i, 1);
    (*i)++;
    if (!part)
        return (NULL);
    return (part);
}

static char	*process_char_in_expansion(t_shell *shell, char *str, int *i)
{
    if (str[*i] == '\'')
        return (handle_single_quotes(str, i));
    else if (str[*i] == '"')
        return (handle_double_quotes(shell, str, i));
    else if (str[*i] == '$' && str[*i + 1]
        && !is_dollar_terminator(str[*i + 1]))
        return (expand_dollar(shell, str, i));
    else
        return (handle_regular_char(str, i));
}

// ✅ Añade esta función aquí
static char	*remove_quotes_from_token(char *token)
{
    char	*result;
    int		len;
    int		start;
    int		end;

    if (!token || token[0] == '\0')
        return (ft_strdup(""));
    len = ft_strlen(token);
    start = 0;
    end = len;
    if (len >= 2 && ((token[0] == '"' && token[len - 1] == '"') || 
        (token[0] == '\'' && token[len - 1] == '\'')))
    {
        start = 1;
        end = len - 1;
    }
    result = malloc(end - start + 1);
    if (!result)
        return (NULL);
    ft_strlcpy(result, token + start, end - start + 1);
    return (result);
}

void	expand_variables(t_token *tokens)
{
    t_token	*current;
    char	*expanded;

    current = tokens;
    while (current)
    {
        if (current->value)
        {
            expanded = remove_quotes_from_token(current->value);
            free(current->value);
            current->value = expanded;
        }
        current = current->next;
    }
}

char	*expand_string(t_shell *shell, char *str)
{
    int		i;
    char	*result;
    char	*part;
    char	*new_result;

    if (!str || !str[0])
        return (ft_strdup(""));
    i = 0;
    result = ft_strdup("");
    if (!result)
        return (NULL);
    while (str[i])
    {
        part = process_char_in_expansion(shell, str, &i);
        if (!part)
            continue;
        new_result = ft_strjoin(result, part);
        free(part);
        if (!new_result)
        {
            free(result);
            return (NULL);
        }
        free(result);
        result = new_result;
    }
    return (result);
}
