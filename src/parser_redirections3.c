/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser_redirections3.c                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: axgimene <axgimene@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/08 20:00:23 by axgimene          #+#    #+#             */
/*   Updated: 2025/11/03 16:34:06 by axgimene         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"


static int	handle_heredoc(char *delimiter)
{
	int		fd;
	char	*line;

	fd = open("/tmp/heredoc", O_WRONLY | O_CREAT | O_TRUNC, 0644);
	if (fd == -1)
		return (perror ("heredoc"), -1);
	while (1)
	{
		line = readline("> ");
		if (!line || ft_strcmp(line, delimiter, ft_strlen(delimiter)) == 0)
		{
			free(line);
			break ;
		}
		write(fd, line, ft_strlen(line));
		write(fd, "\n", 1);
		free(line);
	}
	close(fd);
	fd = open("/tmp/heredoc", O_RDONLY);
	if (fd == -1)
		perror("minishell: heredoc");
	return (fd);
}

static int	handle_input_redirection(char *filename)
{
	int	fd;
	
	fd = open(filename, O_RDONLY);
	if (fd == -1)
	{
		perror("minishell");
		return (-1);
	}
	return (fd);
}

static int	handle_output_redirection(char *filename)
{
	int	fd;

	fd = open(filename, O_WRONLY | O_CREAT | O_TRUNC, 0644);
	if (fd == -1)
	{
		perror("minishell");
		return (-1);
	}
	return (fd);
}

static int	handle_append_redirection(char *filename)
{
	int	fd;

	fd = open(filename, O_WRONLY | O_CREAT | O_APPEND, 0644);
	if (fd == -1)
	{
		perror("minishell");
		return (-1);
	}
	return (fd);
}

int	handle_redirection(t_token **tokens, t_cmd *cmd)
{
	t_token_type	type;
	char			*filename;

	if (!tokens || !*tokens || !cmd)
		return (0);
	type = (*tokens)->type;
	*tokens = (*tokens)->next;
	if (!*tokens || (*tokens)->type != T_WORD)
		return (0);
	filename = (*tokens)->value;
	if (type == T_REDIR_IN)
	{
		if (cmd->in_fd >= 0)
			close(cmd->in_fd);
		cmd->in_fd = handle_input_redirection(filename);
	}
	else if (type == T_REDIR_OUT)
	{
		if (cmd->out_fd >= 0)
			close(cmd->out_fd);
		cmd->out_fd = handle_output_redirection(filename);
	}
	else if (type == T_APPEND)
	{
		if (cmd->out_fd >= 0)
			close(cmd->out_fd);
		cmd->out_fd = handle_append_redirection(filename);
	}
	else if (type == T_HEREDOC)
	{
		if (cmd->in_fd >= 0)
			close(cmd->in_fd);
		cmd->in_fd = handle_heredoc(filename);
	}
	*tokens = (*tokens)->next;
	return (1);
}
