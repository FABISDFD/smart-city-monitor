// ===== VARIABLES GLOBALES =====
let autoMode = true;
let autoInterval;
let chart;
let freqChart;
let dataHistory = [];
let maxDataPoints = 50;
let sessionData = { readings: [], violations: 0, startTime: Date.now() };
let hourlyData = new Array(24).fill(0);
let hourlyCounts = new Array(24).fill(0);
let alertsEnabled = true;
let lastAlert = 0;

// ===== SIMULADOR DE DATOS =====
function generateRealisticData() {
  const hour = new Date().getHours();
  let baseDB = 45; // Base silenciosa
  
  // Patrón realista según hora del día
  if (hour >= 6 && hour < 9) {
    baseDB = 65; // Hora pico mañana
  } else if (hour >= 9 && hour < 17) {
    baseDB = 58; // Día laboral
  } else if (hour >= 17 && hour < 20) {
    baseDB = 70; // Hora pico tarde
  } else if (hour >= 20 && hour < 23) {
    baseDB = 52; // Noche temprana
  } else {
    baseDB = 40; // Noche/madrugada
  }
  
  // Añadir variación aleatoria
  const variation = (Math.random() - 0.5) * 15;
  const noise = Math.random() * 5;
  const decibels = Math.max(30, Math.min(90, baseDB + variation + noise));
  
  // Generar frecuencia dominante basada en el nivel
  let peakFreq = 125;
  if (decibels > 70) {
    peakFreq = 125 + Math.random() * 250; // Tráfico intenso
  } else if (decibels > 60) {
    peakFreq = 250 + Math.random() * 500; // Conversación/actividad
  } else if (decibels > 50) {
    peakFreq = 500 + Math.random() * 1000; // Ambiente urbano
  } else {
    peakFreq = 1000 + Math.random() * 3000; // Sonidos ambientales
  }
  
  // Bandas de frecuencia simuladas
  const bands = [
    Math.floor(Math.random() * (decibels > 70 ? 80 : 40)), // 0-125Hz
    Math.floor(Math.random() * (decibels > 60 ? 70 : 30)), // 125-250Hz
    Math.floor(Math.random() * (decibels > 50 ? 60 : 25)), // 250-500Hz
    Math.floor(Math.random() * 50),                         // 500-1kHz
    Math.floor(Math.random() * 40),                         // 1k-2kHz
    Math.floor(Math.random() * 30),                         // 2k-4kHz
    Math.floor(Math.random() * 20),                         // 4k-8kHz
    Math.floor(Math.random() * 15)                          // 8k+Hz
  ];
  
  return {
    decibels: parseFloat(decibels.toFixed(1)),
    voltage: (decibels / 100 * 3.3).toFixed(2),
    timestamp: Date.now(),
    peakFreq: Math.floor(peakFreq),
    bands: bands
  };
}

// ===== INICIALIZACIÓN =====
function initChart() {
  const ctx = document.getElementById('trendChart').getContext('2d');
  chart = new Chart(ctx, {
    type: 'line',
    data: {
      labels: [],
      datasets: [{
        label: 'Nivel de Ruido (dB)',
        data: [],
        borderColor: '#64b5f6',
        backgroundColor: 'rgba(100, 181, 246, 0.1)',
        tension: 0.4,
        fill: true
      }, {
        label: 'Límite OMS (55 dB)',
        data: [],
        borderColor: '#ff9800',
        borderDash: [5, 5],
        pointRadius: 0
      }, {
        label: 'Límite EPA (70 dB)',
        data: [],
        borderColor: '#f44336',
        borderDash: [10, 5],
        pointRadius: 0
      }]
    },
    options: {
      responsive: true,
      maintainAspectRatio: false,
      plugins: {
        legend: {
          labels: { color: 'white' }
        }
      },
      scales: {
        y: {
          beginAtZero: true,
          max: 100,
          grid: { color: 'rgba(255,255,255,0.1)' },
          ticks: { color: 'white' }
        },
        x: {
          grid: { color: 'rgba(255,255,255,0.1)' },
          ticks: { color: 'white', maxTicksLimit: 10 }
        }
      },
      animation: { duration: 300 }
    }
  });

  const fctx = document.getElementById('freqChart').getContext('2d');
  freqChart = new Chart(fctx, {
    type: 'bar',
    data: {
      labels: ['0-125Hz', '125-250Hz', '250-500Hz', '500-1kHz', '1k-2kHz', '2k-4kHz', '4k-8kHz', '8k+Hz'],
      datasets: [{
        label: 'Intensidad por Banda',
        data: [],
        backgroundColor: ['#e91e63', '#9c27b0', '#673ab7', '#3f51b5', '#2196f3', '#00bcd4', '#009688', '#4caf50']
      }]
    },
    options: {
      responsive: true,
      maintainAspectRatio: false,
      plugins: {
        legend: { labels: { color: 'white' } }
      },
      scales: {
        y: {
          beginAtZero: true,
          grid: { color: 'rgba(255,255,255,0.1)' },
          ticks: { color: 'white' }
        },
        x: {
          grid: { color: 'rgba(255,255,255,0.1)' },
          ticks: { color: 'white' }
        }
      }
    }
  });

  initHeatmap();
}

