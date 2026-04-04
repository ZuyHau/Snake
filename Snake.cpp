/******************************************************************************
 * PROJECT: SNAKE MASTER EDITION - CYBER CORE ULTIMATE (v5.0 FINAL)
 * AUTHOR: NGUYEN LE DUY HAU - MSSV: 25520518
 * FACULTY: COMPUTER ENGINEERING - UIT (VNU)
 * "Vặn ốc" tối thượng - Full HUD, Multiverse, Skin Database & Neural Shield
 ******************************************************************************/

#include <iostream>
#include <vector>
#include <string>
#include <windows.h>
#include <conio.h>
#include <ctime>
#include <fstream>

using namespace std;

// --- CONFIG HỆ THỐNG ---
#define SCREEN_W 120
#define SCREEN_H 42
#define GAME_W 65
#define GAME_H 25

// Cyber-Core Color Palette
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

enum State { BOOTING, MAIN_MENU, MISSION_SELECT, SKIN_DATABASE, SETTINGS, IN_GAME, PAUSED, CRITICAL_FAILURE };
enum Dir { STOP = 0, LEFT, RIGHT, UP, DOWN };
struct Point { int x, y; };

// ============================================================================
// [MODULE 1] DATA PERSISTENCE (NEURAL SHIELD)
// ============================================================================
class EngineData {
public:
    int highScore = 0;
    int currentSkin = 0;   // 0: Neon, 1: Ruby, 2: Gold
    int selectedLevel = 0; // 0: Classic, 1: Walls, 2: Portal
    int diffLevel = 1;
    string owner = "NGUYEN_LE_DUY_HAU";
    string id = "25520518";

    void Load() {
        ifstream f("system.log");
        if (f.is_open()) {
            if (!(f >> owner >> id >> highScore >> currentSkin >> selectedLevel >> diffLevel)) {
                highScore = 0; currentSkin = 0; selectedLevel = 0; diffLevel = 1;
            }
            f.close();
            if (currentSkin < 0 || currentSkin > 2) currentSkin = 0;
            if (selectedLevel < 0 || selectedLevel > 2) selectedLevel = 0;
        }
    }
    void Sync() {
        ofstream f("system.log");
        f << owner << " " << id << " " << highScore << " " << currentSkin << " " << selectedLevel << " " << diffLevel;
        f.close();
    }
};

// ============================================================================
// [MODULE 2] CORE RENDERER (WIN32 BUFFER)
// ============================================================================
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
    void ResetBuffer() {
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
    void DrawShadow(int x, int y, int w, int h) {
        for (int i = x + 1; i < x + w + 1; i++) Put(i, y + h + 1, 0x2593, C_DARK_GRAY);
        for (int i = y + 1; i <= y + h + 1; i++) Put(x + w + 1, i, 0x2593, C_DARK_GRAY);
    }
    void DrawIndustrialBorder(int x, int y, int w, int h, WORD col, bool shadow = false, string tag = "") {
        if (shadow) DrawShadow(x, y, w, h);
        for (int i = x; i <= x + w; i++) { Put(i, y, 0x2550, col); Put(i, y + h, 0x2550, col); }
        for (int i = y; i <= y + h; i++) { Put(x, i, 0x2551, col); Put(x + w, i, 0x2551, col); }
        Put(x, y, 0x2554, col); Put(x + w, y, 0x2557, col);
        Put(x, y + h, 0x255A, col); Put(x + w, y + h, 0x255D, col);
        if (!tag.empty()) Write(x + 2, y, "[ " + tag + " ]", col);
    }
    void DrawUltimateSnakeTitle(int x, int y) {
        string line[5] = {
            " XXXXX  XX   XX  XXXXX  XX  XX XXXXXXX",
            "XX   XX XXX  XX XX   XX XX XX  XX     ",
            "  XXX   XX X XX XXXXXXX XXXX   XXXXX  ",
            "    XXX XX  XXX XX   XX XX XX  XX     ",
            "XXXXXX  XX   XX XX   XX XX  XX XXXXXXX"
        };
        for (int i = 0; i < 5; i++) {
            WORD col = (i < 2) ? C_CYAN : (i < 4 ? C_PINK : C_DARK_PURPLE);
            for (size_t j = 0; j < line[i].length(); j++)
                if (line[i][j] == 'X') Put(x + (int)j, y + i, 0x2588, col);
        }
    }
    void DrawProgressBar(int x, int y, string label, int percent, WORD col) {
        Write(x, y, label, C_GRAY);
        int bars = percent / 10;
        for (int i = 0; i < 10; i++) Put(x + 7 + i, y, (i < bars ? 0x2588 : 0x2591), (i < bars ? col : C_DARK_GRAY));
        Write(x + 19, y, to_string(percent) + "%", col);
    }
    void Flush() { WriteConsoleOutputW(hOut, buffer, { (short)SCREEN_W, (short)SCREEN_H }, { 0, 0 }, &rect); }
};

