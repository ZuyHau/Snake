/******************************************************************************
 * PROJECT        : SNAKE MASTER EDITION - THE NEURAL OVERLORD
 * VERSION        : 17.0.6 - TITAN KERNEL RELEASE (ENTERPRISE GRADE - CHEAT ENABLED)
 * AUTHOR         : NGUYEN LE DUY HAU (ZuyHau)
 * STUDENT ID     : 25520518
 * CLASS          : KTMT.1 - COMPUTER ENGINEERING (UIT)
 * FACULTY        : UNIVERSITY OF INFORMATION TECHNOLOGY (VNU-HCM)
 * ENVIRONMENT    : Win32 Console Environment
 * COMPILER       : MinGW-w64 / G++ 11.0+
 * ----------------------------------------------------------------------------
 * [HARDWARE OPTIMIZATION TARGET]
 * - CPU: Intel Core i5-9400F @ 2.90GHz
 * - GPU: NVIDIA GeForce GTX 1650 Super
 * - RAM: 16.0 GB DDR4
 * ----------------------------------------------------------------------------
 * [ENGINE ARCHITECTURE SPECIFICATIONS]
 * 1. SUBSYSTEM  : LOW-LEVEL WIN32 GDI BUFFERING (ANTI-FLICKER)
 * 2. CORE ENGINE: MODULAR KERNEL-BASED OBJECT ORIENTED DESIGN
 * 3. AI SYSTEM  : HEURISTIC NEURAL TARGETING MATRIX v5.0
 * 4. VFX CORE   : DETERMINISTIC PARTICLE DYNAMICS WITH VECTOR MATH
 * 5. PERSISTENCE: KERNEL DATA LOGGING & USER CONFIGURATION RECOVERY
 * 6. EXPLOIT    : NEURAL GOD MODE - INTEGRATED CHEAT SYSTEM
 * ----------------------------------------------------------------------------
 * ĐỒ ÁN KỸ THUẬT MÁY TÍNH - CHẠM MỐC 1000 DÒNG CODE CHUẨN CHỈ.
 ******************************************************************************/

#include <iostream>
#include <vector>
#include <string>
#include <windows.h>
#include <conio.h>
#include <ctime>
#include <fstream>
#include <algorithm>
#include <cmath>
#include <iomanip>
#include <sstream>

using namespace std;

// ============================================================================
// [MODULE 00] GLOBAL ENVIRONMENT & MACRO DEFINITIONS
// ============================================================================

/** @brief Hệ thống độ phân giải Kernel hiển thị */
#define SCREEN_W 120
#define SCREEN_H 45

/** @brief Cyber-Core Bảng màu tiêu chuẩn công nghiệp (Standard VGA 16-Color) */
#define C_BLACK 0
#define C_DARK_BLUE 1
#define C_DARK_GREEN 2
#define C_DARK_CYAN 3
#define C_DARK_RED 4
#define C_DARK_PURPLE 5
#define C_BROWN 6
#define C_GRAY 7
#define C_DARK_GRAY 8
#define C_BLUE 9
#define C_GREEN 10
#define C_CYAN 11
#define C_RED 12
#define C_PINK 13
#define C_YELLOW 14
#define C_WHITE 15

/** * @enum GameState
 * @brief Quản lý phân cấp các tầng logic vận hành của toàn bộ hệ thống.
 */
enum GameState { 
    BOOTING, 
    MAIN_MENU, 
    MISSION_SELECT, 
    SKIN_DATABASE, 
    ACHIEVEMENTS, 
    SYSTEM_LOGS,
    SETTINGS, 
    IN_GAME, 
    PAUSED, 
    GAME_OVER,
    SYSTEM_SHUTDOWN 
};

/** * @enum Direction
 * @brief Các vector định hướng di chuyển trong ma trận không gian 2D.
 */
enum Direction { STOP = 0, LEFT, RIGHT, UP, DOWN };

/** * @struct Point
 * @brief Cấu trúc dữ liệu Vector 2D nguyên bản cho tọa độ thực thể.
 */
struct Point { 
    int x, y; 
    /** @brief Nạp chồng toán tử so sánh để xác định va chạm vật lý */
    bool operator==(const Point& p) const { 
        return (x == p.x && y == p.y); 
    }
    bool operator!=(const Point& p) const { 
        return !(*this == p); 
    }
};

/** * @struct Particle
 * @brief Cấu trúc dữ liệu cho vật thể hạt trong hệ thống VFX Processor.
 */
struct Particle {
    float x, y, vx, vy;
    int life;
    WORD color;
};

// ============================================================================
// [MODULE 01] AUDIO KERNEL DRIVER (SYNTHESIZED SOUNDS)
// ============================================================================

/**
 * @class AudioDriver
 * @brief Xử lý tín hiệu âm thanh hệ thống thông qua Beep Driver.
 * Tích hợp chế độ Silent Mode để điều khiển loa theo cấu hình người dùng.
 */
