# 🚨 PROBLEMAS CRÍTICOS - VISTA RÁPIDA

**LEER ESTO PRIMERO - 5 MINUTOS**

---

## ❌ PROBLEMA #1: CRASH AL SALIR

**Archivo:** `main.c` + `utils/errors.c`  
**¿Qué pasa?** El shell CRASHEA cuando haces `exit` o Ctrl+D

```c
// ❌ MAL: Doble free
cleanup_on_exit() {
    cleanup_shell(g_shell);  // Libera todo
}

null_input() {
    cleanup_shell(g_shell);  // Intenta liberar DE NUEVO → CRASH!
    exit(0);
}

// ✅ BIEN: Una sola vez
cleanup_on_exit() {
    cleanup_shell(g_shell);  // Una vez
}

null_input() {
    // null_input hace lo suyo
    // cleanup_on_exit lo limpia
    exit(0);
}
```

**Solución:** Consolidar TODO en `cleanup_shell()` e llamarla una sola vez

---

## ❌ PROBLEMA #2: MEMORY LEAK EN CADA COMANDO

**Archivo:** `execute/execution_single_command.c`  
**¿Qué pasa?** ~200 bytes se pierden cada vez que escribes un comando inválido

```c
// ❌ MAL: path_env nunca se libera en el padre
void just_execute_it_man(t_shell *shell) {
    path_env = get_path_values(...);  // MALLOC aquí
    
    pid = fork();
    if (pid == 0) {  // HIJO
        // El hijo sí lo libera
        free_path_env(path_env);
        exit(127);
    }
    // ❌ PADRE: path_env se pierde aquí!
}

// ✅ BIEN: Liberar en ambos lados
void just_execute_it_man(t_shell *shell) {
    path_env = get_path_values(...);
    
    pid = fork();
    if (pid == 0) {  // HIJO
        free_path_env(path_env);  // ✅ En hijo
        exit(127);
    } else if (pid > 0) {  // PADRE
        free_path_env(path_env);  // ✅ En padre - NUEVA
        status_wait(pid, status);
    }
}
```

**Solución:** Liberar `path_env` en el padre después de fork

---

## ❌ PROBLEMA #3: SHELL MODIFICA EL SO

**Archivo:** `utils/struct_utils.c`  
**¿Qué pasa?** Cuando haces `export HOME=/tmp` en el shell, el SO se modifica

```c
// ❌ MAL: Apunta directo al environment del SO
void init_shell(t_shell *shell, char **envp) {
    shell->env = envp;  // DIRECTO - Modifica el SO!
}

// ✅ BIEN: Copiar cada variable
void init_shell(t_shell *shell, char **envp) {
    int count = 0;
    while (envp[count]) count++;
    
    shell->env = malloc((count + 1) * sizeof(char *));
    
    for (int i = 0; i < count; i++)
        shell->env[i] = ft_strdup(envp[i]);  // ✅ COPIAR
    
    shell->env[count] = NULL;
}
```

**Solución:** Copiar el environment en `init_shell()` y liberar en `cleanup_shell()`

---

## ❌ PROBLEMA #4: MEMORY LEAK EN PROMPT

**Archivo:** `main.c`  
**¿Qué pasa?** ~100 bytes se pierden por cada iteración del loop principal

```c
// ❌ MAL: Anterior prompt se sobrescribe
while (1) {
    shell.prompt = format_cwd(...);  // Anterior se pierde
    shell.prompt = ft_strdup("...");  // Anterior se pierde
    input = readline(shell.prompt);
}

// ✅ BIEN: Liberar antes de asignar
while (1) {
    if (shell.prompt) {           // ✅ NUEVA
        free(shell.prompt);        // ✅ NUEVA
        shell.prompt = NULL;       // ✅ NUEVA
    }
    
    shell.prompt = format_cwd(...);
    input = readline(shell.prompt);
}
```

**Solución:** Liberar el prompt anterior ANTES de asignarlo de nuevo

---

## 📊 RESUMEN DE PROBLEMAS

### 🔴 CRÍTICOS (Arreglar AHORA)
```
✗ Doble free en cleanup         → CRASH
✗ path_env leak                 → ~200 bytes/cmd
✗ Environment no copiado        → Modifica SO
✗ Prompt sobrescrito            → ~100 bytes/loop
```

### 🟡 IMPORTANTES
```
⚠ FDs no se cierran             → ~50 KB después de muchos pipes
⚠ Variables no inicializadas    → Posible segfault
⚠ Validaciones faltantes        → Acceso a NULL
⚠ Exit status incorrecto        → $? da valores equivocados
```

### 🟢 MENORES
```
○ Código duplicado              → Mantenimiento difícil
○ Falta restore de FDs          → Redirecciones pueden fallar
○ Validación de av              → Posible segfault
```

---

## ⏱️ TIEMPO PARA ARREGLAR

| Problema | Tiempo | Dificultad | Impacto |
|----------|--------|-----------|---------|
| Doble free | 5 min | ⭐ Fácil | 🔴 CRÍTICO |
| path_env | 10 min | ⭐⭐ Medio | 🔴 CRÍTICO |
| Environment | 15 min | ⭐⭐ Medio | 🔴 CRÍTICO |
| Prompt | 3 min | ⭐ Fácil | 🔴 CRÍTICO |
| FDs | 20 min | ⭐⭐⭐ Difícil | 🟡 IMPORTANTE |

---

## 🚀 COMIENZA AQUÍ

### Paso 1: Entender (5 min)
Leer este documento

### Paso 2: Backup (2 min)
```bash
cd /home/axgimene/gg
git add -A
git commit -m "backup: antes de arreglar bugs"
```

### Paso 3: Arreglar (35 min)
Abre `GUIA_RAPIDA_10_FIXES.md`  
Implementa los FIX #1, #2, #3, #4

### Paso 4: Testear (10 min)
```bash
make clean && make
make asan
./minishell <<< "exit"
```

### Paso 5: Validar (20 min)
```bash
make debug
valgrind --leak-check=full --suppressions=readline_leaks.supp ./minishell <<< "exit"
```

---

## ✅ CÓMO SABER QUE ESTÁ ARREGLADO

```
✅ Compilar sin errores
✅ No crashes al hacer exit
✅ No crashes con Ctrl+D
✅ Valgrind reporta "definitely lost: 0 bytes"
✅ Comandos básicos funcionan
✅ Shell responde rápidamente
```

---

## 📚 DOCUMENTOS

- `ANALISIS_COMPLETO.md` - Visión general
- `GUIA_RAPIDA_10_FIXES.md` - Los 10 fixes (leer después)
- `PROBLEMAS_ENCONTRADOS.md` - Análisis técnico completo
- `PLAN_ACCION_DETALLADO.md` - Plan paso a paso

---

## 🎯 SIGUIENTE

**Abre ahora:** `GUIA_RAPIDA_10_FIXES.md`

Ahí encontrarás el código exacto para cada fix.

---

Generado: 27 de Noviembre 2025
