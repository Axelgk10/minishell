	// /* ************************************************************************** */
	// /*                                                                            */
	// /*                                                        :::      ::::::::   */
	// /*   main.c                                             :+:      :+:    :+:   */
	// /*                                                    +:+ +:+         +:+     */
	// /*   By: axgimene <axgimene@student.42.fr>          +#+  +:+       +#+        */
	// /*                                                +#+#+#+#+#+   +#+           */
	// /*   Created: 2025/10/04 09:30:18 by axgimene          #+#    #+#             */
	// /*   Updated: 2025/11/03 12:43:36 by axgimene         ###   ########.fr       */
	// /*                                                                            */
	// /* ************************************************************************** */

	// #include "../minishell.h"

	// volatile sig_atomic_t	g_signal = 0;

	// // static void	read_input(t_shell *shell)
	// // {
	// // 	char	*input;

	// // 	setup_signals();
	// // 	input = readline("minishell$ ");
	// // 	if (g_signal == SIGINT)
	// // 	{
	// // 		shell->exit_status = 130;
	// // 		g_signal = 0;
	// // 	}
	// // 	if (!input)
	// // 	{
	// // 		printf("exit\n");
	// // 		cleanup_shell(shell);
	// // 		exit(shell->exit_status);
	// // 	}
	// // 	if (*input)
	// // 		add_history(input);
	// // 	shell->tokens = tokenize(input);
	// // 	free(input);
	// // }

	// // static void	process_input(t_shell *shell)
	// // {
	// // 	if (!shell->tokens)
	// // 		return ;
	// // 	expand_variables(shell, shell->tokens);
	// // 	shell->commands = parse_tokens(&shell->tokens);
	// // 	if (shell->commands)
	// // 	{
	// // 		shell->stdin_copy = dup(STDIN_FILENO);
	// // 		shell->stdout_copy = dup(STDOUT_FILENO);
	// // 		execute_commands(shell);
	// // 		restore_std_fd(shell);
	// // 		free_commands(shell->commands);
	// // 		shell->commands = NULL;
	// // 	}
	// // 	free_tokens(shell->tokens);
	// // 	shell->tokens = NULL;
	// // }

	// // int	main(int argc, char **argv, char **env)
	// // {
	// // 	t_shell	shell;

	// // 	(void)argc;
	// // 	(void)argv;
	// // 	init_shell(&shell, env);
	// // 	while (1)
	// // 	{
	// // 		read_input(&shell);
	// // 		process_input(&shell);
	// // 	}
	// // 	return (0);
	// // }

	// void	setup_signals(void)
	// {
	// // TODO: Implementar
	// }

	// void	handle_sigint(int sig)
	// {
	// (void)sig;
	// // TODO: Implementar
	// }

	// void	handle_sigquit(int sig)
	// {
	// (void)sig;
	// // TODO: Implementar
	// }

	// int	execute_commands(t_shell *shell)
	// {
	// (void)shell;
	// return (0);
	// }

	// int	execute_single_command(t_shell *shell, t_cmd *cmd)
	// {
	// (void)shell;
	// (void)cmd;
	// return (0);
	// }

	// int	execute_pipeline(t_shell *shell)
	// {
	// (void)shell;
	// return (0);
	// }

	// void	handle_redirections(t_cmd *cmd)
	// {
	// (void)cmd;
	// }

	// void	restore_std_fd(t_shell *shell)
	// {
	// (void)shell;
	// }

	// char	**copy_env(char **env)
	// {
	// char	**new_env;
	// int		i;
	// int		count;

	// count = 0;
	// if (!env)
	// return (NULL);
	// while (env[count])
	// count++;
	// new_env = malloc(sizeof(char *) * (count + 1));
	// if (!new_env)
	// return (NULL);
	// i = 0;
	// while (i < count)
	// {
	// new_env[i] = ft_strdup(env[i]);
	// if (!new_env[i])
	// return (NULL);
	// i++;
	// }
	// new_env[i] = NULL;
	// return (new_env);
	// }

	// void	init_shell(t_shell *shell, char **env)
	// {
	// if (!shell)
	// return ;
	// shell->env = copy_env(env);
	// shell->tokens = NULL;
	// shell->commands = NULL;
	// shell->exit_status = 0;
	// shell->stdin_copy = dup(STDIN_FILENO);
	// shell->stdout_copy = dup(STDOUT_FILENO);
	// }

	// void	cleanup_shell(t_shell *shell)
	// {
	// int	i;

	// if (!shell)
	// return ;
	// if (shell->tokens)
	// free_tokens(&shell->tokens);
	// if (shell->commands)
	// free_commands(&shell->commands);
	// if (shell->env)
	// {
	// i = 0;
	// while (shell->env[i])
	// {
	// free(shell->env[i]);
	// i++;
	// }
	// free(shell->env);
	// }
	// if (shell->stdin_copy > 2)
	// close(shell->stdin_copy);
	// if (shell->stdout_copy > 2)
	// close(shell->stdout_copy);
	// }

	// void	print_error(char *cmd, char *msg)
	// {
	// printf("minishell: %s: %s\n", cmd, msg);
	// }

	// char	*find_executable(char *cmd, char **env)
	// {
	// (void)cmd;
	// (void)env;
	// return (NULL);
	// }

	// // ============================================================================
	// // MAIN - Función principal
	// // ============================================================================

	// int	main(int argc, char **argv, char **env)
	// {
	// t_shell	shell;
	// char	*input;

	// (void)argc;
	// (void)argv;

	// // Inicializar shell
	// init_shell(&shell, env);

	// printf("=== MINISHELL - TEST MODE ===\n");
	// printf("Escribe 'exit' para salir\n\n");

	// // Loop principal
	// while (1)
	// {
	// // Leer input
	// input = readline("minishell$ ");

	// // Verificar EOF (Ctrl+D)
	// if (!input)
	// {
	// printf("exit\n");
	// break ;
	// }

	// // Agregar a historial si no está vacío
	// if (*input)
	// add_history(input);

	// // Salir si el usuario escribe "exit"
	// if (ft_strncmp(input, "exit", 4) == 0)
	// {
	// free(input);
	// break ;
	// }

	// // Procesar input
	// shell.tokens = tokenize(input);

	// if (shell.tokens)
	// {
	// // Expandir variables
	// expand_variables(&shell, shell.tokens);

	// // Parsear tokens en comandos
	// shell.commands = parse_tokens(&shell.tokens);

	// // Ejecutar comandos
	// if (shell.commands)
	// {
	// execute_commands(&shell);
	// free_commands(&shell.commands);
	// shell.commands = NULL;
	// }

	// // Liberar tokens
	// free_tokens(&shell.tokens);
	// shell.tokens = NULL;
	// }

	// free(input);
	// }

	// // Limpiar y salir
	// cleanup_shell(&shell);

	// return (shell.exit_status);
	// }

