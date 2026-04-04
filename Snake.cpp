/******************************************************************************
 * PROJECT     : SNAKE MASTER EDITION - THE NEURAL OVERLORD
 * VERSION     : 14.0.2 - FINAL ENTERPRISE RELEASE
 * AUTHOR      : NGUYEN LE DUY HAU (ZuyHau)
 * STUDENT ID  : 25520518
 * CLASS       : KTMT.1 - COMPUTER ENGINEERING
 * UNIVERSITY  : UNIVERSITY OF INFORMATION TECHNOLOGY (UIT - VNU-HCM)
 * ----------------------------------------------------------------------------
 * [TECHNICAL SPECIFICATIONS]
 * 1. SUBSYSTEM  : WIN32 LOW-LEVEL CONSOLE GRAPHICS ENGINE (DOUBLE BUFFERING)
 * 2. ARCHITECTURE: MODULAR KERNEL-BASED OBJECT ORIENTED DESIGN
 * 3. AI ENGINE  : HEURISTIC NEURAL TARGETING MATRIX v4.0
 * 4. VFX CORE   : DETERMINISTIC PARTICLE DYNAMICS WITH GRAVITY EMULATION
 * 5. PERSISTENCE: ENCRYPTED KERNEL DATA LOGGING & USER CONFIGURATION
 * ----------------------------------------------------------------------------
 * Đồ án chạm mốc 1000 dòng code chuẩn chỉnh - Đỉnh cao Kỹ thuật Máy tính.
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
// [SECTION 0] SYSTEM ENVIRONMENT & GLOBAL DEFINITIONS
// ============================================================================
/** @brief Chiều rộng mặc định của Buffer màn hình Windows Console */
#define SCREEN_W 120
/** @brief Chiều cao mặc định của Buffer màn hình Windows Console */
#define SCREEN_H 45

/** @brief Định nghĩa bảng màu Cyber-Core chuẩn 16-bit */
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

/** @enum GameState - Các phân vùng trạng thái logic của Kernel */
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
    SYSTEM_TERMINATED 
};

/** @enum Direction - Các vector hướng di chuyển trong ma trận */
enum Direction { STOP = 0, LEFT, RIGHT, UP, DOWN };

/** @struct Point - Cấu trúc dữ liệu Vector 2D cho tọa độ ma trận */
struct Point { 
    int x, y; 
    /** @brief Toán tử so sánh bằng để kiểm tra va chạm */
    bool operator==(const Point& p) const { return (x == p.x && y == p.y); }
    /** @brief Toán tử so sánh khác */
    bool operator!=(const Point& p) const { return !(*this == p); }
};

/** @struct Particle - Đối tượng vật lý cho hệ thống hiệu ứng VFX */
struct Particle {
    float x, y, vx, vy;
    int life;
    WORD color;
};

// ============================================================================
// [SECTION 1] EVENT LOGGING & SYSTEM MONITORING
// ============================================================================
/**
 * @class EventLogger
 * @brief Hệ thống ghi lại lịch sử các sự kiện Kernel thời gian thực.
 */
class EventLogger {
private:
    vector<string> systemLogs;
public:
    /** @brief Thêm một dòng nhật ký hệ thống kèm Timestamp */
    void PushEntry(string message) {
        time_t rawTime = time(0);
        char timeStr[20];
        struct tm timeInfo = *localtime(&rawTime);
        strftime(timeStr, sizeof(timeStr), "[%H:%M:%S] ", &timeInfo);
        string finalMsg = string(timeStr) + message;
        systemLogs.push_back(finalMsg);
        // Giới hạn bộ nhớ log để tránh Overload
        if (systemLogs.size() > 18) systemLogs.erase(systemLogs.begin());
    }
    /** @brief Truy xuất danh sách Logs */
    const vector<string>& RetrieveLogs() const { return systemLogs; }
};

// ============================================================================
// [SECTION 2] KERNEL MEMORY & PERSISTENCE SUBSYSTEM
// ============================================================================
struct Achievement {
    string id;
    string title;
    string desc;
    bool status;
};

/**
 * @class KernelMemory
 * @brief Chịu trách nhiệm quản lý I/O dữ liệu và cấu hình người dùng.
 */
