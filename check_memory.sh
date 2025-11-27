#!/bin/bash

# Script para verificar memory leaks con múltiples herramientas
# Uso: ./check_memory.sh [comando a testear]

set -e

# Colores para output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Comando por defecto
TEST_CMD="${1:-echo 'test' | exit}"

echo -e "${BLUE}========================================${NC}"
echo -e "${BLUE}  MEMORY LEAK DETECTION - FULL REPORT${NC}"
echo -e "${BLUE}========================================${NC}"
echo ""

# Función para mostrar sección
show_section() {
    echo ""
    echo -e "${YELLOW}━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━${NC}"
    echo -e "${YELLOW}  $1${NC}"
    echo -e "${YELLOW}━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━${NC}"
    echo ""
}

# 1. AddressSanitizer
show_section "1. AddressSanitizer (ASan)"
if make asan > /dev/null 2>&1; then
    echo -e "${GREEN}✓ Compilado con AddressSanitizer${NC}"
    echo "Ejecutando..."
    echo ""
    ASAN_OPTIONS=detect_leaks=1:abort_on_error=0:check_initialization_order=1 \
        timeout 5 ./minishell <<< "$TEST_CMD" 2>&1 | head -100
    echo ""
    echo -e "${GREEN}✓ AddressSanitizer completado${NC}"
else
    echo -e "${RED}✗ Error al compilar con AddressSanitizer${NC}"
fi

# 2. LeakSanitizer
show_section "2. LeakSanitizer (LSan)"
if make lsan > /dev/null 2>&1; then
    echo -e "${GREEN}✓ Compilado con LeakSanitizer${NC}"
    echo "Ejecutando..."
    echo ""
    LSAN_OPTIONS=report_objects=1:use_unaligned=0 \
        timeout 5 ./minishell <<< "$TEST_CMD" 2>&1 | head -100
    echo ""
    echo -e "${GREEN}✓ LeakSanitizer completado${NC}"
else
    echo -e "${RED}✗ Error al compilar con LeakSanitizer${NC}"
fi

# 3. Valgrind
show_section "3. Valgrind (Análisis Completo)"
if make debug > /dev/null 2>&1; then
    echo -e "${GREEN}✓ Compilado en modo debug${NC}"
    echo "Ejecutando Valgrind (esto puede tardar)..."
    echo ""
    
    # Usar archivo de supresiones de readline automáticamente
    SUPP_FILE=""
    if [ -f "readline_leaks.supp" ]; then
        SUPP_FILE="--suppressions=readline_leaks.supp"
        echo -e "${GREEN}✓ Usando supresiones de readline_leaks.supp${NC}"
    else
        echo -e "${YELLOW}⚠️  Archivo readline_leaks.supp no encontrado${NC}"
        echo "   Los leaks de readline aparecerán en el reporte"
    fi
    
    valgrind --leak-check=full \
             --show-leak-kinds=all \
             --track-origins=yes \
             --verbose \
             --error-limit=no \
             $SUPP_FILE \
             ./minishell <<< "$TEST_CMD" 2>&1 | tee valgrind_report.txt | tail -150
    echo ""
    echo -e "${GREEN}✓ Valgrind completado (reporte guardado en valgrind_report.txt)${NC}"
else
    echo -e "${RED}✗ Error al compilar en modo debug${NC}"
fi

# 4. Resumen de leaks encontrados
show_section "4. RESUMEN DE LEAKS"
echo "Buscando leaks en los reportes..."
echo ""

# Contar leaks en Valgrind
if [ -f "valgrind_report.txt" ]; then
    DEFINITE_LEAKS=$(grep -c "definitely lost" valgrind_report.txt 2>/dev/null || echo "0")
    INDIRECT_LEAKS=$(grep -c "indirectly lost" valgrind_report.txt 2>/dev/null || echo "0")
    POSSIBLE_LEAKS=$(grep -c "possibly lost" valgrind_report.txt 2>/dev/null || echo "0")
    STILL_REACHABLE=$(grep -c "still reachable" valgrind_report.txt 2>/dev/null || echo "0")
    
    echo -e "Valgrind Report:"
    echo -e "  ${RED}Definitely lost:${NC} $DEFINITE_LEAKS"
    echo -e "  ${YELLOW}Indirectly lost:${NC} $INDIRECT_LEAKS"
    echo -e "  ${YELLOW}Possibly lost:${NC} $POSSIBLE_LEAKS"
    echo -e "  ${BLUE}Still reachable:${NC} $STILL_REACHABLE"
    echo ""
    
    if [ "$DEFINITE_LEAKS" -gt 0 ] || [ "$INDIRECT_LEAKS" -gt 0 ]; then
        echo -e "${RED}⚠️  SE ENCONTRARON LEAKS CRÍTICOS${NC}"
    elif [ "$POSSIBLE_LEAKS" -gt 0 ]; then
        echo -e "${YELLOW}⚠️  Posibles leaks detectados${NC}"
    elif [ "$STILL_REACHABLE" -gt 0 ]; then
        echo -e "${BLUE}ℹ️  Solo memoria 'still reachable' (normalmente aceptable)${NC}"
    else
        echo -e "${GREEN}✓ No se encontraron leaks${NC}"
    fi
fi

# 5. Información adicional
show_section "5. INFORMACIÓN ADICIONAL"
echo "Archivos generados:"
echo "  - valgrind_report.txt (reporte completo de Valgrind)"
echo ""
echo "Para ver el reporte completo de Valgrind:"
echo "  cat valgrind_report.txt"
echo ""
echo "Para buscar solo leaks definitivos:"
echo "  grep 'definitely lost' valgrind_report.txt"
echo ""
echo "Para buscar still reachable:"
echo "  grep 'still reachable' valgrind_report.txt"
echo ""

# 6. Recomendaciones
show_section "6. RECOMENDACIONES"
if [ -f "valgrind_report.txt" ]; then
    if grep -q "definitely lost" valgrind_report.txt && ! grep -q "definitely lost: 0 bytes" valgrind_report.txt; then
        echo -e "${RED}⚠️  ACCIÓN REQUERIDA:${NC}"
        echo "   - Revisa los leaks 'definitely lost' en valgrind_report.txt"
        echo "   - Asegúrate de liberar toda la memoria con free()"
        echo "   - Verifica que free_tokens() y free_commands() se llamen correctamente"
    fi
    
    if grep -q "still reachable" valgrind_report.txt && ! grep -q "still reachable: 0 bytes" valgrind_report.txt; then
        echo -e "${BLUE}ℹ️  Still Reachable detectado:${NC}"
        echo "   - Esto NO es un leak crítico"
        echo "   - La memoria está accesible pero no liberada"
        echo "   - Puede ser aceptable si es al final del programa"
        echo "   - Para limpiarlo, asegúrate de llamar cleanup_shell() antes de exit()"
    fi
fi

echo ""
echo -e "${GREEN}========================================${NC}"
echo -e "${GREEN}  ANÁLISIS COMPLETADO${NC}"
echo -e "${GREEN}========================================${NC}"

