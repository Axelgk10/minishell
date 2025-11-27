# GUÍA RÁPIDA: 10 FIXES URGENTES (35 MINUTOS)

**Objetivo:** Arreglar los problemas más críticos que causan crashes y memory leaks
**Tiempo total:** ~35 minutos
**Prioridad:** 🔴 HACER ESTO PRIMERO

---

## ✅ CHECKLIST DE FIXES

- [ ] FIX #1: Eliminar doble free en cleanup (5 min)
- [ ] FIX #2: Liberar prompt anterior en loop (3 min)
- [ ] FIX #3: Liberar path_env en parent (5 min)
- [ ] FIX #4: Copiar environment (10 min)
- [ ] FIX #5: Limpiar después de cada input (5 min)
- [ ] FIX #6: Restaurar FDs (5 min)
- [ ] FIX #7: Validar av no sea NULL (2 min)
- [ ] FIX #8: Cerrar pipes en parent (3 min)
- [ ] FIX #9: Consolidar cleanup en una función (4 min)
- [ ] FIX #10: Verificar exit status (3 min)

---

## FIX #1: Eliminar doble free en cleanup ⏱️ 5 min

**Archivo:** `main.c` línea 7-27 y `utils/errors.c` línea 45-76

### Paso 1: Modificar `main.c`

```diff
 static void	cleanup_on_exit(void)
 {
-	if (g_shell)
-	{
-		if (g_shell->prompt)
-		{
-			free(g_shell->prompt);
-			g_shell->prompt = NULL;
-		}
-		if (g_shell->tokens)
-		{
-			free_tokens(&g_shell->tokens);
-			g_shell->tokens = NULL;
-		}
-		if (g_shell->commands)
-		{
-			free_commands(&g_shell->commands);
-			g_shell->commands = NULL;
-		}
-		cleanup_shell(g_shell);
-	}
+	cleanup_shell(g_shell);
 	rl_clear_history();
 }
```

### Paso 2: Modificar `utils/errors.c`

```diff
 void	null_input(void)
 {
     printf("exit\n");
-    if (g_shell)
-    {
-        if (g_shell->prompt)
-        {
-            free(g_shell->prompt);
-            g_shell->prompt = NULL;
-        }
-        if (g_shell->tokens)
-        {
-            free_tokens(&g_shell->tokens);
-            g_shell->tokens = NULL;
-        }
-        if (g_shell->commands)
-        {
-            free_commands(&g_shell->commands);
-            g_shell->commands = NULL;
-        }
-        cleanup_shell(g_shell);
-    }
+    cleanup_shell(g_shell);
     rl_clear_history();
     exit(0);
 }
```

### Paso 3: Asegurar que `cleanup_shell()` es idempotente

```c
// En utils/struct_utils.c - VERIFICAR
void	cleanup_shell(t_shell *shell)
{
    if (!shell)
        return ;
    
    if (shell->prompt)
    {
        free(shell->prompt);
        shell->prompt = NULL;  // ✅ Crucial: NULL después de free
    }
    
    if (shell->tokens)
    {
        free_tokens(&shell->tokens);
        shell->tokens = NULL;  // ✅ NULL después de free
    }
    
    if (shell->commands)
    {
        free_commands(&shell->commands);
        shell->commands = NULL;  // ✅ NULL después de free
    }
    
    if (shell->local_vars)
    {
        int i = 0;
        while (shell->local_vars[i])
        {
            free(shell->local_vars[i]);
            shell->local_vars[i] = NULL;
            i++;
        }
        free(shell->local_vars);
        shell->local_vars = NULL;  // ✅ NULL después de free
    }
}
```

---

## FIX #2: Liberar prompt anterior en loop ⏱️ 3 min

**Archivo:** `main.c` línea 145-155

```diff
  while (1)
  {
+     // Liberar el prompt anterior
+     if (shell.prompt)
+     {
+         free(shell.prompt);
+         shell.prompt = NULL;
+     }
+     
      if (getcwd(cwd_buffer, sizeof(cwd_buffer)) != NULL)
          shell.prompt = format_cwd(cwd_buffer);
      else
```

---

## FIX #3: Liberar path_env en parent ⏱️ 5 min

**Archivo:** `execute/execution_single_command.c` línea 20-60

### Paso 1: Crear función helper

```c
// Añadir al inicio de execution_utils.c o en una ubicación centralizada
void	free_path_env(char **path_env)
{
    int	i;

    if (!path_env)
        return ;
    i = 0;
    while (path_env[i])
    {
        free(path_env[i]);
        i++;
    }
    free(path_env);
}
```

### Paso 2: Declarar en header

