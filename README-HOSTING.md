# 🏙️ Smart City Monitor - Versión Hosting Completa

## 🌐 Sistema de 3 Capas para Hosting

Esta solución permite acceder al monitor desde **cualquier lugar del mundo**:

```
[ESP32 Local] ←→ [Backend en Hosting] ←→ [Frontend en Hosting]
```

## 📦 Estructura del Proyecto

```
smart_city_dashboard_fixed/
├── backend/               # Servidor Node.js
│   ├── server.js         # API REST
│   ├── package.json      # Dependencias
│   └── .env.example      # Configuración
├── web-hosting/          # Frontend para hosting
│   ├── index.html        # Dashboard
│   ├── style.css         # Estilos
│   └── app.js            # JavaScript
└── smart_city_dashboard_fixed.ino  # Código ESP32
```

## 🚀 Guía de Instalación Completa

### PASO 1: Preparar el ESP32

1. **Cargar el código Arduino** con las credenciales WiFi correctas
2. **Conectar a tu red WiFi** local
3. **Anotar la IP** del ESP32 (ej: `192.168.1.100`)

### PASO 2: Configurar y Desplegar Backend

#### Opción A: Heroku (Gratis con tarjeta)

```bash
# 1. Instalar Heroku CLI
# https://devcenter.heroku.com/articles/heroku-cli

# 2. Login
heroku login

# 3. Crear app
cd backend
heroku create tu-smart-city-api

# 4. Configurar variables
heroku config:set ESP32_IP=192.168.1.100

# 5. Desplegar
git init
git add .
git commit -m "Backend Smart City"
git push heroku main

# 6. Tu backend estará en:
# https://tu-smart-city-api.herokuapp.com
```

#### Opción B: Render (100% Gratis)

