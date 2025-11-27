#include "minishell.h"

int g_exit_status = 0;
t_shell *g_shell = NULL;

// ✅ Función de limpieza automática al salir
void	cleanup_on_exit(void)
{
    if (g_shell)
        cleanup_shell(g_shell);
}

void	free_shell_after_execution(t_shell *shell)
{
	if (!shell)
		return ;
	if (shell->tokens)
	{
		free_tokens(&shell->tokens);
		shell->tokens = NULL;
	}
	if (shell->commands)
	{
		free_commands(&shell->commands);
		shell->commands = NULL;
	}
}

static void	evaluate_struct(t_shell *shell)
{
	if (!shell || !shell->commands)
		return ;
	if (!shell->commands->next)
	{
		if(shell->commands->is_builtin)
			execute_builtin(shell);
		else
			just_execute_it_man(shell);
	}
	else if (shell->commands->next)
		execute_pipeline(shell, shell->commands);
}

int	check_unclosed_quotes(char *input)
{
	int	i;
	int	single_count;
	int	double_count;

	i = 0;
	single_count = 0;
	double_count = 0;
	while (input[i])
	{
		if (input[i] == '\'' && (i == 0 || input[i - 1] != '\\'))
			single_count++;
		else if (input[i] == '"' && (i == 0 || input[i - 1] != '\\'))
			double_count++;
		i++;
	}
	if (single_count % 2 != 0)
	{
		write(STDERR_FILENO, "minishell: unclosed single quote\n", 33);
		return (1);
	}
	if (double_count % 2 != 0)
	{
		return (write(STDERR_FILENO, "minishell: unclosed double quote\n", 33), 1);
	}
	return (0);
}

static void	process_input(t_shell	*shell, char *input)
{
    if (check_unclosed_quotes(input))
    {
        free(input);
        return ;
    }
    add_history(input);
    free_shell_after_execution(shell);
    shell->tokens = tokenize(input);
    if (shell->tokens)
    {
        expand_variables(shell->tokens);  // ✅ Sin shell
        shell->commands = parse_tokens(shell->tokens);
        if (shell->commands)
        {
            evaluate_struct(shell);
        }
        free_shell_after_execution(shell);
    }
    free(input);
}

int	main(int argc, char **argv, char **envp)
{
    char	*input;
    t_shell	shell;

    (void)argc;
    (void)argv;
    ft_memset(&shell, 0, sizeof(t_shell));
    g_shell = &shell;
    atexit(cleanup_on_exit);
    init_shell(&shell, envp);
    init_signals();
    while (1)
    {
        input = readline(shell.prompt);
        if (!input)
        {
            ft_putstr_fd("exit\n", STDOUT_FILENO);
            break ;
        }
        process_input(&shell, input);
        free(input);
        // ✅ Verifica si fue exit
        if (shell.commands && !ft_strcmp(shell.commands->av[0], "exit"))
            break ;
    }
    return (g_exit_status);
}
