/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser_parser_main4.c                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: axgimene <axgimene@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/25 09:26:02 by axgimene          #+#    #+#             */
/*   Updated: 2025/11/10 19:55:21 by axgimene         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

static int	is_redirection_token(t_token_type type)
{
    return (type >= T_REDIR_IN && type <= T_HEREDOC);
}

static void	handle_redirection_token(t_token **current_token, t_cmd **current_cmd)
{
    if (!handle_redirection(current_token, *current_cmd))
        *current_cmd = NULL;
}

static int	is_valid_redir_sequence(t_token *token)
{
    if (!token)
        return (1);
    if (is_redirection_token(token->type))
    {
        if (!token->next || token->next->type != T_WORD)
        {
            write(2, "minishell: syntax error near unexpected token `newline'\n", 57);
            return (0);
        }
    }
    return (1);
}

static int	handle_pipe_token(t_token **current_token, t_cmd **current_cmd)
{
    t_cmd	*new_cmd;

    if (!(*current_cmd)->av || !(*current_cmd)->av[0])
    {
        ft_putstr_fd("minishell: syntax error near unexpected token `|'\n", 2);
        return (0);
    }
    set_builtin_flag(*current_cmd);
    if (pipe((*current_cmd)->pipe) == -1)
    {
        perror("pipe");
        return (0);
    }
    new_cmd = create_command();
    if (!new_cmd)
    {
        close((*current_cmd)->pipe[0]);
        close((*current_cmd)->pipe[1]);
        return (0);
    }
    if ((*current_cmd)->out_fd == -1)
        (*current_cmd)->out_fd = (*current_cmd)->pipe[1];
    new_cmd->in_fd = (*current_cmd)->pipe[0];
    (*current_cmd)->next = new_cmd;
    *current_cmd = new_cmd;
    *current_token = (*current_token)->next;
    return (1);
}

static int	process_token_in_parser(t_token **current_token, t_cmd **current_cmd)
{
    if (*current_cmd == NULL)
        return (0);
    if ((*current_token)->type == T_WORD)
    {
        add_arg_to_command(*current_cmd, (*current_token)->value);
        *current_token = (*current_token)->next;
        return (1);
    }
    else if ((*current_token)->type == T_PIPE)
        return (handle_pipe_token(current_token, current_cmd));
    else if (is_redirection_token((*current_token)->type))
    {
        if (!is_valid_redir_sequence(*current_token))
        {
            *current_cmd = NULL;
            return (0);
        }
        handle_redirection_token(current_token, current_cmd);
        return (*current_cmd != NULL);
    }
    else
    {
        *current_token = (*current_token)->next;
        return (1);
    }
}

t_cmd	*parse_tokens(t_token **tokens)
{
    t_cmd	*head;
    t_cmd	*current_cmd;
    t_token	*current;

    if (!tokens || !*tokens)
        return (NULL);
    current = *tokens;
    head = NULL;
    init_first_command(&head, &current_cmd);
    if (!current_cmd)
        return (NULL);
    while (current)
    {
        if (!process_token_in_parser(&current, &current_cmd))
        {
            free_commands(&head);
            return (NULL);
        }
    }
    if (!current_cmd || !current_cmd->av || !current_cmd->av[0])
    {
        write(2, "minishell: syntax error near unexpected token `newline'\n", 57);
        free_commands(&head);
        return (NULL);
    }
    set_builtin_flag(current_cmd);
    return (head);
}
