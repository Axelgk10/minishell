/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gguardam <gguardam@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/05 15:24:21 by axgimene          #+#    #+#             */
/*   Updated: 2025/12/10 12:13:50 by gguardam         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MINISHELL_H
# define MINISHELL_H
# define _POSIX_C_SOURCE 200809L //Signals, chdir, cwd

# include <sys/stat.h> //Ax
# include <dirent.h> //Ax
# include <termios.h> //Ax
# include <fcntl.h> //Ax
# include <errno.h> //Ax
# include <stdio.h> //Printf
# include <stdlib.h> //Free Malloc
# include <readline/readline.h> //Readline
# include <readline/history.h> //AddHistory
# include <unistd.h> //getcwd
# include <sys/types.h> //PID type
# include <sys/wait.h> //wait functions
# include <signal.h> //Sigaction
# include "libft.h/libft.h"

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
	char			**av;
	int				ac;
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
	char		**local_vars;
	char		*prompt;
	char		*logical_pwd;
	int			exit_status;
	int			stdin_copy;
	int			stdout_copy;
	int			env_count;
}	t_shell;

// Pipe data structure for execute_pipeline
typedef struct s_pipe_data
{
	int		pipe_fd[2];
	int		prev_pipe_out;
	int		i;
	pid_t	*pids;
}	t_pipe_data;

extern int	g_exit_status;

//dir_manager
int				change_directory(t_shell *shell, char *path);
char			*format_cwd(char *cwd);
char			*get_home_shortcut(char *cwd);
void			update_pwd_after_cd(t_shell *shell);

//dir_helpers
int				handle_home_cd(t_shell *shell);
int				handle_oldpwd_cd(t_shell *shell);
int				handle_tilde_cd(t_shell *shell, char *path);
int				handle_getcwd_error(t_shell *shell, char *path);
int				handle_relative_cd(t_shell *shell, char *path);

//path_utils
char			*create_tilde_result(char *cwd, char *home, size_t home_len, \
size_t cwd_len);
char			*get_env_value_from_shell(t_shell *shell, char *var_name);
char			*process_parent_directory(char *resolved);
char			*handle_dotdot_path(char *resolved, char **remaining_path);
char			*resolve_logical_path(char *logical_pwd, char *path);

//commands
int				ft_echo(t_cmd *command);
int				ft_env(t_shell *shell);
int				ft_pwd(t_shell *shell, t_cmd *command);
void			update_envs(t_shell *shell);
void			manage_exit(t_shell *shell);
void			ft_export_env(t_shell *shell);

//execution
void			just_execute_it_man(t_shell *shell);
void			execute_builtin(t_shell *shell);
int				execute_pipeline(t_shell *shell, t_cmd *commands);

//execution piped builtins
int				execute_builtin_cd_pwd_exit(t_shell *shell, t_cmd *cmd);
int				execute_builtin_env_echo_export(t_shell *shell, t_cmd *cmd);
int				execute_builtin_in_pipeline(t_shell *shell, t_cmd *cmd);

//execution piped child
void			execute_child_process(t_shell *shell,
					t_cmd *cmd, t_pipe_data *data);

//execution utils
void			status_wait(pid_t pid, int status);
void			fd_checker(t_shell *shell);
int				create_pipe_if_needed(t_cmd *current, int *pipe_fd);
void			cleanup_path_env(char **path_env);
void			fd_redirections(int prev_fd, t_cmd	*current, int *pipe_fd);
void			wait_for_childs(t_shell *shell);
char			*find_binary(char *command, char **paths);
int				got_path(t_shell *shell);

//Utils main
void			null_input(t_shell *shell);
int				check_unclosed_quotes(char *input);
void			process_input(t_shell *shell, char *input);

//Utils Errors
void			error_executing\
(int site_of_error, char **env, char **cmd_params);
int				write_error_message(int fd, char *cmd, char *arg, char *msg);

//Utils Validation
int				check_pipe_at_start(t_shell *shell, char *input);

//Utils Envs
int				export_variables(t_shell *shell);
int				unset_variables(t_shell *shell);
int				is_valid_var_name(char *name);
int				process_export_arg(t_shell *shell, char *arg);
int				find_variable_index(char **env_var,
					char *var_name, int name_len);
int				count_env_vars(char **env_var);
void			del_var(t_shell *shell);
int				free_memory(char *modified_arg,
					char *var_name, char *var_value);
int				set_local_var(t_shell *shell);
char			*move_local_var_to_env(t_shell *shell, char *var_name);

//Utils Envs Export
int				add_or_append(t_shell *shell, int i);
int				check_append_flag(char *str);
int				var_exists(t_shell *shell, char *var_name, int name_len);
char			*process_plus_append(char *var_name, char *arg);
void			cleanup_export_vars(char *var_name, char *new_var, \
char *local_var, char *arg);
int				find_env_index(char **env, char *mod_env_name);

//Utils Envs Helpers
void			remove_from_array(char **array, int index);
int				var_matches(char *env_var, char *var_name, int name_len);
int				do_append(t_shell *shell, char *arg, char *mod_env_name, int i);
int				find_env_index(char **env, char *mod_env_name);
void			add_var_to_env(t_shell *shell, char *arg, char *var_name, \
char *new_var);

//Utils Local Envs
char			*create_assignment_from_append(char *arg);
int				should_append(char	*arg);

//Execution Utils
char			**get_path_values(char **env, const char *var_name);

//Utils Export
//char	*create_var_without_value(char *var_assignment, int *name_len);
char			*create_var_with_value(char *var_assignment, char *equals_pos,
					char **var_name, int *name_len);
void			update_env_array(char **env_var, char *new_var, char *var_name,
					int name_len);
void			add_or_modify_var(char **env_var, char *var_assignment);
char			*extract_var_name(char *var_assignment, char *equals_pos);
char			*extract_var_value(char *var_assignment);

//signals
void			sigint_handler(int sig);
void			sigint_handler_cmd(int sig);
void			set_signals_for_cmd(void);
void			restore_signals(void);
void			init_signals(void);

//Struct Utils
void			init_shell(t_shell *shell, char **envp);
void			cleanup_command_fds(t_cmd *cmd);
void			cleanup_shell(t_shell *shell);
void			check_struct(t_shell *shell);

// ============================================================================
// PARSER FUNCTIONS
// ============================================================================
// Pipeline: INPUT STRING → [TOKENIZE] → TOKENS → [EXPAND] → [PARSE] → CMDS
// ============================================================================

// ────────────────────────────────────────────────────────────────────────────
// TOKENIZACIÓN - Convierte input string en lista de tokens
// ────────────────────────────────────────────────────────────────────────────
t_token			*tokenize(char *input);
t_token_type	get_token_type(char *str);
int				is_metachar(char c);
char			*extract_word(char *input, int *i);
char			*extract_metachar(char *input, int *i);
void			handle_quotes_in_token(char *input, int *i, char *quote);
int				check_unclosed_quotes(char *input);

// ────────────────────────────────────────────────────────────────────────────
// EXPANSIÓN - Procesa variables ($VAR) y elimina comillas
// ────────────────────────────────────────────────────────────────────────────
void			expand_variables(t_shell *shell, t_token *tokens);
char			*expand_string(t_shell *shell, char *str);
char			*expand_dollar(t_shell *shell, char *str, int *i);
char			*handle_single_quotes(char *str, int *i);
char			*handle_double_quotes(t_shell *shell, char *str, int *i);
char			*process_double_quote_content(t_shell *shell,
					char *str, int *i);
char			*handle_single_quote_in_double(char *result, int *i);
char			*handle_dollar_in_double(t_shell *shell, char *result, \
char *str, int *i);
char			*handle_regular_in_double(char *result, char *str, int *i);
int				is_dollar_terminator(char c);
char			*join_and_free(char *result, char *part);

// ────────────────────────────────────────────────────────────────────────────
// PARSING - Convierte tokens en comandos ejecutables
// ────────────────────────────────────────────────────────────────────────────
t_cmd			*parse_tokens(t_token *tokens);
t_cmd			*create_command(void);
void			init_first_command(t_cmd **head, t_cmd **current_cmd);
void			add_arg_to_command(t_cmd *cmd, char *arg);
void			set_builtin_flag(t_cmd *cmd);
int				handle_pipe_token(t_token **current_token, t_cmd **current_cmd);
int				setup_pipe_fds(t_cmd *current_cmd, t_cmd *new_cmd);
void			handle_pipe_in_parse(t_token **token, t_cmd **cmd);
int				handle_redirection(t_token **tokens, t_cmd *cmd);
int				handle_input_redirection(char *filename);
int				handle_output_redirection(char *filename);
int				handle_append_redirection(char *filename);
int				handle_heredoc(char *delimiter);
void			handle_redir_in_parse(t_token **token, t_cmd **cmd);
void			print_redir_error(t_token *filename_token);
int				is_redirection_token(t_token_type type);
int				is_valid_redir_sequence(t_token *token);
int				validate_final_command(t_cmd *current_cmd);
void			free_command_args_and_fds(t_cmd *cmd);

// Signals
void			setup_signals(void);
void			handle_sigint(int sig);
void			handle_sigquit(int sig);

// Utils
char			*find_executable(char *cmd, char **env);
void			free_tokens(t_token **tokens);
void			free_commands(t_cmd **commands);
char			**copy_env(char **env);
void			print_error(char *cmd, char *msg);

// Cleanup functions
void			cleanup_shell(t_shell *shell);
void			free_shell_after_execution(t_shell *shell);

#endif