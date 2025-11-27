/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser_parser_main4.c                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: axgimene <axgimene@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/25 09:26:02 by axgimene          #+#    #+#             */
/*   Updated: 2025/11/27 20:01:43 by axgimene         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

static int	process_word_token(t_token *current_token, t_cmd *current_cmd)
{
    if (!current_cmd || current_token)
        return (0);
    add_arg_to_command(current_cmd, current_token->value);
    return (1);
}

static int	process_pipe_token(t_token **current_token, t_cmd **current_cmd)
{
    t_cmd	*new_cmd;

    (void)current_token;
    if(!current_cmd || !*current_cmd)
        return (0);
    new_cmd = create_command();
    if (!new_cmd)
        return (0);
    if(!setup_pipe_fds(*current_cmd, new_cmd))
    {
        free(new_cmd);
        return (0);
    }
    (*current_cmd)->next = new_cmd;
    *current_cmd = new_cmd;
    return (1);
}

static int	process_redirection(t_token **current_token, t_cmd *current_cmd)
{
    if (!current_token || !*current_token || !current_cmd)
        return (0);
    
    if (!handle_redirection(current_token, current_cmd))
    {
        ft_putstr_fd("minishell: syntax error near redirection\n", 2);
        return (0);
    }
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
            // ✅ Crea comando solo si no existe
            if (!current_cmd)
            {
                current_cmd = create_command();
                if (!current_cmd)
                {
                    free_commands(&head);
                    return (NULL);
                }
                if (!head)
                    head = current_cmd;
            }
            if (!process_word_token(current_token, current_cmd))
            {
                free_commands(&head);
                return (NULL);
            }
            current_token = current_token->next;
        }
        else if (current_token->type == T_PIPE)
        {
            if (!current_cmd || !current_cmd->av || !current_cmd->av[0])
            {
                ft_putstr_fd("minishell: syntax error near `|'\n", 2);
                free_commands(&head);
                return (NULL);
            }
            set_builtin_flag(current_cmd);
            if (!process_pipe_token(&current_token, &current_cmd))
            {
                free_commands(&head);
                return (NULL);
            }
            current_token = current_token->next;
        }
        else if (is_redirection_token(current_token->type))
        {
            if (!process_redirection(&current_token, current_cmd))
            {
                free_commands(&head);
                return (NULL);
            }
            // ✅ NO avanza current_token aquí (handle_redirection ya lo hace)
        }
        else
        {
            // ✅ Token desconocido - avanza y continúa
            current_token = current_token->next;
        }
    }
    
    // ✅ Valida que el último comando sea válido
    if (!current_cmd || !current_cmd->av || !current_cmd->av[0])
    {
        if (current_cmd)
            free_commands(&head);
        return (NULL);
    }
    
    // ✅ Marca el último comando como builtin si aplica
    set_builtin_flag(current_cmd);
    
    return (head);
}
