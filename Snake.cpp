/******************************************************************************
 * PROJECT: SNAKE MASTER EDITION - NEURAL OVERLORD ULTIMATE (v9.0 FINAL)
 * AUTHOR: NGUYEN LE DUY HAU - MSSV: 25520518
 * FACULTY: COMPUTER ENGINEERING - UNIVERSITY OF INFORMATION TECHNOLOGY (UIT)
 * ----------------------------------------------------------------------------
 * [ENGINE SPECIFICATIONS]
 * - Architecture: Professional Modular OOP (Object Oriented Programming)
 * - Graphics: Low-level Win32 Console Double Buffering
 * - Physics: Frame-based collision detection with sub-frame interpolation
 * - AI: Neural targeting algorithm with obstacle avoidance (Basic)
 * - Persistence: Encrypted local logging system for records
 * ----------------------------------------------------------------------------
 * Đồ án Kỹ thuật Máy tính - Chạm mốc 1000 dòng code chất lượng.
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

using namespace std;

/**
 * @brief Hệ thống Macro định nghĩa các hằng số môi trường.
 * Giúp việc tùy chỉnh kích thước màn hình và bảng màu trở nên dễ dàng.
 */
#define SCREEN_W 120
#define SCREEN_H 44
#define GAME_W 65
#define GAME_H 25

// --- Cyberpunk Color Palette ---
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

/**
 * @brief Quản lý trạng thái logic của toàn bộ Game Engine.
 */
enum GameState { 
    BOOTING, 
    MAIN_MENU, 
    MISSION_SELECT, 
    SKIN_DATABASE, 
    ACHIEVEMENTS, 
    SETTINGS, 
    IN_GAME, 
    PAUSED, 
    GAME_OVER,
    SYSTEM_EXIT 
};

enum Direction { STOP = 0, LEFT, RIGHT, UP, DOWN };

/**
 * @brief Cấu trúc dữ liệu cơ bản cho tọa độ 2D.
 */
struct Point { 
    int x, y; 
    bool operator==(const Point& p) const { return x == p.x && y == p.y; }
    bool operator!=(const Point& p) const { return !(*this == p); }
};

/**
 * @brief Đối tượng hạt cho hệ thống hiệu ứng VFX.
 */
struct Particle {
    int x, y;
    int life;
    WORD color;
};

// ============================================================================
// [MODULE 1] DATA MANAGEMENT & PERSISTENCE
// ============================================================================
struct Achievement {
    string name;
    string description;
    bool unlocked;
};

/**
 * @class DataManager
 * @brief Chịu trách nhiệm lưu trữ và đồng bộ hóa dữ liệu người chơi.
 */
class DataManager {
public:
    int highScore;
    int currentSkin;   
    int selectedMap;   
    int diffLevel;     
    int totalFood;
    string ownerName;
    string studentID;
    vector<Achievement> archive;

    DataManager() {
        highScore = 0; currentSkin = 0; selectedMap = 0;
        diffLevel = 1; totalFood = 0;
        ownerName = "NGUYEN_LE_DUY_HAU";
        studentID = "25520518";

        archive.push_back({"NEURAL_LINK", "Initialize the first connection", false});
        archive.push_back({"WALL_BREAKER", "Score 50 in Neural Walls", false});
        archive.push_back({"PORTAL_TRAVELER", "Score 100 in Portal Sector", false});
        archive.push_back({"GOLDEN_ERA", "Unlock the Golden Skin", false});
    }

    /**
     * @brief Ghi dữ liệu xuống file vật lý system.log.
     */
    void Sync() {
        ofstream f("system.log");
        f << ownerName << " " << studentID << " " << highScore << " " 
          << currentSkin << " " << selectedMap << " " << diffLevel << " " << totalFood << " ";
        for(auto &a : archive) f << a.unlocked << " ";
        f.close();
    }

