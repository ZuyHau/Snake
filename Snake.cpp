#include <iostream>
#include <windows.h>
#include <cstdlib>
#include <conio.h>
#include <ctime>
#include <cctype>
using namespace std;

// --- SYSTEM DEFINITIONS ---
#define SCREEN_W 120
#define SCREEN_H 42

// Cyber-Core Color Palette
#define C_DARK_GRAY 8
#define C_GREEN 10
#define C_CYAN 11
#define C_RED 12
#define C_PINK 13
#define C_YELLOW 14
#define C_WHITE 15
#define C_DARK_PURPLE 5
#define C_GRAY 7

// Thêm trạng thái IN_GAME để chạy logic của file Snake.cpp
enum State { BOOTING, MAIN_MENU, SETTINGS, EDIT_PROFILE, IN_GAME };

// ============================================================================
// DỮ LIỆU HIỂN THỊ TRÊN MENU 
// ============================================================================
class EngineData {
public:
    int highScore = 1540;
    int diffLevel = 1;
    string owner = "DUY HAU";
    string id = "25520518";
};

// ============================================================================
// BỘ CÔNG CỤ VẼ GIAO DIỆN 
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
        for (int i = x; i < x + w; i++) Put(i, y + h, 0x2593, C_DARK_GRAY);
        for (int i = y; i <= y + h; i++) { Put(x + w, i, 0x2593, C_DARK_GRAY); Put(x + w + 1, i, 0x2592, C_DARK_GRAY); }
    }

    void DrawIndustrialBorder(int x, int y, int w, int h, WORD col, bool shadow = false, string tag = "") {
        if (shadow) DrawShadow(x + 1, y + 1, w, h);
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
            for (size_t j = 0; j < line[i].length(); j++) {
                if (line[i][j] == 'X') Put(x + (int)j, y + i, 0x2588, col);
            }
        }
    }

    void DrawProgressBar(int x, int y, string label, int percent, WORD col) {
        Write(x, y, label, C_GRAY);
        Write(x + 6, y, "[", C_DARK_GRAY);
        int bars = percent / 10;
        for (int i = 0; i < 10; i++) {
            if (i < bars) Put(x + 7 + i, y, 0x2588, col);
            else Put(x + 7 + i, y, 0x2591, C_DARK_GRAY);
        }
        Write(x + 17, y, "]", C_DARK_GRAY);
        Write(x + 19, y, to_string(percent) + "%", col);
    }

    void Flush() {
        WriteConsoleOutputW(hOut, buffer, { (short)SCREEN_W, (short)SCREEN_H }, { 0, 0 }, &rect);
    }
};

// ============================================================================
// LÕI GAME 
// ============================================================================
struct Point {
    int x, y;
};

class CONRAN {
public:
    struct Point A[100];
    int DoDai;
    
    CONRAN() {
        DoDai = 3;
        A[0].x = 10; A[0].y = 10;
        A[1].x = 11; A[1].y = 10;
        A[2].x = 12; A[2].y = 10;
    }
    
    // Đổi gotoxy thành rd.Put để vẽ vào giao diện mới
    void Ve(MasterRenderer& rd, int offsetX, int offsetY) {
        for (int i = 0; i < DoDai; i++) {
            rd.Put(A[i].x + offsetX, A[i].y + offsetY, 'X', C_GREEN); // In chữ X như code cũ
        }
    }
    
    // Logic di chuyển giữ nguyên 100%
    void DiChuyen(int Huong) {
        for (int i = DoDai - 1; i > 0; i--)
            A[i] = A[i - 1];
        if (Huong == 0) A[0].x = A[0].x + 1;
        if (Huong == 1) A[0].y = A[0].y + 1;
        if (Huong == 2) A[0].x = A[0].x - 1;
        if (Huong == 3) A[0].y = A[0].y - 1;
    }
};

// ============================================================================
// HỆ THỐNG ĐIỀU KHIỂN CHUNG
// ============================================================================
class GameEngine {
private:
    MasterRenderer rd;
    EngineData data;
    State state = BOOTING;

    // Biến cho Menu
    int menuIdx = 0;
    int bootLevel = 0;
    int editStep = 0;
    long long frameCount = 0;
    bool isActive = true;

    // Biến cho Game 
    CONRAN r;
    int Huong = 0;
    int ChieuRong = 50;
    int ChieuCao = 30;

public:
    GameEngine() { srand(time(0)); }