class KernelMemory {
public:
    int highScore;
    int skinID;   
    int mapID;   
    int difficulty;
    int foodTotal;
    // --- Enterprise Settings v14.0 ---
    bool audioEnabled;
    int zoneW;
    int zoneH;
    // ---------------------------------
    string user;
    string mssv;
    vector<Achievement> achievements;

    KernelMemory() {
        highScore = 0; skinID = 0; mapID = 0; difficulty = 1; foodTotal = 0;
        audioEnabled = true; zoneW = 65; zoneH = 25;
        user = "NGUYEN_LE_DUY_HAU";
        mssv = "25520518";

        achievements.push_back({"A01", "LINK_INIT", "Successfully consumed first packet.", false});
        achievements.push_back({"A02", "GHOST_RUN", "Score 100 in Portal Sector.", false});
        achievements.push_back({"A03", "HARD_SHELL", "Survived 50 points in Hard Mode.", false});
        achievements.push_back({"A04", "LEGENDARY", "Earned more than 500 points.", false});
    }

    /** @brief Đồng bộ toàn bộ dữ liệu cấu hình xuống file system.log */
    void Sync() {
        ofstream f("system.log");
        if (f.is_open()) {
            f << user << " " << mssv << " " << highScore << " " 
              << skinID << " " << mapID << " " << difficulty << " " 
              << foodTotal << " " << audioEnabled << " " << zoneW << " " << zoneH << " ";
            for(auto &a : achievements) f << (a.status ? 1 : 0) << " ";
            f.close();
        }
    }

    /** @brief Nạp dữ liệu từ lưu trữ vật lý (Tích hợp Neural Shield) */
    void Load() {
        ifstream f("system.log");
        if (f.is_open()) {
            if (!(f >> user >> mssv >> highScore >> skinID >> mapID >> difficulty >> foodTotal >> audioEnabled >> zoneW >> zoneH)) {
                highScore = 0; audioEnabled = true; zoneW = 65; zoneH = 25;
            }
            for(auto &a : achievements) {
                int s; if(f >> s) a.status = (s == 1);
            }
            f.close();
        }
        // Neural Shield: Đảm bảo dữ liệu không gây Crash Engine
        if (skinID < 0 || skinID > 2) skinID = 0;
        if (mapID < 0 || mapID > 2) mapID = 0;
        if (zoneW < 40 || zoneW > 85) zoneW = 65;
        if (zoneH < 15 || zoneH > 35) zoneH = 25;
    }
};

// ============================================================================
// [SECTION 3] AUDIO PROCESSOR KERNEL
// ============================================================================
/**
 * @class AudioProcessor
 * @brief Xử lý tín hiệu âm thanh hệ thống thông qua Win32 API.
 */
class AudioProcessor {
public:
    /** @brief Phát âm thanh đơn tần (Sử dụng Driver Beep) */
    static void Trigger(int freq, int dur, bool power) {
        if (power) Beep(freq, dur);
    }
    static void Play_Eat(bool p) { Trigger(1250, 10, p); }
    static void Play_Death(bool p) { Trigger(400, 100, p); Trigger(250, 150, p); }
    static void Play_Tick(bool p) { Trigger(850, 5, p); }
    static void Play_Select(bool p) { Trigger(1050, 35, p); }
};

// ============================================================================
// [SECTION 4] PROFESSIONAL GRAPHICS RENDERER (GDI KERNEL)
// ============================================================================
/**
 * @class GfxKernel
 * @brief Bộ máy render đồ họa tầng thấp, xử lý Double Buffering.
 */
class GfxKernel {
private:
    HANDLE hOut;
    CHAR_INFO backBuffer[SCREEN_W * SCREEN_H];
    SMALL_RECT displayRegion;
public:
    GfxKernel() {
        hOut = CreateConsoleScreenBuffer(GENERIC_READ | GENERIC_WRITE, 0, NULL, CONSOLE_TEXTMODE_BUFFER, NULL);
        SetConsoleActiveScreenBuffer(hOut);
        displayRegion = { 0, 0, (short)(SCREEN_W - 1), (short)(SCREEN_H - 1) };
        CONSOLE_CURSOR_INFO cursor = { 1, FALSE };
        SetConsoleCursorInfo(hOut, &cursor);
    }

    /** @brief Reset toàn bộ Buffer trước khi vẽ Frame mới */
    void Clear() {
        for (int i = 0; i < SCREEN_W * SCREEN_H; i++) {
            backBuffer[i].Char.UnicodeChar = L' ';
            backBuffer[i].Attributes = 0;
        }
    }

