/* 
Wrapper header to hide all the dotMatrix stuff, So I only have to care about a single function
*/

#pragma once
#ifndef dotMatrixEK
#define dotMatrixEK

namespace dotMatrix{

//forward Declarations
void setup(const uint8_t latchPin, 
            const uint8_t clockPin, 
            const uint8_t dataPin, 
            const uint8_t LEDPins[]);

void displayOnMatrix(byte screenBuffer[8], 
                        const uint8_t LEDPin[],
                        const int latchPin,
                        const int clockPin,
                        const int dataPin);

void matrixCol(byte value, const uint8_t LEDPin[]);
void matrixRows(byte value, int latchPin, int dataPin, int clockPin);

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

//handles columns
void matrixCol(byte value, const uint8_t LEDPin[]){
    byte cols = 0x01;
    for(int i = 0; i < 8; i++){
        digitalWrite(LEDPin[i], ((value & cols) == cols) ? LOW : HIGH);
        cols <<= 1;
    }
}

//handles rows
void matrixRows(byte value, int latchPin, int clockPin, int dataPin){
    digitalWrite(latchPin, LOW);
    shiftOut(dataPin, clockPin, LSBFIRST, value);
    digitalWrite(latchPin, HIGH);
}

}
#endif