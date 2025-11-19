# 🏙️ Smart City - Monitor Acústico (Versión ESP32)

## 🔗 Conexión con ESP32 Real

Esta versión se conecta a tu ESP32 físico para obtener datos reales del sensor de sonido.

## 📋 Requisitos Previos

### Hardware:
- ✅ ESP32 con sensor de sonido en GPIO 33
- ✅ Código Arduino cargado (`smart_city_dashboard_fixed.ino`)
- ✅ ESP32 conectado a tu WiFi

### Software:
- ✅ Navegador moderno (Chrome, Firefox, Edge)
- ✅ ESP32 y PC en la misma red WiFi

## 🚀 Configuración Paso a Paso

### 1️⃣ Preparar el ESP32

1. **Cargar el código Arduino** en tu ESP32
2. **Abrir Monitor Serie** (115200 baud)
3. **Anotar la IP** que muestra el ESP32:
   ```
   ✓ WiFi conectado!
   Dirección IP: 192.168.1.100  ← ESTA IP
   ```

### 2️⃣ Configurar la Página Web

**Opción A: Editar manualmente**
1. Abre `app-esp32.js` 
2. En la línea 2, cambia:
   ```javascript
   const ESP32_IP = "192.168.1.100"; // ⚠️ Pon la IP de tu ESP32
   ```

**Opción B: Desde el navegador**
1. Abre `index-esp32.html`
2. Click en botón "⚙️ Configurar ESP32"
3. Ingresa la IP del ESP32

**Opción C: Usar URL**
```
index-esp32.html?esp32=192.168.1.100
```

### 3️⃣ Probar Localmente

```bash
# Opción 1: Python
python -m http.server 8000

# Opción 2: PHP
php -S localhost:8000

# Opción 3: Node.js (si tienes http-server)
npx http-server -p 8000
```

Luego abre: `http://localhost:8000/index-esp32.html`

### 4️⃣ Subir a Hosting

#### GitHub Pages (Gratis):
```bash
# 1. Crear repo
git init
git add .
git commit -m "Smart City Monitor"
git remote add origin https://github.com/usuario/repo.git
git push -u origin main

# 2. Activar Pages en Settings → Pages
```

#### Netlify (Más fácil):
1. Arrastra la carpeta `web/` a netlify.com
2. Configura la IP del ESP32
3. ¡Listo! URL: `https://tu-app.netlify.app`

## 🔧 Solución de Problemas

### ❌ "ESP32 Desconectado - Modo Simulación"

**Causas comunes:**

1. **IP Incorrecta**
   - Verifica la IP en el Monitor Serie del ESP32
   - Asegúrate que no cambió (usar IP estática ayuda)

2. **ESP32 y PC en redes diferentes**
   - Ambos deben estar en la misma WiFi
   - No funcionará con datos móviles

3. **CORS Bloqueado**
   - Si usas `file://`, el navegador bloquea CORS
   - Solución: Usar servidor local (python, etc.)

4. **Firewall**
   - El firewall puede bloquear puerto 80
   - Solución: Permitir tráfico HTTP en red local

### 🔄 Cambiar de Simulación a ESP32

En `app-esp32.js` línea 4:
```javascript
const USE_ESP32 = true;  // true = ESP32, false = simulación
```

### 🌐 Acceso desde Internet

**⚠️ Por seguridad, el ESP32 solo es accesible en tu red local.**

Para acceso externo necesitas:
1. **Port Forwarding** en tu router (puerto 80 → ESP32)
2. **IP Pública** o servicio DNS dinámico (No-IP, DuckDNS)
3. **⚠️ RIESGO**: Tu ESP32 quedará expuesto a internet

**Alternativa recomendada**: Usar backend intermedio
- ESP32 → Backend (Node.js/PHP en hosting) → Frontend
- El backend hace de puente seguro

## 📊 Endpoints API del ESP32

| Endpoint | Método | Descripción |
|----------|--------|-------------|
| `/data` | GET | Datos actuales (dB, voltaje, frecuencia) |
| `/frequency` | GET | Bandas espectrales (8 bandas) |
| `/api/status` | GET | Estado del ESP32 (IP, SSID, uptime) |
| `/` | GET | Dashboard embebido (opcional) |

### Ejemplo de respuesta `/data`:
```json
{
  "decibels": 62.5,
  "voltage": 2.1,
  "timestamp": 123456,
  "peakFreq": 425
}
```

### Ejemplo de respuesta `/frequency`:
```json
{
  "bands": [45, 62, 38, 25, 18, 12, 8, 5],
  "labels": ["0-125Hz", "125-250Hz", "250-500Hz", ...]
}
```

## ✨ Características

✅ **Conexión automática** con ESP32
✅ **Fallback a simulación** si no hay conexión
✅ **Reconexión automática** cada 30 segundos
✅ **Indicador visual** de estado de conexión
✅ **Datos en tiempo real** desde sensor físico
✅ **Compatible con hosting** externo

## 🎯 Diferencias con Versión Simulada

| Característica | Simulación | ESP32 Real |
|----------------|-----------|------------|
| Fuente datos | Algoritmo | Sensor físico |
| Necesita red | No | Sí (misma WiFi) |
| Hosting | Cualquiera | Requiere config |
| Precisión | Aproximada | Real |
| Latencia | 0ms | 50-200ms |

## 📝 Notas Importantes

1. **Mismo WiFi**: ESP32 y dispositivo que accede deben estar en la misma red
2. **HTTPS**: Si tu hosting usa HTTPS, puede haber problemas con ESP32 (HTTP)
3. **CORS**: Ya configurado en el código del ESP32
4. **IP Dinámica**: La IP del ESP32 puede cambiar al reiniciar
5. **Hosting Gratuito**: GitHub Pages y Netlify funcionan perfectamente

## 🔐 Seguridad

- ⚠️ No expongas el ESP32 a internet sin protección
- ✅ Usa contraseña fuerte en el WiFi
- ✅ Mantén el firmware actualizado
- ⚠️ CORS está abierto (`*`) - OK para red local

## 📞 Soporte

Si tienes problemas:
1. Verifica IP del ESP32 en Monitor Serie
2. Asegúrate que ambos están en misma red
3. Prueba primero localmente antes de hosting
4. Revisa consola del navegador (F12) para errores

---

**Desarrollado para Smart City Monitoring Project**