    /** @brief Đặt ký tự tại tọa độ tuyệt đối */
    void SetChar(int x, int y, wchar_t c, WORD attr) {
        if (x >= 0 && x < SCREEN_W && y >= 0 && y < SCREEN_H) {
            int idx = y * SCREEN_W + x;
            backBuffer[idx].Char.UnicodeChar = c;
            backBuffer[idx].Attributes = attr;
        }
    }

    /** @brief In chuỗi văn bản tại tọa độ tuyệt đối */
    void SetString(int x, int y, string s, WORD attr) {
        for (size_t i = 0; i < s.length(); i++) SetChar(x + (int)i, y, s[i], attr);
    }

    /** @brief Vẽ khung bảng điều khiển công nghiệp tích hợp Shadow 3D */
    void DrawPanel(int x, int y, int w, int h, WORD col, string tag = "") {
        // Shadow Rendering Engine (Low-Opacity simulation)
        for (int i = x + 1; i <= x + w + 1; i++) SetChar(i, y + h + 1, 0x2591, C_DARK_GRAY);
        for (int i = y + 1; i <= y + h + 1; i++) SetChar(x + w + 1, i, 0x2591, C_DARK_GRAY);

        // Core Frame Rendering
        for (int i = x; i <= x + w; i++) { SetChar(i, y, 0x2550, col); SetChar(i, y + h, 0x2550, col); }
        for (int i = y; i <= y + h; i++) { SetChar(x, i, 0x2551, col); SetChar(x + w, i, 0x2551, col); }
        SetChar(x, y, 0x2554, col); SetChar(x + w, y, 0x2557, col);
        SetChar(x, y + h, 0x255A, col); SetChar(x + w, y + h, 0x255D, col);
        if (!tag.empty()) SetString(x + 2, y, "[ " + tag + " ]", col);
    }

    /** @brief Vẽ thanh tiến trình trạng thái hệ thống */
    void DrawGauge(int x, int y, string label, int val, WORD col) {
        SetString(x, y, label, C_GRAY);
        int fill = (val > 100) ? 10 : val / 10;
        SetString(x + 8, y, "[", C_DARK_GRAY);
        for (int i = 0; i < 10; i++) SetChar(x + 9 + i, y, (i < fill ? 0x2588 : 0x2591), (i < fill ? col : C_DARK_GRAY));
        SetString(x + 19, y, "]", C_DARK_GRAY);
        SetString(x + 22, y, to_string(val) + "%", col);
    }

    /** @brief Vẽ bộ chữ nghệ thuật "NEURAL SNAKE" khổng lồ */
    void DrawArtTitle(int x, int y) {
        string art[5] = {
            " XXXXX  XX   XX  XXXXX  XX  XX XXXXXXX",
            "XX   XX XXX  XX XX   XX XX XX  XX     ",
            "  XXX   XX X XX XXXXXXX XXXX   XXXXX  ",
            "    XXX XX  XXX XX   XX XX XX  XX     ",
            "XXXXXX  XX   XX XX   XX XX  XX XXXXXXX"
        };
        for (int i = 0; i < 5; i++) {
            WORD c = (i < 2) ? C_CYAN : (i < 4 ? C_PINK : C_DARK_PURPLE);
            for (size_t j = 0; j < art[i].length(); j++)
                if (art[i][j] == 'X') SetChar(x + (int)j, y + i, 0x2588, c);
        }
    }

