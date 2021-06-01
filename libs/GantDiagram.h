//
// Created by Vitalik on 22.03.2021.
//

#ifndef LABA2_GANTDIAGRAM_H
#define LABA2_GANTDIAGRAM_H

//just to poviyobuvatsa
#include <iostream>
#include <string>
#include <conio.h>
#include <windows.h>

#include "Matrix.h"

enum TextColor {
    N_BLUE = 1,
    N_GREEN = 2,
    N_CYAN = 3,
    N_RED = 4,
    N_PURPLE = 5,
    N_YELLOW = 6,
    N_DEFAULT_WHITE = 7,
    N_GRAY = 8,
    N_BRIGHT_BLUE = 9,
    N_BRIGHT_GREEN = 10,
    N_BRIGHT_CYAN = 11,
    N_BRIGHT_RED = 12,
    N_PINK = 13,
    N_BRIGHT_YELLOW = 14,
    N_BRIGHT_WHITE = 15
};

#define BLUE 1
#define GREEN 2
#define CYAN 3
#define RED 4
#define PURPLE 5
#define YELLOW 6
#define DEFAULT_WHITE 7
#define GRAY 8
#define BRIGHT_BLUE 9
#define BRIGHT_GREEN 10
#define BRIGHT_CYAN 11
#define BRIGHT_RED 12
#define PINK 13
#define BRIGHT_YELLOW 14
#define BRIGHT_WHITE 15

void resetConsoleTextColor(){
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), TextColor(N_DEFAULT_WHITE));
}
/*
void textColor(TextColor textColor = TextColor(N_DEFAULT_WHITE)){
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), textColor);
}*/

void textColor(int color = DEFAULT_WHITE){
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), (WORD)color);
}


void printColorString(std::string str, int color = DEFAULT_WHITE) {
    textColor(color);
    std::cout << str << std::endl;
    resetConsoleTextColor();
}

void setCursor( COORD c) {
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), c);
}

void setCursor(int x, int y) {
    COORD c;
    c.X = static_cast<short>(x);
    c.Y = static_cast<short>(y);
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), c);
}


COORD drawAxis(COORD curPos, int yAxisLength, int xAxisLength) {
    COORD zeroPos;
    //draw Y narrow
    curPos.Y = curPos.Y+1;
    //curPos.X = 0;
    setCursor(curPos);
    putch('^');

    for (int i = 1; i < yAxisLength - 1; ++i) {
        curPos.Y = curPos.Y+1;
        setCursor(curPos);
        putch('|');
    }
    curPos.Y = curPos.Y+1;
    setCursor(curPos);
    putch('0');
    zeroPos = curPos;
    for (int i = 0; i < xAxisLength - 1; ++i) {
        curPos.X = curPos.X+1;
        setCursor(curPos);
        putch('-');
    }
    curPos.X = curPos.X+1;
    setCursor(curPos);
    putch('>');
    return zeroPos;
}

void drawLine(COORD from, unsigned int length, char symbol = '#'){
    setCursor(from);
    for (int i = 0; i < length; ++i) {
        putchar(symbol);
    }

}

void writeWord(COORD from, std::string str) {
    setCursor(from);
    for(char symbol: str){
        putchar(symbol);
    }
}

void writeVWord(COORD from, std::string str) {
    static int color = 0;
    color = (color + 1)%2;
    if(color == 0) {
        textColor(BRIGHT_WHITE);
    } else {
        textColor(BRIGHT_YELLOW);
    }

    for (char symbol: str) {
        setCursor(from);
        putchar(symbol);
        from.Y = from.Y + static_cast<short>(1);
    }
    textColor();
}


void drawGantDiagram(Matrix<int> start, Matrix<int> end) {
    if(start.getHeight() != end.getHeight() || start.getWidth() != end.getWidth()) {

        return;
    }
    unsigned  int n = start.getHeight();
    unsigned  int m = start.getWidth();
    int scale = 2;
    int yAxisLength = m*scale+scale;
    int xAxisLength = (end.max()+1)*scale;

    CONSOLE_SCREEN_BUFFER_INFO cbsi;
    if (!GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &cbsi)) {
        std::cout << "ass";
        return;
    }

    COORD pos = cbsi.dwCursorPosition;
    pos.X = pos.X + 5;
    COORD zeroPos = drawAxis(pos, yAxisLength, xAxisLength);


    int length;


    //sign oy
    pos.X = 0;
    for (int j = 0; j < m; ++j) {
        pos.Y = zeroPos.Y - static_cast<short>(m - j*scale+scale);
        writeWord(pos, "GVM" + std::to_string(j+1));
    }
    //sign ox

    for (int i = 1; i*scale < xAxisLength; i++) {
        pos.Y = zeroPos.Y + static_cast<short>(1);
        pos.X = zeroPos.X + static_cast<short>(i*scale);
        writeVWord(pos, std::to_string(i));
    }



    //draw diagram lines
    for (int i = 0; i < n; ++i) {
        textColor(i+9);
        for (int j = 0; j < m; ++j) {
            length = (end[i][j] - start[i][j])*scale;
            pos.X = zeroPos.X + static_cast<short>(scale*start[i][j]+1);
            pos.Y = zeroPos.Y - static_cast<short>(m - j*scale+scale);
            drawLine(pos, length, i+48+1);
        }
    }
    textColor();

    pos.X = 0;
    pos.Y = zeroPos.Y + 3;
    setCursor(pos);
}

/*
class GantDiagram {

};
*/


#endif //LABA2_GANTDIAGRAM_H
