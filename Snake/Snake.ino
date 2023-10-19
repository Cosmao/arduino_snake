#include "dotMatrix.h"

/* 
TODO Add growing snake
TODO Add apples to make it grow
TODO Collision detection, go around other side?
TODO Make it remember the direction and keep going if you dont change
TODO make you lose somehow
 */

//Joystick pins
const int xAxisPin = 0;
const int yAxisPin = 1;
int limiter = 0;

//Basic struct, I probably want to link them together and iterate util a nullPTR
struct snakePart{
    uint8_t x;
    uint8_t y;
    snakePart *ptr;
};

snakePart myHead;

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
    myHead.ptr = nullptr;
}

void loop(){
    limiter++;
    dotMatrix::displayOnMatrix(screenBuffer);
    if(limiter % 50 == 0){
        moveDot(myHead);
        updateScreenBuffer(myHead.x, myHead.y);
    }  
}

//Moves the dot according to joystick
void moveDot(snakePart &ptr){
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

//Cleans screenbuffer and adds the lone dot back to it
void updateScreenBuffer(uint8_t x, uint8_t y){
    for(auto &i : screenBuffer){
        i = 0x00;
    }
    screenBuffer[x] = y;
}

//Moves everything to the left and makes it reappear on the right side when it reaches the end
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