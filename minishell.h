/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: axgimene <axgimene@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/04 09:21:14 by axgimene          #+#    #+#             */
/*   Updated: 2025/11/06 13:07:59 by axgimene         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MINISHELL_H
# define MINISHELL_H

# include <stdio.h>
# include <readline/readline.h>
# include <readline/history.h>
# include <stdlib.h>
# include <unistd.h>
# include <signal.h>
# include <sys/wait.h>
# include <sys/stat.h>
# include <dirent.h>
# include <termios.h>
# include <fcntl.h>
# include <errno.h>
# include "./libft/libft.h"

// Global variable for signals (as required)
extern volatile sig_atomic_t	g_signal;

// Token types
typedef enum e_token_type
{
	T_WORD,
	T_PIPE,
	T_REDIR_IN,
	T_REDIR_OUT,
	T_APPEND,
	T_HEREDOC,
	T_EOF
}	t_token_type;

// typedef	struct s_token
// {
// 	char	*str;
// 	int		type;
// }	t_token;

// typedef	struct s_cmd
// {
// 	char	*argv;
// 	int		infile;
// 	int		outfile;
// }	t_cmd;

// t_token *tkn = malloc(1 * sizeof(t_token));

// ft_lstnew(tkn);
// ft_lstnew(cmd);

// (t_token *) lst_tkn.content;
// (t_cmd	*)	lst_cmd.content;

// typedef struct s_list
// {
// 	void	*content;
// 	t_list	*next;
// }	t_list;


// Token structure
typedef struct s_token
{
	t_token_type	type;
	char			*value;
	struct s_token	*next;
}	t_token;

// Command structure
typedef struct s_cmd
{
	char			**as;
	char			**av;
	int				in_fd;
	int				out_fd;
	int				pipe[2];
	int				is_builtin;
	struct s_cmd	*next;
}	t_cmd;

// Main shell structure
typedef struct s_shell
{
	t_token		*tokens;
	t_cmd		*commands;
	char		**env;
	int			exit_status;
	int			stdin_copy;
	int			stdout_copy;
}	t_shell;

// Function prototypes
// Main
void	init_shell(t_shell *shell, char **env);
void	cleanup_shell(t_shell *shell);
// Parser tokenizer
int	is_metachar(char c);
t_token_type	get_token_type(char *str);
t_token	*tokenize(char *input);
t_cmd	*parse_tokens(t_token **tokens);
void	expand_variables(t_shell *shell, t_token *tokens);
char	*expand_string(t_shell *shell, char *str);
char	*handle_single_quotes(char *str, int *i);
char	*handle_double_quotes(t_shell *shell, char *str, int *i);
char	*extract_word(char *input, int *i);
char	*extract_metachar(char *input, int *i);
int		handle_redirection(t_token **tokens, t_cmd *cmd);
void	set_builtin_flag(t_cmd *cmd);
t_cmd	*create_command(void);
void	add_arg_to_command(t_cmd *cmd, char *arg);
void	init_first_command(t_cmd **head, t_cmd **current_cmd);
void	handle_pipe_in_parse(t_token **token, t_cmd **cmd);
void	handle_redir_in_parse(t_token **token, t_cmd **cmd);
char	*expand_dollar(t_shell *shell, char *str, int *i);
int		is_dollar_terminator(char c);
int		handle_append_redirection(char *filename);
int		handle_output_redirection(char *filename);
int		handle_input_redirection(char *filename);
int		handle_heredoc(char *delimiter);
void	handle_quotes_in_token(char *input, int *i, char *quote);


// Execution functions
int	execute_commands(t_shell *shell);
int	execute_single_command(t_shell *shell, t_cmd *cmd);
int	execute_pipeline(t_shell *shell, t_cmd *commands);
int	execute_builtin(t_shell *shell, t_cmd *cmd);

// Builtins
int	builtin_echo(t_cmd *cmd);
int	builtin_pwd(void);
int	builtin_env(t_shell *shell);
int	builtin_cd(t_shell *shell, t_cmd *cmd);
int	builtin_exit(t_shell *shell, t_cmd *cmd);
int	builtin_export(t_shell *shell, t_cmd *cmd);
int	builtin_unset(t_shell *shell, t_cmd *cmd);

// Signals
void	setup_signals(void);
void	handle_sigint(int sig);
void	handle_sigquit(int sig);
// Utils
char	*find_executable(char *cmd, char **env);
void	free_tokens(t_token **tokens);
void	free_commands(t_cmd **commands);
char	**copy_env(char **env);
void	print_error(char *cmd, char *msg);

#endif