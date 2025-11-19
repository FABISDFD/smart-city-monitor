# 🚀 INSTRUCCIONES PARA SUBIR A GITHUB

## ⚡ Opción Rápida (Windows PowerShell)

```powershell
# Ejecutar en PowerShell (reemplaza TU_USUARIO por tu usuario de GitHub)
.\subir_github.ps1 TU_USUARIO
```

Ejemplo:
```powershell
.\subir_github.ps1 johndoe
```

---

## 📝 Pasos Manuales

### 1️⃣ Crear Repositorio en GitHub

1. Ve a: **https://github.com/new**
2. Configuración:
   - **Nombre**: `smart-city-monitor`
   - **Descripción**: Sistema de monitoreo acústico con ESP32
   - **Visibilidad**: ✅ **Público** (o Privado si prefieres)
   - ❌ **NO marcar** "Add a README file"
   - ❌ **NO marcar** "Add .gitignore"
   - ❌ **NO marcar** "Choose a license"
3. Click en **"Create repository"**

### 2️⃣ Conectar tu Repositorio Local

En PowerShell, ejecuta (reemplaza `TU_USUARIO`):

```powershell
git remote add origin https://github.com/TU_USUARIO/smart-city-monitor.git
```

Ejemplo:
```powershell
git remote add origin https://github.com/johndoe/smart-city-monitor.git
```

### 3️⃣ Subir el Código

```powershell
git push -u origin main
```

### 4️⃣ Autenticación GitHub

GitHub te pedirá credenciales:

- **Username**: Tu usuario de GitHub
- **Password**: ⚠️ **NO es tu contraseña**, necesitas un **Personal Access Token**

#### 🔑 Crear Personal Access Token:

1. Ve a: **https://github.com/settings/tokens**
2. Click en **"Generate new token"** → **"Generate new token (classic)"**
3. Configuración:
   - **Note**: `Smart City Monitor Deploy`
   - **Expiration**: 90 días (o más)
   - **Scopes**: ✅ Marcar **`repo`** (acceso completo a repositorios)
4. Click en **"Generate token"**
5. **COPIA EL TOKEN** (solo se muestra una vez)
6. Úsalo como contraseña cuando Git lo pida

---

## ✅ Verificación

Después del push, verifica:

1. **Abre tu repositorio**: `https://github.com/TU_USUARIO/smart-city-monitor`
2. Deberías ver todos los archivos:
   - ✅ `README.md`
   - ✅ `backend/`
   - ✅ `web-hosting/`
   - ✅ `smart_city_dashboard_fixed.ino`
   - ✅ Etc.

---

## 🔄 Próximos Pasos (Después de Subir)

### Conectar con Hostinger

1. **Ir a hPanel**: https://panel.hostinger.com
2. **Ir a Git**: Menú lateral → "Git"
3. **Crear despliegue**:
   - Click en "Crear nuevo despliegue desde Git"
   - Proveedor: **GitHub**
   - Click en **"Autorizar"**
   - Permitir acceso a Hostinger

4. **Configurar Backend**:
   ```
   Repositorio: TU_USUARIO/smart-city-monitor
   Rama: main
   Carpeta origen: backend/
   Destino: domains/tudominio.com/public_html/api
   Auto-deploy: ✅ Activado
   ```

5. **Configurar Frontend**:
   ```
   Repositorio: TU_USUARIO/smart-city-monitor
   Rama: main
   Carpeta origen: web-hosting/
   Destino: domains/tudominio.com/public_html/
   Auto-deploy: ✅ Activado
   ```

6. **Activar Node.js**:
   - hPanel → Node.js → Crear aplicación
   - Puerto: 3000
   - Archivo: server.js
   - Directorio: `/domains/tudominio.com/public_html/api`

7. **Configurar Variables de Entorno**:
   ```
   ESP32_IP=abc123.ngrok-free.app
   PORT=3000
   NODE_ENV=production
   ```

8. **Exponer ESP32**:
   ```powershell
   ngrok http 192.168.1.100:80
   ```
   Copiar URL generada y configurar en `ESP32_IP`

---

## 🛠️ Comandos Git Útiles

### Ver estado
```powershell
git status
```

### Ver remote configurado
```powershell
git remote -v
```

### Ver commits
```powershell
git log --oneline
```

### Actualizar código (después de cambios locales)
```powershell
git add .
git commit -m "Descripción del cambio"
git push origin main
```

### Bajar cambios de GitHub
```powershell
git pull origin main
```

---

## 🐛 Solución de Problemas

### Error: "remote origin already exists"

```powershell
# Eliminar remote existente
git remote remove origin

# Agregar nuevamente
git remote add origin https://github.com/TU_USUARIO/smart-city-monitor.git
```

### Error: "Updates were rejected"

```powershell
# Forzar push (cuidado, sobrescribe GitHub)
git push -u origin main --force
```

### Error: "Authentication failed"

- Verifica que estés usando un **Personal Access Token**, no tu contraseña
- El token debe tener permisos `repo`
- Crear nuevo token: https://github.com/settings/tokens

### Error: "Repository not found"

- Verifica que el repositorio exista en GitHub
- Verifica que el usuario sea correcto
- Si es privado, verifica permisos de acceso

---

## 💡 Consejos

✅ **Guarda tu token** en un lugar seguro  
✅ **No compartas** tu token (es como una contraseña)  
✅ **Usa .gitignore** para no subir archivos sensibles (`.env`, `node_modules/`)  
✅ **Haz commits frecuentes** con mensajes descriptivos  
✅ **Usa ramas** para features nuevas (`git checkout -b mi-feature`)  

---

## 📚 Recursos

- [Git Documentation](https://git-scm.com/doc)
- [GitHub Guides](https://guides.github.com/)
- [GITHUB-HOSTINGER.md](GITHUB-HOSTINGER.md) - Guía completa paso a paso
- [HOSTINGER-GUIA.md](HOSTINGER-GUIA.md) - Guía específica de Hostinger

---

**¿Necesitas ayuda?** Abre un issue en GitHub después de crear el repositorio.

🌟 **¡No olvides darle una estrella al repo cuando lo crees!**
