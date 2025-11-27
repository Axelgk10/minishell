/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   struct_utils.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: axgimene <axgimene@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/10 18:37:43 by gguardam          #+#    #+#             */
/*   Updated: 2025/11/24 20:17:03 by axgimene         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

void	init_shell(t_shell *shell, char **envp)
{
    int		env_count;
    int		i;

    if (!shell)
        return ;
    
    // ✅ Inicializar primero para seguridad (en caso de cleanup anticipado)
    shell->env = NULL;
    shell->env_count = 0;
    shell->local_vars = NULL;
    shell->tokens = NULL;
    shell->commands = NULL;
    shell->prompt = NULL;
    shell->exit_status = 0;
    shell->stdin_copy = STDIN_FILENO;
    shell->stdout_copy = STDOUT_FILENO;
    
    // Copiar variables de entorno
    env_count = 0;
    while (envp && envp[env_count])
        env_count++;
    
    shell->env = malloc((env_count + 1) * sizeof(char *));
    if (!shell->env)
    {
        ft_putstr_fd("Error: malloc failed\n", STDERR_FILENO);
        exit(1);
    }
    
    // ✅ SET env_count BEFORE strdup calls in case of failure
    shell->env_count = env_count;
    
    i = 0;
    while (i < env_count)
    {
        shell->env[i] = ft_strdup(envp[i]);
        if (!shell->env[i])
        {
            // ✅ Si strdup falla, limpiar lo que ya fue asignado
            int j = 0;
            while (j < i)
            {
                free(shell->env[j]);
                j++;
            }
            free(shell->env);
            shell->env = NULL;
            shell->env_count = 0;
            ft_putstr_fd("Error: malloc failed\n", STDERR_FILENO);
            exit(1);
        }
        i++;
    }
    shell->env[env_count] = NULL;
}

void	cleanup_shell(t_shell *shell)
{
    int i;
    
    if (!shell)
        return ;
    
    // Liberar environment (copiado en init_shell)
    if (shell->env && shell->env_count > 0)
    {
        i = 0;
        // ✅ Liberar todos los strings en el array
        // El array tiene indices 0 a env_count-1 con strings
        // E índice env_count tiene NULL
        while (i < shell->env_count)
        {
            if (shell->env[i])
            {
                free(shell->env[i]);
                shell->env[i] = NULL;
            }
            i++;
        }
        // ✅ Liberar el array mismo (que contiene los punteros)
        free(shell->env);
        shell->env = NULL;
    }
    else if (shell->env)
    {
        // If env_count is 0 but env is allocated, free it anyway
        free(shell->env);
        shell->env = NULL;
    }
    
    // Liberar prompt
    if (shell->prompt)
    {
        free(shell->prompt);
        shell->prompt = NULL;
    }
    
    // Liberar tokens
    if (shell->tokens)
    {
        free_tokens(&shell->tokens);
        shell->tokens = NULL;
    }
    
    // Liberar commands
    if (shell->commands)
    {
        free_commands(&shell->commands);
        shell->commands = NULL;
    }
    
    // Liberar local_vars
    if (shell->local_vars)
    {
        i = 0;
        while (shell->local_vars[i])
        {
            free(shell->local_vars[i]);
            shell->local_vars[i] = NULL;
            i++;
        }
        free(shell->local_vars);
        shell->local_vars = NULL;
    }
}

void	check_struct(t_shell *shell)
{
    int i;
    t_cmd *current;

    i = 0;
    printf("Stdin: %d\n", shell->stdin_copy);
    printf("Stdout: %d\n", shell->stdout_copy);
    printf("Exit Status: %d\n", shell->exit_status);
    printf("/////////////////////Commands///////////////////////////\n");
    current = shell->commands;  // Usar una variable temporal
    while(current)
    {
        printf("FDdin: %d\n", current->in_fd);
        printf("FDout: %d\n", current->out_fd);
        printf("Pipe 0: %d\n", current->pipe[0]);
        printf("Pipe 1: %d\n", current->pipe[1]);
        printf("Is Built in: %d\n", current->is_builtin);
        i = 0;
        if (current->av)  // Verificar que av no sea NULL
        {
            while(current->av[i])
            {
                printf("As %d: %s\n", i, current->av[i]);
                i++;
            }
        }
        printf("////////////////////////////////////////////////\n");
        current = current->next;  // Mover solo la variable temporal
    }
}