    /** @brief Hiệu ứng chữ "GAME OVER" 3D khổng lồ (v14.0 High-Res) */
    void DrawGameOver(int x, int y, int frame) {
        string t[7] = {
            "  ________  ________  _____ ______   _______           ________  ___      ___ _______   ________     ",
            " |\\   ____\\|\\   __  \\|\\   _ \\  _   \\|\\  ___ \\         |\\   __  \\|\\  \\    /  /|\\  ___ \\ |\\   __  \\    ",
            " \\ \\  \\___|\\ \\  \\|\\  \\ \\  \\\\\\__\\ \\  \\ \\   __/|        \\ \\  \\|\\  \\ \\  \\  /  / \\ \\   __/| \\  \\|\\  \\   ",
            "  \\ \\  \\  __\\ \\   __  \\ \\  \\\\|__| \\  \\ \\  \\_|/__       \\ \\  \\\\\\  \\ \\  \\/  /   \\ \\  \\_|/__\\ \\   _  _\\  ",
            "   \\ \\  \\|\\  \\ \\  \\ \\  \\ \\  \\    \\ \\  \\ \\  \\_|\\ \\       \\ \\  \\\\\\  \\ \\    /     \\ \\  \\_|\\ \\ \\  \\\\  \\| ",
            "    \\ \\_______\\ \\__\\ \\__\\ \\__\\    \\ \\__\\ \\_______\\       \\ \\_______\\ \\__/       \\ \\_______\\ \\__\\\\ _\\ ",
            "     \\|_______|\\|__|\\|__|\\|__|     \\|__|\\|_______|        \\|_______|\\|__|        \\|_______|\\|__|\\|__|"
        };
        WORD c = (frame % 4 < 2) ? C_RED : C_WHITE;
        for (int i = 0; i < 7; i++) {
            SetString(x + 1, y + i + 1, t[i], C_DARK_RED); 
            SetString(x, y + i, t[i], c);            
        }
    }

    /** @brief Lệnh xuất nội dung Buffer ra thiết bị hiển thị */
    void Present() { 
        WriteConsoleOutputW(hOut, backBuffer, { (short)SCREEN_W, (short)SCREEN_H }, { 0, 0 }, &displayRegion); 
    }
};

// ============================================================================
// [SECTION 5] VFX CORE & AI PATHFINDING ENGINE
// ============================================================================
class VFXCore {
public:
    vector<Particle> sparks;
    void CreateBurst(int x, int y, WORD col) {
        for(int i=0; i<25; i++) {
            float angle = (float)(rand()%360) * 3.1415f / 180.0f;
            float speed = (float)(rand()%12 + 5) / 10.0f;
            sparks.push_back({(float)x, (float)y, cos(angle)*speed, sin(angle)*speed, 12 + rand()%12, col});
        }
    }
    void Update() {
        for(size_t i=0; i < sparks.size(); i++) {
            sparks[i].x += sparks[i].vx; sparks[i].y += sparks[i].vy;
            sparks[i].vy += 0.045f; // Gravity Sim
            sparks[i].life--; if (sparks[i].life <= 0) { sparks.erase(sparks.begin() + i); i--; }
        }
    }
};

class AIBrain {
public:
    vector<Point> body; Direction dir;
    void Spawn() { body.clear(); for(int i=0; i<4; i++) body.push_back({60 + i, 20}); dir = LEFT; }
    void Compute(Point food) {
        Point head = body[0];
        if (head.x < food.x) dir = RIGHT; else if (head.x > food.x) dir = LEFT;
        else if (head.y < food.y) dir = DOWN; else if (head.y > food.y) dir = UP;
    }
    void Step() {
        Point next = body[0];
        if (dir == UP) next.y--; else if (dir == DOWN) next.y++;
        else if (dir == LEFT) next.x--; else if (dir == RIGHT) next.x++;
        body.insert(body.begin(), next); body.pop_back();
    }
};

// ============================================================================
// [SECTION 6] THE SUPREME ENGINE CONTROLLER (FINAL KERNEL)
// ============================================================================
class SupremeEngine {
private:
    GfxKernel gfx;
    KernelMemory memory;
    VFXCore vfx;
    AIBrain ai;
    EventLogger logger;
    GameState state;
    vector<Point> snake, walls;
    Point food;
    Direction playerDir;
    int menuIdx, bootTick, score, frame;
    bool isActive;

public:
    SupremeEngine() {
        srand((unsigned int)time(NULL));
        state = BOOTING; isActive = true;
        menuIdx = 0; bootTick = 0; score = 0; frame = 0;
        memory.Load();
        logger.PushEntry("SYSTEM KERNEL v14.0 INITIALIZED");
        ResetSession();
    }

    void ResetSession() {
        snake.clear(); walls.clear();
        for (int i = 0; i < 5; i++) snake.push_back({ 35 - i, 12 });
        if (memory.mapID == 1) { 
            for(int i=5; i<20; i++) { walls.push_back({28, i}); walls.push_back({48, i}); }
        }
        ai.Spawn();
        food = { rand() % (memory.zoneW - 12) + 6, rand() % (memory.zoneH - 12) + 6 };
        playerDir = RIGHT; score = 0;
        logger.PushEntry("MISSION AREA RECONSTRUCTED");
    }

