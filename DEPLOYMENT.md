# 🚀 Deployment Automático para Hostinger

Este repositorio está configurado para deployment directo en Hostinger.

## 📁 Estructura para Hostinger

Los archivos están en la raíz para facilitar el deployment automático:

```
/
├── index.html          # Frontend - Dashboard
├── style.css           # Estilos
├── app.js              # JavaScript frontend
├── .htaccess           # Configuración Apache
├── server.js           # Backend Node.js
├── package.json        # Dependencias Node.js
└── .env.example        # Variables de entorno (crear .env)
```

## ⚙️ Configuración en Hostinger

### 1. Conectar GitHub

1. **hPanel** → **Git** → Crear deployment
2. **Repositorio**: `FABISDFD/smart-city-monitor`
3. **Rama**: `main`
4. **Destino**: `/domains/tudominio.com/public_html`
5. **Auto-deploy**: ✅ Activado

### 2. Configurar Node.js

1. **hPanel** → **Node.js** → Crear aplicación
2. Configuración:
   ```
   Versión: 18.x
   Directorio: /public_html
   Archivo: server.js
   Puerto: 3000
   URL: /api
   ```

### 3. Variables de Entorno

En Node.js → Variables de entorno:

```bash
ESP32_IP=tu-esp32-url.ngrok-free.app
PORT=3000
NODE_ENV=production
```

### 4. Actualizar Backend URL

Edita `app.js` línea 2:
```javascript
const BACKEND_URL = 'https://tudominio.com/api';
```

Luego haz commit y push:
```bash
git add app.js
git commit -m "Update: Backend URL"
git push origin main
```

## 🔄 Auto-Deployment

Cada vez que hagas `git push origin main`, Hostinger actualizará automáticamente:
- ✅ Frontend (archivos HTML/CSS/JS)
- ✅ Backend (Node.js)
- ✅ Configuración (.htaccess)

## 📝 Después del Deployment

1. **Instalar dependencias** (automático, pero puedes verificar):
   ```bash
   ssh usuario@tudominio.com -p 65002
   cd domains/tudominio.com/public_html
   npm install
   ```

2. **Verificar estado**:
   - Dashboard: `https://tudominio.com`
   - API: `https://tudominio.com/api/status`

## 🔗 Enlaces Útiles

- **GitHub**: https://github.com/FABISDFD/smart-city-monitor
- **Guía completa**: Ver `DESPLEGAR-HOSTINGER.md`
- **Documentación**: Ver `README.md`

---

**✅ Listo para deployment directo en Hostinger**
