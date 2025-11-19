# 🚀 Desplegar desde GitHub a Hostinger

Guía completa para desplegar tu proyecto Smart City Monitor desde GitHub a Hostinger con auto-deployment.

## 📋 Requisitos Previos

- ✅ Cuenta de **GitHub** (gratis)
- ✅ Cuenta de **Hostinger** (Plan Business ~$3.99/mes)
- ✅ Repositorio **público** o **privado**
- ✅ **Git** instalado en tu PC

## 🎯 Ventajas de GitHub Deployment

✨ **Auto-deployment**: Cada push actualiza automáticamente tu sitio  
🔄 **Control de versiones**: Historial completo de cambios  
👥 **Colaboración**: Múltiples desarrolladores  
🔙 **Rollback fácil**: Volver a versiones anteriores  
📝 **Documentación**: README y wikis integradas  

---

## PARTE 1: Preparar Repositorio GitHub

### Paso 1.1: Inicializar Git

```bash
# Navegar a tu proyecto
cd C:\Users\fleal\OneDrive\Desktop\smart_city_dashboard_fixed

# Inicializar repositorio
git init

# Agregar todos los archivos
git add .

# Primer commit
git commit -m "Initial commit: Smart City Monitor"
```

### Paso 1.2: Crear Repositorio en GitHub

1. Ve a **github.com** e inicia sesión
2. Click en **"New repository"** (botón verde +)
3. Configuración:
   - **Nombre**: `smart-city-monitor`
   - **Descripción**: "Sistema de monitoreo acústico con ESP32"
   - **Visibilidad**: Público (o Privado si prefieres)
   - ❌ NO marcar "Initialize with README" (ya tienes uno)
4. Click en **"Create repository"**

### Paso 1.3: Conectar y Subir

```bash
# Conectar repositorio remoto (cambia TU_USUARIO por tu usuario GitHub)
git remote add origin https://github.com/TU_USUARIO/smart-city-monitor.git

# Cambiar nombre de rama a main (GitHub usa main, no master)
git branch -M main

# Subir código
git push -u origin main
```

**🔐 Autenticación**: GitHub pedirá tu usuario y contraseña. Si tienes 2FA activo, necesitas crear un **Personal Access Token**:

1. GitHub → **Settings** → **Developer settings** → **Personal access tokens** → **Tokens (classic)**
2. **Generate new token**
3. Marcar: `repo` (acceso completo)
4. Copiar token (guárdalo, solo se muestra una vez)
5. Usar token como contraseña en Git

### Paso 1.4: Verificar Subida

```bash
# Ver estado
git status

# Ver repositorio remoto
git remote -v

# Ver commits
git log --oneline
```

Abre tu repositorio en GitHub: `https://github.com/TU_USUARIO/smart-city-monitor`  
Deberías ver todos tus archivos.

---

## PARTE 2: Conectar GitHub con Hostinger

### Paso 2.1: Acceder a hPanel

1. Ingresa a **panel.hostinger.com**
2. Selecciona tu dominio
3. Ve a **"Git"** en el menú lateral (sección Avanzado)

### Paso 2.2: Crear Deployment GitHub

1. Click en **"Crear nuevo despliegue desde Git"**
2. Seleccionar proveedor: **GitHub**
3. Click en **"Autorizar"**
4. Se abrirá ventana de GitHub pidiendo permisos
5. Autorizar Hostinger a acceder a tus repositorios

### Paso 2.3: Configurar Repositorio

**Configuración del Backend (Node.js)**:

```
┌─────────────────────────────────────────┐
│ Repositorio:  TU_USUARIO/smart-city-monitor
│ Rama:         main
│ Carpeta:      backend/
│ Destino:      domains/tudominio.com/public_html/api
│ Auto-deploy:  ✅ Activado
└─────────────────────────────────────────┘
```

**Configuración del Frontend (HTML/CSS/JS)**:

```
┌─────────────────────────────────────────┐
│ Repositorio:  TU_USUARIO/smart-city-monitor
│ Rama:         main
│ Carpeta:      web-hosting/
│ Destino:      domains/tudominio.com/public_html/
│ Auto-deploy:  ✅ Activado
└─────────────────────────────────────────┘
```

### Paso 2.4: Configurar Scripts de Despliegue

Hostinger ejecutará estos comandos automáticamente:

**Para Backend** (carpeta `api/`):
```bash
# Script post-deployment
npm install --production
pm2 restart all || pm2 start server.js --name smart-city-api
```

**Para Frontend** (carpeta raíz):
```bash
# No requiere build, solo copiar archivos
# Hostinger lo hace automáticamente
```

---

