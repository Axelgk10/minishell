/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser_expander_utils7.c                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: axgimene <axgimene@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/30 12:00:50 by axgimene          #+#    #+#             */
/*   Updated: 2025/11/05 11:33:35 by axgimene         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

static char	*handle_regular_char(char *str, int *i)
{
    char	*part;

    part = ft_substr(str, *i, 1);
    if (!part)
        return (ft_strdup(""));
    (*i)++;
    return (part);
}

char	*handle_double_quotes(t_shell *shell, char *str, int *i)
{
    int		start;
    char	*quoted_content;
    char	*expanded;

    ++(*i);
    start = (*i);
	
    while (str[*i] && str[*i] != '"')
        (*i)++;
    
    quoted_content = ft_substr(str, start, *i - start);
    if (!quoted_content)
        return (NULL);
    
    expanded = expand_string(shell, quoted_content);
    free(quoted_content);
    
    if (str[*i] == '"')
        (*i)++;
    
    return (expanded);
}

static char	*process_char_in_expansion(t_shell *shell, char *str, int *i)
{
    if (str[*i] == '\'')
        return (handle_single_quotes(str, i));
    else if (str[*i] == '"')
        return (handle_double_quotes(shell, str, i));
    else if (str[*i] == '$' && str[*i + 1] && !is_dollar_terminator(str[*i + 1]))
        return (expand_dollar(shell, str, i));
    else
        return (handle_regular_char(str, i));
}

char	*expand_string(t_shell *shell, char *str)
{
    int		i;
    char	*result;
    char	*temp;
    char	*part;

    i = 0;
    result = ft_strdup("");
    
    while (str && str[i])
    {
        part = process_char_in_expansion(shell, str, &i);
        if (!part)
            part = ft_strdup("");
        
        temp = result;
        result = ft_strjoin(result, part);
        free(temp);
        free(part);
    }

    return (result);
}

void	expand_variables(t_shell *shell, t_token *tokens)
{
    t_token	*current;
    char	*expanded;

    current = tokens;
    while (current)
    {
        if (current->type == T_WORD)
        {
            expanded = expand_string(shell, current->value);
            
            if (expanded)
            {
                free(current->value);
                current->value = expanded;
            }
        }
        current = current->next;
    }
}