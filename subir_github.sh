#!/bin/bash

# Script para subir a GitHub - Smart City Monitor
# Ejecutar: bash subir_github.sh TU_USUARIO_GITHUB

echo ""
echo "========================================"
echo "  SUBIR PROYECTO A GITHUB"
echo "========================================"
echo ""

# Verificar si se proporcionó el usuario
if [ -z "$1" ]; then
    echo "❌ ERROR: Debes proporcionar tu usuario de GitHub"
    echo ""
    echo "Uso: bash subir_github.sh TU_USUARIO_GITHUB"
    echo "Ejemplo: bash subir_github.sh johndoe"
    echo ""
    exit 1
fi

GITHUB_USER=$1
REPO_NAME="smart-city-monitor"
REPO_URL="https://github.com/$GITHUB_USER/$REPO_NAME.git"

echo "📦 Usuario GitHub: $GITHUB_USER"
echo "📁 Repositorio: $REPO_NAME"
echo "🔗 URL: $REPO_URL"
echo ""

# Verificar si ya existe el remote
if git remote | grep -q "^origin$"; then
    echo "🔄 Remote 'origin' ya existe, actualizando..."
    git remote remove origin
fi

# Agregar remote
echo "➕ Agregando remote..."
git remote add origin "$REPO_URL"

if [ $? -ne 0 ]; then
    echo "❌ Error al agregar remote"
    exit 1
fi

echo "✅ Remote agregado correctamente"
echo ""

# Verificar rama
CURRENT_BRANCH=$(git branch --show-current)
echo "📍 Rama actual: $CURRENT_BRANCH"

if [ "$CURRENT_BRANCH" != "main" ]; then
    echo "🔄 Cambiando a rama 'main'..."
    git branch -M main
fi

echo ""
echo "========================================="
echo "  ✅ LISTO PARA SUBIR A GITHUB"
echo "========================================="
echo ""
echo "PASOS FINALES:"
echo ""
echo "1️⃣  Crear repositorio en GitHub:"
echo "   👉 https://github.com/new"
echo "   • Nombre: $REPO_NAME"
echo "   • Visibilidad: Público"
echo "   • ❌ NO marcar 'Initialize with README'"
echo ""
echo "2️⃣  Ejecutar el push:"
echo "   git push -u origin main"
echo ""
echo "3️⃣  Si GitHub pide autenticación:"
echo "   • Usuario: $GITHUB_USER"
echo "   • Contraseña: Tu token personal (no tu contraseña)"
echo "   • Crear token: https://github.com/settings/tokens"
echo ""
echo "========================================="
echo ""
