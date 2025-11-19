# 🚀 DESPLEGAR A HOSTINGER - GUÍA RÁPIDA

## 📋 INFORMACIÓN DE TU PROYECTO

- **Repositorio GitHub**: https://github.com/FABISDFD/smart-city-monitor
- **Usuario GitHub**: FABISDFD
- **Rama**: main

---

## OPCIÓN 1: YA TIENES HOSTINGER (Ir directo al paso 3)

## OPCIÓN 2: NO TIENES HOSTINGER (Empezar desde paso 1)

---

## PASO 1: CREAR CUENTA EN HOSTINGER

### 1.1 Registrarse

1. Ve a: **https://www.hostinger.com**
2. Click en **"Empezar"** o **"Hosting Web"**
3. Selecciona plan **Business** (~$3.99/mes)
   - ✅ Incluye Node.js (necesario para backend)
   - ✅ Dominio gratis primer año
   - ✅ SSL gratis
   - ✅ 100 GB almacenamiento

4. Selecciona duración (12 meses recomendado)
5. Completa registro con email
6. Realiza el pago

### 1.2 Configurar Dominio

Durante el registro, te pedirá elegir dominio:

**Opción A**: Usar dominio nuevo (gratis)
- Busca un nombre disponible
- Ejemplo: `mi-ciudad-inteligente.com`

**Opción B**: Usar dominio existente
- Conectar tu dominio actual
- Actualizar nameservers

---

## PASO 2: ACCEDER A HOSTINGER

1. Ve a: **https://panel.hostinger.com** (hPanel)
2. Inicia sesión con tu cuenta
3. Selecciona tu dominio en la lista

---

## PASO 3: CONECTAR GITHUB CON HOSTINGER

### 3.1 Autorizar GitHub

1. En hPanel → Menú lateral → **"Git"** (sección Avanzado)
2. Click en **"Crear nuevo despliegue desde Git"**
3. Seleccionar proveedor: **GitHub**
4. Click en **"Conectar con GitHub"**
5. Se abrirá ventana de GitHub
6. Click **"Authorize Hostinger"**
7. Confirmar acceso

---

## PASO 4: DESPLEGAR BACKEND (Node.js)

### 4.1 Crear Deployment del Backend

En hPanel → Git → Crear despliegue:

```
┌─────────────────────────────────────────┐
│ Proveedor:    GitHub                    │
│ Repositorio:  FABISDFD/smart-city-monitor │
│ Rama:         main                      │
│ Carpeta:      backend                   │
│ Destino:      /public_html/api          │
│ Auto-deploy:  ✅ Activado               │
└─────────────────────────────────────────┘
```

### 4.2 Configurar Node.js

1. hPanel → **"Node.js"** (menú lateral)
2. Click **"Crear aplicación"**
3. Configurar:

```
┌─────────────────────────────────────────┐
│ Versión Node:   18.x                    │
│ Modo:           Producción              │
│ Directorio:     /public_html/api        │
│ Archivo inicio: server.js               │
│ Puerto:         3000                    │
│ URL app:        /api                    │
└─────────────────────────────────────────┘
```

4. Click **"Crear"**

### 4.3 Configurar Variables de Entorno

En la misma página de Node.js → **"Variables de entorno"**:

```bash
ESP32_IP=CAMBIAR_ESTO            # Lo configuramos después con Ngrok
PORT=3000
NODE_ENV=production
ESP32_TIMEOUT=3000
ESP32_CHECK_INTERVAL=30000
```

Por ahora deja `ESP32_IP=localhost` (lo cambiaremos después).

### 4.4 Instalar Dependencias

Hostinger instalará automáticamente al hacer el deploy, pero si quieres verificar:

1. hPanel → **"SSH Access"** → Activar SSH
2. Conectar por SSH:
   ```bash
   ssh u123456@tudominio.com -p 65002
   ```
3. Verificar instalación:
   ```bash
   cd domains/tudominio.com/public_html/api
   ls -la
   # Deberías ver: server.js, package.json, node_modules/
   ```

---

## PASO 5: DESPLEGAR FRONTEND (HTML/CSS/JS)

