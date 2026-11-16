# AFOUND MODS: ESP & Enhanced Speedometer HUD (GTA V)

> **All-in-one In-Game Menu Mod untuk Grand Theft Auto V (PC Enhanced Edition) yang menggabungkan sistem Speedometer HUD dinamis dan sistem ESP (Wallhack/Visuals) berkinerja tinggi berbasis ScriptHookV C++20.**

---

## 📸 Fitur Utama

### 🚗 1. Enhanced Speedometer HUD
- **Dua Gaya Tampilan**:
  - **Gaya 0 (Minimalis)**: Terletak bersih di atas minimap atau di tengah layar.
  - **Gaya 1 (Forza Horizon)**: Tachometer dial melingkar 250° di kanan bawah dengan redline dinamis, gear hub di tengah, dan peredaman jarum RPM (*needle damping*).
- **Animasi Center Transisi (High-Speed Mode)**:
  - Otomatis berpindah ke tengah layar saat kecepatan melewati ambang batas (`70 km/h` default) dengan kurva animasi *EaseInOutCubic* dan jeda cooldown 2 detik.
- **Animasi Perpindahan Gigi (Upshift Shimmer)**:
  - Efek transisi warna kuning menyala dan gelombang kilau (*sine wave shimmer*) saat akselerasi gear naik.
- **Monitoring Hardware & HUD Top**:
  - Jam dunia in-game & Jam nyata laptop.
  - FPS counter rolling 500ms.
  - CPU & GPU usage monitor real-time.
  - Indikator bahan bakar (*fuel*) dan kerusakan mesin (*damage*).
  - Kecepatan jalan kaki (*on-foot speed*) & ketinggian (*altitude*).

---

### 🎯 2. Advanced ESP System (Wallhack & Visuals)
- **Target Tracking**:
  - **Warga / NPC Sipil**: Box, Skeleton, Head Dot, jarak, dan status kecepatan.
  - **Musuh / Polisi / Hostile**: Ditandai otomatis dengan warna khusus saat menembak atau menjadi agresif.
  - **Kendaraan**: 2D Box proporsional, HP/kerusakan kendaraan, dan titik ban.
  - **Hewan & Item**: Pelacakan hewan liar serta senjata/pickup di tanah.
- **Visuals Canggih**:
  - **Head Marker Proporsional**: Membingkai tengkorak kepala NPC sesuai dimensi jarak 3D (`SKEL_Head`).
  - **3D Skeleton Wireframe**: Kerangka tulang NPC tembus dinding (*anti-clip*) dengan opsi kualitas densitas.
  - **Snaplines**: Garis penunjuk target dari bawah layar atau karakter dengan batas jarak dinamis.
  - **Custom Crosshair & Dynamic Aim Circle**: Indikator bidik di tengah layar dengan auto-hide saat tidak menembak.
  - **Kill Satisfying Streak**: Efek banner kill streak animasi popup bergaya kompetitif disertai sound effect khusus.

---

### 📋 3. In-Game Menu & Preset Manager
- Buka/tutup menu secara instan tanpa delay menggunakan tombol **`F12`**.
- Anti-konflik kontrol: memblokir navigasi ponsel GTA V dan otomatis menutup menu jika pause menu dibuka.
- **Sistem Tema & Preset**:
  - Simpan dan muat tema speedometer (`speedometer_themes/`).
  - Simpan dan muat konfigurasi ESP secara instan (`esp_presets/`).
  - Opsi reset pengaturan ke default kapan saja.

---

## ⌨️ Kontrol Game & Shortcut

| Tombol | Fungsi |
|---|---|
| **F12** | Buka / Tutup Menu Utama **AFOUND MODS** |
| **`[`** / **L** | Toggle Master ESP On / Off secara langsung |
| **`]`** | Toggle Sembunyikan / Tampilkan Speedometer HUD |
| **Panah Atas / Bawah** | Navigasi opsi menu (Mendukung scroll cepat saat ditahan) |
| **Panah Kiri / Kanan** | Mengubah angka slider / opsi nilai |
| **Enter** | Masuk submenu / Konfirmasi pilihan |
| **Esc / Backspace** | Kembali ke menu sebelumnya / Keluar |
| **Delete** | Menghapus file tema kustom yang sedang disorot di daftar tema |

---

## 📦 Panduan Instalasi

### Prasyarat:
1. Pastikan Anda sudah menginstal **ScriptHookV** terbaru (`ScriptHookV.dll` dan `dinput8.dll`) di folder utama GTA V Anda.

### Langkah Pemasangan:
1. Download rilis terbaru dari repositori ini.
2. Salin file dan folder berikut langsung ke direktori utama GTA V Anda:
   ```text
   GTA V Root Folder/
   ├── Speedometer.asi
   ├── Speedometer.ini
   ├── speedometer_themes/
   └── esp_presets/
   ```
3. Jalankan game GTA V (Story Mode).
4. Tekan **`F12`** di dalam game untuk membuka menu!

---

## 🛠️ Kompilasi dari Source Code

Proyek ini ditulis dalam **C++20** dan dapat dikompilasi langsung menggunakan **MSVC (Microsoft Visual Studio 2022 / Build Tools)**:

1. Clone repositori ini:
   ```bash
   git clone https://github.com/found-id/ESP-Speedometer-GTA-V-Enhanced-Mods-Menu.git
   ```
2. Buka terminal command prompt / PowerShell, masuk ke folder `src`:
   ```cmd
   cd ESP-Speedometer-GTA-V-Enhanced-Mods-Menu\src
   ```
3. Jalankan script build:
   ```cmd
   build.bat
   ```
4. Output binary `Speedometer.asi` akan otomatis ter-generate di root repositori.

---

## 📄 Struktur Repositori

```text
├── .gitignore
├── README.md
├── Speedometer.asi          # Pre-compiled ASI binary release
├── Speedometer.ini          # Default configuration file
├── src/
│   ├── main.cpp             # Source code utama C++20 (~5.400 lines)
│   └── build.bat            # Script kompilasi MSVC x64
├── sdk/                     # ScriptHookV C++ SDK
├── speedometer_themes/      # Tema kustom speedometer
├── esp_presets/             # Preset konfigurasi ESP
└── docs/
    └── CATATAN_TEKNIS.md    # Catatan arsitektur & formula teknis
```

---

## ⚖️ Lisensi & Penafian
* Mod ini dibuat murni untuk mode **Single Player (Story Mode)**.
* **JANGAN** gunakan mod ini pada GTA Online karena akan memicu sistem anti-cheat (BattlEye).