// ============================================================================
// [MODULE 3] GAME ENGINE ARCHITECTURE
// ============================================================================
class SnakeEliteEngine {
private:
    MasterRenderer rd;
    EngineData data;
    State state = BOOTING;
    vector<Point> snake, walls;
    Point food;
    Dir direction = RIGHT;
    int menuIdx = 0, bootLevel = 0, score = 0;
    long long frameCount = 0;
    bool isActive = true;

public:
    SnakeEliteEngine() { srand(time(0)); data.Load(); InitSnake(); }

    void InitSnake() {
        snake.clear(); walls.clear();
        for (int i = 0; i < 5; i++) snake.push_back({ 20 - i, 12 });
        if (data.selectedLevel == 1) { // Neural Walls
            for (int i = 6; i < 18; i++) { walls.push_back({ 25, i }); walls.push_back({ 45, i }); }
        }
        food = { 35, 15 }; score = 0; direction = RIGHT;
    }

    void PollInput() {
        if (state == IN_GAME) {
            if ((GetAsyncKeyState('W') & 0x8000) && direction != DOWN) direction = UP;
            if ((GetAsyncKeyState('S') & 0x8000) && direction != UP) direction = DOWN;
            if ((GetAsyncKeyState('A') & 0x8000) && direction != RIGHT) direction = LEFT;
            if ((GetAsyncKeyState('D') & 0x8000) && direction != LEFT) direction = RIGHT;
            if (GetAsyncKeyState(VK_ESCAPE) & 0x8000) state = PAUSED;
        } else {
            int maxIdx = (state == MAIN_MENU) ? 4 : (state == MISSION_SELECT || state == SKIN_DATABASE ? 2 : 1);
            if (GetAsyncKeyState(VK_UP) & 0x8000) { menuIdx = (menuIdx <= 0) ? maxIdx : menuIdx - 1; Beep(700, 20); Sleep(150); }
            if (GetAsyncKeyState(VK_DOWN) & 0x8000) { menuIdx = (menuIdx >= maxIdx) ? 0 : menuIdx + 1; Beep(700, 20); Sleep(150); }
            if (GetAsyncKeyState(VK_RETURN) & 0x8000) { TriggerAction(); Beep(900, 50); Sleep(200); }
        }
    }

    void TriggerAction() {
        if (state == BOOTING) state = MAIN_MENU;
        else if (state == MAIN_MENU) {
            if (menuIdx == 0) { InitSnake(); state = IN_GAME; }
            else if (menuIdx == 1) { state = MISSION_SELECT; menuIdx = 0; }
            else if (menuIdx == 2) { state = SKIN_DATABASE; menuIdx = 0; }
            else if (menuIdx == 3) { state = SETTINGS; menuIdx = 0; }
            else isActive = false;
        } 
        else if (state == MISSION_SELECT) {
            if (menuIdx < 3) { data.selectedLevel = menuIdx; data.Sync(); state = MAIN_MENU; }
        }
        else if (state == SKIN_DATABASE) {
            if (menuIdx < 3) { data.currentSkin = menuIdx; data.Sync(); state = MAIN_MENU; }
        }
        else if (state == SETTINGS) {
            if (menuIdx == 0) data.diffLevel = (data.diffLevel + 1) % 3;
            else { data.Sync(); state = MAIN_MENU; }
        }
        else if (state == CRITICAL_FAILURE || state == PAUSED) state = MAIN_MENU;
    }

    void Update() {
        frameCount++;
        if (state == BOOTING) { if (++bootLevel > 100) state = MAIN_MENU; return; }
        if (state != IN_GAME) return;

        static int moveTick = 0;
        int limit = (data.diffLevel == 0) ? 8 : (data.diffLevel == 1 ? 4 : 2);
        if (++moveTick < limit) return;
        moveTick = 0;

        Point head = snake[0];
        if (direction == UP) head.y--; else if (direction == DOWN) head.y++;
        else if (direction == LEFT) head.x--; else if (direction == RIGHT) head.x++;

        if (data.selectedLevel == 2) { // Portal Logic
            if (head.x <= 0) head.x = GAME_W - 1; else if (head.x >= GAME_W) head.x = 1;
            if (head.y <= 0) head.y = GAME_H - 1; else if (head.y >= GAME_H) head.y = 1;
        } else if (head.x <= 0 || head.x >= GAME_W || head.y <= 0 || head.y >= GAME_H) { state = CRITICAL_FAILURE; return; }
        
        for (auto w : walls) if (head.x == w.x && head.y == w.y) { state = CRITICAL_FAILURE; return; }
        for (size_t i = 1; i < snake.size(); i++) if (head.x == snake[i].x && head.y == snake[i].y) { state = CRITICAL_FAILURE; return; }

        snake.insert(snake.begin(), head);
        if (head.x == food.x && head.y == food.y) {
            score += 10; if (score > data.highScore) data.highScore = score;
            food = { rand() % (GAME_W - 2) + 1, rand() % (GAME_H - 2) + 1 };
            Beep(1200, 20);
        } else snake.pop_back();
    }

