# Script PowerShell para subir a GitHub - Smart City Monitor
# Ejecutar: .\subir_github.ps1 TU_USUARIO_GITHUB

param(
    [Parameter(Mandatory=$true)]
    [string]$GitHubUser
)

Write-Host "`n========================================" -ForegroundColor Cyan
Write-Host "  SUBIR PROYECTO A GITHUB" -ForegroundColor Green
Write-Host "========================================`n" -ForegroundColor Cyan

$RepoName = "smart-city-monitor"
$RepoUrl = "https://github.com/$GitHubUser/$RepoName.git"

Write-Host "📦 Usuario GitHub: $GitHubUser" -ForegroundColor Yellow
Write-Host "📁 Repositorio: $RepoName" -ForegroundColor Yellow
Write-Host "🔗 URL: $RepoUrl`n" -ForegroundColor Yellow

# Verificar si ya existe el remote
$existingRemote = git remote 2>$null | Where-Object { $_ -eq "origin" }

if ($existingRemote) {
    Write-Host "🔄 Remote 'origin' ya existe, actualizando..." -ForegroundColor Yellow
    git remote remove origin
}

# Agregar remote
Write-Host "➕ Agregando remote..." -ForegroundColor Cyan
git remote add origin $RepoUrl

if ($LASTEXITCODE -ne 0) {
    Write-Host "❌ Error al agregar remote" -ForegroundColor Red
    exit 1
}

Write-Host "✅ Remote agregado correctamente`n" -ForegroundColor Green

# Verificar rama
$currentBranch = git branch --show-current

Write-Host "📍 Rama actual: $currentBranch" -ForegroundColor White

if ($currentBranch -ne "main") {
    Write-Host "🔄 Cambiando a rama 'main'..." -ForegroundColor Yellow
    git branch -M main
}

Write-Host "`n=========================================" -ForegroundColor Cyan
Write-Host "  ✅ LISTO PARA SUBIR A GITHUB" -ForegroundColor Green
Write-Host "=========================================`n" -ForegroundColor Cyan

Write-Host "PASOS FINALES:`n" -ForegroundColor Yellow

Write-Host "1️⃣  Crear repositorio en GitHub:" -ForegroundColor Cyan
Write-Host "   👉 https://github.com/new" -ForegroundColor White
Write-Host "   • Nombre: $RepoName" -ForegroundColor Gray
Write-Host "   • Visibilidad: Público" -ForegroundColor Gray
Write-Host "   • ❌ NO marcar 'Initialize with README'`n" -ForegroundColor Red

Write-Host "2️⃣  Ejecutar el push:" -ForegroundColor Cyan
Write-Host "   git push -u origin main`n" -ForegroundColor Yellow

Write-Host "3️⃣  Si GitHub pide autenticación:" -ForegroundColor Cyan
Write-Host "   • Usuario: $GitHubUser" -ForegroundColor White
Write-Host "   • Contraseña: Tu token personal (no tu contraseña)" -ForegroundColor White
Write-Host "   • Crear token: https://github.com/settings/tokens`n" -ForegroundColor White

Write-Host "=========================================`n" -ForegroundColor Cyan

Write-Host "💡 TIP: Para ejecutar el push directamente, usa:" -ForegroundColor Magenta
Write-Host "   .\subir_github.ps1 $GitHubUser -Push`n" -ForegroundColor Yellow