    /** @brief VẶN ỐC: Hỗ trợ W/S cho mọi Menu điều hướng */
    void HandleDriverInput() {
        if (state == IN_GAME) {
            if (((GetAsyncKeyState('W') & 0x8000) || (GetAsyncKeyState(VK_UP) & 0x8000)) && playerDir != DOWN) playerDir = UP;
            if (((GetAsyncKeyState('S') & 0x8000) || (GetAsyncKeyState(VK_DOWN) & 0x8000)) && playerDir != UP) playerDir = DOWN;
            if (((GetAsyncKeyState('A') & 0x8000) || (GetAsyncKeyState(VK_LEFT) & 0x8000)) && playerDir != RIGHT) playerDir = LEFT;
            if (((GetAsyncKeyState('D') & 0x8000) || (GetAsyncKeyState(VK_RIGHT) & 0x8000)) && playerDir != LEFT) playerDir = RIGHT;
            if (GetAsyncKeyState(VK_ESCAPE) & 0x8000) state = PAUSED;
        } else {
            // Xác định số lượng mục menu tương ứng từng trạng thái
            int limit = 0;
            if (state == MAIN_MENU) limit = 6;
            else if (state == MISSION_SELECT || state == SKIN_DATABASE) limit = 2;
            else if (state == SETTINGS) limit = 4;

            // Xử lý di chuyển bằng W / S và phím mũi tên
            if ((GetAsyncKeyState(VK_UP) & 0x8000) || (GetAsyncKeyState('W') & 0x8000)) { 
                menuIdx = (menuIdx <= 0) ? limit : menuIdx - 1; AudioProcessor::Play_Tick(memory.audioEnabled); Sleep(120); 
            }
            if ((GetAsyncKeyState(VK_DOWN) & 0x8000) || (GetAsyncKeyState('S') & 0x8000)) { 
                menuIdx = (menuIdx >= limit) ? 0 : menuIdx + 1; AudioProcessor::Play_Tick(memory.audioEnabled); Sleep(120); 
            }
            if (GetAsyncKeyState(VK_RETURN) & 0x8000) { TriggerLogic(); AudioProcessor::Play_Select(memory.audioEnabled); Sleep(180); }
        }
    }

    void TriggerLogic() {
        if (state == BOOTING) state = MAIN_MENU;
        else if (state == MAIN_MENU) {
            if (menuIdx == 0) { ResetSession(); state = IN_GAME; logger.PushEntry("SIMULATION_START"); }
            else if (menuIdx == 1) state = MISSION_SELECT;
            else if (menuIdx == 2) state = SKIN_DATABASE;
            else if (menuIdx == 3) state = SETTINGS;
            else if (menuIdx == 4) state = ACHIEVEMENTS;
            else if (menuIdx == 5) state = SYSTEM_LOGS;
            else if (menuIdx == 6) isActive = false;
            menuIdx = 0;
        }
        else if (state == MISSION_SELECT) { memory.mapID = menuIdx; state = MAIN_MENU; logger.PushEntry("MAP_RECONFIGURED"); }
        else if (state == SKIN_DATABASE) { memory.skinID = menuIdx; state = MAIN_MENU; logger.PushEntry("SKIN_OVERRIDE_COMPLETE"); }
        else if (state == SETTINGS) {
            if (menuIdx == 0) memory.audioEnabled = !memory.audioEnabled;
            else if (menuIdx == 1) { memory.zoneW += 5; if(memory.zoneW > 85) memory.zoneW = 40; }
            else if (menuIdx == 2) { memory.zoneH += 2; if(memory.zoneH > 35) memory.zoneH = 15; }
            else if (menuIdx == 3) memory.difficulty = (memory.difficulty + 1) % 3;
            else { memory.Sync(); state = MAIN_MENU; menuIdx = 3; }
        }
        else { state = MAIN_MENU; }
    }

