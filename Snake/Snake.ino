#include "dotMatrix.h"

/* 
Everything about the dotMatrix screen is handled by dotMatrix.h
*/

uint8_t interval = 0;

//screenbuffer already setup in an angle for the wave to properly work
byte screenBuffer[] = {
    0x80, 0x40,
    0x20, 0x10,
    0x8, 0x4,
    0x2, 0x1
};

void setup(){
    dotMatrix::setup();
}

void loop(){
    interval ++;
    dotMatrix::displayOnMatrix(screenBuffer);
    if(interval % 20 == 0)
        wave();
}

void wave(){
    for(auto &a : screenBuffer){
        a <<= 1;
        if(a == 0){
            a = 0x1;
        }
    }
}