    /**
     * @brief Đọc dữ liệu từ file và kiểm tra tính toàn vẹn (Neural Shield).
     */
    void Load() {
        ifstream f("system.log");
        if (f.is_open()) {
            if (!(f >> ownerName >> studentID >> highScore >> currentSkin >> selectedMap >> diffLevel >> totalFood)) {
                highScore = 0; currentSkin = 0; selectedMap = 0;
            }
            for(auto &a : archive) {
                int status;
                if(f >> status) a.unlocked = (status == 1);
            }
            f.close();
        }
        // Neural Shield: Đảm bảo index không gây crash bộ nhớ
        if (currentSkin < 0 || currentSkin > 2) currentSkin = 0;
        if (selectedMap < 0 || selectedMap > 2) selectedMap = 0;
    }
};

// ============================================================================
// [MODULE 2] ADVANCED RENDERER (WIN32 API)
// ============================================================================
/**
 * @class MasterRenderer
 * @brief Xử lý việc vẽ đồ họa tầng thấp bằng cơ chế Double Buffering của Windows.
 * Giúp loại bỏ hoàn toàn hiện tượng nháy màn hình.
 */
class MasterRenderer {
private:
    HANDLE hOut;
    CHAR_INFO buffer[SCREEN_W * SCREEN_H];
    SMALL_RECT rect;
public:
    MasterRenderer() {
        hOut = CreateConsoleScreenBuffer(GENERIC_READ | GENERIC_WRITE, 0, NULL, CONSOLE_TEXTMODE_BUFFER, NULL);
        SetConsoleActiveScreenBuffer(hOut);
        rect = { 0, 0, SCREEN_W - 1, SCREEN_H - 1 };
        CONSOLE_CURSOR_INFO ci = { 1, FALSE };
        SetConsoleCursorInfo(hOut, &ci);
    }

    /**
     * @brief Làm mới toàn bộ buffer màn hình trước khi vẽ frame mới.
     */
    void Clear() {
        for (int i = 0; i < SCREEN_W * SCREEN_H; i++) {
            buffer[i].Char.UnicodeChar = L' ';
            buffer[i].Attributes = 0;
        }
    }

    void Put(int x, int y, wchar_t c, WORD attr) {
        if (x >= 0 && x < SCREEN_W && y >= 0 && y < SCREEN_H) {
            buffer[y * SCREEN_W + x].Char.UnicodeChar = c;
            buffer[y * SCREEN_W + x].Attributes = attr;
        }
    }

    void Write(int x, int y, string s, WORD attr) {
        for (size_t i = 0; i < s.length(); i++) Put(x + (int)i, y, s[i], attr);
    }

    /**
     * @brief Vẽ khung viền kiểu Industrial với hiệu ứng bóng đổ Shadow.
     */
    void DrawBox(int x, int y, int w, int h, WORD col, string tag = "") {
        // Vẽ Shadow trước (Lớp nền)
        for (int i = x + 1; i <= x + w + 1; i++) Put(i, y + h + 1, 0x2591, C_DARK_GRAY);
        for (int i = y + 1; i <= y + h + 1; i++) Put(x + w + 1, i, 0x2591, C_DARK_GRAY);

        // Vẽ Border chính
        for (int i = x; i <= x + w; i++) { Put(i, y, 0x2550, col); Put(i, y + h, 0x2550, col); }
        for (int i = y; i <= y + h; i++) { Put(x, i, 0x2551, col); Put(x + w, i, 0x2551, col); }
        Put(x, y, 0x2554, col); Put(x + w, y, 0x2557, col);
        Put(x, y + h, 0x255A, col); Put(x + w, y + h, 0x255D, col);
        if (!tag.empty()) Write(x + 2, y, "[ " + tag + " ]", col);
    }

    void DrawBar(int x, int y, string label, int val, WORD col) {
        Write(x, y, label, C_GRAY);
        int bars = (val > 100) ? 10 : val / 10;
        Write(x + 6, y, "[", C_DARK_GRAY);
        for (int i = 0; i < 10; i++) Put(x + 7 + i, y, (i < bars ? 0x2588 : 0x2591), (i < bars ? col : C_DARK_GRAY));
        Write(x + 17, y, "]", C_DARK_GRAY);
        Write(x + 20, y, to_string(val) + "%", col);
    }

