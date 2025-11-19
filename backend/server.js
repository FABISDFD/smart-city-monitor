const express = require('express');
const cors = require('cors');
const axios = require('axios');
require('dotenv').config();

const app = express();
const PORT = process.env.PORT || 3000;
const ESP32_IP = process.env.ESP32_IP || '192.168.1.100';
const ESP32_URL = `http://${ESP32_IP}`;

// Middleware
app.use(cors());
app.use(express.json());
app.use(express.static('public'));

// Cache para datos simulados si ESP32 no está disponible
let useSimulation = false;
let lastCheckTime = 0;
const CHECK_INTERVAL = 30000; // 30 segundos

// ===== FUNCIONES AUXILIARES =====

// Simulador de datos (fallback)
function generateRealisticData() {
  const hour = new Date().getHours();
  let baseDB = 45;
  
  if (hour >= 6 && hour < 9) baseDB = 65;
  else if (hour >= 9 && hour < 17) baseDB = 58;
  else if (hour >= 17 && hour < 20) baseDB = 70;
  else if (hour >= 20 && hour < 23) baseDB = 52;
  else baseDB = 40;
  
  const variation = (Math.random() - 0.5) * 15;
  const noise = Math.random() * 5;
  const decibels = Math.max(30, Math.min(90, baseDB + variation + noise));
  
  let peakFreq = 125;
  if (decibels > 70) peakFreq = 125 + Math.random() * 250;
  else if (decibels > 60) peakFreq = 250 + Math.random() * 500;
  else if (decibels > 50) peakFreq = 500 + Math.random() * 1000;
  else peakFreq = 1000 + Math.random() * 3000;
  
  return {
    decibels: parseFloat(decibels.toFixed(1)),
    voltage: (decibels / 100 * 3.3).toFixed(2),
    timestamp: Date.now(),
    peakFreq: Math.floor(peakFreq),
    source: 'simulation'
  };
}

function generateFrequencyBands(decibels) {
  return [
    Math.floor(Math.random() * (decibels > 70 ? 80 : 40)),
    Math.floor(Math.random() * (decibels > 60 ? 70 : 30)),
    Math.floor(Math.random() * (decibels > 50 ? 60 : 25)),
    Math.floor(Math.random() * 50),
    Math.floor(Math.random() * 40),
    Math.floor(Math.random() * 30),
    Math.floor(Math.random() * 20),
    Math.floor(Math.random() * 15)
  ];
}

// Verificar conexión con ESP32
async function checkESP32Connection() {
  const now = Date.now();
  if (now - lastCheckTime < CHECK_INTERVAL && useSimulation !== null) {
    return !useSimulation;
  }
  
  try {
    await axios.get(`${ESP32_URL}/api/status`, { timeout: 3000 });
    useSimulation = false;
    lastCheckTime = now;
    console.log('✓ ESP32 conectado');
    return true;
  } catch (error) {
    useSimulation = true;
    lastCheckTime = now;
    console.log('✗ ESP32 no disponible, usando simulación');
    return false;
  }
}

// Obtener datos del ESP32 o simulación
async function fetchData(endpoint) {
  try {
    const response = await axios.get(`${ESP32_URL}${endpoint}`, { 
      timeout: 3000 
    });
    return { data: response.data, source: 'esp32' };
  } catch (error) {
    console.warn(`Error ESP32 ${endpoint}:`, error.message);
    throw error;
  }
}

// ===== RUTAS API =====

// Página de inicio
app.get('/', (req, res) => {
  res.send(`
    <!DOCTYPE html>
    <html>
    <head>
      <title>Smart City API</title>
      <style>
        body { font-family: Arial; max-width: 800px; margin: 50px auto; padding: 20px; }
        h1 { color: #2a5298; }
        .endpoint { background: #f5f5f5; padding: 10px; margin: 10px 0; border-radius: 5px; }
        .status { padding: 10px; border-radius: 5px; margin: 20px 0; }
        .online { background: #4caf50; color: white; }
        .offline { background: #ff9800; color: white; }
      </style>
    </head>
    <body>
      <h1>🏙️ Smart City Monitor API</h1>
      <div class="status ${useSimulation ? 'offline' : 'online'}">
        Estado: ${useSimulation ? '⚠️ Simulación (ESP32 no conectado)' : '✓ ESP32 Conectado'}
      </div>
      <h2>Endpoints Disponibles:</h2>
      <div class="endpoint">
        <strong>GET /api/data</strong><br>
        Obtiene datos de ruido en tiempo real
      </div>
      <div class="endpoint">
        <strong>GET /api/frequency</strong><br>
        Análisis espectral (8 bandas de frecuencia)
      </div>
      <div class="endpoint">
        <strong>GET /api/status</strong><br>
        Estado del sistema
      </div>
      <div class="endpoint">
        <strong>GET /api/check-esp32</strong><br>
        Verificar conexión con ESP32
      </div>
      <h2>Documentación:</h2>
      <p>Consulta README.md para más información</p>
    </body>
    </html>
  `);
});