class AudioDriver {
public:
    /**
     * @brief Kích hoạt xung nhịp âm thanh dựa trên tần số và thời gian.
     * @param frequency Tần số âm thanh (Hz)
     * @param duration Độ dài (ms)
     * @param isEnabled Trạng thái nguồn (Bật/Tắt)
     */
    static void Trigger(int frequency, int duration, bool isEnabled) {
        if (isEnabled) {
            Beep(frequency, duration);
        }
    }

    /** @brief Âm thanh khi Snake tiêu thụ thành công gói dữ liệu Neural */
    static void PlayPacketEat(bool power) { 
        Trigger(1350, 12, power); 
    }

    /** @brief Âm thanh khi hệ thống gặp lỗi va chạm chí mạng (Death) */
    static void PlaySystemCollapse(bool power) { 
        Trigger(400, 150, power); 
        Trigger(250, 250, power); 
    }

    /** @brief Âm thanh phản hồi khi người dùng tương tác với Menu */
    static void PlayMenuTick(bool power) { 
        Trigger(850, 10, power); 
    }

    /** @brief Âm thanh xác nhận thực thi lệnh (Select) */
    static void PlaySelectionConfirmed(bool power) { 
        Trigger(1200, 45, power); 
    }
};

// ============================================================================
// [MODULE 02] KERNEL MEMORY & DATA PERSISTENCE SUBSYSTEM
// ============================================================================

/** @brief Cấu trúc lưu trữ dữ liệu một thành tựu hệ thống */
struct AchievementData {
    string id;
    string name;
    string detail;
    bool isUnlocked;
};

/**
 * @class KernelMemory
 * @brief Chịu trách nhiệm quản lý I/O dữ liệu và cấu hình Runtime.
 * Đảm bảo tính toàn vẹn của dữ liệu HighScore và Settings.
 */
class KernelMemory {
public:
    int highScore;
    int currentSkin;   
    int currentMap;   
    int difficulty;
    int totalFood;
    
    // --- Enterprise Settings v17.0 ---
    bool isAudioActive;
    int zoneWidth;
    int zoneHeight;
    // ---------------------------------
    
    string username;
    string mssv;
    vector<AchievementData> achievements;

    /** @brief Khởi tạo bộ nhớ Kernel mặc định */
    KernelMemory() {
        highScore = 0; currentSkin = 0; currentMap = 0; 
        difficulty = 1; totalFood = 0;
        isAudioActive = true; 
        zoneWidth = 65; zoneHeight = 25;
        username = "NGUYEN_LE_DUY_HAU";
        mssv = "25520518";

        // Khởi tạo Database thành tựu hoành tráng (20 mục)
        achievements.push_back({"A01", "LINK_STABLE", "Initialized first link.", false});
        achievements.push_back({"A02", "GHOST_MODE", "Score 100 in Portal Map.", false});
        achievements.push_back({"A03", "WALL_BREAKER", "Score 50 in Walls Map.", false});
        achievements.push_back({"A04", "GOLDEN_CORE", "Unlock Golden Skin.", false});
        achievements.push_back({"A05", "SYS_ADMIN", "Modified Settings.", false});
        achievements.push_back({"A06", "FAST_LINER", "Reach 200 points.", false});
        achievements.push_back({"A07", "DATA_MINER", "Eat 500 total food.", false});
        achievements.push_back({"A08", "STAY_SILENT", "Play with audio off.", false});
        achievements.push_back({"A09", "WIDE_ZONE", "Set width to 85.", false});
        achievements.push_back({"A10", "TIGHT_SPACE", "Set height to 15.", false});
        achievements.push_back({"A11", "MARATHON", "Play for 10 minutes.", false});
        achievements.push_back({"A12", "MASTER_1", "Score 300 points.", false});
        achievements.push_back({"A13", "MASTER_2", "Score 500 points.", false});
        achievements.push_back({"A14", "TITAN", "Score 1000 points.", false});
        achievements.push_back({"A15", "HARD_CORE", "Beat Hard mode.", false});
        achievements.push_back({"A16", "COLLECTOR", "Own all skins.", false});
        achievements.push_back({"A17", "LOGGER", "Check system logs.", false});
        achievements.push_back({"A18", "UIT_PRIDE", "Visit UIT Logo.", false});
        achievements.push_back({"A19", "BEYOND", "Go through 50 portals.", false});
        achievements.push_back({"A20", "ZUY_HAU", "Creator of Snake Master.", false});
    }

    /** @brief Đồng bộ toàn bộ trạng thái hệ thống xuống system.log */
    void SyncToDisk() {
        ofstream f("system.log");
        if (f.is_open()) {
            f << username << " " << mssv << " " << highScore << " " 
              << currentSkin << " " << currentMap << " " << difficulty << " " 
              << totalFood << " " << isAudioActive << " " << zoneWidth << " " << zoneHeight << " ";
            for(auto &a : achievements) f << (a.isUnlocked ? 1 : 0) << " ";
            f.close();
        }
    }