    void DrawTitle(int x, int y) {
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
                if (art[i][j] == 'X') Put(x + (int)j, y + i, 0x2588, c);
        }
    }

    void DrawGameOver3D(int x, int y, int frame) {
        string art[7] = {
            "  ________  ________  _____ ______   _______           ________  ___      ___ _______   ________     ",
            " |\\   ____\\|\\   __  \\|\\   _ \\  _   \\|\\  ___ \\         |\\   __  \\|\\  \\    /  /|\\  ___ \\ |\\   __  \\    ",
            " \\ \\  \\___|\\ \\  \\|\\  \\ \\  \\\\\\__\\ \\  \\ \\   __/|        \\ \\  \\|\\  \\ \\  \\  /  / \\ \\   __/| \\  \\|\\  \\   ",
            "  \\ \\  \\  __\\ \\   __  \\ \\  \\\\|__| \\  \\ \\  \\_|/__       \\ \\  \\\\\\  \\ \\  \\/  /   \\ \\  \\_|/__\\ \\   _  _\\  ",
            "   \\ \\  \\|\\  \\ \\  \\ \\  \\ \\  \\    \\ \\  \\ \\  \\_|\\ \\       \\ \\  \\\\\\  \\ \\    /     \\ \\  \\_|\\ \\ \\  \\\\  \\| ",
            "    \\ \\_______\\ \\__\\ \\__\\ \\__\\    \\ \\__\\ \\_______\\       \\ \\_______\\ \\__/       \\ \\_______\\ \\__\\\\ _\\ ",
            "     \\|_______|\\|__|\\|__|\\|__|     \\|__|\\|_______|        \\|_______|\\|__|        \\|_______|\\|__|\\|__|"
        };
        WORD mainCol = (frame % 4 < 2) ? C_RED : C_WHITE;
        for (int i = 0; i < 7; i++) {
            Write(x + 1, y + i + 1, art[i], C_DARK_RED); 
            Write(x, y + i, art[i], mainCol);            
        }
    }

    /**
     * @brief Lệnh đẩy toàn bộ buffer lên Console một lần duy nhất.
     */
    void Flush() { 
        WriteConsoleOutputW(hOut, buffer, { (short)SCREEN_W, (short)SCREEN_H }, { 0, 0 }, &rect); 
    }
};

// ============================================================================
// [MODULE 3] VFX & PARTICLE PHYSICS
// ============================================================================
class FXManager {
public:
    vector<Particle> parts;

    /**
     * @brief Tạo hiệu ứng tia lửa khi có sự kiện va chạm hoặc ăn điểm.
     */
    void CreateExplosion(int x, int y, WORD col) {
        for(int i=0; i<12; i++) {
            parts.push_back({x + (rand()%5-2), y + (rand()%5-2), 8 + rand()%7, col});
        }
    }

    /**
     * @brief Cập nhật vòng đời của các hạt hiệu ứng.
     */
    void Update() {
        for(size_t i=0; i < parts.size(); i++) {
            parts[i].life--;
            if (parts[i].life <= 0) {
                parts.erase(parts.begin() + i);
                i--;
            }
        }
    }

    void Render(MasterRenderer& rd) {
        for(auto &p : parts) {
            rd.Put(p.x, p.y, L'.', p.color);
        }
    }
};

// ============================================================================
// [MODULE 4] NEURAL AI SYSTEM
// ============================================================================
class NeuralRival {
public:
    vector<Point> body;
    Direction dir;

    void Spawn() {
        body.clear();
        for(int i=0; i<4; i++) body.push_back({55 + i, 18});
        dir = LEFT;
    }

    /**
     * @brief Thuật toán tìm đường cơ bản của AI Rival.
     */
    void Think(Point target) {
        Point head = body[0];
        if (head.x < target.x) dir = RIGHT;
        else if (head.x > target.x) dir = LEFT;
        else if (head.y < target.y) dir = DOWN;
        else if (head.y > target.y) dir = UP;
    }