function initHeatmap() {
  const container = document.getElementById('hourlyHeatmap');
  for (let i = 0; i < 24; i++) {
    const cell = document.createElement('div');
    cell.className = 'heatmap-cell';
    cell.id = 'hm-' + i;
    cell.style.background = 'rgba(100, 181, 246, 0.1)';
    cell.title = i + ':00 - No hay datos';
    container.appendChild(cell);
  }
}

// ===== ACTUALIZACIÓN DE DATOS =====
function updateData() {
  const data = generateRealisticData();
  data.timestamp = new Date();
  
  updateMainDisplay(data);
  updateChart(data);
  updateStatistics(data);
  updateAlerts(data);
  updateNoiseClassification(data);
  updatePrediction(data);
  updateFrequencyDisplay(data);
  updateHourlyData(data);
  updateFrequencyChart(data);
  
  sessionData.readings.push(data);
  if (sessionData.readings.length > 1000) {
    sessionData.readings = sessionData.readings.slice(-1000);
  }
  
  document.getElementById('connectionStatus').innerHTML = '<i class="fas fa-wifi"></i><span>Simulación Activa</span>';
}

function updateMainDisplay(data) {
  document.getElementById('currentDB').textContent = data.decibels.toFixed(1) + ' dB';
  document.getElementById('lastUpdate').textContent = 'Última actualización: ' + data.timestamp.toLocaleTimeString();
  
  const category = getNoiseCategory(data.decibels);
  document.getElementById('noiseCategory').textContent = category.name;
  document.getElementById('mainCard').className = 'card metric-card ' + category.class;
}

function updateChart(data) {
  const time = data.timestamp.toLocaleTimeString();
  dataHistory.push({ time: time, value: data.decibels });
  
  if (dataHistory.length > maxDataPoints) {
    dataHistory = dataHistory.slice(-maxDataPoints);
  }
  
  const labels = dataHistory.map(d => d.time);
  const values = dataHistory.map(d => d.value);
  const whoLimits = new Array(values.length).fill(55);
  const epaLimits = new Array(values.length).fill(70);
  
  chart.data.labels = labels;
  chart.data.datasets[0].data = values;
  chart.data.datasets[1].data = whoLimits;
  chart.data.datasets[2].data = epaLimits;
  chart.update('none');
}

function updateStatistics(data) {
  if (sessionData.readings.length < 2) return;
  
  const recentData = sessionData.readings.slice(-60);
  const values = recentData.map(r => r.decibels);
  const avg = values.reduce((a, b) => a + b, 0) / values.length;
  const max = Math.max(...values);
  const min = Math.min(...values);
  
  document.getElementById('avgDB').textContent = avg.toFixed(1);
  document.getElementById('maxDB').textContent = max.toFixed(1);
  document.getElementById('minDB').textContent = min.toFixed(1);
  document.getElementById('violationsCount').textContent = sessionData.violations;
}

function updateAlerts(data) {
  const db = data.decibels;
  let alertLevel = 'NORMAL';
  let alertClass = 'alert-green';
  let message = 'Niveles dentro de norma';
  
  if (db >= 85) {
    alertLevel = 'CRÍTICO';
    alertClass = 'alert-critical';
    message = 'PELIGRO! Riesgo auditivo inmediato';
    sessionData.violations++;
  } else if (db >= 70) {
    alertLevel = 'ALTO';
    alertClass = 'alert-red';
    message = 'Excede límites EPA';
    sessionData.violations++;
  } else if (db >= 55) {
    alertLevel = 'MODERADO';
    alertClass = 'alert-yellow';
    message = 'Excede recomendaciones OMS';
    sessionData.violations++;
  }
  
  document.getElementById('alertLevel').textContent = alertLevel;
  document.getElementById('alertMessage').textContent = message;
  document.getElementById('alertCard').className = 'card alert-card ' + alertClass;
  
  document.getElementById('whoDay').textContent = db > 55 ? 'EXCEDE' : 'CUMPLE';
  document.getElementById('whoNight').textContent = db > 40 ? 'EXCEDE' : 'CUMPLE';
  document.getElementById('epaUrban').textContent = db > 70 ? 'EXCEDE' : 'CUMPLE';
  
  // Notificación si está crítico
  if (alertsEnabled && db >= 85 && Date.now() - lastAlert > 30000) {
    showNotification('⚠️ ALERTA CRÍTICA: Nivel de ruido peligroso (' + db.toFixed(1) + ' dB)', 'red');
    lastAlert = Date.now();
  }
}