### 5.1 Actualizar URL del Backend

**⚠️ IMPORTANTE**: Antes de desplegar el frontend, debes actualizarlo en GitHub:

1. En tu PC, edita el archivo:
   ```
   web-hosting/app.js
   ```

2. Línea 2, cambia:
   ```javascript
   const BACKEND_URL = 'https://tudominio.com/api'; // Pon tu dominio real
   ```
   Ejemplo:
   ```javascript
   const BACKEND_URL = 'https://mi-ciudad-inteligente.com/api';
   ```

3. Guarda y sube a GitHub:
   ```bash
   git add web-hosting/app.js
   git commit -m "Update: Backend URL to Hostinger domain"
   git push origin main
   ```

### 5.2 Crear Deployment del Frontend

En hPanel → Git → Crear otro despliegue:

```
┌─────────────────────────────────────────┐
│ Proveedor:    GitHub                    │
│ Repositorio:  FABISDFD/smart-city-monitor │
│ Rama:         main                      │
│ Carpeta:      web-hosting               │
│ Destino:      /public_html              │
│ Auto-deploy:  ✅ Activado               │
└─────────────────────────────────────────┘
```

**Nota**: El frontend va a la raíz (`/public_html`) para que sea accesible en `https://tudominio.com`

---

## PASO 6: CONFIGURAR SSL (HTTPS)

### 6.1 Activar SSL Gratis

1. hPanel → **"Seguridad"** → **"SSL"**
2. Seleccionar: **"Let's Encrypt SSL"**
3. Click **"Instalar SSL"**
4. Esperar 5-10 minutos (se instala automáticamente)
5. Verificar: Ir a `https://tudominio.com` (debe mostrar candado verde)

### 6.2 Forzar HTTPS

El archivo `.htaccess` ya está configurado para redirigir HTTP → HTTPS automáticamente.

---

## PASO 7: EXPONER ESP32 A INTERNET

Tu ESP32 está en tu red local. Para que Hostinger pueda comunicarse con él, necesitas exponerlo a internet.

### OPCIÓN A: Ngrok (Rápido, Temporal)

**Instalar Ngrok**:

1. Ve a: https://ngrok.com/download
2. Descarga para Windows
3. Descomprime el archivo
4. Crea cuenta en ngrok.com
5. Copia tu authtoken

**Configurar Ngrok**:

```powershell
# En PowerShell (desde carpeta donde descomprimiste ngrok)
.\ngrok config add-authtoken TU_TOKEN_DE_NGROK

# Exponer ESP32 (mantener corriendo)
.\ngrok http 192.168.1.100:80
```

**Resultado**:
```
Forwarding  https://abc123.ngrok-free.app -> http://192.168.1.100:80
```

**Configurar en Hostinger**:

1. Copia la URL: `abc123.ngrok-free.app`
2. hPanel → Node.js → Tu aplicación → Variables de entorno
3. Editar `ESP32_IP` → Poner: `abc123.ngrok-free.app`
4. Guardar → Reiniciar aplicación

⚠️ **Limitación**: Ngrok gratis se desconecta cada 2 horas, debes reiniciarlo.

### OPCIÓN B: Cloudflare Tunnel (Gratis, Permanente)

**Mejor para producción**, no se desconecta:

```powershell
# Descargar: https://github.com/cloudflare/cloudflared/releases
# Extraer cloudflared.exe

# Autenticar
.\cloudflared tunnel login

# Crear túnel
.\cloudflared tunnel create smart-city-esp32

# Configurar DNS (si tienes dominio en Cloudflare)
.\cloudflared tunnel route dns smart-city-esp32 esp32.tudominio.com

# Ejecutar (mantener corriendo)
.\cloudflared tunnel run --url http://192.168.1.100:80 smart-city-esp32
```

**Configurar en Hostinger**:
```
ESP32_IP=esp32.tudominio.com
```

---

## PASO 8: VERIFICAR TODO

### 8.1 Verificar Backend

Abre en navegador:
```
https://tudominio.com/api/status
```

Debe mostrar:
```json
{
  "backend": "online",
  "esp32": "connected" o "disconnected",
  "mode": "esp32" o "simulation"
}
```