## PARTE 3: Configurar Node.js en Hostinger

### Paso 3.1: Crear Aplicación Node.js

1. hPanel → **"Node.js"** (menú Avanzado)
2. Click en **"Crear aplicación"**
3. Configuración:

```
┌─────────────────────────────────────────┐
│ Versión Node.js:  18.x
│ Modo aplicación:  Producción
│ Directorio raíz:  /domains/tudominio.com/public_html/api
│ URL aplicación:   https://tudominio.com/api
│ Archivo inicio:   server.js
│ Puerto:           3000
└─────────────────────────────────────────┘
```

4. Click en **"Crear"**

### Paso 3.2: Configurar Variables de Entorno

En la misma página de Node.js, sección **"Variables de entorno"**:

```
ESP32_IP=abc123.ngrok-free.app
PORT=3000
NODE_ENV=production
ESP32_TIMEOUT=3000
ESP32_CHECK_INTERVAL=30000
```

**⚠️ Importante**: No incluyas `.env` en GitHub (está en `.gitignore`)

---

## PARTE 4: Configurar Frontend

### Paso 4.1: Actualizar app.js

**ANTES de hacer push**, edita `web-hosting/app.js` línea 2:

```javascript
const BACKEND_URL = 'https://tudominio.com/api'; // Cambia tudominio.com
```

### Paso 4.2: Commit y Push

```bash
# Agregar cambios
git add web-hosting/app.js

# Commit
git commit -m "Update: Backend URL for production"

# Push (activará auto-deployment)
git push origin main
```

### Paso 4.3: Verificar Despliegue

1. GitHub → Tu repo → **"Actions"** (tab)
2. Verás el workflow ejecutándose
3. Hostinger → **"Git"** → Ver último despliegue
4. Estado: ✅ **Desplegado** (color verde)

---

## PARTE 5: Exponer ESP32 (Ngrok/Cloudflare)

### Opción A: Ngrok (Rápido)

```bash
# En tu PC donde está el ESP32
ngrok http 192.168.1.100:80

# Copia la URL generada: https://abc123.ngrok-free.app
```

**Configurar en Hostinger**:
1. hPanel → Node.js → Tu aplicación
2. Variables de entorno
3. Editar `ESP32_IP` → `abc123.ngrok-free.app`
4. Guardar → Reiniciar aplicación

### Opción B: Cloudflare Tunnel (Permanente)

```bash
# Instalar cloudflared
# Windows: https://github.com/cloudflare/cloudflared/releases

# Login
cloudflared tunnel login

# Crear túnel
cloudflared tunnel create smart-city-esp32

# Configurar DNS
cloudflared tunnel route dns smart-city-esp32 esp32.tudominio.com

# Ejecutar (mantener corriendo)
cloudflared tunnel run --url http://192.168.1.100:80 smart-city-esp32
```

**Configurar en Hostinger**:
```
ESP32_IP=esp32.tudominio.com
```

---

## PARTE 6: SSL y Dominio

### Paso 6.1: Activar SSL Gratis

1. hPanel → **"Seguridad"** → **"SSL"**
2. Seleccionar: **"Let's Encrypt SSL"**
3. Click en **"Instalar"**
4. Esperar 5-10 minutos
5. Verificar: `https://tudominio.com` (candado verde)

### Paso 6.2: Forzar HTTPS

El archivo `.htaccess` ya incluye redirección HTTPS:

```apache
RewriteCond %{HTTPS} off
RewriteRule ^(.*)$ https://%{HTTP_HOST}%{REQUEST_URI} [L,R=301]
```

---

## 🔄 Workflow de Desarrollo

### Hacer Cambios

```bash
# 1. Editar archivos localmente
code web-hosting/app.js

# 2. Ver cambios
git status
git diff

# 3. Agregar cambios
git add .

# 4. Commit con mensaje descriptivo
git commit -m "Fix: Corregir gráfico de frecuencias"

# 5. Push (activa auto-deployment)
git push origin main

# 6. Verificar en GitHub Actions
# https://github.com/TU_USUARIO/smart-city-monitor/actions

# 7. Esperar 1-2 minutos
# 8. ¡Cambios en vivo! https://tudominio.com
```

### Crear Ramas (Opcional)

```bash
# Crear rama de desarrollo
git checkout -b desarrollo

# Hacer cambios y commits
git add .
git commit -m "Feature: Nueva métrica promedio"

# Push rama
git push origin desarrollo

# En Hostinger, crear deployment separado apuntando a rama 'desarrollo'
# URL: https://dev.tudominio.com
```

### Revertir Cambios

