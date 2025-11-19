#include <WiFi.h>
#include <WebServer.h>
#include <math.h>

// WiFi
char ssid[] = "iPhone";
char pass[] = "Moto241412..";

#define SOUND_SENSOR_PIN 33  // Pin ADC

WebServer server(80);

// Configuración del muestreo
const int sampleWindow = 30;   
const float Vref = 0.01;       
float lastdB = 0;              
float currentdB = 0;           

// Análisis de frecuencia
#define FFT_SAMPLES 64
int frequencyBands[8] = {0}; // 8 bandas de frecuencia
int peakFrequency = 0;

// Historial para predicción
#define HISTORY_SIZE 60
float dbHistory[HISTORY_SIZE] = {0};
int historyIndex = 0;

unsigned long lastMeasurement = 0;
const unsigned long measurementInterval = 500; // 500ms entre mediciones

void setup() {
  Serial.begin(115200);
  pinMode(SOUND_SENSOR_PIN, INPUT);

  // Conectar WiFi
  WiFi.begin(ssid, pass);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Conectando a WiFi...");
  }
  
  Serial.println("WiFi conectado!");
  Serial.print("Dirección IP: ");
  Serial.println(WiFi.localIP());

  // Configurar rutas del servidor web
  server.on("/", handleRoot);
  server.on("/data", handleData);
  server.on("/history", handleHistory);
  server.on("/frequency", handleFrequency);
  
  // Iniciar servidor web
  server.begin();
  Serial.println("Servidor web iniciado");
  Serial.println("Accede a tu dashboard en: http://" + WiFi.localIP().toString());
}

void loop() {
  server.handleClient();
  
  // Tomar medición cada 500ms
  if (millis() - lastMeasurement >= measurementInterval) {
    measureSound();
    lastMeasurement = millis();
  }
}

void measureSound() {
  unsigned long startMillis = millis();
  int signalMax = 0;
  int signalMin = 4095;
  int samples[FFT_SAMPLES] = {0};
  int sampleIndex = 0;

  // Captura de muestras con FFT simple
  while (millis() - startMillis < sampleWindow) {
    int sample = analogRead(SOUND_SENSOR_PIN);
    if (sample < 4095) {
      if (sample > signalMax) signalMax = sample;
      if (sample < signalMin) signalMin = sample;
      
      // Guardar muestra para FFT
      if (sampleIndex < FFT_SAMPLES) {
        samples[sampleIndex++] = sample;
      }
    }
  }

  int peakToPeak = signalMax - signalMin;
  float voltage = (peakToPeak / 4095.0) * 3.3;

  // Calcular dB
  float dB = (20.0 * log10(voltage / Vref)) * 2.2;
  if (dB < 0) dB = 0;

  // Filtro suavizado
  currentdB = (lastdB * 0.7) + (dB * 0.3);
  lastdB = currentdB;

  // Guardar en historial
  dbHistory[historyIndex] = currentdB;
  historyIndex = (historyIndex + 1) % HISTORY_SIZE;

  // FFT simple - análisis de bandas de frecuencia
  analyzeFrequencies(samples, sampleIndex);

  Serial.print("Dirección IP: ");
  Serial.println(WiFi.localIP());
  Serial.println("Servidor web iniciado");
  Serial.println("Accede a tu dashboard en: http://" + WiFi.localIP().toString());


  Serial.print("Sonido: ");
  Serial.print(currentdB, 1);
  Serial.print(" dB | Freq dominante: ");
  Serial.print(peakFrequency);
  Serial.println(" Hz");
}