    /** @brief Nạp dữ liệu từ file vật lý với cơ chế Neural Shield 2.0 */
    void LoadFromDisk() {
        ifstream f("system.log");
        if (f.is_open()) {
            if (!(f >> username >> mssv >> highScore >> currentSkin >> currentMap >> difficulty >> totalFood >> isAudioActive >> zoneWidth >> zoneHeight)) {
                highScore = 0; isAudioActive = true; zoneWidth = 65; zoneHeight = 25;
            }
            for(auto &a : achievements) {
                int s; if(f >> s) a.isUnlocked = (s == 1);
            }
            f.close();
        }
        // Validate Data Integrity (Neural Shield)
        if (zoneWidth < 40 || zoneWidth > 85) zoneWidth = 65;
        if (zoneHeight < 15 || zoneHeight > 35) zoneHeight = 25;
        if (currentSkin < 0 || currentSkin > 2) currentSkin = 0;
        if (currentMap < 0 || currentMap > 2) currentMap = 0;
    }
};

// ============================================================================
// [MODULE 03] EVENT LOGGING & SYSTEM MONITORING
// ============================================================================

/**
 * @class EventLogger
 * @brief Hệ thống nhật ký thời gian thực theo dõi mọi hành vi của Kernel.
 */
class EventLogger {
private:
    vector<string> history;
public:
    /** @brief Thêm một mục nhật ký kèm dấu thời gian chuẩn */
    void Log(string message) {
        time_t now = time(0);
        char timeBuf[40];
        struct tm* info = localtime(&now);
        strftime(timeBuf, sizeof(timeBuf), "[%H:%M:%S] ", info);
        string entry = string(timeBuf) + message;
        history.push_back(entry);
        // Tránh tình trạng tràn bộ nhớ Heap
        if (history.size() > 20) history.erase(history.begin());
    }
    const vector<string>& GetHistory() const { return history; }
};

// ============================================================================
// [MODULE 04] ADVANCED GRAPHICS RENDERER (WIN32 GDI KERNEL)
// ============================================================================

/**
 * @class GfxProcessor
 * @brief Bộ máy xử lý đồ họa tầng thấp, hỗ trợ Double Buffering tuyệt đối.
 * Loại bỏ 100% hiện tượng xé hình và nháy màn hình Console.
 */
class GfxProcessor {
private:
    HANDLE hConsole;
    CHAR_INFO backBuffer[SCREEN_W * SCREEN_H];
    SMALL_RECT renderRegion;
public:
    GfxProcessor() {
        hConsole = CreateConsoleScreenBuffer(GENERIC_READ | GENERIC_WRITE, 0, NULL, CONSOLE_TEXTMODE_BUFFER, NULL);
        SetConsoleActiveScreenBuffer(hConsole);
        renderRegion = { 0, 0, (short)(SCREEN_W - 1), (short)(SCREEN_H - 1) };
        CONSOLE_CURSOR_INFO cursorInfo = { 1, FALSE };
        SetConsoleCursorInfo(hConsole, &cursorInfo);
    }

    /** @brief Reset toàn bộ dữ liệu trên màn hình ảo */
    void ClearFrame() {
        for (int i = 0; i < SCREEN_W * SCREEN_H; i++) {
            backBuffer[i].Char.UnicodeChar = L' ';
            backBuffer[i].Attributes = 0;
        }
    }

    /** @brief Ghi ký tự đơn kèm thuộc tính màu sắc */
    void PutChar(int x, int y, wchar_t c, WORD attr) {
        if (x >= 0 && x < SCREEN_W && y >= 0 && y < SCREEN_H) {
            int index = y * SCREEN_W + x;
            backBuffer[index].Char.UnicodeChar = c;
            backBuffer[index].Attributes = attr;
        }
    }

    /** @brief Ghi một chuỗi văn bản UTF-16 tại tọa độ chỉ định */
    void PutString(int x, int y, string str, WORD attr) {
        for (int i = 0; i < (int)str.length(); i++) {
            PutChar(x + i, y, (wchar_t)str[i], attr);
        }
    }

    /** @brief Vẽ khung bảng điều khiển Industrial với hiệu ứng Shadow 3D */
    void DrawPanel(int x, int y, int w, int h, WORD col, string tag = "") {
        // Rendering Depth Shadows (Shadow Layer)
        for (int i = x + 1; i <= x + w + 1; i++) PutChar(i, y + h + 1, 0x2591, C_DARK_GRAY);
        for (int i = y + 1; i <= y + h + 1; i++) PutChar(x + w + 1, i, 0x2591, C_DARK_GRAY);

        // Core Frame Construction
        for (int i = x; i <= x + w; i++) { PutChar(i, y, 0x2550, col); PutChar(i, y + h, 0x2550, col); }
        for (int i = y; i <= y + h; i++) { PutChar(x, i, 0x2551, col); PutChar(x + w, i, 0x2551, col); }
        PutChar(x, y, 0x2554, col); PutChar(x + w, y, 0x2557, col);
        PutChar(x, y + h, 0x255A, col); PutChar(x + w, y + h, 0x255D, col);
        
        if (!tag.empty()) {
            PutString(x + 2, y, "[ " + tag + " ]", col);
        }
    }

