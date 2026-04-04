/******************************************************************************
 * PROJECT: SNAKE MASTER EDITION - CYBER CORE ULTIMATE (ANTI-MOJIBAKE FIX)
 * AUTHOR: NGUYEN LE DUY HAU - MSSV: 25520518
 * FACULTY: COMPUTER ENGINEERING - UIT (VNU)
 ******************************************************************************/

#include <iostream>
#include <vector>
#include <string>
#include <windows.h>
#include <conio.h>
#include <ctime>
#include <fstream>

using namespace std;

// --- SYSTEM DEFINITIONS ---
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

enum State { BOOTING, MAIN_MENU, SETTINGS, SKIN_DATABASE, MISSION_LOAD, IN_GAME, PAUSED, CRITICAL_FAILURE, EXIT_SYS };
enum Dir { STOP = 0, LEFT, RIGHT, UP, DOWN };

struct Point { int x, y; };

// ============================================================================
// [MODULE 1] DATA PERSISTENCE
// ============================================================================
class EngineData {
public:
    int highScore = 1540;
    int currentSkin = 0;
    int diffLevel = 1;
    string owner = "DUY HAU";
    string id = "25520518";

    void Sync() {
        ofstream f("system.log");
        f << owner << " " << id << " " << highScore;
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

    // Vẽ bóng đổ bằng mã Hex Unicode để tránh lỗi font
    void DrawShadow(int x, int y, int w, int h) {
        for (int i = x; i < x + w; i++) {
            Put(i, y + h, 0x2593, C_DARK_GRAY); // 0x2593 = ▓
        }
        for (int i = y; i <= y + h; i++) {
            Put(x + w, i, 0x2593, C_DARK_GRAY); // 0x2593 = ▓
            Put(x + w + 1, i, 0x2592, C_DARK_GRAY); // 0x2592 = ▒
        }
    }

    void DrawIndustrialBorder(int x, int y, int w, int h, WORD col, bool shadow = false, string tag = "") {
        if (shadow) DrawShadow(x + 1, y + 1, w, h);

        for (int i = x; i <= x + w; i++) { Put(i, y, 0x2550, col); Put(i, y + h, 0x2550, col); } // 0x2550 = ═
        for (int i = y; i <= y + h; i++) { Put(x, i, 0x2551, col); Put(x + w, i, 0x2551, col); } // 0x2551 = ║
        Put(x, y, 0x2554, col); Put(x + w, y, 0x2557, col); // ╔ , ╗
        Put(x, y + h, 0x255A, col); Put(x + w, y + h, 0x255D, col); // ╚ , ╝

        if (!tag.empty()) Write(x + 2, y, "[ " + tag + " ]", col);
    }

    // Font khối hoàn toàn bằng ký tự ASCII chữ X, sau đó thay bằng mã màu Hex để an toàn
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
            for (size_t j = 0; j < line[i].length(); j++) {
                if (line[i][j] == 'X') Put(x + (int)j, y + i, 0x2588, col); // 0x2588 = █
            }
        }
    }

    void DrawProgressBar(int x, int y, string label, int percent, WORD col) {
        Write(x, y, label, C_GRAY);
        Write(x + 6, y, "[", C_DARK_GRAY);
        int bars = percent / 10;
        for (int i = 0; i < 10; i++) {
            if (i < bars) Put(x + 7 + i, y, 0x2588, col); // 0x2588 = █
            else Put(x + 7 + i, y, 0x2591, C_DARK_GRAY);  // 0x2591 = ░
        }
        Write(x + 17, y, "]", C_DARK_GRAY);
        Write(x + 19, y, to_string(percent) + "%", col);
    }

    void Flush() {
        WriteConsoleOutputW(hOut, buffer, { (short)SCREEN_W, (short)SCREEN_H }, { 0, 0 }, &rect);
    }
};

// ============================================================================
// [MODULE 3] GAME ENGINE ARCHITECTURE
// ============================================================================
class SnakeEliteEngine {
private:
    MasterRenderer rd;
    EngineData data;
    State state = BOOTING;
    vector<Point> snake;
    Point food;
    Dir direction = RIGHT;

    int menuIdx = 0;
    int bootLevel = 0;
    int score = 0;
    long long frameCount = 0;
    bool isActive = true;

public:
    SnakeEliteEngine() {
        srand(time(0));
        InitSnake();
    }

    void InitSnake() {
        snake.clear();
        for (int i = 0; i < 5; i++) snake.push_back({ 20 - i, 12 });
        food = { 30, 15 };
        score = 0; direction = RIGHT;
    }

