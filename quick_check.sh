#!/bin/bash

# Comando rápido para ver todo - versión simplificada
# Uso: ./quick_check.sh

echo "🔍 Ejecutando análisis completo de memory leaks..."
echo ""

# Compilar con AddressSanitizer y ejecutar
echo "1️⃣ AddressSanitizer:"
make asan > /dev/null 2>&1 && ASAN_OPTIONS=detect_leaks=1 timeout 3 ./minishell <<< "echo test" 2>&1 | grep -E "(leak|error|SUMMARY)" | head -20
echo ""

# Compilar debug y ejecutar Valgrind
echo "2️⃣ Valgrind:"
make debug > /dev/null 2>&1
if [ -f "readline_leaks.supp" ]; then
    valgrind --leak-check=full --show-leak-kinds=all --suppressions=readline_leaks.supp --quiet ./minishell <<< "echo test" 2>&1 | grep -E "(definitely|indirectly|possibly|still reachable|ERROR SUMMARY)" | head -10
else
    valgrind --leak-check=full --show-leak-kinds=all --quiet ./minishell <<< "echo test" 2>&1 | grep -E "(definitely|indirectly|possibly|still reachable|ERROR SUMMARY)" | head -10
fi
echo ""

echo "✅ Análisis completado"
echo "💡 Para ver el reporte completo: make check-memory"