### 8.2 Verificar Frontend

Abre en navegador:
```
https://tudominio.com
```

Debe mostrar el dashboard con:
- ✅ Gráficos animados
- ✅ Indicador de conexión (esquina superior derecha)
- ✅ Datos actualizándose cada 2 segundos

### 8.3 Verificar ESP32

Si configuraste Ngrok/Cloudflare, prueba:
```
curl https://tudominio.com/api/check-esp32
```

---

## 📊 RESUMEN DE URLs

Después de completar todo:

```
Dashboard:       https://tudominio.com
Backend API:     https://tudominio.com/api
Status:          https://tudominio.com/api/status
Datos:           https://tudominio.com/api/data
Frecuencias:     https://tudominio.com/api/frequency
Verificar ESP32: https://tudominio.com/api/check-esp32
GitHub:          https://github.com/FABISDFD/smart-city-monitor
hPanel:          https://panel.hostinger.com
```

---

## 🔄 ACTUALIZAR EL SITIO (DESPUÉS)

Cuando hagas cambios:

```powershell
# 1. Editar archivos localmente
code web-hosting/app.js

# 2. Commit y push
git add .
git commit -m "Fix: Descripción del cambio"
git push origin main

# 3. ¡Hostinger actualiza automáticamente en 1-2 minutos!
```

---

## 🐛 SOLUCIÓN DE PROBLEMAS

### Error: "502 Bad Gateway"

**Causa**: Node.js no está corriendo

**Solución**:
1. hPanel → Node.js → Tu aplicación
2. Click **"Reiniciar"**

### Error: "Cannot GET /api/status"

**Causa**: Proxy `.htaccess` no configurado

**Solución**:
1. Verificar que `.htaccess` esté en `/public_html/`
2. Contenido debe incluir:
   ```apache
   RewriteRule ^api/(.*)$ http://localhost:3000/$1 [P,L]
   ```

### Frontend muestra "Simulación Local"

**Causa**: `BACKEND_URL` incorrecto en `app.js`

**Solución**:
1. Editar `web-hosting/app.js` línea 2
2. Poner tu dominio real
3. Push a GitHub

### ESP32 "disconnected"

**Causa**: Ngrok/túnel no está corriendo o `ESP32_IP` incorrecto

**Solución**:
1. Verificar que Ngrok esté corriendo
2. Probar URL directamente en navegador
3. Actualizar `ESP32_IP` en variables de entorno

---

## 💰 COSTOS

- **Hostinger Business**: ~$3.99/mes (incluye todo)
- **Dominio**: Gratis primer año
- **SSL**: Gratis (Let's Encrypt)
- **Ngrok**: Gratis (con limitación 2 horas)
- **Cloudflare Tunnel**: Gratis (sin limitación)

**Total**: ~$4/mes 🎉

---

## 📞 SOPORTE

- **Hostinger**: Chat 24/7 en panel.hostinger.com
- **Documentación**: https://support.hostinger.com
- **GitHub Issues**: https://github.com/FABISDFD/smart-city-monitor/issues

---

## ✅ CHECKLIST FINAL

Marca cuando completes cada paso:

- [ ] Cuenta Hostinger creada (Plan Business)
- [ ] Dominio configurado
- [ ] GitHub autorizado en Hostinger
- [ ] Backend desplegado (`/public_html/api`)
- [ ] Node.js configurado (puerto 3000)
- [ ] Variables de entorno configuradas
- [ ] Frontend desplegado (`/public_html`)
- [ ] `BACKEND_URL` actualizado en `app.js`
- [ ] SSL activado (HTTPS)
- [ ] ESP32 expuesto (Ngrok/Cloudflare)
- [ ] `ESP32_IP` configurado
- [ ] Dashboard accesible: `https://tudominio.com`
- [ ] API respondiendo: `https://tudominio.com/api/status`
- [ ] Auto-deployment funcionando (test con un push)

---

**🎉 ¡LISTO! Tu Smart City Monitor está en producción 24/7 accesible desde cualquier lugar del mundo! 🌍**

---

**¿Necesitas ayuda?** Dime en qué paso estás y te guío.