```bash
# Ver historial
git log --oneline

# Revertir último commit
git revert HEAD
git push origin main

# Volver a commit específico
git reset --hard abc1234
git push origin main --force
```

---

## 📊 Monitoreo y Logs

### Ver Logs en Hostinger

```bash
# SSH a Hostinger
ssh u123456789@tudominio.com -p 65002

# Ver logs Node.js
cd domains/tudominio.com/logs
tail -f error.log
tail -f access.log

# Ver logs de PM2 (gestor de procesos)
pm2 logs smart-city-api
pm2 status
```

### GitHub Actions (CI/CD)

Tu repositorio puede incluir tests automáticos:

Crea `.github/workflows/test.yml`:

```yaml
name: Tests

on: [push, pull_request]

jobs:
  test:
    runs-on: ubuntu-latest
    steps:
      - uses: actions/checkout@v3
      - uses: actions/setup-node@v3
        with:
          node-version: '18'
      - run: cd backend && npm install
      - run: cd backend && npm test
```

---

## 🛠️ Comandos Útiles

### Git

```bash
# Ver estado
git status

# Ver cambios
git diff

# Ver ramas
git branch -a

# Cambiar rama
git checkout main

# Actualizar desde GitHub
git pull origin main

# Ver commits
git log --oneline --graph

# Deshacer cambios locales
git restore archivo.js
git restore .

# Limpiar archivos no rastreados
git clean -fd
```

### Hostinger SSH

```bash
# Conectar
ssh u123456789@tudominio.com -p 65002

# Navegar a proyecto
cd domains/tudominio.com/public_html/api

# Ver archivos
ls -la

# Ver variables de entorno
cat .env

# Reiniciar Node.js
pm2 restart all

# Ver uso de recursos
pm2 monit

# Actualizar manualmente
git pull origin main
npm install
pm2 restart all
```

---

## ✅ Verificación Final

### Checklist de Despliegue

- [ ] Repositorio GitHub creado y código subido
- [ ] GitHub conectado con Hostinger
- [ ] Node.js configurado en Hostinger (puerto 3000)
- [ ] Variables de entorno configuradas
- [ ] Frontend y backend desplegados
- [ ] `BACKEND_URL` actualizado en `app.js`
- [ ] SSL activado (HTTPS)
- [ ] ESP32 expuesto con Ngrok/Cloudflare
- [ ] `ESP32_IP` configurado en backend
- [ ] Dashboard accesible: `https://tudominio.com`
- [ ] API respondiendo: `https://tudominio.com/api/status`

### Test de Endpoints

```bash
# Status del backend
curl https://tudominio.com/api/status

# Datos acústicos
curl https://tudominio.com/api/data

# Frecuencias
curl https://tudominio.com/api/frequency

# Verificar ESP32
curl https://tudominio.com/api/check-esp32
```

### Test del Frontend

1. Abrir: `https://tudominio.com`
2. Verificar indicador de conexión (esquina superior derecha)
3. Comprobar que gráficos se actualicen cada 2 segundos
4. Probar botones de control
5. Verificar responsive (F12 → modo móvil)

---

## 🎉 ¡Listo!

Tu Smart City Monitor está desplegado y configurado con:

✅ **Auto-deployment** desde GitHub  
✅ **Backend Node.js** con proxy ESP32  
✅ **Frontend responsive** con visualizaciones  
✅ **SSL gratis** (HTTPS)  
✅ **Modo simulación** de respaldo  

### URLs Finales

```
Dashboard:     https://tudominio.com
API Status:    https://tudominio.com/api/status
API Data:      https://tudominio.com/api/data
API Frequency: https://tudominio.com/api/frequency
GitHub Repo:   https://github.com/TU_USUARIO/smart-city-monitor
```

### Próximos Pasos

1. **Personalizar**: Editar colores, logos, textos
2. **Optimizar**: Agregar caché, comprimir imágenes
3. **Monitorear**: Configurar alertas (UptimeRobot, etc.)
4. **Escalar**: Agregar más ESP32, base de datos
5. **Compartir**: Invitar colaboradores en GitHub

---

## 📚 Recursos Adicionales

- [GitHub Docs](https://docs.github.com/)
- [Hostinger Tutoriales](https://support.hostinger.com/)
- [Git Cheat Sheet](https://education.github.com/git-cheat-sheet-education.pdf)
- [Ngrok Documentation](https://ngrok.com/docs)
- [Cloudflare Tunnel](https://developers.cloudflare.com/cloudflare-one/connections/connect-apps/)

---

**¿Necesitas ayuda?** Abre un issue en GitHub o contacta soporte de Hostinger (chat 24/7).

🌟 **¡Si te sirve este proyecto, dale una estrella en GitHub!**