/* *** comentarios *** */

/* *** comentarios *** */

#include "../minishell.h"

volatile sig_atomic_t	g_signal = 0;

// ============================================================================
// STUBS TEMPORALES
// ============================================================================

void	setup_signals(void)
{
    // TODO: Implementar
}

void	handle_sigint(int sig)
{
    (void)sig;
    // TODO: Implementar
}

void	handle_sigquit(int sig)
{
    (void)sig;
    // TODO: Implementar
}

// ✅ ELIMINADA: execute_pipeline - Ya está en executor.c

void	handle_redirections(t_cmd *cmd)
{
    (void)cmd;
}

void	restore_std_fd(t_shell *shell)
{
    (void)shell;
}

char	**copy_env(char **env)
{
    char	**new_env;
    int		i;
    int		count;

    count = 0;
    if (!env)
        return (NULL);
    while (env[count])
        count++;
    new_env = malloc(sizeof(char *) * (count + 1));
    if (!new_env)
        return (NULL);
    i = 0;
    while (i < count)
    {
        new_env[i] = ft_strdup(env[i]);
        if (!new_env[i])
            return (NULL);
        i++;
    }
    new_env[i] = NULL;
    return (new_env);
}

void	init_shell(t_shell *shell, char **env)
{
    if (!shell)
        return ;
    shell->env = copy_env(env);
    shell->tokens = NULL;
    shell->commands = NULL;
    shell->exit_status = 0;
    shell->stdin_copy = dup(STDIN_FILENO);
    shell->stdout_copy = dup(STDOUT_FILENO);
}

void	cleanup_shell(t_shell *shell)
{
    int	i;

    if (!shell)
        return ;
    if (shell->tokens)
        free_tokens(&shell->tokens);
    if (shell->commands)
        free_commands(&shell->commands);
    if (shell->env)
    {
        i = 0;
        while (shell->env[i])
        {
            free(shell->env[i]);
            i++;
        }
        free(shell->env);
    }
    if (shell->stdin_copy > 2)
        close(shell->stdin_copy);
    if (shell->stdout_copy > 2)
        close(shell->stdout_copy);
}

void	print_error(char *cmd, char *msg)
{
    printf("minishell: %s: %s\n", cmd, msg);
}

char	*find_executable(char *cmd, char **env)
{
    (void)cmd;
    (void)env;
    return (NULL);
}

// ============================================================================
// MAIN
// ============================================================================

int	main(int argc, char **argv, char **env)
{
    t_shell	shell;
    char	*input;

    (void)argc;
    (void)argv;
    
    init_shell(&shell, env);
    
    printf("=== MINISHELL - TEST MODE ===\n");
    printf("Escribe 'exit' para salir\n\n");
    
    while (1)
    {
        input = readline("minishell$ ");
        
        if (!input)
        {
            printf("exit\n");
            break ;
        }
        if (*input)
            add_history(input);
        
        if (ft_strncmp(input, "exit", 4) == 0)
        {
            free(input);
            break ;
        }
        
        shell.tokens = tokenize(input);
        printf("%s\n", input);
        if (shell.tokens)
        {
            expand_variables(&shell, shell.tokens);
            shell.commands = parse_tokens(&shell.tokens);
            
            if (shell.commands)
            {
                execute_commands(&shell);
                free_commands(&shell.commands);
                shell.commands = NULL;
            }
            
            free_tokens(&shell.tokens);
            shell.tokens = NULL;
        }
        
        free(input);
    }
    
    cleanup_shell(&shell);
    
    return (shell.exit_status);
}