void analyzeFrequencies(int* samples, int count) {
  // Reset bandas
  for (int i = 0; i < 8; i++) {
    frequencyBands[i] = 0;
  }
  
  if (count < 8) return;
  
  // FFT simplificado - dividir en 8 bandas
  // Banda 0: 0-125 Hz (bajo/tráfico)
  // Banda 1: 125-250 Hz (motor/vehículos)
  // Banda 2: 250-500 Hz (voces/conversación)
  // Banda 3: 500-1k Hz (música/alarmas)
  // Banda 4: 1k-2k Hz (alto/industrial)
  // Banda 5: 2k-4k Hz (muy alto/sirenas)
  // Banda 6: 4k-8k Hz (agudo/pájaros)
  // Banda 7: 8k+ Hz (muy agudo)
  
  int bandsPerSection = count / 8;
  int maxBand = 0;
  int maxValue = 0;
  
  for (int band = 0; band < 8; band++) {
    int sum = 0;
    for (int i = 0; i < bandsPerSection; i++) {
      int idx = band * bandsPerSection + i;
      if (idx < count) {
        sum += abs(samples[idx] - 2048); // Centrar en 0
      }
    }
    frequencyBands[band] = sum / bandsPerSection;
    
    if (frequencyBands[band] > maxValue) {
      maxValue = frequencyBands[band];
      maxBand = band;
    }
  }
  
  // Estimar frecuencia dominante
  peakFrequency = (maxBand * 1000) + 125;
}