// Obtener datos de ruido
app.get('/api/data', async (req, res) => {
  try {
    await checkESP32Connection();
    
    if (!useSimulation) {
      const result = await fetchData('/data');
      return res.json({ ...result.data, source: 'esp32' });
    }
    
    // Usar simulación
    const data = generateRealisticData();
    res.json(data);
    
  } catch (error) {
    // Fallback a simulación
    const data = generateRealisticData();
    res.json(data);
  }
});

// Obtener análisis de frecuencia
app.get('/api/frequency', async (req, res) => {
  try {
    await checkESP32Connection();
    
    if (!useSimulation) {
      const result = await fetchData('/frequency');
      return res.json({ ...result.data, source: 'esp32' });
    }
    
    // Usar simulación
    const bands = generateFrequencyBands(60);
    res.json({
      bands: bands,
      labels: ['0-125Hz', '125-250Hz', '250-500Hz', '500-1kHz', '1k-2kHz', '2k-4kHz', '4k-8kHz', '8k+Hz'],
      source: 'simulation'
    });
    
  } catch (error) {
    const bands = generateFrequencyBands(60);
    res.json({
      bands: bands,
      labels: ['0-125Hz', '125-250Hz', '250-500Hz', '500-1kHz', '1k-2kHz', '2k-4kHz', '4k-8kHz', '8k+Hz'],
      source: 'simulation'
    });
  }
});

// Estado del sistema
app.get('/api/status', async (req, res) => {
  const isConnected = await checkESP32Connection();
  
  const status = {
    backend: 'online',
    esp32: isConnected ? 'connected' : 'disconnected',
    mode: useSimulation ? 'simulation' : 'esp32',
    esp32_ip: ESP32_IP,
    uptime: process.uptime(),
    timestamp: new Date().toISOString()
  };
  
  // Intentar obtener info del ESP32
  if (isConnected) {
    try {
      const result = await fetchData('/api/status');
      status.esp32_info = result.data;
    } catch (error) {
      // No pasa nada
    }
  }
  
  res.json(status);
});

// Forzar verificación de ESP32
app.get('/api/check-esp32', async (req, res) => {
  lastCheckTime = 0; // Forzar nueva verificación
  const isConnected = await checkESP32Connection();
  
  res.json({
    connected: isConnected,
    mode: useSimulation ? 'simulation' : 'esp32',
    message: isConnected ? 'ESP32 conectado correctamente' : 'ESP32 no disponible, usando simulación'
  });
});

// Configurar IP del ESP32
app.post('/api/configure', (req, res) => {
  const { esp32_ip } = req.body;
  
  if (!esp32_ip) {
    return res.status(400).json({ error: 'IP requerida' });
  }
  
  // Validar formato IP
  const ipRegex = /^(\d{1,3}\.){3}\d{1,3}$/;
  if (!ipRegex.test(esp32_ip)) {
    return res.status(400).json({ error: 'Formato de IP inválido' });
  }
  
  process.env.ESP32_IP = esp32_ip;
  ESP32_IP = esp32_ip;
  ESP32_URL = `http://${esp32_ip}`;
  lastCheckTime = 0; // Forzar verificación
  
  res.json({ 
    success: true, 
    message: 'IP configurada correctamente',
    new_ip: esp32_ip
  });
});

// Health check para hosting
app.get('/health', (req, res) => {
  res.json({ status: 'ok' });
});

// Manejo de errores
app.use((err, req, res, next) => {
  console.error(err.stack);
  res.status(500).json({ error: 'Error interno del servidor' });
});

// Iniciar servidor
app.listen(PORT, () => {
  console.log(`
╔════════════════════════════════════════╗
║  Smart City Backend Server Running     ║
╚════════════════════════════════════════╝

🌐 URL: http://localhost:${PORT}
📡 ESP32 IP: ${ESP32_IP}
📊 Modo: Verificando conexión...

Endpoints:
  GET  /api/data       - Datos de ruido
  GET  /api/frequency  - Análisis espectral
  GET  /api/status     - Estado del sistema
  POST /api/configure  - Configurar IP ESP32

  `);
  
  // Verificar conexión inicial
  checkESP32Connection();
});