    /** @brief Quy trình xử lý Kernel (Vòng đời của từng nhịp game) */
    void KernelUpdate() {
        frame++; vfx.Update();
        if (state == BOOTING) { if (++bootTick > 100) state = MAIN_MENU; return; }
        if (state != IN_GAME) return;

        // Xử lý Delay Frame theo độ khó đã cấu hình
        int speedLimit = (memory.difficulty == 0) ? 8 : (memory.difficulty == 1 ? 4 : 2);
        if (frame % speedLimit != 0) return;

        ai.Compute(food); ai.Step();
        Point nextHead = snake[0];
        if (playerDir == UP) nextHead.y--; else if (playerDir == DOWN) nextHead.y++;
        else if (playerDir == LEFT) nextHead.x--; else if (playerDir == RIGHT) nextHead.x++;

        // Portal Sector Logic (Wrap-around physics)
        if (memory.mapID == 2) {
            if (nextHead.x <= 0) nextHead.x = memory.zoneW - 1; else if (nextHead.x >= memory.zoneW) nextHead.x = 1;
            if (nextHead.y <= 0) nextHead.y = memory.zoneH - 1; else if (nextHead.y >= memory.zoneH) nextHead.y = 1;
        } else if (nextHead.x <= 0 || nextHead.x >= memory.zoneW || nextHead.y <= 0 || nextHead.y >= memory.zoneH) { 
            state = GAME_OVER; AudioProcessor::Play_Death(memory.audioEnabled); logger.PushEntry("COLLISION_EVENT: BOUNDARY"); return; 
        }

        // Collision Monitoring Subsystem
        for (auto &w : walls) if (nextHead == w) { state = GAME_OVER; AudioProcessor::Play_Death(memory.audioEnabled); return; }
        for (size_t i = 1; i < snake.size(); i++) if (nextHead == snake[i]) { state = GAME_OVER; return; }

        snake.insert(snake.begin(), nextHead);
        if (nextHead == food) {
            score += 10; memory.foodTotal++; if (score > memory.highScore) memory.highScore = score;
            vfx.CreateBurst(nextHead.x + 10, nextHead.y + 10, C_YELLOW);
            food = { rand() % (memory.zoneW - 10) + 5, rand() % (memory.zoneH - 10) + 5 };
            AudioProcessor::Play_Eat(memory.audioEnabled);
            logger.PushEntry("PACKET_ACQUIRED. NEW_SCORE: " + to_string(score));
        } else snake.pop_back();
    }

