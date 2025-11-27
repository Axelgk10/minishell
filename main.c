#include "minishell.h"

int g_exit_status = 0;
t_shell *g_shell = NULL;

// ✅ Función de limpieza automática al salir - Consolidada
static void	cleanup_on_exit(void)
{
	if (g_shell)
	{
		// Liberar environment
		if (g_shell->env)
		{
			int i = 0;
			int count = g_shell->env_count;
			
			// ✅ Si env_count es 0 pero env está asignado, contar hasta NULL
			if (count == 0)
			{
				while (g_shell->env[i] != NULL)
					i++;
				count = i;
			}
			
			// ✅ Liberar todos los strings
			i = 0;
			while (i < count)
			{
				if (g_shell->env[i])
				{
					free(g_shell->env[i]);
					g_shell->env[i] = NULL;
				}
				i++;
			}
			free(g_shell->env);
			g_shell->env = NULL;
		}
		
		// Liberar otros campos
		if (g_shell->prompt)
		{
			free(g_shell->prompt);
			g_shell->prompt = NULL;
		}
		if (g_shell->tokens)
		{
			free_tokens(&g_shell->tokens);
			g_shell->tokens = NULL;
		}
		if (g_shell->commands)
		{
			free_commands(&g_shell->commands);
			g_shell->commands = NULL;
		}
		if (g_shell->local_vars)
		{
			// TODO: implement local_vars cleanup if needed
		}
		
		// ✅ Free the shell structure itself (was allocated on heap)
		free(g_shell);
		g_shell = NULL;
	}
	rl_clear_history();
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
    
    // Libera memoria previa
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
    
    shell->tokens = tokenize(input);
    if (shell->tokens)
    {
        expand_variables(shell, shell->tokens);
        shell->commands = parse_tokens(shell->tokens);
        if (shell->commands)
        {
            evaluate_struct(shell);
        }
    }
    
    // Libera después de ejecutar
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
    free(input);
}

int	main(int argc, char **argv, char **envp)
{
    char	*input;
    t_shell	*shell;
    char    cwd_buffer[1024];

    (void)argc;
    (void)argv;
    
    // ✅ Allocate shell on heap instead of stack
    shell = (t_shell *)malloc(sizeof(t_shell));
    if (!shell)
    {
        ft_putstr_fd("Error: malloc failed for shell structure\n", STDERR_FILENO);
        return (1);
    }
    
    g_shell = shell;
    // ✅ Registra función de limpieza automática ANTES de cualquier inicialización
    atexit(cleanup_on_exit);
    init_shell(shell, envp);
    init_signals();
    while (1)
    {
        // ✅ LIBERA EL PROMPT ANTERIOR ANTES DE REASIGNAR
        if (shell->prompt)
        {
            free(shell->prompt);
            shell->prompt = NULL;
        }
        
        if (getcwd(cwd_buffer, sizeof(cwd_buffer)) != NULL)
            shell->prompt = format_cwd(cwd_buffer);
        else
            shell->prompt = ft_strdup("Minishell$ ");
        
        input = readline(shell->prompt);
        
        if (!input)
        {
            // ✅ Libera el prompt antes de salir
            if (shell->prompt)
            {
                free(shell->prompt);
                shell->prompt = NULL;
            }
            null_input();
        }
        if (input && *input)
            process_input(shell, input);
        else
        {
            free(input);
            input = NULL;
        }
    }
    // ✅ Esta línea nunca se alcanza, pero por seguridad
    cleanup_shell(shell);
    free(shell);
    return (0);
}
