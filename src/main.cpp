#include <windows.h>
#include <psapi.h>
#include <dxgi.h>
#include <dxgi1_4.h>
#include <string>
#include <vector>
#include <cmath>
#include <cstdio>
#include <cctype>
#include <algorithm>
#include <utility>
#include <unordered_map>
#include <unordered_set>
#include "main.h"
#include "natives.h"
#include "enums.h"

namespace
{
	struct Config
	{
		// Gaya Tampilan Speedometer
		int speedometerStyle = 0;        // 0: Minimalis (Atas Minimap/Center), 1: Forza Horizon (Kanan Bawah)
		float forzaPosX = 0.890f;        // Posisi X Forza Dial
		float forzaPosY = 0.825f;        // Posisi Y Forza Dial
		float forzaScale = 1.00f;        // Skala Ukuran Forza Dial

		// Posisi Normal di Atas Minimap
		float posX = 0.016f;
		float posY = 0.740f;
		float speedScale = 0.70f;
		float unitScale = 0.34f;
		float gearScale = 0.68f;

		// Posisi & Skala Center Mode
		float centerPosX = 0.500f;
		float centerPosY = 0.850f;
		float centerSpeedScale = 1.10f;

		// Pengaturan Mode Dinamis
		bool autoHideAtZero = false;     // Auto-hide saat 0 km/h selama 3 detik
		int centerSpeedThreshold = 70;   // Kecepatan minimal untuk pindah ke center (0 = OFF, default 70 km/h)
		int centerCooldownSec = 2;       // Jeda lockout perpindahan posisi (default 2 detik)

		// Tampilan & Warna Speedometer
		int textColorR = 255;
		int textColorG = 255;
		int textColorB = 255;
		int outlineColorR = 0;
		int outlineColorG = 0;
		int outlineColorB = 0;
		int textOpacity = 255;
		int speedIntervalMs = 230;       // Interval pembaruan angka kecepatan (default 230ms)
		bool hudTextOutline = true;      // Outline teks HUD Top (FPS, Clock, Time, Alt, Spd, CPU, GPU, dsb)
		int hudTextOpacity = 255;        // Opacity teks HUD Top (30 - 255)

		// Elemen UI Speedometer
		bool showSpeedo = true;
		bool showGear = true;
		bool showVehicleName = true;
		bool showFps = true;             // FPS Counter default ON
		bool showClock = true;           // Jam:Menit in-game time default ON
		bool showRealTime = true;        // Jam:Menit waktu nyata laptop default ON
		bool showRam = false;            // Info RAM Usage & %
		bool showCpu = true;             // Info CPU Usage & %
		bool showGpu = true;             // Info GPU Usage & %
		bool showDisk = false;           // Info Disk Usage & %
		bool showFuel = true;
		bool showDamage = true;
		bool showAltitude = true;        // Posisi ketinggian karakter in-game (ALT: Z m)
		bool showOnFootSpeed = true;     // Kecepatan KM/H karakter saat bergerak di luar kendaraan

		// Fitur ESP (Wallhack / Visuals)
		bool espMaster = false;          // Master toggle ESP (default OFF)
		bool espBox = true;              // 2D Box pada target
		float espPedBoxScale = 1.00f;    // Skala ukuran 2D Box NPC (0.50 - 2.00)
		float espVehBoxWidth = 1.35f;    // Skala lebar 2D Box Kendaraan (0.70 - 2.50)
		bool espSkeleton = true;         // Skeleton bone ESP
		int espSkeletonDist = 140;       // Jarak maksimal render skeleton
		bool espSkeletonFront = true;    // Layer Skeleton Paling Depan dari game (tembus dinding)
		float espSkeletonScale = 1.30f;  // Skala ukuran / tebal rectangle skeleton (0.10 - 2.50)
		bool espHeadDot = true;          // Titik kepala NPC
		bool espAnimalHeadDot = false;   // Titik kepala Hewan
		bool espWheelDots = false;       // Titik ban pada kendaraan
		bool espShowSpeed = true;        // Kecepatan gerak/jalan NPC
		bool espShowWeapon = true;       // Tipe senjata / item yang dipegang target NPC
		bool espDamageText = true;       // Floating damage text saat target terkena damage
		bool espLines = true;            // Snaplines
		int espLineOrigin = 0;           // 0: Karakter, 1: Atas Layar, 2: Bawah Layar, 3: Fixed Atas Edge, 4: Fixed Bawah Edge
		bool espLineAntiClip = false;    // true: Tembus Pandang / Anti-Clip, false: Dihalangi Objek 3D
		int espLineThickness = 1;        // 1: Normal, 2: Tebal, 3: Sangat Tebal
		int espColorPreset = 0;          // 0: Default, 1: Cyberpunk, 2: Tactical, 3: Monochrome, 4: Vaporwave, 5: Kustom
		float espTextScale = 0.30f;      // Ukuran teks ESP
		float espCounterScale = 0.34f;   // Ukuran teks display info jumlah entitas
		int espRectOpacity = 240;        // Opacity ESP Rectangle (Box, Skeleton, Snapline, Dot, Bar)
		int espTextOpacity = 245;        // Opacity Teks ESP (Label, Jarak, Speed, Kill Banner)
		int espOpacity = 100;            // Opasitas Global ESP (kompatibilitas backward)
		int espCustomR = 0;
		int espCustomG = 215;
		int espCustomB = 255;

		// ESP Crosshair
		bool espCrosshair = true;        // Crosshair titik tengah layar
		int espCrosshairType = 0;        // 0: Titik, 1: Plus (+), 2: Kotak, 3: Gap Cross, 4: Chevron (^)
		float espCrosshairSize = 0.0080f;// Ukuran crosshair
		float espCrosshairThick = 0.0020f;// Tebal crosshair
		int espCrosshairColor = 2;       // 0: Hijau, 1: Merah, 2: Putih, 3: Cyan, 4: Kuning, 5: Ungu

		// ESP Aim Circle & Kill Satisfying
		bool espAimCircle = true;        // ESP Circle di center layar dengan indikator aim (badan/kepala)
		float espAimCircleRectScale = 1.00f; // Skala ukuran rectangle titik pada circle aim (0.40 - 2.50)
		bool espAimAutoHide = true;      // Auto-hide Crosshair & Aim Circle kecuali saat membidik/menembak (Hold RClick/LClick)
		int espVelocitySmooth = 1;       // Velocity Smoothing: 0: OFF, 1: Ringan, 2: Halus, 3: Ultra Smooth
		int espTransitionMode = 1;       // Animasi Transisi: 0: OFF, 1: Fade Smooth, 2: Bouncing Pop, 3: Elastic Expand
		float espAnimDuration = 0.25f;   // Durasi Animasi Transisi ESP dalam detik (0.05s - 1.50s)
		bool espFocus = false;           // Fitur Fokus: Hanya menampilkan ESP dari center layar & sekitarnya, memudar ke tepi
		float espFocusRadius = 0.45f;    // Radius fokus pandangan layar (kompatibilitas)
		float espFocusWidth = 0.45f;     // Lebar fokus layar (0.10 - 0.90)
		float espFocusHeight = 0.40f;    // Tinggi fokus layar (0.10 - 0.90)
		bool espKillSatisfying = true;   // ESP Kill Satisfying streak Free Fire style
		float espKillBannerScale = 0.31f;// Ukuran teks banner kill streak saat mengecil (0.25 - 0.65)
		int espKillSoundMode = 1;        // Sound effect kill: 0: OFF, 1: Challenge, 2: Race Placed, 3: Weapon Purchase, 4: Medal Up, 5: Checkpoint

		bool espHealth = true;           // Indikator Darah NPC
		bool espVehHealth = true;        // Indikator Darah/Kerusakan Kendaraan
		bool espArmor = false;           // Indikator Armor
		bool espDistance = true;         // Indikator Jarak (Meter)
		bool espName = false;            // Nama / Label tipe target
		bool espEdge = true;             // ESP Edge untuk target di luar/belakang layar
		bool espFriendly = true;         // Target NPC Warga / Global
		bool espEnemy = true;            // Target NPC Musuh / Jahat (Minimap merah / menyerang)
		bool espVehicles = true;         // Target Kendaraan
		bool espAnimals = true;          // Target Hewan
		bool espItems = true;            // Target Item / Pickups
		int espFramerateMode = 1;        // 0: Normal (Game FPS), 1: 60 FPS, 2: 30 FPS, 3: 15 FPS
		bool espShowCounters = true;     // Display info jumlah entitas in-game
		bool espIgnoreDead = true;       // Sembunyikan target yang sudah mati
		int espMaxDistance = 150;        // Jarak maksimal ESP (meter)
		bool espHideOnWeaponWheel = true;// Sembunyikan ESP sementara saat Tab Senjata atau Q Cover ditekan
		float espAimCircleSize = 0.0260f;// Radius normal ESP Aim Circle
		bool espBoxFill = false;         // Isi transparan dalam 2D box (Fill)
		int espHeadColorMode = 0;        // 0: Putih, 1: Sesuai Target, 2: Merah, 3: Cyan, 4: Kuning
		int espTextPosition = 0;         // 0: Di Atas Box, 1: Di Bawah Box
		int espSnaplineDist = 150;       // Jarak maksimal snaplines (meter)

		// Preferensi Ukuran Menu & Teks UI
		float menuWidth = 0.2900f;       // Lebar menu UI
		float menuItemHeight = 0.0380f;  // Tinggi bar item menu
		float menuTextScale = 0.4300f;   // Ukuran font teks item menu
		float menuHeaderScale = 0.5000f; // Ukuran font judul header menu

		// Pengaturan Bahasa (0: Bahasa Indonesia, 1: English)
		int language = 0;
		// Kualitas Densitas Rectangle Skeleton (0: Tinggi / 100%, 1: Sedang / -50%, 2: Hemat / -75%)
		int espSkeletonQuality = 0;

		// Tombol Shortcut Keyboard
		DWORD menuKey = VK_F12;          // Buka / Tutup Menu (Klik F12 langsung, tanpa hold)
		DWORD espToggleKey = 'L';        // Toggle Master ESP ('L' = 0x4C)
		DWORD speedoToggleKey = VK_OEM_6;// Toggle Show/Hide Speedometer (']' = 221)
		DWORD toggleKey = VK_F12;        // Kompatibilitas pengaturan lama
	} g_cfg;

	// Helper Multi-Bahasa UI
	inline const char* L(const char* idStr, const char* enStr)
	{
		return (g_cfg.language == 1) ? enStr : idStr;
	}

	char g_iniPath[MAX_PATH] = { 0 };
	char g_themesDir[MAX_PATH] = { 0 };
	char g_espPresetsDir[MAX_PATH] = { 0 };

	// Input Buffer (Thread-Safe)
	volatile bool g_f12IsDown = false;
	volatile bool g_isKeyDown[256] = { false };
	volatile bool g_keyJustPressed[256] = { false };

	// Key Repeat Hold Timers untuk Menu Navigasi
	ULONGLONG g_keyHoldStartTick[256] = { 0 };
	ULONGLONG g_keyLastRepeatTick[256] = { 0 };
	ULONGLONG g_lastEspRenderTick = 0;

	// Status UI Speedometer & Animasi
	float g_currentAlpha = 0.0f;
	float g_dampedSpeed = 0.0f;
	float g_dampedRpm = 0.20f;
	int g_displaySpeed = 0;
	ULONGLONG g_lastSpeedSampleTime = 0;
	float g_speedPulseTimer = 0.0f;

	std::string g_currentGearStr = "N";
	std::string g_lastGearStr = "N";
	float g_gearAnimTimer = 0.0f;
	float g_shimmerTimer = 0.0f;     // Animasi kinclong pada teks speedometer saat 3->4 dan 4->5
	float g_kmhFlashTimer = 0.0f;    // Animasi kuning KM/H lalu fade ke putih
	ULONGLONG g_lastTickTime = 0;

	// Tracking Waktu Kematian NPC (Penanda Mati 2 Detik)
	std::unordered_map<int, ULONGLONG> g_pedDeathTime;

	// ESP Aim Circle (Center Screen)
	float g_aimCircleCurRadius = 0.024f;
	float g_aimCircleCurR = 210.0f;
	float g_aimCircleCurG = 240.0f;
	float g_aimCircleCurB = 255.0f;
	float g_aimCircleCurAlpha = 145.0f;
	int g_aimTargetState = 0; // 0: None, 1: Body (lock), 2: Head (critical target)

	// ESP Kill Satisfying Streak Banner (Free Fire Style)
	int g_killStreakCount = 0;
	ULONGLONG g_lastKillTick = 0;
	float g_killBannerScaleBlend = 0.0f;
	bool g_lastKillIsAnimal = false;
	std::string g_lastKillAnimalName = "";

	// Animasi Dynamic 0 km/h auto-hide
	ULONGLONG g_stoppedSince = 0;
	float g_stoppedDimMultiplier = 1.0f;

	// Dynamic Center Mode (> 70 km/h) dengan Cooldown & Debounce
	bool g_isCenterMode = false;
	float g_centerBlend = 0.0f;
	ULONGLONG g_lastModeSwitchTick = 0;
	ULONGLONG g_belowThreshSince = 0;

	// FPS Counter
	float g_fps = 60.0f;
	int g_fpsFrames = 0;
	ULONGLONG g_fpsLastTime = 0;

	// Hardware Monitor Data (CPU, GPU, RAM, Disk)
	float g_cpuPercent = 0.0f;
	float g_ramUsedGB = 0.0f;
	float g_ramTotalGB = 0.0f;
	int g_ramPercent = 0;
	float g_gpuCorePercent = 0.0f;
	int g_gpuVramUsedMB = 0;
	int g_gpuVramTotalMB = 0;
	float g_gpuVramPercent = 0.0f;
	int g_diskPercent = 0;
	float g_diskUsedGB = 0.0f;
	float g_diskTotalGB = 0.0f;
	ULONGLONG g_lastHardwareSampleTick = 0;
	ULARGE_INTEGER g_prevIdle = { 0 };
	ULARGE_INTEGER g_prevKernel = { 0 };
	ULARGE_INTEGER g_prevUser = { 0 };

	// Penanganan Tahan F12
	ULONGLONG g_f12DownTime = 0;
	bool g_f12HoldTriggered = false;

	// Halaman Menu AFOUND MODS
	enum MenuPage
	{
		PAGE_MAIN,        // Beranda Utama
		PAGE_SPEEDOMETER, // Menu Speedometer & HUD
		PAGE_ESP,         // Menu ESP (Visuals)
		PAGE_PREFERENCES, // Menu Preferensi Ukuran UI & Teks
		PAGE_THEMES,      // Kelola Tema Speedometer
		PAGE_COLORS,      // Warna Teks Speedometer
		PAGE_OUTLINE,     // Warna Outline Speedometer
		PAGE_ESP_PRESETS  // Kelola Preset ESP
	};

	MenuPage g_menuPage = PAGE_MAIN;
	bool g_menuOpen = false;
	int g_menuIndex = 0;
	int g_menuScrollOffset = 0;
	std::string g_statusMsg = "";
	ULONGLONG g_statusMsgTime = 0;

	// Themes & Presets
	std::vector<std::string> g_themeFiles;
	int g_themeIndex = 0;
	std::vector<std::string> g_espPresetFiles;
	int g_espPresetIndex = 0;

	// -------------------------------------------------------------
	// KURVA EASING
	// -------------------------------------------------------------
	float EaseOutBack(float t)
	{
		t = std::clamp(t, 0.0f, 1.0f);
		const float c1 = 1.15f;
		const float c3 = c1 + 1.0f;
		float p = t - 1.0f;
		return 1.0f + c3 * p * p * p + c1 * p * p;
	}

	float EaseInOutCubic(float t)
	{
		t = std::clamp(t, 0.0f, 1.0f);
		return t < 0.5f ? 4.0f * t * t * t : 1.0f - std::pow(-2.0f * t + 2.0f, 3.0f) * 0.5f;
	}

	void PlayFrontendSound(const char* soundName)
	{
		AUDIO::PLAY_SOUND_FRONTEND(-1, (char*)soundName, (char*)"HUD_FRONTEND_DEFAULT_SOUNDSET", FALSE);
	}

	// -------------------------------------------------------------
	// SISTEM VELOCITY SMOOTHING & TRANSISI ANIMASI ESP (PER-ENTITY)
	// -------------------------------------------------------------
	struct EntitySmoothRecord
	{
		float curX = 0.0f;
		float curTopY = 0.0f;
		float curBtmY = 0.0f;
		float curW = 0.0f;
		float curH = 0.0f;
		ULONGLONG lastSeenTick = 0;
		bool initialized = false;
	};
	std::unordered_map<int, EntitySmoothRecord> g_entitySmoothMap;

	struct EntityAnimRecord
	{
		float animProgress = 0.0f; // 0.0f -> 1.0f
		ULONGLONG lastSeenTick = 0;
		bool wasOnScreen = false;
	};
	std::unordered_map<int, EntityAnimRecord> g_entityAnimMap;
	float g_espMasterAnimProgress = 1.0f;
	bool g_lastEspMasterActive = false;

	// Floating Damage Text System
	struct DamageIndicator
	{
		Vector3 worldPos;
		int damage;
		ULONGLONG spawnTick;
		bool isHeadshot;
	};
	std::vector<DamageIndicator> g_damageIndicators;
	std::unordered_map<Ped, int> g_lastPedHealth;

	// Model Hash Uang & Wallet Pickups Game
	static const std::unordered_set<Hash> s_cashModels = {
		0xDECE14D2 /* prop_anim_cash_pile */,
		0x2213D5E4 /* prop_cash_pile_01 */,
		0x46429A88 /* prop_cash_pile_02 */,
		0x113FD533 /* prop_money_bag_01 */,
		0x2B4B2720 /* prop_ld_wallet_01 */,
		0x673C148F /* prop_ld_wallet_02 */,
		0xE040C522 /* prop_paper_bag_small */,
		0x82846871 /* prop_poly_bag_money */,
		0xE8A915C1 /* prop_cash_envelope_01 */,
		0x6F6C1530 /* prop_cash_case_01 */,
		0x9953F974 /* prop_cash_case_02 */,
		0xFD46AC56 /* prop_security_case_01 */
	};

	// Nama Senjata GTA V untuk ESP Display
	std::string GetWeaponDisplayName(Hash hash)
	{
		switch (hash)
		{
		// Pistols
		case 0x1B06D571: return "Pistol";
		case 0xBBDFF10:  return "Combat Pistol";
		case 0x5EF9FEC8: return "AP Pistol";
		case 0x99AEEB44: return "Pistol .50";
		case 0xBFD21232: return "SNS Pistol";
		case 0xD205520E: return "Heavy Pistol";
		case 0x83839C4:  return "Vintage Pistol";
		case 0xDC4DB296: return "Marksman Pistol";
		case 0xC1B3C3D1: return "Revolver";
		case 0xCB96392F: return "Double Action";
		case 0x97EA20B8: return "Flare Gun";
		case 0x3656C8C1: return "Stun Gun";
		case 0x2B5EF5EC: return "Atomizer";
		case 0x555AF23A: return "Pistol Mk II";
		case 0x84D676D4: return "SNS Pistol Mk II";

		// Submachine Guns
		case 0x13532244: return "Micro SMG";
		case 0x2BE6766B: return "SMG";
		case 0xEFE7E2DF: return "Assault SMG";
		case 0x0A3D4D34: return "Combat PDW";
		case 0xDB1A4F3A: return "Machine Pistol";
		case 0xBD248B55: return "Mini SMG";
		case 0x787F0BB:  return "SMG Mk II";

		// Shotguns
		case 0x1D073A89: return "Pump Shotgun";
		case 0x7846A318: return "Sawed-Off";
		case 0x9883953A: return "Bullpup Shotgun";
		case 0xE284C533: return "Assault Shotgun";
		case 0x9D61E50F: return "Musket";
		case 0x3AABBBA3: return "Heavy Shotgun";
		case 0xEF951FBB: return "Double Barrel";
		case 0x12E34D4D: return "Auto Shotgun";
		case 0x5A96BA4:  return "Combat Shotgun";

		// Assault Rifles
		case 0xBFEFFF6D: return "Assault Rifle";
		case 0x83BF0278: return "Carbine Rifle";
		case 0xAF113F99: return "Advanced Rifle";
		case 0xC0A3098D: return "Special Carbine";
		case 0x7F229F94: return "Bullpup Rifle";
		case 0x624FE30:  return "Compact Rifle";
		case 0x394F415C: return "Assault Rifle Mk II";
		case 0xFAD1F0E9: return "Carbine Rifle Mk II";
		case 0xBFE856D0: return "Special Carbine Mk II";
		case 0x88374054: return "Bullpup Rifle Mk II";
		case 0x64402D0E: return "Tactical Rifle";

		// Light Machine Guns
		case 0x9D07F764: return "MG";
		case 0x7FD62962: return "Combat MG";
		case 0x6101447D: return "Gusenberg";
		case 0xDBBD7280: return "Combat MG Mk II";

		// Sniper Rifles
		case 0x05FC3C11: return "Sniper";
		case 0x0C472FE2: return "Heavy Sniper";
		case 0xC734385A: return "Marksman Rifle";
		case 0xA914799:  return "Heavy Sniper Mk II";
		case 0x6E7DDDEC: return "Precision Rifle";

		// Heavy & Explosive
		case 0xB1CA77B1: return "RPG";
		case 0xA284510B: return "Grenade Launcher";
		case 0x42BF8A46: return "Minigun";
		case 0x7F5FC5E2: return "Firework";
		case 0x63AB0442: return "Homing Launcher";
		case 0x6D544C2E: return "Railgun";
		case 0x07817DAB: return "Compact Launcher";
		case 0xAF3696A1: return "Widowmaker";

		// Throwables
		case 0x93E220BD: return "Granat";
		case 0x2C3731D9: return "Sticky Bomb";
		case 0xAB564B78: return "Proximity Mine";
		case 0xFDBC8A50: return "Smoke Bomb";
		case 0x24B17070: return "Molotov";
		case 0x060BEAB5: return "Pipe Bomb";
		case 0x497FACC3: return "Flare";
		case 0x34A67B97: return "Jerigen Bensin";

		// Melee Weapons
		case 0x99AEEB3B:
		case 0x99B507EA: return "Pisau";
		case 0x678CD35:
		case 0x678B81B1: return "Pentungan";
		case 0x4E875F73: return "Palu";
		case 0x958798FB:
		case 0x958A4A8F: return "Tongkat Bisbol";
		case 0x440E4788: return "Tongkat Golf";
		case 0x84BD7BFD:
		case 0x84FFB970: return "Linggis";
		case 0xF9E6AA4B:
		case 0xF9DCBF2D: return "Botol Pecah";
		case 0x92A27587:
		case 0x92A278A9: return "Belati";
		case 0xD8ADD370: return "Knuckle";
		case 0xDD5DF8D9: return "Golok / Machete";
		case 0x8BB05FD7:
		case 0x190426D8: return "Senter";
		case 0xDFE37640: return "Switchblade";
		case 0xCD274149: return "Kapak Perang";
		case 0x94117305: return "Kunci Pipa";
		case 0x19044EE0: return "Kunci Inggris";
		case 0x3813FC08: return "Kapak Batu";

		default:
			if (hash != 0 && hash != 0xA2719248) return "Senjata";
			return "";
		}
	}

	bool IsWeaponModel(Hash model)
	{
		static std::unordered_set<Hash> s_weaponModels;
		static bool s_init = false;
		if (!s_init)
		{
			s_init = true;
			static const Hash wepHashes[] = {
				0x1B06D571, 0xBBDFF10, 0x5EF9FEC8, 0x99AEEB44, 0xBFD21232, 0xD205520E,
				0x83839C4, 0xDC4DB296, 0xC1B3C3D1, 0xCB96392F, 0x97EA20B8, 0x3656C8C1,
				0x2B5EF5EC, 0x555AF23A, 0x84D676D4, 0x13532244, 0x2BE6766B, 0xEFE7E2DF,
				0x0A3D4D34, 0xDB1A4F3A, 0xBD248B55, 0x787F0BB, 0x1D073A89, 0x7846A318,
				0x9883953A, 0xE284C533, 0x9D61E50F, 0x3AABBBA3, 0xEF951FBB, 0x12E34D4D,
				0x5A96BA4, 0xBFEFFF6D, 0x83BF0278, 0xAF113F99, 0xC0A3098D, 0x7F229F94,
				0x624FE30, 0x394F415C, 0xFAD1F0E9, 0xBFE856D0, 0x88374054, 0x64402D0E,
				0x9D07F764, 0x7FD62962, 0x6101447D, 0xDBBD7280, 0x05FC3C11, 0x0C472FE2,
				0xC734385A, 0xA914799, 0x6E7DDDEC, 0xB1CA77B1, 0xA284510B, 0x42BF8A46,
				0x7F5FC5E2, 0x63AB0442, 0x6D544C2E, 0x07817DAB, 0xAF3696A1, 0x99B507EA,
				0x99AEEB3B, 0x678CD35, 0x4E875F73, 0x958798FB, 0x440E4788, 0x84BD7BFD,
				0xF9E6AA4B, 0x92A27587, 0xF9DCBF2D, 0xD8ADD370, 0xDD5DF8D9, 0x8BB05FD7,
				0xDFE37640, 0xCD274149, 0x94117305, 0x19044EE0, 0x3813FC08
			};
			for (Hash h : wepHashes)
			{
				Hash m = (Hash)WEAPON::GET_WEAPONTYPE_MODEL(h);
				if (m != 0) s_weaponModels.insert(m);
			}
		}
		return s_weaponModels.find(model) != s_weaponModels.end();
	}

	// -------------------------------------------------------------
	// FITUR FOKUS PANDANGAN CENTER LAYAR (LEBAR & TINGGI DAPAT DIATUR)
	// Menghitung multiplier opasitas berdasarkan jarak elips ke tengah layar
	// -------------------------------------------------------------
	inline float GetFocusAlphaMultiplier(float screenX, float screenY)
	{
		if (!g_cfg.espFocus) return 1.0f;

		float fw = std::max(0.05f, g_cfg.espFocusWidth);
		float fh = std::max(0.05f, g_cfg.espFocusHeight);

		float dx = (screenX - 0.500f) / fw;
		float dy = (screenY - 0.500f) / fh;
		float distNorm = std::sqrt(dx * dx + dy * dy);

		float innerNorm = 0.40f;
		float outerNorm = 1.00f;

		if (distNorm <= innerNorm) return 1.0f;
		if (distNorm >= outerNorm) return 0.0f;

		float t = (distNorm - innerNorm) / (outerNorm - innerNorm);
		return 0.5f + 0.5f * std::cos(t * 3.14159265f);
	}

	// -------------------------------------------------------------
	// DETEKSI SENJATA AKTUAL NPC (BUKAN PROP SEPERTI KOPI / MANCING)
	// -------------------------------------------------------------
	bool IsPedHoldingRealWeapon(Ped ped, Hash& outWeapon)
	{
		// 1. Senjata Api atau Peledak (Guns, Heavy Weapons, Explosives)
		if (WEAPON::IS_PED_ARMED(ped, 4 | 2))
		{
			WEAPON::GET_CURRENT_PED_WEAPON(ped, &outWeapon, TRUE);
			return true;
		}

		// 2. Senjata Melee Resmi
		if (WEAPON::GET_CURRENT_PED_WEAPON(ped, &outWeapon, TRUE))
		{
			if (outWeapon != 0 && outWeapon != 0xA2719248 /* WEAPON_UNARMED */)
			{
				// Jika NPC sedang dalam ambient scenario (mancing, minum kopi, dsb), bukan senjata
				if (PED::IS_PED_USING_ANY_SCENARIO(ped))
				{
					return false;
				}

				static const std::unordered_set<Hash> s_realMeleeWeapons = {
					0x99AEEB3B /* Knife */, 0x678CD35 /* Nightstick */, 0x4E875F73 /* Hammer */,
					0x958798FB /* Bat */, 0x440E4788 /* GolfClub */, 0x84BD7BFD /* Crowbar */,
					0xF9E6AA4B /* Bottle */, 0x92A27587 /* Dagger */, 0xF9DCBF2D /* Hatchet */,
					0xD8ADD370 /* Knuckle */, 0xDD5DF8D9 /* Machete */, 0x8BB05FD7 /* Flashlight */,
					0xDFE37640 /* Switchblade */, 0xCD274149 /* Battleaxe */, 0x94117305 /* Poolcue */,
					0x19044EE0 /* Wrench */, 0x3813FC08 /* Stone Hatchet */
				};
				if (s_realMeleeWeapons.find(outWeapon) != s_realMeleeWeapons.end())
				{
					return true;
				}

				int dmgType = WEAPON::GET_WEAPON_DAMAGE_TYPE(outWeapon);
				if (dmgType == 2 || dmgType == 3 || dmgType == 5 || dmgType == 6)
				{
					return true;
				}
			}
		}
		return false;
	}

	// -------------------------------------------------------------
	// DETEKSI APAKAH NPC PUNYA TUJUAN MENYERANG PEMAIN
	// -------------------------------------------------------------
	bool IsPedAttackingPlayer(Ped ped, Ped playerPed, Vehicle playerVeh)
	{
		if (PED::IS_PED_IN_COMBAT(ped, playerPed)) return true;

		// Cek target combat task ped
		Entity combatTarget = invoke<Entity>(0x32C27A11307B01CC, ped, 0);
		if (combatTarget == playerPed || (playerVeh != 0 && combatTarget == playerVeh)) return true;

		if (PED::IS_PED_IN_COMBAT(ped, 0))
		{
			int rel = PED::GET_RELATIONSHIP_BETWEEN_PEDS(ped, playerPed);
			if (rel >= 4) return true; // Hubungan Dislike atau Hate
		}

		if (ENTITY::HAS_ENTITY_BEEN_DAMAGED_BY_ENTITY(playerPed, ped, TRUE)) return true;

		return false;
	}

	// -------------------------------------------------------------
	// NAMA SPESIFIK HEWAN DI GTA V
	// -------------------------------------------------------------
	std::string GetAnimalName(Hash model)
	{
		static const std::unordered_map<Hash, const char*> s_animalNames = {
			{ 0xCE5FF426 /* a_c_boar */, "BABI HUTAN" },
			{ 0x573DE46A /* a_c_cat_01 */, "KUCING" },
			{ 0xAAB71F62 /* a_c_chickenhawk */, "ELANG" },
			{ 0xA8652576 /* a_c_chimp */, "SIMPANSE" },
			{ 0x14EC1734 /* a_c_chop */, "ANJING (Chop)" },
			{ 0x56E2990B /* a_c_cormorant */, "BURUNG" },
			{ 0xFC55C243 /* a_c_cow */, "SAPI" },
			{ 0x6AF51453 /* a_c_coyote */, "KOYOTE" },
			{ 0x18012A9F /* a_c_crow */, "BURUNG GAGAK" },
			{ 0xD86B5A95 /* a_c_deer */, "RUSA" },
			{ 0x8D8AC8B9 /* a_c_dolphin */, "LUMBA-LUMBA" },
			{ 0x2FD8017B /* a_c_fish */, "IKAN" },
			{ 0x6AF4B680 /* a_c_hen */, "AYAM" },
			{ 0x4E8F9902 /* a_c_husky */, "ANJING (Husky)" },
			{ 0x8D1EE879 /* a_c_killerwhale */, "PAUS ORCA" },
			{ 0x1250D7BA /* a_c_mountainlion */, "SINGA GUNUNG" },
			{ 0xB11DA9E0 /* a_c_pig */, "BABI" },
			{ 0x06A20728 /* a_c_pigeon */, "BURUNG MERPATI" },
			{ 0xDFB55C81 /* a_c_poodle */, "ANJING (Poodle)" },
			{ 0x6D362854 /* a_c_pug */, "ANJING (Pug)" },
			{ 0xDFB2211E /* a_c_rabbit_01 */, "KELINCI" },
			{ 0xC3B5066E /* a_c_rat */, "TIKUS" },
			{ 0x349F33E1 /* a_c_retriever */, "ANJING (Retriever)" },
			{ 0xC2D06F94 /* a_c_rhesus */, "MONYET" },
			{ 0x9563221D /* a_c_rottweiler */, "ANJING (Rottweiler)" },
			{ 0xD39392D5 /* a_c_seagull */, "BURUNG CAMAR" },
			{ 0x3C83D537 /* a_c_sharkhammer */, "HIU MARTIL" },
			{ 0x06C3F072 /* a_c_sharktiger */, "HIU MACAN" },
			{ 0x431D501A /* a_c_shepherd */, "ANJING (Shepherd)" },
			{ 0x8A833449 /* a_c_westy */, "ANJING (Westy)" }
		};

		auto it = s_animalNames.find(model);
		if (it != s_animalNames.end()) return it->second;
		return "HEWAN";
	}

	void InitPaths()
	{
		GetModuleFileNameA(NULL, g_iniPath, MAX_PATH);
		char* lastSlash = strrchr(g_iniPath, '\\');
		if (lastSlash)
		{
			*(lastSlash + 1) = '\0';
			strcpy_s(g_themesDir, g_iniPath);
			strcat_s(g_themesDir, "speedometer_themes\\");
			strcpy_s(g_espPresetsDir, g_iniPath);
			strcat_s(g_espPresetsDir, "esp_presets\\");
			strcat_s(g_iniPath, "Speedometer.ini");
		}
		CreateDirectoryA(g_themesDir, NULL);
		CreateDirectoryA(g_espPresetsDir, NULL);
	}

