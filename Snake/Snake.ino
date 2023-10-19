#include "dotMatrix.h"

/* 
Everything about the dotMatrix screen is handled by dotMatrix.h
*/

const int xAxisPin = 0;
const int yAxisPin = 1;

struct snakeHead{
    uint8_t x;
    uint8_t y;
    snakeHead *ptr;
};

snakeHead myHead;

//empty screenBuffer
//column by column
byte screenBuffer[] = {
    0x00, 0x00,
    0x00, 0x00,
    0x00, 0x00,
    0x00, 0x00
};

void setup(){
    dotMatrix::setup();
    myHead.x = 4;
    myHead.y = 4;
}

void loop(){
    dotMatrix::displayOnMatrix(screenBuffer);
    moveDot(myHead);
    updateScreenBuffer(myHead.x, myHead.y);
}

//Change to keeping track of direction
void moveDot(snakeHead &ptr){
    int xVal, yVal;
    xVal = analogRead(xAxisPin);
    yVal = analogRead(yAxisPin);
    if(xVal < 100){
        if(ptr.x == 7)
            return;
        ptr.x++;
        return;
    }
    if(xVal > 900){
        if(ptr.x == 0)
            return;
        ptr.x--;
        return;
    }
    if(yVal < 100){
        if(ptr.y == 0x01)
            return;
        ptr.y >>= 1;
        return;
    }
    if(yVal > 900){
        if(ptr.y == 0x80)
            return;
        ptr.y <<= 1;
        return;
    }
}

void updateScreenBuffer(uint8_t x, uint8_t y){
    for(auto &i : screenBuffer){
        i = 0x00;
    }
    screenBuffer[x] = y;
}

void wave(){
    for(auto &a : screenBuffer){
        if((a & 0x80) == 0x80){
            a <<= 1;
            a++;
        }
        else{
            a <<= 1;
        }
        if(a == 0){
            a = 0x1;
        }
    }
}