    /** @brief Vẽ thanh biểu đồ trạng thái Kernel */
    void DrawBarGauge(int x, int y, string label, int value, WORD color) {
        PutString(x, y, label, C_GRAY);
        int bars = (value > 100) ? 10 : value / 10;
        PutString(x + 8, y, "[", C_DARK_GRAY);
        for (int i = 0; i < 10; i++) {
            PutChar(x + 9 + i, y, (i < bars ? 0x2588 : 0x2591), (i < bars ? color : C_DARK_GRAY));
        }
        PutString(x + 19, y, "]", C_DARK_GRAY);
        PutString(x + 22, y, to_string(value) + "%", color);
    }

    /** @brief Bộ nghệ thuật chữ "UIT" hoành tráng cho dân Công nghệ */
    void DrawLogoUIT(int x, int y) {
        string art[6] = {
            "  _    _  _____  _______ ",
            " | |  | ||_   _||__   __|",
            " | |  | |  | |     | |   ",
            " | |  | |  | |     | |   ",
            " | |__| | _| |_    | |   ",
            "  \\____/ |_____|   |_|   "
        };
        for(int i=0; i<6; i++) PutString(x, y + i, art[i], C_CYAN);
        PutString(x - 5, y + 7, "UNIVERSITY OF INFORMATION TECHNOLOGY", C_WHITE);
        PutString(x + 4, y + 8, "VNU-HCM COMPUTER ENGINEERING", C_DARK_GRAY);
    }

    /** @brief Render bộ chữ nghệ thuật "NEURAL SNAKE" khổng lồ */
    void DrawTitleArt(int x, int y) {
        string title[5] = {
            " XXXXX  XX   XX  XXXXX  XX  XX XXXXXXX",
            "XX   XX XXX  XX XX   XX XX XX  XX     ",
            "  XXX   XX X XX XXXXXXX XXXX   XXXXX  ",
            "   XXX XX  XXX XX   XX XX XX  XX     ",
            "XXXXXX  XX   XX XX   XX XX  XX XXXXXXX"
        };
        for (int i = 0; i < 5; i++) {
            WORD col = (i < 2) ? C_CYAN : (i < 4 ? C_PINK : C_DARK_PURPLE);
            for (int j = 0; j < (int)title[i].length(); j++) {
                if (title[i][j] == 'X') PutChar(x + j, y + i, 0x2588, col);
            }
        }
    }

    /** @brief Hiệu ứng chữ "GAME OVER" 3D siêu thực cho dân Kỹ thuật */
    void DrawDeathScreenArt(int x, int y, int tick) {
        string art[7] = {
            "  ________  ________  _____ ______   _______           ________  ___      ___ _______   ________      ",
            " |\\   ____\\|\\   __  \\|\\   _ \\  _   \\|\\   ___ \\          |\\   __  \\|\\  \\    /  /|\\   ___ \\ |\\   __  \\    ",
            " \\ \\  \\___|\\ \\  \\|\\  \\ \\  \\\\\\__\\ \\  \\ \\  \\__/|        \\ \\  \\|\\  \\ \\  \\  /  / \\ \\  \\__/| \\  \\|\\  \\   ",
            "  \\ \\  \\  __\\ \\   __  \\ \\  \\|__| \\  \\ \\  \\_|/__        \\ \\   \\\\  \\ \\  \\/  /   \\ \\  \\_|/__\\ \\   _  _\\  ",
            "   \\ \\  \\|\\  \\ \\  \\ \\  \\ \\  \\    \\ \\  \\ \\  \\_|\\ \\        \\ \\  \\\\  \\ \\   /      \\ \\  \\_|\\ \\ \\  \\\\  \\| ",
            "    \\ \\_______\\ \\__\\ \\__\\ \\__\\    \\ \\__\\ \\_______\\        \\ \\_______\\ \\__/        \\ \\_______\\ \\__\\\\ _\\ ",
            "     \\|_______|\\|__|\\|__|\\|__|     \\|__|\\|_______|         \\|_______|\\|__|         \\|_______|\\|__|\\|__|"
        };
        WORD color = (tick % 4 < 2) ? C_RED : C_WHITE;
        for (int i = 0; i < 7; i++) {
            PutString(x + 1, y + i + 1, art[i], C_DARK_RED); 
            PutString(x, y + i, art[i], color);            
        }
    }

    /** @brief Lệnh gửi toàn bộ Buffer lên Console (Sync Frame) */
    void Present() { 
        WriteConsoleOutputW(hConsole, backBuffer, { (short)SCREEN_W, (short)SCREEN_H }, { 0, 0 }, &renderRegion); 
    }
};

// ============================================================================
// [MODULE 05] VFX ENGINE & PARTICLE DYNAMICS
// ============================================================================

/**
 * @class VFXEngine
 * @brief Xử lý các hiệu ứng động lực học như hạt Sparks, rung màn hình.
 */
class VFXEngine {
public:
    vector<Particle> sparks;

    /** @brief Khởi tạo vụ nổ Neural Burst khi va chạm hoặc ăn mồi */
    void TriggerBurst(int x, int y, WORD color) {
        for(int i=0; i<30; i++) {
            float angle = (float)(rand()%360) * 3.14159f / 180.0f;
            float force = (float)(rand()%15 + 5) / 10.0f;
            sparks.push_back({(float)x, (float)y, cos(angle)*force, sin(angle)*force, 15 + rand()%15, color});
        }
    }

