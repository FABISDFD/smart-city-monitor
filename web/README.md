# 🏙️ Smart City - Monitor Acústico Urbano

Sistema de monitoreo de contaminación acústica en tiempo real con visualización de datos, análisis espectral y predicciones.

## 📦 Contenido

- `index.html` - Página principal
- `style.css` - Estilos y diseño responsive
- `app.js` - Lógica de simulación y visualización
- `README.md` - Este archivo

## 🚀 Instalación en Hosting

### Opción 1: Hosting Gratuito (GitHub Pages)

1. Crea un repositorio en GitHub
2. Sube todos los archivos de la carpeta `web/`
3. Ve a Settings → Pages
4. Selecciona branch `main` y carpeta `/root`
5. Tu sitio estará en: `https://tuusuario.github.io/nombre-repo/`

### Opción 2: Netlify (Gratis)

1. Ve a [netlify.com](https://www.netlify.com/)
2. Arrastra la carpeta `web/` al área de drop
3. Tu sitio estará listo en segundos
4. URL: `https://nombre-aleatorio.netlify.app`

### Opción 3: Vercel (Gratis)

1. Ve a [vercel.com](https://vercel.com/)
2. Importa desde GitHub o sube archivos
3. Deploy automático
4. URL: `https://nombre-proyecto.vercel.app`

### Opción 4: Hosting Tradicional (cPanel/FTP)

1. Accede a tu panel de hosting
2. Busca el administrador de archivos o usa FTP
3. Sube los archivos a la carpeta `public_html/` o `www/`
4. Accede desde: `https://tudominio.com/`

## 🌐 Dependencias (CDN)

Este proyecto usa librerías desde CDN, **no requiere instalación local**:

- **Chart.js** v4.4.0 - Gráficos interactivos
- **Font Awesome** v6.4.0 - Iconos

## ✨ Características

- ✅ **Monitoreo en tiempo real** con datos simulados realistas
- ✅ **Gráficos interactivos** de tendencias temporales
- ✅ **Análisis espectral** en 8 bandas de frecuencia
- ✅ **Mapa de calor** por horas del día
- ✅ **Sistema de alertas** según normativas OMS/EPA
- ✅ **Predicción** de tendencias
- ✅ **Exportación** de datos CSV
- ✅ **Generación de reportes** TXT
- ✅ **100% Responsive** - funciona en móviles y tablets

## 🎯 Datos Simulados

El sistema simula datos realistas según la hora del día:

- **6:00-9:00** → 65 dB (Hora pico mañana)
- **9:00-17:00** → 58 dB (Día laboral)
- **17:00-20:00** → 70 dB (Hora pico tarde)
- **20:00-23:00** → 52 dB (Noche temprana)
- **23:00-6:00** → 40 dB (Noche/madrugada)

## 🔧 Personalización

### Cambiar intervalo de actualización:
En `app.js` línea final:
```javascript
autoInterval = setInterval(updateData, 2000); // 2000 = 2 segundos
```

### Modificar colores:
En `style.css` busca:
```css
background: linear-gradient(135deg, #1e3c72 0%, #2a5298 100%);
```

### Ajustar límites de alerta:
En `app.js` función `updateAlerts()`:
```javascript
if (db >= 85) { // Cambiar umbral crítico
if (db >= 70) { // Cambiar umbral alto
if (db >= 55) { // Cambiar umbral moderado
```

## 📱 Compatibilidad

- ✅ Chrome/Edge (últimas versiones)
- ✅ Firefox (últimas versiones)
- ✅ Safari (iOS 12+)
- ✅ Opera
- ✅ Móviles Android/iOS

## 🌟 Próximas Funcionalidades

- [ ] Conexión con ESP32 real vía API REST
- [ ] Base de datos para histórico
- [ ] Mapas interactivos con múltiples sensores
- [ ] Notificaciones push
- [ ] Dashboard administrativo
- [ ] Exportación a PDF con gráficos

## 📄 Licencia

Proyecto educativo - Uso libre

## 👨‍💻 Soporte

Para conectar con ESP32 real, consulta el archivo `smart_city_dashboard_fixed.ino`

---

**Desarrollado para Smart City Monitoring Project**
