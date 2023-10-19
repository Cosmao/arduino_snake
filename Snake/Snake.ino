#include "dotMatrix.h"

/* 
TODO Add growing snake
TODO Add apples to make it grow
TODO Collision detection, go around other side?
TODO make you lose somehow
 */

#define up 0x00
#define down 0x01
#define right 0x02
#define left 0x04

//Joystick pins
const int xAxisPin = 0;
const int yAxisPin = 1;
const int zAxisPin = 10;
int limiter = 0;

//Basic struct, I probably want to link them together and iterate util a nullPTR
struct snakePart{
    uint8_t x;
    uint8_t y;
    snakePart *ptr;
};

struct snakeHead : snakePart{
    bool addPartOnMove;
    uint8_t direction;
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
    pinMode(zAxisPin, INPUT_PULLUP);
    Serial.begin(9600);
    myHead.x = 4;
    myHead.y = 4;
    myHead.direction = 0x00;
    myHead.ptr = nullptr;
    myHead.addPartOnMove = false;
}

void loop(){
    limiter++;
    dotMatrix::displayOnMatrix(screenBuffer);
    setDirection(myHead);
    if(limiter % 50 == 0){
        //moveDot(myHead);
        move(myHead);
        updateScreenBuffer(myHead.x, myHead.y);
    }  
}

//1024 max 0 min
//512 avg
//Sets the directions
void setDirection(snakeHead &snake){
    int xVal, yVal;
    xVal = analogRead(xAxisPin);
    yVal = analogRead(yAxisPin);

    if(xVal < 100){
        snake.direction = left;
    }
    if(xVal > 900){
        snake.direction = right;
    }
    if(yVal < 100){
        snake.direction = up;
    }
    if(yVal > 900){
        snake.direction = down;
    }
}

//moves depending on the direction set
void move(snakeHead &snake){
    switch(snake.direction){
        case left:
            if(snake.x != 7)
                snake.x++;
        break;

        case right:
            if(snake.x != 0)
                snake.x--;
        break;

        case up:
            if(snake.y !=0x01)
                snake.y >>= 1;
        break;
        
        case down:
            if(snake.y !=0x80)
                snake.y <<= 1;
        break;
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