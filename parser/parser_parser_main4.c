/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser_parser_main4.c                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: axgimene <axgimene@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/25 09:26:02 by axgimene          #+#    #+#             */
/*   Updated: 2025/11/27 18:39:06 by axgimene         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

static int	process_word_token(t_token *current_token, t_cmd *current_cmd)
{
    if (!current_cmd)
        return (0);
    add_arg_to_command(current_cmd, current_token->value);
    return (1);
}

static int	process_pipe_token(t_token **current_token, t_cmd **current_cmd)
{
    t_cmd	*new_cmd;

    (void)current_token;
    new_cmd = create_command();
    if (!new_cmd)
        return (0);
    setup_pipe_fds(*current_cmd, new_cmd);
    (*current_cmd)->next = new_cmd;
    *current_cmd = new_cmd;
    return (1);
}

t_cmd	*parse_tokens(t_token *tokens)
{
    t_cmd	*head;
    t_cmd	*current_cmd;
    t_token	*current_token;

    head = NULL;
    current_cmd = NULL;
    current_token = tokens;
    while (current_token)
    {
        if (current_token->type == T_WORD)
        {
            if (!current_cmd)
            {
                current_cmd = create_command();
                if (!head)
                    head = current_cmd;
            }
            if (!process_word_token(current_token, current_cmd))
                return (NULL);
            current_token = current_token->next;
        }
        else if (current_token->type == T_PIPE)
        {
            set_builtin_flag(current_cmd);  // ✅ Marca ANTES de crear nuevo comando
            if (!process_pipe_token(&current_token, &current_cmd))
                return (NULL);
            current_token = current_token->next;
        }
        else if (current_token->type == T_REDIR_IN || 
                current_token->type == T_REDIR_OUT || 
                current_token->type == T_APPEND || 
                current_token->type == T_HEREDOC)
        {
            if (!handle_redirection(&current_token, current_cmd))
                return (NULL);
        }
        else
        {
            current_token = current_token->next;
        }
    }
    // ✅ CRÍTICO: Marca el último comando como builtin si aplica
    if (current_cmd)
        set_builtin_flag(current_cmd);
    return (head);
}