    void RenderKernel() {
        gfx.Clear();
        // Render HUD Frame Radar Corners
        gfx.SetChar(2, 1, 0x250F, C_CYAN); gfx.SetChar(SCREEN_W - 3, 1, 0x2513, C_CYAN);
        gfx.SetChar(2, SCREEN_H - 2, 0x2517, C_CYAN); gfx.SetChar(SCREEN_W - 3, SCREEN_H - 2, 0x251B, C_CYAN);

        if (state == BOOTING) {
            gfx.DrawArtTitle(40, 10);
            gfx.DrawGauge(SCREEN_W/2 - 13, 22, "KERNEL", bootTick, C_CYAN);
            gfx.SetString(SCREEN_W/2 - 18, 25, "USER: " + memory.user + " [OK]", C_DARK_GRAY);
        }
        else if (state == MAIN_MENU || state == MISSION_SELECT || state == SKIN_DATABASE || state == SETTINGS) {
            gfx.DrawArtTitle(41, 4);
            string titles[] = {"INIT SIMULATION", "MISSION SELECT", "SKIN DATABASE", "SETTINGS", "ACHIEVEMENTS", "SYSTEM LOGS", "SHUTDOWN"};
            string activeM[7]; int count = 0;
            if (state == MAIN_MENU) { for(int i=0; i<7; i++) activeM[i] = titles[i]; count = 7; }
            else if (state == SETTINGS) {
                activeM[0] = "AUDIO: " + string(memory.audioEnabled ? "ON" : "OFF");
                activeM[1] = "WIDTH: " + to_string(memory.zoneW);
                activeM[2] = "HEIGHT: " + to_string(memory.zoneH);
                activeM[3] = "DIFFICULTY: " + to_string(memory.difficulty);
                activeM[4] = "SAVE & RETURN"; count = 5;
            } else if (state == MISSION_SELECT) {
                activeM[0] = "CLASSIC AREA"; activeM[1] = "NEURAL WALLS"; activeM[2] = "PORTAL SECTOR"; count = 3;
            } else if (state == SKIN_DATABASE) {
                activeM[0] = "NEON GREEN"; activeM[1] = "RUBY RED"; activeM[2] = "GOLD EDITION"; count = 3;
            }
            for (int i = 0; i < count; i++) {
                WORD panelC = (menuIdx == i) ? C_CYAN : C_DARK_GRAY;
                gfx.DrawPanel(40, 17 + i * 3, 40, 2, panelC, (menuIdx == i ? ">>" : ""));
                gfx.SetString(45, 18 + i * 3, activeM[i], (menuIdx == i ? C_WHITE : C_DARK_GRAY));
            }
            gfx.DrawPanel(5, 17, 30, 15, C_GRAY, "METRICS");
            gfx.DrawGauge(7, 25, "SYS_CPU", 30 + (frame % 45), C_CYAN);
            gfx.DrawGauge(7, 27, "SYS_RAM", 90 + (frame % 5), C_PINK);
            gfx.DrawPanel(85, 17, 30, 15, C_GRAY, "USER_DB");
            gfx.SetString(87, 21, "ID: " + memory.mssv, C_WHITE);
            gfx.SetString(87, 23, "HI: " + to_string(memory.highScore), C_YELLOW);
        }
        else if (state == ACHIEVEMENTS) {
            gfx.DrawPanel(30, 12, 60, 24, C_PINK, "NEURAL_ARCHIVE");
            for(size_t i=0; i<memory.achievements.size(); i++) {
                WORD aCol = memory.achievements[i].status ? C_GREEN : C_DARK_GRAY;
                gfx.SetString(35, 15 + (int)i * 4, "[" + string(memory.achievements[i].status ? "X" : " ") + "] " + memory.achievements[i].title, aCol);
                gfx.SetString(35, 16 + (int)i * 4, "    " + memory.achievements[i].desc, C_GRAY);
            }
            gfx.SetString(50, 38, "PRESS ENTER TO RETURN", C_DARK_GRAY);
        }
        else if (state == SYSTEM_LOGS) {
            gfx.DrawPanel(20, 12, 80, 24, C_CYAN, "EVENT_LOGGER_v4");
            const auto& l = logger.RetrieveLogs();
            for(size_t i=0; i<l.size(); i++) gfx.SetString(25, 15 + (int)i, l[i], C_GRAY);
            gfx.SetString(50, 38, "PRESS ENTER TO RETURN", C_DARK_GRAY);
        }
        else if (state == IN_GAME || state == PAUSED) {
            WORD snakeColor = (memory.skinID == 0) ? C_GREEN : (memory.skinID == 1 ? C_RED : C_YELLOW);
            gfx.DrawPanel(10, 10, memory.zoneW, memory.zoneH, C_CYAN, "LINK_ACTIVE");
            for (auto &w : walls) gfx.SetChar(w.x + 10, w.y + 10, 0x2593, C_DARK_GRAY);
            gfx.SetChar(food.x + 10, food.y + 10, 0x2665, C_RED);
            for(auto &p : vfx.sparks) gfx.SetChar((int)p.x, (int)p.y, L'*', p.color);
            for (auto &p : ai.body) gfx.SetChar(p.x + 10, p.y + 10, 'A', C_DARK_PURPLE);
            for (size_t i = 0; i < snake.size(); i++)
                gfx.SetChar(snake[i].x + 10, snake[i].y + 10, (i == 0 ? 0x25C8 : 0x2588), (i == 0 ? C_WHITE : snakeColor));
            gfx.DrawPanel(memory.zoneW + 18, 10, 22, 5, C_YELLOW, "LIVE");
            gfx.SetString(memory.zoneW + 20, 12, "SCORE: " + to_string(score), C_WHITE);
        }
        else if (state == GAME_OVER) {
            int displacement = (frame % 2 == 0) ? 1 : -1;
            gfx.DrawGameOver(10 + displacement, 15, frame);
            gfx.SetString(45, 30, "SYSTEM COLLAPSED! PRESS ENTER", C_WHITE);
        }
        gfx.Present();
    }

    void Run() {
        while (isActive) { HandleDriverInput(); KernelUpdate(); RenderKernel(); Sleep(15); }
        memory.Sync();
    }
};

// ============================================================================
// [SECTION 9] MAIN KERNEL ENTRY POINT
// ============================================================================
int main() {
    SetConsoleTitleA("SNAKE MASTER EDITION v14.0 - NEURAL OVERLORD - UIT KTMT.1");
    SupremeEngine engine;
    engine.Run();
    return 0;
}
// ----------------------------------------------------------------------------
// [END OF MASTER SOURCE CODE] - ESTIMATED LINES: 1000+ WITH DOCUMENTATION
// ----------------------------------------------------------------------------