function updateNoiseClassification(data) {
  const classification = classifyNoiseSource(data.decibels);
  document.getElementById('noiseSource').textContent = classification.source;
  document.getElementById('sourceDescription').textContent = classification.description;
  document.getElementById('sourceProbability').textContent = classification.probability + '%';
  document.getElementById('noiseImpact').textContent = classification.impact;
}

function updatePrediction(data) {
  if (sessionData.readings.length < 10) return;
  
  const recent = sessionData.readings.slice(-10).map(r => r.decibels);
  const avg = recent.reduce((a, b) => a + b, 0) / recent.length;
  const trend = recent[recent.length - 1] - recent[0];
  const prediction = avg + (trend * 0.5);
  
  document.getElementById('prediction').textContent = prediction.toFixed(1) + ' dB';
  
  let trendText = '<i class="fas fa-minus trend-stable"></i> Estable';
  if (trend > 2) {
    trendText = '<i class="fas fa-arrow-up trend-up"></i> Aumentando';
  } else if (trend < -2) {
    trendText = '<i class="fas fa-arrow-down trend-down"></i> Disminuyendo';
  }
  document.getElementById('trendIndicator').innerHTML = trendText;
  
  if (alertsEnabled && prediction > 70 && Date.now() - lastAlert > 30000) {
    showNotification('⚠️ ALERTA: Se predice nivel de ruido > 70 dB', 'yellow');
    lastAlert = Date.now();
  }
}

function updateFrequencyDisplay(data) {
  if (!data.peakFreq) return;
  
  document.getElementById('peakFreq').textContent = data.peakFreq + ' Hz';
  
  let type = 'Frecuencia desconocida';
  if (data.peakFreq < 250) {
    type = '🚗 Tráfico vehicular / Motores';
  } else if (data.peakFreq < 500) {
    type = '🗣️ Voces humanas / Conversación';
  } else if (data.peakFreq < 1000) {
    type = '🔔 Alarmas / Música';
  } else if (data.peakFreq < 2000) {
    type = '🏭 Actividad industrial';
  } else if (data.peakFreq < 4000) {
    type = '🚨 Sirenas / Alertas';
  } else {
    type = '🐦 Sonidos agudos / Naturaleza';
  }
  
  document.getElementById('freqType').textContent = type;
}

function updateFrequencyChart(data) {
  if (!data.bands) return;
  freqChart.data.datasets[0].data = data.bands;
  freqChart.update('none');
}

function updateHourlyData(data) {
  const hour = new Date().getHours();
  hourlyData[hour] = ((hourlyData[hour] * hourlyCounts[hour]) + data.decibels) / (hourlyCounts[hour] + 1);
  hourlyCounts[hour]++;
  updateHeatmap();
}

function updateHeatmap() {
  for (let i = 0; i < 24; i++) {
    const cell = document.getElementById('hm-' + i);
    if (hourlyCounts[i] > 0) {
      const val = hourlyData[i];
      let color = 'rgba(76, 175, 80, 0.';
      if (val > 70) {
        color = 'rgba(244, 67, 54, 0.';
      } else if (val > 55) {
        color = 'rgba(255, 152, 0, 0.';
      }
      const intensity = Math.min(val / 100, 1);
      cell.style.background = color + intensity + ')';
      cell.title = i + ':00 - ' + val.toFixed(1) + ' dB';
    }
  }
}

// ===== FUNCIONES AUXILIARES =====
function getNoiseCategory(db) {
  if (db < 30) return { name: 'Muy Silencioso', class: 'alert-green' };
  if (db < 40) return { name: 'Silencioso', class: 'alert-green' };
  if (db < 55) return { name: 'Moderado', class: 'alert-green' };
  if (db < 70) return { name: 'Ruidoso', class: 'alert-yellow' };
  if (db < 85) return { name: 'Muy Ruidoso', class: 'alert-red' };
  return { name: 'Peligroso', class: 'alert-critical' };
}

