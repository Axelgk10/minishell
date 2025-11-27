/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   errors.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: axgimene <axgimene@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/10 18:37:37 by gguardam          #+#    #+#             */
/*   Updated: 2025/11/24 20:23:29 by axgimene         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

extern t_shell *g_shell;

void	error_executing\
(int site_of_error, char **env, char **cmd_params)
{
    (void)env;
    (void)cmd_params;
    if (site_of_error == 0)
        exit((perror("command not found"), 127));
    else if (site_of_error == 1)
        exit((perror("bin not found"), 127));
    else
        exit((perror("execve"), 127));
}

int	write_error_message(int fd, char *cmd, char *arg, char *error_msg)
{
    ft_putstr_fd(cmd, fd);
    ft_putstr_fd(": ", fd);
    if (arg && arg[0] != '\0')
    {
        ft_putstr_fd(arg, fd);
        ft_putstr_fd(": ", fd);
    }
    ft_putstr_fd(error_msg, fd);
    ft_putstr_fd("\n", fd);
    return (127);
}

void	null_input(void)
{
    printf("exit\n");
    
    // ✅ LIBERA TODO ANTES DE SALIR
    if (g_shell)
    {
        // Libera el prompt actual
        if (g_shell->prompt)
        {
            free(g_shell->prompt);
            g_shell->prompt = NULL;
        }
        
        // Libera tokens del último comando
        if (g_shell->tokens)
        {
            free_tokens(&g_shell->tokens);
            g_shell->tokens = NULL;
        }
        
        // Libera commands del último comando
        if (g_shell->commands)
        {
            free_commands(&g_shell->commands);
            g_shell->commands = NULL;
        }
        
        // ✅ Llama a cleanup_shell para liberar local_vars también
        cleanup_shell(g_shell);
    }
    
    // ✅ Libera readline history
    rl_clear_history();
    
    exit(0);
}