1. Ve a [render.com](https://render.com)
2. Crea nuevo **Web Service**
3. Conecta tu repositorio GitHub
4. Configuración:
   - **Environment**: Node
   - **Build Command**: `npm install`
   - **Start Command**: `npm start`
5. Variables de entorno:
   - `ESP32_IP` = `192.168.1.100`
6. Deploy automático

#### Opción C: Railway (Gratis)

1. Ve a [railway.app](https://railway.app)
2. New Project → Deploy from GitHub
3. Selecciona carpeta `backend/`
4. Añadir variable: `ESP32_IP=192.168.1.100`
5. Deploy

### PASO 3: Configurar y Desplegar Frontend

#### Preparación:

Edita `web-hosting/app.js` línea 2:
```javascript
const BACKEND_URL = 'https://tu-backend.herokuapp.com'; // ⚠️ TU URL AQUÍ
```

#### Opción A: Netlify (Más Fácil)

```bash
# 1. Instalar Netlify CLI (opcional)
npm install -g netlify-cli

# 2. Opción drag & drop:
# - Ve a https://app.netlify.com/drop
# - Arrastra la carpeta web-hosting/
# - ¡Listo!

# 3. O con CLI:
cd web-hosting
netlify deploy --prod

# Tu sitio estará en:
# https://nombre-aleatorio.netlify.app
```

#### Opción B: Vercel

```bash
# 1. Instalar Vercel CLI
npm install -g vercel

# 2. Deploy
cd web-hosting
vercel --prod

# Tu sitio estará en:
# https://smart-city.vercel.app
```

#### Opción C: GitHub Pages

```bash
# 1. Crear repositorio
git init
git add .
git commit -m "Smart City Frontend"
git remote add origin https://github.com/usuario/smart-city.git
git push -u origin main

# 2. En GitHub: Settings → Pages
# Source: main branch, folder: /web-hosting

# Tu sitio estará en:
# https://usuario.github.io/smart-city/
```

## 🔧 Configuración Avanzada

### Conectar ESP32 desde Internet

#### Opción 1: Port Forwarding (Router)

1. Accede a tu router (192.168.1.1)
2. Port Forwarding:
   - Puerto externo: 8080
   - Puerto interno: 80
   - IP: 192.168.1.100 (ESP32)
3. Configura backend con tu IP pública

#### Opción 2: Ngrok (Túnel temporal)

```bash
# 1. Instalar ngrok
# https://ngrok.com/download

# 2. Exponer ESP32
ngrok http 192.168.1.100:80

# 3. Obtendrás URL pública:
# https://abc123.ngrok.io

# 4. Configurar en backend:
heroku config:set ESP32_IP=abc123.ngrok.io
```

#### Opción 3: No-IP / DuckDNS (DNS Dinámico)

1. Registrarse en [noip.com](https://www.noip.com/)
2. Crear hostname: `mi-esp32.ddns.net`
3. Instalar cliente No-IP en PC con ESP32
4. Configurar Port Forwarding
5. Usar hostname en backend

## 📡 Endpoints API

### Backend → ESP32

| Endpoint | Descripción |
|----------|-------------|
| `GET /api/data` | Datos de ruido actual |
| `GET /api/frequency` | Análisis espectral |
| `GET /api/status` | Estado del sistema |
| `POST /api/configure` | Cambiar IP del ESP32 |

### Ejemplo de uso:

```javascript
// Obtener datos
fetch('https://tu-backend.herokuapp.com/api/data')
  .then(res => res.json())
  .then(data => console.log(data));

// Configurar nueva IP
fetch('https://tu-backend.herokuapp.com/api/configure', {
  method: 'POST',
  headers: { 'Content-Type': 'application/json' },
  body: JSON.stringify({ esp32_ip: '192.168.1.200' })
});
```

## ⚙️ Variables de Entorno

### Backend (.env)

```bash
ESP32_IP=192.168.1.100    # IP de tu ESP32
PORT=3000                  # Puerto del servidor
NODE_ENV=production        # Entorno
```

### Frontend (app.js)

```javascript
const BACKEND_URL = 'https://tu-backend.herokuapp.com';
const USE_BACKEND = true;  // true = backend, false = simulación
```

## 🔄 Flujo de Datos

```
1. ESP32 mide sonido → Genera datos
2. Backend consulta ESP32 cada 2 segundos
3. Backend almacena datos (cache)
4. Frontend consulta backend cada 2 segundos
5. Frontend muestra datos en dashboard
```

## 🛡️ Seguridad

### Recomendaciones:

1. **Autenticación**: Añade API keys
2. **HTTPS**: Usa solo conexiones seguras
3. **Rate Limiting**: Limita requests por IP
4. **Firewall**: Configura reglas en ESP32
5. **VPN**: Conecta ESP32 a VPN

### Añadir API Key (Ejemplo):

**Backend (server.js):**
```javascript
const API_KEY = process.env.API_KEY || 'tu-clave-secreta';

app.use('/api', (req, res, next) => {
  const key = req.headers['x-api-key'];
  if (key !== API_KEY) {
    return res.status(401).json({ error: 'No autorizado' });
  }
  next();
});
```

**Frontend (app.js):**
```javascript
async function fetchFromBackend(endpoint) {
  const response = await fetch(`${BACKEND_URL}${endpoint}`, {
    headers: {
      'x-api-key': 'tu-clave-secreta'
    }
  });
  return await response.json();
}
```

## 🐛 Solución de Problemas

### Backend no conecta con ESP32

**Problema**: `ESP32 no disponible, usando simulación`

**Soluciones**:
1. Verifica que ESP32 esté encendido
2. Confirma IP correcta en variables de entorno
3. Si usas Port Forwarding, verifica que esté activo
4. Prueba acceder desde navegador: `http://IP-ESP32/data`

### Frontend no conecta con Backend

**Problema**: `Backend no disponible - Simulación Local`

**Soluciones**:
1. Verifica URL del backend en `app.js`
2. Confirma que backend esté desplegado
3. Revisa CORS en backend (ya configurado)
4. Abre consola del navegador (F12) para ver errores

### CORS Errors

Si ves errores de CORS:

**Backend ya tiene CORS habilitado**, pero verifica:
```javascript
// En server.js - ya incluido
app.use(cors());
```

## 📊 Modo de Operación

El sistema tiene 3 modos:

1. **ESP32 Real**: Backend conectado a ESP32 físico
2. **Simulación Backend**: Backend genera datos simulados
3. **Simulación Local**: Frontend genera datos sin backend

El sistema cambia automáticamente entre modos según disponibilidad.

## 💰 Costos de Hosting

| Servicio | Costo | Límites |
|----------|-------|---------|
| **Netlify** | Gratis | 100 GB/mes, 300 min build |
| **Vercel** | Gratis | 100 GB/mes, sin build |
| **Heroku** | Gratis con tarjeta | 550 hrs/mes (duerme después 30min inactivo) |
| **Render** | Gratis | Duerme después 15min |
| **Railway** | $5 crédito | Luego $5/mes |

## 🎯 Próximos Pasos

1. ✅ Desplegar backend
2. ✅ Configurar IP del ESP32
3. ✅ Desplegar frontend
4. ✅ Configurar URL del backend
5. ✅ Probar conexión
6. 🔄 Opcional: Configurar dominio personalizado
7. 🔒 Opcional: Añadir autenticación

## 📞 Soporte

Para problemas comunes:
1. Revisa logs del backend (Heroku/Render dashboard)
2. Revisa consola del navegador (F12)
3. Verifica que ESP32 esté accesible
4. Confirma variables de entorno

---

**¿Listo para desplegar? Sigue la guía paso a paso y tendrás tu Smart City Monitor accesible desde cualquier lugar! 🌍**