    void PollInput() {
        if (state == IN_GAME) {
            if ((GetAsyncKeyState('W') & 0x8000) && direction != DOWN) direction = UP;
            if ((GetAsyncKeyState('S') & 0x8000) && direction != UP) direction = DOWN;
            if ((GetAsyncKeyState('A') & 0x8000) && direction != RIGHT) direction = LEFT;
            if ((GetAsyncKeyState('D') & 0x8000) && direction != LEFT) direction = RIGHT;
            if (GetAsyncKeyState(VK_ESCAPE) & 0x8000) state = PAUSED;
        } else {
            int maxMenu = (state == MAIN_MENU) ? 4 : 1;
            if (GetAsyncKeyState(VK_UP) & 0x8000) {
                menuIdx = (menuIdx <= 0) ? maxMenu : menuIdx - 1;
                Beep(700, 20); Sleep(150);
            }
            if (GetAsyncKeyState(VK_DOWN) & 0x8000) {
                menuIdx = (menuIdx >= maxMenu) ? 0 : menuIdx + 1;
                Beep(700, 20); Sleep(150);
            }
            if (GetAsyncKeyState(VK_RETURN) & 0x8000) {
                TriggerAction();
                Beep(900, 50); Sleep(200);
            }
        }
    }

    void TriggerAction() {
        if (state == BOOTING) state = MAIN_MENU;
        else if (state == MAIN_MENU) {
            if (menuIdx == 0) { InitSnake(); state = IN_GAME; }
            else if (menuIdx == 1) { state = SETTINGS; menuIdx = 0; }
            else if (menuIdx == 4) isActive = false;
        } else if (state == SETTINGS) {
            if (menuIdx == 0) data.diffLevel = (data.diffLevel + 1) % 3;
            else state = MAIN_MENU;
        } else if (state == CRITICAL_FAILURE || state == PAUSED) {
            state = MAIN_MENU; menuIdx = 0;
        }
    }

    void Update() {
        frameCount++;
        if (state == BOOTING) { if (++bootLevel > 100) state = MAIN_MENU; return; }
        if (state != IN_GAME) return;

        static int moveTick = 0;
        int limit = (data.diffLevel == 0) ? 8 : (data.diffLevel == 1 ? 4 : 1);
        if (++moveTick < limit) return;
        moveTick = 0;

        Point head = snake[0];
        if (direction == UP) head.y--; else if (direction == DOWN) head.y++;
        else if (direction == LEFT) head.x--; else if (direction == RIGHT) head.x++;

        if (head.x <= 0 || head.x >= GAME_W || head.y <= 0 || head.y >= GAME_H) { state = CRITICAL_FAILURE; return; }
        for (size_t i = 1; i < snake.size(); i++) if (head.x == snake[i].x && head.y == snake[i].y) { state = CRITICAL_FAILURE; return; }

        snake.insert(snake.begin(), head);
        if (head.x == food.x && head.y == food.y) {
            score += 10;
            food = { rand() % (GAME_W - 2) + 1, rand() % (GAME_H - 2) + 1 };
            Beep(1200, 20);
        } else snake.pop_back();
    }