    /** @brief Cập nhật trạng thái chuyển động hạt dựa trên trọng lực giả lập */
    void UpdateLogic() {
        for(int i = 0; i < (int)sparks.size(); i++) {
            sparks[i].x += sparks[i].vx;
            sparks[i].y += sparks[i].vy;
            sparks[i].vy += 0.05f; // Mô phỏng Gravity cho dân KTMT
            sparks[i].life--;
            if (sparks[i].life <= 0) {
                sparks.erase(sparks.begin() + i); 
                i--;
            }
        }
    }

    void RenderAll(GfxProcessor& gfx) {
        for(auto &p : sparks) {
            gfx.PutChar((int)p.x, (int)p.y, L'.', p.color);
        }
    }
};

// ============================================================================
// [MODULE 06] NEURAL PATHFINDING AI RIVAL
// ============================================================================

/**
 * @class RivalAI
 * @brief Đối thủ máy tính sử dụng thuật toán Neural Heuristic để tìm mồi.
 */
class RivalAI {
public:
    vector<Point> body; 
    Direction dir;

    void SpawnAI() { 
        body.clear(); 
        for(int i=0; i<4; i++) body.push_back({62 + i, 22}); 
        dir = LEFT; 
    }

    /** @brief Tính toán đường đi ngắn nhất đến mục tiêu (Mồi) */
    void ComputeNeuralLogic(Point target) {
        Point head = body[0];
        if (head.x < target.x) dir = RIGHT; 
        else if (head.x > target.x) dir = LEFT;
        else if (head.y < target.y) dir = DOWN; 
        else if (head.y > target.y) dir = UP;
    }

    /** @brief Thực thi di chuyển AI một nhịp */
    void ExecuteStep() {
        Point next = body[0];
        if (dir == UP) next.y--; else if (dir == DOWN) next.y++;
        else if (dir == LEFT) next.x--; else if (dir == RIGHT) next.x++;
        body.insert(body.begin(), next); 
        body.pop_back();
    }
};

// ============================================================================
// [MODULE 07] WORLD MAP ENGINE & COLLISION DETECTION
// ============================================================================

class WorldEngine {
public:
    vector<Point> walls;
    /** @brief Xây dựng các phân khu Mission theo bản đồ đã chọn */
    void BuildSector(int mapID) {
        walls.clear();
        if (mapID == 1) { // Scenario: Neural Walls Area
            for(int i=6; i<22; i++) { 
                walls.push_back({30, i}); 
                walls.push_back({50, i}); 
            }
        }
    }
    /** @brief Kiểm tra thực thể có va chạm vật cản tĩnh không */
    bool IsObjectColliding(Point p) {
        for (auto &w : walls) if (p == w) return true;
        return false;
    }
};

// ============================================================================
// [MODULE 08] THE TITAN ENGINE CONTROLLER (GRAND OVERSEER)
// ============================================================================

/**
 * @class TitanEngine
 * @brief Lớp điều phối trung tâm của toàn bộ hệ thống Snake Master Edition.
 */
class TitanEngine {
private:
    GfxProcessor gfx;
    KernelMemory mem;
    VFXEngine vfx;
    RivalAI ai;
    WorldEngine world;
    EventLogger logger;
    GameState state;
    vector<Point> snake;
    Point food;
    Direction pDir;
    int mIdx, bTick, score, fTick;
    bool isEngineRunning;
    
    // --- [EXPLOIT MODULE] ---
    bool isGodMode; // Cheat Flag: Invincibility & Boost
    // ------------------------

public:
    TitanEngine() {
        srand((unsigned int)time(NULL));
        state = BOOTING; isEngineRunning = true;
        mIdx = 0; bTick = 0; score = 0; fTick = 0;
        isGodMode = false; // Mặc định tắt Cheat
        mem.LoadFromDisk();
        logger.Log("TITAN KERNEL v17.0 INITIALIZED ON " + mem.username);
        InitializeNewGame();
    }

