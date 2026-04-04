#include <iostream>
#include <windows.h>
#include <cstdlib>
#include <conio.h>
#include <vector>

using namespace std;


void gotoxy(int column, int line);
void VeKhung(int rong, int cao); 


struct Point {
    int x, y;
};

enum Dir { STOP = 0, LEFT, RIGHT, UP, DOWN };
enum State { BOOTING, MAIN_MENU, IN_GAME, CRITICAL_FAILURE };

class CONRAN {
public:
    vector<Point> snake; 
    
    void InitSnake() {
        snake.clear();
      
        for (int i = 0; i < 5; i++) snake.push_back({ 20 - i, 12 });
    }
    
    void Ve() {
        for (size_t i = 0; i < snake.size(); i++) {
            gotoxy(snake[i].x, snake[i].y);
            if (i == 0) cout << "O"; 
            else cout << "x";      
        }
    }

    void DiChuyen(Dir direction) {
        
        for (size_t i = snake.size() - 1; i > 0; i--) {
            snake[i] = snake[i - 1];
        }

    
        switch (direction) {
            case UP:    snake[0].y--; break;
            case DOWN:  snake[0].y++; break;
            case LEFT:  snake[0].x--; break;
            case RIGHT: snake[0].x++; break;
            default: break;
        }
    }
};

int main() {
    CONRAN r;
    r.InitSnake();
    Dir direction = RIGHT;
    char t;

 
    int chieuRongKhung = 50;  
    int chieuCaoKhung = 20;   

    while (1) {
        if (kbhit()) {
            t = getch();
            if (t == 'a' && direction != RIGHT) direction = LEFT;
            if (t == 'd' && direction != LEFT)  direction = RIGHT;
            if (t == 'w' && direction != DOWN)  direction = UP;
            if (t == 's' && direction != UP)    direction = DOWN;
            if (t == 27) break; 
        }
        
        system("cls");
        
    
        VeKhung(chieuRongKhung, chieuCaoKhung); 
        r.Ve();
        r.DiChuyen(direction);
        
        Sleep(100); 
    }

    return 0;
}

void gotoxy(int column, int line) {
    COORD coord;
    coord.X = column;
    coord.Y = line;
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
}

void VeKhung(int rong, int cao) {
 
    for (int i = 0; i <= rong; i++) {
        gotoxy(i, 0); cout << "#";
        gotoxy(i, cao); cout << "#";
    }
    for (int i = 0; i <= cao; i++) {
        gotoxy(0, i); cout << "#";
        gotoxy(rong, i); cout << "#";
    }
}