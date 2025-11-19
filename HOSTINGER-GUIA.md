# 🏙️ Smart City Monitor - Despliegue en Hostinger

## 📋 Requisitos Previos

- **Cuenta Hostinger** (Plan Premium o Business con Node.js)
- **Dominio** configurado en Hostinger
- **Acceso SSH** habilitado (en panel de Hostinger)
- **ESP32** con código cargado y conectado a WiFi

## 🎯 Arquitectura en Hostinger

```
[ESP32 Local] ←→ [Backend Node.js Hostinger] ←→ [Frontend Hostinger]
                   tu-dominio.com:3000           tu-dominio.com
```

## 🚀 GUÍA DE DESPLIEGUE PASO A PASO

### PARTE 1: Preparar Archivos para Hostinger

Ya tienes todo listo en:
- `backend/` - Backend Node.js
- `web-hosting/` - Frontend HTML/CSS/JS

### PARTE 2: Desplegar Backend en Hostinger

#### 1. Activar Node.js en Hostinger

1. **Ingresa a hPanel** (panel.hostinger.com)
2. Ve a **"Avanzado"** → **"Node.js"**
3. Click en **"Crear Aplicación Node.js"**
4. Configuración:
   - **Versión Node.js**: 18.x o superior
   - **Modo aplicación**: Producción
   - **Directorio raíz**: `/domains/tudominio.com/public_html/api`
   - **URL aplicación**: `https://tudominio.com/api`
   - **Archivo de inicio**: `server.js`
   - **Puerto**: 3000

5. Click en **"Crear"**

#### 2. Subir Backend vía FTP

**Opción A: File Manager (hPanel)**

1. En hPanel → **"Archivos"** → **"Administrador de archivos"**
2. Navega a `/domains/tudominio.com/public_html/`
3. Crea carpeta `api/`
4. Entra a `api/` y sube:
   - `server.js`
   - `package.json`
5. Crea archivo `.env` con:
   ```
   ESP32_IP=TU_IP_PUBLICA:PUERTO
   PORT=3000
   NODE_ENV=production
   ```

**Opción B: FileZilla (FTP)**

1. Descarga **FileZilla** (filezilla-project.org)
2. Conecta con datos de Hostinger:
   - **Host**: ftp.tudominio.com
   - **Usuario**: Tu usuario FTP
   - **Contraseña**: Tu contraseña FTP
   - **Puerto**: 21

3. Navega a `/public_html/api/`
4. Sube `server.js`, `package.json`, `.env`

#### 3. Instalar Dependencias vía SSH

1. **Habilitar SSH en Hostinger**:
   - hPanel → **"Avanzado"** → **"SSH Access"**
   - Activar SSH

2. **Conectar por SSH**:
   ```bash
   ssh u123456789@tudominio.com -p 65002
   # Contraseña: tu contraseña de hosting
   ```

3. **Instalar paquetes**:
   ```bash
   cd domains/tudominio.com/public_html/api
   npm install
   ```

4. **Iniciar aplicación**:
   ```bash
   # Hostinger lo inicia automáticamente
   # O manualmente:
   node server.js
   ```

5. **Verificar**:
   ```bash
   # Debe estar corriendo en puerto 3000
   curl http://localhost:3000/api/status
   ```

#### 4. Configurar Variables de Entorno

En hPanel → **Node.js** → Tu aplicación → **Variables de entorno**:

```
ESP32_IP=TU_IP_PUBLICA_ESP32
PORT=3000
NODE_ENV=production
```

### PARTE 3: Desplegar Frontend en Hostinger

#### 1. Preparar Frontend

Edita `web-hosting/app.js` línea 2:
```javascript
const BACKEND_URL = 'https://tudominio.com/api'; // Tu dominio
```

#### 2. Subir Frontend

**Via File Manager:**

1. hPanel → **"Archivos"** → **"Administrador de archivos"**
2. Navega a `/domains/tudominio.com/public_html/`
3. Sube:
   - `index.html`
   - `style.css`
   - `app.js`

**Via FTP:**

```
/public_html/
├── index.html
├── style.css
├── app.js
└── api/
    ├── server.js
    ├── package.json
    └── .env
```

#### 3. Configurar .htaccess (Opcional)

Crea `/public_html/.htaccess`:

```apache
# Redirigir API requests
RewriteEngine On
RewriteRule ^api/(.*)$ http://localhost:3000/$1 [P,L]

# Habilitar CORS
Header set Access-Control-Allow-Origin "*"
Header set Access-Control-Allow-Methods "GET, POST, OPTIONS"
Header set Access-Control-Allow-Headers "Content-Type"

# Cache estático
<FilesMatch "\.(css|js|jpg|png|gif|ico)$">
Header set Cache-Control "max-age=2592000, public"
</FilesMatch>
```

### PARTE 4: Exponer ESP32 a Internet

Tu ESP32 está en red local, necesitas hacerlo accesible desde internet:

#### Opción A: Ngrok (Recomendado - Fácil)