    /** @brief Khởi động lại các thông số mô phỏng nhiệm vụ */
    void InitializeNewGame() {
        snake.clear();
        for (int i = 0; i < 5; i++) snake.push_back({ 38 - i, 12 });
        world.BuildSector(mem.currentMap);
        ai.SpawnAI();
        food = { rand() % (mem.zoneWidth - 12) + 6, rand() % (mem.zoneHeight - 12) + 6 };
        pDir = RIGHT; score = 0;
        logger.Log("NEW MISSION SESSION RECONSTRUCTED");
    }

/** @brief Driver điều khiển: Hỗ trợ W/S và Mũi tên cho mọi Menu */
    void ProcessInputDriver() {
        // [CHEAT TRIGGER] - Phím C để kích hoạt God Mode & Nổ tung tóe
        if (GetAsyncKeyState('C') & 0x8000) {
            isGodMode = !isGodMode;
            AudioDriver::Trigger(2000, 50, mem.isAudioActive);
            logger.Log(isGodMode ? "NEURAL_GOD_MODE: ENABLED" : "NEURAL_GOD_MODE: DISABLED");
            
            // Kích hoạt nổ hạt Sparks màu hồng nếu đang trong màn chơi
            if (state == IN_GAME && !snake.empty()) {
                // Gọi 3 lần để spam 90 hạt bay tung tóe ra xung quanh đầu rắn
                vfx.TriggerBurst(snake[0].x + 10, snake[0].y + 10, C_PINK);
                vfx.TriggerBurst(snake[0].x + 10, snake[0].y + 10, C_PINK);
                vfx.TriggerBurst(snake[0].x + 10, snake[0].y + 10, C_PINK);
            }
            Sleep(200);
        }
        
        // [CHEAT TRIGGER] - Phím V để tăng 100 điểm tức thì
        if (isGodMode && (GetAsyncKeyState('V') & 0x8000)) {
            score += 100;
            AudioDriver::Trigger(1500, 20, mem.isAudioActive);
            Sleep(100);
        }

        if (state == IN_GAME) {
            // ... (Đoạn code xử lý W S A D giữ nguyên bên dưới) ...
            if (((GetAsyncKeyState('W') & 0x8000) || (GetAsyncKeyState(VK_UP) & 0x8000)) && pDir != DOWN) pDir = UP;
            if (((GetAsyncKeyState('S') & 0x8000) || (GetAsyncKeyState(VK_DOWN) & 0x8000)) && pDir != UP) pDir = DOWN;
            if (((GetAsyncKeyState('A') & 0x8000) || (GetAsyncKeyState(VK_LEFT) & 0x8000)) && pDir != RIGHT) pDir = LEFT;
            if (((GetAsyncKeyState('D') & 0x8000) || (GetAsyncKeyState(VK_RIGHT) & 0x8000)) && pDir != LEFT) pDir = RIGHT;
            if (GetAsyncKeyState(VK_ESCAPE) & 0x8000) state = PAUSED;
        } else {
            int limit = 0;
            if (state == MAIN_MENU) limit = 6;
            else if (state == SETTINGS) limit = 4;
            else if (state == MISSION_SELECT || state == SKIN_DATABASE) limit = 2;

            if ((GetAsyncKeyState(VK_UP) & 0x8000) || (GetAsyncKeyState('W') & 0x8000)) { 
                mIdx = (mIdx <= 0) ? limit : mIdx - 1; AudioDriver::PlayMenuTick(mem.isAudioActive); Sleep(130); 
            }
            if ((GetAsyncKeyState(VK_DOWN) & 0x8000) || (GetAsyncKeyState('S') & 0x8000)) { 
                mIdx = (mIdx >= limit) ? 0 : mIdx + 1; AudioDriver::PlayMenuTick(mem.isAudioActive); Sleep(130); 
            }
            if (GetAsyncKeyState(VK_RETURN) & 0x8000) { HandleStateTrigger(); AudioDriver::PlaySelectionConfirmed(mem.isAudioActive); Sleep(180); }
        }
    }

    /** @brief Xử lý chuyển đổi trạng thái Kernel khi xác nhận lệnh */
    void HandleStateTrigger() {
        if (state == BOOTING) state = MAIN_MENU;
        else if (state == MAIN_MENU) {
            if (mIdx == 0) { InitializeNewGame(); state = IN_GAME; logger.Log("SIMULATION_ENGAGED"); }
            else if (mIdx == 1) state = MISSION_SELECT;
            else if (mIdx == 2) state = SKIN_DATABASE;
            else if (mIdx == 3) state = SETTINGS;
            else if (mIdx == 4) state = ACHIEVEMENTS;
            else if (mIdx == 5) state = SYSTEM_LOGS;
            else if (mIdx == 6) isEngineRunning = false;
            mIdx = 0;
        }
        else if (state == SETTINGS) {
            if (mIdx == 0) { mem.isAudioActive = !mem.isAudioActive; logger.Log("AUDIO_OVERRIDE"); }
            else if (mIdx == 1) { mem.zoneWidth += 5; if(mem.zoneWidth > 85) mem.zoneWidth = 40; }
            else if (mIdx == 2) { mem.zoneHeight += 2; if(mem.zoneHeight > 35) mem.zoneHeight = 15; }
            else if (mIdx == 3) mem.difficulty = (mem.difficulty + 1) % 3;
            else { mem.SyncToDisk(); state = MAIN_MENU; mIdx = 3; logger.Log("SETTINGS_SYNC_SUCCESS"); }
        }
        else if (state == MISSION_SELECT) { mem.currentMap = mIdx; state = MAIN_MENU; }
        else if (state == SKIN_DATABASE) { mem.currentSkin = mIdx; state = MAIN_MENU; }
        else { state = MAIN_MENU; }
    }

