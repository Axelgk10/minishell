#include "minishell.h"

int g_exit_status = 0;

/*static void	cleanup_on_exit(t_shell *shell)
{
	if (shell)
	{
		if (shell->env)
		{
			int i = 0;
			int count = shell->env_count;
			if (count == 0)
			{
				while (shell->env[i] != NULL)
					i++;
				count = i;
			}
			i = 0;
			while (i < count)
			{
				if (shell->env[i])
				{
					free(shell->env[i]);
					shell->env[i] = NULL;
				}
				i++;
			}
			free(shell->env);
			shell->env = NULL;
		}
		if (shell->prompt)
		{
			free(shell->prompt);
			shell->prompt = NULL;
		}
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
		if (shell->local_vars)
		{
			// TODO: implement local_vars cleanup if needed
		}
		free(shell);
		shell = NULL;
	}
	rl_clear_history();
}*/

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

static int	check_unclosed_quotes(char *input)
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
		return (write(STDERR_FILENO, "minishell: unclosed double quote\n", 33), 1);
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
	shell->tokens = tokenize(input);
	if (shell->tokens)
	{
		expand_variables(shell, shell->tokens);
		shell->commands = parse_tokens(shell->tokens);
		if (shell->commands)
			evaluate_struct(shell);
		else
			shell->exit_status = 2;
	}
}

static void	restart_loop(char **input, t_shell *shell)
{
	if(*input)
	{
		free(*input);
		*input = NULL;
	}
	if (shell->prompt)
	{
		free(shell->prompt);
		shell->prompt = NULL;
	}
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

static void	shell_core(t_shell *shell)
{
	char	*input;
	char	*cwd_buffer;

	while (1)
	{
		cwd_buffer = getcwd(NULL, 0);
		if(cwd_buffer)
		{
			shell->prompt = format_cwd(cwd_buffer);
			free(cwd_buffer);
			cwd_buffer = NULL;
		}
		input = readline(shell->prompt);
		if (!input)
			null_input(shell);
		if (input && *input)
			process_input(shell, input);
		restart_loop(&input, shell);
		free(cwd_buffer);
	}
}

//g_shell = shell; //Claudia nos hacia copiar la estructura en una variable global, pero no podemos tenerla porque ya estamos utilizando la variable 
//atexit(cleanup_on_exit); //No se puede utilizar
int	main(int argc, char **argv, char **envp)
{	
	t_shell	*shell;

	(void)argc;
	(void)argv;
	shell = (t_shell *)malloc(sizeof(t_shell));
	if (!shell)
		return (ft_putstr_fd("Error: malloc failed for shell structure\n", STDERR_FILENO), 1);
	init_shell(shell, envp);
	init_signals();
	shell_core(shell);
	cleanup_shell(shell);
	free(shell);
	return (shell->exit_status);
}
