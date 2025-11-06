/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser_parser_main4.c                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: axgimene <axgimene@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/25 09:26:02 by axgimene          #+#    #+#             */
/*   Updated: 2025/11/06 19:11:47 by axgimene         ###   ########.fr       */
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
    {
        free_commands(current_cmd);
        *current_cmd = NULL;
    }
}

static void	handle_pipe_token(t_token **current_token, t_cmd **current_cmd)
{
    t_cmd	*new_cmd;

    set_builtin_flag(*current_cmd);
    if (pipe((*current_cmd)->pipe) == -1)
    {
        perror("pipe");
        free_commands(current_cmd);
        *current_cmd = NULL;
        return ;
    }
    new_cmd = create_command();
    if (!new_cmd)
    {
        close((*current_cmd)->pipe[0]);
        close((*current_cmd)->pipe[1]);
        free_commands(current_cmd);
        *current_cmd = NULL;
        return ;
    }
    if ((*current_cmd)->out_fd == -1)
        (*current_cmd)->out_fd = (*current_cmd)->pipe[1];
    new_cmd->in_fd = (*current_cmd)->pipe[0];
    (*current_cmd)->next = new_cmd;
    *current_cmd = new_cmd;
    *current_token = (*current_token)->next;
}

static void	process_token_in_parser(t_token **current_token, t_cmd **current_cmd)
{
    if ((*current_token)->type == T_WORD)
    {
        add_arg_to_command(*current_cmd, (*current_token)->value);
        *current_token = (*current_token)->next;
    }
    else if ((*current_token)->type == T_PIPE)
    {
        handle_pipe_token(current_token, current_cmd);
    }
    else if (is_redirection_token((*current_token)->type))
        handle_redirection_token(current_token, current_cmd);
    else
        *current_token = (*current_token)->next;
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
        process_token_in_parser(&current, &current_cmd);
        if (!current_cmd)
            return (free_commands(&head), NULL);
    }
    set_builtin_flag(current_cmd);
    return (head);
}