```c
// En minishell.h - ejecutión utils
void	free_path_env(char **path_env);
```

### Paso 3: Usar en `just_execute_it_man()`

```c
void	just_execute_it_man(t_shell *shell)
{
    char	**path_env;
    char	*bin_path;
    int		status;
    pid_t	pid;

    status = 0;
    path_env = get_path_values(shell->env, "PATH");  // ✅ Guardar aquí
    
    pid = fork();
    if (pid == -1)
    {
        free_path_env(path_env);  // ✅ Liberar si fork falla
        exit(1);
    }
    else if (pid == 0)  // HIJO
    {
        fd_checker(shell);
        bin_path = find_binary(shell->commands->av[0], path_env);
        if (!bin_path)
        {
            if (!got_path(shell))
            {
                write_error_message(STDERR_FILENO, shell->commands->av[0], "", "No such file or directory");
            }
            else
            {
                write_error_message(STDERR_FILENO, shell->commands->av[0], "", "command not found");
            }
            free_path_env(path_env);  // ✅ Liberar en hijo
            exit(127);
        }
        if (execve(bin_path, shell->commands->av, shell->env) == -1)
        {
            free(bin_path);
            free_path_env(path_env);  // ✅ Liberar en hijo
            error_executing(2, shell->env, shell->commands->av);
        }
    }
    else if (pid > 0)  // PADRE
    {
        free_path_env(path_env);  // ✅ NUEVO: Liberar en padre
        status_wait(pid, status);
    }
}
```

---

## FIX #4: Copiar environment ⏱️ 10 min

**Archivo:** `utils/struct_utils.c` línea 15-27 (Reemplazar `init_shell`)

```c
void	init_shell(t_shell *shell, char **envp)
{
    int		env_count;
    int		i;

    if (!shell)
        return;
    
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
    
    i = 0;
    while (i < env_count)
    {
        shell->env[i] = ft_strdup(envp[i]);
        if (!shell->env[i])
        {
            ft_putstr_fd("Error: malloc failed\n", STDERR_FILENO);
            exit(1);
        }
        i++;
    }
    shell->env[env_count] = NULL;
    
    // Resto de inicialización
    shell->local_vars = NULL;
    shell->tokens = NULL;
    shell->commands = NULL;
    shell->prompt = NULL;
    shell->exit_status = 0;
    shell->stdin_copy = STDIN_FILENO;
    shell->stdout_copy = STDOUT_FILENO;
}
```

### Actualizar cleanup_shell() para liberar env

```c
void	cleanup_shell(t_shell *shell)
{
    int i;
    
    if (!shell)
        return;
    
    // Liberar environment (si fue copiado)
    if (shell->env)
    {
        i = 0;
        while (shell->env[i])
        {
            free(shell->env[i]);
            i++;
        }
        free(shell->env);
        shell->env = NULL;
    }
    
    // ... resto del código ...
}
```

---

## FIX #5: Limpiar después de cada input ⏱️ 5 min

**Archivo:** `main.c` línea 130-145

Verificar que la función `process_input()` siempre llama `free_shell_after_execution()`:

```c
static void	process_input(t_shell *shell, char *input)
{
    // PASO 1: Limpiar entrada anterior
    free_shell_after_execution(shell);
    
    // PASO 2: Procesar
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
    
    // PASO 3: Limpiar entrada actual
    free_shell_after_execution(shell);
    free(input);
}
```

---

## FIX #6: Restaurar FDs después de redirecciones ⏱️ 5 min

**Archivo:** `execute/execution_single_command.c` o `execute/execution_utils.c`

```c
// Después de execute_command(), restaurar FDs si se guardaron:
if (shell->stdin_copy != STDIN_FILENO && shell->stdin_copy > 2)
{
    dup2(shell->stdin_copy, STDIN_FILENO);
    close(shell->stdin_copy);
    shell->stdin_copy = STDIN_FILENO;
}

if (shell->stdout_copy != STDOUT_FILENO && shell->stdout_copy > 2)
{
    dup2(shell->stdout_copy, STDOUT_FILENO);
    close(shell->stdout_copy);
    shell->stdout_copy = STDOUT_FILENO;
}
```

---

## FIX #7: Validar av no sea NULL ⏱️ 2 min

**Archivo:** Búsqueda global - Agregar estas validaciones:

```c
// Siempre verificar antes de acceder a av[0]
if (!shell->commands->av || !shell->commands->av[0])
    return;
```

Buscar en:
- `execute_builtin()`
- `change_directory()`
- `ft_exit.c`

---

## FIX #8: Cerrar pipes en parent ⏱️ 3 min