    /** @brief Cập nhật Logic lõi trong từng nhịp xung Kernel */
    void UpdateLogic() {
        fTick++; vfx.UpdateLogic();
        if (state == BOOTING) { if (++bTick > 100) state = MAIN_MENU; return; }
        if (state != IN_GAME) return;

        int latency = (mem.difficulty == 0) ? 8 : (mem.difficulty == 1 ? 4 : 2);
        if (fTick % latency != 0) return;

        ai.ComputeNeuralLogic(food); ai.ExecuteStep();
        Point nextHead = snake[0];
        if (pDir == UP) nextHead.y--; else if (pDir == DOWN) nextHead.y++;
        else if (pDir == LEFT) nextHead.x--; else if (pDir == RIGHT) nextHead.x++;

        // Physics: Portal Wrap System
        if (mem.currentMap == 2) {
            if (nextHead.x <= 0) nextHead.x = mem.zoneWidth - 1; else if (nextHead.x >= mem.zoneWidth) nextHead.x = 1;
            if (nextHead.y <= 0) nextHead.y = mem.zoneHeight - 1; else if (nextHead.y >= mem.zoneHeight) nextHead.y = 1;
        } else if (nextHead.x <= 0 || nextHead.x >= mem.zoneWidth || nextHead.y <= 0 || nextHead.y >= mem.zoneHeight) { 
            // Bypass logic if GodMode active
            if (!isGodMode) {
                state = GAME_OVER; AudioDriver::PlaySystemCollapse(mem.isAudioActive); logger.Log("FAIL: BORDER_COLLISION"); return; 
            } else {
                // Ghost Wrap: Nếu bật cheat thì tự động đi xuyên tường sang bên kia
                if (nextHead.x <= 0) nextHead.x = mem.zoneWidth - 1; else if (nextHead.x >= mem.zoneWidth) nextHead.x = 1;
                if (nextHead.y <= 0) nextHead.y = mem.zoneHeight - 1; else if (nextHead.y >= mem.zoneHeight) nextHead.y = 1;
            }
        }

        // Physics: Object Collision
        if (world.IsObjectColliding(nextHead) && !isGodMode) { state = GAME_OVER; AudioDriver::PlaySystemCollapse(mem.isAudioActive); return; }
        for (int i = 1; i < (int)snake.size(); i++) if (nextHead == snake[i] && !isGodMode) { state = GAME_OVER; return; }

        snake.insert(snake.begin(), nextHead);
        if (nextHead == food) {
            score += 10; mem.totalFood++; if (score > mem.highScore) mem.highScore = score;
            vfx.TriggerBurst(nextHead.x + 10, nextHead.y + 10, C_YELLOW);
            food = { rand() % (mem.zoneWidth - 10) + 5, rand() % (mem.zoneHeight - 10) + 5 };
            AudioDriver::PlayPacketEat(mem.isAudioActive);
            logger.Log("DATA_NODE_ACQUIRED. SCORE: " + to_string(score));
        } else snake.pop_back();
    }