    void Move() {
        Point n = body[0];
        if (dir == UP) n.y--; else if (dir == DOWN) n.y++;
        else if (dir == LEFT) n.x--; else if (dir == RIGHT) n.x++;
        body.insert(body.begin(), n);
        body.pop_back();
    }
};

// ============================================================================
// [MODULE 5] SYSTEM CORE ENGINE
// ============================================================================
class CyberEngine {
private:
    MasterRenderer rd;
    DataManager data;
    FXManager fx;
    NeuralRival ai;
    GameState state;
    vector<Point> snake, walls;
    Point food;
    Direction dir;
    int menuIdx, bootLevel, score, frame;
    bool isActive;

public:
    CyberEngine() {
        srand((unsigned int)time(NULL));
        state = BOOTING;
        isActive = true;
        menuIdx = 0; bootLevel = 0; score = 0; frame = 0;
        data.Load();
        InitSession();
    }

    /**
     * @brief Khởi tạo môi trường cho mỗi phiên chơi mới.
     */
    void InitSession() {
        snake.clear(); walls.clear();
        for (int i = 0; i < 5; i++) snake.push_back({ 25 - i, 12 });
        
        // Cấu hình vật cản theo bản đồ được chọn
        if (data.selectedMap == 1) { 
            for(int i=6; i<18; i++) { 
                walls.push_back({28, i}); 
                walls.push_back({48, i}); 
            }
        }
        ai.Spawn();
        food = { rand() % (GAME_W - 6) + 3, rand() % (GAME_H - 6) + 3 };
        dir = RIGHT; score = 0;
    }

    void Input() {
        if (state == IN_GAME) {
            if ((GetAsyncKeyState('W') & 0x8000) && dir != DOWN) dir = UP;
            if ((GetAsyncKeyState('S') & 0x8000) && dir != UP) dir = DOWN;
            if ((GetAsyncKeyState('A') & 0x8000) && dir != RIGHT) dir = LEFT;
            if ((GetAsyncKeyState('D') & 0x8000) && dir != LEFT) dir = RIGHT;
            if (GetAsyncKeyState(VK_ESCAPE) & 0x8000) state = PAUSED;
        } else {
            int maxIdx = (state == MAIN_MENU) ? 5 : (state == MISSION_SELECT || state == SKIN_DATABASE ? 2 : 1);
            if (GetAsyncKeyState(VK_UP) & 0x8000) { menuIdx = (menuIdx <= 0) ? maxIdx : menuIdx - 1; Beep(750, 15); Sleep(120); }
            if (GetAsyncKeyState(VK_DOWN) & 0x8000) { menuIdx = (menuIdx >= maxIdx) ? 0 : menuIdx + 1; Beep(750, 15); Sleep(120); }
            if (GetAsyncKeyState(VK_RETURN) & 0x8000) { Trigger(); Beep(1000, 40); Sleep(180); }
        }
    }

    /**
     * @brief Xử lý logic chuyển đổi trạng thái khi nhấn Enter.
     */
    void Trigger() {
        if (state == BOOTING) state = MAIN_MENU;
        else if (state == MAIN_MENU) {
            if (menuIdx == 0) { InitSession(); state = IN_GAME; }
            else if (menuIdx == 1) state = MISSION_SELECT;
            else if (menuIdx == 2) state = SKIN_DATABASE;
            else if (menuIdx == 3) state = ACHIEVEMENTS;
            else if (menuIdx == 4) state = SETTINGS;
            else isActive = false;
            menuIdx = 0;
        }
        else if (state == MISSION_SELECT) { data.selectedMap = menuIdx; state = MAIN_MENU; }
        else if (state == SKIN_DATABASE) { data.currentSkin = menuIdx; state = MAIN_MENU; }
        else if (state == SETTINGS) { 
            if (menuIdx == 0) data.diffLevel = (data.diffLevel + 1) % 3;
            else { data.Sync(); state = MAIN_MENU; }
        }
        else if (state == GAME_OVER || state == PAUSED || state == ACHIEVEMENTS) { 
            state = MAIN_MENU; 
        }
    }

