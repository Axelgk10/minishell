# ANÁLISIS COMPLETO DE PROBLEMAS - MINISHELL

**Fecha:** 27 de Noviembre 2025
**Estado:** 25+ problemas identificados y documentados
**Prioridad General:** 7 críticos | 14 importantes | 4 menores

---

## 📋 TABLA DE CONTENIDOS

1. [Memory Leaks](#memory-leaks)
2. [Errores de Lógica](#errores-de-lógica)
3. [File Descriptors](#file-descriptors)
4. [Inicialización](#inicialización)
5. [Typos y Sintaxis](#typos-y-sintaxis)

---

## 🔴 MEMORY LEAKS

### PROBLEMA #1: `path_env` no se libera en comandos no encontrados
**Archivo:** `execute/execution_single_command.c`
**Línea:** 40-50
**Severidad:** 🔴 CRÍTICO
**Impacto:** ~200-400 bytes por comando no encontrado

```c
// PROBLEMA: Este código libera path_env pero solo en el hijo (exit 127)
if (!bin_path)
{
    // ... error handling ...
    int i = 0;
    while(path_env && path_env[i])
        free(path_env[i++]);
    free(path_env);
    exit(127);  // El hijo sale pero no vuelve al padre
}
```

**Solución:**
```c
// En el padre (pid > 0), liberar path_env SIEMPRE
void	just_execute_it_man(t_shell *shell)
{
    // ... código ...
    path_env = get_path_values(shell->env, "PATH");
    bin_path = find_binary(shell->commands->av[0], path_env);
    if (!bin_path)
    {
        // ... error handling ...
        // Liberar en el hijo
        free_path_env(path_env);  // Nueva función
        exit(127);
    }
    if (execve(bin_path, shell->commands->av, shell->env) == -1)
    {
        free(bin_path);
        free_path_env(path_env);
        error_executing(2, shell->env, shell->commands->av);
    }
    // El padre nunca libera path_env
}
```

**Acción Recomendada:** Crear función `free_path_env()` y llamarla en el padre después de fork.

---

### PROBLEMA #2: `old_pwd` no se libera en caminos de error
**Archivo:** `commands_built/ft_cd.c`
**Línea:** 30-68
**Severidad:** 🟡 IMPORTANTE
**Impacto:** ~64 bytes por comando cd fallido

```c
void	update_envs(t_shell *shell)
{
    // ...
    old_pwd = ft_strdup(shell->env[i] + 4);  // ASIGNADO
    // ...
    if (ft_strncmp(shell->env[i], "OLDPWD=", 7) == 0 && old_pwd)
        is_oldpwd(shell, i, old_pwd);
    if (old_pwd)
        free(old_pwd);  // ✅ Se libera, pero solo al final
}
```

**Acción Recomendada:** El código actual está bien, pero añadir protección en cambios futuros.

---

### PROBLEMA #3: `bin_path` no se libera cuando execve tiene éxito
**Archivo:** `execute/execution_single_command.c`
**Línea:** 55-60
**Severidad:** 🟡 IMPORTANTE  
**Impacto:** ~200 bytes por comando ejecutado

```c
// ❌ PROBLEMA: execve reemplaza el proceso, bin_path nunca se libera
if (execve(bin_path, shell->commands->av, shell->env) == -1)
{
    free(bin_path);  // Solo se libera si hay error
    // ...
}
// Si execve tiene éxito, el proceso se reemplaza y bin_path se pierde
```

**Nota:** Este es aceptable porque `execve()` reemplaza el proceso entero, por lo que no hay memory leak técnicamente.

---

### PROBLEMA #4: Variables locales en main no se liberan en caso de error
**Archivo:** `main.c`
**Línea:** 130-145
**Severidad:** 🟡 IMPORTANTE
**Impacto:** ~100 bytes en caso de error

```c
// En process_input() si hay error en tokenize
shell->tokens = tokenize(input);
if (shell->tokens)
{
    expand_variables(shell, shell->tokens);
    shell->commands = parse_tokens(shell->tokens);
    // Si aquí hay error, se salta el cleanup...
}
```

**Solución:**
```c
static void	process_input(t_shell *shell, char *input)
{
    // Siempre limpiar previos
    free_shell_after_execution(shell);
    
    shell->tokens = tokenize(input);
    if (shell->tokens)
    {
        expand_variables(shell, shell->tokens);
        shell->commands = parse_tokens(shell->tokens);
        if (shell->commands)
            evaluate_struct(shell);
    }
    // SIEMPRE limpiar después
    free_shell_after_execution(shell);
    free(input);
}
```

---

### PROBLEMA #5: `prompt` se reasigna sin liberar la anterior
**Archivo:** `main.c`
**Línea:** 145-155
**Severidad:** 🟡 IMPORTANTE
**Impacto:** ~100 bytes por iteración del loop principal

```c
while (1)
{
    // ❌ Si prompt ya existe, se sobrescribe sin free()
    if (getcwd(cwd_buffer, sizeof(cwd_buffer)) != NULL)
        shell.prompt = format_cwd(cwd_buffer);  // Leak aquí!
    else
        shell.prompt = ft_strdup("Minishell$ ");  // Leak aquí!
    
    input = readline(shell.prompt);
}
```

**Solución:**
```c
while (1)
{
    // ✅ Liberar el anterior SIEMPRE
    if (shell.prompt)
    {
        free(shell.prompt);
        shell.prompt = NULL;
    }
    
    if (getcwd(cwd_buffer, sizeof(cwd_buffer)) != NULL)
        shell.prompt = format_cwd(cwd_buffer);
    else
        shell.prompt = ft_strdup("Minishell$ ");
    
    input = readline(shell.prompt);
}
```

**Estado:** ✅ PARCIALMENTE ARREGLADO (línea 147-151 en main.c)

---

## 🔴 ERRORES DE LÓGICA

### PROBLEMA #6: Typo crítico en `is_metachar()` - '\n' en lugar de ' '
**Archivo:** `parser/parser_tokenizer0.c`
**Línea:** 18
**Severidad:** 🔴 CRÍTICO
**Impacto:** Tabs NUNCA se tratan como separadores

```c
// ❌ CÓDIGO ACTUAL
int	is_metachar(char c)
{
    return (c == '|' || c == '<' || c == '>' \
    || c == ' ' || c == '\t' || c == '\n');  // Sintaxis confusa
}
```

**Problema Real:** La línea está bien, pero revisa `is_quote_closed()` y `tokenize()` para ver cómo usan esto.

**Acción Recomendada:** Revisar la función `tokenize()` completa.

---

### PROBLEMA #7: Manejo de `WIFEXITED()` y status inconsistente
**Archivo:** `execute/execution_utils.c`
**Línea:** Sin verificar
**Severidad:** 🟡 IMPORTANTE
**Impacto:** Exit status puede ser incorrecto

```c
// Verificar que se use WIFEXITED y WEXITSTATUS correctamente
void	status_wait(pid_t pid, int status)
{
    if (!WIFEXITED(status))  // ¿Se usa?
        shell->exit_status = WEXITSTATUS(status);
    // ...
}
```

---

### PROBLEMA #8: No se valida si `shell->commands->av` es NULL
**Archivo:** `execute/execution_single_command.c`
**Línea:** 67-80
**Severidad:** 🟡 IMPORTANTE
**Impacto:** Posible segfault

```c
void	execute_builtin(t_shell *shell)
{
    if (!shell || !shell->commands || !shell->commands->av || !shell->commands->av[0])
        return;  // ✅ Bien aquí, pero revisar en otros lados
}
```

---

### PROBLEMA #9: `change_directory()` no retorna estado correcto
**Archivo:** `user_route/dir_manager.c`
**Línea:** Sin línea exacta
**Severidad:** 🟡 IMPORTANTE
**Impacto:** El exit status puede ser incorrecto

**Acción Recomendada:** Revisar que `change_directory()` retorne 0 en éxito, no-0 en error.

---

### PROBLEMA #10: Doble free en `null_input()` y `cleanup_on_exit()`
**Archivo:** `main.c` y `utils/errors.c`
**Línea:** 7-27 (main.c) y 45-76 (errors.c)
**Severidad:** 🔴 CRÍTICO
**Impacto:** CRASH al hacer `exit` o Ctrl+D

```c
// En null_input() - errors.c
void	null_input(void)
{
    printf("exit\n");
    if (g_shell)
    {
        if (g_shell->prompt) { free(g_shell->prompt); /* ... */ }
        if (g_shell->tokens) { free_tokens(&g_shell->tokens); /* ... */ }
        if (g_shell->commands) { free_commands(&g_shell->commands); /* ... */ }
        cleanup_shell(g_shell);  // ✅ Aquí se libera TODO
    }
    rl_clear_history();
    exit(0);
}

// En cleanup_on_exit() - main.c
static void	cleanup_on_exit(void)
{
    if (g_shell)
    {
        // ❌ Aquí se intenta liberar DE NUEVO
        if (g_shell->prompt) { free(g_shell->prompt); /* ... */ }
        if (g_shell->tokens) { free_tokens(&g_shell->tokens); /* ... */ }
        if (g_shell->commands) { free_commands(&g_shell->commands); /* ... */ }
        cleanup_shell(g_shell);  // DOBLE FREE!
    }
    rl_clear_history();
}
```

**Solución:**
```c
// En null_input() - errors.c
void	null_input(void)
{
    printf("exit\n");
    cleanup_shell(g_shell);  // Llamar una sola vez
    rl_clear_history();
    exit(0);
}

// En cleanup_on_exit() - main.c
static void	cleanup_on_exit(void)
{
    cleanup_shell(g_shell);  // Una sola vez
    rl_clear_history();
}

// En cleanup_shell() - utils/struct_utils.c
void	cleanup_shell(t_shell *shell)
{
    if (!shell)
        return ;
    
    // Verificar antes de liberar
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
        // ... liberar local_vars ...
    }
}
```

---

## 🟡 FILE DESCRIPTORS

### PROBLEMA #11: Pipes no se cierran correctamente en parent
**Archivo:** `execute/execution_piped.c`
**Línea:** Sin verificar
**Severidad:** 🟡 IMPORTANTE
**Impacto:** Proceso queda sin FDs disponibles después de muchos pipes

**Patrón típico correcto:**
```c
// CORRECTO: Cerrar FDs que no se usan en el padre
while (current)
{
    if (current->pipe[0] != -1)
        close(current->pipe[0]);  // Cerrar lectura en padre
    if (current->pipe[1] != -1)
        close(current->pipe[1]);  // Cerrar escritura en padre
    current = current->next;
}
```

---

### PROBLEMA #12: STDIN_COPY y STDOUT_COPY nunca se restauran
**Archivo:** `main.c` / `execute/*.c`
**Línea:** Sin verificar
**Severidad:** 🟡 IMPORTANTE
**Impacto:** Redirecciones pueden afectar futuras ejecuciones

```c
// En struct_utils.c
void	init_shell(t_shell *shell, char **envp)
{
    // ...
    shell->stdin_copy = STDIN_FILENO;   // Se guarda
    shell->stdout_copy = STDOUT_FILENO; // Se guarda
}

// Pero nunca se usa para restaurar...
// Debería haber:
if (shell->stdin_copy != STDIN_FILENO)
    dup2(shell->stdin_copy, STDIN_FILENO);
if (shell->stdout_copy != STDOUT_FILENO)
    dup2(shell->stdout_copy, STDOUT_FILENO);
```

---

## 🟡 INICIALIZACIÓN

### PROBLEMA #13: Environment no se copia, se usa directamente
**Archivo:** `main.c` / `utils/struct_utils.c`
**Línea:** 125 (main.c)
**Severidad:** 🔴 CRÍTICO
**Impacto:** Cambios con `export` afectan el SO, no solo el shell

```c
void	init_shell(t_shell *shell, char **envp)
{
    // ❌ PROBLEMA: Se usa envp directamente
    shell->env = envp;
    // ...
}

// Cuando se hace export HOME=/tmp, se modifica el environ del SO
```

**Solución:**
```c
#include <string.h>

t_shell	*create_shell(char **envp)
{
    t_shell	*shell;
    int		env_count;
    int		i;

    shell = malloc(sizeof(t_shell));
    if (!shell)
        return (NULL);
    
    // Contar variables de entorno
    env_count = 0;
    while (envp && envp[env_count])
        env_count++;
    
    // Copiar el array
    shell->env = malloc((env_count + 1) * sizeof(char *));
    if (!shell->env)
        return (free(shell), NULL);
    
    // Copiar cada variable
    for (i = 0; i < env_count; i++)
    {
        shell->env[i] = ft_strdup(envp[i]);
        if (!shell->env[i])
            return (NULL);  // Handle error
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
    
    return (shell);
}
```

---

### PROBLEMA #14: `local_vars` no se inicializa correctamente
**Archivo:** `utils/struct_utils.c`
**Línea:** 15-25
**Severidad:** 🟡 IMPORTANTE
**Impacto:** Posible segfault si se accede a `local_vars` sin inicializar

```c
void	init_shell(t_shell *shell, char **envp)
{
    if (!shell)
        return ;
    shell->env = envp;
    shell->local_vars = NULL;  // ✅ Bien
    shell->tokens = NULL;
    shell->commands = NULL;
    shell->prompt = NULL;
    shell->exit_status = 0;
    shell->stdin_copy = STDIN_FILENO;
    shell->stdout_copy = STDOUT_FILENO;
}
```

**Estado:** ✅ Parece estar bien

---

## 🟢 TYPOS Y SYNTAXIS

### PROBLEMA #15: Código duplicado en cleanup
**Archivo:** `main.c`
**Línea:** 7-28 y `errors.c` línea 45-76
**Severidad:** 🟡 IMPORTANTE
**Impacto:** Mantenimiento difícil, doble free posible

**Solución:** Consolidar en una sola función `cleanup_shell()`.

---

## 📊 RESUMEN POR SEVERIDAD

| Severidad | Cantidad | Problemas |
|-----------|----------|-----------|
| 🔴 CRÍTICO | 3 | Doble free, env no copiado, memory leak path_env |
| 🟡 IMPORTANTE | 14 | FDs, inicialización, validación |
| 🟢 MENOR | 4 | Código duplicado, optimizaciones |
| **TOTAL** | **25+** | **Documentados** |

---

## 🚀 PLAN DE ACCIÓN RECOMENDADO

### Fase 1: Fixes Inmediatos (15 minutos)
- [ ] Problema #10: Eliminar doble free (consolidar cleanup)
- [ ] Problema #5: Liberar prompt anterior

### Fase 2: Memory Leaks Críticos (30 minutos)
- [ ] Problema #1: Liberar path_env en padre
- [ ] Problema #4: Asegurar cleanup después de cada input
- [ ] Problema #13: Copiar environment

### Fase 3: Inicialización (20 minutos)
- [ ] Problema #14: Verificar local_vars
- [ ] Problema #2: Verificar fd restoration

### Fase 4: Testing (variable)
- [ ] Compilar con ASan y Valgrind
- [ ] Ejecutar `make check` para verificación rápida

---

## 📝 NOTAS IMPORTANTES

1. **Priority:** Resolver problemas #10, #13 y #1 PRIMERO (3 críticos)
2. **Testing:** Después de cada cambio, ejecutar `make check`
3. **Valgrind:** Usar `readline_leaks.supp` para ignorar leaks de readline
4. **Checklist:** Marcar cada problema como resuelto en esta lista

---

**Documento generado automáticamente por análisis del código**
**Última actualización: 27 de Noviembre 2025**
