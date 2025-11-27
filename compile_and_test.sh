#!/bin/bash

cd /home/axgimene/gg

echo "🔨 Limpiando..."
make clean

echo ""
echo "🏗️ Compilando..."
make 2>&1 | tail -30

if [ $? -eq 0 ]; then
    echo ""
    echo "✅ Compilación exitosa"
    echo ""
    echo "🧪 Testeando..."
    echo "exit" | ./minishell
    echo ""
    echo "✅ Test básico completado"
else
    echo ""
    echo "❌ Error en compilación"
fi