    void RenderAll() {
        rd.ResetBuffer();

        if (state == BOOTING) {
            rd.DrawUltimateSnakeTitle(40, 10);
            rd.Write(SCREEN_W / 2 - 12, 19, ">>> SYSTEM BOOTING...", C_DARK_GRAY);
            rd.DrawIndustrialBorder(SCREEN_W / 2 - 26, 21, 52, 2, C_DARK_GRAY);
            for (int i = 0; i < bootLevel / 2; i++) rd.Put(SCREEN_W / 2 - 25 + i, 22, 0x2588, C_CYAN);
            rd.Write(SCREEN_W / 2 + 28, 22, to_string(bootLevel) + "%", C_CYAN);
        }
        else if (state == MAIN_MENU || state == SETTINGS) {
            if (frameCount % 10 == 0) {
                for (int i = 0; i < 15; i++) rd.Put(rand() % SCREEN_W, rand() % SCREEN_H, L'.', C_DARK_GRAY);
            }

            // HUD Camera Corners (Thay bằng mã Hex an toàn)
            rd.Put(2, 1, 0x250F, C_CYAN); rd.Put(3, 1, 0x2501, C_CYAN); rd.Put(2, 2, 0x2503, C_CYAN);
            rd.Put(SCREEN_W - 4, 1, 0x2513, C_CYAN); rd.Put(SCREEN_W - 5, 1, 0x2501, C_CYAN); rd.Put(SCREEN_W - 4, 2, 0x2503, C_CYAN);
            rd.Put(2, SCREEN_H - 2, 0x2517, C_CYAN); rd.Put(3, SCREEN_H - 2, 0x2501, C_CYAN); rd.Put(2, SCREEN_H - 3, 0x2503, C_CYAN);
            rd.Put(SCREEN_W - 4, SCREEN_H - 2, 0x251B, C_CYAN); rd.Put(SCREEN_W - 5, SCREEN_H - 2, 0x2501, C_CYAN); rd.Put(SCREEN_W - 4, SCREEN_H - 3, 0x2503, C_CYAN);

            rd.Write(5, 1, " X:120 Y:042 ", C_DARK_GRAY);
            rd.Write(SCREEN_W - 22, 1, " TARGET_LOCK: OFF ", C_DARK_GRAY);

            rd.DrawUltimateSnakeTitle(41, 4);
            rd.Write(48, 11, "--- MASTER EDITION ---", C_GRAY); // Text chuẩn ASCII

            string m[] = { "START SIMULATION", "CONFIG ENGINE", "SKIN DATABASE", "LEADERBOARD", "SHUTDOWN SYSTEM" };
            int count = (state == MAIN_MENU) ? 5 : 2;
            if (state == SETTINGS) {
                m[0] = "DIFFICULTY: " + string(data.diffLevel == 0 ? "EASY" : data.diffLevel == 1 ? "NORM" : "HARD");
                m[1] = "RETURN TO TERMINAL";
            }

            int menuStartX = 42;
            int menuStartY = 16;

            for (int i = 0; i < count; i++) {
                int y = menuStartY + i * 4;
                int len = m[i].length();

                if (menuIdx == i) {
                    rd.DrawIndustrialBorder(menuStartX - 4, y - 1, len + 8, 2, C_CYAN, true);
                    if (frameCount % 20 < 10) {
                        rd.Put(menuStartX - 2, y, 0x25BA, C_PINK); // 0x25BA = ►
                        rd.Put(menuStartX + len + 1, y, 0x25C4, C_PINK); // 0x25C4 = ◄
                    }
                    rd.Write(menuStartX, y, m[i], C_WHITE);
                } else {
                    rd.DrawIndustrialBorder(menuStartX - 2, y - 1, len + 4, 2, C_DARK_GRAY, false);
                    rd.Write(menuStartX, y, m[i], C_DARK_GRAY);
                }
            }

            rd.DrawIndustrialBorder(5, 16, 28, 16, C_GRAY, true, "SYS_TELEMETRY");
            rd.Write(7, 18, "CORE_STATUS  : [ACTIVE]", C_GREEN);
            rd.Write(7, 20, "SECURE_CONN  : [VERIFIED]", C_CYAN);
            rd.Write(7, 22, "UI_RENDERER  : [DX_12_ON]", C_YELLOW);

            rd.DrawProgressBar(7, 25, "CPU ", 40 + (frameCount % 30), C_CYAN);
            rd.DrawProgressBar(7, 27, "RAM ", 75 + (frameCount % 10), C_PINK);
            rd.DrawProgressBar(7, 29, "TEMP", 60 + (frameCount % 15), C_YELLOW);

            rd.DrawIndustrialBorder(85, 16, 30, 16, C_GRAY, true, "USER_DATABASE");
            rd.Write(87, 18, "AUTHOR:", C_DARK_GRAY);  rd.Write(95, 18, data.owner + " - UIT", C_CYAN);
            rd.Write(87, 20, "ID_NUM:", C_DARK_GRAY);  rd.Write(95, 20, data.id, C_WHITE);
            rd.Write(87, 22, "RECORD:", C_DARK_GRAY);  rd.Write(95, 22, to_string(data.highScore) + " PTS", C_YELLOW);
            rd.Write(87, 24, "MAP   :", C_DARK_GRAY);  rd.Write(95, 24, "SECTOR-7", C_GRAY);
            rd.Write(87, 26, "SKIN  :", C_DARK_GRAY);  rd.Write(95, 26, "CYBER-NEON", C_PINK);
            rd.Write(87, 28, "PING  :", C_DARK_GRAY);  rd.Write(95, 28, to_string(12 + (frameCount % 5)) + " ms", C_GREEN);

            rd.Write(SCREEN_W / 2 - 30, SCREEN_H - 3, ">>> USE [W,A,S,D] OR [ARROWS] TO NAVIGATE - [ENTER] TO SELECT <<<", C_DARK_GRAY);
        }
        else if (state == IN_GAME) {
            rd.DrawIndustrialBorder(10, 10, GAME_W, GAME_H, C_CYAN, true, "MISSION_AREA");
            rd.Put(food.x + 10, food.y + 10, 0x2665, C_RED); // 0x2665 = ♥
            for (size_t i = 0; i < snake.size(); i++)
                rd.Put(snake[i].x + 10, snake[i].y + 10, (i == 0 ? 0x25C8 : 0x2588), (i == 0 ? C_WHITE : C_GREEN)); // ◈ or █

            rd.DrawIndustrialBorder(GAME_W + 15, 10, 20, 4, C_YELLOW, true, "LIVE_STATS");
            rd.Write(GAME_W + 17, 12, "SCORE: " + to_string(score), C_WHITE);
        }
        else if (state == CRITICAL_FAILURE) {
            rd.DrawIndustrialBorder(SCREEN_W / 2 - 20, 18, 40, 7, C_RED, true, "FATAL_ERROR");
            rd.Write(SCREEN_W / 2 - 12, 20, "CRITICAL COLLISION DETECTED!", C_WHITE);
            rd.Write(SCREEN_W / 2 - 10, 22, ">>> PRESS ENTER TO REBOOT", C_DARK_GRAY);
        }

        rd.Flush();
    }

    void Run() {
        while (isActive) { PollInput(); Update(); RenderAll(); Sleep(15); }
    }
};

int main() {
    SetConsoleTitleA("SNAKE MASTER EDITION - CYBER CORE ULTIMATE");
    SnakeEliteEngine engine;
    engine.Run();
    return 0;
}
