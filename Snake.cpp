/******************************************************************************
 * PROJECT: SNAKE MASTER EDITION - MULTIVERSE MISSIONS (v2.5)
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

#define SCREEN_W 120
#define SCREEN_H 42
#define GAME_W 60
#define GAME_H 22

#define C_DARK_GRAY 8
#define C_GRAY 7
#define C_GREEN 10
#define C_CYAN 11
#define C_RED 12
#define C_PINK 13
#define C_YELLOW 14
#define C_WHITE 15

enum State { BOOTING, MAIN_MENU, MISSION_SELECT, IN_GAME, SETTINGS, PAUSED, CRITICAL_FAILURE };
enum Dir { STOP = 0, LEFT, RIGHT, UP, DOWN };
struct Point { int x, y; };

// ============================================================================
// [MODULE 1] DATA PERSISTENCE
// ============================================================================
class EngineData {
public:
    int highScore = 0;
    int currentSkin = 0;
    int selectedLevel = 0; // 0: Classic, 1: Neural Walls, 2: Portal
    string owner = "DUY HAU";
    string id = "25520518";

    void Load() {
        ifstream f("system.log");
        if (f.is_open()) {
            f >> owner >> id >> highScore >> currentSkin >> selectedLevel;
            f.close();
        }
    }

    void Sync() {
        ofstream f("system.log");
        f << owner << " " << id << " " << highScore << " " << currentSkin << " " << selectedLevel;
        f.close();
    }
};

// ============================================================================
// [MODULE 2] CORE RENDERER
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
    void Flush() { WriteConsoleOutputW(hOut, buffer, { (short)SCREEN_W, (short)SCREEN_H }, { 0, 0 }, &rect); }
};

// ============================================================================
// [MODULE 3] GAME ENGINE
// ============================================================================
class SnakeEliteEngine {
private:
    MasterRenderer rd;
    EngineData data;
    State state = BOOTING;
    vector<Point> snake;
    vector<Point> walls;
    Point food;
    Dir direction = RIGHT;
    int menuIdx = 0;
    int bootLevel = 0;
    int score = 0;
    bool isActive = true;

public:
    SnakeEliteEngine() {
        srand(time(0));
        data.Load();
        InitSnake();
    }

    void InitSnake() {
        snake.clear(); walls.clear();
        for (int i = 0; i < 5; i++) snake.push_back({ 20 - i, 10 });
        
        // Vặn ốc: Tạo map dựa trên data.selectedLevel
        if (data.selectedLevel == 1) { // Neural Walls
            for(int i=6; i<16; i++) { walls.push_back({25, i}); walls.push_back({45, i}); }
        }

        food = { 35, 10 };
        score = 0; direction = RIGHT;
    }

    void TriggerAction() {
        if (state == BOOTING) state = MAIN_MENU;
        else if (state == MAIN_MENU) {
            if (menuIdx == 0) { state = MISSION_SELECT; menuIdx = 0; }
            else if (menuIdx == 1) { state = SETTINGS; menuIdx = 0; }
            else if (menuIdx == 2) isActive = false;
        }
        else if (state == MISSION_SELECT) {
            data.selectedLevel = menuIdx;
            InitSnake();
            state = IN_GAME;
        }
        else if (state == SETTINGS) {
            if (menuIdx == 0) data.currentSkin = (data.currentSkin + 1) % 3;
            else { data.Sync(); state = MAIN_MENU; menuIdx = 0; }
        }
        else if (state == PAUSED || state == CRITICAL_FAILURE) state = MAIN_MENU;
    }

    void Update() {
        if (state == BOOTING) { if (++bootLevel > 100) state = MAIN_MENU; return; }

        if (kbhit()) {
            char t = getch();
            if (state == IN_GAME) {
                if ((t == 'w' || t == 'W') && direction != DOWN) direction = UP;
                if ((t == 's' || t == 'S') && direction != UP) direction = DOWN;
                if ((t == 'a' || t == 'A') && direction != RIGHT) direction = LEFT;
                if ((t == 'd' || t == 'D') && direction != LEFT) direction = RIGHT;
                if (t == 27) state = PAUSED;
            } else {
                int maxIdx = (state == MAIN_MENU) ? 2 : (state == MISSION_SELECT ? 2 : 1);
                if (t == 72 || t == 'w') menuIdx = (menuIdx <= 0) ? maxIdx : menuIdx - 1;
                if (t == 80 || t == 's') menuIdx = (menuIdx >= maxIdx) ? 0 : menuIdx + 1;
                if (t == 13) TriggerAction();
            }
        }

        if (state != IN_GAME) return;

        static int moveTick = 0;
        if (++moveTick < max(1, 6 - (score / 100))) return;
        moveTick = 0;

        Point head = snake[0];
        if (direction == UP) head.y--; else if (direction == DOWN) head.y++;
        else if (direction == LEFT) head.x--; else if (direction == RIGHT) head.x++;

        // Vặn ốc: Logic PORTAL (Xuyên không)
        if (data.selectedLevel == 2) {
            if (head.x <= 0) head.x = GAME_W - 1;
            else if (head.x >= GAME_W) head.x = 1;
            if (head.y <= 0) head.y = GAME_H - 1;
            else if (head.y >= GAME_H) head.y = 1;
        } else {
            if (head.x <= 0 || head.x >= GAME_W || head.y <= 0 || head.y >= GAME_H) { state = CRITICAL_FAILURE; return; }
        }

        for (auto w : walls) if (head.x == w.x && head.y == w.y) { state = CRITICAL_FAILURE; return; }
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
            rd.Write(SCREEN_W/2 - 12, 20, "LOADING MULTIVERSE CORE...", C_CYAN);
            for (int i = 0; i < bootLevel * 38 / 100; i++) rd.Put(SCREEN_W/2 - 19 + i, 22, 0x2588, C_CYAN);
        }
        else if (state == MAIN_MENU || state == MISSION_SELECT || state == SETTINGS) {
            rd.Write(SCREEN_W/2 - 15, 8, "=== SNAKE MASTER EDITION ===", C_PINK);
            string m[3]; int count = 0;
            if (state == MAIN_MENU) { m[0] = "SELECT MISSION"; m[1] = "SKIN DATABASE"; m[2] = "SHUTDOWN"; count = 3; }
            else if (state == MISSION_SELECT) { m[0] = "CLASSIC AREA"; m[1] = "NEURAL WALLS"; m[2] = "PORTAL SECTOR"; count = 3; }
            else { m[0] = "CHANGE SKIN"; m[1] = "BACK"; count = 2; }

            for (int i = 0; i < count; i++) {
                WORD col = (menuIdx == i) ? C_CYAN : C_DARK_GRAY;
                rd.Write(SCREEN_W/2 - 8, 18 + i * 2, (menuIdx == i ? ">> " : "   ") + m[i], col);
            }
        }
        else if (state == IN_GAME || state == PAUSED) {
            WORD snakeCol = (data.currentSkin == 0) ? C_GREEN : (data.currentSkin == 1 ? C_PINK : C_YELLOW);
            string tags[] = {"CLASSIC", "NEURAL_WALLS", "PORTAL_SECTOR"};
            rd.DrawBorder(5, 5, GAME_W, GAME_H, C_CYAN, tags[data.selectedLevel]);
            for (auto w : walls) rd.Put(w.x + 5, w.y + 5, 0x2593, C_DARK_GRAY);
            rd.Put(food.x + 5, food.y + 5, 0x2665, C_RED);
            for (size_t i = 0; i < snake.size(); i++)
                rd.Put(snake[i].x + 5, snake[i].y + 5, (i == 0 ? 0x25C8 : 0x2588), (i == 0 ? C_WHITE : snakeCol));
            rd.Write(GAME_W + 12, 7, "SCORE: " + to_string(score), C_WHITE);
            rd.Write(GAME_W + 12, 9, "HIGH : " + to_string(data.highScore), C_PINK);
        }
        else if (state == CRITICAL_FAILURE) rd.Write(SCREEN_W/2 - 10, SCREEN_H/2, "CORE COLLAPSED! [ENTER]", C_RED);
        rd.Flush();
    }
    void Run() { while (isActive) { Update(); RenderAll(); Sleep(15); } }
};

int main() {
    SetConsoleTitleA("SNAKE MASTER EDITION - MULTIVERSE");
    SnakeEliteEngine engine;
    engine.Run();
    return 0;
}