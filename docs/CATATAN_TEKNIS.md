# CATATAN TEKNIS & MEMORI PENGEMBANGAN: AFOUND MODS (GTA V ENHANCED)
> **Dokumen ini dibuat sebagai referensi memori permanen untuk AI Agent dan pengembang agar seluruh arsitektur, konfigurasi, riwayat perubahan, dan formula teknis tidak hilang lintas sesi/percakapan.**

---

## 📌 Ringkasan Mod
- **Nama Mod**: **AFOUND MODS** (Enhanced Speedometer HUD & Advanced ESP System)
- **Game Host**: Grand Theft Auto V (PC Enhanced Edition)
- **Runtime Loader**: ScriptHookV (`ScriptHookV.dll`, `dinput8.dll`)
- **Lokasi Source Code**: [`speedometer_mod/main.cpp`](file:///e:/App/GTA_V_Enhanced/GTAVEnhanced/speedometer_mod/main.cpp) (~3.289 baris C++20)
- **Output Binary**: [`Speedometer.asi`](file:///e:/App/GTA_V_Enhanced/GTAVEnhanced/Speedometer.asi) (Folder root GTA V)
- **File Konfigurasi Utama**: [`Speedometer.ini`](file:///e:/App/GTA_V_Enhanced/GTAVEnhanced/Speedometer.ini)
- **Folder Tema Pengguna**: [`speedometer_themes/`](file:///e:/App/GTA_V_Enhanced/GTAVEnhanced/speedometer_themes/)
- **Build Script**: [`speedometer_mod/build.bat`](file:///e:/App/GTA_V_Enhanced/GTAVEnhanced/speedometer_mod/build.bat) (MSVC x64 Native / C++20)

---

## ⌨️ Kontrol Game & Hotkeys (State Terkini)

| Tombol | Kode Virtual | Fungsi Utama |
|---|---|---|
| **F12** | `VK_F12` (123) | **Buka / Tutup Menu AFOUND MODS** (Sekali klik langsung, *instant toggle*, tanpa perlu ditahan). |
| **`[`** | `VK_OEM_4` (219) | **Toggle Master ESP On / Off** (Langsung aktif/nonaktif dari gameplay). |
| **`]`** | `VK_OEM_6` (221) | **Toggle Show / Hide Speedometer** (Langsung sembunyikan/tampilkan speedometer). |
| **Panah Atas / Bawah** | `VK_UP` / `VK_DOWN` | Navigasi menu (Mendukung *Key Repeat*: hold >320ms dengan interval 65ms untuk scroll cepat). |
| **Panah Kiri / Kanan** | `VK_LEFT` / `VK_RIGHT` | Mengubah nilai angka / slider / opsi menu. |
| **Enter** | `VK_RETURN` | Pilih item, masuk submenu, simpan tema kustom. |
| **Esc / Backspace** | `VK_ESCAPE` / `VK_BACK` | Kembali ke halaman menu sebelumnya / keluar dari menu. |
| **Delete** | `VK_DELETE` | Menghapus file tema kustom yang sedang disorot di submenu tema. |

### 🛡️ Proteksi Kontrol & Anti-Double Control
- Saat menu AFOUND terbuka:
  - Tombol ponsel GTA V diblokir (`CONTROLS::DISABLE_CONTROL_ACTION` pada kontrol 27, 172-177, 180-181) dan ponsel disembunyikan/dihancurkan (`MOBILE::DESTROY_MOBILE_PHONE()`).
  - Pemain tetap leluasa mengemudikan kendaraan (maju, mundur, stir) atau berjalan kaki saat menu terbuka.
  - Jika **Pause Menu GTA V** dibuka, atau pengguna menekan tombol menu mod/trainer lain (F1, F3-F8, F10, Insert, Numpad), menu AFOUND otomatis menutup diri dengan elegan untuk mencegah konflik kontrol (*double control*).
  - Tombol `F12`, `[`, dan `]` diproteksi secara khusus agar tidak pernah salah dideteksi sebagai mod pihak ketiga.

---

## 🚗 Sistem Speedometer & HUD Kendaraan

### 1. Gaya Tampilan (Speedometer Styles)
- **Gaya 0: Minimalis (Diatas Minimap)**
  - Posisi default: `X = 0.016`, `Y = 0.740`.
  - Tampilan teks bersih, indikator KM/H, gigi transmisi (`1-6`, `R`, `N`), nama kendaraan, status bahan bakar, dan kerusakan mesin.
- **Gaya 1: Forza Horizon Dial (Kanan Bawah)**
  - Posisi default: `X = 0.885`, `Y = 0.825` (dapat diatur lewat menu atau INI).
  - Arc tachometer 250 derajat melingkar dari angka 0 sampai 10 RPM.
  - Redline dinamis di atas 8.000 RPM (warna merah menyala).
  - Hub gear indicator di tengah lingkaran: warna hijau neon untuk gigi maju, **kuning terang** untuk gigi mundur (`R`).
  - Jarum RPM (*needle*) dengan peredaman fisika halus (`g_dampedRpm`).
  - Pembacaan digital 3-digit speed di samping dial: angka nol di depan (*leading zeros*) diberi warna abu-abu redup (*dimmed*).

### 2. Fitur Dinamis & Animasi UI
- **Pindah Otomatis ke Center Layar (High-Speed Mode)**:
  - Ambang batas default: `70 km/h` (dapat disesuaikan di menu atau dinonaktifkan).
  - Posisi Center: `X = 0.500`, `Y = 0.850`, Skala `1.10`.
  - Transisi menggunakan kurva elastis halus `EaseInOutCubic` dan `EaseOutBack`.
  - Cooldown delay perpindahan: **2 detik** (mencegah speedometer bolak-balik saat kecepatan berada di sekitar ambang batas).
  - Saat berpindah ke center, indikator bahan bakar, kerusakan, dan nama kendaraan memudar (*fade-out*) di posisinya, dan teks gigi tersusun rata sejajar dengan angka kecepatan.
- **Auto-Hide saat 0 km/h (Diam 3 Detik)**:
  - Jika mobil berhenti (`0 km/h`) selama 3 detik, speedometer memudar perlahan agar layar bersih saat parkir/ngobrol. Seketika muncul kembali saat digas.
- **Animasi Transisi Kuning & Kinclong Transmisi (Upshift)**:
  - Saat transisi naik ke gigi berikutnya, label `KM/H`, **angka speedometer**, dan **angka gigi** secara bersamaan bertransisi ke warna kuning menyala (`g_kmhFlashTimer = 0.50s`) lalu memudar halus (*smooth fade*) kembali ke warna putih.
  - Berlaku konsisten di kedua gaya: **Gaya Minimalis** dan **Gaya Forza Horizon**.
  - Efek kilau gelombang (*sine shimmer wave*) aktif khusus saat perpindahan gigi dari 3 ke 4 dan 4 ke 5.
- **FPS Counter & Jam:Menit Real-time**:
  - FPS dihitung rolling setiap 500ms dari frame delta game.
  - Jam dan menit diambil langsung dari waktu dunia GTA V (`TIME::GET_CLOCK_HOURS()`, `TIME::GET_CLOCK_MINUTES()`).

---

## 🎯 Sistem ESP (Wallhack & Visuals)

### 1. Target yang Didukung
- **NPC Global / Warga** (`espFriendly`).
- **NPC Musuh / Jahat** (`espEnemy`):
  - Deteksi otomatis dari blip merah radar (`col == 1 || col == 49 || col == 75 || col == 59`).
  - Deteksi status tempur (`PED::IS_PED_IN_COMBAT`).
  - Deteksi status menembak (`PED::IS_PED_SHOOTING`).
  - Deteksi relasi permusuhan (`PED::GET_RELATIONSHIP_BETWEEN_PEDS >= 4`).
- **Kendaraan** (`espVehicles`).
- **Hewan** (`espAnimals`).
- **Item / Pickups** (`espItems`).

### 2. Penanda Kepala NPC (Head ESP Marker - Revisi Sesuai Ukuran Kepala)
- **Bukan Titik Kecil Lagi**: Menghitung koordinat tulang kepala [`SKEL_Head`](file:///e:/App/GTA_V_Enhanced/GTAVEnhanced/speedometer_mod/main.cpp#L2224) (Bone ID 31086) dengan batas atas kepala (`+0.13m`) dan dagu (`-0.12m`).
- **Dimensi Proporsional**: Menggunakan rasio manusia `headW = headH * 0.78f`.
- **Visual**:
  - 4 garis border yang membingkai se-kepala NPC secara dinamis mengikuti skala jarak 3D.
  - Tint transparan tipis di area kepala.
  - Micro-dot di pusat kepala untuk titik bidik headshot yang akurat.

### 3. Anti-Blinking / Solusi Hilang pada Sudut Kamera Perspektif Tertentu
- **Penyebab Bug Sebelumnya**: Native `GRAPHICS::_WORLD3D_TO_SCREEN2D` mengembalikan `FALSE` seketika jika sebuah titik keluar dari batas frustum layar (misal kaki keluar sedikit di bawah layar atau kepala di atas layar saat kamera menunduk/mendongak). Operator `&&` menyebabkan seluruh box langsung hilang/berkedip.
- **Solusi Multi-Point Frustum Fallback**:
  - Memproyeksikan 3 titik sekaligus: Atas (`topPos`), Bawah (`btmPos`), dan Tengah (`midPos`).
  - Menggunakan logika fallback:
    - Jika kedua titik proyeksi ada (`pTop && pBtm`), gunakan koordinat persis.
    - Jika kaki di luar layar tapi kepala & torso terlihat (`pTop && pMid`), estimasi tinggi dari torso (`(sMidY - sTopY) * 2.0f`).
    - Jika kepala di luar layar tapi kaki & torso terlihat (`pBtm && pMid`), estimasi tinggi ke bawah.
    - Jika hanya satu titik yang terlihat, estimasi tinggi 2D dari dimensi bounding model 3D (`maxDim.z - minDim.z`).
  - Diaplikasikan pada **Ped** dan **Kendaraan**, membuat tampilan ESP 100% stabil tanpa kedip.

### 4. ESP Skeleton (Tulang) Anti-Clip Tembus Pandang
- Membaca koordinat tulang asli GTA V: Kepala, Leher, Spine, Pelvis, Bahu, Lengan, Tangan, Paha, Betis, Kaki.
- **Anti-Clip Camera Bias**: Menggeser posisi 3D tulang sebesar `16 cm` ke arah kamera pemain (`BiasTowardCam`), sehingga garis tulang selalu melayang di permukaan model dan tidak pernah tertutup oleh mesh baju/badan karakter.

### 5. ESP Aim Circle (Center Layar) - 100% Fixed Screen Center Ring
- **Bebas Geser Kamera (100% Fixed Center)**: Posisi lingkaran dikunci mutlak pada pusat layar 2D monitor (`cx = 0.500f, cy = 0.500f`). Tidak lagi menggunakan vektor 3D kamera yang rentan bergeser/goyang saat kamera bergerak atau berputar (terutama kamera kendaraan orang ketiga).
- **Garis Lingkaran Mulus Sempurna**: Dirender menggunakan 96-segmen titik overlap rapat 2D (`dotSize = 0.0017f * thickness`, `dotH = dotSize * aspect`) yang membentuk ring kontinu solid tanpa patahan atau celah.
- **Status Responsif & Halus (Smooth Interpolation)**:
  - **Normal**: Lingkaran rileks (radius `0.024`, warna cyan lembut).
  - **Aim Badan (Lock)**: Berubah warna kuning-oranye penanda sasaran tepat di badan NPC.
  - **Aim Kepala (Critical)**: Lingkaran mengecil halus (radius `50%`), berubah warna merah-crimson neon, memunculkan 4 penanda notch presisi dan micro-cross di pusat titik (penanda siap headshot).

### 6. ESP Kill Satisfying Banner (Free Fire Style - Teks Bersinar Murni)
- **Teks Bersinar Murni Tanpa Background / Garis Luar**: Semua kotak background hitam pekat, garis border luar, dan sayap flare telah dihilangkan. Banner kini hanya menampilkan teks bersih bersinar `- ELIMINATED -` dan `Nx NPC TERBUNUH` yang sangat kontras dengan drop shadow dan outline.
- **Animasi Membesar & Mengecil**: Banner membesar dengan punch bounce selama 2 detik pertama setelah kill, lalu mengecil menjadi badge ringkas menunggu kill berikutnya.
- **Reset Otomatis 10 Detik Inaktivitas**: Jika tidak ada aktivitas membunuh selama 10 detik, banner memudar dan kill count direset kembali ke 0x.

### 7. Sembunyi Sementara saat Tab Senjata (Weapon Wheel)
- Ketika pemain menahan tombol `Tab` atau roda senjata in-game aktif (`UI::IS_HUD_COMPONENT_ACTIVE(19)` atau `CONTROLS::IS_CONTROL_PRESSED(0, 37)`), seluruh tampilan visual ESP otomatis disembunyikan sementara agar tidak mengganggu pemilihan senjata. Begitu Tab dilepas, ESP seketika kembali tampil. Dapat diatur on/off lewat menu (`Sembunyikan saat Tab Senjata`).

### 8. Display Info Jumlah Entitas (Tengah Atas Layar)
- Posisi telah dipindahkan dari kiri atas ke **Tengah Atas Layar** (`drawX = 0.500f - textW * 0.5f, drawY = 0.014f`).
- Ditampilkan tanpa background box, menggunakan teks ber-shadow kontras tinggi (`DrawTextElement`) agar jelas terbaca di atas latar langit maupun perkotaan.
- Menghitung real-time jumlah NPC warga, musuh minimap, mobil, hewan, dan item/pickups.

### 9. Opsi Konfigurasi ESP yang Diperluas
- **Isi Box Transparan (Fill)**: Mengaktifkan tint transparan tipis di dalam kotak 2D bounding box target.
- **Warna Kotak Kepala**: Pilihan warna bingkai kepala: Putih, Sesuai Target, Merah, Cyan, atau Kuning.
- **Jarak Maks Snapline**: Jarak batas garis snapline (50m sampai 500m).
- **Posisi Teks Info Target**: Opsi letak teks nama/jarak: Di Atas Box atau Di Bawah Box.
- **Ukuran Aim Circle**: Slider ukuran radius lingkaran aim circle (0.012 sampai 0.050).
- **Sembunyikan saat Tab Senjata**: Opsi toggle auto-hide ESP saat Weapon Wheel terbuka.

---

## ⚙️ Format File Konfigurasi (Speedometer.ini)

```ini
[SETTINGS]
SpeedometerStyle=1
ForzaPosX=0.8850
ForzaPosY=0.8250
ForzaScale=1.00
PosX=0.0160
PosY=0.7400
SpeedScale=0.70
UnitScale=0.34
GearScale=0.68
CenterPosX=0.5000
CenterPosY=0.8500
CenterSpeedScale=1.10
AutoHideAtZero=1
CenterSpeedThreshold=70
CenterCooldownSec=2
TextColorR=255
TextColorG=255
TextColorB=255
OutlineColorR=0
OutlineColorG=0
OutlineColorB=0
TextOpacity=255
SpeedIntervalMs=230
ShowSpeedo=1
ShowGear=1
ShowVehicleName=1
ShowFps=1
ShowClock=1
ShowFuel=0
ShowDamage=0

; Tombol Shortcut Keyboard:
MenuKey=123          ; F12 (Buka/Tutup Menu)
EspToggleKey=219     ; '[' (Toggle Master ESP)
SpeedoToggleKey=221  ; ']' (Toggle Speedometer)
ToggleKey=123

[ESP]
EspMaster=0
EspBox=1
EspPedBoxScale=1.00
EspVehBoxWidth=1.35
EspSkeleton=1
EspSkeletonDist=140
EspSkeletonFront=1
EspHeadDot=1
EspAnimalHeadDot=0
EspWheelDots=0
EspShowSpeed=1
EspLines=1
EspLineOrigin=0
EspLineAntiClip=0
EspLineThickness=1
EspColorPreset=0
EspTextScale=0.30
EspCounterScale=0.34
EspOpacity=100
EspCrosshair=1
EspCrosshairType=0
EspCrosshairSize=0.0080
EspCrosshairThick=0.0020
EspCrosshairColor=2
EspAimCircle=1
EspAimAutoHide=1
EspAimCircleSize=0.0260
EspKillSatisfying=1
EspKillBannerScale=0.31
EspHealth=1
EspVehHealth=1
EspArmor=0
EspDistance=1
EspName=0
EspEdge=1
EspFramerateMode=1
EspFriendly=1
EspEnemy=1
EspVehicles=1
EspAnimals=1
EspItems=1
EspShowCounters=1
EspIgnoreDead=1
EspMaxDistance=150
EspHideOnWeaponWheel=1
EspBoxFill=0
EspHeadColorMode=0
EspTextPosition=0
EspSnaplineDist=150
EspCustomR=0
EspCustomG=215
EspCustomB=255
EspSkeletonScale=1.30
EspAimCircleRectScale=1.00
EspVelocitySmooth=1
EspTransitionMode=1
EspFocus=0
EspFocusRadius=0.45
EspRectOpacity=240
EspTextOpacity=245
EspFocusWidth=0.45
EspFocusHeight=0.40
EspKillSoundMode=1
EspAnimDuration=0.25
EspShowWeapon=1
EspDamageText=1
EspSkeletonQuality=0

[PREFERENCES]
MenuWidth=0.2900
MenuItemHeight=0.0380
MenuTextScale=0.4300
MenuHeaderScale=0.5000
```

---

## 🛠️ Panduan Build & Kompilasi
Jika melakukan modifikasi pada kode C++:
1. Masuk ke direktori: `e:\App\GTA_V_Enhanced\GTAVEnhanced\speedometer_mod`
2. Jalankan perintah kompilasi:
   ```cmd
   build.bat
   ```
3. Script otomatis memanggil `vcvars64.bat` dari MSVC 2022 Build Tools dan mengompilasi file menjadi `Speedometer.asi` di root folder GTA V.
