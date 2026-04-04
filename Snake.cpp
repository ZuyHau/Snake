#include <iostream>
#include <windows.h>
#include <cstdlib>
#include <conio.h>
using namespace std;
void VeKhung(int rong, int cao);
void gotoxy( int column, int line );
struct Point{
    int x,y;
};
class CONRAN{
public:
    struct Point A[100];
    int DoDai;
    CONRAN(){
        DoDai = 3;
        A[0].x = 10; A[0].y = 10;
        A[1].x = 11; A[1].y = 10;
        A[2].x = 12; A[2].y = 10;
    }
    void Ve(){
        for (int i = 0; i < DoDai; i++){
            gotoxy(A[i].x,A[i].y);
            cout<<"X";
        }
    }
    void DiChuyen(int Huong){
        for (int i = DoDai-1; i>0;i--)
            A[i] = A[i-1];
        if (Huong==0) A[0].x = A[0].x + 1;
        if (Huong==1) A[0].y = A[0].y + 1;
        if (Huong==2) A[0].x = A[0].x - 1;
        if (Huong==3) A[0].y = A[0].y - 1;

    }
};

int main()
{
    CONRAN r;
    int Huong = 0;
    char t;
    int ChieuRong = 50;
    int ChieuCao = 30;

    while (1){
        if (kbhit()){
            t = getch();
            if (t=='a') Huong = 2;
            if (t=='w') Huong = 3;
            if (t=='d') Huong = 0;
            if (t=='s') Huong = 1;
        }
        system("cls");
        VeKhung(ChieuRong,ChieuCao);
        r.Ve();
        r.DiChuyen(Huong);
        Sleep(300);
    }

    return 0;
}


void gotoxy( int column, int line )
  {
  COORD coord;
  coord.X = column;
  coord.Y = line;
  SetConsoleCursorPosition(
    GetStdHandle( STD_OUTPUT_HANDLE ),
    coord
    );
  }

void VeKhung(int rong, int cao) {
    // Vẽ tường trên (y = 0) và tường dưới (y = cao)
    for (int i = 0; i <= rong; i++) {
        gotoxy(i, 0); cout << "#";
        gotoxy(i, cao); cout << "#";
    }
    // Vẽ tường trái (x = 0) và tường phải (x = rong)
    for (int i = 0; i <= cao; i++) {
        gotoxy(0, i); cout << "#";
        gotoxy(rong, i); cout << "#";
    }
}