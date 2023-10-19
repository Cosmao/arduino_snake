/* 
Wrapper header to hide all the dotMatrix stuff, So I only have to care about a single function
*/

#pragma once
#ifndef dotMatrixEK
#define dotMatrixEK

namespace dotMatrix{

//If I dont want the default pins remove the define
#define DotMatrixDefault
#ifdef DotMatrixDefault
const int latchPin = 12;
const int clockPin = 13;
const int dataPin = 11;
const uint8_t LEDPins[] = { 2, 3, 4, 5, 6, 7, 8, 9};
#endif

//forward Declarations
//Everything is overloaded to handle with DotMatrixDefault
//and if you want to use your own pins for w/e reason
void setup(const uint8_t latchPin, 
            const uint8_t clockPin, 
            const uint8_t dataPin, 
            const uint8_t LEDPins[]);
void setup();
void displayOnMatrix(byte screenBuffer[8], 
                        const uint8_t LEDPin[],
                        const int latchPin,
                        const int clockPin,
                        const int dataPin);

void displayOnMatrix(byte screenBuffer[8]);
void matrixCol(byte value, const uint8_t LEDPin[]);
void matrixCol(byte value);
void matrixRows(byte value, int latchPin, int dataPin, int clockPin);
void matrixRows(byte value);

//Sets up all pins for output mode
void setup(const uint8_t latchPin, 
            const uint8_t clockPin, 
            const uint8_t dataPin, 
            const uint8_t LEDPins[]){
    pinMode(latchPin, OUTPUT);
    pinMode(clockPin, OUTPUT);
    pinMode(dataPin, OUTPUT);
    for(int i = 0; i < 8; i++){
        pinMode(LEDPins[i], OUTPUT);
    }
}

void setup(){
    pinMode(latchPin, OUTPUT);
    pinMode(clockPin, OUTPUT);
    pinMode(dataPin, OUTPUT);
    for(int i = 0; i < 8; i++)
        pinMode(LEDPins[i], OUTPUT);
}

//Takes a screenBuffer array to be displayed on the dot matrix
void displayOnMatrix(byte screenBuffer[8], 
                        const uint8_t LEDPin[],
                        const int latchPin,
                        const int clockPin,
                        const int dataPin){
    int cols = 0x01;
    for(int i = 0; i < 8; i++){
        matrixCol(cols, LEDPin);
        matrixRows(screenBuffer[i], latchPin, clockPin, dataPin);
        delay(1);
        matrixRows(0x00, latchPin, clockPin, dataPin);
        cols <<= 1;
    }
}

void displayOnMatrix(byte screenBuffer[8]){
    int cols = 0x01;
    for(int i = 0; i < 8; i++){
        matrixCol(cols);
        matrixRows(screenBuffer[i]);
        delay(1);
        matrixRows(0x00);
        cols <<= 1;
    }
}

//handles columns
void matrixCol(byte value, const uint8_t LEDPin[]){
    byte cols = 0x01;
    for(int i = 0; i < 8; i++){
        digitalWrite(LEDPin[i], ((value & cols) == cols) ? LOW : HIGH);
        cols <<= 1;
    }
}

void matrixCol(byte value){
    byte cols = 0x01;
    for(int i = 0; i < 8; i++){
        digitalWrite(LEDPins[i], ((value & cols) == cols) ? LOW : HIGH);
        cols <<= 1;
    }
}


//handles rows
void matrixRows(byte value, int latchPin, int clockPin, int dataPin){
    digitalWrite(latchPin, LOW);
    shiftOut(dataPin, clockPin, LSBFIRST, value);
    digitalWrite(latchPin, HIGH);
}

void matrixRows(byte value){
    digitalWrite(latchPin, LOW);
    shiftOut(dataPin, clockPin, LSBFIRST, value);
    digitalWrite(latchPin, HIGH);
}

}
#endif