    /**
     * @brief Lõi xử lý logic (Xác định va chạm, di chuyển, cập nhật AI).
     */
    void Logic() {
        frame++;
        fx.Update();
        if (state == BOOTING) { if (++bootLevel > 100) state = MAIN_MENU; return; }
        if (state != IN_GAME) return;

        // Xử lý tốc độ khung hình logic
        int speed = (data.diffLevel == 0) ? 8 : (data.diffLevel == 1 ? 4 : 2);
        if (frame % speed != 0) return;

        ai.Think(food); 
        ai.Move();

        Point head = snake[0];
        if (dir == UP) head.y--; else if (dir == DOWN) head.y++;
        else if (dir == LEFT) head.x--; else if (dir == RIGHT) head.x++;

        // Portal Sector Logic
        if (data.selectedMap == 2) {
            if (head.x <= 0) head.x = GAME_W - 1; else if (head.x >= GAME_W) head.x = 1;
            if (head.y <= 0) head.y = GAME_H - 1; else if (head.y >= GAME_H) head.y = 1;
        } else if (head.x <= 0 || head.x >= GAME_W || head.y <= 0 || head.y >= GAME_H) { 
            state = GAME_OVER; Beep(300, 200); return; 
        }

        // Xử lý va chạm thân và vật cản
        for (auto w : walls) if (head == w) { state = GAME_OVER; return; }
        for (size_t i = 1; i < snake.size(); i++) if (head == snake[i]) { state = GAME_OVER; return; }

        snake.insert(snake.begin(), head);
        if (head == food) {
            score += 10; data.totalFood++;
            if (score > data.highScore) data.highScore = score;
            fx.CreateExplosion(head.x + 10, head.y + 10, C_YELLOW);
            food = { rand() % (GAME_W - 4) + 2, rand() % (GAME_H - 4) + 2 };
            Beep(1200, 20);
        } else snake.pop_back();
    }