```bash
# 1. Instalar en PC donde está el ESP32
# Descarga: https://ngrok.com/download

# 2. Registrarse y obtener token
ngrok config add-authtoken TU_TOKEN

# 3. Exponer ESP32
ngrok http 192.168.1.100:80

# 4. Obtendrás URL pública:
# https://abc123.ngrok-free.app

# 5. Configurar en Hostinger (.env):
ESP32_IP=abc123.ngrok-free.app
```

**⚠️ Ngrok gratis se desconecta cada 2 horas, debes reiniciarlo**

#### Opción B: Port Forwarding + No-IP

1. **Configurar Port Forwarding en tu router**:
   - Puerto externo: 8080
   - Puerto interno: 80
   - IP interna: 192.168.1.100 (ESP32)

2. **Obtener dominio gratuito**:
   - Registrarse en [noip.com](https://www.noip.com)
   - Crear hostname: `mi-esp32.ddns.net`
   - Instalar cliente No-IP en PC

3. **Configurar en Hostinger**:
   ```
   ESP32_IP=mi-esp32.ddns.net:8080
   ```

#### Opción C: Cloudflare Tunnel (Gratis y permanente)

```bash
# 1. Instalar cloudflared
# Windows: https://github.com/cloudflare/cloudflared/releases

# 2. Login
cloudflared tunnel login

# 3. Crear túnel
cloudflared tunnel create smart-city-esp32

# 4. Configurar
cloudflared tunnel route dns smart-city-esp32 esp32.tudominio.com

# 5. Ejecutar
cloudflared tunnel run --url http://192.168.1.100:80 smart-city-esp32
```

## ✅ Verificación Final

### 1. Verificar Backend

```bash
# Desde tu navegador:
https://tudominio.com/api/status

# Debe responder:
{
  "backend": "online",
  "esp32": "connected" o "disconnected",
  "mode": "esp32" o "simulation"
}
```

### 2. Verificar Frontend

```bash
# Abrir en navegador:
https://tudominio.com

# Debe mostrar dashboard y estado de conexión
```

### 3. Verificar Conexión ESP32

```bash
# Test directo:
curl https://TU_URL_NGROK/data

# Debe devolver JSON con datos
```

## 🔧 Configuración de Hostinger Node.js

### Reiniciar Aplicación Node.js

```bash
# Via SSH:
cd ~/domains/tudominio.com/public_html/api
npm restart

# O desde hPanel:
Node.js → Tu aplicación → Reiniciar
```

### Ver Logs

```bash
# Via SSH:
tail -f ~/domains/tudominio.com/logs/error.log
tail -f ~/domains/tudominio.com/logs/access.log
```

### Actualizar Código

```bash
# Via SSH:
cd ~/domains/tudominio.com/public_html/api
git pull  # Si usas Git
# O sube archivos via FTP

npm install  # Si actualizaste package.json
pm2 restart server  # Reiniciar
```

## 📱 Configuración SSL (HTTPS)

Hostinger incluye **SSL gratis con Let's Encrypt**:

1. hPanel → **"Seguridad"** → **"SSL"**
2. Activar **"Let's Encrypt SSL"**
3. Esperar 5-10 minutos
4. Actualizar URLs en código:
   ```javascript
   const BACKEND_URL = 'https://tudominio.com/api';
   ```

## 🐛 Solución de Problemas

### Error: "Cannot GET /api/status"

**Solución**:
```bash
# Verificar que Node.js esté corriendo:
ps aux | grep node

# Reiniciar:
cd ~/domains/tudominio.com/public_html/api
node server.js &
```

### Error: "ESP32 no disponible"

**Solución**:
1. Verifica que Ngrok/túnel esté activo
2. Prueba la URL directamente en navegador
3. Revisa variable `ESP32_IP` en .env

### Error: "502 Bad Gateway"

**Solución**:
```bash
# Node.js no está corriendo
cd ~/domains/tudominio.com/public_html/api
npm install
node server.js &
```

### Frontend muestra "Simulación Local"

**Solución**:
1. Verifica `BACKEND_URL` en app.js
2. Confirma que backend responda: `https://tudominio.com/api/status`
3. Revisa consola del navegador (F12)

## 💰 Costos Hostinger

| Plan | Precio | Node.js | Dominio |
|------|--------|---------|---------|
| **Premium** | ~$2.99/mes | ❌ No | ✅ Sí |
| **Business** | ~$3.99/mes | ✅ Sí | ✅ Sí |
| **Cloud** | ~$9.99/mes | ✅ Sí | ✅ Sí |

**Recomendación**: Plan Business (incluye Node.js)

## 🎯 Resumen de URLs

```
Frontend:    https://tudominio.com
Backend API: https://tudominio.com/api
Estado:      https://tudominio.com/api/status
Datos:       https://tudominio.com/api/data
ESP32:       https://tu-url-ngrok.app (temporal)
```

## 📞 Soporte Hostinger

- **Chat**: 24/7 en panel.hostinger.com
- **Base conocimiento**: support.hostinger.com
- **Email**: support@hostinger.com

---

**¡Tu Smart City Monitor estará en línea 24/7 accesible desde cualquier lugar! 🌍**
