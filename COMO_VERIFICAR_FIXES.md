# 🧪 CÓMO VERIFICAR LOS FIXES

**Para compilar y verificar que los fixes funcionan**

---

## 📋 OPCIÓN 1: Compilación Manual (WSL)

### Paso 1: Abrir terminal en WSL
```bash
# En tu terminal WSL/Linux:
cd /home/axgimene/gg
```

### Paso 2: Limpiar y compilar
```bash
make clean
make
```

**Resultado esperado:** Compilación sin errores

### Paso 3: Test rápido
```bash
# Debería salir sin crashes
echo "exit" | ./minishell
```

**Resultado esperado:** Sale limpiamente con "exit"

---

## 📋 OPCIÓN 2: Test con ASan (Rápido)

```bash
cd /home/axgimene/gg

# Compilar con ASan
make asan

# Ejecutar con detección de leaks
ASAN_OPTIONS=detect_leaks=1:abort_on_error=0 timeout 3 ./minishell <<< "exit"
```

**Resultado esperado:**
- Sin mensajes de "leak detected"
- Sin "AddressSanitizer:SEGV"
- Sale con "exit"

---

## 📋 OPCIÓN 3: Test con Valgrind (Completo)

```bash
cd /home/axgimene/gg

# Compilar en debug
make debug

# Ejecutar con Valgrind (puede tardar)
valgrind --leak-check=full \
  --show-leak-kinds=all \
  --suppressions=readline_leaks.supp \
  --quiet ./minishell <<< "exit"
```

**Resultado esperado:**
- "definitely lost: 0 bytes"
- "indirectly lost: 0 bytes"
- No hay "Invalid read" o "Invalid write"

---

## 🧪 COMANDOS DE VERIFICACIÓN RÁPIDA

### Test 1: Compilar
```bash
make clean && make
echo $?  # Debe mostrar 0
```

### Test 2: Exit sin crash
```bash
./minishell <<< "exit"
echo "Exit status: $?"  # Debe mostrar exit status normal
```

### Test 3: Ctrl+D sin crash
```bash
./minishell <<< ""  # Simula Ctrl+D
echo "Exit status: $?"
```

### Test 4: Comando inválido (memory leak)
```bash
ASAN_OPTIONS=detect_leaks=1 ./minishell <<< "invalidcommand; exit"
# Debe no reportar leaks
```

### Test 5: Environment aislado
```bash
./minishell << 'EOF'
export TEST_VAR=hello
echo $TEST_VAR
exit
EOF
# En el shell: debe mostrar "hello"

# En la terminal actual: TEST_VAR no debe existir
echo $TEST_VAR  # Debe estar vacío
```

---

## ✅ CHECKLIST DE VERIFICACIÓN

- [ ] `make clean && make` compila sin errores
- [ ] `./minishell <<< "exit"` sale sin crash
- [ ] `echo "" | ./minishell` (Ctrl+D simulado) no crashea
- [ ] ASan no reporta leaks nuevos
- [ ] Valgrind reporta "definitely lost: 0 bytes"
- [ ] Comandos básicos funcionan (echo, cd, pwd)
- [ ] Export no modifica el SO
- [ ] No hay segmentation faults

---

## 🐛 SI HAY ERRORES

### Error: "make: command not found"
→ Asegúrate de estar en WSL/Linux, no en PowerShell de Windows

### Error: undefined reference
→ Probablemente algún archivo C no se compiló
→ Verifica que no haya errores de sintaxis

### Crash al hacer exit
→ El doble free no se arregló bien
→ Revisa `main.c` línea 7-12 y `errors.c` línea 45-51

### Memory leak reportado por Valgrind
→ Valgrind puede reportar leaks de readline (aceptable)
→ Si says "definitely lost", ver PROBLEMAS_ENCONTRADOS.md

### Command not found
→ Es esperado si el comando no existe
→ Verifica que sea un error de "command not found", no un crash

---

## 🎯 FLUJO DE VERIFICACIÓN RECOMENDADO

### 1. Quick Check (2 minutos)
```bash
cd /home/axgimene/gg
make clean && make && ./minishell <<< "exit"
```

### 2. ASan Check (5 minutos)
```bash
make asan
ASAN_OPTIONS=detect_leaks=1 timeout 3 ./minishell <<< "invalidcommand; exit"
```

### 3. Full Valgrind Check (10 minutos)
```bash
make debug
valgrind --leak-check=full --quiet ./minishell <<< "exit"
```

### 4. Interactive Test (5 minutos)
```bash
./minishell
# Escribir: echo hello
# Escribir: cd /tmp
# Escribir: pwd
# Escribir: export TEST=1
# Escribir: exit
# Verificar que todo funciona y no crashea
```

---

## 📊 MÉTRICAS ESPERADAS

| Métrica | Antes | Después | Mejora |
|---------|-------|---------|--------|
| Crashes al exit | 1 | 0 | ✅ 100% |
| Memory leaks | 5+ | 0-1 | ✅ ~80% |
| FD leaks | 20+ | ~5 | ✅ ~75% |
| SO modifications | Sí | No | ✅ 100% |

---

## 💾 GUARDAR PROGRESO

Después de verificar que todo funciona:

```bash
cd /home/axgimene/gg

# Ver cambios
git diff

# Guardar
git add -A
git commit -m "fix: 4 fixes críticos implementados

- Eliminar doble free en cleanup (FIX #1, #2)
- Liberar path_env en padre (FIX #3)
- Copiar environment (FIX #4)

Arregla: crashes al salir, memory leaks, SO modifications
"
```

---

## 🚀 SIGUIENTE

Una vez verificado que todo funciona:

1. **Opcional:** Implementar más fixes (ver GUIA_RAPIDA_10_FIXES.md)
2. **Recomendado:** Ejecutar `./check_memory.sh` para análisis completo
3. **Final:** Revisar PLAN_ACCION_DETALLADO.md para completitud

---

## 📞 DUDAS FRECUENTES

**P: ¿Necesito compilar en WSL?**  
R: Sí, `make` solo funciona en Linux/WSL

**P: ¿Cuánto tarda la compilación?**  
R: ~5-10 segundos en la mayoría de sistemas

**P: ¿Cuánto tarda Valgrind?**  
R: ~20-30 segundos, es lento pero exhaustivo

**P: ¿Los leaks de readline son un problema?**  
R: No, readline es de la librería. Usa `readline_leaks.supp` para ignorarlos

**P: ¿Qué si sigue crasheando?**  
R: Revisa PROBLEMAS_ENCONTRADOS.md para diagnosticar qué pasó

---

**Documento generado:** 27 de Noviembre 2025  
**Objetivo:** Verificación simple de los fixes  
**Tiempo estimado:** 20 minutos para todos los tests