    void PollInput() {
        // --- CHẾ ĐỘ ĐANG CHƠI GAME ---
        if (state == IN_GAME) {
            if (kbhit()) {
                char t = getch();
                if (t == 0 || t == -32 || t == 224) { if (kbhit()) getch(); return; } 

                // Phím gốc từ file Snake.cpp
                if (t == 'a') Huong = 2;
                if (t == 'w') Huong = 3;
                if (t == 'd') Huong = 0;
                if (t == 's') Huong = 1;

                if (t == 27) state = MAIN_MENU; // Bấm ESC để thoát về Menu
            }
            return;
        }

        // --- CHẾ ĐỘ GÕ VĂN BẢN (CHỈNH SỬA PROFILE) ---
        if (state == EDIT_PROFILE) {
            if (kbhit()) {
                char c = getch();

                if (c == 0 || c == -32 || c == 224) { if (kbhit()) getch(); return; }

                if (c == 13) { 
                    Beep(900, 50);
                    if (editStep == 0) editStep = 1;
                    else { state = SETTINGS; menuIdx = 1; Sleep(200); }
                }
                else if (c == 8) { 
                    if (editStep == 0 && data.owner.length() > 0) data.owner.erase(data.owner.length() - 1);
                    if (editStep == 1 && data.id.length() > 0) data.id.erase(data.id.length() - 1);
                }
                else if (isprint(c)) { 
                    if (editStep == 0 && data.owner.length() < 15) data.owner += toupper(c);
                    if (editStep == 1 && data.id.length() < 15) data.id += toupper(c);
                }
            }
            return; 
        }

        // --- CHẾ ĐỘ MENU BÌNH THƯỜNG ---
        int maxMenu = (state == MAIN_MENU) ? 4 : (state == SETTINGS ? 2 : 0);
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

    void TriggerAction() {
        if (state == BOOTING) {
            state = MAIN_MENU;
        }
        else if (state == MAIN_MENU) {
            if (menuIdx == 0) { 
                state = IN_GAME; // VÀO GAME
                r = CONRAN();    // Reset rắn
                Huong = 0;
            }
            else if (menuIdx == 1) { state = SETTINGS; menuIdx = 0; }
            else if (menuIdx == 4) isActive = false;
        }
        else if (state == SETTINGS) {
            if (menuIdx == 0) data.diffLevel = (data.diffLevel + 1) % 3;
            else if (menuIdx == 1) {
                state = EDIT_PROFILE;
                editStep = 0;
                while (kbhit()) getch();
            }
            else { state = MAIN_MENU; menuIdx = 1; }
        }
    }

    void Update() {
        frameCount++;
        if (state == BOOTING) {
            if (++bootLevel > 100) state = MAIN_MENU;
        }
        else if (state == IN_GAME) {
            // Trong file Snake.cpp cũ là Sleep(300). Giao diện chạy ở Sleep(15). 
            // Cứ 20 khung hình (15*20 = 300ms) thì di chuyển 1 lần để tốc độ giống y hệt file cũ.
            if (frameCount % 20 == 0) {
                r.DiChuyen(Huong); 
            }
        }
    }

    void RenderAll() {
        rd.ResetBuffer();

        // 1. MÀN HÌNH LOADING
        if (state == BOOTING) {
            rd.DrawUltimateSnakeTitle(40, 10);
            rd.Write(SCREEN_W / 2 - 12, 19, ">>> SYSTEM BOOTING...", C_DARK_GRAY);
            rd.DrawIndustrialBorder(SCREEN_W / 2 - 26, 21, 52, 2, C_DARK_GRAY);
            for (int i = 0; i < bootLevel / 2; i++) rd.Put(SCREEN_W / 2 - 25 + i, 22, 0x2588, C_CYAN);
            rd.Write(SCREEN_W / 2 + 28, 22, to_string(bootLevel) + "%", C_CYAN);
        }
        // 2. MÀN HÌNH ĐANG CHƠI GAME
        else if (state == IN_GAME) {
            int offsetX = 5;
            int offsetY = 5;
            
            // Vẽ khung với kích thước ChieuRong = 50, ChieuCao = 30 như file cũ
            rd.DrawIndustrialBorder(offsetX, offsetY, ChieuRong, ChieuCao, C_CYAN, true, "MISSION_AREA");
            
            // Gọi hàm Vẽ Rắn nguyên thủy
            r.Ve(rd, offsetX, offsetY);
            
            rd.Write(ChieuRong + 10, 5, ">>> PRESS ESC TO QUIT", C_DARK_GRAY);
        }
        // 3. MÀN HÌNH MENU / CẤU HÌNH / CHỈNH SỬA
        else if (state == MAIN_MENU || state == SETTINGS || state == EDIT_PROFILE) {
            if (frameCount % 10 == 0) {
                for (int i = 0; i < 15; i++) rd.Put(rand() % SCREEN_W, rand() % SCREEN_H, L'.', C_DARK_GRAY);
            }

            rd.Put(2, 1, 0x250F, C_CYAN); rd.Put(3, 1, 0x2501, C_CYAN); rd.Put(2, 2, 0x2503, C_CYAN);
            rd.Put(SCREEN_W - 4, 1, 0x2513, C_CYAN); rd.Put(SCREEN_W - 5, 1, 0x2501, C_CYAN); rd.Put(SCREEN_W - 4, 2, 0x2503, C_CYAN);
            rd.Put(2, SCREEN_H - 2, 0x2517, C_CYAN); rd.Put(3, SCREEN_H - 2, 0x2501, C_CYAN); rd.Put(2, SCREEN_H - 3, 0x2503, C_CYAN);
            rd.Put(SCREEN_W - 4, SCREEN_H - 2, 0x251B, C_CYAN); rd.Put(SCREEN_W - 5, SCREEN_H - 2, 0x2501, C_CYAN); rd.Put(SCREEN_W - 4, SCREEN_H - 3, 0x2503, C_CYAN);

            rd.Write(5, 1, " X:120 Y:042 ", C_DARK_GRAY);
            if (state == EDIT_PROFILE) rd.Write(SCREEN_W - 24, 1, " DATA_OVERRIDE: ON ", C_YELLOW);
            else rd.Write(SCREEN_W - 22, 1, " TARGET_LOCK: OFF ", C_DARK_GRAY);

            rd.DrawUltimateSnakeTitle(41, 4);
            rd.Write(48, 11, "--- MASTER EDITION ---", C_GRAY);

            string m[5] = { "START SIMULATION", "CONFIG ENGINE", "SKIN DATABASE", "LEADERBOARD", "SHUTDOWN SYSTEM" };
            int count = (state == MAIN_MENU) ? 5 : 3;

            if (state == SETTINGS || state == EDIT_PROFILE) {
                m[0] = "DIFFICULTY: " + string(data.diffLevel == 0 ? "EASY" : data.diffLevel == 1 ? "NORM" : "HARD");
                m[1] = "EDIT USER PROFILE";
                m[2] = "RETURN TO TERMINAL";
            }

            int menuStartX = 42;
            int menuStartY = 16;

            for (int i = 0; i < count; i++) {
                int y = menuStartY + i * 4;
                int len = m[i].length();

                if (menuIdx == i && state != EDIT_PROFILE) {
                    rd.DrawIndustrialBorder(menuStartX - 4, y - 1, len + 8, 2, C_CYAN, true);
                    if (frameCount % 20 < 10) { rd.Put(menuStartX - 2, y, 0x25BA, C_PINK); rd.Put(menuStartX + len + 1, y, 0x25C4, C_PINK); }
                    rd.Write(menuStartX, y, m[i], C_WHITE);
                }
                else if (menuIdx == i && state == EDIT_PROFILE) {
                    rd.DrawIndustrialBorder(menuStartX - 4, y - 1, len + 8, 2, C_YELLOW, true);
                    rd.Write(menuStartX, y, m[i], C_YELLOW);
                }
                else {
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

            rd.DrawIndustrialBorder(85, 16, 30, 16, (state == EDIT_PROFILE ? C_YELLOW : C_GRAY), true, "USER_DATABASE");

            rd.Write(87, 18, "NAME:", C_DARK_GRAY);
            if (state == EDIT_PROFILE && editStep == 0) rd.Write(95, 18, data.owner + (frameCount % 20 < 10 ? "_" : " "), C_YELLOW);
            else rd.Write(95, 18, data.owner, C_CYAN);

            rd.Write(87, 20, "MSSV:", C_DARK_GRAY);
            if (state == EDIT_PROFILE && editStep == 1) rd.Write(95, 20, data.id + (frameCount % 20 < 10 ? "_" : " "), C_YELLOW);
            else rd.Write(95, 20, data.id, C_WHITE);

            rd.Write(87, 22, "RECORD:", C_DARK_GRAY);  rd.Write(95, 22, to_string(data.highScore) + " PTS", C_YELLOW);
            rd.Write(87, 24, "MAP   :", C_DARK_GRAY);  rd.Write(95, 24, "SECTOR-7", C_GRAY);
            rd.Write(87, 26, "SKIN  :", C_DARK_GRAY);  rd.Write(95, 26, "CYBER-NEON", C_PINK);
            rd.Write(87, 28, "PING  :", C_DARK_GRAY);  rd.Write(95, 28, to_string(12 + (frameCount % 5)) + " ms", C_GREEN);

            if (state == EDIT_PROFILE) rd.Write(SCREEN_W / 2 - 25, SCREEN_H - 3, ">>> TYPING MODE... PRESS [ENTER] TO SAVE <<<", C_YELLOW);
            else rd.Write(SCREEN_W / 2 - 30, SCREEN_H - 3, ">>> USE [W,A,S,D] OR [ARROWS] TO NAVIGATE - [ENTER] TO SELECT <<<", C_DARK_GRAY);
        }

        rd.Flush();
    }

    void Run() {
        while (isActive) {
            PollInput();
            Update();
            RenderAll();
            Sleep(15);
        }
    }
};

int main() {
    SetConsoleTitleA("SNAKE MASTER EDITION - CYBER CORE ULTIMATE");
    GameEngine engine;
    engine.Run();
    return 0;
}