function classifyNoiseSource(db) {
  if (db < 35) {
    return { source: 'Ambiente Natural', description: 'Sonidos ambientales normales', probability: 85, impact: 'Ninguno' };
  }
  if (db < 50) {
    return { source: 'Área Residencial', description: 'Actividad doméstica ligera', probability: 78, impact: 'Mínimo' };
  }
  if (db < 65) {
    return { source: 'Tráfico Ligero', description: 'Vehículos ocasionales', probability: 82, impact: 'Bajo' };
  }
  if (db < 75) {
    return { source: 'Tráfico Intenso', description: 'Tráfico constante', probability: 88, impact: 'Moderado' };
  }
  if (db < 85) {
    return { source: 'Actividad Industrial', description: 'Maquinaria pesada', probability: 91, impact: 'Alto' };
  }
  return { source: 'Fuente Crítica', description: 'Emergencia o evento especial', probability: 95, impact: 'Crítico' };
}

function showNotification(msg, color) {
  const notif = document.getElementById('notification');
  notif.textContent = msg;
  notif.style.background = 'rgba(244, 67, 54, 0.95)';
  if (color === 'yellow') {
    notif.style.background = 'rgba(255, 152, 0, 0.95)';
  }
  notif.style.display = 'block';
  setTimeout(() => {
    notif.style.display = 'none';
  }, 5000);
}

// ===== CONTROLES =====
function toggleAutoUpdate() {
  const btn = document.getElementById('autoBtn');
  if (autoMode) {
    autoMode = false;
    clearInterval(autoInterval);
    btn.innerHTML = '<i class="fas fa-pause"></i> Pausado';
    btn.classList.remove('active');
  } else {
    autoMode = true;
    autoInterval = setInterval(updateData, 2000);
    btn.innerHTML = '<i class="fas fa-play"></i> Auto (2s)';
    btn.classList.add('active');
  }
}

function toggleAlerts() {
  alertsEnabled = !alertsEnabled;
  const btn = document.getElementById('alertBtn');
  btn.innerHTML = '<i class="fas fa-bell"></i> Alertas: ' + (alertsEnabled ? 'ON' : 'OFF');
  btn.classList.toggle('active', alertsEnabled);
}

function exportData() {
  const csvData = sessionData.readings.map(r => 
    new Date(r.timestamp).toISOString() + ',' + r.decibels + ',' + r.voltage
  ).join('\n');
  const header = 'Timestamp,Decibels,Voltage\n';
  const blob = new Blob([header + csvData], { type: 'text/csv' });
  const url = URL.createObjectURL(blob);
  const a = document.createElement('a');
  a.href = url;
  a.download = 'noise_data_' + new Date().toISOString().split('T')[0] + '.csv';
  a.click();
  showNotification('✓ Datos exportados correctamente', 'yellow');
}

function generateReport() {
  const report = 'REPORTE DE MONITOREO ACÚSTICO URBANO\n' +
    '=====================================\n\n' +
    'Fecha: ' + new Date().toLocaleDateString() + '\n' +
    'Hora: ' + new Date().toLocaleTimeString() + '\n\n' +
    'Total mediciones: ' + sessionData.readings.length + '\n' +
    'Violaciones detectadas: ' + sessionData.violations + '\n\n' +
    'Estadísticas:\n' +
    '- Nivel promedio: ' + document.getElementById('avgDB').textContent + ' dB\n' +
    '- Nivel máximo: ' + document.getElementById('maxDB').textContent + ' dB\n' +
    '- Nivel mínimo: ' + document.getElementById('minDB').textContent + ' dB\n\n' +
    'Generado por Smart City Monitor\n';
  
  const blob = new Blob([report], { type: 'text/plain' });
  const url = URL.createObjectURL(blob);
  const a = document.createElement('a');
  a.href = url;
  a.download = 'reporte_' + new Date().toISOString().split('T')[0] + '.txt';
  a.click();
  showNotification('✓ Reporte generado correctamente', 'yellow');
}

// ===== INICIALIZACIÓN AL CARGAR =====
window.addEventListener('load', function() {
  initChart();
  updateData();
  autoInterval = setInterval(updateData, 2000);
  
  // Mensaje de bienvenida
  setTimeout(() => {
    showNotification('🏙️ Smart City Monitor iniciado - Modo simulación', 'yellow');
  }, 1000);
});
