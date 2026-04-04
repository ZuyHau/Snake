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
#define SCREEN_H 40
#define GAME_W 50
#define GAME_H 20

// Bảng màu Cyber-Core
#define C_CYAN 11
#define C_PINK 13
#define C_YELLOW 14
#define C_WHITE 15
#define C_DARK_GRAY 8
#define C_GREEN 10
#define C_RED 12

enum State { BOOTING, MAIN_MENU, IN_GAME, PAUSED, CRITICAL_FAILURE };
enum Dir { STOP = 0, LEFT, RIGHT, UP, DOWN };
struct Point { int x, y; };

// ============================================================================
// [MODULE 1] QUẢN LÝ DỮ LIỆU (HIGH SCORE)
// ============================================================================
class EngineData {
public:
    int highScore = 0;
    string owner = "DUY HAU";
    string id = "25520518";

    void Load() {
        ifstream f("system.log");
        if (f.is_open()) { f >> owner >> id >> highScore; f.close(); }
    }
    void Sync() {
        ofstream f("system.log");
        f << owner << " " << id << " " << highScore;
        f.close();
    }
};

// ============================================================================
// [MODULE 2] RENDERER XỊN (CHỐNG NHÁY MÀN HÌNH)
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
        if (!tag.empty()) Write(x + 2, y, "[ " + tag + " ]", col);
    }
    void Flush() {
        WriteConsoleOutputW(hOut, buffer, { (short)SCREEN_W, (short)SCREEN_H }, { 0, 0 }, &rect);
    }
};

// ============================================================================
// [MODULE 3] HỆ THỐNG GAME (NƠI RenderAll XUẤT HIỆN)
// ============================================================================
class SnakeEliteEngine {
private:
    MasterRenderer rd;
    EngineData data;
    State state = BOOTING;
    vector<Point> snake;
    Point food;
    Dir direction = RIGHT;
    int score = 0;
    int bootLevel = 0;
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
        food = { 30, 10 };
        score = 0; direction = RIGHT;
    }

    void Update() {
        frameCount++;
        if (state == BOOTING) { if (++bootLevel > 100) state = IN_GAME; return; }
        if (state != IN_GAME) return;

        // Input vặn ốc
        if (kbhit()) {
            char t = getch();
            if (t == 'a' && direction != RIGHT) direction = LEFT;
            if (t == 'd' && direction != LEFT)  direction = RIGHT;
            if (t == 'w' && direction != DOWN)  direction = UP;
            if (t == 's' && direction != UP)    direction = DOWN;
            if (t == 27) state = PAUSED;
        }

        // Tốc độ Overclock
        static int moveTick = 0;
        int limit = max(1, 5 - (score / 50)); 
        if (++moveTick < limit) return;
        moveTick = 0;

        // Logic di chuyển
        Point head = snake[0];
        if (direction == UP) head.y--; else if (direction == DOWN) head.y++;
        else if (direction == LEFT) head.x--; else if (direction == RIGHT) head.x++;

        // Va chạm
        if (head.x <= 0 || head.x >= GAME_W || head.y <= 0 || head.y >= GAME_H) { state = CRITICAL_FAILURE; return; }
        
        snake.insert(snake.begin(), head);
        if (head.x == food.x && head.y == food.y) {
            score += 10;
            if (score > data.highScore) { data.highScore = score; data.Sync(); }
            food = { rand() % (GAME_W - 2) + 1, rand() % (GAME_H - 2) + 1 };
            Beep(1000, 20);
        } else snake.pop_back();
    }

    // ĐÂY NÈ HẬU! Hàm RenderAll gom tất cả việc vẽ vào một chỗ
    void RenderAll() {
        rd.ResetBuffer();

        if (state == BOOTING) {
            rd.Write(SCREEN_W/2 - 10, 10, "SYSTEM BOOTING...", C_CYAN);
            rd.DrawBorder(SCREEN_W/2 - 15, 12, 30, 2, C_DARK_GRAY);
            for(int i=0; i<bootLevel*30/100; i++) rd.Put(SCREEN_W/2 - 14 + i, 13, 0x2588, C_CYAN);
        }
        else if (state == IN_GAME) {
            rd.DrawBorder(2, 2, GAME_W, GAME_H, C_CYAN, "MISSION_AREA");
            rd.Put(food.x + 2, food.y + 2, 0x2665, C_RED); // Mồi ♥
            for (size_t i = 0; i < snake.size(); i++)
                rd.Put(snake[i].x + 2, snake[i].y + 2, (i == 0 ? 'O' : 'x'), (i == 0 ? C_WHITE : C_GREEN));

            rd.DrawBorder(GAME_W + 5, 2, 25, 6, C_YELLOW, "STATS");
            rd.Write(GAME_W + 7, 4, "SCORE: " + to_string(score), C_WHITE);
            rd.Write(GAME_W + 7, 6, "HIGH : " + to_string(data.highScore), C_PINK);
        }
        else if (state == CRITICAL_FAILURE) {
            rd.Write(SCREEN_W/2 - 10, SCREEN_H/2, "GAME OVER! PRESS R", C_RED);
        }

        rd.Flush(); // Đẩy toàn bộ buffer ra màn hình 1 lần duy nhất
    }

    void Run() {
        while (isActive) {
            Update();
            RenderAll();
            Sleep(20);
        }
    }
};

int main() {
    SnakeEliteEngine engine;
    engine.Run();
    return 0;
}