    void RenderAll() {
        rd.ResetBuffer();
        if (state == BOOTING) {
            rd.DrawUltimateSnakeTitle(40, 10);
            rd.DrawProgressBar(SCREEN_W / 2 - 13, 21, "CORE", bootLevel, C_CYAN);
            rd.Write(SCREEN_W / 2 - 15, 25, "NGUYEN LE DUY HAU - 25520518", C_DARK_GRAY);
        }
        else if (state == MAIN_MENU || state == MISSION_SELECT || state == SKIN_DATABASE || state == SETTINGS) {
            // HUD RADA Corners
            rd.Put(2, 1, 0x250F, C_CYAN); rd.Put(SCREEN_W - 4, 1, 0x2513, C_CYAN);
            rd.Put(2, SCREEN_H - 2, 0x2517, C_CYAN); rd.Put(SCREEN_W - 4, SCREEN_H - 2, 0x251B, C_CYAN);

            rd.DrawUltimateSnakeTitle(41, 4);
            string m[5]; int count = 0;
            if (state == MAIN_MENU) {
                m[0] = "START SIMULATION"; m[1] = "MISSION SELECT"; m[2] = "SKIN DATABASE"; m[3] = "CONFIG ENGINE"; m[4] = "SHUTDOWN"; count = 5;
            } else if (state == MISSION_SELECT) {
                m[0] = "CLASSIC AREA"; m[1] = "NEURAL WALLS"; m[2] = "PORTAL SECTOR"; count = 3;
            } else if (state == SKIN_DATABASE) {
                m[0] = "NEON GREEN"; m[1] = "RUBY CORE"; m[2] = "GOLD EDITION"; count = 3;
            } else if (state == SETTINGS) {
                m[0] = "DIFFICULTY: " + string(data.diffLevel == 0 ? "EASY" : data.diffLevel == 1 ? "NORM" : "HARD");
                m[1] = "SAVE & RETURN"; count = 2;
            }

            for (int i = 0; i < count; i++) {
                WORD col = (menuIdx == i) ? C_CYAN : C_DARK_GRAY;
                rd.DrawIndustrialBorder(42, 16 + i * 4, 30, 2, col, true);
                rd.Write(45, 17 + i * 4, m[i], (menuIdx == i ? C_WHITE : C_DARK_GRAY));
            }

            // TELEMETRY HUD (CPU/RAM BARS)
            rd.DrawIndustrialBorder(5, 16, 28, 16, C_GRAY, true, "TELEMETRY");
            rd.DrawProgressBar(7, 25, "CPU", 40 + (frameCount % 30), C_CYAN);
            rd.DrawProgressBar(7, 27, "RAM", 75 + (frameCount % 10), C_PINK);
            rd.DrawProgressBar(7, 29, "TMP", 60 + (frameCount % 15), C_YELLOW);

            // DATABASE HUD
            rd.DrawIndustrialBorder(85, 16, 30, 16, C_GRAY, true, "DATABASE");
            rd.Write(87, 18, "AUTHOR: " + data.owner, C_CYAN);
            rd.Write(87, 20, "ID_NUM: " + data.id, C_WHITE);
            rd.Write(87, 22, "RECORD: " + to_string(data.highScore) + " PTS", C_YELLOW);
            rd.Write(87, 24, "SKIN  : " + to_string(data.currentSkin), C_PINK);
        }
        else if (state == IN_GAME || state == PAUSED) {
            WORD skCol = (data.currentSkin == 0) ? C_GREEN : (data.currentSkin == 1 ? C_RED : C_YELLOW);
            rd.DrawIndustrialBorder(10, 10, GAME_W, GAME_H, C_CYAN, true, "MISSION_AREA");
            for (auto w : walls) rd.Put(w.x + 10, w.y + 10, 0x2593, C_DARK_GRAY);
            rd.Put(food.x + 10, food.y + 10, 0x2665, C_RED);
            for (size_t i = 0; i < snake.size(); i++)
                rd.Put(snake[i].x + 10, snake[i].y + 10, (i == 0 ? 0x25C8 : 0x2588), (i == 0 ? C_WHITE : skCol));
            
            rd.DrawIndustrialBorder(GAME_W + 15, 10, 22, 6, C_YELLOW, true, "STATS");
            rd.Write(GAME_W + 17, 12, "SCORE: " + to_string(score), C_WHITE);
            rd.Write(GAME_W + 17, 14, "MAP  : SECTOR-07", C_GRAY);
            if (state == PAUSED) rd.Write(SCREEN_W/2 - 5, SCREEN_H/2, "PAUSED", C_YELLOW);
        }
        else if (state == CRITICAL_FAILURE) {
            rd.DrawIndustrialBorder(SCREEN_W/2 - 20, 18, 40, 5, C_RED, true, "ERROR");
            rd.Write(SCREEN_W/2 - 13, 20, "CRITICAL COLLISION DETECTED!", C_WHITE);
        }
        rd.Flush();
    }
    void Run() { while (isActive) { PollInput(); Update(); RenderAll(); Sleep(15); } }
};

int main() {
    SetConsoleTitleA("SNAKE MASTER EDITION - v5.0");
    SnakeEliteEngine engine; engine.Run(); return 0;
}