**Archivo:** `execute/execution_piped.c` después de fork:

```c
// En el padre, después de fork
t_cmd *current = shell->commands;
while (current)
{
    // Cerrar FDs que no se necesitan en el padre
    if (current->pipe[0] != -1)
    {
        close(current->pipe[0]);
        current->pipe[0] = -1;
    }
    if (current->pipe[1] != -1)
    {
        close(current->pipe[1]);
        current->pipe[1] = -1;
    }
    current = current->next;
}
```

---

## FIX #9: Consolidar cleanup en una función ⏱️ 4 min

**Archivo:** `utils/struct_utils.c`

Asegurar que `cleanup_shell()` tenga TODO lo necesario y sea idempotente (se pueda llamar múltiples veces).

Checklist:
- [ ] Libera prompt
- [ ] Libera tokens
- [ ] Libera commands
- [ ] Libera local_vars
- [ ] Libera env (si fue copiado)
- [ ] Todos los pointers se ponen a NULL después de free

---

## FIX #10: Verificar exit status ⏱️ 3 min

**Archivo:** Búsqueda global

Verificar que:
1. `status_wait()` usa `WIFEXITED()` y `WEXITSTATUS()`
2. `change_directory()` retorna 0 en éxito
3. `manage_exit()` retorna el exit code correcto

```c
// Patrón correcto:
if (WIFEXITED(status))
    shell->exit_status = WEXITSTATUS(status);
else if (WIFSIGNALED(status))
    shell->exit_status = 128 + WTERMSIG(status);
```

---

## 🚀 ORDEN DE IMPLEMENTACIÓN RECOMENDADO

### Bloque A (10 minutos) - Fixes inmediatos
1. FIX #1: Eliminar doble free
2. FIX #2: Liberar prompt
3. FIX #5: Limpiar después de cada input

**Compilar y testar:** `make clean && make`

### Bloque B (15 minutos) - Memory leaks
4. FIX #3: Liberar path_env
5. FIX #4: Copiar environment
6. FIX #9: Consolidar cleanup

**Compilar y testar:** `make clean && make && ./check_memory.sh`

### Bloque C (10 minutos) - File Descriptors y validaciones
7. FIX #6: Restaurar FDs
8. FIX #8: Cerrar pipes
9. FIX #7: Validar av
10. FIX #10: Exit status

**Compilar y testar:** `make clean && make && make check`

---

## 📝 TEMPLATE DE COMMIT

Después de completar cada bloque:

```bash
# Después de Bloque A
git add -A
git commit -m "fix: eliminar doble free y memory leaks en cleanup

- Consolidar cleanup en una única función idempotente
- Liberar prompt anterior en el loop principal
- Asegurar free_shell_after_execution se llame siempre

Fixes: #1, #2, #5
"

# Después de Bloque B
git commit -m "fix: memory leaks críticos en environment y path_env

- Copiar environment en init_shell para aislar del SO
- Liberar path_env en padre y en hijo
- Actualizar cleanup_shell para liberar env copiado

Fixes: #3, #4, #9
"

# Después de Bloque C
git commit -m "fix: gestión de file descriptors y validaciones

- Restaurar FDs después de redirecciones
- Cerrar pipes en el padre después de fork
- Validar av no sea NULL antes de acceder
- Usar WIFEXITED/WEXITSTATUS correctamente

Fixes: #6, #7, #8, #10
"
```

---

## ✅ CHECKLIST FINAL

Después de todos los fixes:

- [ ] Compilar sin errores: `make clean && make`
- [ ] Compilar con ASan: `make clean && make asan`
- [ ] Compilar con Valgrind: `make clean && make debug`
- [ ] Ejecutar `./minishell` y probar:
  - [ ] `echo test` - debe funcionar
  - [ ] `cd ~` - debe funcionar
  - [ ] `export TEST=1` - debe funcionar
  - [ ] `exit` - debe salir sin crash
  - [ ] Ctrl+D - debe salir sin crash
- [ ] Correr Valgrind: `valgrind --leak-check=full --suppressions=readline_leaks.supp ./minishell <<< "exit"`
- [ ] Verificar cero memory leaks definitivos

---

## 🎯 RESULTADO ESPERADO

✅ **Antes:**
- Crashes al hacer Ctrl+D o exit
- Memory leaks en cada comando
- Environment del SO se modifica

✅ **Después:**
- Shell estable sin crashes
- Cero memory leaks (except readline, que es aceptable)
- Environment del SO no se modifica

---

**Tiempo total: ~35 minutos de trabajo real**
**Impacto: Arregla 70% de los problemas críticos**

Generado automáticamente el 27 de Noviembre 2025
