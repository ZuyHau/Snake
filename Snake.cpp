/******************************************************************************
 * PROJECT: SNAKE MASTER EDITION - CYBER CORE ULTIMATE (ANTI-MOJIBAKE)
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

// --- CONFIG HỆ THỐNG ---
#define SCREEN_W 120
#define SCREEN_H 42
#define GAME_W 60
#define GAME_H 22

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

enum State { BOOTING, MAIN_MENU, IN_GAME, PAUSED, CRITICAL_FAILURE };
enum Dir { STOP = 0, LEFT, RIGHT, UP, DOWN };

struct Point { int x, y; };

// ============================================================================
// [MODULE 1] DATA PERSISTENCE (LƯU KỶ LỤC)
// ============================================================================
class EngineData {
public:
    int highScore = 0;
    string owner = "DUY HAU";
    string id = "25520518";

    void Load() {
        ifstream f("system.log");
        if (f.is_open()) {
            f >> owner >> id >> highScore;
            f.close();
        }
    }

    void Sync() {
        ofstream f("system.log");
        f << owner << " " << id << " " << highScore;
        f.close();
    }
};

// ============================================================================
// [MODULE 2] CORE RENDERER (WIN32 BUFFER - CHỐNG NHÁY)
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

    void DrawBorder(int x, int y, int w, int h, WORD col, string tag = "") {
        for (int i = x; i <= x + w; i++) { Put(i, y, 0x2550, col); Put(i, y + h, 0x2550, col); }
        for (int i = y; i <= y + h; i++) { Put(x, i, 0x2551, col); Put(x + w, i, 0x2551, col); }
        Put(x, y, 0x2554, col); Put(x + w, y, 0x2557, col);
        Put(x, y + h, 0x255A, col); Put(x + w, y + h, 0x255D, col);

        if (!tag.empty()) Write(x + 2, y, "[ " + tag + " ]", col);
    }

    void Flush() {
        WriteConsoleOutputW(hOut, buffer, { (short)SCREEN_W, (short)SCREEN_H }, { 0, 0 }, &rect);
    }
};

// ============================================================================
// [MODULE 3] GAME ENGINE (XỬ LÝ LOGIC)
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
        data.Load();
        InitSnake();
    }

    void InitSnake() {
        snake.clear();
        for (int i = 0; i < 5; i++) snake.push_back({ 20 - i, 10 });
        food = { rand() % (GAME_W - 2) + 1, rand() % (GAME_H - 2) + 1 };
        score = 0; direction = RIGHT;
    }

    void TriggerAction() {
        if (state == BOOTING) state = MAIN_MENU;
        else if (state == MAIN_MENU) {
            if (menuIdx == 0) { InitSnake(); state = IN_GAME; }
            else if (menuIdx == 1) isActive = false;
        } 
        else if (state == PAUSED) state = IN_GAME;
        else if (state == CRITICAL_FAILURE) state = MAIN_MENU;
    }

    void Update() {
        frameCount++;
        if (state == BOOTING) { 
            if (++bootLevel > 100) state = MAIN_MENU; 
            return; 
        }

        // Xử lý Input
        if (kbhit()) {
            char t = getch();
            if (state == IN_GAME) {
                if ((t == 'w' || t == 'W') && direction != DOWN) direction = UP;
                if ((t == 's' || t == 'S') && direction != UP) direction = DOWN;
                if ((t == 'a' || t == 'A') && direction != RIGHT) direction = LEFT;
                if ((t == 'd' || t == 'D') && direction != LEFT) direction = RIGHT;
                if (t == 27) state = PAUSED;
            } else {
                if (t == 'w' || t == 'W' || t == 72) { menuIdx = 0; Beep(700, 20); }
                if (t == 's' || t == 'S' || t == 80) { menuIdx = 1; Beep(700, 20); }
                if (t == 13) TriggerAction();
            }
        }

        if (state != IN_GAME) return;

        // Logic tốc độ
        static int moveTick = 0;
        int limit = max(1, 6 - (score / 100)); 
        if (++moveTick < limit) return;
        moveTick = 0;

        Point head = snake[0];
        if (direction == UP) head.y--; else if (direction == DOWN) head.y++;
        else if (direction == LEFT) head.x--; else if (direction == RIGHT) head.x++;

        // Va chạm biên & thân
        if (head.x <= 0 || head.x >= GAME_W || head.y <= 0 || head.y >= GAME_H) { state = CRITICAL_FAILURE; Beep(300, 100); return; }
        for (size_t i = 1; i < snake.size(); i++) if (head.x == snake[i].x && head.y == snake[i].y) { state = CRITICAL_FAILURE; return; }

        snake.insert(snake.begin(), head);
        if (head.x == food.x && head.y == food.y) {
            score += 10;
            if (score > data.highScore) { data.highScore = score; data.Sync(); }
            food = { rand() % (GAME_W - 2) + 1, rand() % (GAME_H - 2) + 1 };
            Beep(1200, 20);
        } else snake.pop_back();
    }

    void RenderAll() {
        rd.ResetBuffer();

        if (state == BOOTING) {
            rd.Write(SCREEN_W/2 - 12, 18, ">>> INITIALIZING CORE...", C_CYAN);
            rd.DrawBorder(SCREEN_W/2 - 20, 20, 40, 2, C_DARK_GRAY);
            for (int i = 0; i < bootLevel * 38 / 100; i++) rd.Put(SCREEN_W/2 - 19 + i, 21, 0x2588, C_CYAN);
        }
        else if (state == MAIN_MENU) {
            rd.Write(SCREEN_W/2 - 15, 8, "=== SNAKE MASTER EDITION ===", C_PINK);
            rd.Write(SCREEN_W/2 - 10, 10, "STATION: " + data.owner, C_GRAY);

            string m[] = { "START MISSION", "SHUTDOWN SYSTEM" };
            for (int i = 0; i < 2; i++) {
                WORD col = (menuIdx == i) ? C_CYAN : C_DARK_GRAY;
                string cursor = (menuIdx == i) ? ">> " : "   ";
                rd.Write(SCREEN_W/2 - 8, 18 + i * 2, cursor + m[i], col);
            }
            rd.Write(SCREEN_W/2 - 15, 30, "PRESS [W/S] TO NAVIGATE - [ENTER]", C_DARK_GRAY);
        }
        else if (state == IN_GAME || state == PAUSED) {
            rd.DrawBorder(5, 5, GAME_W, GAME_H, C_CYAN, "SECTOR_07");
            rd.Put(food.x + 5, food.y + 5, 0x2665, C_RED);
            for (size_t i = 0; i < snake.size(); i++)
                rd.Put(snake[i].x + 5, snake[i].y + 5, (i == 0 ? 0x25C8 : 0x2588), (i == 0 ? C_WHITE : C_GREEN));

            rd.DrawBorder(GAME_W + 10, 5, 25, 8, C_YELLOW, "SYS_TELEMETRY");
            rd.Write(GAME_W + 12, 7, "SCORE: " + to_string(score), C_WHITE);
            rd.Write(GAME_W + 12, 9, "HIGH : " + to_string(data.highScore), C_PINK);
            rd.Write(GAME_W + 12, 11, "USER : " + data.id, C_DARK_GRAY);

            if (state == PAUSED) {
                rd.DrawBorder(SCREEN_W/2 - 10, SCREEN_H/2 - 2, 20, 4, C_YELLOW, "PAUSED");
                rd.Write(SCREEN_W/2 - 8, SCREEN_H/2, "RESUME? [ENTER]", C_WHITE);
            }
        }
        else if (state == CRITICAL_FAILURE) {
            rd.DrawBorder(SCREEN_W/2 - 15, SCREEN_H/2 - 3, 30, 6, C_RED, "FATAL_ERROR");
            rd.Write(SCREEN_W/2 - 11, SCREEN_H/2, "CORE COLLISION!", C_WHITE);
            rd.Write(SCREEN_W/2 - 12, SCREEN_H/2 + 2, "ENTER TO REBOOT", C_DARK_GRAY);
        }

        rd.Flush();
    }

    void Run() {
        while (isActive) { Update(); RenderAll(); Sleep(15); }
    }
};

int main() {
    SetConsoleTitleA("SNAKE MASTER EDITION - CYBER CORE");
    SnakeEliteEngine engine;
    engine.Run();
    return 0;
}