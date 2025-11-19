@echo off
echo.
echo ========================================
echo   CREANDO REPOSITORIO EN GITHUB
echo ========================================
echo.

REM Abrir GitHub para crear repositorio
start https://github.com/new?name=smart-city-monitor^&description=Sistema+de+monitoreo+acustico+con+ESP32^&visibility=public

echo Repositorio configurado para crearse...
echo.
echo PASOS:
echo 1. Se abrira GitHub en tu navegador
echo 2. Click en "Create repository" (boton verde)
echo 3. Espera 5 segundos y vuelve aqui
echo.
pause

echo.
echo Configurando remote...
git remote remove origin 2>nul
git remote add origin https://github.com/YOUR_USERNAME/smart-city-monitor.git

echo.
echo Subiendo a GitHub...
git push -u origin main

echo.
echo ========================================
echo   LISTO! Repositorio en GitHub
echo ========================================
echo.
pause
