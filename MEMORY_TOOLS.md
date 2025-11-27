# Herramientas para Detectar Memory Leaks y Still Reachable

## 1. **AddressSanitizer (ASan)** ⭐ RECOMENDADO
**Muy rápido y efectivo, detecta leaks, use-after-free, buffer overflows, etc.**

### Uso:
```bash
# Compilar con ASan
make asan

# Ejecutar normalmente (detecta automáticamente)
./minishell

# O con opciones adicionales
ASAN_OPTIONS=detect_leaks=1:abort_on_error=1 ./minishell
```

### Ventajas:
- ✅ Muy rápido (2-3x más rápido que Valgrind)
- ✅ Detecta leaks, use-after-free, buffer overflows
- ✅ Stack traces precisos
- ✅ Integrado en GCC/Clang

### Desventajas:
- ❌ Usa más memoria (~2x)
- ❌ Puede ser incompatible con algunas librerías (readline puede dar falsos positivos)

---

## 2. **LeakSanitizer (LSan)**
**Solo detecta memory leaks, más rápido que Valgrind**

### Uso:
```bash
# Compilar con LSan
make lsan

# Ejecutar
./minishell

# O con opciones
LSAN_OPTIONS=report_objects=1:use_unaligned=0 ./minishell
```

### Ventajas:
- ✅ Más rápido que Valgrind
- ✅ Solo se enfoca en leaks
- ✅ Menos overhead que ASan

---

## 3. **Valgrind** (ya lo conoces)
**La herramienta más completa pero más lenta**

### Uso mejorado:
```bash
# Compilar en modo debug
make debug

# Valgrind completo
valgrind --leak-check=full \
         --show-leak-kinds=all \
         --track-origins=yes \
         --suppressions=readline_leaks.supp \
         ./minishell

# Solo leaks (más rápido)
valgrind --leak-check=full --show-leak-kinds=all ./minishell

# Con Massif (análisis de uso de memoria)
valgrind --tool=massif ./minishell
ms_print massif.out.* > massif_report.txt
```

### Opciones útiles:
- `--leak-check=full`: Análisis detallado de leaks
- `--show-leak-kinds=all`: Muestra todos los tipos (definite, indirect, possible, reachable)
- `--track-origins=yes`: Rastrea el origen de valores no inicializados
- `--suppressions=file.supp`: Ignora falsos positivos (útil para readline)

---

## 4. **mtrace** (Glibc)
**Herramienta simple integrada en glibc**

### Uso:
```c
// En tu código, al inicio de main():
#include <mcheck.h>
mtrace();  // Activa el tracing
```

```bash
# Compilar normal
make

# Ejecutar con variable de entorno
MALLOC_TRACE=mtrace.log ./minishell

# Analizar
mtrace ./minishell mtrace.log
```

### Ventajas:
- ✅ Simple y ligero
- ✅ Integrado en glibc

### Desventajas:
- ❌ Menos detallado que Valgrind/ASan

---

## 5. **Dr. Memory** (Windows/Linux)
**Alternativa a Valgrind, más rápido en algunos casos**

### Uso:
```bash
# Instalar (Ubuntu)
sudo apt-get install drmemory

# Ejecutar
drmemory -leaks_only -- ./minishell
```

---

## 6. **Static Analysis Tools**

### cppcheck:
```bash
# Instalar
sudo apt-get install cppcheck

# Analizar
cppcheck --enable=all --suppress=missingIncludeSystem ./
```

### clang static analyzer:
```bash
# Analizar
scan-build make
```

---

## 7. **Combinación Recomendada para Minishell**

### Para desarrollo diario:
```bash
# 1. Compilar con ASan
make asan

# 2. Ejecutar con opciones
ASAN_OPTIONS=detect_leaks=1:abort_on_error=0:check_initialization_order=1 ./minishell
```

### Para análisis completo antes de entregar:
```bash
# 1. Compilar debug
make debug

# 2. Valgrind completo
valgrind --leak-check=full \
         --show-leak-kinds=all \
         --track-origins=yes \
         --suppressions=readline_leaks.supp \
         --verbose \
         ./minishell < test_input.txt
```

---

## 8. **Script Todo-en-Uno** ⭐ NUEVO

### Ejecutar todas las herramientas de una vez:
```bash
# Opción 1: Usar el script directamente
./check_memory.sh "echo test | exit"

# Opción 2: Usar el Makefile
make check-memory

# Opción 3: Con comando personalizado
./check_memory.sh "ls -la | grep test"
```

El script ejecuta:
1. ✅ AddressSanitizer
2. ✅ LeakSanitizer  
3. ✅ Valgrind completo
4. ✅ Resumen de leaks encontrados
5. ✅ Recomendaciones

Genera un archivo `valgrind_report.txt` con el reporte completo.

---

## 9. **Still Reachable - Qué hacer**

### Si Valgrind muestra "still reachable":
1. **No es un leak crítico** - La memoria está accesible pero no liberada
2. **Puede ser aceptable** si es al final del programa (el OS la libera)
3. **Para limpiarlo:**
   - Asegúrate de llamar `cleanup_shell()` antes de `exit()`
   - Libera todo en `null_input()` y al salir de `main()`
   - Usa `atexit()` para cleanup automático si es necesario

### Suprimir leaks de readline automáticamente:
El archivo `readline_leaks.supp` ya está configurado y se usa automáticamente en:
- `make check` - Usa supresiones automáticamente
- `make check-memory` - Usa supresiones automáticamente
- `./check_memory.sh` - Detecta y usa el archivo automáticamente

**Uso manual:**
```bash
valgrind --leak-check=full \
         --show-leak-kinds=all \
         --suppressions=readline_leaks.supp \
         ./minishell
```

**El archivo `readline_leaks.supp` incluye supresiones para:**
- ✅ Inicialización de readline (keymaps, terminal)
- ✅ History (add_history)
- ✅ Prompt expansion
- ✅ Undo operations
- ✅ Locale handling
- ✅ Screen size (setenv)
- ✅ Termcap/terminfo
- ✅ Ncurses leaks
- ✅ Completion leaks

---

## 10. **Comparación Rápida**

| Herramienta | Velocidad | Detección | Facilidad |
|------------|-----------|-----------|-----------|
| AddressSanitizer | ⭐⭐⭐⭐⭐ | ⭐⭐⭐⭐⭐ | ⭐⭐⭐⭐ |
| LeakSanitizer | ⭐⭐⭐⭐ | ⭐⭐⭐⭐ | ⭐⭐⭐⭐ |
| Valgrind | ⭐⭐ | ⭐⭐⭐⭐⭐ | ⭐⭐⭐ |
| mtrace | ⭐⭐⭐⭐ | ⭐⭐ | ⭐⭐⭐⭐⭐ |

---

## Tips Finales

1. **Usa ASan para desarrollo rápido** - Detecta la mayoría de problemas
2. **Usa Valgrind antes de entregar** - Análisis más completo
3. **Crea un archivo `.supp` para readline** - Evita falsos positivos
4. **Testa con diferentes inputs** - Algunos leaks solo aparecen con ciertos comandos
5. **Revisa "still reachable" al final** - Asegúrate de liberar todo en cleanup