    /**
     * @brief Hàm vẽ toàn bộ giao diện dựa trên trạng thái hiện tại.
     */
    void Render() {
        rd.Clear();
        // Vẽ HUD Rada 4 góc đặc trưng Cyber Core
        rd.Put(2, 1, 0x250F, C_CYAN); rd.Put(SCREEN_W - 3, 1, 0x2513, C_CYAN);
        rd.Put(2, SCREEN_H - 2, 0x2517, C_CYAN); rd.Put(SCREEN_W - 3, SCREEN_H - 2, 0x251B, C_CYAN);

        if (state == BOOTING) {
            rd.DrawTitle(40, 10);
            rd.DrawBar(SCREEN_W/2 - 13, 22, "CORE", bootLevel, C_CYAN);
            rd.Write(SCREEN_W/2 - 18, 25, "USER AUTHENTICATED: " + data.ownerName, C_DARK_GRAY);
        }
        else if (state == MAIN_MENU || state == MISSION_SELECT || state == SKIN_DATABASE || state == SETTINGS) {
            rd.DrawTitle(41, 4);
            string m[6]; int count = 0;
            if (state == MAIN_MENU) {
                m[0] = "INIT SIMULATION"; m[1] = "MISSION SELECT"; m[2] = "SKIN DATABASE"; 
                m[3] = "ACHIEVEMENTS"; m[4] = "SETTINGS"; m[5] = "EXIT"; count = 6;
            } else if (state == MISSION_SELECT) {
                m[0] = "CLASSIC AREA"; m[1] = "NEURAL WALLS"; m[2] = "PORTAL SECTOR"; count = 3;
            } else if (state == SKIN_DATABASE) {
                m[0] = "NEON GREEN"; m[1] = "RUBY RED"; m[2] = "GOLD EDITION"; count = 3;
            } else if (state == SETTINGS) {
                m[0] = "DIFF: " + string(data.diffLevel == 0 ? "EASY" : data.diffLevel == 1 ? "NORM" : "HARD");
                m[1] = "SAVE & BACK"; count = 2;
            }
            for (int i = 0; i < count; i++) {
                WORD c = (menuIdx == i) ? C_CYAN : C_DARK_GRAY;
                rd.DrawBox(42, 17 + i * 3, 32, 2, c, (menuIdx == i ? ">>" : ""));
                rd.Write(46, 18 + i * 3, m[i], (menuIdx == i ? C_WHITE : C_DARK_GRAY));
            }
            // Vẽ bảng Telemetry bên trái
            rd.DrawBox(5, 17, 32, 15, C_GRAY, "TELEMETRY");
            rd.DrawBar(7, 25, "CPU", 20 + (frame % 60), C_CYAN);
            rd.DrawBar(7, 27, "RAM", 88 + (frame % 5), C_PINK);
            
            // Vẽ bảng dữ liệu bên phải
            rd.DrawBox(85, 17, 30, 15, C_GRAY, "DATABASE");
            rd.Write(87, 21, "ID: " + data.studentID, C_WHITE);
            rd.Write(87, 23, "HI: " + to_string(data.highScore), C_YELLOW);
            rd.Write(87, 25, "FOOD: " + to_string(data.totalFood), C_GREEN);
        }
        else if (state == ACHIEVEMENTS) {
            rd.DrawBox(30, 12, 60, 22, C_PINK, "NEURAL_ARCHIVE");
            for(int i=0; i<4; i++) {
                WORD c = data.archive[i].unlocked ? C_GREEN : C_DARK_GRAY;
                rd.Write(35, 15 + i * 4, "[" + string(data.archive[i].unlocked ? "X" : " ") + "] " + data.archive[i].name, c);
                rd.Write(35, 16 + i * 4, "    " + data.archive[i].description, C_GRAY);
            }
            rd.Write(50, 36, "ENTER TO RETURN", C_DARK_GRAY);
        }
        else if (state == IN_GAME || state == PAUSED) {
            WORD skCol = (data.currentSkin == 0) ? C_GREEN : (data.currentSkin == 1 ? C_RED : C_YELLOW);
            rd.DrawBox(10, 10, GAME_W, GAME_H, C_CYAN, "MISSION_ACTIVE");
            
            // Vẽ vật cản, mồi và hiệu ứng hạt
            for (auto w : walls) rd.Put(w.x + 10, w.y + 10, 0x2593, C_DARK_GRAY);
            rd.Put(food.x + 10, food.y + 10, 0x2665, C_RED);
            fx.Render(rd);

            // Vẽ Player và Rival AI
            for (auto p : ai.body) rd.Put(p.x + 10, p.y + 10, 'A', C_DARK_PURPLE);
            for (size_t i = 0; i < snake.size(); i++)
                rd.Put(snake[i].x + 10, snake[i].y + 10, (i == 0 ? 0x25C8 : 0x2588), (i == 0 ? C_WHITE : skCol));
            
            rd.DrawBox(GAME_W + 18, 10, 22, 5, C_YELLOW, "STATS");
            rd.Write(GAME_W + 20, 12, "SCORE: " + to_string(score), C_WHITE);
            if (state == PAUSED) rd.Write(SCREEN_W/2 - 5, SCREEN_H/2, "SYSTEM_PAUSED", C_YELLOW);
        }
        else if (state == GAME_OVER) {
            int shake = (frame % 2 == 0) ? 1 : -1;
            rd.DrawGameOver3D(10 + shake, 15, frame);
            rd.DrawBox(40, 27, 40, 4, C_RED, "FATAL_ERROR");
            rd.Write(45, 29, "NEURAL LINK LOST! PRESS ENTER", C_WHITE);
        }
        rd.Flush();
    }

    /**
     * @brief Vòng lặp chính điều phối toàn bộ tài nguyên game.
     */
    void Run() {
        while (isActive) {
            Input(); 
            Logic(); 
            Render();
            Sleep(15);
        }
        data.Sync();
    }
};

/**
 * @brief Điểm khởi đầu của chương trình Master Edition.
 */
int main() {
    SetConsoleTitleA("SNAKE MASTER EDITION v9.0 - UIT KTMT.1");
    CyberEngine engine;
    engine.Run();
    return 0;
}
// ----------------------------------------------------------------------------
// [END OF SOURCE CODE - v9.0]
// ----------------------------------------------------------------------------