/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   struct_utils.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: axgimene <axgimene@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/10 18:37:43 by gguardam          #+#    #+#             */
/*   Updated: 2025/11/27 20:05:53 by axgimene         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

char	**copy_env(char **env)
{
    char	**new_env;
    int		i;
    int		count;
    int		j;

    if (!env)
        return (NULL);
    count = 0;
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
        {
            j = 0;
            while (j < i)
            {
                free(new_env[j]);
                new_env[j] = NULL;
                j++;
            }
            free(new_env);
            new_env = NULL;
            return (NULL);
        }
        i++;
    }
    new_env[count] = NULL;
    return (new_env);
}

void	init_shell(t_shell *shell, char **envp)
{
    char	*cwd;

    if (!shell)
        return ;
    cwd = getcwd(NULL, 0);
    shell->prompt = format_cwd(cwd);
    if (cwd)
        free(cwd);
    shell->tokens = NULL;
    shell->commands = NULL;
    shell->exit_status = 0;
    shell->stdin_copy = dup(STDIN_FILENO);
    shell->stdout_copy = dup(STDOUT_FILENO);
    shell->env = copy_env(envp);
    if (!shell->env)
    {
        shell->env = malloc(sizeof(char *));
        if (shell->env)
            shell->env[0] = NULL;
    }
    // ✅ CRÍTICO: Inicializa local_vars AQUÍ
    shell->local_vars = malloc(sizeof(char *));
    if (shell->local_vars)
        shell->local_vars[0] = NULL;
    else
        shell->local_vars = NULL;  // ✅ Asegúrate de que sea NULL si falla
}

void	cleanup_shell(t_shell *shell)
{
    int	i;

    if (!shell)
        return ;
    
    // ✅ Close file descriptors safely
    if (shell->stdin_copy > 0)
        close(shell->stdin_copy);
    if (shell->stdout_copy > 0)
        close(shell->stdout_copy);
    
    // ✅ Free prompt safely
    if (shell->prompt)
    {
        free(shell->prompt);
        shell->prompt = NULL;
    }
    
    // ✅ Free tokens safely
    if (shell->tokens)
    {
        free_tokens(&shell->tokens);
        shell->tokens = NULL;
    }
    
    // ✅ Free commands safely
    if (shell->commands)
    {
        free_commands(&shell->commands);
        shell->commands = NULL;
    }
    
    // ✅ Verify env is not NULL before freeing
    if (shell->env)
    {
        i = 0;
        while (shell->env[i])
        {
            free(shell->env[i]);
            shell->env[i] = NULL;
            i++;
        }
        free(shell->env);
        shell->env = NULL;
    }
    
    // ✅ Verify local_vars is not NULL before freeing
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