void handleRoot() {
  String html = "<!DOCTYPE html><html lang='es'><head>";
  html += "<meta charset='UTF-8'>";
  html += "<meta name='viewport' content='width=device-width, initial-scale=1.0'>";
  html += "<title>Smart City - Monitor Acústico Urbano</title>";
  html += "<script src='https://cdn.jsdelivr.net/npm/chart.js'></script>";
  html += "<link rel='stylesheet' href='https://cdnjs.cloudflare.com/ajax/libs/font-awesome/6.0.0/css/all.min.css'>";
  
  // CSS
  html += "<style>";
  html += "* { margin: 0; padding: 0; box-sizing: border-box; }";
  html += "body { font-family: 'Segoe UI', Tahoma, Geneva, Verdana, sans-serif; background: linear-gradient(135deg, #1e3c72 0%, #2a5298 100%); color: white; min-height: 100vh; }";
  html += ".header { background: rgba(0,0,0,0.3); padding: 1rem 2rem; display: flex; justify-content: space-between; align-items: center; backdrop-filter: blur(10px); }";
  html += ".logo { display: flex; align-items: center; gap: 10px; font-size: 1.5rem; font-weight: bold; }";
  html += ".status-indicator { display: flex; align-items: center; gap: 10px; padding: 8px 15px; border-radius: 20px; background: rgba(76, 175, 80, 0.2); border: 1px solid #4caf50; }";
  html += ".container { padding: 2rem; max-width: 1400px; margin: 0 auto; }";
  html += ".dashboard-grid { display: grid; grid-template-columns: repeat(auto-fit, minmax(300px, 1fr)); gap: 2rem; margin-bottom: 2rem; }";
  html += ".card { background: rgba(255, 255, 255, 0.1); border-radius: 15px; padding: 1.5rem; backdrop-filter: blur(15px); border: 1px solid rgba(255, 255, 255, 0.2); transition: all 0.3s ease; }";
  html += ".card:hover { transform: translateY(-5px); box-shadow: 0 10px 30px rgba(0,0,0,0.3); }";
  html += ".metric-card { text-align: center; }";
  html += ".metric-value { font-size: 3rem; font-weight: bold; margin: 1rem 0; text-shadow: 2px 2px 4px rgba(0,0,0,0.5); }";
  html += ".metric-label { font-size: 1rem; opacity: 0.9; margin-bottom: 0.5rem; }";
  html += ".metric-sublabel { font-size: 0.8rem; opacity: 0.7; }";
  html += ".alert-card { border-left: 4px solid #ff5722; }";
  html += ".alert-green { border-left-color: #4caf50; background: rgba(76, 175, 80, 0.1); }";
  html += ".alert-yellow { border-left-color: #ff9800; background: rgba(255, 152, 0, 0.1); }";
  html += ".alert-red { border-left-color: #f44336; background: rgba(244, 67, 54, 0.1); }";
  html += ".alert-critical { border-left-color: #9c27b0; background: rgba(156, 39, 176, 0.1); }";
  html += ".chart-container { height: 300px; margin: 1rem 0; }";
  html += ".controls { display: flex; flex-wrap: wrap; gap: 1rem; margin: 2rem 0; justify-content: center; }";
  html += ".btn { background: rgba(255, 255, 255, 0.2); border: 1px solid rgba(255, 255, 255, 0.3); color: white; padding: 12px 24px; border-radius: 25px; cursor: pointer; font-size: 16px; transition: all 0.3s ease; display: flex; align-items: center; gap: 8px; }";
  html += ".btn:hover { background: rgba(255, 255, 255, 0.3); transform: scale(1.05); }";
  html += ".btn.active { background: rgba(76, 175, 80, 0.3); border-color: #4caf50; }";
  html += ".statistics-grid { display: grid; grid-template-columns: repeat(auto-fit, minmax(200px, 1fr)); gap: 1rem; margin-top: 2rem; }";
  html += ".stat-item { text-align: center; padding: 1rem; }";
  html += ".stat-value { font-size: 1.8rem; font-weight: bold; color: #64b5f6; }";
  html += ".regulations { margin-top: 1rem; padding: 1rem; background: rgba(0,0,0,0.2); border-radius: 10px; }";
  html += ".regulation-item { display: flex; justify-content: space-between; padding: 0.5rem 0; border-bottom: 1px solid rgba(255,255,255,0.1); }";
  html += ".timestamp { font-size: 0.9rem; opacity: 0.8; margin-top: 1rem; }";
  html += ".prediction-card { background: linear-gradient(135deg, rgba(156, 39, 176, 0.2), rgba(103, 58, 183, 0.2)); }";
  html += ".frequency-display { font-size: 2rem; color: #ba68c8; margin: 0.5rem 0; }";
  html += ".notification { position: fixed; top: 80px; right: 20px; padding: 1rem 2rem; border-radius: 10px; background: rgba(244, 67, 54, 0.95); box-shadow: 0 5px 15px rgba(0,0,0,0.3); display: none; animation: slideIn 0.3s ease; z-index: 1000; }";
  html += "@keyframes slideIn { from { transform: translateX(400px); opacity: 0; } to { transform: translateX(0); opacity: 1; } }";
  html += ".heatmap { display: grid; grid-template-columns: repeat(24, 1fr); gap: 2px; margin: 1rem 0; }";
  html += ".heatmap-cell { height: 30px; border-radius: 3px; transition: all 0.3s; cursor: pointer; }";
  html += ".heatmap-cell:hover { transform: scale(1.2); z-index: 10; }";
  html += ".trend-up { color: #f44336; }";
  html += ".trend-down { color: #4caf50; }";
  html += ".trend-stable { color: #ff9800; }";
  html += "@media (max-width: 768px) { .dashboard-grid { grid-template-columns: 1fr; } .controls { flex-direction: column; } }";
  html += "</style></head>";
  
  // BODY
  html += "<body>";
  html += "<div class='header'>";
  html += "<div class='logo'><i class='fas fa-city'></i><span>Smart City - Monitor Acústico</span></div>";
  html += "<div class='status-indicator' id='connectionStatus'><i class='fas fa-wifi'></i><span>Conectado</span></div>";
  html += "</div>";
  
  html += "<div class='container'>";
  html += "<div class='dashboard-grid'>";
  
  // Tarjeta principal
  html += "<div class='card metric-card' id='mainCard'>";
  html += "<div class='metric-label'>Nivel de Ruido Actual</div>";
  html += "<div class='metric-value' id='currentDB'>-- dB</div>";
  html += "<div class='metric-sublabel' id='noiseCategory'>Calculando...</div>";
  html += "<div class='timestamp' id='lastUpdate'>Última actualización: --</div>";
  html += "</div>";
  
  // Tarjeta de alertas
  html += "<div class='card alert-card' id='alertCard'>";
  html += "<h3><i class='fas fa-exclamation-triangle'></i> Estado de Alerta</h3>";
  html += "<div class='metric-value' id='alertLevel'>NORMAL</div>";
  html += "<div id='alertMessage'>Niveles dentro de norma</div>";
  html += "<div class='regulations'>";
  html += "<div class='regulation-item'><span>OMS Día (55 dB):</span><span id='whoDay'>--</span></div>";
  html += "<div class='regulation-item'><span>OMS Noche (40 dB):</span><span id='whoNight'>--</span></div>";
  html += "<div class='regulation-item'><span>EPA Urbano (70 dB):</span><span id='epaUrban'>--</span></div>";
  html += "</div></div>";
  
  // Estadísticas
  html += "<div class='card'>";
  html += "<h3><i class='fas fa-chart-bar'></i> Estadísticas (Última Hora)</h3>";
  html += "<div class='statistics-grid'>";
  html += "<div class='stat-item'><div class='stat-value' id='avgDB'>--</div><div>Promedio</div></div>";
  html += "<div class='stat-item'><div class='stat-value' id='maxDB'>--</div><div>Máximo</div></div>";
  html += "<div class='stat-item'><div class='stat-value' id='minDB'>--</div><div>Mínimo</div></div>";
  html += "<div class='stat-item'><div class='stat-value' id='violationsCount'>--</div><div>Violaciones</div></div>";
  html += "</div></div>";
  
  // Clasificación
  html += "<div class='card'>";
  html += "<h3><i class='fas fa-tags'></i> Clasificación de Ruido</h3>";
  html += "<div class='metric-value' id='noiseSource'>Analizando...</div>";
  html += "<div id='sourceDescription'>Determinando origen del ruido</div>";
  html += "<div style='margin-top: 1rem;'>";
  html += "<div><strong>Probabilidad:</strong> <span id='sourceProbability'>--%</span></div>";
  html += "<div><strong>Impacto:</strong> <span id='noiseImpact'>--</span></div>";
  html += "</div></div>";
  
  // Predicción
  html += "<div class='card prediction-card'>";
  html += "<h3><i class='fas fa-crystal-ball'></i> Predicción y Tendencia</h3>";
  html += "<div><strong>Tendencia:</strong> <span id='trendIndicator'>--</span></div>";
  html += "<div><strong>Predicción (5min):</strong> <span id='prediction'>-- dB</span></div>";
  html += "<div class='frequency-display'><i class='fas fa-wave-square'></i> <span id='peakFreq'>-- Hz</span></div>";
  html += "<div style='font-size: 0.9rem; opacity: 0.8;' id='freqType'>Analizando frecuencias...</div>";
  html += "</div>";
  
  html += "</div>"; // Cerrar dashboard-grid
  
  // Gráfico principal
  html += "<div class='card'>";
  html += "<h3><i class='fas fa-chart-line'></i> Tendencia Temporal</h3>";
  html += "<div class='chart-container'><canvas id='trendChart'></canvas></div>";
  html += "</div>";
  
  // Nuevo: Gráfico de frecuencia
  html += "<div class='card'>";
  html += "<h3><i class='fas fa-signal'></i> Análisis Espectral</h3>";
  html += "<div class='chart-container'><canvas id='freqChart'></canvas></div>";
  html += "</div>";
  
  // Nuevo: Mapa de calor por horas
  html += "<div class='card'>";
  html += "<h3><i class='fas fa-calendar-day'></i> Patrón Diario (24h)</h3>";
  html += "<div class='heatmap' id='hourlyHeatmap'></div>";
  html += "<div style='display: flex; justify-content: space-between; margin-top: 0.5rem; font-size: 0.8rem;'>";
  html += "<span>00:00</span><span>06:00</span><span>12:00</span><span>18:00</span><span>23:00</span>";
  html += "</div></div>";
  
  // Controles
  html += "<div class='controls'>";
  html += "<button class='btn active' id='autoBtn' onclick='toggleAutoUpdate()'>";
  html += "<i class='fas fa-play'></i> Auto (2s)</button>";
  html += "<button class='btn' onclick='updateData()'><i class='fas fa-sync'></i> Actualizar</button>";
  html += "<button class='btn' onclick='exportData()'><i class='fas fa-download'></i> Exportar Datos</button>";
  html += "<button class='btn' onclick='generateReport()'><i class='fas fa-file-pdf'></i> Reporte</button>";
  html += "<button class='btn' id='alertBtn' onclick='toggleAlerts()'><i class='fas fa-bell'></i> Alertas: ON</button>";
  html += "</div>";
  
  html += "<div class='notification' id='notification'></div>";
  
  html += "</div>"; // Cerrar container
  
  // JavaScript
  html += "<script>";
  html += "let autoMode = true;";
  html += "let autoInterval;";
  html += "let chart;";
  html += "let freqChart;";
  html += "let dataHistory = [];";
  html += "let maxDataPoints = 50;";
  html += "let sessionData = { readings: [], violations: 0, startTime: Date.now() };";
  html += "let hourlyData = new Array(24).fill(0);";
  html += "let hourlyCounts = new Array(24).fill(0);";
  html += "let alertsEnabled = true;";
  html += "let lastAlert = 0;";
  
  // Función de inicialización de gráfico
  html += "function initChart() {";
  html += "const ctx = document.getElementById('trendChart').getContext('2d');";
  html += "chart = new Chart(ctx, {";
  html += "type: 'line',";
  html += "data: { labels: [], datasets: [{";
  html += "label: 'Nivel de Ruido (dB)', data: [], borderColor: '#64b5f6',";
  html += "backgroundColor: 'rgba(100, 181, 246, 0.1)', tension: 0.4, fill: true";
  html += "}, { label: 'Límite OMS (55 dB)', data: [], borderColor: '#ff9800',";
  html += "borderDash: [5, 5], pointRadius: 0 }, { label: 'Límite EPA (70 dB)',";
  html += "data: [], borderColor: '#f44336', borderDash: [10, 5], pointRadius: 0 }] },";
  html += "options: { responsive: true, maintainAspectRatio: false,";
  html += "plugins: { legend: { labels: { color: 'white' } } },";
  html += "scales: { y: { beginAtZero: true, max: 100, grid: { color: 'rgba(255,255,255,0.1)' },";
  html += "ticks: { color: 'white' } }, x: { grid: { color: 'rgba(255,255,255,0.1)' },";
  html += "ticks: { color: 'white', maxTicksLimit: 10 } } }, animation: { duration: 300 } } });";
  html += "const fctx = document.getElementById('freqChart').getContext('2d');";
  html += "freqChart = new Chart(fctx, { type: 'bar',";
  html += "data: { labels: [], datasets: [{ label: 'Intensidad por Banda',";
  html += "data: [], backgroundColor: ['#e91e63','#9c27b0','#673ab7','#3f51b5','#2196f3','#00bcd4','#009688','#4caf50'] }] },";
  html += "options: { responsive: true, maintainAspectRatio: false,";
  html += "plugins: { legend: { labels: { color: 'white' } } },";
  html += "scales: { y: { beginAtZero: true, grid: { color: 'rgba(255,255,255,0.1)' }, ticks: { color: 'white' } },";
  html += "x: { grid: { color: 'rgba(255,255,255,0.1)' }, ticks: { color: 'white' } } } } });";
  html += "initHeatmap(); }";
  
  // Función updateData
  html += "async function updateData() {";
  html += "try { const response = await fetch('/data'); const data = await response.json();";
  html += "data.timestamp = new Date(); updateMainDisplay(data); updateChart(data);";
  html += "updateStatistics(data); updateAlerts(data); updateNoiseClassification(data);";
  html += "updatePrediction(data); updateFrequencyDisplay(data); updateHourlyData(data);";
  html += "sessionData.readings.push(data); if (sessionData.readings.length > 1000) {";
  html += "sessionData.readings = sessionData.readings.slice(-1000); }";
  html += "document.getElementById('connectionStatus').innerHTML = '<i class=\"fas fa-wifi\"></i><span>Conectado</span>';";
  html += "await updateFrequencyChart();";
  html += "} catch (error) { console.error('Error:', error);";
  html += "document.getElementById('connectionStatus').innerHTML = '<i class=\"fas fa-wifi-slash\"></i><span>Error</span>'; } }";
  
  // updateMainDisplay
  html += "function updateMainDisplay(data) {";
  html += "document.getElementById('currentDB').textContent = data.decibels.toFixed(1) + ' dB';";
  html += "document.getElementById('lastUpdate').textContent = 'Última actualización: ' + data.timestamp.toLocaleTimeString();";
  html += "const category = getNoiseCategory(data.decibels);";
  html += "document.getElementById('noiseCategory').textContent = category.name;";
  html += "document.getElementById('mainCard').className = 'card metric-card ' + category.class; }";
  
  // updateChart
  html += "function updateChart(data) {";
  html += "const time = data.timestamp.toLocaleTimeString();";
  html += "dataHistory.push({ time: time, value: data.decibels });";
  html += "if (dataHistory.length > maxDataPoints) dataHistory = dataHistory.slice(-maxDataPoints);";
  html += "const labels = dataHistory.map(d => d.time); const values = dataHistory.map(d => d.value);";
  html += "const whoLimits = new Array(values.length).fill(55);";
  html += "const epaLimits = new Array(values.length).fill(70);";
  html += "chart.data.labels = labels; chart.data.datasets[0].data = values;";
  html += "chart.data.datasets[1].data = whoLimits; chart.data.datasets[2].data = epaLimits;";
  html += "chart.update('none'); }";
  
  // updateStatistics
  html += "function updateStatistics(data) {";
  html += "if (sessionData.readings.length < 2) return;";
  html += "const recentData = sessionData.readings.slice(-60);";
  html += "const values = recentData.map(r => r.decibels);";
  html += "const avg = values.reduce((a, b) => a + b, 0) / values.length;";
  html += "const max = Math.max(...values); const min = Math.min(...values);";
  html += "document.getElementById('avgDB').textContent = avg.toFixed(1);";
  html += "document.getElementById('maxDB').textContent = max.toFixed(1);";
  html += "document.getElementById('minDB').textContent = min.toFixed(1);";
  html += "document.getElementById('violationsCount').textContent = sessionData.violations; }";
  
  // updateAlerts
  html += "function updateAlerts(data) {";
  html += "const db = data.decibels; let alertLevel = 'NORMAL'; let alertClass = 'alert-green';";
  html += "let message = 'Niveles dentro de norma';";
  html += "if (db >= 85) { alertLevel = 'CRÍTICO'; alertClass = 'alert-critical';";
  html += "message = 'PELIGRO! Riesgo auditivo inmediato'; sessionData.violations++; }";
  html += "else if (db >= 70) { alertLevel = 'ALTO'; alertClass = 'alert-red';";
  html += "message = 'Excede límites EPA'; sessionData.violations++; }";
  html += "else if (db >= 55) { alertLevel = 'MODERADO'; alertClass = 'alert-yellow';";
  html += "message = 'Excede recomendaciones OMS'; sessionData.violations++; }";
  html += "document.getElementById('alertLevel').textContent = alertLevel;";
  html += "document.getElementById('alertMessage').textContent = message;";
  html += "document.getElementById('alertCard').className = 'card alert-card ' + alertClass;";
  html += "document.getElementById('whoDay').textContent = db > 55 ? 'EXCEDE' : 'CUMPLE';";
  html += "document.getElementById('whoNight').textContent = db > 40 ? 'EXCEDE' : 'CUMPLE';";
  html += "document.getElementById('epaUrban').textContent = db > 70 ? 'EXCEDE' : 'CUMPLE'; }";
  
  // updateNoiseClassification
  html += "function updateNoiseClassification(data) {";
  html += "const classification = classifyNoiseSource(data.decibels);";
  html += "document.getElementById('noiseSource').textContent = classification.source;";
  html += "document.getElementById('sourceDescription').textContent = classification.description;";
  html += "document.getElementById('sourceProbability').textContent = classification.probability + '%';";
  html += "document.getElementById('noiseImpact').textContent = classification.impact; }";
  
  // getNoiseCategory
  html += "function getNoiseCategory(db) {";
  html += "if (db < 30) return { name: 'Muy Silencioso', class: 'alert-green' };";
  html += "if (db < 40) return { name: 'Silencioso', class: 'alert-green' };";
  html += "if (db < 55) return { name: 'Moderado', class: 'alert-green' };";
  html += "if (db < 70) return { name: 'Ruidoso', class: 'alert-yellow' };";
  html += "if (db < 85) return { name: 'Muy Ruidoso', class: 'alert-red' };";
  html += "return { name: 'Peligroso', class: 'alert-critical' }; }";
  
  // classifyNoiseSource
  html += "function classifyNoiseSource(db) {";
  html += "if (db < 35) return { source: 'Ambiente Natural', description: 'Sonidos ambientales normales', probability: 85, impact: 'Ninguno' };";
  html += "if (db < 50) return { source: 'Área Residencial', description: 'Actividad doméstica ligera', probability: 78, impact: 'Mínimo' };";
  html += "if (db < 65) return { source: 'Tráfico Ligero', description: 'Vehículos ocasionales', probability: 82, impact: 'Bajo' };";
  html += "if (db < 75) return { source: 'Tráfico Intenso', description: 'Tráfico constante', probability: 88, impact: 'Moderado' };";
  html += "if (db < 85) return { source: 'Actividad Industrial', description: 'Maquinaria pesada', probability: 91, impact: 'Alto' };";
  html += "return { source: 'Fuente Crítica', description: 'Emergencia o evento especial', probability: 95, impact: 'Crítico' }; }";
  
  // toggleAutoUpdate
  html += "function toggleAutoUpdate() {";
  html += "const btn = document.getElementById('autoBtn');";
  html += "if (autoMode) { autoMode = false; clearInterval(autoInterval);";
  html += "btn.innerHTML = '<i class=\"fas fa-pause\"></i> Pausado'; btn.classList.remove('active'); }";
  html += "else { autoMode = true; autoInterval = setInterval(updateData, 2000);";
  html += "btn.innerHTML = '<i class=\"fas fa-play\"></i> Auto (2s)'; btn.classList.add('active'); } }";
  
  // exportData
  html += "function exportData() {";
  html += "const csvData = sessionData.readings.map(r => r.timestamp.toISOString() + ',' + r.decibels + ',' + r.voltage).join('\\n');";
  html += "const header = 'Timestamp,Decibels,Voltage\\n';";
  html += "const blob = new Blob([header + csvData], { type: 'text/csv' });";
  html += "const url = URL.createObjectURL(blob); const a = document.createElement('a');";
  html += "a.href = url; a.download = 'noise_data_' + new Date().toISOString().split('T')[0] + '.csv'; a.click(); }";
  
  // generateReport
  html += "function generateReport() {";
  html += "const report = 'REPORTE DE MONITOREO ACÚSTICO URBANO\\n' +";
  html += "'Fecha: ' + new Date().toLocaleDateString() + '\\n' +";
  html += "'Total mediciones: ' + sessionData.readings.length + '\\n' +";
  html += "'Violaciones: ' + sessionData.violations + '\\n';";
  html += "const blob = new Blob([report], { type: 'text/plain' });";
  html += "const url = URL.createObjectURL(blob); const a = document.createElement('a');";
  html += "a.href = url; a.download = 'reporte_' + new Date().toISOString().split('T')[0] + '.txt'; a.click(); }";
  
  // Nuevas funciones
  html += "async function updateFrequencyChart() {";
  html += "try { const response = await fetch('/frequency'); const data = await response.json();";
  html += "freqChart.data.labels = data.labels; freqChart.data.datasets[0].data = data.bands;";
  html += "freqChart.update('none'); } catch (e) { console.error('Error freq:', e); } }";
  
  html += "function updatePrediction(data) {";
  html += "if (sessionData.readings.length < 10) return;";
  html += "const recent = sessionData.readings.slice(-10).map(r => r.decibels);";
  html += "const avg = recent.reduce((a, b) => a + b, 0) / recent.length;";
  html += "const trend = recent[recent.length-1] - recent[0];";
  html += "const prediction = avg + (trend * 0.5);";
  html += "document.getElementById('prediction').textContent = prediction.toFixed(1) + ' dB';";
  html += "let trendText = '<i class=\"fas fa-minus trend-stable\"></i> Estable';";
  html += "if (trend > 2) trendText = '<i class=\"fas fa-arrow-up trend-up\"></i> Aumentando';";
  html += "else if (trend < -2) trendText = '<i class=\"fas fa-arrow-down trend-down\"></i> Disminuyendo';";
  html += "document.getElementById('trendIndicator').innerHTML = trendText;";
  html += "if (alertsEnabled && prediction > 70 && Date.now() - lastAlert > 30000) {";
  html += "showNotification('⚠️ ALERTA: Se predice nivel de ruido > 70 dB', 'red');";
  html += "lastAlert = Date.now(); } }";
  
  html += "function updateFrequencyDisplay(data) {";
  html += "if (!data.peakFreq) return;";
  html += "document.getElementById('peakFreq').textContent = data.peakFreq + ' Hz';";
  html += "let type = 'Frecuencia desconocida';";
  html += "if (data.peakFreq < 250) type = '🚗 Tráfico vehicular / Motores';";
  html += "else if (data.peakFreq < 500) type = '🗣️ Voces humanas / Conversación';";
  html += "else if (data.peakFreq < 1000) type = '🔔 Alarmas / Música';";
  html += "else if (data.peakFreq < 2000) type = '🏭 Actividad industrial';";
  html += "else if (data.peakFreq < 4000) type = '🚨 Sirenas / Alertas';";
  html += "else type = '🐦 Sonidos agudos / Naturaleza';";
  html += "document.getElementById('freqType').textContent = type; }";
  
  html += "function updateHourlyData(data) {";
  html += "const hour = new Date().getHours();";
  html += "hourlyData[hour] = ((hourlyData[hour] * hourlyCounts[hour]) + data.decibels) / (hourlyCounts[hour] + 1);";
  html += "hourlyCounts[hour]++;";
  html += "updateHeatmap(); }";
  
  html += "function initHeatmap() {";
  html += "const container = document.getElementById('hourlyHeatmap');";
  html += "for (let i = 0; i < 24; i++) {";
  html += "const cell = document.createElement('div');";
  html += "cell.className = 'heatmap-cell'; cell.id = 'hm-' + i;";
  html += "cell.style.background = 'rgba(100, 181, 246, 0.1)';";
  html += "cell.title = i + ':00 - No hay datos'; container.appendChild(cell); } }";
  
  html += "function updateHeatmap() {";
  html += "for (let i = 0; i < 24; i++) {";
  html += "const cell = document.getElementById('hm-' + i);";
  html += "if (hourlyCounts[i] > 0) {";
  html += "const val = hourlyData[i]; let color = 'rgba(76, 175, 80, 0.';";
  html += "if (val > 70) color = 'rgba(244, 67, 54, 0.';";
  html += "else if (val > 55) color = 'rgba(255, 152, 0, 0.';";
  html += "const intensity = Math.min(val / 100, 1);";
  html += "cell.style.background = color + intensity + ')';";
  html += "cell.title = i + ':00 - ' + val.toFixed(1) + ' dB'; } } }";
  
  html += "function showNotification(msg, color) {";
  html += "const notif = document.getElementById('notification');";
  html += "notif.textContent = msg; notif.style.background = 'rgba(244, 67, 54, 0.95)';";
  html += "if (color === 'yellow') notif.style.background = 'rgba(255, 152, 0, 0.95)';";
  html += "notif.style.display = 'block';";
  html += "setTimeout(() => { notif.style.display = 'none'; }, 5000); }";
  
  html += "function toggleAlerts() {";
  html += "alertsEnabled = !alertsEnabled; const btn = document.getElementById('alertBtn');";
  html += "btn.innerHTML = '<i class=\"fas fa-bell\"></i> Alertas: ' + (alertsEnabled ? 'ON' : 'OFF');";
  html += "btn.classList.toggle('active', alertsEnabled); }";
  
  // Inicialización
  html += "window.addEventListener('load', function() {";
  html += "initChart(); updateData(); autoInterval = setInterval(updateData, 2000); });";
  
  html += "</script></body></html>";
  
  server.send(200, "text/html", html);
}

void handleData() {
  String json = "{";
  json += "\"decibels\":" + String(currentdB) + ",";
  json += "\"voltage\":" + String((analogRead(SOUND_SENSOR_PIN) / 4095.0) * 3.3) + ",";
  json += "\"timestamp\":" + String(millis()) + ",";
  json += "\"peakFreq\":" + String(peakFrequency);
  json += "}";
  
  server.send(200, "application/json", json);
}

void handleHistory() {
  String json = "{\"history\":[";
  for (int i = 0; i < HISTORY_SIZE; i++) {
    int idx = (historyIndex + i) % HISTORY_SIZE;
    if (i > 0) json += ",";
    json += String(dbHistory[idx], 1);
  }
  json += "]}";
  
  server.send(200, "application/json", json);
}

void handleFrequency() {
  String json = "{\"bands\":[";
  for (int i = 0; i < 8; i++) {
    if (i > 0) json += ",";
    json += String(frequencyBands[i]);
  }
  json += "],\"labels\":[\"0-125Hz\",\"125-250Hz\",\"250-500Hz\",\"500-1kHz\",\"1k-2kHz\",\"2k-4kHz\",\"4k-8kHz\",\"8k+Hz\"]}";
  
  server.send(200, "application/json", json);
}