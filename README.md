# 🏙️ Smart City Acoustic Monitor

Sistema de monitoreo acústico en tiempo real para ciudades inteligentes, con ESP32 y dashboard web interactivo.

![Dashboard](https://img.shields.io/badge/Dashboard-Live-brightgreen)
![ESP32](https://img.shields.io/badge/ESP32-D0WD--V3-blue)
![Node.js](https://img.shields.io/badge/Node.js-18.x-green)
![License](https://img.shields.io/badge/License-MIT-yellow)

## 🎯 Características

- 📊 **Monitoreo en tiempo real** - Medición continua de niveles de ruido (dB)
- 🎵 **Análisis de frecuencias** - 8 bandas espectrales (31Hz - 16kHz)
- 🌡️ **Clasificación automática** - Silencioso, Normal, Ruidoso, Muy Ruidoso
- 📈 **Visualización avanzada** - Gráficos interactivos con Chart.js
- 🗺️ **Mapa de calor** - Distribución temporal del ruido
- 📱 **Diseño responsive** - Compatible con móviles y tablets
- 🔒 **Portal WiFi cautivo** - Configuración sin hardcodeo de credenciales
- ☁️ **Modo simulación** - Funciona sin hardware ESP32

## 🚀 Demo en Vivo

- **Dashboard**: [https://tudominio.com](https://tudominio.com)
- **API Status**: [https://tudominio.com/api/status](https://tudominio.com/api/status)

## 📦 Estructura del Proyecto

```
smart_city_dashboard_fixed/
├── 📁 backend/                 # Backend Node.js + Express
│   ├── server.js              # API REST con proxy ESP32
│   ├── package.json           # Dependencias Node.js
│   ├── .env.example           # Template de configuración
│   └── .env.hostinger         # Config específica Hostinger
│
├── 📁 web-hosting/             # Frontend producción
│   ├── index.html             # Dashboard HTML5
│   ├── style.css              # Estilos CSS3 + glassmorphism
│   ├── app.js                 # JavaScript conectado a backend
│   └── .htaccess              # Config Apache + CORS
│
├── 📁 web/                     # Frontend desarrollo
│   ├── index.html             # Dashboard simulación local
│   ├── style.css              # Estilos
│   ├── app.js                 # JavaScript con simulador
│   └── README.md              # Documentación frontend
│
├── smart_city_dashboard_fixed.ino  # Firmware ESP32
├── HOSTINGER-GUIA.md          # Guía despliegue Hostinger
├── README-HOSTING.md          # Guía despliegue general
└── README.md                  # Este archivo

```

## 🛠️ Tecnologías

### Hardware
- **ESP32-D0WD-V3** (rev 3.1)
- **Sensor de sonido** analógico (GPIO 33)
- **WiFi 802.11 b/g/n**

### Backend
- **Node.js** 18.x
- **Express** 4.18.2
- **CORS** 2.8.5
- **Axios** 1.6.0

### Frontend
- **HTML5** + **CSS3**
- **JavaScript ES6+**
- **Chart.js** 4.4.0
- **Font Awesome** 6.4.0

### Hosting
- **Hostinger** (Plan Business)
- **Node.js** + **Apache**
- **SSL** gratis (Let's Encrypt)

## 🚀 Instalación Rápida

### Opción 1: Desplegar en Hostinger (Recomendado)

```bash
# 1. Clonar repositorio
git clone https://github.com/TU_USUARIO/smart-city-monitor.git
cd smart-city-monitor

# 2. Instalar dependencias localmente (opcional para desarrollo)
cd backend
npm install

# 3. Seguir guía de despliegue
# Ver HOSTINGER-GUIA.md para pasos completos
```

**Pasos en Hostinger**:
1. Conecta tu repositorio GitHub en hPanel
2. Activa Node.js (puerto 3000)
3. Configura variables de entorno
4. ¡Listo! Auto-deployment configurado

### Opción 2: Desarrollo Local

```bash
# 1. Clonar repositorio
git clone https://github.com/TU_USUARIO/smart-city-monitor.git
cd smart-city-monitor

# 2. Instalar backend
cd backend
npm install
cp .env.example .env
# Editar .env con tus configuraciones

# 3. Iniciar backend
npm start
# Backend corriendo en http://localhost:3000

# 4. Abrir frontend
cd ../web-hosting
# Editar app.js línea 2: const BACKEND_URL = 'http://localhost:3000'
# Abrir index.html en navegador
```

## ⚙️ Configuración

### Variables de Entorno (Backend)

Crea archivo `backend/.env`:

```env
# IP pública de tu ESP32 (usar Ngrok, No-IP o Cloudflare)
ESP32_IP=abc123.ngrok-free.app

# Puerto del servidor
PORT=3000

# Ambiente
NODE_ENV=production

# Timeouts
ESP32_TIMEOUT=3000
ESP32_CHECK_INTERVAL=30000
```

### Configuración Frontend

Edita `web-hosting/app.js` línea 2:

```javascript
const BACKEND_URL = 'https://tudominio.com/api'; // Tu dominio
```

### Firmware ESP32

1. Abrir `smart_city_dashboard_fixed.ino` en Arduino IDE
2. Seleccionar placa: **ESP32 Dev Module**
3. Puerto: **COM3** (o tu puerto)
4. Velocidad: **115200** baud
5. Subir código (mantener botón BOOT al inicio)

## 🌐 Exponer ESP32 a Internet

### Opción A: Ngrok (Rápido - Temporal)

```bash
# 1. Descargar e instalar Ngrok
# https://ngrok.com/download

# 2. Registrarse y configurar token
ngrok config add-authtoken TU_TOKEN

# 3. Exponer ESP32
ngrok http 192.168.1.100:80

# 4. Obtendrás URL pública
# https://abc123.ngrok-free.app

# 5. Configurar en backend/.env
ESP32_IP=abc123.ngrok-free.app
```

⚠️ **Nota**: Ngrok gratis se desconecta cada 2 horas

### Opción B: Cloudflare Tunnel (Gratis - Permanente)

```bash
# 1. Instalar cloudflared
# Windows: https://github.com/cloudflare/cloudflared/releases

# 2. Autenticarse
cloudflared tunnel login

# 3. Crear túnel
cloudflared tunnel create smart-city-esp32

# 4. Configurar DNS
cloudflared tunnel route dns smart-city-esp32 esp32.tudominio.com

# 5. Ejecutar túnel
cloudflared tunnel run --url http://192.168.1.100:80 smart-city-esp32

# 6. Configurar en backend/.env
ESP32_IP=esp32.tudominio.com
```

### Opción C: Port Forwarding + No-IP

Ver guía completa en `HOSTINGER-GUIA.md`

## 📡 API Endpoints

### GET /api/status
Estado del sistema

```json
{
  "backend": "online",
  "esp32": "connected",
  "mode": "esp32",
  "timestamp": "2025-11-19T10:30:00.000Z"
}
```

### GET /api/data
Datos acústicos actuales

```json
{
  "decibels": 65.4,
  "level": "Normal",
  "timestamp": "2025-11-19T10:30:00.000Z",
  "quality": 85
}
```

### GET /api/frequency
Análisis espectral (8 bandas)

```json
{
  "bands": [
    { "frequency": "31Hz", "amplitude": 42 },
    { "frequency": "63Hz", "amplitude": 58 },
    ...
  ],
  "timestamp": "2025-11-19T10:30:00.000Z"
}
```

### POST /api/configure
Configurar IP del ESP32 dinámicamente

```bash
curl -X POST https://tudominio.com/api/configure \
  -H "Content-Type: application/json" \
  -d '{"esp32_ip": "nueva-ip.ngrok-free.app"}'
```

## 🐛 Solución de Problemas

### Backend no se conecta al ESP32

```bash
# Verificar que ESP32 esté accesible
curl http://192.168.1.100/data

# Verificar variables de entorno
echo $ESP32_IP  # Linux/Mac
echo %ESP32_IP%  # Windows

# Ver logs del backend
tail -f ~/domains/tudominio.com/logs/error.log
```

### Frontend muestra "Simulación Local"

1. Verificar `BACKEND_URL` en `app.js`
2. Probar API directamente: `https://tudominio.com/api/status`
3. Revisar consola del navegador (F12)
4. Verificar CORS en backend

### ESP32 no responde

1. Verificar conexión WiFi (LED en placa)
2. Resetear ESP32 (botón RST)
3. Verificar en Serial Monitor: `192.168.1.100` (IP asignada)
4. Probar acceso local: `http://192.168.1.100`

## 📊 Capturas de Pantalla

### Dashboard Principal
![Dashboard](docs/dashboard.png)

### Análisis de Frecuencias
![Frequency Analysis](docs/frequency.png)

### Mapa de Calor
![Heatmap](docs/heatmap.png)

## 🤝 Contribuir

Las contribuciones son bienvenidas. Por favor:

1. Fork el proyecto
2. Crea una rama (`git checkout -b feature/AmazingFeature`)
3. Commit tus cambios (`git commit -m 'Add: AmazingFeature'`)
4. Push a la rama (`git push origin feature/AmazingFeature`)
5. Abre un Pull Request

## 📝 Licencia

Este proyecto está bajo la Licencia MIT. Ver `LICENSE` para más información.

## 👨‍💻 Autor

**Tu Nombre**
- GitHub: [@TU_USUARIO](https://github.com/TU_USUARIO)
- Email: tu@email.com

## 🙏 Agradecimientos

- [ESP32 Arduino Core](https://github.com/espressif/arduino-esp32)
- [Chart.js](https://www.chartjs.org/)
- [Font Awesome](https://fontawesome.com/)
- [Hostinger](https://www.hostinger.com/)

## 📚 Documentación Adicional

- [Guía de Despliegue en Hostinger](HOSTINGER-GUIA.md)
- [Guía General de Hosting](README-HOSTING.md)
- [Documentación del Frontend](web/README.md)

---

⭐ **Si te gusta este proyecto, dale una estrella en GitHub!**