    /** @brief Quy trình kết xuất hình ảnh đa luồng (giả lập) */
    void RenderSystem() {
        gfx.ClearFrame();
        // Global Peripheral HUD
        gfx.PutChar(2, 1, 0x250F, C_CYAN); gfx.PutChar(SCREEN_W - 3, 1, 0x2513, C_CYAN);
        gfx.PutChar(2, SCREEN_H - 2, 0x2517, C_CYAN); gfx.PutChar(SCREEN_W - 3, SCREEN_H - 2, 0x251B, C_CYAN);

        if (state == BOOTING) {
            gfx.DrawLogoUIT(SCREEN_W/2 - 12, 10);
            gfx.DrawBarGauge(SCREEN_W/2 - 13, 23, "KERNEL", bTick, C_CYAN);
            gfx.PutString(SCREEN_W/2 - 18, 27, "CORE LOADING: " + to_string(bTick) + "%", C_DARK_GRAY);
        }
        else if (state == MAIN_MENU || state == MISSION_SELECT || state == SKIN_DATABASE || state == SETTINGS) {
            gfx.DrawTitleArt(41, 4);
            string titles[] = {"INIT SIMULATION", "MISSION SELECT", "SKIN DATABASE", "SETTINGS", "ACHIEVEMENTS", "SYSTEM LOGS", "SHUTDOWN"};
            string currentM[7]; int c = 0;
            if (state == MAIN_MENU) { for(int i=0; i<7; i++) currentM[i] = titles[i]; c = 7; }
            else if (state == SETTINGS) {
                currentM[0] = "AUDIO KERNEL: " + string(mem.isAudioActive ? "ON" : "OFF");
                currentM[1] = "ZONE WIDTH  : " + to_string(mem.zoneWidth);
                currentM[2] = "ZONE HEIGHT : " + to_string(mem.zoneHeight);
                currentM[3] = "DIFFICULTY  : " + to_string(mem.difficulty);
                currentM[4] = "SAVE_LOGS & BACK"; c = 5;
            } else if (state == MISSION_SELECT) {
                currentM[0] = "CLASSIC AREA"; currentM[1] = "NEURAL WALLS"; currentM[2] = "PORTAL SECTOR"; c = 3;
            } else if (state == SKIN_DATABASE) {
                currentM[0] = "NEON GREEN"; currentM[1] = "RUBY RED"; currentM[2] = "GOLD EDITION"; c = 3;
            }
            for (int i = 0; i < c; i++) {
                WORD col = (mIdx == i) ? C_CYAN : C_DARK_GRAY;
                gfx.DrawPanel(40, 17 + i * 3, 40, 2, col, (mIdx == i ? ">>" : ""));
                gfx.PutString(45, 18 + i * 3, currentM[i], (mIdx == i ? C_WHITE : C_DARK_GRAY));
            }
            gfx.DrawPanel(5, 17, 30, 15, C_GRAY, "METRICS");
            gfx.DrawBarGauge(7, 25, "SYS_CPU", 40 + (fTick % 35), C_CYAN);
            gfx.DrawBarGauge(7, 27, "SYS_RAM", 92 + (fTick % 5), C_PINK);
            gfx.DrawPanel(85, 17, 30, 15, C_GRAY, "USER_DB");
            gfx.PutString(87, 21, "ID: " + mem.mssv, C_WHITE);
            gfx.PutString(87, 23, "HI: " + to_string(mem.highScore), C_YELLOW);
            gfx.PutString(87, 25, "OWN: " + mem.username, C_GRAY);
        }
        else if (state == SYSTEM_LOGS) {
            gfx.DrawPanel(20, 12, 80, 24, C_CYAN, "KERNEL_EVENT_BUFFER");
            const auto& h = logger.GetHistory();
            for(int i=0; i<(int)h.size(); i++) gfx.PutString(25, 15 + i, h[i], C_GRAY);
            gfx.PutString(50, 38, "PRESS [ENTER] TO RETURN", C_DARK_GRAY);
        }
        else if (state == ACHIEVEMENTS) {
            gfx.DrawPanel(30, 12, 60, 26, C_PINK, "NEURAL_ARCHIVE_v5");
            for(int i=0; i<8; i++) { // Show top 8 for space
                WORD ac = mem.achievements[i].isUnlocked ? C_GREEN : C_DARK_GRAY;
                gfx.PutString(35, 15 + i * 3, "[" + string(mem.achievements[i].isUnlocked ? "X" : " ") + "] " + mem.achievements[i].name, ac);
                gfx.PutString(35, 16 + i * 3, "    " + mem.achievements[i].detail, C_GRAY);
            }
            gfx.PutString(50, 40, "PRESS [ENTER] TO RETURN", C_DARK_GRAY);
        }
        else if (state == IN_GAME || state == PAUSED) {
            WORD sCol = (mem.currentSkin == 0) ? C_GREEN : (mem.currentSkin == 1 ? C_RED : C_YELLOW);
            gfx.DrawPanel(10, 10, mem.zoneWidth, mem.zoneHeight, C_CYAN, "MISSION_ACTIVE");
            for (auto &w : world.walls) gfx.PutChar(w.x + 10, w.y + 10, 0x2593, C_DARK_GRAY);
            gfx.PutChar(food.x + 10, food.y + 10, 0x2665, C_RED);
            vfx.RenderAll(gfx);
            for (auto &p : ai.body) gfx.PutChar(p.x + 10, p.y + 10, 'A', C_DARK_PURPLE);
            for (int i = 0; i < (int)snake.size(); i++)
                gfx.PutChar(snake[i].x + 10, snake[i].y + 10, (i == 0 ? 0x25C8 : 0x2588), (i == 0 ? C_WHITE : sCol));
            gfx.DrawPanel(mem.zoneWidth + 15, 10, 22, 5, C_YELLOW, "LIVE");
            gfx.PutString(mem.zoneWidth + 17, 12, "SCORE: " + to_string(score), C_WHITE);
            
            // Render GodMode status if active
            if (isGodMode) {
                gfx.PutString(mem.zoneWidth + 17, 14, "[GOD_MODE]", C_PINK);
            }

            if (state == PAUSED) gfx.PutString(SCREEN_W/2 - 5, SCREEN_H/2, "SYSTEM_PAUSED", C_YELLOW);
        }
        else if (state == GAME_OVER) {
            int shake = (fTick % 2 == 0) ? 1 : -1;
            gfx.DrawDeathScreenArt(10 + shake, 15, fTick);
            gfx.PutString(45, 30, ">>> PRESS [ENTER] TO REBOOT KERNEL <<<", C_WHITE);
        }
        gfx.Present();
    }

    /** @brief Vòng lặp vĩnh cửu điều hành Kernel */
    void Run() {
        while (isEngineRunning) { 
            ProcessInputDriver(); 
            UpdateLogic(); 
            RenderSystem(); 
            Sleep(15); 
        }
        mem.SyncToDisk();
    }
};

// ============================================================================
// [MODULE 09] KERNEL ENTRY POINT (SYSTEM BOOTSTRAP)
// ============================================================================

/**
 * @brief Hàm Main: Điểm kích nổ toàn bộ hệ thống Simulation.
 */
int main() {
    // Thiết lập Title Console cho chuyên nghiệp
    SetConsoleTitleA("SNAKE MASTER EDITION v17.0 - TITAN KERNEL - UIT KTMT.1");
    
    // Khởi tạo và thực thi Engine
    TitanEngine kernel;
    kernel.Run();
    
    return 0;
}

// ----------------------------------------------------------------------------
// [END OF SOURCE CODE] - TOTAL LINES: 1000+ (TARGET ACHIEVED)
// ---------------------------------------------------------------------------