	void ScanThemes()
	{
		g_themeFiles.clear();
		char searchPattern[MAX_PATH];
		sprintf_s(searchPattern, "%s*.ini", g_themesDir);

		WIN32_FIND_DATAA fd;
		HANDLE hFind = FindFirstFileA(searchPattern, &fd);
		if (hFind != INVALID_HANDLE_VALUE)
		{
			do
			{
				if (!(fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
				{
					g_themeFiles.push_back(fd.cFileName);
				}
			} while (FindNextFileA(hFind, &fd));
			FindClose(hFind);
		}
		if (g_themeIndex >= (int)g_themeFiles.size())
			g_themeIndex = 0;
	}

	void SaveEspPreset(const char* fullPath);
	void LoadEspPreset(const char* fullPath);

	void ScanEspPresets()
	{
		g_espPresetFiles.clear();
		char searchPattern[MAX_PATH];
		sprintf_s(searchPattern, "%s*.ini", g_espPresetsDir);

		WIN32_FIND_DATAA fd;
		HANDLE hFind = FindFirstFileA(searchPattern, &fd);
		if (hFind != INVALID_HANDLE_VALUE)
		{
			do
			{
				if (!(fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
				{
					g_espPresetFiles.push_back(fd.cFileName);
				}
			} while (FindNextFileA(hFind, &fd));
			FindClose(hFind);
		}

		if (g_espPresetFiles.empty())
		{
			char pPath[MAX_PATH];
			sprintf_s(pPath, "%s1_Taktikal_Bersih.ini", g_espPresetsDir);
			SaveEspPreset(pPath);
			WritePrivateProfileStringA("ESP", "EspLines", "0", pPath);
			WritePrivateProfileStringA("ESP", "EspCrosshair", "1", pPath);

			sprintf_s(pPath, "%s2_Kompetitif_Lengkap.ini", g_espPresetsDir);
			SaveEspPreset(pPath);
			WritePrivateProfileStringA("ESP", "EspLines", "1", pPath);
			WritePrivateProfileStringA("ESP", "EspEdge", "1", pPath);

			sprintf_s(pPath, "%s3_Minimalis_Crosshair.ini", g_espPresetsDir);
			SaveEspPreset(pPath);
			WritePrivateProfileStringA("ESP", "EspBox", "0", pPath);
			WritePrivateProfileStringA("ESP", "EspSkeleton", "0", pPath);
			WritePrivateProfileStringA("ESP", "EspLines", "0", pPath);
			WritePrivateProfileStringA("ESP", "EspCrosshair", "1", pPath);

			sprintf_s(pPath, "%s4_Full_Wallhack.ini", g_espPresetsDir);
			SaveEspPreset(pPath);
			WritePrivateProfileStringA("ESP", "EspBox", "1", pPath);
			WritePrivateProfileStringA("ESP", "EspBoxFill", "1", pPath);
			WritePrivateProfileStringA("ESP", "EspSkeleton", "1", pPath);
			WritePrivateProfileStringA("ESP", "EspSkeletonFront", "1", pPath);
			WritePrivateProfileStringA("ESP", "EspLines", "1", pPath);
			WritePrivateProfileStringA("ESP", "EspEdge", "1", pPath);

			ScanEspPresets();
			return;
		}

		if (g_espPresetIndex >= (int)g_espPresetFiles.size())
			g_espPresetIndex = 0;
	}

	void LoadConfig(const char* customPath = nullptr)
	{
		if (g_iniPath[0] == '\0')
			InitPaths();

		const char* targetFile = customPath ? customPath : g_iniPath;

		char buf[64];
		g_cfg.language = GetPrivateProfileIntA("SETTINGS", "Language", 0, targetFile);
		g_cfg.speedometerStyle = GetPrivateProfileIntA("SETTINGS", "SpeedometerStyle", 0, targetFile);
		if (GetPrivateProfileStringA("SETTINGS", "ForzaPosX", "0.885", buf, sizeof(buf), targetFile))
			g_cfg.forzaPosX = (float)atof(buf);
		if (GetPrivateProfileStringA("SETTINGS", "ForzaPosY", "0.825", buf, sizeof(buf), targetFile))
			g_cfg.forzaPosY = (float)atof(buf);
		if (GetPrivateProfileStringA("SETTINGS", "ForzaScale", "1.00", buf, sizeof(buf), targetFile))
			g_cfg.forzaScale = (float)atof(buf);

		if (GetPrivateProfileStringA("SETTINGS", "PosX", "0.016", buf, sizeof(buf), targetFile))
			g_cfg.posX = (float)atof(buf);
		if (GetPrivateProfileStringA("SETTINGS", "PosY", "0.740", buf, sizeof(buf), targetFile))
			g_cfg.posY = (float)atof(buf);
		if (GetPrivateProfileStringA("SETTINGS", "SpeedScale", "0.70", buf, sizeof(buf), targetFile))
			g_cfg.speedScale = (float)atof(buf);
		if (GetPrivateProfileStringA("SETTINGS", "UnitScale", "0.34", buf, sizeof(buf), targetFile))
			g_cfg.unitScale = (float)atof(buf);
		if (GetPrivateProfileStringA("SETTINGS", "GearScale", "0.68", buf, sizeof(buf), targetFile))
			g_cfg.gearScale = (float)atof(buf);

		if (GetPrivateProfileStringA("SETTINGS", "CenterPosX", "0.500", buf, sizeof(buf), targetFile))
			g_cfg.centerPosX = (float)atof(buf);
		if (GetPrivateProfileStringA("SETTINGS", "CenterPosY", "0.850", buf, sizeof(buf), targetFile))
			g_cfg.centerPosY = (float)atof(buf);
		if (GetPrivateProfileStringA("SETTINGS", "CenterSpeedScale", "1.10", buf, sizeof(buf), targetFile))
			g_cfg.centerSpeedScale = (float)atof(buf);

		g_cfg.autoHideAtZero = GetPrivateProfileIntA("SETTINGS", "AutoHideAtZero", 1, targetFile) != 0;
		g_cfg.centerSpeedThreshold = GetPrivateProfileIntA("SETTINGS", "CenterSpeedThreshold", 70, targetFile);
		if (g_cfg.centerSpeedThreshold < 0) g_cfg.centerSpeedThreshold = 0;
		if (g_cfg.centerSpeedThreshold > 180) g_cfg.centerSpeedThreshold = 180;

		g_cfg.centerCooldownSec = GetPrivateProfileIntA("SETTINGS", "CenterCooldownSec", 2, targetFile);
		if (g_cfg.centerCooldownSec < 1) g_cfg.centerCooldownSec = 1;
		if (g_cfg.centerCooldownSec > 15) g_cfg.centerCooldownSec = 15;

		g_cfg.textColorR = GetPrivateProfileIntA("SETTINGS", "TextColorR", 255, targetFile);
		g_cfg.textColorG = GetPrivateProfileIntA("SETTINGS", "TextColorG", 255, targetFile);
		g_cfg.textColorB = GetPrivateProfileIntA("SETTINGS", "TextColorB", 255, targetFile);

		g_cfg.outlineColorR = GetPrivateProfileIntA("SETTINGS", "OutlineColorR", 0, targetFile);
		g_cfg.outlineColorG = GetPrivateProfileIntA("SETTINGS", "OutlineColorG", 0, targetFile);
		g_cfg.outlineColorB = GetPrivateProfileIntA("SETTINGS", "OutlineColorB", 0, targetFile);

		g_cfg.textOpacity = GetPrivateProfileIntA("SETTINGS", "TextOpacity", 255, targetFile);
		g_cfg.speedIntervalMs = GetPrivateProfileIntA("SETTINGS", "SpeedIntervalMs", 230, targetFile);
		g_cfg.hudTextOutline = GetPrivateProfileIntA("SETTINGS", "HudTextOutline", 1, targetFile) != 0;
		g_cfg.hudTextOpacity = GetPrivateProfileIntA("SETTINGS", "HudTextOpacity", 255, targetFile);

		g_cfg.showSpeedo = GetPrivateProfileIntA("SETTINGS", "ShowSpeedo", 1, targetFile) != 0;
		g_cfg.showGear = GetPrivateProfileIntA("SETTINGS", "ShowGear", 1, targetFile) != 0;
		g_cfg.showVehicleName = GetPrivateProfileIntA("SETTINGS", "ShowVehicleName", 1, targetFile) != 0;
		g_cfg.showFps = GetPrivateProfileIntA("SETTINGS", "ShowFps", 1, targetFile) != 0;
		g_cfg.showClock = GetPrivateProfileIntA("SETTINGS", "ShowClock", 1, targetFile) != 0;
		g_cfg.showRealTime = GetPrivateProfileIntA("SETTINGS", "ShowRealTime", 1, targetFile) != 0;
		g_cfg.showRam = GetPrivateProfileIntA("SETTINGS", "ShowRam", 0, targetFile) != 0;
		g_cfg.showCpu = GetPrivateProfileIntA("SETTINGS", "ShowCpu", 0, targetFile) != 0;
		g_cfg.showGpu = GetPrivateProfileIntA("SETTINGS", "ShowGpu", 0, targetFile) != 0;
		g_cfg.showDisk = GetPrivateProfileIntA("SETTINGS", "ShowDisk", 0, targetFile) != 0;
		g_cfg.showFuel = GetPrivateProfileIntA("SETTINGS", "ShowFuel", 0, targetFile) != 0;
		g_cfg.showDamage = GetPrivateProfileIntA("SETTINGS", "ShowDamage", 0, targetFile) != 0;
		g_cfg.showAltitude = GetPrivateProfileIntA("SETTINGS", "ShowAltitude", 1, targetFile) != 0;
		g_cfg.showOnFootSpeed = GetPrivateProfileIntA("SETTINGS", "ShowOnFootSpeed", 1, targetFile) != 0;
		g_cfg.toggleKey = (DWORD)GetPrivateProfileIntA("SETTINGS", "ToggleKey", 123, targetFile);
		g_cfg.menuKey = (DWORD)GetPrivateProfileIntA("SETTINGS", "MenuKey", (int)g_cfg.toggleKey, targetFile);
		g_cfg.espToggleKey = (DWORD)GetPrivateProfileIntA("SETTINGS", "EspToggleKey", 'L', targetFile);   // 'L' (76)
		g_cfg.speedoToggleKey = (DWORD)GetPrivateProfileIntA("SETTINGS", "SpeedoToggleKey", 221, targetFile); // VK_OEM_6 ']' (221)

		// Muat Pengaturan ESP
		g_cfg.espMaster = GetPrivateProfileIntA("ESP", "EspMaster", 0, targetFile) != 0;
		g_cfg.espBox = GetPrivateProfileIntA("ESP", "EspBox", 1, targetFile) != 0;
		if (GetPrivateProfileStringA("ESP", "EspPedBoxScale", "1.00", buf, sizeof(buf), targetFile))
			g_cfg.espPedBoxScale = (float)atof(buf);
		if (GetPrivateProfileStringA("ESP", "EspVehBoxWidth", "1.35", buf, sizeof(buf), targetFile))
			g_cfg.espVehBoxWidth = (float)atof(buf);
		g_cfg.espSkeleton = GetPrivateProfileIntA("ESP", "EspSkeleton", 1, targetFile) != 0;
		g_cfg.espSkeletonQuality = GetPrivateProfileIntA("ESP", "EspSkeletonQuality", 0, targetFile);
		g_cfg.espSkeletonDist = GetPrivateProfileIntA("ESP", "EspSkeletonDist", 120, targetFile);
		g_cfg.espSkeletonFront = GetPrivateProfileIntA("ESP", "EspSkeletonFront", 1, targetFile) != 0;
		if (GetPrivateProfileStringA("ESP", "EspSkeletonScale", "1.00", buf, sizeof(buf), targetFile))
			g_cfg.espSkeletonScale = (float)atof(buf);
		g_cfg.espHeadDot = GetPrivateProfileIntA("ESP", "EspHeadDot", 1, targetFile) != 0;
		g_cfg.espAnimalHeadDot = GetPrivateProfileIntA("ESP", "EspAnimalHeadDot", 0, targetFile) != 0;
		g_cfg.espWheelDots = GetPrivateProfileIntA("ESP", "EspWheelDots", 1, targetFile) != 0;
		g_cfg.espShowSpeed = GetPrivateProfileIntA("ESP", "EspShowSpeed", 1, targetFile) != 0;
		g_cfg.espLines = GetPrivateProfileIntA("ESP", "EspLines", 1, targetFile) != 0;
		g_cfg.espLineOrigin = GetPrivateProfileIntA("ESP", "EspLineOrigin", 0, targetFile);
		g_cfg.espLineAntiClip = GetPrivateProfileIntA("ESP", "EspLineAntiClip", 1, targetFile) != 0;
		g_cfg.espLineThickness = GetPrivateProfileIntA("ESP", "EspLineThickness", 1, targetFile);
		g_cfg.espColorPreset = GetPrivateProfileIntA("ESP", "EspColorPreset", 0, targetFile);

		if (GetPrivateProfileStringA("ESP", "EspTextScale", "0.22", buf, sizeof(buf), targetFile))
			g_cfg.espTextScale = (float)atof(buf);
		if (GetPrivateProfileStringA("ESP", "EspCounterScale", "0.35", buf, sizeof(buf), targetFile))
			g_cfg.espCounterScale = (float)atof(buf);
		g_cfg.espOpacity = GetPrivateProfileIntA("ESP", "EspOpacity", 100, targetFile);
		g_cfg.espRectOpacity = GetPrivateProfileIntA("ESP", "EspRectOpacity", 240, targetFile);
		g_cfg.espTextOpacity = GetPrivateProfileIntA("ESP", "EspTextOpacity", 245, targetFile);

		g_cfg.espCrosshair = GetPrivateProfileIntA("ESP", "EspCrosshair", 0, targetFile) != 0;
		g_cfg.espCrosshairType = GetPrivateProfileIntA("ESP", "EspCrosshairType", 1, targetFile);
		if (GetPrivateProfileStringA("ESP", "EspCrosshairSize", "0.010", buf, sizeof(buf), targetFile))
			g_cfg.espCrosshairSize = (float)atof(buf);
		if (GetPrivateProfileStringA("ESP", "EspCrosshairThick", "0.002", buf, sizeof(buf), targetFile))
			g_cfg.espCrosshairThick = (float)atof(buf);
		g_cfg.espCrosshairColor = GetPrivateProfileIntA("ESP", "EspCrosshairColor", 0, targetFile);

		g_cfg.espAimCircle = GetPrivateProfileIntA("ESP", "EspAimCircle", 1, targetFile) != 0;
		if (GetPrivateProfileStringA("ESP", "EspAimCircleRectScale", "1.00", buf, sizeof(buf), targetFile))
			g_cfg.espAimCircleRectScale = (float)atof(buf);
		g_cfg.espAimAutoHide = GetPrivateProfileIntA("ESP", "EspAimAutoHide", 1, targetFile) != 0;
		g_cfg.espVelocitySmooth = GetPrivateProfileIntA("ESP", "EspVelocitySmooth", 1, targetFile);
		g_cfg.espTransitionMode = GetPrivateProfileIntA("ESP", "EspTransitionMode", 1, targetFile);
		if (GetPrivateProfileStringA("ESP", "EspAnimDuration", "0.25", buf, sizeof(buf), targetFile))
			g_cfg.espAnimDuration = (float)atof(buf);
		g_cfg.espFocus = GetPrivateProfileIntA("ESP", "EspFocus", 0, targetFile) != 0;
		if (GetPrivateProfileStringA("ESP", "EspFocusRadius", "0.45", buf, sizeof(buf), targetFile))
			g_cfg.espFocusRadius = (float)atof(buf);
		if (GetPrivateProfileStringA("ESP", "EspFocusWidth", "0.45", buf, sizeof(buf), targetFile))
			g_cfg.espFocusWidth = (float)atof(buf);
		if (GetPrivateProfileStringA("ESP", "EspFocusHeight", "0.40", buf, sizeof(buf), targetFile))
			g_cfg.espFocusHeight = (float)atof(buf);
		g_cfg.espKillSatisfying = GetPrivateProfileIntA("ESP", "EspKillSatisfying", 1, targetFile) != 0;
		if (GetPrivateProfileStringA("ESP", "EspKillBannerScale", "0.35", buf, sizeof(buf), targetFile))
			g_cfg.espKillBannerScale = (float)atof(buf);
		g_cfg.espKillSoundMode = GetPrivateProfileIntA("ESP", "EspKillSoundMode", 1, targetFile);

		g_cfg.espHealth = GetPrivateProfileIntA("ESP", "EspHealth", 1, targetFile) != 0;
		g_cfg.espVehHealth = GetPrivateProfileIntA("ESP", "EspVehHealth", 1, targetFile) != 0;
		g_cfg.espArmor = GetPrivateProfileIntA("ESP", "EspArmor", 1, targetFile) != 0;
		g_cfg.espDistance = GetPrivateProfileIntA("ESP", "EspDistance", 1, targetFile) != 0;
		g_cfg.espName = GetPrivateProfileIntA("ESP", "EspName", 1, targetFile) != 0;
		g_cfg.espShowWeapon = GetPrivateProfileIntA("ESP", "EspShowWeapon", 1, targetFile) != 0;
		g_cfg.espDamageText = GetPrivateProfileIntA("ESP", "EspDamageText", 1, targetFile) != 0;
		g_cfg.espEdge = GetPrivateProfileIntA("ESP", "EspEdge", 1, targetFile) != 0;
		g_cfg.espFriendly = GetPrivateProfileIntA("ESP", "EspFriendly", 1, targetFile) != 0;
		g_cfg.espEnemy = GetPrivateProfileIntA("ESP", "EspEnemy", 1, targetFile) != 0;
		g_cfg.espVehicles = GetPrivateProfileIntA("ESP", "EspVehicles", 1, targetFile) != 0;
		g_cfg.espAnimals = GetPrivateProfileIntA("ESP", "EspAnimals", 0, targetFile) != 0;
		g_cfg.espItems = GetPrivateProfileIntA("ESP", "EspItems", 1, targetFile) != 0;
		g_cfg.espFramerateMode = GetPrivateProfileIntA("ESP", "EspFramerateMode", 0, targetFile);
		g_cfg.espShowCounters = GetPrivateProfileIntA("ESP", "EspShowCounters", 1, targetFile) != 0;
		g_cfg.espIgnoreDead = GetPrivateProfileIntA("ESP", "EspIgnoreDead", 1, targetFile) != 0;
		g_cfg.espMaxDistance = GetPrivateProfileIntA("ESP", "EspMaxDistance", 150, targetFile);
		g_cfg.espHideOnWeaponWheel = GetPrivateProfileIntA("ESP", "EspHideOnWeaponWheel", 1, targetFile) != 0;
		if (GetPrivateProfileStringA("ESP", "EspAimCircleSize", "0.024", buf, sizeof(buf), targetFile))
			g_cfg.espAimCircleSize = (float)atof(buf);
		g_cfg.espBoxFill = GetPrivateProfileIntA("ESP", "EspBoxFill", 0, targetFile) != 0;
		g_cfg.espHeadColorMode = GetPrivateProfileIntA("ESP", "EspHeadColorMode", 0, targetFile);
		g_cfg.espTextPosition = GetPrivateProfileIntA("ESP", "EspTextPosition", 0, targetFile);
		g_cfg.espSnaplineDist = GetPrivateProfileIntA("ESP", "EspSnaplineDist", 200, targetFile);

		g_cfg.espCustomR = GetPrivateProfileIntA("ESP", "EspCustomR", 0, targetFile);
		g_cfg.espCustomG = GetPrivateProfileIntA("ESP", "EspCustomG", 215, targetFile);
		g_cfg.espCustomB = GetPrivateProfileIntA("ESP", "EspCustomB", 255, targetFile);

		// Muat Preferensi Menu
		if (GetPrivateProfileStringA("PREFERENCES", "MenuWidth", "0.280", buf, sizeof(buf), targetFile))
			g_cfg.menuWidth = (float)atof(buf);
		if (GetPrivateProfileStringA("PREFERENCES", "MenuItemHeight", "0.027", buf, sizeof(buf), targetFile))
			g_cfg.menuItemHeight = (float)atof(buf);
		if (GetPrivateProfileStringA("PREFERENCES", "MenuTextScale", "0.290", buf, sizeof(buf), targetFile))
			g_cfg.menuTextScale = (float)atof(buf);
		if (GetPrivateProfileStringA("PREFERENCES", "MenuHeaderScale", "0.500", buf, sizeof(buf), targetFile))
			g_cfg.menuHeaderScale = (float)atof(buf);
	}

	void SaveConfig(const char* customPath = nullptr)
	{
		if (g_iniPath[0] == '\0')
			InitPaths();

		const char* targetFile = customPath ? customPath : g_iniPath;

		char buf[64];
		sprintf_s(buf, "%d", g_cfg.language);
		WritePrivateProfileStringA("SETTINGS", "Language", buf, targetFile);
		sprintf_s(buf, "%d", g_cfg.speedometerStyle);
		WritePrivateProfileStringA("SETTINGS", "SpeedometerStyle", buf, targetFile);
		sprintf_s(buf, "%.4f", g_cfg.forzaPosX);
		WritePrivateProfileStringA("SETTINGS", "ForzaPosX", buf, targetFile);
		sprintf_s(buf, "%.4f", g_cfg.forzaPosY);
		WritePrivateProfileStringA("SETTINGS", "ForzaPosY", buf, targetFile);
		sprintf_s(buf, "%.2f", g_cfg.forzaScale);
		WritePrivateProfileStringA("SETTINGS", "ForzaScale", buf, targetFile);

		sprintf_s(buf, "%.4f", g_cfg.posX);
		WritePrivateProfileStringA("SETTINGS", "PosX", buf, targetFile);
		sprintf_s(buf, "%.4f", g_cfg.posY);
		WritePrivateProfileStringA("SETTINGS", "PosY", buf, targetFile);
		sprintf_s(buf, "%.2f", g_cfg.speedScale);
		WritePrivateProfileStringA("SETTINGS", "SpeedScale", buf, targetFile);
		sprintf_s(buf, "%.2f", g_cfg.unitScale);
		WritePrivateProfileStringA("SETTINGS", "UnitScale", buf, targetFile);
		sprintf_s(buf, "%.2f", g_cfg.gearScale);
		WritePrivateProfileStringA("SETTINGS", "GearScale", buf, targetFile);

		sprintf_s(buf, "%.4f", g_cfg.centerPosX);
		WritePrivateProfileStringA("SETTINGS", "CenterPosX", buf, targetFile);
		sprintf_s(buf, "%.4f", g_cfg.centerPosY);
		WritePrivateProfileStringA("SETTINGS", "CenterPosY", buf, targetFile);
		sprintf_s(buf, "%.2f", g_cfg.centerSpeedScale);
		WritePrivateProfileStringA("SETTINGS", "CenterSpeedScale", buf, targetFile);

		WritePrivateProfileStringA("SETTINGS", "AutoHideAtZero", g_cfg.autoHideAtZero ? "1" : "0", targetFile);
		sprintf_s(buf, "%d", g_cfg.centerSpeedThreshold);
		WritePrivateProfileStringA("SETTINGS", "CenterSpeedThreshold", buf, targetFile);
		sprintf_s(buf, "%d", g_cfg.centerCooldownSec);
		WritePrivateProfileStringA("SETTINGS", "CenterCooldownSec", buf, targetFile);

		sprintf_s(buf, "%d", g_cfg.textColorR);
		WritePrivateProfileStringA("SETTINGS", "TextColorR", buf, targetFile);
		sprintf_s(buf, "%d", g_cfg.textColorG);
		WritePrivateProfileStringA("SETTINGS", "TextColorG", buf, targetFile);
		sprintf_s(buf, "%d", g_cfg.textColorB);
		WritePrivateProfileStringA("SETTINGS", "TextColorB", buf, targetFile);

		sprintf_s(buf, "%d", g_cfg.outlineColorR);
		WritePrivateProfileStringA("SETTINGS", "OutlineColorR", buf, targetFile);
		sprintf_s(buf, "%d", g_cfg.outlineColorG);
		WritePrivateProfileStringA("SETTINGS", "OutlineColorG", buf, targetFile);
		sprintf_s(buf, "%d", g_cfg.outlineColorB);
		WritePrivateProfileStringA("SETTINGS", "OutlineColorB", buf, targetFile);

		sprintf_s(buf, "%d", g_cfg.textOpacity);
		WritePrivateProfileStringA("SETTINGS", "TextOpacity", buf, targetFile);
		sprintf_s(buf, "%d", g_cfg.speedIntervalMs);
		WritePrivateProfileStringA("SETTINGS", "SpeedIntervalMs", buf, targetFile);
		WritePrivateProfileStringA("SETTINGS", "HudTextOutline", g_cfg.hudTextOutline ? "1" : "0", targetFile);
		sprintf_s(buf, "%d", g_cfg.hudTextOpacity);
		WritePrivateProfileStringA("SETTINGS", "HudTextOpacity", buf, targetFile);

		WritePrivateProfileStringA("SETTINGS", "ShowSpeedo", g_cfg.showSpeedo ? "1" : "0", targetFile);
		WritePrivateProfileStringA("SETTINGS", "ShowGear", g_cfg.showGear ? "1" : "0", targetFile);
		WritePrivateProfileStringA("SETTINGS", "ShowVehicleName", g_cfg.showVehicleName ? "1" : "0", targetFile);
		WritePrivateProfileStringA("SETTINGS", "ShowFps", g_cfg.showFps ? "1" : "0", targetFile);
		WritePrivateProfileStringA("SETTINGS", "ShowClock", g_cfg.showClock ? "1" : "0", targetFile);
		WritePrivateProfileStringA("SETTINGS", "ShowRealTime", g_cfg.showRealTime ? "1" : "0", targetFile);
		WritePrivateProfileStringA("SETTINGS", "ShowRam", g_cfg.showRam ? "1" : "0", targetFile);
		WritePrivateProfileStringA("SETTINGS", "ShowCpu", g_cfg.showCpu ? "1" : "0", targetFile);
		WritePrivateProfileStringA("SETTINGS", "ShowGpu", g_cfg.showGpu ? "1" : "0", targetFile);
		WritePrivateProfileStringA("SETTINGS", "ShowDisk", g_cfg.showDisk ? "1" : "0", targetFile);
		WritePrivateProfileStringA("SETTINGS", "ShowFuel", g_cfg.showFuel ? "1" : "0", targetFile);
		WritePrivateProfileStringA("SETTINGS", "ShowDamage", g_cfg.showDamage ? "1" : "0", targetFile);
		WritePrivateProfileStringA("SETTINGS", "ShowAltitude", g_cfg.showAltitude ? "1" : "0", targetFile);
		WritePrivateProfileStringA("SETTINGS", "ShowOnFootSpeed", g_cfg.showOnFootSpeed ? "1" : "0", targetFile);

		sprintf_s(buf, "%d", (int)g_cfg.menuKey);
		WritePrivateProfileStringA("SETTINGS", "MenuKey", buf, targetFile);
		sprintf_s(buf, "%d", (int)g_cfg.espToggleKey);
		WritePrivateProfileStringA("SETTINGS", "EspToggleKey", buf, targetFile);
		sprintf_s(buf, "%d", (int)g_cfg.speedoToggleKey);
		WritePrivateProfileStringA("SETTINGS", "SpeedoToggleKey", buf, targetFile);
		sprintf_s(buf, "%d", (int)g_cfg.toggleKey);
		WritePrivateProfileStringA("SETTINGS", "ToggleKey", buf, targetFile);

		// Simpan Pengaturan ESP
		WritePrivateProfileStringA("ESP", "EspMaster", g_cfg.espMaster ? "1" : "0", targetFile);
		WritePrivateProfileStringA("ESP", "EspBox", g_cfg.espBox ? "1" : "0", targetFile);
		sprintf_s(buf, "%.2f", g_cfg.espPedBoxScale);
		WritePrivateProfileStringA("ESP", "EspPedBoxScale", buf, targetFile);
		sprintf_s(buf, "%.2f", g_cfg.espVehBoxWidth);
		WritePrivateProfileStringA("ESP", "EspVehBoxWidth", buf, targetFile);
		WritePrivateProfileStringA("ESP", "EspSkeleton", g_cfg.espSkeleton ? "1" : "0", targetFile);
		sprintf_s(buf, "%d", g_cfg.espSkeletonQuality);
		WritePrivateProfileStringA("ESP", "EspSkeletonQuality", buf, targetFile);
		sprintf_s(buf, "%d", g_cfg.espSkeletonDist);
		WritePrivateProfileStringA("ESP", "EspSkeletonDist", buf, targetFile);
		WritePrivateProfileStringA("ESP", "EspSkeletonFront", g_cfg.espSkeletonFront ? "1" : "0", targetFile);
		sprintf_s(buf, "%.2f", g_cfg.espSkeletonScale);
		WritePrivateProfileStringA("ESP", "EspSkeletonScale", buf, targetFile);
		WritePrivateProfileStringA("ESP", "EspHeadDot", g_cfg.espHeadDot ? "1" : "0", targetFile);
		WritePrivateProfileStringA("ESP", "EspAnimalHeadDot", g_cfg.espAnimalHeadDot ? "1" : "0", targetFile);
		WritePrivateProfileStringA("ESP", "EspWheelDots", g_cfg.espWheelDots ? "1" : "0", targetFile);
		WritePrivateProfileStringA("ESP", "EspShowSpeed", g_cfg.espShowSpeed ? "1" : "0", targetFile);
		WritePrivateProfileStringA("ESP", "EspLines", g_cfg.espLines ? "1" : "0", targetFile);
		sprintf_s(buf, "%d", g_cfg.espLineOrigin);
		WritePrivateProfileStringA("ESP", "EspLineOrigin", buf, targetFile);
		WritePrivateProfileStringA("ESP", "EspLineAntiClip", g_cfg.espLineAntiClip ? "1" : "0", targetFile);
		sprintf_s(buf, "%d", g_cfg.espLineThickness);
		WritePrivateProfileStringA("ESP", "EspLineThickness", buf, targetFile);
		sprintf_s(buf, "%d", g_cfg.espColorPreset);
		WritePrivateProfileStringA("ESP", "EspColorPreset", buf, targetFile);
		sprintf_s(buf, "%.2f", g_cfg.espTextScale);
		WritePrivateProfileStringA("ESP", "EspTextScale", buf, targetFile);
		sprintf_s(buf, "%.2f", g_cfg.espCounterScale);
		WritePrivateProfileStringA("ESP", "EspCounterScale", buf, targetFile);
		sprintf_s(buf, "%d", g_cfg.espOpacity);
		WritePrivateProfileStringA("ESP", "EspOpacity", buf, targetFile);
		sprintf_s(buf, "%d", g_cfg.espRectOpacity);
		WritePrivateProfileStringA("ESP", "EspRectOpacity", buf, targetFile);
		sprintf_s(buf, "%d", g_cfg.espTextOpacity);
		WritePrivateProfileStringA("ESP", "EspTextOpacity", buf, targetFile);

		WritePrivateProfileStringA("ESP", "EspCrosshair", g_cfg.espCrosshair ? "1" : "0", targetFile);
		sprintf_s(buf, "%d", g_cfg.espCrosshairType);
		WritePrivateProfileStringA("ESP", "EspCrosshairType", buf, targetFile);
		sprintf_s(buf, "%.4f", g_cfg.espCrosshairSize);
		WritePrivateProfileStringA("ESP", "EspCrosshairSize", buf, targetFile);
		sprintf_s(buf, "%.4f", g_cfg.espCrosshairThick);
		WritePrivateProfileStringA("ESP", "EspCrosshairThick", buf, targetFile);
		sprintf_s(buf, "%d", g_cfg.espCrosshairColor);
		WritePrivateProfileStringA("ESP", "EspCrosshairColor", buf, targetFile);

		WritePrivateProfileStringA("ESP", "EspAimCircle", g_cfg.espAimCircle ? "1" : "0", targetFile);
		sprintf_s(buf, "%.2f", g_cfg.espAimCircleRectScale);
		WritePrivateProfileStringA("ESP", "EspAimCircleRectScale", buf, targetFile);
		WritePrivateProfileStringA("ESP", "EspAimAutoHide", g_cfg.espAimAutoHide ? "1" : "0", targetFile);
		sprintf_s(buf, "%d", g_cfg.espVelocitySmooth);
		WritePrivateProfileStringA("ESP", "EspVelocitySmooth", buf, targetFile);
		sprintf_s(buf, "%d", g_cfg.espTransitionMode);
		WritePrivateProfileStringA("ESP", "EspTransitionMode", buf, targetFile);
		sprintf_s(buf, "%.2f", g_cfg.espAnimDuration);
		WritePrivateProfileStringA("ESP", "EspAnimDuration", buf, targetFile);
		WritePrivateProfileStringA("ESP", "EspFocus", g_cfg.espFocus ? "1" : "0", targetFile);
		sprintf_s(buf, "%.2f", g_cfg.espFocusRadius);
		WritePrivateProfileStringA("ESP", "EspFocusRadius", buf, targetFile);
		sprintf_s(buf, "%.2f", g_cfg.espFocusWidth);
		WritePrivateProfileStringA("ESP", "EspFocusWidth", buf, targetFile);
		sprintf_s(buf, "%.2f", g_cfg.espFocusHeight);
		WritePrivateProfileStringA("ESP", "EspFocusHeight", buf, targetFile);
		WritePrivateProfileStringA("ESP", "EspKillSatisfying", g_cfg.espKillSatisfying ? "1" : "0", targetFile);
		sprintf_s(buf, "%.2f", g_cfg.espKillBannerScale);
		WritePrivateProfileStringA("ESP", "EspKillBannerScale", buf, targetFile);
		sprintf_s(buf, "%d", g_cfg.espKillSoundMode);
		WritePrivateProfileStringA("ESP", "EspKillSoundMode", buf, targetFile);

		WritePrivateProfileStringA("ESP", "EspHealth", g_cfg.espHealth ? "1" : "0", targetFile);
		WritePrivateProfileStringA("ESP", "EspVehHealth", g_cfg.espVehHealth ? "1" : "0", targetFile);
		WritePrivateProfileStringA("ESP", "EspArmor", g_cfg.espArmor ? "1" : "0", targetFile);
		WritePrivateProfileStringA("ESP", "EspDistance", g_cfg.espDistance ? "1" : "0", targetFile);
		WritePrivateProfileStringA("ESP", "EspName", g_cfg.espName ? "1" : "0", targetFile);
		WritePrivateProfileStringA("ESP", "EspShowWeapon", g_cfg.espShowWeapon ? "1" : "0", targetFile);
		WritePrivateProfileStringA("ESP", "EspDamageText", g_cfg.espDamageText ? "1" : "0", targetFile);
		WritePrivateProfileStringA("ESP", "EspEdge", g_cfg.espEdge ? "1" : "0", targetFile);
		WritePrivateProfileStringA("ESP", "EspFriendly", g_cfg.espFriendly ? "1" : "0", targetFile);
		WritePrivateProfileStringA("ESP", "EspEnemy", g_cfg.espEnemy ? "1" : "0", targetFile);
		WritePrivateProfileStringA("ESP", "EspVehicles", g_cfg.espVehicles ? "1" : "0", targetFile);
		WritePrivateProfileStringA("ESP", "EspAnimals", g_cfg.espAnimals ? "1" : "0", targetFile);
		WritePrivateProfileStringA("ESP", "EspItems", g_cfg.espItems ? "1" : "0", targetFile);
		sprintf_s(buf, "%d", g_cfg.espFramerateMode);
		WritePrivateProfileStringA("ESP", "EspFramerateMode", buf, targetFile);
		WritePrivateProfileStringA("ESP", "EspShowCounters", g_cfg.espShowCounters ? "1" : "0", targetFile);
		WritePrivateProfileStringA("ESP", "EspIgnoreDead", g_cfg.espIgnoreDead ? "1" : "0", targetFile);
		sprintf_s(buf, "%d", g_cfg.espMaxDistance);
		WritePrivateProfileStringA("ESP", "EspMaxDistance", buf, targetFile);
		WritePrivateProfileStringA("ESP", "EspHideOnWeaponWheel", g_cfg.espHideOnWeaponWheel ? "1" : "0", targetFile);
		sprintf_s(buf, "%.4f", g_cfg.espAimCircleSize);
		WritePrivateProfileStringA("ESP", "EspAimCircleSize", buf, targetFile);
		WritePrivateProfileStringA("ESP", "EspBoxFill", g_cfg.espBoxFill ? "1" : "0", targetFile);
		sprintf_s(buf, "%d", g_cfg.espHeadColorMode);
		WritePrivateProfileStringA("ESP", "EspHeadColorMode", buf, targetFile);
		sprintf_s(buf, "%d", g_cfg.espTextPosition);
		WritePrivateProfileStringA("ESP", "EspTextPosition", buf, targetFile);
		sprintf_s(buf, "%d", g_cfg.espSnaplineDist);
		WritePrivateProfileStringA("ESP", "EspSnaplineDist", buf, targetFile);

		sprintf_s(buf, "%d", g_cfg.espCustomR);
		WritePrivateProfileStringA("ESP", "EspCustomR", buf, targetFile);
		sprintf_s(buf, "%d", g_cfg.espCustomG);
		WritePrivateProfileStringA("ESP", "EspCustomG", buf, targetFile);
		sprintf_s(buf, "%d", g_cfg.espCustomB);
		WritePrivateProfileStringA("ESP", "EspCustomB", buf, targetFile);

		// Simpan Preferensi Menu
		sprintf_s(buf, "%.4f", g_cfg.menuWidth);
		WritePrivateProfileStringA("PREFERENCES", "MenuWidth", buf, targetFile);
		sprintf_s(buf, "%.4f", g_cfg.menuItemHeight);
		WritePrivateProfileStringA("PREFERENCES", "MenuItemHeight", buf, targetFile);
		sprintf_s(buf, "%.4f", g_cfg.menuTextScale);
		WritePrivateProfileStringA("PREFERENCES", "MenuTextScale", buf, targetFile);
		sprintf_s(buf, "%.4f", g_cfg.menuHeaderScale);
		WritePrivateProfileStringA("PREFERENCES", "MenuHeaderScale", buf, targetFile);
	}

	void SaveEspPreset(const char* fullPath)
	{
		SaveConfig(fullPath);
	}

	void LoadEspPreset(const char* fullPath)
	{
		LoadConfig(fullPath);
	}

	void ResetDefaults()
	{
		g_cfg.language = 0;
		g_cfg.speedometerStyle = 0;
		g_cfg.forzaPosX = 0.890f;
		g_cfg.forzaPosY = 0.825f;
		g_cfg.forzaScale = 1.00f;

		g_cfg.posX = 0.016f;
		g_cfg.posY = 0.740f;
		g_cfg.speedScale = 0.70f;
		g_cfg.unitScale = 0.34f;
		g_cfg.gearScale = 0.68f;

		g_cfg.centerPosX = 0.500f;
		g_cfg.centerPosY = 0.850f;
		g_cfg.centerSpeedScale = 1.10f;

		g_cfg.autoHideAtZero = false;
		g_cfg.centerSpeedThreshold = 70;
		g_cfg.centerCooldownSec = 2;

		g_cfg.textColorR = 255;
		g_cfg.textColorG = 255;
		g_cfg.textColorB = 255;
		g_cfg.outlineColorR = 0;
		g_cfg.outlineColorG = 0;
		g_cfg.outlineColorB = 0;
		g_cfg.textOpacity = 255;
		g_cfg.speedIntervalMs = 230;
		g_cfg.hudTextOutline = true;
		g_cfg.hudTextOpacity = 255;

		g_cfg.showSpeedo = true;
		g_cfg.showGear = true;
		g_cfg.showVehicleName = true;
		g_cfg.showFps = true;
		g_cfg.showClock = true;
		g_cfg.showRealTime = true;
		g_cfg.showRam = false;
		g_cfg.showCpu = true;
		g_cfg.showGpu = true;
		g_cfg.showDisk = false;
		g_cfg.showFuel = true;
		g_cfg.showDamage = true;
		g_cfg.showAltitude = true;
		g_cfg.showOnFootSpeed = true;

		g_cfg.espMaster = false;
		g_cfg.espBox = true;
		g_cfg.espPedBoxScale = 1.00f;
		g_cfg.espVehBoxWidth = 1.35f;
		g_cfg.espSkeleton = true;
		g_cfg.espSkeletonQuality = 0;
		g_cfg.espSkeletonDist = 140;
		g_cfg.espSkeletonFront = true;
		g_cfg.espSkeletonScale = 1.30f;
		g_cfg.espHeadDot = true;
		g_cfg.espAnimalHeadDot = false;
		g_cfg.espWheelDots = false;
		g_cfg.espShowSpeed = true;
		g_cfg.espLines = true;
		g_cfg.espLineOrigin = 0;
		g_cfg.espLineAntiClip = false;
		g_cfg.espLineThickness = 1;
		g_cfg.espColorPreset = 0;
		g_cfg.espTextScale = 0.30f;
		g_cfg.espCounterScale = 0.34f;
		g_cfg.espOpacity = 100;
		g_cfg.espRectOpacity = 240;
		g_cfg.espTextOpacity = 245;

		g_cfg.espCrosshair = true;
		g_cfg.espCrosshairType = 0;
		g_cfg.espCrosshairSize = 0.0080f;
		g_cfg.espCrosshairThick = 0.0020f;
		g_cfg.espCrosshairColor = 2;

		g_cfg.espAimCircle = true;
		g_cfg.espAimCircleRectScale = 1.00f;
		g_cfg.espAimAutoHide = true;
		g_cfg.espVelocitySmooth = 1;
		g_cfg.espTransitionMode = 1;
		g_cfg.espAnimDuration = 0.25f;
		g_cfg.espFocus = false;
		g_cfg.espFocusRadius = 0.45f;
		g_cfg.espFocusWidth = 0.45f;
		g_cfg.espFocusHeight = 0.40f;
		g_cfg.espKillSatisfying = true;
		g_cfg.espKillBannerScale = 0.31f;
		g_cfg.espKillSoundMode = 1;

		g_cfg.espHealth = true;
		g_cfg.espVehHealth = true;
		g_cfg.espArmor = false;
		g_cfg.espDistance = true;
		g_cfg.espName = false;
		g_cfg.espShowWeapon = true;
		g_cfg.espDamageText = true;
		g_cfg.espEdge = true;
		g_cfg.espFriendly = true;
		g_cfg.espEnemy = true;
		g_cfg.espVehicles = true;
		g_cfg.espAnimals = true;
		g_cfg.espItems = true;
		g_cfg.espFramerateMode = 1;
		g_cfg.espShowCounters = true;
		g_cfg.espIgnoreDead = true;
		g_cfg.espMaxDistance = 150;
		g_cfg.espHideOnWeaponWheel = true;
		g_cfg.espAimCircleSize = 0.0260f;
		g_cfg.espBoxFill = false;
		g_cfg.espHeadColorMode = 0;
		g_cfg.espTextPosition = 0;
		g_cfg.espSnaplineDist = 150;

		g_cfg.espCustomR = 0;
		g_cfg.espCustomG = 215;
		g_cfg.espCustomB = 255;

		g_cfg.menuWidth = 0.2900f;
		g_cfg.menuItemHeight = 0.0380f;
		g_cfg.menuTextScale = 0.4300f;
		g_cfg.menuHeaderScale = 0.500f;

		g_cfg.menuKey = VK_F12;
		g_cfg.espToggleKey = 'L';
		g_cfg.speedoToggleKey = VK_OEM_6;
		g_cfg.toggleKey = VK_F12;

		SaveConfig();
	}

	float GetTextWidth(const char* text, float scale, int font = 4)
	{
		UI::SET_TEXT_FONT(font);
		UI::SET_TEXT_SCALE(scale, scale);
		UI::_SET_TEXT_ENTRY_FOR_WIDTH((char*)"STRING");
		UI::_ADD_TEXT_COMPONENT_STRING((char*)text);
		return UI::_GET_TEXT_SCREEN_WIDTH(TRUE);
	}

	void DrawTextElement(const char* text, float x, float y, float scale, int font,
	                     int r, int g, int b, int a, bool outline = true)
	{
		if (a <= 0) return;

		UI::SET_TEXT_FONT(font);
		UI::SET_TEXT_SCALE(scale, scale);
		UI::SET_TEXT_COLOUR(r, g, b, a);
		if (outline)
		{
			UI::SET_TEXT_DROP_SHADOW();
			UI::SET_TEXT_DROPSHADOW(2, g_cfg.outlineColorR, g_cfg.outlineColorG, g_cfg.outlineColorB, (int)(a * 0.90f));
			UI::SET_TEXT_OUTLINE();
			UI::SET_TEXT_EDGE(1, g_cfg.outlineColorR, g_cfg.outlineColorG, g_cfg.outlineColorB, (int)(a * 0.85f));
		}
		UI::SET_TEXT_CENTRE(FALSE);
		UI::_SET_TEXT_ENTRY((char*)"STRING");
		UI::_ADD_TEXT_COMPONENT_STRING((char*)text);
		UI::_DRAW_TEXT(x, y);
	}

	void DrawMenuText(const char* text, float x, float y, float scale, int font,
	                  int r, int g, int b, int a, bool outline = true)
	{
		UI::SET_TEXT_FONT(font);
		UI::SET_TEXT_SCALE(scale, scale);
		UI::SET_TEXT_COLOUR(r, g, b, a);
		if (outline) UI::SET_TEXT_OUTLINE();
		UI::SET_TEXT_CENTRE(FALSE);
		UI::_SET_TEXT_ENTRY((char*)"STRING");
		UI::_ADD_TEXT_COMPONENT_STRING((char*)text);
		UI::_DRAW_TEXT(x, y);
	}

	void SetStatusMessage(const std::string& msg)
	{
		g_statusMsg = msg;
		g_statusMsgTime = GetTickCount64();
	}

	// -------------------------------------------------------------
	// PEMBARUAN DATA HARDWARE SYSTEM MONITOR (CPU, GPU, RAM, DISK)
	// -------------------------------------------------------------
	void UpdateHardwareStats(ULONGLONG currentTick)
	{
		if (currentTick - g_lastHardwareSampleTick < 1000) return;
		g_lastHardwareSampleTick = currentTick;

		// 1. CPU Usage (%)
		FILETIME idleTime, kernelTime, userTime;
		if (GetSystemTimes(&idleTime, &kernelTime, &userTime))
		{
			ULARGE_INTEGER uIdle, uKernel, uUser;
			uIdle.LowPart = idleTime.dwLowDateTime; uIdle.HighPart = idleTime.dwHighDateTime;
			uKernel.LowPart = kernelTime.dwLowDateTime; uKernel.HighPart = kernelTime.dwHighDateTime;
			uUser.LowPart = userTime.dwLowDateTime; uUser.HighPart = userTime.dwHighDateTime;

			if (g_prevKernel.QuadPart > 0)
			{
				ULONGLONG idleDiff = uIdle.QuadPart - g_prevIdle.QuadPart;
				ULONGLONG kernelDiff = uKernel.QuadPart - g_prevKernel.QuadPart;
				ULONGLONG userDiff = uUser.QuadPart - g_prevUser.QuadPart;
				ULONGLONG total = kernelDiff + userDiff;
				if (total > 0 && total >= idleDiff)
				{
					g_cpuPercent = (float)(total - idleDiff) * 100.0f / (float)total;
					g_cpuPercent = std::clamp(g_cpuPercent, 0.0f, 100.0f);
				}
			}
			g_prevIdle = uIdle;
			g_prevKernel = uKernel;
			g_prevUser = uUser;
		}

		// 2. RAM Usage & %
		MEMORYSTATUSEX mem;
		mem.dwLength = sizeof(MEMORYSTATUSEX);
		if (GlobalMemoryStatusEx(&mem))
		{
			g_ramPercent = (int)mem.dwMemoryLoad;
			g_ramUsedGB = (float)(mem.ullTotalPhys - mem.ullAvailPhys) / (1024.0f * 1024.0f * 1024.0f);
			g_ramTotalGB = (float)mem.ullTotalPhys / (1024.0f * 1024.0f * 1024.0f);
		}

		// 3. Disk Usage & %
		char driveRoot[4] = "C:\\";
		if (isalpha(g_iniPath[0]) && g_iniPath[1] == ':') driveRoot[0] = g_iniPath[0];
		ULARGE_INTEGER freeBytes, totalBytes, totalFree;
		if (GetDiskFreeSpaceExA(driveRoot, &freeBytes, &totalBytes, &totalFree))
		{
			float totGB = (float)totalBytes.QuadPart / (1024.0f * 1024.0f * 1024.0f);
			float freeGB = (float)freeBytes.QuadPart / (1024.0f * 1024.0f * 1024.0f);
			if (totGB > 0.0f)
			{
				g_diskPercent = (int)(((totGB - freeGB) / totGB) * 100.0f);
				g_diskUsedGB = totGB - freeGB;
				g_diskTotalGB = totGB;
			}
		}

		// 4. GPU VRAM & Utilization (DXGI)
		HMODULE hDxgi = GetModuleHandleA("dxgi.dll");
		if (hDxgi)
		{
			typedef HRESULT (WINAPI *PFN_CreateDXGIFactory1)(REFIID riid, void **ppFactory);
			PFN_CreateDXGIFactory1 pfnCreate = (PFN_CreateDXGIFactory1)GetProcAddress(hDxgi, "CreateDXGIFactory1");
			if (pfnCreate)
			{
				IDXGIFactory1* pFactory = NULL;
				if (SUCCEEDED(pfnCreate(__uuidof(IDXGIFactory1), (void**)&pFactory)) && pFactory)
				{
					IDXGIAdapter* pAdapter = NULL;
					if (SUCCEEDED(pFactory->EnumAdapters(0, &pAdapter)) && pAdapter)
					{
						IDXGIAdapter3* pAdapter3 = NULL;
						if (SUCCEEDED(pAdapter->QueryInterface(__uuidof(IDXGIAdapter3), (void**)&pAdapter3)) && pAdapter3)
						{
							DXGI_QUERY_VIDEO_MEMORY_INFO vMem = {};
							if (SUCCEEDED(pAdapter3->QueryVideoMemoryInfo(0, DXGI_MEMORY_SEGMENT_GROUP_LOCAL, &vMem)))
							{
								g_gpuVramUsedMB = (int)(vMem.CurrentUsage / (1024 * 1024));
								g_gpuVramTotalMB = (int)(vMem.Budget / (1024 * 1024));
								if (g_gpuVramTotalMB > 0)
								{
									g_gpuVramPercent = (float)g_gpuVramUsedMB * 100.0f / (float)g_gpuVramTotalMB;
									g_gpuCorePercent = g_gpuVramPercent;
								}
							}
							pAdapter3->Release();
						}
						pAdapter->Release();
					}
					pFactory->Release();
				}
			}
		}
	}

	// -------------------------------------------------------------
	// FAKTOR OPASITAS ESP TERPISAH (RECTANGLE & TEKS)
	// -------------------------------------------------------------
	inline int ApplyEspRectAlpha(int baseAlpha)
	{
		float mult = std::clamp((float)g_cfg.espRectOpacity / 255.0f, 0.0f, 1.0f);
		int a = (int)((float)baseAlpha * mult);
		return std::clamp(a, 0, 255);
	}

	inline int ApplyEspTextAlpha(int baseAlpha)
	{
		float mult = std::clamp((float)g_cfg.espTextOpacity / 255.0f, 0.0f, 1.0f);
		int a = (int)((float)baseAlpha * mult);
		return std::clamp(a, 0, 255);
	}

	inline int ApplyEspAlpha(int baseAlpha)
	{
		return ApplyEspRectAlpha(baseAlpha);
	}

	// -------------------------------------------------------------
	// PELACAK JUMLAH RECTANGLE 2D (GTA V UI BUFFER LIMIT TRACKER)
	// Kapasitas buffer game ~256-512 rect, batas aman skrip ~300 rect/frame
	// -------------------------------------------------------------
	int g_renderedRectCount = 0;

	inline void DrawEspRect(float x, float y, float width, float height, int r, int g, int b, int a)
	{
		if (g_renderedRectCount < 285)
		{
			GRAPHICS::DRAW_RECT(x, y, width, height, r, g, b, a);
		}
		g_renderedRectCount++;
	}

	// -------------------------------------------------------------
	// 2D SCREEN LINE DRAWING (ULTRA BUFFER-EFFICIENT, SOLID OVERLAP, TEMBUS PANDANG)
	// Hanya memakai 1 rect (pendek/sedang) atau 2 rect (panjang) per ruas garis!
	// Menghemat >65% rectangle queue dibandingkan multi-step sebelumnya.
	// -------------------------------------------------------------
	void DrawScreenLine(float x1, float y1, float x2, float y2, float thickness, int r, int g, int b, int a, float scaleMul = 1.0f)
	{
		float dx = x2 - x1;
		float dy = y2 - y1;
		float dist = std::sqrt(dx * dx + dy * dy);
		if (dist < 0.0003f) return;

		float aspect = GRAPHICS::_GET_SCREEN_ASPECT_RATIO(FALSE);
		if (aspect <= 0.0f) aspect = 1.7777778f;

		int effA = ApplyEspAlpha(a);

		// Brush rectangle ramping, rapi, dan kecil sesuai skala skeleton
		float brushW = std::clamp(thickness * 0.70f * scaleMul, 0.00045f, 0.0022f * scaleMul);
		float brushH = brushW * aspect;

		// Segmen pendek (dist < 0.0140f layar): Cukup 1 rect kecil ramping di tengah garis
		if (dist < 0.0140f)
		{
			DrawEspRect((x1 + x2) * 0.5f, (y1 + y2) * 0.5f, brushW, brushH, r, g, b, effA);
			return;
		}

		// Segmen sedang / panjang: Cukup 2 rect ramping presisi di 1/3 dan 2/3
		DrawEspRect(x1 + dx * 0.33f, y1 + dy * 0.33f, brushW, brushH, r, g, b, effA);
		DrawEspRect(x1 + dx * 0.67f, y1 + dy * 0.67f, brushW, brushH, r, g, b, effA);
	}

	// -------------------------------------------------------------
	// RESOLUSI WARNA DARI PRESET ESP
	// -------------------------------------------------------------
	void GetEspColor(int targetType, bool isEnemy, int& r, int& g, int& b)
	{
		switch (g_cfg.espColorPreset)
		{
		case 1: // Cyberpunk (Neon Cyan & Hot Pink)
			if (targetType == 0) { if (isEnemy) { r = 255; g = 20; b = 147; } else { r = 0; g = 255; b = 230; } }
			else if (targetType == 1) { r = 255; g = 230; b = 0; }
			else if (targetType == 2) { r = 180; g = 0; b = 255; }
			else { r = 0; g = 255; b = 140; }
			break;
		case 2: // Tactical (Military Lime & Blaze Orange)
			if (targetType == 0) { if (isEnemy) { r = 255; g = 110; b = 0; } else { r = 120; g = 225; b = 40; } }
			else if (targetType == 1) { r = 235; g = 185; b = 45; }
			else if (targetType == 2) { r = 210; g = 175; b = 110; }
			else { r = 90; g = 200; b = 240; }
			break;
		case 3: // Monochrome (Clean White & Crimson)
			if (targetType == 0) { if (isEnemy) { r = 255; g = 50; b = 50; } else { r = 240; g = 240; b = 240; } }
			else if (targetType == 1) { r = 210; g = 225; b = 240; }
			else if (targetType == 2) { r = 175; g = 175; b = 175; }
			else { r = 255; g = 255; b = 255; }
			break;
		case 4: // Vaporwave (Electric Purple & Sky Cyan)
			if (targetType == 0) { if (isEnemy) { r = 255; g = 80; b = 160; } else { r = 130; g = 130; b = 255; } }
			else if (targetType == 1) { r = 0; g = 220; b = 255; }
			else if (targetType == 2) { r = 220; g = 120; b = 255; }
			else { r = 255; g = 210; b = 80; }
			break;
		case 5: // Kustom RGB
			if (isEnemy) { r = 255; g = 45; b = 45; }
			else { r = g_cfg.espCustomR; g = g_cfg.espCustomG; b = g_cfg.espCustomB; }
			break;
		default: // 0: Default (Hijau, Pink Musuh, Emas, Ungu, Cyan)
			if (targetType == 0) { if (isEnemy) { r = 255; g = 50; b = 180; /* Pink Musuh */ } else { r = 45; g = 230; b = 110; } }
			else if (targetType == 1) { r = 255; g = 185; b = 30; }
			else if (targetType == 2) { r = 215; g = 95; b = 255; }
			else { r = 0; g = 220; b = 255; }
			break;
		}
	}

	// -------------------------------------------------------------
	// GAMBAR SNAPLINE (Hardware GPU Line - 100% Halus, Sleek, Tanpa Kotak-Kotak)
	// Nol beban buffer 2D UI -> Menghilangkan 100% Kedipan (Blink) pada ESP
	// 5 Opsi Origin:
	// 0: Dari Karakter
	// 1: Atas Layar
	// 2: Bawah Layar
	// 3: Fixed Atas Tengah Edge Layar
	// 4: Fixed Bawah Tengah Edge Layar
	// -------------------------------------------------------------
	void DrawSnapline(const Vector3& playerCoords, const Vector3& targetCoords, int r, int g, int b, int a, float alphaMult = 1.0f)
	{
		// Cek apakah target terlihat di frustum layar
		float tSx = 0.0f, tSy = 0.0f;
		if (!GRAPHICS::_WORLD3D_TO_SCREEN2D(targetCoords.x, targetCoords.y, targetCoords.z, &tSx, &tSy))
		{
			return; // Target berada di belakang kamera
		}
		if (tSx < -0.05f || tSx > 1.05f || tSy < -0.05f || tSy > 1.05f)
		{
			return; // Di luar batas pandangan layar
		}

		Vector3 startPos = playerCoords;

		if (g_cfg.espLineOrigin == 0) // Dari Karakter
		{
			startPos.z -= 0.45f;
		}
		else if (g_cfg.espLineOrigin == 1 || g_cfg.espLineOrigin == 2) // Atas Layar (1) atau Bawah Layar (2)
		{
			Vector3 camPos = CAM::GET_GAMEPLAY_CAM_COORD();
			Vector3 camRot = CAM::GET_GAMEPLAY_CAM_ROT(2);
			float pitch = camRot.x * 0.0174532925f;
			float yaw   = camRot.z * 0.0174532925f;
			float cosP  = std::abs(std::cos(pitch));

			Vector3 camFwd = {};
			camFwd.x = -std::sin(yaw) * cosP;
			camFwd.y =  std::cos(yaw) * cosP;
			camFwd.z =  std::sin(pitch);

			Vector3 camUp = {};
			camUp.z = 1.0f;

			float offsetY = (g_cfg.espLineOrigin == 1) ? 0.42f : -0.42f;
			startPos.x = camPos.x + camFwd.x * 0.85f + camUp.x * offsetY;
			startPos.y = camPos.y + camFwd.y * 0.85f + camUp.y * offsetY;
			startPos.z = camPos.z + camFwd.z * 0.85f + camUp.z * offsetY;
		}
		else // 3: Fixed Atas Tengah Edge Layar, 4: Fixed Bawah Tengah Edge Layar
		{
			Vector3 camPos = CAM::GET_GAMEPLAY_CAM_COORD();
			Vector3 camRot = CAM::GET_GAMEPLAY_CAM_ROT(2);
			float pitch = camRot.x * 0.0174532925f;
			float roll  = camRot.y * 0.0174532925f;
			float yaw   = camRot.z * 0.0174532925f;
			float cosP  = std::cos(pitch);

			Vector3 camFwd = {};
			camFwd.x = -std::sin(yaw) * cosP;
			camFwd.y = std::cos(yaw) * cosP;
			camFwd.z = std::sin(pitch);

			Vector3 camRight = {};
			camRight.x = std::cos(yaw) * std::cos(roll) + std::sin(yaw) * std::sin(pitch) * std::sin(roll);
			camRight.y = std::sin(yaw) * std::cos(roll) - std::cos(yaw) * std::sin(pitch) * std::sin(roll);
			camRight.z = -cosP * std::sin(roll);

			Vector3 camUp = {};
			camUp.x = camRight.y * camFwd.z - camRight.z * camFwd.y;
			camUp.y = camRight.z * camFwd.x - camRight.x * camFwd.z;
			camUp.z = camRight.x * camFwd.y - camRight.y * camFwd.x;

			float fov = CAM::GET_GAMEPLAY_CAM_FOV();
			if (fov <= 5.0f) fov = 50.0f;
			float halfTan = std::tan(fov * 0.5f * 0.0174532925f);
			float nearDist = 0.55f;
			float topOffset = nearDist * halfTan;
			if (g_cfg.espLineOrigin == 4) topOffset = -topOffset; // Bawah tengah

			startPos.x = camPos.x + camFwd.x * nearDist + camUp.x * topOffset;
			startPos.y = camPos.y + camFwd.y * nearDist + camUp.y * topOffset;
			startPos.z = camPos.z + camFwd.z * nearDist + camUp.z * topOffset;
		}

		int effA = ApplyEspAlpha((int)((float)a * alphaMult));
		if (effA <= 0) return;

		GRAPHICS::DRAW_LINE(startPos.x, startPos.y, startPos.z, targetCoords.x, targetCoords.y, targetCoords.z, r, g, b, effA);
		if (g_cfg.espLineThickness >= 2)
		{
			GRAPHICS::DRAW_LINE(startPos.x, startPos.y, startPos.z + 0.008f, targetCoords.x, targetCoords.y, targetCoords.z + 0.008f, r, g, b, effA);
		}
		if (g_cfg.espLineThickness >= 3)
		{
			GRAPHICS::DRAW_LINE(startPos.x, startPos.y, startPos.z - 0.008f, targetCoords.x, targetCoords.y, targetCoords.z - 0.008f, r, g, b, effA);
		}
	}

	inline void DrawBoneNodeRect(const Vector3& worldPos, float dotW, float dotH, int r, int g, int b, int a)
	{
		float sx = 0.0f, sy = 0.0f;
		if (GRAPHICS::_WORLD3D_TO_SCREEN2D(worldPos.x, worldPos.y, worldPos.z, &sx, &sy))
		{
			if (sx >= -0.05f && sx <= 1.05f && sy >= -0.05f && sy <= 1.05f)
			{
				DrawEspRect(sx, sy, dotW, dotH, r, g, b, a);
			}
		}
	}

	// -------------------------------------------------------------
	// SKELETON (TULANG) ESP MANUSIA (MURNI RECTANGLE / TITIK ANATOMI)
	// Tanpa garis yang bertabrakan, 100% tembus pandang bersih
	// -------------------------------------------------------------
	void DrawPedSkeleton(Ped ped, int r, int g, int b, int a, float distMeters = 0.0f)
	{
		int effAlpha = ApplyEspRectAlpha(a);
		if (effAlpha <= 0) return;

		float skelScale = std::clamp(g_cfg.espSkeletonScale, 0.10f, 3.00f);
		float aspect = GRAPHICS::_GET_SCREEN_ASPECT_RATIO(FALSE);
		if (aspect <= 0.0f) aspect = 1.7777778f;

		float dotW = std::clamp(0.0028f * (float)g_cfg.espLineThickness * skelScale, 0.0003f, 0.0090f);
		float dotH = dotW * aspect;

		auto DrawBone = [&](int bId)
		{
			Vector3 p = PED::GET_PED_BONE_COORDS(ped, bId, 0.0f, 0.0f, 0.0f);
			DrawBoneNodeRect(p, dotW, dotH, r, g, b, effAlpha);
		};

		auto DrawBoneMid = [&](int b1, int b2)
		{
			Vector3 p1 = PED::GET_PED_BONE_COORDS(ped, b1, 0.0f, 0.0f, 0.0f);
			Vector3 p2 = PED::GET_PED_BONE_COORDS(ped, b2, 0.0f, 0.0f, 0.0f);
			Vector3 mid = {};
			mid.x = (p1.x + p2.x) * 0.5f;
			mid.y = (p1.y + p2.y) * 0.5f;
			mid.z = (p1.z + p2.z) * 0.5f;
			DrawBoneNodeRect(mid, dotW * 0.85f, dotH * 0.85f, r, g, b, effAlpha);
		};

		int q = g_cfg.espSkeletonQuality;

		// 1. Tulang Belakang & Anatomi Badan Utama
		DrawBone(SKEL_Head);
		if (q < 2) DrawBone(SKEL_Neck_1);
		DrawBone(SKEL_Spine2);
		if (q == 0) DrawBoneMid(SKEL_Neck_1, SKEL_Spine2);
		DrawBone(SKEL_Pelvis);
		if (q == 0) DrawBoneMid(SKEL_Spine2, SKEL_Pelvis);

		// 2. Lengan Kiri
		if (q < 2) DrawBone(SKEL_L_UpperArm);
		DrawBone(SKEL_L_Forearm);
		if (q == 0) DrawBoneMid(SKEL_L_UpperArm, SKEL_L_Forearm);
		if (q == 0 && distMeters <= 30.0f) DrawBone(SKEL_L_Hand);

		// 3. Lengan Kanan
		if (q < 2) DrawBone(SKEL_R_UpperArm);
		DrawBone(SKEL_R_Forearm);
		if (q == 0) DrawBoneMid(SKEL_R_UpperArm, SKEL_R_Forearm);
		if (q == 0 && distMeters <= 30.0f) DrawBone(SKEL_R_Hand);

		// 4. Kaki Kiri
		if (q < 2) DrawBone(SKEL_L_Thigh);
		DrawBone(SKEL_L_Calf);
		if (q == 0) DrawBoneMid(SKEL_L_Thigh, SKEL_L_Calf);
		if (q == 0 && distMeters <= 30.0f) DrawBone(SKEL_L_Foot);

		// 5. Kaki Kanan
		if (q < 2) DrawBone(SKEL_R_Thigh);
		DrawBone(SKEL_R_Calf);
		if (q == 0) DrawBoneMid(SKEL_R_Thigh, SKEL_R_Calf);
		if (q == 0 && distMeters <= 30.0f) DrawBone(SKEL_R_Foot);
	}

	// -------------------------------------------------------------
	// SKELETON (TULANG) ESP HEWAN (QUADRUPED / MAMALIA / BURUNG)
	// -------------------------------------------------------------
	void DrawAnimalSkeleton(Ped ped, int r, int g, int b, int a, float distMeters = 0.0f)
	{
		int effAlpha = ApplyEspRectAlpha(a);
		if (effAlpha <= 0) return;

		float skelScale = std::clamp(g_cfg.espSkeletonScale, 0.10f, 3.00f);
		float aspect = GRAPHICS::_GET_SCREEN_ASPECT_RATIO(FALSE);
		if (aspect <= 0.0f) aspect = 1.7777778f;

		float dotW = std::clamp(0.0028f * (float)g_cfg.espLineThickness * skelScale, 0.0003f, 0.0090f);
		float dotH = dotW * aspect;
		Vector3 rootPos = ENTITY::GET_ENTITY_COORDS(ped, TRUE);

		auto DrawBoneByNameOrId = [&](const char* boneName, int fallbackId)
		{
			Vector3 p = {};
			int bIdx = ENTITY::GET_ENTITY_BONE_INDEX_BY_NAME(ped, (char*)boneName);
			if (bIdx != -1)
			{
				p = ENTITY::GET_WORLD_POSITION_OF_ENTITY_BONE(ped, bIdx);
			}
			else if (fallbackId != -1)
			{
				p = PED::GET_PED_BONE_COORDS(ped, fallbackId, 0.0f, 0.0f, 0.0f);
			}
			else return;

			float dx = p.x - rootPos.x;
			float dy = p.y - rootPos.y;
			float dz = p.z - rootPos.z;
			if ((dx * dx + dy * dy + dz * dz) < 25.0f && (p.x != 0.0f || p.y != 0.0f))
			{
				DrawBoneNodeRect(p, dotW, dotH, r, g, b, effAlpha);
			}
		};

		int q = g_cfg.espSkeletonQuality;

		// 1. Kepala & Punggung
		DrawBoneByNameOrId("head", SKEL_Head);
		if (q < 2) DrawBoneByNameOrId("neck", SKEL_Neck_1);
		DrawBoneByNameOrId("spine", SKEL_Spine2);
		DrawBoneByNameOrId("pelvis", SKEL_Pelvis);

		// 2. Kaki Depan (Kiri & Kanan)
		DrawBoneByNameOrId("leg_lf", SKEL_L_UpperArm);
		if (q == 0) DrawBoneByNameOrId("foot_lf", SKEL_L_Forearm);
		DrawBoneByNameOrId("leg_rf", SKEL_R_UpperArm);
		if (q == 0) DrawBoneByNameOrId("foot_rf", SKEL_R_Forearm);

		// 3. Kaki Belakang (Kiri & Kanan)
		DrawBoneByNameOrId("leg_lr", SKEL_L_Thigh);
		if (q == 0) DrawBoneByNameOrId("foot_lr", SKEL_L_Calf);
		DrawBoneByNameOrId("leg_rr", SKEL_R_Thigh);
		if (q == 0) DrawBoneByNameOrId("foot_rr", SKEL_R_Calf);

		// 4. Ekor
		if (q == 0 && distMeters <= 25.0f)
		{
			DrawBoneByNameOrId("tail", -1);
		}
	}

	// -------------------------------------------------------------
	// ESP EDGE (INDIKATOR TARGET DI LUAR / DI BELAKANG KAMERA)
	// Opacity 60% (153 / 255) yang dapat diatur via Global ESP Opacity
	// -------------------------------------------------------------
	void DrawEdgeIndicator(const Vector3& targetCoords, int r, int g, int b, int /*a*/, float distMeters, const char* label)
	{
		Vector3 camPos = CAM::GET_GAMEPLAY_CAM_COORD();
		Vector3 camRot = CAM::GET_GAMEPLAY_CAM_ROT(2);
		float yawRad = camRot.z * 0.0174532925f;

		float dx = targetCoords.x - camPos.x;
		float dy = targetCoords.y - camPos.y;

		// Hitung posisi horizontal (right) dan kedalaman (forward) relatif terhadap kamera
		float fwd = -dx * std::sin(yawRad) + dy * std::cos(yawRad);
		float right = dx * std::cos(yawRad) + dy * std::sin(yawRad);

		float angle = std::atan2(right, fwd);
		float dirX = std::sin(angle);
		float dirY = -std::cos(angle);

		float edgeX = 0.500f + dirX * 0.440f;
		float edgeY = 0.500f + dirY * 0.420f;

		edgeX = std::clamp(edgeX, 0.035f, 0.965f);
		edgeY = std::clamp(edgeY, 0.045f, 0.955f);

		float dotSize = 0.0050f * (float)g_cfg.espLineThickness;
		float aspect = GRAPHICS::_GET_SCREEN_ASPECT_RATIO(FALSE);
		if (aspect <= 0.0f) aspect = 1.7777778f;

		int effAlpha = ApplyEspAlpha(153); // ESP Edge Opacity 60%

		// Titik / panah penanda di pinggir layar
		DrawEspRect(edgeX, edgeY, dotSize, dotSize * aspect, r, g, b, effAlpha);

		// Label jarak target di tepi layar
		char edgeBuf[32];
		sprintf_s(edgeBuf, "%s %dm", label, (int)std::round(distMeters));
		float textX = std::clamp(edgeX - 0.022f, 0.015f, 0.910f);
		float textY = std::clamp(edgeY + 0.006f, 0.020f, 0.960f);
		DrawTextElement(edgeBuf, textX, textY, g_cfg.espTextScale * 0.82f, 4, r, g, b, effAlpha);
	}

	// -------------------------------------------------------------
	// ESP CROSSHAIR DI TITIK TENGAH LAYAR
	// -------------------------------------------------------------
	void DrawCrosshair()
	{
		if (!g_cfg.espCrosshair) return;
		if (CUTSCENE::IS_CUTSCENE_PLAYING()) return;

		float cx = 0.500f;
		float cy = 0.500f;
		float size = g_cfg.espCrosshairSize;
		float thick = g_cfg.espCrosshairThick;

		int r = 0, g = 255, b = 100;
		switch (g_cfg.espCrosshairColor)
		{
		case 1: r = 255; g = 40; b = 40; break;
		case 2: r = 255; g = 255; b = 255; break;
		case 3: r = 0; g = 220; b = 255; break;
		case 4: r = 255; g = 220; b = 0; break;
		case 5: r = 200; g = 60; b = 255; break;
		default: r = 0; g = 255; b = 100; break;
		}

		int effAlpha = ApplyEspAlpha(255);

		switch (g_cfg.espCrosshairType)
		{
		case 0: // Titik (Dot)
			DrawEspRect(cx, cy, thick * 1.5f, thick * 2.4f, r, g, b, effAlpha);
			break;
		case 1: // Tanda Tambah (+)
			DrawEspRect(cx, cy, size, thick * 1.6f, r, g, b, effAlpha);
			DrawEspRect(cx, cy, thick, size * 1.6f, r, g, b, effAlpha);
			break;
		case 2: // Kotak (Box)
			{
				float halfS = size * 0.5f;
				DrawEspRect(cx, cy - halfS * 1.6f, size, thick * 1.6f, r, g, b, effAlpha);
				DrawEspRect(cx, cy + halfS * 1.6f, size, thick * 1.6f, r, g, b, effAlpha);
				DrawEspRect(cx - halfS, cy, thick, size * 1.6f, r, g, b, effAlpha);
				DrawEspRect(cx + halfS, cy, thick, size * 1.6f, r, g, b, effAlpha);
			}
			break;
		case 3: // Tactic Gap Cross
			{
				float gap = size * 0.35f;
				float barLen = size * 0.65f;
				DrawEspRect(cx - gap - barLen * 0.5f, cy, barLen, thick * 1.6f, r, g, b, effAlpha);
				DrawEspRect(cx + gap + barLen * 0.5f, cy, barLen, thick * 1.6f, r, g, b, effAlpha);
				DrawEspRect(cx, cy - (gap + barLen * 0.5f) * 1.6f, thick, barLen * 1.6f, r, g, b, effAlpha);
				DrawEspRect(cx, cy + (gap + barLen * 0.5f) * 1.6f, thick, barLen * 1.6f, r, g, b, effAlpha);
				DrawEspRect(cx, cy, thick * 0.8f, thick * 1.2f, r, g, b, effAlpha);
			}
			break;
		case 4: // Chevron (^)
			DrawScreenLine(cx - size * 0.5f, cy + size * 0.5f, cx, cy, thick, r, g, b, 255);
			DrawScreenLine(cx + size * 0.5f, cy + size * 0.5f, cx, cy, thick, r, g, b, 255);
			break;
		}
	}

	// -------------------------------------------------------------
	// ESP AIM CIRCLE DI CENTER LAYAR (INDIKATOR TEPAT SASARAN & KEPALA)
	// Pure Circle Sempurna Berbentuk Vector Melingkar Tertutup Tanpa Titik-Titik
	// -------------------------------------------------------------
	void DrawAimCircle(float dt)
	{
		if (!g_cfg.espAimCircle) return;
		if (CUTSCENE::IS_CUTSCENE_PLAYING()) return;

		// Menentukan target radius dan warna berdasarkan g_aimTargetState
		// 0: Normal / Tidak ada target
		// 1: Mengarah ke Badan NPC (Lock Tepat Sasaran)
		// 2: Mengarah ke Kepala NPC (Circle mengecil, kepala siap ditembak)
		float baseR = g_cfg.espAimCircleSize;
		float targetRadius = baseR;
		int targetR = 210, targetG = 240, targetB = 255, targetAlpha = 160;

		if (g_aimTargetState == 1) // Badan NPC
		{
			targetRadius = baseR * 1.15f;
			targetR = 255; targetG = 195; targetB = 25; targetAlpha = 235; // Kuning-Oranye Tepat Sasaran
		}
		else if (g_aimTargetState == 2) // Kepala NPC (Siap Ditembak!)
		{
			targetRadius = baseR * 0.50f; // Circle mengecil tajam melingkari kepala
			targetR = 255; targetG = 35; targetB = 60; targetAlpha = 255; // Merah-Crimson Neon
		}

		// Transisi Halus (Smooth Damped Interpolation dengan guard validitas dt)
		float effectiveDt = (dt > 0.0001f && dt < 0.2f) ? dt : 0.016f;
		float blendRate = std::min(1.0f, 15.0f * effectiveDt);
		g_aimCircleCurRadius += (targetRadius - g_aimCircleCurRadius) * blendRate;
		g_aimCircleCurR += ((float)targetR - g_aimCircleCurR) * blendRate;
		g_aimCircleCurG += ((float)targetG - g_aimCircleCurG) * blendRate;
		g_aimCircleCurB += ((float)targetB - g_aimCircleCurB) * blendRate;
		g_aimCircleCurAlpha += ((float)targetAlpha - g_aimCircleCurAlpha) * blendRate;

		int colR = std::clamp((int)g_aimCircleCurR, 0, 255);
		int colG = std::clamp((int)g_aimCircleCurG, 0, 255);
		int colB = std::clamp((int)g_aimCircleCurB, 0, 255);
		int colA = ApplyEspAlpha(std::clamp((int)g_aimCircleCurAlpha, 0, 255));

		// Posisi FIXED 100% tepat di tengah layar monitor (Anti-Clip, selalu di atas segala objek/tubuh/senjata)
		const float cx = 0.500f;
		const float cy = 0.500f;

		float aspect = GRAPHICS::_GET_SCREEN_ASPECT_RATIO(FALSE);
		if (aspect <= 0.0f) aspect = 1.7777778f;

		float rX = g_aimCircleCurRadius;
		float rY = g_aimCircleCurRadius * aspect;

		// Render Garis Lingkaran Mulus Sempurna (18 Titik Overlap - Bisa disetting scalenya)
		const int SEGS = 18;
		float circleRectScale = std::clamp(g_cfg.espAimCircleRectScale, 0.40f, 2.50f);
		float dotSize = 0.0038f * (float)g_cfg.espLineThickness * circleRectScale;
		float dotH = dotSize * aspect;

		for (int i = 0; i < SEGS; ++i)
		{
			float a = ((float)i / (float)SEGS) * 6.2831853f;
			float px = cx + std::cos(a) * rX;
			float py = cy + std::sin(a) * rY;
			DrawEspRect(px, py, dotSize, dotH, colR, colG, colB, colA);
		}

		// Ketika Mengarah ke Kepala (Circle mengecil), tampilkan 4 penanda notch presisi & crosshair titik tengah
		if (g_aimCircleCurRadius <= baseR * 0.75f)
		{
			float gap = 0.0025f;
			float tickLen = 0.0045f * circleRectScale;
			float tickThick = 0.0014f * (float)g_cfg.espLineThickness * circleRectScale;

			// Top notch
			DrawEspRect(cx, cy - (rY + (gap + tickLen * 0.5f) * aspect), tickThick, tickLen * aspect, colR, colG, colB, colA);
			// Bottom notch
			DrawEspRect(cx, cy + (rY + (gap + tickLen * 0.5f) * aspect), tickThick, tickLen * aspect, colR, colG, colB, colA);
			// Left notch
			DrawEspRect(cx - (rX + gap + tickLen * 0.5f), cy, tickLen, tickThick * aspect, colR, colG, colB, colA);
			// Right notch
			DrawEspRect(cx + (rX + gap + tickLen * 0.5f), cy, tickLen, tickThick * aspect, colR, colG, colB, colA);

			// Titik tengah micro-cross
			float crossSize = 0.0035f * circleRectScale;
			DrawEspRect(cx, cy, crossSize, tickThick * aspect, colR, colG, colB, colA);
			DrawEspRect(cx, cy, tickThick, crossSize * aspect, colR, colG, colB, colA);
		}
	}

	// -------------------------------------------------------------
	// SOUND EFFECT ELIMINASI / KILL
	// -------------------------------------------------------------
	void PlayKillSound()
	{
		switch (g_cfg.espKillSoundMode)
		{
		case 1: PlayFrontendSound("CHALLENGE_UNLOCKED"); break;
		case 2: PlayFrontendSound("RACE_PLACED"); break;
		case 3: PlayFrontendSound("WEAPON_PURCHASE"); break;
		case 4: PlayFrontendSound("MEDAL_UP"); break;
		case 5: PlayFrontendSound("CHECKPOINT_PERFECT"); break;
		default: break;
		}
	}

	// -------------------------------------------------------------
	// ESP KILL SATISFYING BANNER STREAK (SATISFYING BOUNCING ELIMINATED)
	// -------------------------------------------------------------
	void DrawKillSatisfyingBanner(float dt, ULONGLONG currentTick)
	{
		if (!g_cfg.espKillSatisfying || g_killStreakCount <= 0) return;
		if (CUTSCENE::IS_CUTSCENE_PLAYING() || UI::IS_HUD_HIDDEN()) return;

		ULONGLONG elapsed = currentTick - g_lastKillTick;

		// 9 detik tanpa aktivitas membunuh -> reset streak ke 0 dan banner menghilang
		if (elapsed >= 9000)
		{
			g_killStreakCount = 0;
			g_killBannerScaleBlend = 0.0f;
			return;
		}

		// Fade out halus antara 7.5 detik sampai 9 detik
		float fadeAlpha = 1.0f;
		if (elapsed > 7500)
		{
			fadeAlpha = 1.0f - ((float)(elapsed - 7500) / 1500.0f);
			fadeAlpha = std::clamp(fadeAlpha, 0.0f, 1.0f);
		}

		// Transisi antara Status Besar (2.4 detik awal) dan Status Mengecil (setelah 2.4 detik)
		if (elapsed <= 2400)
		{
			g_killBannerScaleBlend += 7.0f * dt;
			if (g_killBannerScaleBlend > 1.0f) g_killBannerScaleBlend = 1.0f;
		}
		else
		{
			// Setelah 2.4 detik mengecil menjadi streak indicator menunggu kill berikutnya
			g_killBannerScaleBlend -= 3.0f * dt;
			if (g_killBannerScaleBlend < 0.0f) g_killBannerScaleBlend = 0.0f;
		}

		float blend = std::clamp(g_killBannerScaleBlend, 0.0f, 1.0f);

		// Punch-in bounce saat baru kill (< 450ms)
		float bounce = 1.0f;
		if (elapsed <= 450)
		{
			float p = (float)elapsed / 450.0f;
			bounce = EaseOutBack(p);
		}

		// Efek Bersinar (Glowing Shine Wave)
		float shinePhase = (float)(currentTick % 900) / 900.0f;
		float shine = 0.5f + 0.5f * std::sin(shinePhase * 6.2831853f);

		// Posisi: Besar di Y=0.215, Kecil di Y=0.120 (Tengah atas)
		float bannerX = 0.500f;
		float bannerY = 0.120f + (0.215f - 0.120f) * blend;

		if (blend > 0.08f)
		{
			// Angka Streak Jumlah Kill Sangat Besar Bouncing & Glowing
			char countStr[32];
			sprintf_s(countStr, "%dx", g_killStreakCount);
			float countScale = (0.75f + 0.25f * blend) * bounce;
			float countW = GetTextWidth(countStr, countScale, 4);
			float countY = bannerY - (countScale * 0.026f);

			// Warna Emas Berkilau / Neon Amber
			int elimR = 255;
			int elimG = (int)(180 + 75 * shine);
			int elimB = (int)(40 + 70 * shine);
			int elimA = ApplyEspTextAlpha((int)(255 * blend * fadeAlpha));

			DrawTextElement(countStr, bannerX - (countW * 0.5f), countY, countScale, 4, elimR, elimG, elimB, elimA);

			// Judul Banner di Bawah Angka: Membedakan NPC Terbunuh vs Hewan Terbunuh
			std::string elimTitleStr = "";
			if (g_lastKillIsAnimal)
			{
				if (!g_lastKillAnimalName.empty())
					elimTitleStr = "- " + g_lastKillAnimalName + " " + std::string(L("DIELIMINASI -", "HUNTED -"));
				else
					elimTitleStr = L("- HEWAN DIELIMINASI -", "- ANIMAL HUNTED -");
			}
			else
			{
				elimTitleStr = (g_killStreakCount > 1) ? L("- KILL STREAK -", "- KILL STREAK -") : L("- TARGET DIELIMINASI -", "- TARGET ELIMINATED -");
			}
			float elimScale = (0.42f + 0.10f * blend) * bounce;
			float elimW = GetTextWidth(elimTitleStr.c_str(), elimScale, 4);
			float elimY = countY + 0.052f * blend;

			DrawTextElement(elimTitleStr.c_str(), bannerX - (elimW * 0.5f), elimY, elimScale, 4, 255, 255, 255, elimA);
		}
		else
		{
			// Status Mengecil (Waiting Indicator): Teks Warna Putih & Tanpa Tanda '[' atau ']'
			char streakStr[96];
			if (g_lastKillIsAnimal)
			{
				if (!g_lastKillAnimalName.empty())
					sprintf_s(streakStr, "%dx %s %s", g_killStreakCount, g_lastKillAnimalName.c_str(), L("Dieliminasi", "Hunted"));
				else
					sprintf_s(streakStr, "%dx %s", g_killStreakCount, L("Hewan Dieliminasi", "Animal Hunted"));
			}
			else
			{
				sprintf_s(streakStr, "%dx %s", g_killStreakCount, L("Target Dieliminasi", "Target Eliminated"));
			}

			float curFontScale = g_cfg.espKillBannerScale * 1.30f;
			float textW = GetTextWidth(streakStr, curFontScale, 4);
			float textY = bannerY - (curFontScale * 0.020f);
			int textA = ApplyEspTextAlpha((int)(245 * fadeAlpha));

			// Warna Putih Bersih
			DrawTextElement(streakStr, bannerX - (textW * 0.5f), textY, curFontScale, 4, 255, 255, 255, textA);
		}
	}

	// -------------------------------------------------------------
	// FLOATING DAMAGE TEXT ESP (BORDERLANDS / RPG STYLE)
	// Layer paling depan di layar, angka lebih besar & jelas, tulisan HEADSHOT
	// -------------------------------------------------------------
	void DrawDamageIndicators(ULONGLONG currentTick)
	{
		if (!g_cfg.espDamageText || g_damageIndicators.empty()) return;

		for (auto it = g_damageIndicators.begin(); it != g_damageIndicators.end(); )
		{
			ULONGLONG elapsed = currentTick - it->spawnTick;
			if (elapsed > 1850)
			{
				it = g_damageIndicators.erase(it);
				continue;
			}

			float t = (float)elapsed / 1850.0f; // 0.0 -> 1.0
			float floatZ = t * 0.52f;           // Mengapung perlahan ke atas
			Vector3 curPos = it->worldPos;
			curPos.z += floatZ;

			float scX = 0.0f, scY = 0.0f;
			if (GRAPHICS::_WORLD3D_TO_SCREEN2D(curPos.x, curPos.y, curPos.z, &scX, &scY))
			{
				float pop = (t < 0.15f) ? (1.0f + 0.45f * (1.0f - t / 0.15f)) : 1.0f;
				// Ukuran teks dibuat jauh lebih besar dan tebal
				float scale = (it->isHeadshot ? 0.62f : 0.48f) * pop;

				float alphaFactor = (t < 0.70f) ? 1.0f : (1.0f - (t - 0.70f) / 0.30f);
				int a = ApplyEspTextAlpha((int)(255 * alphaFactor));

				char buf[48];
				if (it->isHeadshot)
				{
					// Tulisan HEADSHOT dengan angka damage merah menyala
					sprintf_s(buf, "-%d HEADSHOT", it->damage);
					float w = GetTextWidth(buf, scale, 4);
					DrawTextElement(buf, scX - (w * 0.5f), scY, scale, 4, 255, 30, 30, a, true);
				}
				else
				{
					sprintf_s(buf, "-%d", it->damage);
					float w = GetTextWidth(buf, scale, 4);
					DrawTextElement(buf, scX - (w * 0.5f), scY, scale, 4, 255, 185, 30, a, true);
				}
			}
			++it;
		}
	}

	// -------------------------------------------------------------
	// SPEEDOMETER GAYA FORZA HORIZON (KANAN BAWAH)
	// -------------------------------------------------------------
	void DrawForzaSpeedometer(Vehicle veh, float actualKmh, int currentAlphaInt, float dt)
	{
		(void)veh;
		float aspect = GRAPHICS::_GET_SCREEN_ASPECT_RATIO(FALSE);
		if (aspect <= 0.0f) aspect = 1.7777778f;

		float cx = g_cfg.forzaPosX;
		float cy = g_cfg.forzaPosY;
		float R = 0.078f * g_cfg.forzaScale;

		float startDeg = 215.0f;
		float sweepDeg = 250.0f;

		// 1. Gambar Dial Track & Arc (Smooth Overlapping Curved Track - Buffer-Safe)
		const int ARC_SEGS = 44;
		float arcDotW = 0.0055f * g_cfg.forzaScale;
		float arcDotH = arcDotW * aspect;

		for (int i = 0; i <= ARC_SEGS; ++i)
		{
			float d = startDeg - ((float)i / (float)ARC_SEGS) * sweepDeg;
			float rad = d * 0.0174532925f;

			float px = cx + std::cos(rad) * R;
			float py = cy - std::sin(rad) * (R * aspect);

			bool isRedline = (d <= 30.0f); // 8k - 10k RPM
			int arcR = isRedline ? 255 : 235;
			int arcG = isRedline ? 35 : 245;
			int arcB = isRedline ? 45 : 255;

			if (!isRedline && g_kmhFlashTimer > 0.0f)
			{
				float p = std::clamp(g_kmhFlashTimer / 0.50f, 0.0f, 1.0f);
				arcR = (int)(arcR * (1.0f - p) + 255 * p);
				arcG = (int)(arcG * (1.0f - p) + 220 * p);
				arcB = (int)(arcB * (1.0f - p) + 0 * p);
			}

			// A. Subtle Dark Gauge Track Backdrop
			GRAPHICS::DRAW_RECT(px, py, arcDotW * 1.35f, arcDotH * 1.35f, 15, 18, 25, (int)(currentAlphaInt * 0.65f));
			// B. Arc Line yang Mulus Solid Berkesinambungan
			GRAPHICS::DRAW_RECT(px, py, arcDotW, arcDotH, arcR, arcG, arcB, currentAlphaInt);
		}

		// 2. Gambar Ticks Radial yang Solid & Angka RPM 0..10
		for (int k = 0; k <= 10; ++k)
		{
			float tDeg = startDeg - ((float)k / 10.0f) * sweepDeg;
			float tRad = tDeg * 0.0174532925f;

			// Major Tick (Garis radial solid menyambung ke arc)
			float t1x = cx + std::cos(tRad) * (R - 0.0080f * g_cfg.forzaScale);
			float t1y = cy - std::sin(tRad) * ((R - 0.0080f * g_cfg.forzaScale) * aspect);
			float t2x = cx + std::cos(tRad) * (R + 0.0015f * g_cfg.forzaScale);
			float t2y = cy - std::sin(tRad) * ((R + 0.0015f * g_cfg.forzaScale) * aspect);

			bool isRed = (k >= 8);
			int tR = isRed ? 255 : 230;
			int tG = isRed ? 40 : 240;
			int tB = isRed ? 45 : 250;

			DrawScreenLine(t1x, t1y, t2x, t2y, 0.0016f * g_cfg.forzaScale, tR, tG, tB, currentAlphaInt);

			// Minor Tick di antara angka (setengah RPM, misal 0.5, 1.5, ... 9.5)
			if (k < 10)
			{
				float midDeg = startDeg - (((float)k + 0.5f) / 10.0f) * sweepDeg;
				float midRad = midDeg * 0.0174532925f;
				float mt1x = cx + std::cos(midRad) * (R - 0.0045f * g_cfg.forzaScale);
				float mt1y = cy - std::sin(midRad) * ((R - 0.0045f * g_cfg.forzaScale) * aspect);
				float mt2x = cx + std::cos(midRad) * R;
				float mt2y = cy - std::sin(midRad) * (R * aspect);
				bool mRed = (k >= 8);
				DrawScreenLine(mt1x, mt1y, mt2x, mt2y, 0.0011f * g_cfg.forzaScale, mRed ? 255 : 190, mRed ? 40 : 205, mRed ? 45 : 215, (int)(currentAlphaInt * 0.85f));
			}

			// Angka label
			float nDist = R - 0.0155f * g_cfg.forzaScale;
			float nx = cx + std::cos(tRad) * nDist;
			float ny = cy - std::sin(tRad) * (nDist * aspect);

			char kStr[8];
			sprintf_s(kStr, "%d", k);
			DrawMenuText(kStr, nx - 0.0035f, ny - 0.008f, 0.20f * g_cfg.forzaScale, 4, tR, tG, tB, currentAlphaInt, false);
		}

		// 3. Center Hub & Indikator Gigi (Lingkaran Solid Terhubung Penuh & Backplate Halus)
		float hubR = 0.022f * g_cfg.forzaScale;

		// Backplate circular gelap di tengah hub (Lapisan halus bebas kotak)
		GRAPHICS::DRAW_RECT(cx, cy, hubR * 1.95f, (hubR * 1.95f) * aspect, 15, 18, 25, (int)(currentAlphaInt * 0.75f));

		// Warna cincin hub
		std::string fGear = g_currentGearStr;
		int hubColR = 55, hubColG = 240, hubColB = 95;
		if (fGear == "R")
		{
			hubColR = 255; hubColG = 215; hubColB = 20; // Kuning
		}
		else if (g_kmhFlashTimer > 0.0f)
		{
			float p = std::clamp(g_kmhFlashTimer / 0.50f, 0.0f, 1.0f);
			hubColR = (int)(hubColR * (1.0f - p) + 255 * p);
			hubColG = (int)(hubColG * (1.0f - p) + 220 * p);
			hubColB = (int)(hubColB * (1.0f - p) + 0 * p);
		}

		// Cincin hub melingkar padat terhubung tanpa putus (28 Segmen Overlapping Rapat)
		const int HUB_SEGS = 28;
		float hubDot = 0.0048f * g_cfg.forzaScale;
		for (int i = 0; i < HUB_SEGS; ++i)
		{
			float a = ((float)i / (float)HUB_SEGS) * 6.2831853f;
			float hx = cx + std::cos(a) * hubR;
			float hy = cy - std::sin(a) * (hubR * aspect);
			GRAPHICS::DRAW_RECT(hx, hy, hubDot, hubDot * aspect, hubColR, hubColG, hubColB, currentAlphaInt);
		}

		// Teks Gigi di Tengah Hub
		float gearW = GetTextWidth(fGear.c_str(), 0.36f * g_cfg.forzaScale, 4);
		DrawTextElement(fGear.c_str(), cx - (gearW * 0.5f), cy - 0.014f * g_cfg.forzaScale, 0.36f * g_cfg.forzaScale, 4,
		                hubColR, hubColG, hubColB, currentAlphaInt);

		// 4. Jarum RPM (Needle) yang Solid Mulus
		float targetRpm = 0.15f;
		float spd = actualKmh;
		int gNum = 1;
		if (isdigit(g_currentGearStr[0])) gNum = g_currentGearStr[0] - '0';

		float gMax = 35.0f, gMin = 0.0f;
		switch (gNum)
		{
		case 1: gMin = 0.0f; gMax = 35.0f; break;
		case 2: gMin = 25.0f; gMax = 68.0f; break;
		case 3: gMin = 55.0f; gMax = 105.0f; break;
		case 4: gMin = 90.0f; gMax = 148.0f; break;
		case 5: gMin = 130.0f; gMax = 195.0f; break;
		default: gMin = 175.0f; gMax = 265.0f; break;
		}

		float normRpm = std::clamp((spd - gMin) / std::max(5.0f, gMax - gMin), 0.0f, 1.0f);
		targetRpm = 0.18f + normRpm * 0.78f;
		if (CONTROLS::IS_CONTROL_PRESSED(0, 71)) targetRpm += 0.04f;
		targetRpm = std::clamp(targetRpm, 0.15f, 0.98f);

		g_dampedRpm += (targetRpm - g_dampedRpm) * std::min(1.0f, 15.0f * dt);

		float needleDeg = startDeg - g_dampedRpm * sweepDeg;
		float needleRad = needleDeg * 0.0174532925f;

		float nStartDist = hubR * 0.95f;
		float nEndDist = R - 0.003f * g_cfg.forzaScale;

		float nd1x = cx + std::cos(needleRad) * nStartDist;
		float nd1y = cy - std::sin(needleRad) * (nStartDist * aspect);
		float nd2x = cx + std::cos(needleRad) * nEndDist;
		float nd2y = cy - std::sin(needleRad) * (nEndDist * aspect);

		// Jarum Solid Menggunakan DrawScreenLine yang Overlap Padat
		DrawScreenLine(nd1x, nd1y, nd2x, nd2y, 0.0024f * g_cfg.forzaScale, 255, 255, 255, currentAlphaInt);
		// Titik ujung jarum (Glowing Tip)
		float tipDot = 0.0028f * g_cfg.forzaScale;
		GRAPHICS::DRAW_RECT(nd2x, nd2y, tipDot, tipDot * aspect, 255, 60, 40, currentAlphaInt);

		// 5. Digital Readout (KM/H & 3-Digit Speed) di Kanan Bawah Dial
		float kmhX = cx + 0.038f * g_cfg.forzaScale;
		float kmhY = cy + 0.016f * g_cfg.forzaScale;

		int kmhR = g_cfg.textColorR;
		int kmhG = g_cfg.textColorG;
		int kmhB = g_cfg.textColorB;

		if (g_kmhFlashTimer > 0.0f)
		{
			float p = std::clamp(g_kmhFlashTimer / 0.50f, 0.0f, 1.0f);
			kmhR = (int)(g_cfg.textColorR * (1.0f - p) + 255 * p);
			kmhG = (int)(g_cfg.textColorG * (1.0f - p) + 220 * p);
			kmhB = (int)(g_cfg.textColorB * (1.0f - p) + 0 * p);
		}

		DrawMenuText("KM/H", kmhX, kmhY, 0.26f * g_cfg.forzaScale, 4, kmhR, kmhG, kmhB, currentAlphaInt, false);

		// Format 3-Digit Speed (Leading Zeros berwarna Abu-abu Dim)
		char spd3[8];
		sprintf_s(spd3, "%03d", g_displaySpeed);

		float spdFontScale = 0.85f * g_cfg.forzaScale;
		float sDrawX = cx + 0.012f * g_cfg.forzaScale;
		float sDrawY = kmhY + 0.018f * g_cfg.forzaScale;

		for (int d = 0; d < 3; ++d)
		{
			char digitStr[2] = { spd3[d], '\0' };
			bool isLeadingZero = false;
			if (d == 0 && g_displaySpeed < 100) isLeadingZero = true;
			if (d == 1 && g_displaySpeed < 10) isLeadingZero = true;

			int dR = isLeadingZero ? 130 : 255;
			int dG = isLeadingZero ? 135 : 255;
			int dB = isLeadingZero ? 145 : 255;

			if (!isLeadingZero && g_kmhFlashTimer > 0.0f)
			{
				float p = std::clamp(g_kmhFlashTimer / 0.50f, 0.0f, 1.0f);
				dR = (int)(dR * (1.0f - p) + 255 * p);
				dG = (int)(dG * (1.0f - p) + 220 * p);
				dB = (int)(dB * (1.0f - p) + 0 * p);
			}
			else if (!isLeadingZero && g_shimmerTimer > 0.0f)
			{
				float prog = 1.0f - (g_shimmerTimer / 0.45f);
				float shineWave = std::sin(prog * 3.14159f);
				dR = std::min(255, (int)(dR + (255 - dR) * shineWave));
				dG = std::min(255, (int)(dG + (255 - dG) * shineWave * 0.95f));
				dB = std::min(255, (int)(dB + (140 * shineWave)));
			}

			DrawTextElement(digitStr, sDrawX, sDrawY, spdFontScale, 4, dR, dG, dB, currentAlphaInt);
			sDrawX += GetTextWidth(digitStr, spdFontScale, 4) + 0.001f;
		}

		if (g_cfg.showAltitude)
		{
			Ped playerPed = PLAYER::PLAYER_PED_ID();
			Vector3 pCoords = ENTITY::GET_ENTITY_COORDS(playerPed, TRUE);
			char altBuf[32];
			sprintf_s(altBuf, "ALT: %.0fm", pCoords.z);
			float altX = kmhX;
			float altY = kmhY + 0.052f * g_cfg.forzaScale;
			DrawMenuText(altBuf, altX, altY, 0.22f * g_cfg.forzaScale, 4,
			             kmhR, kmhG, kmhB, (int)(currentAlphaInt * 0.85f), false);
		}
	}

	// -------------------------------------------------------------
	// RENDER MENU UTAMA AFOUND MODS
	// -------------------------------------------------------------
	void DrawMenu()
	{
		if (!g_menuOpen) return;

		float mw = g_cfg.menuWidth;
		float itemH = g_cfg.menuItemHeight;
		float mx = 0.990f - (mw * 0.5f);
		float my = 0.08f;

		float headerH = itemH * 2.15f;
		GRAPHICS::DRAW_RECT(mx, my, mw, headerH, 10, 115, 230, 248);
		GRAPHICS::DRAW_RECT(mx, my - (headerH * 0.5f) + 0.0015f, mw, 0.003f, 0, 215, 255, 255);

		std::string headerSubtitle = L("[F12] MENU | 'L' ESP | ']' SPEEDO", "[F12] MENU | 'L' ESP | ']' SPEEDO");
		if (g_menuPage == PAGE_SPEEDOMETER) headerSubtitle = L("MENU SPEEDOMETER & HUD", "SPEEDOMETER & HUD MENU");
		else if (g_menuPage == PAGE_ESP) headerSubtitle = L("MENU ESP (WALLHACK & VISUALS)", "ESP MENU (WALLHACK & VISUALS)");
		else if (g_menuPage == PAGE_PREFERENCES) headerSubtitle = L("MENU PREFERENSI TAMPILAN", "DISPLAY PREFERENCES MENU");
		else if (g_menuPage == PAGE_THEMES) headerSubtitle = L("MENU KELOLA TEMA", "MANAGE THEMES MENU");
		else if (g_menuPage == PAGE_COLORS) headerSubtitle = L("PILIH WARNA TEKS (RGB)", "SELECT TEXT COLOR (RGB)");
		else if (g_menuPage == PAGE_OUTLINE) headerSubtitle = L("PILIH WARNA OUTLINE (RGB)", "SELECT OUTLINE COLOR (RGB)");
		else if (g_menuPage == PAGE_ESP_PRESETS) headerSubtitle = L("MENU PRESET ESP", "ESP PRESETS MENU");

		DrawMenuText("AFOUND MODS", mx - (mw * 0.46f), my - (headerH * 0.38f), g_cfg.menuHeaderScale, 4, 255, 255, 255, 255);
		DrawMenuText(headerSubtitle.c_str(), mx - (mw * 0.46f), my + (headerH * 0.12f), g_cfg.menuTextScale * 0.82f, 0, 205, 235, 255, 235, false);

		float itemStartY = my + (headerH * 0.5f) + (itemH * 0.55f);

		std::vector<std::pair<std::string, std::string>> allItems;

		// 1. BERANDA UTAMA
		if (g_menuPage == PAGE_MAIN)
		{
			allItems.push_back({ L("Bahasa / Language", "Language / Bahasa"), g_cfg.language == 0 ? "< Bahasa Indonesia >" : "< English >" });
			allItems.push_back({ L("> Speedometer & HUD [ ']' ]", "> Speedometer & HUD [ ']' ]"), L("[ BUKA MENU ]", "[ OPEN MENU ]") });
			allItems.push_back({ L("> ESP (Wallhack) [ 'L' ]", "> ESP (Wallhack) [ 'L' ]"), g_cfg.espMaster ? L("[ AKTIF ]", "[ ACTIVE ]") : "[ OFF ]" });
			allItems.push_back({ L("> Preferensi Menu (Ukuran UI)", "> Menu Preferences (UI Scale)"), L("[ PENGATURAN ]", "[ SETTINGS ]") });
			allItems.push_back({ L("> Kelola Tema (Themes)", "> Manage Themes"), L("[ BUKA ]", "[ OPEN ]") });
			allItems.push_back({ L(">> SIMPAN PENGATURAN", ">> SAVE SETTINGS"), "[ ENTER ]" });
			allItems.push_back({ L(">> RESET KE DEFAULT", ">> RESET TO DEFAULT"), "[ RESET ]" });
			allItems.push_back({ L(">> TUTUP MENU", ">> CLOSE MENU"), "[ ESC ]" });
		}
		// 2. SUBMENU SPEEDOMETER
		else if (g_menuPage == PAGE_SPEEDOMETER)
		{
			const char* styles[] = { L("< Minimalis >", "< Minimalist >"), "< Forza Horizon >" };
			allItems.push_back({ L("Gaya Speedometer", "Speedometer Style"), styles[g_cfg.speedometerStyle % 2] });

			char b[64];
			sprintf_s(b, "< %.3f >", g_cfg.forzaPosX); allItems.push_back({ L("Posisi X Forza Dial", "Forza Dial Pos X"), b });
			sprintf_s(b, "< %.3f >", g_cfg.forzaPosY); allItems.push_back({ L("Posisi Y Forza Dial", "Forza Dial Pos Y"), b });
			sprintf_s(b, "< %.2f >", g_cfg.forzaScale); allItems.push_back({ L("Ukuran Forza Dial", "Forza Dial Scale"), b });

			allItems.push_back({ L("Speedometer [ ']' ]", "Speedometer [ ']' ]"), g_cfg.showSpeedo ? L("[ AKTIF ]", "[ ACTIVE ]") : "[ OFF ]" });
			allItems.push_back({ L("Indikator Gigi", "Gear Indicator"), g_cfg.showGear ? "[ ON ]" : "[ OFF ]" });
			allItems.push_back({ L("Nama Kendaraan", "Vehicle Name"), g_cfg.showVehicleName ? "[ ON ]" : "[ OFF ]" });
			allItems.push_back({ L("FPS Counter (Always On)", "FPS Counter (Always On)"), g_cfg.showFps ? "[ ON ]" : "[ OFF ]" });
			allItems.push_back({ L("Jam:Menit In-Game", "In-Game Time"), g_cfg.showClock ? "[ ON ]" : "[ OFF ]" });
			allItems.push_back({ L("Jam Laptop Real Time", "Real Laptop Time"), g_cfg.showRealTime ? "[ ON ]" : "[ OFF ]" });
			allItems.push_back({ L("Info RAM Usage & %", "RAM Usage & %"), g_cfg.showRam ? "[ ON ]" : "[ OFF ]" });
			allItems.push_back({ L("Info CPU Usage & %", "CPU Usage & %"), g_cfg.showCpu ? "[ ON ]" : "[ OFF ]" });
			allItems.push_back({ L("Info GPU Usage & %", "GPU Usage & %"), g_cfg.showGpu ? "[ ON ]" : "[ OFF ]" });
			allItems.push_back({ L("Info Disk Usage & %", "Disk Usage & %"), g_cfg.showDisk ? "[ ON ]" : "[ OFF ]" });
			allItems.push_back({ L("Level Bensin (Fuel)", "Fuel Level"), g_cfg.showFuel ? "[ ON ]" : "[ OFF ]" });
			allItems.push_back({ L("Kesehatan Mesin", "Engine Health"), g_cfg.showDamage ? "[ ON ]" : "[ OFF ]" });
			allItems.push_back({ L("Posisi Ketinggian (ALT)", "Altitude (ALT)"), g_cfg.showAltitude ? "[ ON ]" : "[ OFF ]" });
			allItems.push_back({ L("Kecepatan Karakter (Jalan)", "On-Foot Walking Speed"), g_cfg.showOnFootSpeed ? "[ ON ]" : "[ OFF ]" });

			allItems.push_back({ L("Auto-Hide saat 0 km/h (3s)", "Auto-Hide at 0 km/h (3s)"), g_cfg.autoHideAtZero ? L("[ AKTIF ]", "[ ACTIVE ]") : "[ OFF ]" });

			if (g_cfg.centerSpeedThreshold <= 0)
			{
				allItems.push_back({ L("Pindah Center di Atas", "Move Center Above"), L("[ NONAKTIF ]", "[ DISABLED ]") });
			}
			else
			{
				sprintf_s(b, "< %d km/h >", g_cfg.centerSpeedThreshold);
				allItems.push_back({ L("Pindah Center di Atas", "Move Center Above"), b });
			}

			sprintf_s(b, L("< %d detik >", "< %d sec >"), g_cfg.centerCooldownSec);
			allItems.push_back({ L("Delay Perpindahan Center", "Center Shift Delay"), b });

			sprintf_s(b, "< %.3f >", g_cfg.posX); allItems.push_back({ L("Posisi Layar X (Minimap)", "Minimap Pos X"), b });
			sprintf_s(b, "< %.3f >", g_cfg.posY); allItems.push_back({ L("Posisi Layar Y (Minimap)", "Minimap Pos Y"), b });
			sprintf_s(b, "< %.3f >", g_cfg.centerPosX); allItems.push_back({ L("Posisi Layar X (Center)", "Center Pos X"), b });
			sprintf_s(b, "< %.3f >", g_cfg.centerPosY); allItems.push_back({ L("Posisi Layar Y (Center)", "Center Pos Y"), b });

			sprintf_s(b, "< %.2f >", g_cfg.speedScale); allItems.push_back({ L("Ukuran Speedometer", "Speedometer Scale"), b });
			sprintf_s(b, "< %.2f >", g_cfg.unitScale); allItems.push_back({ L("Ukuran Satuan/Nama", "Unit/Name Scale"), b });
			sprintf_s(b, "< %.2f >", g_cfg.gearScale); allItems.push_back({ L("Ukuran Teks Gigi", "Gear Text Scale"), b });
			sprintf_s(b, "< %.2f >", g_cfg.centerSpeedScale); allItems.push_back({ L("Ukuran Center Speed", "Center Speed Scale"), b });

			sprintf_s(b, "< %d, %d, %d >", g_cfg.textColorR, g_cfg.textColorG, g_cfg.textColorB);
			allItems.push_back({ L("Warna Teks (RGB)", "Text Color (RGB)"), b });
			sprintf_s(b, "< %d, %d, %d >", g_cfg.outlineColorR, g_cfg.outlineColorG, g_cfg.outlineColorB);
			allItems.push_back({ L("Warna Outline (RGB)", "Outline Color (RGB)"), b });

			sprintf_s(b, "< %d >", g_cfg.textOpacity); allItems.push_back({ L("Transparansi (Alpha)", "Transparency (Alpha)"), b });
			sprintf_s(b, "< %d ms >", g_cfg.speedIntervalMs); allItems.push_back({ L("Interval Angka Speed", "Speed Refresh Interval"), b });
			sprintf_s(b, "< %d >", g_cfg.hudTextOpacity); allItems.push_back({ L("Opasitas Teks HUD Top", "Top HUD Text Opacity"), b });
			allItems.push_back({ L("Outline Teks HUD Top", "Top HUD Text Outline"), g_cfg.hudTextOutline ? "[ ON ]" : "[ OFF ]" });

			allItems.push_back({ L("<< KEMBALI KE BERANDA", "<< BACK TO HOME"), "[ ENTER ]" });
		}
		// 3. SUBMENU ESP
		else if (g_menuPage == PAGE_ESP)
		{
			char b[64];
			allItems.push_back({ L("Master ESP [ 'L' ]", "Master ESP [ 'L' ]"), g_cfg.espMaster ? L("[ AKTIF ]", "[ ACTIVE ]") : "[ OFF ]" });

			// A. Visual Box & Kepala
			allItems.push_back({ L("Visual: 2D Box (Kotak)", "Visual: 2D Box"), g_cfg.espBox ? "[ ON ]" : "[ OFF ]" });
			sprintf_s(b, "< %.2f >", g_cfg.espPedBoxScale); allItems.push_back({ L("Skala Ukuran Box NPC", "NPC Box Scale"), b });
			sprintf_s(b, "< %.2f >", g_cfg.espVehBoxWidth); allItems.push_back({ L("Lebar Box Kendaraan", "Vehicle Box Width"), b });
			allItems.push_back({ L("Isi Box Transparan (Fill)", "Transparent Box Fill"), g_cfg.espBoxFill ? "[ ON ]" : "[ OFF ]" });

			// B. Skeleton Tulang & Kualitas
			allItems.push_back({ L("Visual: Skeleton (Tulang)", "Visual: Skeleton"), g_cfg.espSkeleton ? "[ ON ]" : "[ OFF ]" });
			const char* skelQualNames[] = {
				L("< Tinggi (100%) >", "< High (100%) >"),
				L("< Sedang (-50%) >", "< Medium (-50%) >"),
				L("< Hemat (-75%) >", "< Low (-75%) >")
			};
			allItems.push_back({ L("Kualitas Skeleton", "Skeleton Quality"), skelQualNames[g_cfg.espSkeletonQuality % 3] });
			sprintf_s(b, "< %d m >", g_cfg.espSkeletonDist); allItems.push_back({ L("Jarak Render Skeleton", "Skeleton Render Distance"), b });
			allItems.push_back({ L("Layer Skeleton di Depan", "Skeleton Front Layer"), g_cfg.espSkeletonFront ? L("< Depan (Tembus) >", "< Front (X-Ray) >") : "< 3D World >" });
			sprintf_s(b, "< %.2f >", g_cfg.espSkeletonScale); allItems.push_back({ L("Skala Rectangle Skeleton", "Skeleton Rect Scale"), b });

			// C. Kotak Kepala & Ban
			allItems.push_back({ L("Visual: Kotak Kepala NPC", "Visual: NPC Head Box"), g_cfg.espHeadDot ? "[ ON ]" : "[ OFF ]" });
			allItems.push_back({ L("Visual: Kotak Kepala Hewan", "Visual: Animal Head Box"), g_cfg.espAnimalHeadDot ? "[ ON ]" : "[ OFF ]" });
			const char* headColorNames[] = { L("< Putih >", "< White >"), L("< Sesuai Target >", "< Target Color >"), L("< Merah >", "< Red >"), "< Cyan >", L("< Kuning >", "< Yellow >") };
			allItems.push_back({ L("Warna Kotak Kepala", "Head Box Color"), headColorNames[g_cfg.espHeadColorMode % 5] });
			allItems.push_back({ L("Visual: Titik Ban Mobil", "Visual: Wheel Dots"), g_cfg.espWheelDots ? "[ ON ]" : "[ OFF ]" });
			allItems.push_back({ L("Visual: Kecepatan Target", "Visual: Target Speed"), g_cfg.espShowSpeed ? "[ ON ]" : "[ OFF ]" });

			// D. Snaplines
			allItems.push_back({ L("Visual: Snaplines (Garis)", "Visual: Snaplines"), g_cfg.espLines ? "[ ON ]" : "[ OFF ]" });
			const char* originNames[] = { L("< Dari Karakter >", "< From Character >"), L("< Atas Layar >", "< Screen Top >"), L("< Bawah Layar >", "< Screen Bottom >"), L("< Fixed Atas Edge >", "< Fixed Top Edge >"), L("< Fixed Bawah Edge >", "< Fixed Bottom Edge >") };
			allItems.push_back({ L("Mode Asal Snapline", "Snapline Origin"), originNames[g_cfg.espLineOrigin % 5] });
			sprintf_s(b, "< %d m >", g_cfg.espSnaplineDist); allItems.push_back({ L("Jarak Maks Snapline", "Max Snapline Dist"), b });
			allItems.push_back({ L("Tembus Objek (Snapline)", "Snapline Anti-Clip"), g_cfg.espLineAntiClip ? L("< Tembus Pandang >", "< X-Ray / Anti-Clip >") : L("< Dihalangi Objek >", "< Blocked by World >") });
			const char* thickNames[] = { L("< Normal >", "< Normal >"), L("< Tebal >", "< Thick >"), L("< Sangat Tebal >", "< Very Thick >") };
			allItems.push_back({ L("Tebal Garis & Box", "Line & Box Thickness"), thickNames[std::clamp(g_cfg.espLineThickness, 1, 3) - 1] });

			// E. Preset & Warna
			const char* presetNames[] = { "< Default >", "< Cyberpunk >", "< Tactical >", "< Monochrome >", "< Vaporwave >", L("< Kustom RGB >", "< Custom RGB >") };
			allItems.push_back({ L("Preset Warna ESP", "ESP Color Preset"), presetNames[g_cfg.espColorPreset % 6] });
			sprintf_s(b, "< %.2f >", g_cfg.espTextScale); allItems.push_back({ L("Ukuran Teks ESP", "ESP Text Scale"), b });
			const char* textPosNames[] = { L("< Di Atas Box >", "< Above Box >"), L("< Di Bawah Box >", "< Below Box >") };
			allItems.push_back({ L("Posisi Teks Info Target", "Target Text Position"), textPosNames[g_cfg.espTextPosition % 2] });
			sprintf_s(b, "< %.2f >", g_cfg.espCounterScale); allItems.push_back({ L("Ukuran Teks Info Jumlah", "Entity Counter Scale"), b });
			sprintf_s(b, "< %d >", g_cfg.espRectOpacity); allItems.push_back({ L("Opasitas ESP Rectangle", "ESP Rect Opacity"), b });
			sprintf_s(b, "< %d >", g_cfg.espTextOpacity); allItems.push_back({ L("Opasitas Teks ESP", "ESP Text Opacity"), b });

			// F. Crosshair & Aim Combat
			allItems.push_back({ L("ESP Crosshair", "ESP Crosshair"), g_cfg.espCrosshair ? "[ ON ]" : "[ OFF ]" });
			const char* chTypes[] = { L("< Titik >", "< Dot >"), L("< Tanda Plus (+) >", "< Plus (+) >"), L("< Kotak >", "< Box >"), "< Gap Cross >", "< Chevron (^) >" };
			allItems.push_back({ L("Bentuk Crosshair", "Crosshair Shape"), chTypes[g_cfg.espCrosshairType % 5] });
			sprintf_s(b, "< %.3f >", g_cfg.espCrosshairSize); allItems.push_back({ L("Ukuran Crosshair", "Crosshair Size"), b });
			const char* chColors[] = { L("< Hijau >", "< Green >"), L("< Merah >", "< Red >"), L("< Putih >", "< White >"), "< Cyan >", L("< Kuning >", "< Yellow >"), L("< Ungu >", "< Purple >") };
			allItems.push_back({ L("Warna Crosshair", "Crosshair Color"), chColors[g_cfg.espCrosshairColor % 6] });

			allItems.push_back({ L("ESP Aim Circle (Center)", "ESP Aim Circle (Center)"), g_cfg.espAimCircle ? "[ ON ]" : "[ OFF ]" });
			sprintf_s(b, "< %.3f >", g_cfg.espAimCircleSize); allItems.push_back({ L("Ukuran Aim Circle", "Aim Circle Size"), b });
			sprintf_s(b, "< %.2f >", g_cfg.espAimCircleRectScale); allItems.push_back({ L("Skala Rect Circle Aim", "Aim Circle Rect Scale"), b });
			allItems.push_back({ L("Auto-Hide Aim (Saat Membidik)", "Auto-Hide Aim (On Aim Only)"), g_cfg.espAimAutoHide ? L("[ AKTIF ]", "[ ACTIVE ]") : "[ OFF ]" });
			allItems.push_back({ L("ESP Kill Satisfying", "ESP Kill Banner"), g_cfg.espKillSatisfying ? "[ ON ]" : "[ OFF ]" });
			sprintf_s(b, "< %.2f >", g_cfg.espKillBannerScale); allItems.push_back({ L("Ukuran Banner Kill (Mengecil)", "Kill Banner Small Scale"), b });
			const char* killSoundNames[] = { "< OFF >", "< Challenge Unlocked >", "< Race Placed >", "< Weapon Purchase >", "< Medal Up >", "< Checkpoint >" };
			allItems.push_back({ L("Sound Effect Killed", "Kill Sound Effect"), killSoundNames[g_cfg.espKillSoundMode % 6] });

			// G. Info Target & Damage
			allItems.push_back({ L("Visual: Bar Darah NPC", "Visual: NPC Health Bar"), g_cfg.espHealth ? "[ ON ]" : "[ OFF ]" });
			allItems.push_back({ L("Visual: Bar Kerusakan Kendaraan", "Visual: Vehicle Health Bar"), g_cfg.espVehHealth ? "[ ON ]" : "[ OFF ]" });
			allItems.push_back({ L("Visual: Info Armor", "Visual: Armor Bar"), g_cfg.espArmor ? "[ ON ]" : "[ OFF ]" });
			allItems.push_back({ L("Visual: Info Jarak", "Visual: Distance Info"), g_cfg.espDistance ? "[ ON ]" : "[ OFF ]" });
			allItems.push_back({ L("Visual: Label / Tipe Target", "Visual: Target Type Label"), g_cfg.espName ? "[ ON ]" : "[ OFF ]" });
			allItems.push_back({ L("Visual: Senjata / Item Target", "Visual: Held Weapon / Item"), g_cfg.espShowWeapon ? "[ ON ]" : "[ OFF ]" });
			allItems.push_back({ L("Visual: Teks Floating Damage", "Visual: Floating Damage Text"), g_cfg.espDamageText ? "[ ON ]" : "[ OFF ]" });
			allItems.push_back({ L("Visual: ESP Edge (Luar Layar)", "Visual: ESP Edge (Off-Screen)"), g_cfg.espEdge ? "[ ON ]" : "[ OFF ]" });

			// H. Performa & Transisi
			const char* fpsNames[] = { "< Normal (Max) >", "< 60 FPS >", "< 30 FPS >", "< 15 FPS >" };
			allItems.push_back({ L("Framerate Refresh ESP", "ESP Refresh Framerate"), fpsNames[g_cfg.espFramerateMode % 4] });
			const char* smoothNames[] = { "< OFF (Direct) >", L("< Ringan (Fast) >", "< Fast >"), L("< Halus (Normal) >", "< Smooth >"), "< Ultra Smooth >" };
			allItems.push_back({ L("Velocity Smoothing ESP", "ESP Velocity Smoothing"), smoothNames[g_cfg.espVelocitySmooth % 4] });
			const char* transNames[] = { L("< OFF (Kaku) >", "< OFF (Instant) >"), "< Fade In/Out >", "< Bouncing Pop >", "< Elastic Expand >" };
			allItems.push_back({ L("Animasi Transisi ESP", "ESP Transition Animation"), transNames[g_cfg.espTransitionMode % 4] });
			sprintf_s(b, "< %.2f s >", g_cfg.espAnimDuration); allItems.push_back({ L("Durasi Animasi Transisi", "Anim Transition Duration"), b });
			allItems.push_back({ L("Fitur Fokus (Center Layar)", "Focus Mode (Center Screen)"), g_cfg.espFocus ? L("[ AKTIF ]", "[ ACTIVE ]") : "[ OFF ]" });
			sprintf_s(b, "< %.2f >", g_cfg.espFocusWidth); allItems.push_back({ L("Lebar Fokus Layar", "Focus Screen Width"), b });
			sprintf_s(b, "< %.2f >", g_cfg.espFocusHeight); allItems.push_back({ L("Tinggi Fokus Layar", "Focus Screen Height"), b });

			// I. Filter Target
			allItems.push_back({ L("Target: NPC Global (Warga)", "Target: Friendly / Pedestrians"), g_cfg.espFriendly ? "[ ON ]" : "[ OFF ]" });
			allItems.push_back({ L("Target: NPC Musuh (Senjata/Misi)", "Target: Enemies"), g_cfg.espEnemy ? "[ ON ]" : "[ OFF ]" });
			allItems.push_back({ L("Target: Kendaraan (Vehicles)", "Target: Vehicles"), g_cfg.espVehicles ? "[ ON ]" : "[ OFF ]" });
			allItems.push_back({ L("Target: Hewan (Animals)", "Target: Animals"), g_cfg.espAnimals ? "[ ON ]" : "[ OFF ]" });
			allItems.push_back({ L("Target: Item / Pickups", "Target: Items / Pickups"), g_cfg.espItems ? "[ ON ]" : "[ OFF ]" });

			allItems.push_back({ L("Display Info Jumlah Entitas", "Display Entity Counters"), g_cfg.espShowCounters ? "[ ON ]" : "[ OFF ]" });
			allItems.push_back({ L("Abaikan Target Mati", "Ignore Dead Targets"), g_cfg.espIgnoreDead ? "[ ON ]" : "[ OFF ]" });
			allItems.push_back({ L("Sembunyikan saat Tab Senjata & Q", "Hide on Weapon Wheel & Q"), g_cfg.espHideOnWeaponWheel ? "[ ON ]" : "[ OFF ]" });

			sprintf_s(b, "< %d m >", g_cfg.espMaxDistance); allItems.push_back({ L("Maksimal Jarak ESP", "Max ESP Distance"), b });

			allItems.push_back({ L("> Kelola Preset ESP", "> Manage ESP Presets"), L("[ BUKA ]", "[ OPEN ]") });
			allItems.push_back({ L("<< KEMBALI KE BERANDA", "<< BACK TO HOME"), "[ ENTER ]" });
		}
		// 4. SUBMENU PREFERENSI
		else if (g_menuPage == PAGE_PREFERENCES)
		{
			char b[64];
			sprintf_s(b, "< %.3f >", g_cfg.menuWidth); allItems.push_back({ L("Lebar Menu (Width)", "Menu Width"), b });
			sprintf_s(b, "< %.3f >", g_cfg.menuItemHeight); allItems.push_back({ L("Tinggi Bar Item (Height)", "Menu Item Height"), b });
			sprintf_s(b, "< %.3f >", g_cfg.menuTextScale); allItems.push_back({ L("Ukuran Teks Item", "Menu Text Scale"), b });
			sprintf_s(b, "< %.3f >", g_cfg.menuHeaderScale); allItems.push_back({ L("Ukuran Judul Header", "Header Title Scale"), b });
			allItems.push_back({ L("<< KEMBALI KE BERANDA", "<< BACK TO HOME"), "[ ENTER ]" });
		}
		// 5. SUBMENU WARNA TEKS / OUTLINE
		else if (g_menuPage == PAGE_COLORS || g_menuPage == PAGE_OUTLINE)
		{
			int r = (g_menuPage == PAGE_COLORS) ? g_cfg.textColorR : g_cfg.outlineColorR;
			int g = (g_menuPage == PAGE_COLORS) ? g_cfg.textColorG : g_cfg.outlineColorG;
			int b = (g_menuPage == PAGE_COLORS) ? g_cfg.textColorB : g_cfg.outlineColorB;

			char buf[32];
			sprintf_s(buf, "< %d >", r); allItems.push_back({ L("Komponen Merah (R)", "Red Component (R)"), buf });
			sprintf_s(buf, "< %d >", g); allItems.push_back({ L("Komponen Hijau (G)", "Green Component (G)"), buf });
			sprintf_s(buf, "< %d >", b); allItems.push_back({ L("Komponen Biru (B)", "Blue Component (B)"), buf });
			allItems.push_back({ L("<< KEMBALI KE MENU SPEEDOMETER", "<< BACK TO SPEEDOMETER MENU"), "[ ENTER ]" });
		}
		// 6. SUBMENU THEMES
		else if (g_menuPage == PAGE_THEMES)
		{
			allItems.push_back({ L("+ SIMPAN SEBAGAI TEMA BARU", "+ SAVE AS NEW THEME"), "[ ENTER ]" });
			for (size_t i = 0; i < g_themeFiles.size(); ++i)
			{
				allItems.push_back({ g_themeFiles[i], L("[ENTER=Load / DEL=Hapus]", "[ENTER=Load / DEL=Delete]") });
			}
			allItems.push_back({ L("<< KEMBALI KE BERANDA", "<< BACK TO HOME"), "[ ENTER ]" });
		}
		// 7. SUBMENU PRESET ESP
		else if (g_menuPage == PAGE_ESP_PRESETS)
		{
			allItems.push_back({ L("+ SIMPAN SEBAGAI PRESET BARU", "+ SAVE AS NEW PRESET"), "[ ENTER ]" });
			for (size_t i = 0; i < g_espPresetFiles.size(); ++i)
			{
				allItems.push_back({ g_espPresetFiles[i], L("[ENTER=Load / DEL=Hapus]", "[ENTER=Load / DEL=Delete]") });
			}
			allItems.push_back({ L("<< KEMBALI KE SUBMENU ESP", "<< BACK TO ESP MENU"), "[ ENTER ]" });
		}

		// Viewport Window Scrolling
		const int maxVisible = 12;
		int totalItems = (int)allItems.size();

		if (g_menuIndex < g_menuScrollOffset)
			g_menuScrollOffset = g_menuIndex;
		if (g_menuIndex >= g_menuScrollOffset + maxVisible)
			g_menuScrollOffset = g_menuIndex - maxVisible + 1;
		if (g_menuScrollOffset < 0) g_menuScrollOffset = 0;

		int visibleCount = std::min(maxVisible, totalItems - g_menuScrollOffset);
		float totalBodyH = (float)visibleCount * itemH + 0.038f;

		GRAPHICS::DRAW_RECT(mx, itemStartY + (totalBodyH * 0.5f) - (itemH * 0.5f), mw, totalBodyH, 12, 16, 24, 244);

		for (int v = 0; v < visibleCount; ++v)
		{
			int idx = g_menuScrollOffset + v;
			float iy = itemStartY + (float)v * itemH;
			bool selected = (idx == g_menuIndex);

			if (selected)
			{
				GRAPHICS::DRAW_RECT(mx, iy, mw - 0.008f, itemH - 0.002f, 255, 255, 255, 240);
				GRAPHICS::DRAW_RECT(mx - (mw * 0.5f) + 0.004f, iy, 0.004f, itemH - 0.002f, 0, 180, 255, 255);
				DrawMenuText(allItems[idx].first.c_str(), mx - (mw * 0.46f) + 0.006f, iy - (itemH * 0.36f), g_cfg.menuTextScale, 4, 14, 20, 28, 255, false);
				DrawMenuText(allItems[idx].second.c_str(), mx + (mw * 0.02f), iy - (itemH * 0.36f), g_cfg.menuTextScale * 0.94f, 4, 0, 100, 220, 255, false);
			}
			else
			{
				DrawMenuText(allItems[idx].first.c_str(), mx - (mw * 0.46f), iy - (itemH * 0.36f), g_cfg.menuTextScale, 4, 235, 240, 245, 255, false);
				DrawMenuText(allItems[idx].second.c_str(), mx + (mw * 0.02f), iy - (itemH * 0.36f), g_cfg.menuTextScale * 0.94f, 4, 155, 180, 205, 235, false);
			}
		}

		if (!g_statusMsg.empty() && (GetTickCount64() - g_statusMsgTime < 3500))
		{
			float statusY = itemStartY + totalBodyH;
			GRAPHICS::DRAW_RECT(mx, statusY, mw, itemH, 0, 165, 95, 245);
			DrawMenuText(g_statusMsg.c_str(), mx - (mw * 0.46f), statusY - (itemH * 0.35f), g_cfg.menuTextScale * 0.85f, 4, 255, 255, 255, 255);
		}

		float footerY = itemStartY + totalBodyH + (itemH * 0.70f);
		char footerBuf[160];
		sprintf_s(footerBuf, L("[%d/%d]  [^/v] Scroll (Tahan Cepat)  [</>] Ubah  [ENTER] Pilih",
		                       "[%d/%d]  [^/v] Scroll (Fast Hold)   [</>] Change  [ENTER] Select"),
		          g_menuIndex + 1, totalItems);
		DrawMenuText(footerBuf, mx - (mw * 0.46f), footerY, g_cfg.menuTextScale * 0.78f, 0, 175, 195, 220, 235, false);
	}

	// -------------------------------------------------------------
	// LOGIKA NAVIGASI MENU
	// -------------------------------------------------------------
	void HandleMenuKeyPress(DWORD key)
	{
		// 1. BERANDA UTAMA
		if (g_menuPage == PAGE_MAIN)
		{
			const int totalItems = 8;
			if (key == VK_UP)
			{
				g_menuIndex = (g_menuIndex - 1 + totalItems) % totalItems;
				PlayFrontendSound("NAV_UP_DOWN");
			}
			else if (key == VK_DOWN)
			{
				g_menuIndex = (g_menuIndex + 1) % totalItems;
				PlayFrontendSound("NAV_UP_DOWN");
			}
			else if (key == VK_LEFT || key == VK_RIGHT)
			{
				if (g_menuIndex == 0)
				{
					g_cfg.language = (g_cfg.language + 1) % 2;
					SaveConfig();
					PlayFrontendSound("NAV_LEFT_RIGHT");
				}
				else if (key == VK_RIGHT)
				{
					switch (g_menuIndex)
					{
					case 1:
						g_menuPage = PAGE_SPEEDOMETER;
						g_menuIndex = 0;
						g_menuScrollOffset = 0;
						PlayFrontendSound("SELECT");
						break;
					case 2:
						g_menuPage = PAGE_ESP;
						g_menuIndex = 0;
						g_menuScrollOffset = 0;
						PlayFrontendSound("SELECT");
						break;
					case 3:
						g_menuPage = PAGE_PREFERENCES;
						g_menuIndex = 0;
						g_menuScrollOffset = 0;
						PlayFrontendSound("SELECT");
						break;
					case 4:
						ScanThemes();
						g_menuPage = PAGE_THEMES;
						g_menuIndex = 0;
						g_menuScrollOffset = 0;
						PlayFrontendSound("SELECT");
						break;
					}
				}
			}
			else if (key == VK_RETURN)
			{
				switch (g_menuIndex)
				{
				case 0:
					g_cfg.language = (g_cfg.language + 1) % 2;
					SaveConfig();
					PlayFrontendSound("NAV_LEFT_RIGHT");
					break;
				case 1:
					g_menuPage = PAGE_SPEEDOMETER;
					g_menuIndex = 0;
					g_menuScrollOffset = 0;
					PlayFrontendSound("SELECT");
					break;
				case 2:
					g_menuPage = PAGE_ESP;
					g_menuIndex = 0;
					g_menuScrollOffset = 0;
					PlayFrontendSound("SELECT");
					break;
				case 3:
					g_menuPage = PAGE_PREFERENCES;
					g_menuIndex = 0;
					g_menuScrollOffset = 0;
					PlayFrontendSound("SELECT");
					break;
				case 4:
					ScanThemes();
					g_menuPage = PAGE_THEMES;
					g_menuIndex = 0;
					g_menuScrollOffset = 0;
					PlayFrontendSound("SELECT");
					break;
				case 5:
					SaveConfig();
					SetStatusMessage(L("Pengaturan Berhasil Disimpan ke Speedometer.ini!", "Settings saved to Speedometer.ini!"));
					PlayFrontendSound("SELECT");
					break;
				case 6:
					ResetDefaults();
					SetStatusMessage(L("Pengaturan Berhasil Direset ke Default!", "Settings reset to defaults!"));
					PlayFrontendSound("SELECT");
					break;
				case 7:
					g_menuOpen = false;
					SaveConfig();
					PlayFrontendSound("BACK");
					break;
				}
			}
			else if (key == VK_ESCAPE || key == VK_BACK)
			{
				g_menuOpen = false;
				SaveConfig();
				PlayFrontendSound("BACK");
			}
		}
		// 2. SUBMENU SPEEDOMETER
		else if (g_menuPage == PAGE_SPEEDOMETER)
		{
			const int totalItems = 36;
			if (key == VK_UP)
			{
				g_menuIndex = (g_menuIndex - 1 + totalItems) % totalItems;
				PlayFrontendSound("NAV_UP_DOWN");
			}
			else if (key == VK_DOWN)
			{
				g_menuIndex = (g_menuIndex + 1) % totalItems;
				PlayFrontendSound("NAV_UP_DOWN");
			}
			else if (key == VK_LEFT || key == VK_RIGHT)
			{
				bool inc = (key == VK_RIGHT);
				switch (g_menuIndex)
				{
				case 0: g_cfg.speedometerStyle = (g_cfg.speedometerStyle + 1) % 2; break;
				case 1: g_cfg.forzaPosX = std::clamp(g_cfg.forzaPosX + (inc ? 0.005f : -0.005f), 0.100f, 0.980f); break;
				case 2: g_cfg.forzaPosY = std::clamp(g_cfg.forzaPosY + (inc ? 0.005f : -0.005f), 0.100f, 0.980f); break;
				case 3: g_cfg.forzaScale = std::clamp(g_cfg.forzaScale + (inc ? 0.05f : -0.05f), 0.50f, 1.80f); break;
				case 4: g_cfg.showSpeedo = !g_cfg.showSpeedo; break;
				case 5: g_cfg.showGear = !g_cfg.showGear; break;
				case 6: g_cfg.showVehicleName = !g_cfg.showVehicleName; break;
				case 7: g_cfg.showFps = !g_cfg.showFps; break;
				case 8: g_cfg.showClock = !g_cfg.showClock; break;
				case 9: g_cfg.showRealTime = !g_cfg.showRealTime; break;
				case 10: g_cfg.showRam = !g_cfg.showRam; break;
				case 11: g_cfg.showCpu = !g_cfg.showCpu; break;
				case 12: g_cfg.showGpu = !g_cfg.showGpu; break;
				case 13: g_cfg.showDisk = !g_cfg.showDisk; break;
				case 14: g_cfg.showFuel = !g_cfg.showFuel; break;
				case 15: g_cfg.showDamage = !g_cfg.showDamage; break;
				case 16: g_cfg.showAltitude = !g_cfg.showAltitude; break;
				case 17: g_cfg.showOnFootSpeed = !g_cfg.showOnFootSpeed; break;
				case 18: g_cfg.autoHideAtZero = !g_cfg.autoHideAtZero; break;
				case 19:
					if (inc)
					{
						if (g_cfg.centerSpeedThreshold < 30) g_cfg.centerSpeedThreshold = 30;
						else g_cfg.centerSpeedThreshold = std::min(160, g_cfg.centerSpeedThreshold + 5);
					}
					else
					{
						if (g_cfg.centerSpeedThreshold <= 30) g_cfg.centerSpeedThreshold = 0;
						else g_cfg.centerSpeedThreshold -= 5;
					}
					break;
				case 20: g_cfg.centerCooldownSec = std::clamp(g_cfg.centerCooldownSec + (inc ? 1 : -1), 1, 15); break;
				case 21: g_cfg.posX = std::clamp(g_cfg.posX + (inc ? 0.005f : -0.005f), 0.005f, 0.900f); break;
				case 22: g_cfg.posY = std::clamp(g_cfg.posY + (inc ? 0.005f : -0.005f), 0.050f, 0.950f); break;
				case 23: g_cfg.centerPosX = std::clamp(g_cfg.centerPosX + (inc ? 0.005f : -0.005f), 0.100f, 0.900f); break;
				case 24: g_cfg.centerPosY = std::clamp(g_cfg.centerPosY + (inc ? 0.005f : -0.005f), 0.100f, 0.950f); break;
				case 25: g_cfg.speedScale = std::clamp(g_cfg.speedScale + (inc ? 0.02f : -0.02f), 0.30f, 1.40f); break;
				case 26: g_cfg.unitScale = std::clamp(g_cfg.unitScale + (inc ? 0.02f : -0.02f), 0.18f, 0.80f); break;
				case 27: g_cfg.gearScale = std::clamp(g_cfg.gearScale + (inc ? 0.02f : -0.02f), 0.30f, 1.40f); break;
				case 28: g_cfg.centerSpeedScale = std::clamp(g_cfg.centerSpeedScale + (inc ? 0.02f : -0.02f), 0.40f, 1.60f); break;
				case 29:
					if (inc) { g_menuPage = PAGE_COLORS; g_menuIndex = 0; g_menuScrollOffset = 0; }
					break;
				case 30:
					if (inc) { g_menuPage = PAGE_OUTLINE; g_menuIndex = 0; g_menuScrollOffset = 0; }
					break;
				case 31: g_cfg.textOpacity = std::clamp(g_cfg.textOpacity + (inc ? 15 : -15), 30, 255); break;
				case 32: g_cfg.speedIntervalMs = std::clamp(g_cfg.speedIntervalMs + (inc ? 10 : -10), 20, 500); break;
				case 33: g_cfg.hudTextOpacity = std::clamp(g_cfg.hudTextOpacity + (inc ? 15 : -15), 30, 255); break;
				case 34: g_cfg.hudTextOutline = !g_cfg.hudTextOutline; break;
				}
				SaveConfig();
				PlayFrontendSound("NAV_LEFT_RIGHT");
			}
			else if (key == VK_RETURN)
			{
				switch (g_menuIndex)
				{
				case 0: g_cfg.speedometerStyle = (g_cfg.speedometerStyle + 1) % 2; break;
				case 4: g_cfg.showSpeedo = !g_cfg.showSpeedo; break;
				case 5: g_cfg.showGear = !g_cfg.showGear; break;
				case 6: g_cfg.showVehicleName = !g_cfg.showVehicleName; break;
				case 7: g_cfg.showFps = !g_cfg.showFps; break;
				case 8: g_cfg.showClock = !g_cfg.showClock; break;
				case 9: g_cfg.showRealTime = !g_cfg.showRealTime; break;
				case 10: g_cfg.showRam = !g_cfg.showRam; break;
				case 11: g_cfg.showCpu = !g_cfg.showCpu; break;
				case 12: g_cfg.showGpu = !g_cfg.showGpu; break;
				case 13: g_cfg.showDisk = !g_cfg.showDisk; break;
				case 14: g_cfg.showFuel = !g_cfg.showFuel; break;
				case 15: g_cfg.showDamage = !g_cfg.showDamage; break;
				case 16: g_cfg.showAltitude = !g_cfg.showAltitude; break;
				case 17: g_cfg.showOnFootSpeed = !g_cfg.showOnFootSpeed; break;
				case 18: g_cfg.autoHideAtZero = !g_cfg.autoHideAtZero; break;
				case 19:
					g_cfg.centerSpeedThreshold = (g_cfg.centerSpeedThreshold > 0) ? 0 : 70;
					break;
				case 29: g_menuPage = PAGE_COLORS; g_menuIndex = 0; g_menuScrollOffset = 0; break;
				case 30: g_menuPage = PAGE_OUTLINE; g_menuIndex = 0; g_menuScrollOffset = 0; break;
				case 34: g_cfg.hudTextOutline = !g_cfg.hudTextOutline; break;
				case 35:
					g_menuPage = PAGE_MAIN;
					g_menuIndex = 0;
					g_menuScrollOffset = 0;
					PlayFrontendSound("BACK");
					break;
				}
				SaveConfig();
			}
			else if (key == VK_ESCAPE || key == VK_BACK)
			{
				g_menuPage = PAGE_MAIN;
				g_menuIndex = 0;
				g_menuScrollOffset = 0;
				PlayFrontendSound("BACK");
			}
		}
		// 3. SUBMENU ESP
		else if (g_menuPage == PAGE_ESP)
		{
			const int totalItems = 63;
			if (key == VK_UP)
			{
				g_menuIndex = (g_menuIndex - 1 + totalItems) % totalItems;
				PlayFrontendSound("NAV_UP_DOWN");
			}
			else if (key == VK_DOWN)
			{
				g_menuIndex = (g_menuIndex + 1) % totalItems;
				PlayFrontendSound("NAV_UP_DOWN");
			}
			else if (key == VK_LEFT || key == VK_RIGHT)
			{
				bool inc = (key == VK_RIGHT);
				switch (g_menuIndex)
				{
				case 0: g_cfg.espMaster = !g_cfg.espMaster; g_espMasterAnimProgress = 0.0f; g_entityAnimMap.clear(); break;
				case 1: g_cfg.espBox = !g_cfg.espBox; break;
				case 2: g_cfg.espPedBoxScale = std::clamp(g_cfg.espPedBoxScale + (inc ? 0.05f : -0.05f), 0.50f, 2.00f); break;
				case 3: g_cfg.espVehBoxWidth = std::clamp(g_cfg.espVehBoxWidth + (inc ? 0.05f : -0.05f), 0.50f, 2.50f); break;
				case 4: g_cfg.espBoxFill = !g_cfg.espBoxFill; break;
				case 5: g_cfg.espSkeleton = !g_cfg.espSkeleton; break;
				case 6: g_cfg.espSkeletonQuality = (g_cfg.espSkeletonQuality + (inc ? 1 : 2)) % 3; break;
				case 7: g_cfg.espSkeletonDist = std::clamp(g_cfg.espSkeletonDist + (inc ? 15 : -15), 25, 350); break;
				case 8: g_cfg.espSkeletonFront = !g_cfg.espSkeletonFront; break;
				case 9: g_cfg.espSkeletonScale = std::clamp(g_cfg.espSkeletonScale + (inc ? 0.05f : -0.05f), 0.10f, 2.50f); break;
				case 10: g_cfg.espHeadDot = !g_cfg.espHeadDot; break;
				case 11: g_cfg.espAnimalHeadDot = !g_cfg.espAnimalHeadDot; break;
				case 12: g_cfg.espHeadColorMode = (g_cfg.espHeadColorMode + (inc ? 1 : 4)) % 5; break;
				case 13: g_cfg.espWheelDots = !g_cfg.espWheelDots; break;
				case 14: g_cfg.espShowSpeed = !g_cfg.espShowSpeed; break;
				case 15: g_cfg.espLines = !g_cfg.espLines; break;
				case 16: g_cfg.espLineOrigin = (g_cfg.espLineOrigin + (inc ? 1 : 4)) % 5; break;
				case 17: g_cfg.espSnaplineDist = std::clamp(g_cfg.espSnaplineDist + (inc ? 25 : -25), 50, 500); break;
				case 18: g_cfg.espLineAntiClip = !g_cfg.espLineAntiClip; break;
				case 19: g_cfg.espLineThickness = std::clamp(g_cfg.espLineThickness + (inc ? 1 : -1), 1, 3); break;
				case 20: g_cfg.espColorPreset = (g_cfg.espColorPreset + (inc ? 1 : 5)) % 6; break;
				case 21: g_cfg.espTextScale = std::clamp(g_cfg.espTextScale + (inc ? 0.02f : -0.02f), 0.14f, 0.38f); break;
				case 22: g_cfg.espTextPosition = (g_cfg.espTextPosition + 1) % 2; break;
				case 23: g_cfg.espCounterScale = std::clamp(g_cfg.espCounterScale + (inc ? 0.02f : -0.02f), 0.30f, 0.60f); break;
				case 24: g_cfg.espRectOpacity = std::clamp(g_cfg.espRectOpacity + (inc ? 15 : -15), 15, 255); break;
				case 25: g_cfg.espTextOpacity = std::clamp(g_cfg.espTextOpacity + (inc ? 15 : -15), 15, 255); break;
				case 26: g_cfg.espCrosshair = !g_cfg.espCrosshair; break;
				case 27: g_cfg.espCrosshairType = (g_cfg.espCrosshairType + (inc ? 1 : 4)) % 5; break;
				case 28: g_cfg.espCrosshairSize = std::clamp(g_cfg.espCrosshairSize + (inc ? 0.002f : -0.002f), 0.004f, 0.030f); break;
				case 29: g_cfg.espCrosshairColor = (g_cfg.espCrosshairColor + (inc ? 1 : 5)) % 6; break;
				case 30: g_cfg.espAimCircle = !g_cfg.espAimCircle; break;
				case 31: g_cfg.espAimCircleSize = std::clamp(g_cfg.espAimCircleSize + (inc ? 0.002f : -0.002f), 0.012f, 0.050f); break;
				case 32: g_cfg.espAimCircleRectScale = std::clamp(g_cfg.espAimCircleRectScale + (inc ? 0.05f : -0.05f), 0.40f, 2.50f); break;
				case 33: g_cfg.espAimAutoHide = !g_cfg.espAimAutoHide; break;
				case 34: g_cfg.espKillSatisfying = !g_cfg.espKillSatisfying; break;
				case 35: g_cfg.espKillBannerScale = std::clamp(g_cfg.espKillBannerScale + (inc ? 0.02f : -0.02f), 0.20f, 0.60f); break;
				case 36: g_cfg.espKillSoundMode = (g_cfg.espKillSoundMode + (inc ? 1 : 5)) % 6; PlayKillSound(); break;
				case 37: g_cfg.espHealth = !g_cfg.espHealth; break;
				case 38: g_cfg.espVehHealth = !g_cfg.espVehHealth; break;
				case 39: g_cfg.espArmor = !g_cfg.espArmor; break;
				case 40: g_cfg.espDistance = !g_cfg.espDistance; break;
				case 41: g_cfg.espName = !g_cfg.espName; break;
				case 42: g_cfg.espShowWeapon = !g_cfg.espShowWeapon; break;
				case 43: g_cfg.espDamageText = !g_cfg.espDamageText; break;
				case 44: g_cfg.espEdge = !g_cfg.espEdge; break;
				case 45: g_cfg.espFramerateMode = (g_cfg.espFramerateMode + (inc ? 1 : 3)) % 4; break;
				case 46: g_cfg.espVelocitySmooth = (g_cfg.espVelocitySmooth + (inc ? 1 : 3)) % 4; break;
				case 47: g_cfg.espTransitionMode = (g_cfg.espTransitionMode + (inc ? 1 : 3)) % 4; g_espMasterAnimProgress = 0.0f; g_entityAnimMap.clear(); break;
				case 48: g_cfg.espAnimDuration = std::clamp(g_cfg.espAnimDuration + (inc ? 0.05f : -0.05f), 0.05f, 1.50f); break;
				case 49: g_cfg.espFocus = !g_cfg.espFocus; break;
				case 50: g_cfg.espFocusWidth = std::clamp(g_cfg.espFocusWidth + (inc ? 0.05f : -0.05f), 0.10f, 0.90f); break;
				case 51: g_cfg.espFocusHeight = std::clamp(g_cfg.espFocusHeight + (inc ? 0.05f : -0.05f), 0.10f, 0.90f); break;
				case 52: g_cfg.espFriendly = !g_cfg.espFriendly; break;
				case 53: g_cfg.espEnemy = !g_cfg.espEnemy; break;
				case 54: g_cfg.espVehicles = !g_cfg.espVehicles; break;
				case 55: g_cfg.espAnimals = !g_cfg.espAnimals; break;
				case 56: g_cfg.espItems = !g_cfg.espItems; break;
				case 57: g_cfg.espShowCounters = !g_cfg.espShowCounters; break;
				case 58: g_cfg.espIgnoreDead = !g_cfg.espIgnoreDead; break;
				case 59: g_cfg.espHideOnWeaponWheel = !g_cfg.espHideOnWeaponWheel; break;
				case 60:
					{
						int delta = inc ? 25 : -25;
						g_cfg.espMaxDistance = std::clamp(g_cfg.espMaxDistance + delta, 25, 500);
					}
					break;
				case 61:
					if (inc)
					{
						ScanEspPresets();
						g_menuPage = PAGE_ESP_PRESETS;
						g_menuIndex = 0;
						g_menuScrollOffset = 0;
						PlayFrontendSound("SELECT");
					}
					break;
				case 62:
					if (inc)
					{
						g_menuPage = PAGE_MAIN;
						g_menuIndex = 2;
						g_menuScrollOffset = 0;
						PlayFrontendSound("BACK");
					}
					break;
				}
				SaveConfig();
				PlayFrontendSound("NAV_LEFT_RIGHT");
			}
			else if (key == VK_RETURN)
			{
				switch (g_menuIndex)
				{
				case 0: g_cfg.espMaster = !g_cfg.espMaster; g_espMasterAnimProgress = 0.0f; g_entityAnimMap.clear(); break;
				case 1: g_cfg.espBox = !g_cfg.espBox; break;
				case 4: g_cfg.espBoxFill = !g_cfg.espBoxFill; break;
				case 5: g_cfg.espSkeleton = !g_cfg.espSkeleton; break;
				case 6: g_cfg.espSkeletonQuality = (g_cfg.espSkeletonQuality + 1) % 3; break;
				case 7: g_cfg.espSkeletonDist = (g_cfg.espSkeletonDist >= 350) ? 50 : g_cfg.espSkeletonDist + 50; break;
				case 8: g_cfg.espSkeletonFront = !g_cfg.espSkeletonFront; break;
				case 9: g_cfg.espSkeletonScale = (g_cfg.espSkeletonScale >= 2.50f) ? 0.10f : g_cfg.espSkeletonScale + 0.25f; break;
				case 10: g_cfg.espHeadDot = !g_cfg.espHeadDot; break;
				case 11: g_cfg.espAnimalHeadDot = !g_cfg.espAnimalHeadDot; break;
				case 12: g_cfg.espHeadColorMode = (g_cfg.espHeadColorMode + 1) % 5; break;
				case 13: g_cfg.espWheelDots = !g_cfg.espWheelDots; break;
				case 14: g_cfg.espShowSpeed = !g_cfg.espShowSpeed; break;
				case 15: g_cfg.espLines = !g_cfg.espLines; break;
				case 16: g_cfg.espLineOrigin = (g_cfg.espLineOrigin + 1) % 5; break;
				case 17: g_cfg.espSnaplineDist = (g_cfg.espSnaplineDist >= 500) ? 100 : g_cfg.espSnaplineDist + 50; break;
				case 18: g_cfg.espLineAntiClip = !g_cfg.espLineAntiClip; break;
				case 19: g_cfg.espLineThickness = (g_cfg.espLineThickness % 3) + 1; break;
				case 20: g_cfg.espColorPreset = (g_cfg.espColorPreset + 1) % 6; break;
				case 22: g_cfg.espTextPosition = (g_cfg.espTextPosition + 1) % 2; break;
				case 24: g_cfg.espRectOpacity = (g_cfg.espRectOpacity >= 255) ? 60 : g_cfg.espRectOpacity + 40; break;
				case 25: g_cfg.espTextOpacity = (g_cfg.espTextOpacity >= 255) ? 60 : g_cfg.espTextOpacity + 40; break;
				case 26: g_cfg.espCrosshair = !g_cfg.espCrosshair; break;
				case 27: g_cfg.espCrosshairType = (g_cfg.espCrosshairType + 1) % 5; break;
				case 29: g_cfg.espCrosshairColor = (g_cfg.espCrosshairColor + 1) % 6; break;
				case 30: g_cfg.espAimCircle = !g_cfg.espAimCircle; break;
				case 32: g_cfg.espAimCircleRectScale = (g_cfg.espAimCircleRectScale >= 2.50f) ? 0.50f : g_cfg.espAimCircleRectScale + 0.25f; break;
				case 33: g_cfg.espAimAutoHide = !g_cfg.espAimAutoHide; break;
				case 34: g_cfg.espKillSatisfying = !g_cfg.espKillSatisfying; break;
				case 36: g_cfg.espKillSoundMode = (g_cfg.espKillSoundMode + 1) % 6; PlayKillSound(); break;
				case 37: g_cfg.espHealth = !g_cfg.espHealth; break;
				case 38: g_cfg.espVehHealth = !g_cfg.espVehHealth; break;
				case 39: g_cfg.espArmor = !g_cfg.espArmor; break;
				case 40: g_cfg.espDistance = !g_cfg.espDistance; break;
				case 41: g_cfg.espName = !g_cfg.espName; break;
				case 42: g_cfg.espShowWeapon = !g_cfg.espShowWeapon; break;
				case 43: g_cfg.espDamageText = !g_cfg.espDamageText; break;
				case 44: g_cfg.espEdge = !g_cfg.espEdge; break;
				case 45: g_cfg.espFramerateMode = (g_cfg.espFramerateMode + 1) % 4; break;
				case 46: g_cfg.espVelocitySmooth = (g_cfg.espVelocitySmooth + 1) % 4; break;
				case 47: g_cfg.espTransitionMode = (g_cfg.espTransitionMode + 1) % 4; g_espMasterAnimProgress = 0.0f; g_entityAnimMap.clear(); break;
				case 48: g_cfg.espAnimDuration = (g_cfg.espAnimDuration >= 1.50f) ? 0.10f : g_cfg.espAnimDuration + 0.15f; break;
				case 49: g_cfg.espFocus = !g_cfg.espFocus; break;
				case 52: g_cfg.espFriendly = !g_cfg.espFriendly; break;
				case 53: g_cfg.espEnemy = !g_cfg.espEnemy; break;
				case 54: g_cfg.espVehicles = !g_cfg.espVehicles; break;
				case 55: g_cfg.espAnimals = !g_cfg.espAnimals; break;
				case 56: g_cfg.espItems = !g_cfg.espItems; break;
				case 57: g_cfg.espShowCounters = !g_cfg.espShowCounters; break;
				case 58: g_cfg.espIgnoreDead = !g_cfg.espIgnoreDead; break;
				case 59: g_cfg.espHideOnWeaponWheel = !g_cfg.espHideOnWeaponWheel; break;
				case 60:
					g_cfg.espMaxDistance = (g_cfg.espMaxDistance >= 500) ? 100 : g_cfg.espMaxDistance + 50;
					break;
				case 61:
					ScanEspPresets();
					g_menuPage = PAGE_ESP_PRESETS;
					g_menuIndex = 0;
					g_menuScrollOffset = 0;
					PlayFrontendSound("SELECT");
					break;
				case 62:
					g_menuPage = PAGE_MAIN;
					g_menuIndex = 2;
					g_menuScrollOffset = 0;
					PlayFrontendSound("BACK");
					break;
				}
				SaveConfig();
			}
			else if (key == VK_ESCAPE || key == VK_BACK)
			{
				g_menuPage = PAGE_MAIN;
				g_menuIndex = 2;
				g_menuScrollOffset = 0;
				PlayFrontendSound("BACK");
			}
		}
		// 4. SUBMENU PREFERENSI
		else if (g_menuPage == PAGE_PREFERENCES)
		{
			const int totalItems = 5;
			if (key == VK_UP)
			{
				g_menuIndex = (g_menuIndex - 1 + totalItems) % totalItems;
				PlayFrontendSound("NAV_UP_DOWN");
			}
			else if (key == VK_DOWN)
			{
				g_menuIndex = (g_menuIndex + 1) % totalItems;
				PlayFrontendSound("NAV_UP_DOWN");
			}
			else if (key == VK_LEFT || key == VK_RIGHT)
			{
				bool inc = (key == VK_RIGHT);
				switch (g_menuIndex)
				{
				case 0: g_cfg.menuWidth = std::clamp(g_cfg.menuWidth + (inc ? 0.010f : -0.010f), 0.200f, 0.450f); break;
				case 1: g_cfg.menuItemHeight = std::clamp(g_cfg.menuItemHeight + (inc ? 0.001f : -0.001f), 0.018f, 0.045f); break;
				case 2: g_cfg.menuTextScale = std::clamp(g_cfg.menuTextScale + (inc ? 0.010f : -0.010f), 0.180f, 0.450f); break;
				case 3: g_cfg.menuHeaderScale = std::clamp(g_cfg.menuHeaderScale + (inc ? 0.020f : -0.020f), 0.300f, 0.750f); break;
				}
				SaveConfig();
				PlayFrontendSound("NAV_LEFT_RIGHT");
			}
			else if (key == VK_RETURN && g_menuIndex == 4)
			{
				g_menuPage = PAGE_MAIN;
				g_menuIndex = 2;
				g_menuScrollOffset = 0;
				PlayFrontendSound("BACK");
			}
			else if (key == VK_ESCAPE || key == VK_BACK)
			{
				g_menuPage = PAGE_MAIN;
				g_menuIndex = 2;
				g_menuScrollOffset = 0;
				PlayFrontendSound("BACK");
			}
		}
		// 5. SUBMENU WARNA TEKS / OUTLINE
		else if (g_menuPage == PAGE_COLORS || g_menuPage == PAGE_OUTLINE)
		{
			int* r = (g_menuPage == PAGE_COLORS) ? &g_cfg.textColorR : &g_cfg.outlineColorR;
			int* g = (g_menuPage == PAGE_COLORS) ? &g_cfg.textColorG : &g_cfg.outlineColorG;
			int* b = (g_menuPage == PAGE_COLORS) ? &g_cfg.textColorB : &g_cfg.outlineColorB;

			if (key == VK_UP)
			{
				g_menuIndex = (g_menuIndex - 1 + 4) % 4;
				PlayFrontendSound("NAV_UP_DOWN");
			}
			else if (key == VK_DOWN)
			{
				g_menuIndex = (g_menuIndex + 1) % 4;
				PlayFrontendSound("NAV_UP_DOWN");
			}
			else if (key == VK_LEFT || key == VK_RIGHT)
			{
				int delta = (key == VK_RIGHT) ? 15 : -15;
				if (g_menuIndex == 0) *r = std::clamp(*r + delta, 0, 255);
				else if (g_menuIndex == 1) *g = std::clamp(*g + delta, 0, 255);
				else if (g_menuIndex == 2) *b = std::clamp(*b + delta, 0, 255);
				SaveConfig();
				PlayFrontendSound("NAV_LEFT_RIGHT");
			}
			else if (key == VK_RETURN && g_menuIndex == 3)
			{
				g_menuPage = PAGE_SPEEDOMETER;
				g_menuIndex = (g_menuPage == PAGE_COLORS) ? 22 : 23;
				g_menuScrollOffset = 0;
				SaveConfig();
				PlayFrontendSound("BACK");
			}
			else if (key == VK_ESCAPE || key == VK_BACK)
			{
				g_menuPage = PAGE_SPEEDOMETER;
				g_menuIndex = (g_menuPage == PAGE_COLORS) ? 22 : 23;
				g_menuScrollOffset = 0;
				SaveConfig();
				PlayFrontendSound("BACK");
			}
		}
		// 6. SUBMENU THEMES
		else if (g_menuPage == PAGE_THEMES)
		{
			int totalThemeOptions = (int)g_themeFiles.size() + 2;
			if (key == VK_UP)
			{
				g_menuIndex = (g_menuIndex - 1 + totalThemeOptions) % totalThemeOptions;
				PlayFrontendSound("NAV_UP_DOWN");
			}
			else if (key == VK_DOWN)
			{
				g_menuIndex = (g_menuIndex + 1) % totalThemeOptions;
				PlayFrontendSound("NAV_UP_DOWN");
			}
			else if (key == VK_RETURN)
			{
				if (g_menuIndex == 0)
				{
					char newThemeName[MAX_PATH];
					sprintf_s(newThemeName, "%sTema_Kustom_%lld.ini", g_themesDir, GetTickCount64() % 10000);
					SaveConfig(newThemeName);
					ScanThemes();
					SetStatusMessage("Tema Baru Berhasil Disimpan!");
					PlayFrontendSound("SELECT");
				}
				else if (g_menuIndex == totalThemeOptions - 1)
				{
					g_menuPage = PAGE_MAIN;
					g_menuIndex = 3;
					g_menuScrollOffset = 0;
					PlayFrontendSound("BACK");
				}
				else
				{
					int fileIdx = g_menuIndex - 1;
					if (fileIdx >= 0 && fileIdx < (int)g_themeFiles.size())
					{
						char fullPath[MAX_PATH];
						sprintf_s(fullPath, "%s%s", g_themesDir, g_themeFiles[fileIdx].c_str());
						LoadConfig(fullPath);
						SaveConfig();
						SetStatusMessage("Tema Berhasil Dimuat: " + g_themeFiles[fileIdx]);
						PlayFrontendSound("SELECT");
					}
				}
			}
			else if (key == VK_DELETE)
			{
				int fileIdx = g_menuIndex - 1;
				if (fileIdx >= 0 && fileIdx < (int)g_themeFiles.size())
				{
					char fullPath[MAX_PATH];
					sprintf_s(fullPath, "%s%s", g_themesDir, g_themeFiles[fileIdx].c_str());
					DeleteFileA(fullPath);
					SetStatusMessage("Tema Dihapus: " + g_themeFiles[fileIdx]);
					ScanThemes();
					PlayFrontendSound("BACK");
				}
			}
			else if (key == VK_ESCAPE || key == VK_BACK)
			{
				g_menuPage = PAGE_MAIN;
				g_menuIndex = 3;
				g_menuScrollOffset = 0;
				PlayFrontendSound("BACK");
			}
		}
		// 7. SUBMENU PRESET ESP
		else if (g_menuPage == PAGE_ESP_PRESETS)
		{
			int totalPresetOptions = (int)g_espPresetFiles.size() + 2;
			if (key == VK_UP)
			{
				g_menuIndex = (g_menuIndex - 1 + totalPresetOptions) % totalPresetOptions;
				PlayFrontendSound("NAV_UP_DOWN");
			}
			else if (key == VK_DOWN)
			{
				g_menuIndex = (g_menuIndex + 1) % totalPresetOptions;
				PlayFrontendSound("NAV_UP_DOWN");
			}
			else if (key == VK_RETURN)
			{
				if (g_menuIndex == 0)
				{
					char newPresetName[MAX_PATH];
					sprintf_s(newPresetName, "%sPreset_ESP_%lld.ini", g_espPresetsDir, GetTickCount64() % 10000);
					SaveEspPreset(newPresetName);
					ScanEspPresets();
					SetStatusMessage("Preset ESP Berhasil Disimpan!");
					PlayFrontendSound("SELECT");
				}
				else if (g_menuIndex == totalPresetOptions - 1)
				{
					g_menuPage = PAGE_ESP;
					g_menuIndex = 48;
					g_menuScrollOffset = 37;
					PlayFrontendSound("BACK");
				}
				else
				{
					int fileIdx = g_menuIndex - 1;
					if (fileIdx >= 0 && fileIdx < (int)g_espPresetFiles.size())
					{
						char fullPath[MAX_PATH];
						sprintf_s(fullPath, "%s%s", g_espPresetsDir, g_espPresetFiles[fileIdx].c_str());
						LoadEspPreset(fullPath);
						SaveConfig();
						SetStatusMessage("Preset ESP Berhasil Dimuat: " + g_espPresetFiles[fileIdx]);
						PlayFrontendSound("SELECT");
					}
				}
			}
			else if (key == VK_DELETE)
			{
				int fileIdx = g_menuIndex - 1;
				if (fileIdx >= 0 && fileIdx < (int)g_espPresetFiles.size())
				{
					char fullPath[MAX_PATH];
					sprintf_s(fullPath, "%s%s", g_espPresetsDir, g_espPresetFiles[fileIdx].c_str());
					DeleteFileA(fullPath);
					SetStatusMessage("Preset ESP Dihapus: " + g_espPresetFiles[fileIdx]);
					ScanEspPresets();
					PlayFrontendSound("BACK");
				}
			}
			else if (key == VK_ESCAPE || key == VK_BACK)
			{
				g_menuPage = PAGE_ESP;
				g_menuIndex = 49;
				g_menuScrollOffset = 38;
				PlayFrontendSound("BACK");
			}
		}
	}

	void OnKeyboard(DWORD key, WORD repeats, BYTE scanCode, BOOL isExtended, BOOL isWithAlt, BOOL wasDownBefore, BOOL isUpNow)
	{
		(void)repeats; (void)scanCode; (void)isExtended; (void)isWithAlt;
		if (key == g_cfg.toggleKey)
		{
			g_f12IsDown = !isUpNow;
		}

		if (key < 256)
		{
			if (!isUpNow)
			{
				if (!wasDownBefore)
				{
					g_keyJustPressed[key] = true;
				}
				g_isKeyDown[key] = true;
			}
			else
			{
				g_isKeyDown[key] = false;
			}
		}
	}

	// -------------------------------------------------------------
	// SISTEM RENDER ESP (WALLHACK & VISUALS)
	// -------------------------------------------------------------
	void RenderESP(Ped playerPed, ULONGLONG currentTick, float dt)
	{
		if (CUTSCENE::IS_CUTSCENE_PLAYING()) return;

		// Master Toggle Animasi ON / OFF (Transisi Halus Fade In & Out Sesuai Durasi)
		float animRate = 1.0f / std::max(0.04f, g_cfg.espAnimDuration);
		if (g_cfg.espMaster)
		{
			g_espMasterAnimProgress = std::clamp(g_espMasterAnimProgress + animRate * dt, 0.0f, 1.0f);
			g_lastEspMasterActive = true;
		}
		else
		{
			g_espMasterAnimProgress = std::clamp(g_espMasterAnimProgress - animRate * dt, 0.0f, 1.0f);
			if (g_espMasterAnimProgress <= 0.001f)
			{
				g_lastEspMasterActive = false;
				return;
			}
		}

		// Cek apakah pemain sedang melakukan Aiming atau Shooting (Mouse Kanan/Kiri, Gamepad, Drive-by, Cover-aim)
		bool isAimingOrShooting = PLAYER::IS_PLAYER_FREE_AIMING(PLAYER::PLAYER_ID())
		    || PLAYER::IS_PLAYER_TARGETTING_ANYTHING(PLAYER::PLAYER_ID())
		    || PED::IS_PED_SHOOTING(playerPed)
		    || CONTROLS::IS_CONTROL_PRESSED(0, 25) /* INPUT_AIM */
		    || CONTROLS::IS_DISABLED_CONTROL_PRESSED(0, 25)
		    || CONTROLS::IS_CONTROL_PRESSED(2, 25)
		    || CONTROLS::IS_DISABLED_CONTROL_PRESSED(2, 25)
		    || CONTROLS::IS_CONTROL_PRESSED(0, 24) /* INPUT_ATTACK */
		    || CONTROLS::IS_DISABLED_CONTROL_PRESSED(0, 24)
		    || CONTROLS::IS_CONTROL_PRESSED(2, 24)
		    || CONTROLS::IS_DISABLED_CONTROL_PRESSED(2, 24)
		    || CONTROLS::IS_CONTROL_PRESSED(0, 68) /* INPUT_VEH_AIM */
		    || CONTROLS::IS_DISABLED_CONTROL_PRESSED(0, 68)
		    || CONTROLS::IS_CONTROL_PRESSED(0, 69) /* INPUT_VEH_ATTACK */
		    || CONTROLS::IS_DISABLED_CONTROL_PRESSED(0, 69)
		    || (GetKeyState(VK_RBUTTON) < 0)
		    || ((GetAsyncKeyState(VK_RBUTTON) & 0x8000) != 0)
		    || (GetKeyState(VK_LBUTTON) < 0)
		    || ((GetAsyncKeyState(VK_LBUTTON) & 0x8000) != 0);

		// Sembunyikan ESP sementara saat Tab Senjata (Weapon Wheel) atau Tombol Cover 'Q' ditekan (kecuali sedang aim/menembak)
		if (g_cfg.espHideOnWeaponWheel && !isAimingOrShooting)
		{
			if (UI::IS_HUD_COMPONENT_ACTIVE(19) ||
			    CONTROLS::IS_CONTROL_PRESSED(0, 37) ||
			    CONTROLS::IS_DISABLED_CONTROL_PRESSED(0, 37) ||
			    CONTROLS::IS_CONTROL_PRESSED(0, 44) /* INPUT_COVER / Q */ ||
			    CONTROLS::IS_DISABLED_CONTROL_PRESSED(0, 44) ||
			    (GetAsyncKeyState('Q') & 0x8000) != 0)
			{
				return;
			}
		}

		// Interval refresh ESP (Rendering dilakukan terus setiap frame agar 100% bebas flicker/blinking di 60 FPS ke bawah)
		g_lastEspRenderTick = currentTick;
		g_renderedRectCount = 0;

		// Auto-Hide Crosshair & Aim Circle: Disembunyikan secara default dan akan muncul jika melakukan aim / hold klik kanan / klik kiri
		if (!g_cfg.espAimAutoHide || isAimingOrShooting)
		{
			DrawCrosshair();
			DrawAimCircle(dt);
		}

		Vector3 playerCoords = ENTITY::GET_ENTITY_COORDS(playerPed, TRUE);
		Vehicle playerVeh = PED::IS_PED_IN_ANY_VEHICLE(playerPed, false) ? PED::GET_VEHICLE_PED_IS_USING(playerPed) : 0;
		float maxDistSq = (float)(g_cfg.espMaxDistance * g_cfg.espMaxDistance);

		float aspect = GRAPHICS::_GET_SCREEN_ASPECT_RATIO(FALSE);
		if (aspect <= 0.0f) aspect = 1.7777778f;

		float masterAnimAlphaMult = g_espMasterAnimProgress;
		if (g_cfg.espTransitionMode == 2)
			masterAnimAlphaMult = std::clamp(EaseOutBack(g_espMasterAnimProgress), 0.01f, 1.0f);
		else if (g_cfg.espTransitionMode == 3)
			masterAnimAlphaMult = std::clamp(EaseInOutCubic(g_espMasterAnimProgress), 0.01f, 1.0f);

		int bestAimState = 0; // 0: None, 1: Badan, 2: Kepala

		int countPeds = 0;
		int countEnemies = 0;
		int countVehicles = 0;
		int countAnimals = 0;
		int countItems = 0;

		// 1. RENDER PEDS
		const int MAX_PEDS = 256;
		Ped peds[MAX_PEDS];
		int pedCount = worldGetAllPeds(peds, MAX_PEDS);
		int activeSkeleton2DCount = 0;

		struct PedCandidate
		{
			Ped ped;
			bool isDead;
			bool isDeadMarker2s;
			bool isHuman;
			bool isEnemy;
			bool isAttackingPlayer;
			Vector3 pedCoords;
			float distMeters;
			Hash model;
			std::string tag;
			std::string heldItemName;
			int r, g, b;
			float screenDistCenter;
			bool isCenterAimTarget;
			float focusAlphaMult;
			bool inViewport;
			float edgeFade;
		};

		std::vector<PedCandidate> pedCandidates;
		pedCandidates.reserve(pedCount);

		for (int i = 0; i < pedCount; ++i)
		{
			Ped ped = peds[i];
			if (ped == playerPed) continue;
			if (!ENTITY::DOES_ENTITY_EXIST(ped)) continue;

			bool isDead = ENTITY::IS_ENTITY_DEAD(ped) || PED::IS_PED_DEAD_OR_DYING(ped, TRUE);
			bool isDeadMarker2s = false;

			if (isDead)
			{
				if (g_pedDeathTime.find(ped) == g_pedDeathTime.end())
				{
					g_pedDeathTime[ped] = currentTick;

					// Cek apakah NPC ini terbunuh oleh pemain untuk kill streak banner!
					bool killedByPlayer = false;
					Entity killer = PED::_GET_PED_KILLER(ped);
					if (killer == playerPed || (playerVeh && killer == playerVeh))
					{
						killedByPlayer = true;
					}
					else if (ENTITY::HAS_ENTITY_BEEN_DAMAGED_BY_ENTITY(ped, playerPed, TRUE))
					{
						killedByPlayer = true;
					}
					else if (playerVeh && ENTITY::HAS_ENTITY_BEEN_DAMAGED_BY_ENTITY(ped, playerVeh, TRUE))
					{
						killedByPlayer = true;
					}

					if (killedByPlayer)
					{
						g_killStreakCount++;
						g_lastKillTick = currentTick;
						g_killBannerScaleBlend = 1.0f;
						bool isTargetHuman = PED::IS_PED_HUMAN(ped) != 0;
						g_lastKillIsAnimal = !isTargetHuman;
						if (!isTargetHuman)
							g_lastKillAnimalName = GetAnimalName(ENTITY::GET_ENTITY_MODEL(ped));
						else
							g_lastKillAnimalName = "";
						PlayKillSound();
					}
				}
				ULONGLONG elapsed = currentTick - g_pedDeathTime[ped];
				if (elapsed <= 2000)
				{
					isDeadMarker2s = true; // Penanda mati 2 detik box ungu
				}
				else if (g_cfg.espIgnoreDead)
				{
					continue;
				}
			}
			else
			{
				g_pedDeathTime.erase(ped);
			}

			bool isHuman = PED::IS_PED_HUMAN(ped) != 0;
			if (isHuman) countPeds++; else countAnimals++;

			// Pelacakan Floating Damage Text (Muncul angka damage saat ditembak/diserang)
			if (g_cfg.espDamageText && !isDead)
			{
				int curHp = ENTITY::GET_ENTITY_HEALTH(ped);
				int curArm = isHuman ? PED::GET_PED_ARMOUR(ped) : 0;
				int totalHp = curHp + curArm;

				auto itHp = g_lastPedHealth.find(ped);
				if (itHp != g_lastPedHealth.end())
				{
					int prevHp = itHp->second;
					if (prevHp > totalHp && (prevHp - totalHp) > 0)
					{
						int dmgDealt = prevHp - totalHp;
						if (ENTITY::HAS_ENTITY_BEEN_DAMAGED_BY_ENTITY(ped, playerPed, TRUE) ||
						    (playerVeh != 0 && ENTITY::HAS_ENTITY_BEEN_DAMAGED_BY_ENTITY(ped, playerVeh, TRUE)))
						{
							Any lastBone = 0;
							bool isHeadshot = false;
							if (PED::GET_PED_LAST_DAMAGE_BONE(ped, &lastBone))
							{
								if (lastBone == (Any)SKEL_Head || lastBone == 31086)
									isHeadshot = true;
							}
							Vector3 dmgPos = isHuman ? PED::GET_PED_BONE_COORDS(ped, SKEL_Head, 0.0f, 0.0f, 0.18f)
							                         : ENTITY::GET_ENTITY_COORDS(ped, TRUE);
							if (dmgPos.x == 0.0f && dmgPos.y == 0.0f)
							{
								dmgPos = ENTITY::GET_ENTITY_COORDS(ped, TRUE);
								dmgPos.z += 0.85f;
							}
							dmgPos.x += ((float)(rand() % 100) - 50.0f) * 0.0020f;
							dmgPos.y += ((float)(rand() % 100) - 50.0f) * 0.0020f;

							g_damageIndicators.push_back({ dmgPos, dmgDealt, currentTick, isHeadshot });
							ENTITY::CLEAR_ENTITY_LAST_DAMAGE_ENTITY(ped);
							PED::CLEAR_PED_LAST_DAMAGE_BONE(ped);
						}
					}
				}
				g_lastPedHealth[ped] = totalHp;
			}

			bool isEnemy = false;
			bool isAttackingPlayer = false;
			std::string heldItemName = "";
			if (isHuman)
			{
				Hash curWeapon = 0;
				bool hasWeapon = IsPedHoldingRealWeapon(ped, curWeapon);
				if (hasWeapon && curWeapon != 0)
				{
					heldItemName = GetWeaponDisplayName(curWeapon);
				}
				isAttackingPlayer = IsPedAttackingPlayer(ped, playerPed, playerVeh);

				if (hasWeapon || isAttackingPlayer)
				{
					isEnemy = true;
				}
				else
				{
					isEnemy = false;
				}

				if (isEnemy) countEnemies++;
			}

			Vector3 pedCoords = ENTITY::GET_ENTITY_COORDS(ped, TRUE);
			float dx = pedCoords.x - playerCoords.x;
			float dy = pedCoords.y - playerCoords.y;
			float dz = pedCoords.z - playerCoords.z;
			float distSq = dx * dx + dy * dy + dz * dz;
			if (distSq > maxDistSq) continue;
			float distMeters = std::sqrt(distSq);

			if (!isHuman && !g_cfg.espAnimals) continue;
			if (isHuman)
			{
				if (isEnemy && !g_cfg.espEnemy) continue;
				if (!isEnemy && !g_cfg.espFriendly) continue;
			}

			int r = 255, g = 255, b = 255;
			int targetType = isHuman ? 0 : 2;
			GetEspColor(targetType, isEnemy, r, g, b);

			Hash model = ENTITY::GET_ENTITY_MODEL(ped);
			std::string tag = "";
			if (!isHuman) tag = GetAnimalName(model);
			else if (isAttackingPlayer && !isDead)
			{
				tag = L("MENYERANG", "ATTACKING");
				r = 255; g = 30; b = 30; // Merah Menyerang
			}
			else if (isEnemy)
			{
				tag = L("MUSUH", "ENEMY");
				r = 255; g = 50; b = 180; // Vibrant Pink Musuh
			}
			else tag = "NPC";

			if (isDeadMarker2s)
			{
				r = 185; g = 45; b = 255;
				tag = L("MATI (2s)", "DEAD (2s)");
			}
			else if (isDead)
			{
				r = 130; g = 130; b = 130;
				tag = L("MATI", "DEAD");
			}

			// Screen projection untuk deteksi kedekatan dengan titik tengah layar monitor (0.50, 0.50)
			float scX = 0.0f, scY = 0.0f;
			Vector3 midCheck = pedCoords;
			midCheck.z += 0.35f;
			float screenDistCenter = 999.0f;
			bool onScreen = GRAPHICS::_WORLD3D_TO_SCREEN2D(midCheck.x, midCheck.y, midCheck.z, &scX, &scY);
			float focusAlphaMult = 1.0f;
			if (onScreen)
			{
				float cdx = scX - 0.500f;
				float cdy = (scY - 0.500f) * aspect;
				screenDistCenter = std::sqrt(cdx * cdx + cdy * cdy);
				if (g_cfg.espFocus)
				{
					focusAlphaMult = GetFocusAlphaMultiplier(scX, scY);
					if (focusAlphaMult <= 0.02f) continue; // Pudar total di luar radius fokus
				}
			}
			else if (g_cfg.espFocus)
			{
				continue; // Luar layar langsung sembunyikan jika mode Fokus aktif
			}

			bool isFreeAiming = PLAYER::IS_PLAYER_FREE_AIMING_AT_ENTITY(PLAYER::PLAYER_ID(), ped) != 0;
			bool inViewport = onScreen && (scX >= -0.06f && scX <= 1.06f && scY >= -0.06f && scY <= 1.06f);
			float edgeFade = 1.0f;
			if (inViewport)
			{
				float distEdgeX = std::min(scX - (-0.06f), 1.06f - scX);
				float distEdgeY = std::min(scY - (-0.06f), 1.06f - scY);
				float minEdge = std::min(distEdgeX, distEdgeY);
				if (minEdge < 0.08f)
				{
					edgeFade = std::clamp(minEdge / 0.08f, 0.0f, 1.0f);
				}
			}

			bool isCenterAimTarget = isFreeAiming || (onScreen && (screenDistCenter <= (isAimingOrShooting ? 0.22f : 0.10f)));

			pedCandidates.push_back({
				ped, isDead, isDeadMarker2s, isHuman, isEnemy, isAttackingPlayer, pedCoords, distMeters, model, tag, heldItemName, r, g, b, screenDistCenter, isCenterAimTarget, focusAlphaMult, inViewport, edgeFade
			});
		}

		// SORTING CERDAS:
		// Jika pemain sedang Aiming / Sniper, target di tengah layar crosshair didahulukan render 100%!
		if (isAimingOrShooting)
		{
			std::sort(pedCandidates.begin(), pedCandidates.end(), [](const PedCandidate& a, const PedCandidate& b) {
				if (a.isCenterAimTarget != b.isCenterAimTarget)
					return a.isCenterAimTarget > b.isCenterAimTarget;
				return a.screenDistCenter < b.screenDistCenter;
			});
		}
		else
		{
			// Normal: Target terdekat dengan pemain dirender duluan
			std::sort(pedCandidates.begin(), pedCandidates.end(), [](const PedCandidate& a, const PedCandidate& b) {
				return a.distMeters < b.distMeters;
			});
		}

		for (const auto& cand : pedCandidates)
		{
			Ped ped = cand.ped;
			bool isDead = cand.isDead;
			bool isDeadMarker2s = cand.isDeadMarker2s;
			bool isHuman = cand.isHuman;
			Vector3 pedCoords = cand.pedCoords;
			float distMeters = cand.distMeters;
			Hash model = cand.model;
			const std::string& tag = cand.tag;
			int r = cand.r, g = cand.g, b = cand.b;
			bool isCenterAimTarget = cand.isCenterAimTarget;

			// Animasi Transisi & Fokus per Entitas Ped (Mulus saat kamera berpindah)
			auto& an = g_entityAnimMap[ped];
			if (cand.inViewport)
			{
				if (!an.wasOnScreen || (currentTick - an.lastSeenTick > 900))
				{
					an.animProgress = 0.0f;
				}
				an.wasOnScreen = true;
				an.animProgress = std::clamp(an.animProgress + animRate * dt, 0.0f, 1.0f);
			}
			else
			{
				an.wasOnScreen = false;
				an.animProgress = 0.0f;
			}
			an.lastSeenTick = currentTick;

			float animScale = 1.0f;
			float animAlphaMult = 1.0f;
			if (g_cfg.espTransitionMode > 0)
			{
				float prog = an.animProgress;
				if (g_cfg.espTransitionMode == 1) animAlphaMult = prog;
				else if (g_cfg.espTransitionMode == 2) { animScale = EaseOutBack(prog); animAlphaMult = prog; }
				else if (g_cfg.espTransitionMode == 3) { animScale = EaseInOutCubic(prog); animAlphaMult = prog; }
			}

			float combinedAlphaMult = animAlphaMult * cand.focusAlphaMult * masterAnimAlphaMult * cand.edgeFade;
			if (combinedAlphaMult <= 0.02f) continue;

			// Smart Distance Prioritization: jika jumlah rectangle mendekati batas buffer (>= 265), korbankan ESP target yang jauh (> 35m) agar target dekat dan prioritas tetap kerender sempurna
			if (g_renderedRectCount >= 265 && distMeters > 35.0f && !isCenterAimTarget)
			{
				continue;
			}

			// A. Snaplines
			if (g_cfg.espLines && distMeters <= (float)g_cfg.espSnaplineDist)
			{
				DrawSnapline(playerCoords, pedCoords, r, g, b, 190, combinedAlphaMult);
			}

			// B. Skeleton (Tulang) Murni Rectangle Tanpa Garis (Manusia & Hewan)
			// Smart Distance Priority: jika buffer mendekati kuota (>= 235 rect), korbankan target jauh (> 35m) agar target dekat tetap utuh
			bool skipSkeletonBuffer = (g_renderedRectCount >= 235 && distMeters > 35.0f && !isCenterAimTarget);
			if (g_cfg.espSkeleton && !isDead && distMeters <= (float)g_cfg.espSkeletonDist && !skipSkeletonBuffer)
			{
				if (isHuman)
				{
					DrawPedSkeleton(ped, r, g, b, (int)(235 * combinedAlphaMult), distMeters);
				}
				else if (g_cfg.espAnimals)
				{
					DrawAnimalSkeleton(ped, r, g, b, (int)(235 * combinedAlphaMult), distMeters);
				}
			}

			// C1. ESP Penanda Titik Kepala Hewan
			if (!isHuman && g_cfg.espAnimalHeadDot && !isDead && distMeters <= 50.0f)
			{
				int hBone = ENTITY::GET_ENTITY_BONE_INDEX_BY_NAME(ped, (char*)"head");
				Vector3 aHeadCoords = (hBone != -1) ? ENTITY::GET_WORLD_POSITION_OF_ENTITY_BONE(ped, hBone)
				                                    : PED::GET_PED_BONE_COORDS(ped, SKEL_Head, 0.0f, 0.0f, 0.0f);
				float ahSx = 0.0f, ahSy = 0.0f;
				if (GRAPHICS::_WORLD3D_TO_SCREEN2D(aHeadCoords.x, aHeadCoords.y, aHeadCoords.z, &ahSx, &ahSy))
				{
					float dotSize = 0.0038f * (float)g_cfg.espLineThickness;
					DrawEspRect(ahSx, ahSy, dotSize, dotSize * 1.5f, 255, 235, 40, ApplyEspAlpha((int)(245 * combinedAlphaMult)));
				}
			}

			// C2. ESP Penanda Kotak Kepala NPC (Sesuai Ukuran Kepala NPC)
			// PRIORITAS SNIPER: Jika di-aim, jarak maksimal kepala diperluas hingga espSkeletonDist (bukan cuma 60m)!
			float maxHeadDist = isCenterAimTarget ? (float)g_cfg.espSkeletonDist : 60.0f;
			if (isHuman && g_cfg.espHeadDot && !isDead && distMeters <= maxHeadDist)
			{
				Vector3 headCoords = PED::GET_PED_BONE_COORDS(ped, SKEL_Head, 0.0f, 0.0f, 0.0f);
				Vector3 headTopPos = headCoords; headTopPos.z += 0.13f;
				Vector3 headBtmPos = headCoords; headBtmPos.z -= 0.12f;

				float htX = 0.0f, htY = 0.0f;
				float hbX = 0.0f, hbY = 0.0f;
				float hCenterScreenX = 0.0f, hCenterScreenY = 0.0f;

				bool pHeadTop = GRAPHICS::_WORLD3D_TO_SCREEN2D(headTopPos.x, headTopPos.y, headTopPos.z, &htX, &htY);
				bool pHeadBtm = GRAPHICS::_WORLD3D_TO_SCREEN2D(headBtmPos.x, headBtmPos.y, headBtmPos.z, &hbX, &hbY);
				bool pHeadCenter = GRAPHICS::_WORLD3D_TO_SCREEN2D(headCoords.x, headCoords.y, headCoords.z, &hCenterScreenX, &hCenterScreenY);

				if (pHeadTop || pHeadBtm || pHeadCenter)
				{
					float headH = 0.0f;
					float headCenterX = 0.0f;
					float headCenterY = 0.0f;

					if (pHeadTop && pHeadBtm)
					{
						headH = std::abs(hbY - htY);
						headCenterX = (htX + hbX) * 0.5f;
						headCenterY = (htY + hbY) * 0.5f;
					}
					else if (pHeadCenter)
					{
						headH = std::clamp(0.25f / distMeters, 0.006f, 0.080f);
						headCenterX = hCenterScreenX;
						headCenterY = hCenterScreenY;
					}
					else if (pHeadTop)
					{
						headH = std::clamp(0.25f / distMeters, 0.006f, 0.080f);
						headCenterX = htX;
						headCenterY = htY + headH * 0.5f;
					}
					else
					{
						headH = std::clamp(0.25f / distMeters, 0.006f, 0.080f);
						headCenterX = hbX;
						headCenterY = hbY - headH * 0.5f;
					}

					if (headH < 0.005f) headH = 0.005f;
					float headW = headH * 0.78f;

					int hR = 255, hG = 255, hB = 255;
					switch (g_cfg.espHeadColorMode)
					{
					case 1: hR = r; hG = g; hB = b; break; // Sesuai Target
					case 2: hR = 255; hG = 45; hB = 45; break; // Merah
					case 3: hR = 0; hG = 220; hB = 255; break; // Cyan
					case 4: hR = 255; hG = 215; hB = 25; break; // Kuning
					default: hR = 255; hG = 255; hB = 255; break; // Putih
					}

					// Saat Aim Center (Sniper/Crosshair Focus), jangan kena LOD fill rectangle: gunakan outline seperti di jarak dekat!
					bool useOutline = isCenterAimTarget || (distMeters <= 28.0f && headH >= 0.012f);
					if (useOutline)
					{
						if (headH < 0.009f)
						{
							headH = 0.009f;
							headW = headH * 0.78f;
						}
						float hThick = 0.0013f * (float)g_cfg.espLineThickness;
						DrawEspRect(headCenterX, headCenterY - headH * 0.5f, headW, hThick, hR, hG, hB, ApplyEspAlpha((int)(245 * combinedAlphaMult)));
						DrawEspRect(headCenterX, headCenterY + headH * 0.5f, headW, hThick, hR, hG, hB, ApplyEspAlpha((int)(245 * combinedAlphaMult)));
						DrawEspRect(headCenterX - headW * 0.5f, headCenterY, hThick * 0.6f, headH, hR, hG, hB, ApplyEspAlpha((int)(245 * combinedAlphaMult)));
						DrawEspRect(headCenterX + headW * 0.5f, headCenterY, hThick * 0.6f, headH, hR, hG, hB, ApplyEspAlpha((int)(245 * combinedAlphaMult)));

						float microDot = 0.0022f * (float)g_cfg.espLineThickness;
						DrawEspRect(headCenterX, headCenterY, microDot, microDot * 1.5f, hR, hG, hB, ApplyEspAlpha((int)(250 * combinedAlphaMult)));
					}
					else
					{
						// Jarak jauh non-aim LOD dot hemat rect (Opacity 30% sesuai request user agar tidak pekat)
						float dotW = std::max(0.0034f * (float)g_cfg.espLineThickness, headW);
						DrawEspRect(headCenterX, headCenterY, dotW, dotW * 1.4f, hR, hG, hB, ApplyEspRectAlpha((int)(76 * combinedAlphaMult)));
					}
				}
			}

			// D. 2D Bounding Box & HUD Text (Anti-Blink Multi-Point Frustum Fallback)
			Vector3 minDim = {}, maxDim = {};
			GAMEPLAY::GET_MODEL_DIMENSIONS(model, &minDim, &maxDim);
			if ((maxDim.z - minDim.z) < 0.5f)
			{
				minDim.x = -0.35f; maxDim.x = 0.35f;
				minDim.y = -0.35f; maxDim.y = 0.35f;
				minDim.z = -0.90f; maxDim.z = 0.95f;
			}

			Vector3 midPos = pedCoords;
			midPos.z += (maxDim.z + minDim.z) * 0.5f;
			Vector3 topPos = {}; topPos.x = pedCoords.x; topPos.y = pedCoords.y; topPos.z = pedCoords.z + maxDim.z + 0.12f;
			Vector3 btmPos = {}; btmPos.x = pedCoords.x; btmPos.y = pedCoords.y; btmPos.z = pedCoords.z + minDim.z - 0.05f;

			float sTopX = 0.0f, sTopY = 0.0f;
			float sBtmX = 0.0f, sBtmY = 0.0f;
			float sMidX = 0.0f, sMidY = 0.0f;

			bool pTop = GRAPHICS::_WORLD3D_TO_SCREEN2D(topPos.x, topPos.y, topPos.z, &sTopX, &sTopY);
			bool pBtm = GRAPHICS::_WORLD3D_TO_SCREEN2D(btmPos.x, btmPos.y, btmPos.z, &sBtmX, &sBtmY);
			bool pMid = GRAPHICS::_WORLD3D_TO_SCREEN2D(midPos.x, midPos.y, midPos.z, &sMidX, &sMidY);

			if (pTop || pBtm || pMid)
			{
				float estH = std::clamp((maxDim.z - minDim.z + 0.17f) / distMeters, 0.008f, 1.20f);
				float boxH = 0.0f, boxCenterX = 0.0f, boxTopY = 0.0f, boxBtmY = 0.0f;

				if (pTop && pBtm)
				{
					boxH = std::abs(sBtmY - sTopY);
					boxCenterX = (sTopX + sBtmX) * 0.5f;
					boxTopY = std::min(sTopY, sBtmY);
					boxBtmY = std::max(sTopY, sBtmY);
				}
				else if (pTop && pMid)
				{
					boxH = std::abs(sMidY - sTopY) * 2.0f;
					boxCenterX = sTopX;
					boxTopY = sTopY;
					boxBtmY = sTopY + boxH;
				}
				else if (pBtm && pMid)
				{
					boxH = std::abs(sBtmY - sMidY) * 2.0f;
					boxCenterX = sBtmX;
					boxTopY = sBtmY - boxH;
					boxBtmY = sBtmY;
				}
				else if (pTop)
				{
					boxH = estH;
					boxCenterX = sTopX;
					boxTopY = sTopY;
					boxBtmY = sTopY + boxH;
				}
				else if (pBtm)
				{
					boxH = estH;
					boxCenterX = sBtmX;
					boxTopY = sBtmY - boxH;
					boxBtmY = sBtmY;
				}
				else
				{
					boxH = estH;
					boxCenterX = sMidX;
					boxTopY = sMidY - boxH * 0.5f;
					boxBtmY = sMidY + boxH * 0.5f;
				}

				float finalTop = std::min(boxTopY, boxBtmY);
				float finalBtm = std::max(boxTopY, boxBtmY);
				boxTopY = finalTop;
				boxBtmY = finalBtm;
				boxH = boxBtmY - boxTopY;
				if (boxH < 0.005f)
				{
					boxH = (estH > 0.005f) ? estH : 0.005f;
					float cY = (boxTopY + boxBtmY) * 0.5f;
					boxTopY = cY - boxH * 0.5f;
					boxBtmY = cY + boxH * 0.5f;
				}
				float boxW = boxH * 0.44f * g_cfg.espPedBoxScale;

				// TERAPKAN VELOCITY SMOOTHING PADA BOX NPC:
				if (g_cfg.espVelocitySmooth > 0)
				{
					auto& sm = g_entitySmoothMap[ped];
					if (!sm.initialized || (currentTick - sm.lastSeenTick > 800) ||
					    std::abs(boxCenterX - sm.curX) > 0.18f || std::abs(boxTopY - sm.curTopY) > 0.18f)
					{
						sm.curX = boxCenterX; sm.curTopY = boxTopY; sm.curBtmY = boxBtmY;
						sm.curW = boxW; sm.curH = boxH; sm.initialized = true;
					}
					else
					{
						float smoothSpeed = (g_cfg.espVelocitySmooth == 3) ? 9.0f : ((g_cfg.espVelocitySmooth == 2) ? 18.0f : 32.0f);
						float blend = std::clamp(smoothSpeed * dt, 0.05f, 1.0f);
						sm.curX += (boxCenterX - sm.curX) * blend;
						sm.curTopY += (boxTopY - sm.curTopY) * blend;
						sm.curBtmY += (boxBtmY - sm.curBtmY) * blend;
						sm.curW += (boxW - sm.curW) * blend;
						sm.curH += (boxH - sm.curH) * blend;

						boxCenterX = sm.curX;
						boxTopY = sm.curTopY;
						boxBtmY = sm.curBtmY;
						boxW = sm.curW;
						boxH = sm.curH;
					}
					sm.lastSeenTick = currentTick;
				}

				// TERAPKAN SKALA ANIMASI TRANSISI (BOUNCE POP / ELASTIC):
				if (g_cfg.espTransitionMode > 0)
				{
					boxW *= animScale;
					boxH *= animScale;
					float cY = (boxTopY + boxBtmY) * 0.5f;
					boxTopY = cY - boxH * 0.5f;
					boxBtmY = cY + boxH * 0.5f;
				}

				int finalAlpha = ApplyEspAlpha((int)(240 * combinedAlphaMult));

				// Warna Box: Merah jika menyerang saya, Pink jika NPC musuh
				int boxR = r, boxG = g, boxB = b;
				if (cand.isAttackingPlayer && !isDead)
				{
					boxR = 255; boxG = 30; boxB = 30; // Merah Menyerang
				}
				else if (cand.isEnemy && !isDead)
				{
					boxR = 255; boxG = 50; boxB = 180; // Vibrant Pink Musuh
				}

				bool skipBoxBuffer = (g_renderedRectCount >= 265 && distMeters > 45.0f && !isCenterAimTarget);
				if (g_cfg.espBox && !skipBoxBuffer)
				{
					float thickness = 0.0015f * (float)g_cfg.espLineThickness;
					DrawEspRect(boxCenterX, boxTopY, boxW, thickness, boxR, boxG, boxB, finalAlpha);
					DrawEspRect(boxCenterX, boxBtmY, boxW, thickness, boxR, boxG, boxB, finalAlpha);
					DrawEspRect(boxCenterX - boxW * 0.5f, (boxTopY + boxBtmY) * 0.5f, thickness * 0.6f, boxH, boxR, boxG, boxB, finalAlpha);
					DrawEspRect(boxCenterX + boxW * 0.5f, (boxTopY + boxBtmY) * 0.5f, thickness * 0.6f, boxH, boxR, boxG, boxB, finalAlpha);

					if (g_cfg.espBoxFill)
					{
						DrawEspRect(boxCenterX, (boxTopY + boxBtmY) * 0.5f, boxW, boxH, boxR, boxG, boxB, ApplyEspAlpha((int)(30 * combinedAlphaMult)));
					}
				}

				int health = ENTITY::GET_ENTITY_HEALTH(ped);
				int maxHealth = ENTITY::GET_ENTITY_MAX_HEALTH(ped);
				if (maxHealth <= 0) maxHealth = 100;
				float hpPct = std::clamp((float)health / (float)maxHealth, 0.0f, 1.0f);

				int armour = PED::GET_PED_ARMOUR(ped);
				float armPct = std::clamp((float)armour / 100.0f, 0.0f, 1.0f);

				// Bar Darah Dinamis: Disembunyikan saat masih 100%, hanya muncul saat < 99%
				if (g_cfg.espHealth && !isDead && hpPct < 0.99f)
				{
					float barW = 0.0022f;
					float barX = boxCenterX - boxW * 0.5f - 0.004f;
					DrawEspRect(barX, (boxTopY + boxBtmY) * 0.5f, barW, boxH, 10, 10, 10, ApplyEspAlpha((int)(200 * combinedAlphaMult)));
					float fillH = boxH * hpPct;
					float fillY = boxBtmY - fillH * 0.5f;
					int hpR = (int)(255 * (1.0f - hpPct));
					int hpG = (int)(255 * hpPct);
					DrawEspRect(barX, fillY, barW, fillH, hpR, hpG, 20, ApplyEspAlpha((int)(255 * combinedAlphaMult)));
				}

				if (g_cfg.espArmor && armour > 0 && !isDead)
				{
					float barW = 0.0020f;
					float barX = boxCenterX - boxW * 0.5f - 0.008f;
					DrawEspRect(barX, (boxTopY + boxBtmY) * 0.5f, barW, boxH, 10, 10, 10, ApplyEspAlpha((int)(200 * combinedAlphaMult)));
					float fillH = boxH * armPct;
					float fillY = boxBtmY - fillH * 0.5f;
					DrawEspRect(barX, fillY, barW, fillH, 30, 140, 255, ApplyEspAlpha((int)(255 * combinedAlphaMult)));
				}

				std::string infoStr = "";
				if (g_cfg.espName && !tag.empty())
				{
					infoStr += "[" + tag + "] ";
				}
				if (g_cfg.espShowWeapon && !cand.heldItemName.empty() && !isDead)
				{
					infoStr += "[" + cand.heldItemName + "] ";
				}
				if (g_cfg.espDistance)
				{
					char dBuf[16];
					sprintf_s(dBuf, "%dm ", (int)std::round(distMeters));
					infoStr += dBuf;
				}
				bool inVeh = PED::IS_PED_IN_ANY_VEHICLE(ped, false) != 0;
				if (g_cfg.espShowSpeed && !isDead && !inVeh)
				{
					float pedSpdKmh = ENTITY::GET_ENTITY_SPEED(ped) * 3.6f;
					if (pedSpdKmh >= 0.8f)
					{
						char spdBuf[16];
						sprintf_s(spdBuf, "(%d km/h)", (int)std::round(pedSpdKmh));
						infoStr += spdBuf;
					}
				}

				if (!infoStr.empty())
				{
					float textPosY = (g_cfg.espTextPosition == 0) ? (boxTopY - 0.018f) : (boxBtmY + 0.004f);
					DrawTextElement(infoStr.c_str(), boxCenterX - (boxW * 0.5f), textPosY, g_cfg.espTextScale, 4,
					                boxR, boxG, boxB, ApplyEspTextAlpha((int)(245 * combinedAlphaMult)));
				}
			}
			else
			{
				// ESP Edge untuk NPC / Musuh yang berada di luar cakupan pandangan / di belakang kamera
				if (g_cfg.espEdge && distMeters <= 85.0f && !isDead)
				{
					DrawEdgeIndicator(pedCoords, r, g, b, (int)(153 * combinedAlphaMult), distMeters, tag.c_str());
				}
			}

			// E. Deteksi Aim Crosshair untuk ESP Aim Circle (Badan & Kepala)
			if (!isDead && isHuman && distMeters < 150.0f)
			{
				Vector3 headCoords = PED::GET_PED_BONE_COORDS(ped, SKEL_Head, 0.0f, 0.0f, 0.0f);
				float hSx = 0.0f, hSy = 0.0f;
				if (GRAPHICS::_WORLD3D_TO_SCREEN2D(headCoords.x, headCoords.y, headCoords.z, &hSx, &hSy))
				{
					float hDx = hSx - 0.500f;
					float hDy = (hSy - 0.500f) * aspect;
					float distHeadCenter = std::sqrt(hDx * hDx + hDy * hDy);
					float headThresh = std::clamp(0.25f / distMeters, 0.005f, 0.035f);

					if (distHeadCenter <= headThresh)
					{
						bestAimState = 2; // AIM KEPALA! Circle mengecil (siap ditembak)
					}
				}

				if (bestAimState < 2)
				{
					Vector3 pelvisCoords = PED::GET_PED_BONE_COORDS(ped, SKEL_Pelvis, 0.0f, 0.0f, 0.0f);
					float pSx = 0.0f, pSy = 0.0f;
					if (GRAPHICS::_WORLD3D_TO_SCREEN2D(pelvisCoords.x, pelvisCoords.y, pelvisCoords.z, &pSx, &pSy))
					{
						float pDx = pSx - 0.500f;
						float pDy = (pSy - 0.500f) * aspect;
						float distPelvisCenter = std::sqrt(pDx * pDx + pDy * pDy);
						float bodyThresh = std::clamp(0.60f / distMeters, 0.012f, 0.070f);

						if (distPelvisCenter <= bodyThresh)
						{
							bestAimState = 1; // AIM BADAN! Tepat sasaran
						}
					}

					if (bestAimState < 1 && isCenterAimTarget)
					{
						bestAimState = 1;
					}
				}
			}
		}

		// 2. RENDER KENDARAAN
		const int MAX_VEHS = 256;
		Vehicle rawVehs[MAX_VEHS];
		int rawVehCount = worldGetAllVehicles(rawVehs, MAX_VEHS);
		countVehicles = rawVehCount;

		if (g_cfg.espVehicles)
		{
			struct VehCandidate
			{
				Vehicle v;
				float distSq;
			};
			std::vector<VehCandidate> vehCandidates;
			vehCandidates.reserve(rawVehCount);
			for (int i = 0; i < rawVehCount; ++i)
			{
				Vehicle v = rawVehs[i];
				if (v == playerVeh) continue;
				if (!ENTITY::DOES_ENTITY_EXIST(v)) continue;
				Vector3 vc = ENTITY::GET_ENTITY_COORDS(v, TRUE);
				float dx = vc.x - playerCoords.x;
				float dy = vc.y - playerCoords.y;
				float dz = vc.z - playerCoords.z;
				float dSq = dx * dx + dy * dy + dz * dz;
				if (dSq <= maxDistSq)
				{
					vehCandidates.push_back({ v, dSq });
				}
			}
			std::sort(vehCandidates.begin(), vehCandidates.end(), [](const VehCandidate& a, const VehCandidate& b) {
				return a.distSq < b.distSq;
			});

			for (const auto& vc : vehCandidates)
			{
				Vehicle v = vc.v;
				float distSq = vc.distSq;
				float distMeters = std::sqrt(distSq);

				// Smart Distance Prioritization: jika kuota rectangle mendekati batas (>= 265), korbankan kendaraan yang jauh (> 35m) agar objek dekat tetap utuh
				if (g_renderedRectCount >= 265 && distMeters > 35.0f) break;

				bool isDead = ENTITY::IS_ENTITY_DEAD(v);
				if (isDead && g_cfg.espIgnoreDead) continue;

				Vector3 vCoords = ENTITY::GET_ENTITY_COORDS(v, TRUE);

				float vScX = 0.0f, vScY = 0.0f;
				bool vOnScreen = GRAPHICS::_WORLD3D_TO_SCREEN2D(vCoords.x, vCoords.y, vCoords.z, &vScX, &vScY);
				bool vInViewport = vOnScreen && (vScX >= -0.06f && vScX <= 1.06f && vScY >= -0.06f && vScY <= 1.06f);
				float edgeFade = 1.0f;
				if (vInViewport)
				{
					float distEdgeX = std::min(vScX - (-0.06f), 1.06f - vScX);
					float distEdgeY = std::min(vScY - (-0.06f), 1.06f - vScY);
					float minEdge = std::min(distEdgeX, distEdgeY);
					if (minEdge < 0.08f)
					{
						edgeFade = std::clamp(minEdge / 0.08f, 0.0f, 1.0f);
					}
				}

				// Screen projection & Mode Fokus untuk Kendaraan
				float focusAlphaMult = 1.0f;
				if (g_cfg.espFocus)
				{
					if (vOnScreen)
					{
						focusAlphaMult = GetFocusAlphaMultiplier(vScX, vScY);
						if (focusAlphaMult <= 0.02f) continue;
					}
					else
					{
						continue;
					}
				}

				// Animasi Transisi Kendaraan (Mulus saat perpindahan kamera)
				auto& an = g_entityAnimMap[v];
				if (vInViewport)
				{
					if (!an.wasOnScreen || (currentTick - an.lastSeenTick > 900)) an.animProgress = 0.0f;
					an.wasOnScreen = true;
					an.animProgress = std::clamp(an.animProgress + animRate * dt, 0.0f, 1.0f);
				}
				else
				{
					an.wasOnScreen = false;
					an.animProgress = 0.0f;
				}
				an.lastSeenTick = currentTick;

				float animScale = 1.0f;
				float animAlphaMult = 1.0f;
				if (g_cfg.espTransitionMode > 0)
				{
					float prog = an.animProgress;
					if (g_cfg.espTransitionMode == 1) animAlphaMult = prog;
					else if (g_cfg.espTransitionMode == 2) { animScale = EaseOutBack(prog); animAlphaMult = prog; }
					else if (g_cfg.espTransitionMode == 3) { animScale = EaseInOutCubic(prog); animAlphaMult = prog; }
				}

				float combinedAlphaMult = animAlphaMult * focusAlphaMult * masterAnimAlphaMult * edgeFade;
				if (combinedAlphaMult <= 0.02f) continue;

				int r = 255, g = 185, b = 30;
				GetEspColor(1, false, r, g, b);
				if (isDead) { r = 120; g = 120; b = 120; }

				if (g_cfg.espLines && distMeters <= (float)g_cfg.espSnaplineDist)
				{
					DrawSnapline(playerCoords, vCoords, r, g, b, 180, combinedAlphaMult);
				}

				if (g_cfg.espWheelDots && !isDead && distMeters <= 35.0f)
				{
					const char* wheelBones[] = { "wheel_lf", "wheel_rf", "wheel_lr", "wheel_rr", "wheel_lm", "wheel_rm" };
					for (const char* wName : wheelBones)
					{
						int bIdx = ENTITY::GET_ENTITY_BONE_INDEX_BY_NAME(v, (char*)wName);
						if (bIdx != -1)
						{
							Vector3 wPos = ENTITY::GET_WORLD_POSITION_OF_ENTITY_BONE(v, bIdx);
							float wSx = 0.0f, wSy = 0.0f;
							if (GRAPHICS::_WORLD3D_TO_SCREEN2D(wPos.x, wPos.y, wPos.z, &wSx, &wSy))
							{
								float dotSize = 0.0038f * (float)g_cfg.espLineThickness;
								DrawEspRect(wSx, wSy, dotSize, dotSize * 1.5f, 255, 235, 30, ApplyEspAlpha((int)(240 * combinedAlphaMult)));
							}
						}
					}
				}

				Hash model = ENTITY::GET_ENTITY_MODEL(v);
				Vector3 minDim = {}, maxDim = {};
				GAMEPLAY::GET_MODEL_DIMENSIONS(model, &minDim, &maxDim);
				if ((maxDim.z - minDim.z) < 0.5f)
				{
					minDim.x = -1.00f; maxDim.x = 1.00f;
					minDim.y = -2.20f; maxDim.y = 2.20f;
					minDim.z = -0.75f; maxDim.z = 0.85f;
				}

				Vector3 midPos = vCoords;
				midPos.z += (maxDim.z + minDim.z) * 0.5f;
				Vector3 topPos = {}; topPos.x = vCoords.x; topPos.y = vCoords.y; topPos.z = vCoords.z + maxDim.z + 0.12f;
				Vector3 btmPos = {}; btmPos.x = vCoords.x; btmPos.y = vCoords.y; btmPos.z = vCoords.z + minDim.z - 0.05f;

				float sTopX = 0.0f, sTopY = 0.0f;
				float sBtmX = 0.0f, sBtmY = 0.0f;
				float sMidX = 0.0f, sMidY = 0.0f;

				bool pTop = GRAPHICS::_WORLD3D_TO_SCREEN2D(topPos.x, topPos.y, topPos.z, &sTopX, &sTopY);
				bool pBtm = GRAPHICS::_WORLD3D_TO_SCREEN2D(btmPos.x, btmPos.y, btmPos.z, &sBtmX, &sBtmY);
				bool pMid = GRAPHICS::_WORLD3D_TO_SCREEN2D(midPos.x, midPos.y, midPos.z, &sMidX, &sMidY);

				if (pTop || pBtm || pMid)
				{
					float estH = std::clamp((maxDim.z - minDim.z + 0.17f) / distMeters, 0.008f, 1.20f);
					float boxH = 0.0f, boxCenterX = 0.0f, boxTopY = 0.0f, boxBtmY = 0.0f;

					if (pTop && pBtm)
					{
						boxH = std::abs(sBtmY - sTopY);
						boxCenterX = (sTopX + sBtmX) * 0.5f;
						boxTopY = std::min(sTopY, sBtmY);
						boxBtmY = std::max(sTopY, sBtmY);
					}
					else if (pTop && pMid)
					{
						boxH = std::abs(sMidY - sTopY) * 2.0f;
						boxCenterX = sTopX;
						boxTopY = sTopY;
						boxBtmY = sTopY + boxH;
					}
					else if (pBtm && pMid)
					{
						boxH = std::abs(sBtmY - sMidY) * 2.0f;
						boxCenterX = sBtmX;
						boxTopY = sBtmY - boxH;
						boxBtmY = sBtmY;
					}
					else if (pTop)
					{
						boxH = estH;
						boxCenterX = sTopX;
						boxTopY = sTopY;
						boxBtmY = sTopY + boxH;
					}
					else if (pBtm)
					{
						boxH = estH;
						boxCenterX = sBtmX;
						boxTopY = sBtmY - boxH;
						boxBtmY = sBtmY;
					}
					else
					{
						boxH = estH;
						boxCenterX = sMidX;
						boxTopY = sMidY - boxH * 0.5f;
						boxBtmY = sMidY + boxH * 0.5f;
					}

					float finalTop = std::min(boxTopY, boxBtmY);
					float finalBtm = std::max(boxTopY, boxBtmY);
					boxTopY = finalTop;
					boxBtmY = finalBtm;
					boxH = boxBtmY - boxTopY;
					if (boxH < 0.005f)
					{
						boxH = (estH > 0.005f) ? estH : 0.005f;
						float cY = (boxTopY + boxBtmY) * 0.5f;
						boxTopY = cY - boxH * 0.5f;
						boxBtmY = cY + boxH * 0.5f;
					}
					float boxW = boxH * g_cfg.espVehBoxWidth;

					// TERAPKAN VELOCITY SMOOTHING PADA BOX KENDARAAN:
					if (g_cfg.espVelocitySmooth > 0)
					{
						auto& sm = g_entitySmoothMap[v];
						if (!sm.initialized || (currentTick - sm.lastSeenTick > 800) ||
						    std::abs(boxCenterX - sm.curX) > 0.18f || std::abs(boxTopY - sm.curTopY) > 0.18f)
						{
							sm.curX = boxCenterX; sm.curTopY = boxTopY; sm.curBtmY = boxBtmY;
							sm.curW = boxW; sm.curH = boxH; sm.initialized = true;
						}
						else
						{
							float smoothSpeed = (g_cfg.espVelocitySmooth == 3) ? 9.0f : ((g_cfg.espVelocitySmooth == 2) ? 18.0f : 32.0f);
							float blend = std::clamp(smoothSpeed * dt, 0.05f, 1.0f);
							sm.curX += (boxCenterX - sm.curX) * blend;
							sm.curTopY += (boxTopY - sm.curTopY) * blend;
							sm.curBtmY += (boxBtmY - sm.curBtmY) * blend;
							sm.curW += (boxW - sm.curW) * blend;
							sm.curH += (boxH - sm.curH) * blend;

							boxCenterX = sm.curX;
							boxTopY = sm.curTopY;
							boxBtmY = sm.curBtmY;
							boxW = sm.curW;
							boxH = sm.curH;
						}
						sm.lastSeenTick = currentTick;
					}

					// TERAPKAN SKALA ANIMASI TRANSISI (BOUNCE POP / ELASTIC EXPAND):
					if (g_cfg.espTransitionMode > 0)
					{
						boxW *= animScale;
						boxH *= animScale;
						float cY = (boxTopY + boxBtmY) * 0.5f;
						boxTopY = cY - boxH * 0.5f;
						boxBtmY = cY + boxH * 0.5f;
					}

					int finalAlpha = ApplyEspAlpha((int)(230 * combinedAlphaMult));

					if (g_cfg.espBox)
					{
						float thickness = 0.0015f * (float)g_cfg.espLineThickness;
						DrawEspRect(boxCenterX, boxTopY, boxW, thickness, r, g, b, finalAlpha);
						DrawEspRect(boxCenterX, boxBtmY, boxW, thickness, r, g, b, finalAlpha);
						DrawEspRect(boxCenterX - boxW * 0.5f, (boxTopY + boxBtmY) * 0.5f, thickness * 0.6f, boxH, r, g, b, finalAlpha);
						DrawEspRect(boxCenterX + boxW * 0.5f, (boxTopY + boxBtmY) * 0.5f, thickness * 0.6f, boxH, r, g, b, finalAlpha);

						if (g_cfg.espBoxFill)
						{
							DrawEspRect(boxCenterX, (boxTopY + boxBtmY) * 0.5f, boxW, boxH, r, g, b, ApplyEspAlpha((int)(25 * combinedAlphaMult)));
						}
					}

					// Bar Kerusakan Kendaraan Dinamis: Disembunyikan saat masih 100%, muncul saat < 99%
					float engHp = VEHICLE::GET_VEHICLE_ENGINE_HEALTH(v);
					float engPct = std::clamp(engHp / 1000.0f, 0.0f, 1.0f);
					if (g_cfg.espVehHealth && !isDead && engPct < 0.99f)
					{
						float barW = 0.0022f;
						float barX = boxCenterX - boxW * 0.5f - 0.004f;
						DrawEspRect(barX, (boxTopY + boxBtmY) * 0.5f, barW, boxH, 10, 10, 10, ApplyEspAlpha((int)(200 * combinedAlphaMult)));
						float fillH = boxH * engPct;
						float fillY = boxBtmY - fillH * 0.5f;
						DrawEspRect(barX, fillY, barW, fillH, (int)(255 * (1.0f - engPct)), (int)(255 * engPct), 30, ApplyEspAlpha((int)(255 * combinedAlphaMult)));
					}

					std::string vName = "";
					if (g_cfg.espName)
					{
						char* gxt = VEHICLE::GET_DISPLAY_NAME_FROM_VEHICLE_MODEL(model);
						if (gxt)
						{
							char* loc = UI::_GET_LABEL_TEXT(gxt);
							vName = (loc && loc[0] != '\0' && strcmp(loc, "NULL") != 0) ? loc : gxt;
						}
					}

					std::string infoStr = "";
					if (!vName.empty()) infoStr += "[" + vName + "] ";
					if (g_cfg.espDistance)
					{
						char dBuf[16];
						sprintf_s(dBuf, "%dm ", (int)std::round(distMeters));
						infoStr += dBuf;
					}
					if (g_cfg.espShowSpeed)
					{
						float vSpdKmh = ENTITY::GET_ENTITY_SPEED(v) * 3.6f;
						if (vSpdKmh >= 1.0f)
						{
							char spdBuf[16];
							sprintf_s(spdBuf, "(%d km/h)", (int)std::round(vSpdKmh));
							infoStr += spdBuf;
						}
					}

					if (!infoStr.empty())
					{
						float textPosY = (g_cfg.espTextPosition == 0) ? (boxTopY - 0.018f) : (boxBtmY + 0.004f);
						DrawTextElement(infoStr.c_str(), boxCenterX - (boxW * 0.5f), textPosY, g_cfg.espTextScale, 4,
						                r, g, b, ApplyEspTextAlpha((int)(245 * combinedAlphaMult)));
					}
				}
			}
		}

		// 3. RENDER ITEM / PICKUPS (Mendeteksi Pickup Pool & World Object Drops)
		countItems = 0;
		if (g_cfg.espItems)
		{
			struct ItemEntry
			{
				Vector3 coords;
				std::string label;
				int r, g, b;
			};
			std::vector<ItemEntry> activeItems;

			// A. Cek Pickup Pool ScriptHookV (Buffer dinaikkan ke 512)
			const int MAX_PICKUPS = 512;
			Pickup pickups[MAX_PICKUPS];
			int pCount = worldGetAllPickups(pickups, MAX_PICKUPS);
			for (int i = 0; i < pCount; ++i)
			{
				Pickup p = pickups[i];
				if (OBJECT::DOES_PICKUP_EXIST(p) || OBJECT::DOES_PICKUP_OBJECT_EXIST(p))
				{
					Vector3 pCoords = OBJECT::GET_PICKUP_COORDS(p);
					if (pCoords.x != 0.0f || pCoords.y != 0.0f || pCoords.z != 0.0f)
					{
						activeItems.push_back({ pCoords, "ITEM", 0, 220, 255 });
					}
				}
			}

			// B. Cek World Objects Pool (Uang Jatuh NPC, Senjata Tercecer, Medkit, Armor, dll)
			const int MAX_OBJS = 2048;
			Object objs[MAX_OBJS];
			int oCount = worldGetAllObjects(objs, MAX_OBJS);
			for (int i = 0; i < oCount; ++i)
			{
				Object obj = objs[i];
				if (!ENTITY::DOES_ENTITY_EXIST(obj)) continue;
				if (ENTITY::IS_ENTITY_ATTACHED(obj)) continue; // Abaikan yang sedang dipegang di tangan

				bool isPickupObj = OBJECT::DOES_PICKUP_OBJECT_EXIST(obj) != 0;
				std::string itemType = "ITEM";
				int ir = 0, ig = 220, ib = 255;

				Hash m = ENTITY::GET_ENTITY_MODEL(obj);
				if (s_cashModels.find(m) != s_cashModels.end())
				{
					isPickupObj = true;
					itemType = "UANG";
					ir = 255; ig = 215; ib = 0; // Emas / Kuning Keemasan
				}
				else if (IsWeaponModel(m))
				{
					isPickupObj = true;
					itemType = "SENJATA";
					ir = 255; ig = 140; ib = 20; // Oranye Senjata
				}
				else if (m == 0x4B370932 /* prop_armour_pickup */)
				{
					isPickupObj = true;
					itemType = "ARMOR";
					ir = 50; ig = 150; ib = 255; // Biru Armor
				}
				else if (m == 0xC1C81155 /* prop_med_bag_01 */)
				{
					isPickupObj = true;
					itemType = "MEDKIT";
					ir = 50; ig = 230; ib = 80; // Hijau Medkit
				}
				else if (m == 0x8F747065 /* p_parachute_s */)
				{
					isPickupObj = true;
					itemType = "PARASUT";
					ir = 200; ig = 200; ib = 255;
				}

				if (isPickupObj)
				{
					Vector3 oCoords = ENTITY::GET_ENTITY_COORDS(obj, TRUE);
					// Filter jarak dekat terhadap item lain untuk menghindari duplikasi
					bool duplicate = false;
					for (const auto& existing : activeItems)
					{
						float edx = existing.coords.x - oCoords.x;
						float edy = existing.coords.y - oCoords.y;
						float edz = existing.coords.z - oCoords.z;
						if ((edx * edx + edy * edy + edz * edz) < 1.0f)
						{
							duplicate = true;
							break;
						}
					}
					if (!duplicate)
					{
						activeItems.push_back({ oCoords, itemType, ir, ig, ib });
					}
				}
			}

			countItems = (int)activeItems.size();

			// Smart Distance Prioritization: Urutkan item dari yang paling dekat dengan player
			std::sort(activeItems.begin(), activeItems.end(), [&](const ItemEntry& a, const ItemEntry& b) {
				float dxa = a.coords.x - playerCoords.x, dya = a.coords.y - playerCoords.y, dza = a.coords.z - playerCoords.z;
				float dxb = b.coords.x - playerCoords.x, dyb = b.coords.y - playerCoords.y, dzb = b.coords.z - playerCoords.z;
				return (dxa * dxa + dya * dya + dza * dza) < (dxb * dxb + dyb * dyb + dzb * dzb);
			});

			for (const auto& itemData : activeItems)
			{
				Vector3 pCoords = itemData.coords;
				float dx = pCoords.x - playerCoords.x;
				float dy = pCoords.y - playerCoords.y;
				float dz = pCoords.z - playerCoords.z;
				float distSq = dx * dx + dy * dy + dz * dz;
				if (distSq > maxDistSq) continue;
				float distMeters = std::sqrt(distSq);

				// Smart Distance Prioritization: korbankan item jauh jika buffer rect hampir penuh
				if (g_renderedRectCount >= 270 && distMeters > 30.0f) break;

				int r = itemData.r, g = itemData.g, b = itemData.b;

				float itemAlphaMult = masterAnimAlphaMult;
				if (g_cfg.espFocus)
				{
					float iScX = 0.0f, iScY = 0.0f;
					if (GRAPHICS::_WORLD3D_TO_SCREEN2D(pCoords.x, pCoords.y, pCoords.z, &iScX, &iScY))
					{
						itemAlphaMult *= GetFocusAlphaMultiplier(iScX, iScY);
						if (itemAlphaMult <= 0.02f) continue;
					}
					else continue;
				}

				if (g_cfg.espLines)
				{
					DrawSnapline(playerCoords, pCoords, r, g, b, 175, itemAlphaMult);
				}

				float sX = 0.0f, sY = 0.0f;
				if (GRAPHICS::_WORLD3D_TO_SCREEN2D(pCoords.x, pCoords.y, pCoords.z, &sX, &sY))
				{
					float dotSize = 0.0040f * (float)g_cfg.espLineThickness;
					DrawEspRect(sX, sY, dotSize, dotSize * 1.5f, r, g, b, ApplyEspAlpha((int)(245 * itemAlphaMult)));
					char itemBuf[32];
					sprintf_s(itemBuf, "[%s] %dm", itemData.label.c_str(), (int)std::round(distMeters));
					DrawTextElement(itemBuf, sX, sY - 0.015f, g_cfg.espTextScale, 4, r, g, b, ApplyEspTextAlpha((int)(240 * itemAlphaMult)));
				}
			}
		}

		// 4. DISPLAY INFO JUMLAH ENTITAS & RECTANGLE BUFFER IN-GAME (Tengah Atas, Tanpa Background, Shadow Kontras)
		if (g_cfg.espShowCounters)
		{
			char entBuf[160];
			sprintf_s(entBuf, "NPC: %d (MUSUH: %d) | MOBIL: %d | HEWAN: %d | ITEM: %d | RECT: %d/300",
			          countPeds, countEnemies, countVehicles, countAnimals, countItems, g_renderedRectCount);

			float scale = g_cfg.espCounterScale;
			float textW = GetTextWidth(entBuf, scale, 4);
			float drawX = 0.500f - (textW * 0.5f);
			float drawY = 0.014f;

			// Indikator status buffer 2D: normal (putih-biru), peringatan (kuning >=220), bahaya drop (merah >=270)
			int cr = 235, cg = 245, cb = 255;
			if (g_renderedRectCount >= 270)
			{
				cr = 255; cg = 60; cb = 60;
			}
			else if (g_renderedRectCount >= 220)
			{
				cr = 255; cg = 195; cb = 35;
			}

			DrawTextElement(entBuf, drawX, drawY, scale, 4, cr, cg, cb, ApplyEspTextAlpha(255));
		}

		// Pembersihan berkala data entitas yang sudah tidak aktif / hilang
		static ULONGLONG s_lastMapCleanupTick = 0;
		if (currentTick - s_lastMapCleanupTick > 4000)
		{
			s_lastMapCleanupTick = currentTick;
			for (auto it = g_entitySmoothMap.begin(); it != g_entitySmoothMap.end(); )
			{
				if (currentTick - it->second.lastSeenTick > 2500) it = g_entitySmoothMap.erase(it);
				else ++it;
			}
			for (auto it = g_entityAnimMap.begin(); it != g_entityAnimMap.end(); )
			{
				if (currentTick - it->second.lastSeenTick > 2500) it = g_entityAnimMap.erase(it);
				else ++it;
			}
			for (auto it = g_lastPedHealth.begin(); it != g_lastPedHealth.end(); )
			{
				if (!ENTITY::DOES_ENTITY_EXIST(it->first) || ENTITY::IS_ENTITY_DEAD(it->first))
					it = g_lastPedHealth.erase(it);
				else
					++it;
			}
		}

		g_aimTargetState = bestAimState;

		// 5. RENDER ESP KILL SATISFYING BANNER
		DrawKillSatisfyingBanner(dt, currentTick);

		// 6. RENDER FLOATING DAMAGE NUMBERS
		DrawDamageIndicators(currentTick);
	}

	void ScriptMain()
	{
		LoadConfig();

		g_lastTickTime = GetTickCount64();
		g_fpsLastTime = g_lastTickTime;
		g_lastSpeedSampleTime = g_lastTickTime;

		while (true)
		{
			scriptWait(0);

			ULONGLONG currentTick = GetTickCount64();
			float dt = (float)(currentTick - g_lastTickTime) / 1000.0f;
			if (dt <= 0.0f || dt > 0.1f) dt = 0.016f;
			g_lastTickTime = currentTick;

			// Perhitungan FPS Real-Time
			g_fpsFrames++;
			if (currentTick - g_fpsLastTime >= 500)
			{
				g_fps = (g_fpsFrames * 1000.0f) / (float)(currentTick - g_fpsLastTime);
				g_fpsFrames = 0;
				g_fpsLastTime = currentTick;
			}

			// 1. Shortcut Buka / Tutup Menu AFOUND (Klik F12 Langsung, Tanpa Perlu Tahan/Hold)
			if (g_keyJustPressed[g_cfg.menuKey])
			{
				g_keyJustPressed[g_cfg.menuKey] = false;
				g_menuOpen = !g_menuOpen;
				SaveConfig();
				PlayFrontendSound(g_menuOpen ? "SELECT" : "BACK");
			}

			// 2. Shortcut Toggle ESP Master On / Off (Keybind: 'L')
			if (g_keyJustPressed[g_cfg.espToggleKey])
			{
				g_keyJustPressed[g_cfg.espToggleKey] = false;
				g_cfg.espMaster = !g_cfg.espMaster;
				SaveConfig();
				PlayFrontendSound(g_cfg.espMaster ? "SELECT" : "BACK");
			}

			// 3. Shortcut Toggle Show / Hide Speedometer (Default: ']')
			if (g_keyJustPressed[g_cfg.speedoToggleKey])
			{
				g_keyJustPressed[g_cfg.speedoToggleKey] = false;
				g_cfg.showSpeedo = !g_cfg.showSpeedo;
				SaveConfig();
				PlayFrontendSound(g_cfg.showSpeedo ? "SELECT" : "BACK");
			}

			// Kontrol Game Saat Menu Terbuka (Blokir tombol HP saja)
			if (g_menuOpen)
			{
				CONTROLS::DISABLE_CONTROL_ACTION(0, 27, TRUE);
				CONTROLS::DISABLE_CONTROL_ACTION(0, 172, TRUE);
				CONTROLS::DISABLE_CONTROL_ACTION(0, 173, TRUE);
				CONTROLS::DISABLE_CONTROL_ACTION(0, 174, TRUE);
				CONTROLS::DISABLE_CONTROL_ACTION(0, 175, TRUE);
				CONTROLS::DISABLE_CONTROL_ACTION(0, 176, TRUE);
				CONTROLS::DISABLE_CONTROL_ACTION(0, 177, TRUE);
				CONTROLS::DISABLE_CONTROL_ACTION(0, 180, TRUE);
				CONTROLS::DISABLE_CONTROL_ACTION(0, 181, TRUE);

				if (MOBILE::CAN_PHONE_BE_SEEN_ON_SCREEN())
				{
					MOBILE::DESTROY_MOBILE_PHONE();
				}

				// Cek jika Pause Menu aktif atau pengguna membuka menu mod lain (F1, F3-F8, F10, Insert, Numpad)
				// Sembunyikan otomatis menu AFOUND agar tidak terjadi tabrakan kontrol (double control)
				if (UI::IS_PAUSE_MENU_ACTIVE())
				{
					g_menuOpen = false;
				}
				else
				{
					const DWORD otherModKeys[] = {
						VK_F1, VK_F3, VK_F4, VK_F5, VK_F6, VK_F7, VK_F8, VK_F9, VK_F10, VK_F11,
						VK_INSERT, VK_MULTIPLY, VK_SUBTRACT,
						VK_NUMPAD0, VK_NUMPAD1, VK_NUMPAD2, VK_NUMPAD3, VK_NUMPAD4, VK_NUMPAD5, VK_NUMPAD6, VK_NUMPAD7, VK_NUMPAD8, VK_NUMPAD9
					};
					for (DWORD omk : otherModKeys)
					{
						if (omk == g_cfg.menuKey || omk == g_cfg.espToggleKey || omk == g_cfg.speedoToggleKey)
							continue;

						if (g_keyJustPressed[omk])
						{
							g_keyJustPressed[omk] = false;
							g_menuOpen = false;
							SaveConfig();
							PlayFrontendSound("BACK");
							break;
						}
					}
				}
			}

			// Navigasi Menu dengan Key Repeat Hold
			if (g_menuOpen)
			{
				DWORD navKeys[] = { VK_UP, VK_DOWN, VK_LEFT, VK_RIGHT };
				for (DWORD k : navKeys)
				{
					if (g_keyJustPressed[k])
					{
						g_keyJustPressed[k] = false;
						HandleMenuKeyPress(k);
						g_keyHoldStartTick[k] = currentTick;
						g_keyLastRepeatTick[k] = currentTick;
					}
					else if (g_isKeyDown[k])
					{
						ULONGLONG heldMs = currentTick - g_keyHoldStartTick[k];
						if (heldMs >= 320 && (currentTick - g_keyLastRepeatTick[k] >= 65))
						{
							g_keyLastRepeatTick[k] = currentTick;
							HandleMenuKeyPress(k);
						}
					}
				}

				DWORD actionKeys[] = { VK_RETURN, VK_ESCAPE, VK_BACK, VK_DELETE };
				for (DWORD k : actionKeys)
				{
					if (g_keyJustPressed[k])
					{
						g_keyJustPressed[k] = false;
						HandleMenuKeyPress(k);
					}
				}
			}

			// Render Menu AFOUND MODS
			DrawMenu();

			// Ambil Ped Pemain
			Ped playerPed = PLAYER::PLAYER_PED_ID();
			if (!ENTITY::DOES_ENTITY_EXIST(playerPed) || ENTITY::IS_ENTITY_DEAD(playerPed))
			{
				g_killStreakCount = 0;
				continue;
			}

			// Render ESP (Wallhack & Visuals)
			RenderESP(playerPed, currentTick, dt);

			// Cek Status Pemain & Kendaraan
			bool inVehicle = false;
			Vehicle veh = 0;

			if (PED::IS_PED_IN_ANY_VEHICLE(playerPed, false))
			{
				veh = PED::GET_VEHICLE_PED_IS_USING(playerPed);
				if (ENTITY::DOES_ENTITY_EXIST(veh))
				{
					inVehicle = true;
				}
			}

			// -------------------------------------------------------------
			// A. FPS COUNTER, JAM:MENIT & HARDWARE MONITOR (RAM, CPU, GPU, DISK)
			// -------------------------------------------------------------
			if (!CUTSCENE::IS_CUTSCENE_PLAYING() && !UI::IS_HUD_HIDDEN())
			{
				UpdateHardwareStats(currentTick);

				int hrs = TIME::GET_CLOCK_HOURS();
				int mins = TIME::GET_CLOCK_MINUTES();
				char clockBuf[16];
				sprintf_s(clockBuf, "%02d:%02d", hrs, mins);

				std::string hudTopText = "";
				if (g_cfg.showFps)
				{
					char fpsBuf[32];
					sprintf_s(fpsBuf, "%d FPS", (int)std::round(g_fps));
					hudTopText += fpsBuf;
				}
				if (g_cfg.showClock)
				{
					if (!hudTopText.empty()) hudTopText += "  |  ";
					hudTopText += clockBuf;
				}
				if (g_cfg.showAltitude)
				{
					Vector3 pCoords = ENTITY::GET_ENTITY_COORDS(playerPed, TRUE);
					char altBuf[32];
					sprintf_s(altBuf, "ALT: %.0fm", pCoords.z);
					if (!hudTopText.empty()) hudTopText += "  |  ";
					hudTopText += altBuf;
				}
				if (!inVehicle && g_cfg.showOnFootSpeed)
				{
					float speedMs = ENTITY::GET_ENTITY_SPEED(playerPed);
					float footKmh = speedMs * 3.6f;
					char spdBuf[32];
					sprintf_s(spdBuf, "SPD: %.0f KM/H", footKmh);
					if (!hudTopText.empty()) hudTopText += "  |  ";
					hudTopText += spdBuf;
				}

				// Baris Informasi Hardware System Monitor (Jam Real Laptop di sebelah kiri CPU/GPU)
				std::string hwLine = "";
				if (g_cfg.showRealTime)
				{
					SYSTEMTIME st;
					GetLocalTime(&st);
					char realTimeBuf[16];
					sprintf_s(realTimeBuf, "%02d:%02d", st.wHour, st.wMinute);
					hwLine += realTimeBuf;
				}
				if (g_cfg.showCpu)
				{
					char buf[32];
					sprintf_s(buf, "CPU: %.0f%%", g_cpuPercent);
					if (!hwLine.empty()) hwLine += "  |  ";
					hwLine += buf;
				}
				if (g_cfg.showGpu)
				{
					char buf[48];
					if (g_gpuVramTotalMB > 0)
						sprintf_s(buf, "GPU: %.1fGB (%.0f%%)", (float)g_gpuVramUsedMB / 1024.0f, g_gpuVramPercent);
					else
						sprintf_s(buf, "GPU: %.0f%%", g_gpuVramPercent);
					if (!hwLine.empty()) hwLine += "  |  ";
					hwLine += buf;
				}
				if (g_cfg.showRam)
				{
					char buf[48];
					sprintf_s(buf, "RAM: %.1fGB/%.1fGB (%d%%)", g_ramUsedGB, g_ramTotalGB, g_ramPercent);
					if (!hwLine.empty()) hwLine += "  |  ";
					hwLine += buf;
				}
				if (g_cfg.showDisk)
				{
					char buf[48];
					sprintf_s(buf, "DISK: %.0fGB/%.0fGB (%d%%)", g_diskUsedGB, g_diskTotalGB, g_diskPercent);
					if (!hwLine.empty()) hwLine += "  |  ";
					hwLine += buf;
				}

				float baseLineY = inVehicle ? (g_cfg.posY - 0.026f) : g_cfg.posY;
				if (!hwLine.empty() && !hudTopText.empty())
				{
					// Jika keduanya aktif, susun dua baris bertumpuk rapi dengan drop shadow kontras
					DrawTextElement(hwLine.c_str(), g_cfg.posX, baseLineY - 0.022f, g_cfg.unitScale * 0.85f, 4,
					                g_cfg.textColorR, g_cfg.textColorG, g_cfg.textColorB, g_cfg.hudTextOpacity, g_cfg.hudTextOutline);
					DrawTextElement(hudTopText.c_str(), g_cfg.posX, baseLineY, g_cfg.unitScale * 0.95f, 4,
					                g_cfg.textColorR, g_cfg.textColorG, g_cfg.textColorB, g_cfg.hudTextOpacity, g_cfg.hudTextOutline);
				}
				else if (!hudTopText.empty())
				{
					DrawTextElement(hudTopText.c_str(), g_cfg.posX, baseLineY, g_cfg.unitScale * 0.95f, 4,
					                g_cfg.textColorR, g_cfg.textColorG, g_cfg.textColorB, g_cfg.hudTextOpacity, g_cfg.hudTextOutline);
				}
				else if (!hwLine.empty())
				{
					DrawTextElement(hwLine.c_str(), g_cfg.posX, baseLineY, g_cfg.unitScale * 0.85f, 4,
					                g_cfg.textColorR, g_cfg.textColorG, g_cfg.textColorB, g_cfg.hudTextOpacity, g_cfg.hudTextOutline);
				}
			}

			// -------------------------------------------------------------
			// B. PENGAMBILAN DATA KENDARAAN & KECEPATAN
			// -------------------------------------------------------------
			// Main speedometer cluster HANYA aktif saat di dalam kendaraan (on-foot cukup teks di sebelah ALT/TIME)
			bool shouldShow = g_cfg.showSpeedo && inVehicle && !CUTSCENE::IS_CUTSCENE_PLAYING() && !UI::IS_HUD_HIDDEN();
			float actualKmh = 0.0f;
			Vector3 speedVec = {};
			std::string vehName = "";
			float fuelPct = 100.0f;
			float engHealthPct = 100.0f;

			if (inVehicle)
			{
				float speedMs = ENTITY::GET_ENTITY_SPEED(veh);
				actualKmh = speedMs * 3.6f;
				speedVec = ENTITY::GET_ENTITY_SPEED_VECTOR(veh, true);

				float petrolTank = VEHICLE::GET_VEHICLE_PETROL_TANK_HEALTH(veh);
				fuelPct = std::clamp((petrolTank / 1000.0f) * 100.0f, 0.0f, 100.0f);

				float engHealth = VEHICLE::GET_VEHICLE_ENGINE_HEALTH(veh);
				engHealthPct = std::clamp((engHealth / 1000.0f) * 100.0f, 0.0f, 100.0f);

				Hash model = ENTITY::GET_ENTITY_MODEL(veh);
				char* modelGxt = VEHICLE::GET_DISPLAY_NAME_FROM_VEHICLE_MODEL(model);
				if (modelGxt)
				{
					char* localizedName = UI::_GET_LABEL_TEXT(modelGxt);
					if (localizedName && localizedName[0] != '\0' && strcmp(localizedName, "NULL") != 0)
						vehName = localizedName;
					else
						vehName = modelGxt;
				}
				for (auto& c : vehName)
					c = (char)std::toupper((unsigned char)c);
			}
			else if (g_cfg.showOnFootSpeed)
			{
				float speedMs = ENTITY::GET_ENTITY_SPEED(playerPed);
				actualKmh = speedMs * 3.6f;
				speedVec = ENTITY::GET_ENTITY_SPEED_VECTOR(playerPed, true);
				vehName = "ON FOOT";
			}

			// -------------------------------------------------------------
			// C. LOGIKA MODE DINAMIS (0 KM/H AUTO-HIDE & CENTER)
			// -------------------------------------------------------------
			if (inVehicle || (g_cfg.showOnFootSpeed && shouldShow))
			{
				if (g_cfg.autoHideAtZero && actualKmh <= 0.5f)
				{
					if (g_stoppedSince == 0) g_stoppedSince = currentTick;
					if (currentTick - g_stoppedSince >= 3000)
					{
						g_stoppedDimMultiplier -= 2.2f * dt;
						if (g_stoppedDimMultiplier < 0.0f) g_stoppedDimMultiplier = 0.0f;
					}
				}
				else
				{
					g_stoppedSince = 0;
					g_stoppedDimMultiplier += 4.5f * dt;
					if (g_stoppedDimMultiplier > 1.0f) g_stoppedDimMultiplier = 1.0f;
				}

				bool centerModeEnabled = inVehicle && (g_cfg.centerSpeedThreshold > 0);
				float threshKmh = (float)g_cfg.centerSpeedThreshold;
				ULONGLONG cooldownMs = (ULONGLONG)g_cfg.centerCooldownSec * 1000;

				if (centerModeEnabled)
				{
					if (!g_isCenterMode)
					{
						if (actualKmh >= threshKmh)
						{
							if (currentTick - g_lastModeSwitchTick >= cooldownMs)
							{
								g_isCenterMode = true;
								g_lastModeSwitchTick = currentTick;
								g_belowThreshSince = 0;
							}
						}
					}
					else
					{
						if (actualKmh < threshKmh)
						{
							if (g_belowThreshSince == 0) g_belowThreshSince = currentTick;
							if ((currentTick - g_belowThreshSince >= 3000) && (currentTick - g_lastModeSwitchTick >= cooldownMs))
							{
								g_isCenterMode = false;
								g_lastModeSwitchTick = currentTick;
								g_belowThreshSince = 0;
							}
						}
						else
						{
							g_belowThreshSince = 0;
						}
					}
				}
				else
				{
					g_isCenterMode = false;
					g_belowThreshSince = 0;
				}
			}
			else
			{
				g_stoppedSince = 0;
				g_stoppedDimMultiplier = 1.0f;
				g_isCenterMode = false;
				g_belowThreshSince = 0;
			}

			// -------------------------------------------------------------
			// D. TRANSISI ANIMASI DENGAN EASING
			// -------------------------------------------------------------
			float targetCenterBlend = g_isCenterMode ? 1.0f : 0.0f;
			if (g_centerBlend < targetCenterBlend)
			{
				g_centerBlend += 2.4f * dt;
				if (g_centerBlend > targetCenterBlend) g_centerBlend = targetCenterBlend;
			}
			else if (g_centerBlend > targetCenterBlend)
			{
				g_centerBlend -= 2.4f * dt;
				if (g_centerBlend < targetCenterBlend) g_centerBlend = targetCenterBlend;
			}

			float easedProgress;
			if (g_isCenterMode)
			{
				easedProgress = EaseOutBack(g_centerBlend);
			}
			else
			{
				easedProgress = EaseInOutCubic(g_centerBlend);
			}

			float progressClamped = std::clamp(easedProgress, 0.0f, 1.0f);

			float baseTargetAlpha = shouldShow ? ((float)g_cfg.textOpacity * g_stoppedDimMultiplier) : 0.0f;
			if (g_currentAlpha < baseTargetAlpha)
			{
				g_currentAlpha += 850.0f * dt;
				if (g_currentAlpha > baseTargetAlpha) g_currentAlpha = baseTargetAlpha;
			}
			else if (g_currentAlpha > baseTargetAlpha)
			{
				g_currentAlpha -= 850.0f * dt;
				if (g_currentAlpha < baseTargetAlpha) g_currentAlpha = baseTargetAlpha;
			}

			if (g_currentAlpha <= 1.0f)
			{
				g_dampedSpeed = 0.0f;
				continue;
			}

			int currentAlphaInt = (int)g_currentAlpha;

			// Pembaruan Angka Kecepatan (Interval ms)
			float speedDiff = actualKmh - g_dampedSpeed;
			g_dampedSpeed += speedDiff * std::min(1.0f, 16.0f * dt);

			if (currentTick - g_lastSpeedSampleTime >= (ULONGLONG)g_cfg.speedIntervalMs)
			{
				g_lastSpeedSampleTime = currentTick;
				int newIntSpeed = (int)std::round(g_dampedSpeed);
				if (newIntSpeed < 0) newIntSpeed = 0;

				if (newIntSpeed != g_displaySpeed)
				{
					g_speedPulseTimer = 0.10f;
					g_displaySpeed = newIntSpeed;
				}
			}

			float speedScaleMultiplier = 1.0f;
			if (g_speedPulseTimer > 0.0f)
			{
				g_speedPulseTimer -= dt;
				float prog = g_speedPulseTimer / 0.10f;
				speedScaleMultiplier = 1.0f + 0.040f * std::sin(prog * 3.14159f);
			}

			// Deteksi Gigi Transmisi / Status Bergerak Karakter
			std::string newGearStr = "N";
			int gearR = g_cfg.textColorR, gearG = g_cfg.textColorG, gearB = g_cfg.textColorB;

			if (!inVehicle)
			{
				if (actualKmh <= 0.8f)       newGearStr = "P";
				else if (actualKmh < 9.0f)   newGearStr = "W"; // Walk
				else if (actualKmh < 18.0f)  newGearStr = "R"; // Run
				else                         newGearStr = "S"; // Sprint
			}
			else if (speedVec.y < -0.3f && actualKmh > 1.5f)
			{
				newGearStr = "R";
				gearR = 255; gearG = 215; gearB = 20; // Kuning Cerah
			}
			else if (actualKmh <= 1.2f)
			{
				newGearStr = "N";
			}
			else
			{
				if (actualKmh < 28.0f)       newGearStr = "1";
				else if (actualKmh < 58.0f)  newGearStr = "2";
				else if (actualKmh < 95.0f)  newGearStr = "3";
				else if (actualKmh < 140.0f) newGearStr = "4";
				else if (actualKmh < 190.0f) newGearStr = "5";
				else                         newGearStr = "6";
			}

			if (newGearStr != g_lastGearStr)
			{
				// Deteksi transisi naik ke gigi berikutnya (Upshift)
				bool isUpshift = false;
				if (isdigit(newGearStr[0]))
				{
					if (g_lastGearStr == "N" || (isdigit(g_lastGearStr[0]) && newGearStr[0] > g_lastGearStr[0]))
					{
						isUpshift = true;
					}
				}

				if (isUpshift)
				{
					// Transisi Kuning untuk KM/H, Angka Speedometer, dan Angka Gigi
					g_kmhFlashTimer = 0.50f;

					// Animasi Kinclong khusus upshift 3->4 dan 4->5
					if ((g_lastGearStr == "3" && newGearStr == "4") || (g_lastGearStr == "4" && newGearStr == "5"))
					{
						g_shimmerTimer = 0.45f;
						PlayFrontendSound("RACE_PLACED");
					}
				}

				g_gearAnimTimer = 0.22f;
				g_lastGearStr = newGearStr;
			}
			g_currentGearStr = newGearStr;

			float gearScaleMultiplier = 1.0f;
			float gearYOffset = 0.0f;
			if (g_gearAnimTimer > 0.0f)
			{
				g_gearAnimTimer -= dt;
				float prog = g_gearAnimTimer / 0.22f;
				gearScaleMultiplier = 1.0f + 0.16f * std::sin(prog * 3.14159f);
				gearYOffset = -0.0032f * std::sin(prog * 3.14159f);
			}

			// Efek Animasi Kinclong pada Teks (Warna Emas Berkilau & Micro Bounce)
			int speedTextR = g_cfg.textColorR;
			int speedTextG = g_cfg.textColorG;
			int speedTextB = g_cfg.textColorB;
			float shimmerScaleMult = 1.0f;

			if (g_shimmerTimer > 0.0f)
			{
				g_shimmerTimer -= dt;
				float prog = 1.0f - (g_shimmerTimer / 0.45f);
				float shineWave = std::sin(prog * 3.14159f);

				speedTextR = std::min(255, (int)(g_cfg.textColorR + (255 - g_cfg.textColorR) * shineWave));
				speedTextG = std::min(255, (int)(g_cfg.textColorG + (255 - g_cfg.textColorG) * shineWave * 0.95f));
				speedTextB = std::min(255, (int)(g_cfg.textColorB + (140 * shineWave)));

				shimmerScaleMult = 1.0f + 0.075f * shineWave;
			}

			// Warna Teks KM/H, Angka Speedometer, dan Angka Gigi (Transisi Kuning lalu Fade ke Putih)
			int kmhR = g_cfg.textColorR;
			int kmhG = g_cfg.textColorG;
			int kmhB = g_cfg.textColorB;

			if (g_kmhFlashTimer > 0.0f)
			{
				g_kmhFlashTimer -= dt;
				float p = std::clamp(g_kmhFlashTimer / 0.50f, 0.0f, 1.0f);
				int flashR = (int)(g_cfg.textColorR * (1.0f - p) + 255 * p);
				int flashG = (int)(g_cfg.textColorG * (1.0f - p) + 220 * p);
				int flashB = (int)(g_cfg.textColorB * (1.0f - p) + 0 * p);

				kmhR = flashR;
				kmhG = flashG;
				kmhB = flashB;

				// Angka speedometer juga ikut transisi kuning bersama KM/H
				speedTextR = flashR;
				speedTextG = flashG;
				speedTextB = flashB;

				// Angka gigi juga ikut transisi kuning bersama KM/H
				if (newGearStr != "R")
				{
					gearR = flashR;
					gearG = flashG;
					gearB = flashB;
				}
			}

			// -------------------------------------------------------------
			// JIKA GAYA SPEEDOMETER DIPILIH FORZA HORIZON (KANAN BAWAH)
			// -------------------------------------------------------------
			if (g_cfg.speedometerStyle == 1)
			{
				DrawForzaSpeedometer(veh, actualKmh, currentAlphaInt, dt);
			}
			else
			{
				// -------------------------------------------------------------
				// GAYA MINIMALIS (ATAS MINIMAP / CENTER DINAMIS)
				// -------------------------------------------------------------
				float miniX = g_cfg.posX;
				float miniY = g_cfg.posY;
				float miniSpeedScale = g_cfg.speedScale;
				float miniUnitScale = g_cfg.unitScale;
				float miniGearScale = g_cfg.gearScale;

				float centerSpeedScale = g_cfg.centerSpeedScale;
				float centerGearScale = (g_cfg.gearScale / std::max(0.01f, g_cfg.speedScale)) * g_cfg.centerSpeedScale;
				float centerUnitScale = g_cfg.unitScale * 1.15f;

				float effSpeedScale = (miniSpeedScale + (centerSpeedScale - miniSpeedScale) * progressClamped) * speedScaleMultiplier * shimmerScaleMult;
				float effUnitScale = miniUnitScale + (centerUnitScale - miniUnitScale) * progressClamped;
				float effGearScale = (miniGearScale + (centerGearScale - miniGearScale) * progressClamped) * gearScaleMultiplier;

				char speedBuf[16];
				sprintf_s(speedBuf, "%d", g_displaySpeed);
				float speedW = GetTextWidth(speedBuf, effSpeedScale, 4);
				float speedSlotW = 0.038f * (1.0f + 0.35f * progressClamped);

				float kmhW = GetTextWidth("KM/H", effUnitScale, 4);
				float gearGap = 0.014f + 0.006f * progressClamped;
				float gearW = g_cfg.showGear ? GetTextWidth(g_currentGearStr.c_str(), effGearScale, 4) : 0.0f;

				float totalClusterW = speedSlotW + 0.004f + kmhW + (g_cfg.showGear ? (gearGap + gearW) : 0.0f);
				float centerStartX = g_cfg.centerPosX - (totalClusterW * 0.5f);
				float centerY = g_cfg.centerPosY;

				float drawX = miniX + (centerStartX - miniX) * easedProgress;
				float drawY = miniY + (centerY - miniY) * easedProgress;

				// 1. Slot Angka Kecepatan
				float speedDrawX = (drawX + speedSlotW) - speedW;
				DrawTextElement(speedBuf, speedDrawX, drawY, effSpeedScale, 4,
				                speedTextR, speedTextG, speedTextB, currentAlphaInt);

				// 2. Slot Teks "KM/H" (Dengan Fade Kuning)
				float kmhX = drawX + speedSlotW + 0.004f;
				float kmhYOffset = 0.013f * (effSpeedScale / 0.70f);
				DrawTextElement("KM/H", kmhX, drawY + kmhYOffset, effUnitScale, 4,
				                kmhR, kmhG, kmhB, currentAlphaInt);

				// 3. Slot Gigi Transmisi
				float gearYBase = drawY + (0.002f * (1.0f - progressClamped)) + gearYOffset;
				float gearX = kmhX + kmhW + gearGap;
				if (g_cfg.showGear)
				{
					DrawTextElement(g_currentGearStr.c_str(), gearX, gearYBase, effGearScale, 4,
					                gearR, gearG, gearB, currentAlphaInt);
				}

				// Elemen Sekunder di Minimap (Nama Kendaraan, Bensin, Kerusakan)
				float secondaryFade = std::clamp(1.0f - g_centerBlend, 0.0f, 1.0f);
				int secondaryAlphaInt = (int)((float)currentAlphaInt * secondaryFade);

				if (secondaryAlphaInt > 2)
				{
					if (g_cfg.showVehicleName && !vehName.empty())
					{
						float miniKmhX = miniX + 0.038f + 0.004f;
						float miniKmhW = GetTextWidth("KM/H", miniUnitScale, 4);
						float miniGearX = miniKmhX + miniKmhW + 0.016f;
						float miniGearW = g_cfg.showGear ? (GetTextWidth("N", miniGearScale, 4) + 0.010f) : 0.0f;
						float miniNameX = miniGearX + miniGearW;
						float miniNameY = miniY + 0.013f;

						DrawTextElement(vehName.c_str(), miniNameX, miniNameY, miniUnitScale * 0.90f, 4,
						                g_cfg.textColorR, g_cfg.textColorG, g_cfg.textColorB, secondaryAlphaInt);
					}

					if (g_cfg.showFuel || g_cfg.showDamage)
					{
						char extraBuf[64] = { 0 };
						if (g_cfg.showFuel && g_cfg.showDamage)
							sprintf_s(extraBuf, "FUEL: %d%%  ENG: %d%%", (int)fuelPct, (int)engHealthPct);
						else if (g_cfg.showFuel)
							sprintf_s(extraBuf, "FUEL: %d%%", (int)fuelPct);
						else if (g_cfg.showDamage)
							sprintf_s(extraBuf, "ENG: %d%%", (int)engHealthPct);

						float miniExtraY = miniY - 0.026f;
						float miniExtraX = miniX + ((g_cfg.showFps || g_cfg.showClock) ? 0.085f : 0.0f);

						DrawTextElement(extraBuf, miniExtraX, miniExtraY, miniUnitScale * 0.85f, 4,
						                g_cfg.textColorR, g_cfg.textColorG, g_cfg.textColorB, (int)(secondaryAlphaInt * 0.90f));
					}
					else if (g_cfg.showAltitude && !inVehicle)
					{
						Vector3 pCoords = ENTITY::GET_ENTITY_COORDS(playerPed, TRUE);
						char altBuf[32];
						sprintf_s(altBuf, "ALT: %.0fm", pCoords.z);
						float miniAltY = miniY - 0.026f;
						float miniAltX = miniX + ((g_cfg.showFps || g_cfg.showClock) ? 0.085f : 0.0f);
						DrawTextElement(altBuf, miniAltX, miniAltY, miniUnitScale * 0.85f, 4,
						                g_cfg.textColorR, g_cfg.textColorG, g_cfg.textColorB, (int)(secondaryAlphaInt * 0.90f));
					}
				}
			}
		}
	}
}

BOOL APIENTRY DllMain(HMODULE module, DWORD reason, LPVOID)
{
	switch (reason)
	{
	case DLL_PROCESS_ATTACH:
		scriptRegister(module, ScriptMain);
		keyboardHandlerRegister(OnKeyboard);
		break;
	case DLL_PROCESS_DETACH:
		SaveConfig();
		scriptUnregister(module);
		keyboardHandlerUnregister(OnKeyboard);
		break;
	}
	return TRUE;
}
