#include "dotMatrix.h"

const int latchPin = 12;
const int clockPin = 13;
const int dataPin = 11;
const uint8_t LEDPin[] = {2, 3, 4, 5, 6, 7, 8, 9};
uint8_t interval = 0;

byte screenBuffer[] = {
    0b10000000, 0b01000000,
    0b00100000, 0b00010000,
    0b00001000, 0b00000100,
    0b00000010, 0b00000001
};

void setup(){
    dotMatrix::setup(latchPin, clockPin, dataPin, LEDPin);
}

void loop(){
    interval ++;
    dotMatrix::displayOnMatrix(screenBuffer, LEDPin, latchPin, clockPin, dataPin);
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