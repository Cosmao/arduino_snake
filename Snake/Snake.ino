#include "dotMatrix.h"

/* 
TODO Add apples to make it grow
TODO Collision detection, go around other side?
TODO make you lose somehow
TODO use the cleanup function to properly free the tail
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
    snakePart *nextPartPtr;
};

struct snakeHead : snakePart{
    bool addPartOnMove;
    uint8_t direction;
    snakePart *endPartPtr;
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
    myHead.direction = 0x00;
    myHead.x = 4;
    myHead.y = 4;
    myHead.addPartOnMove = false;
    myHead.endPartPtr = nullptr;
    myHead.nextPartPtr = nullptr;
}

void loop(){
    limiter++;
    dotMatrix::displayOnMatrix(screenBuffer);
    setDirection(myHead);
    if(limiter % 50 == 0){
        move(myHead);
        updateScreenBuffer(myHead);
    }  
}

//1024 max 0 min
//512 avg
//Sets the directions
void setDirection(snakeHead &snake){
    int xVal, yVal, zVal;
    xVal = analogRead(xAxisPin);
    yVal = analogRead(yAxisPin);
    zVal = digitalRead(zAxisPin);

    if(zVal == 0)
        snake.addPartOnMove = true;

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
    if(snake.addPartOnMove == true)
        addPart(snake);
    
    if(snake.nextPartPtr != nullptr)
        moveSnakePieces(*snake.nextPartPtr, snake.x, snake.y); //update pieces before head

    switch(snake.direction){ //directions are kinda fucked since I started doing this with the angle on the joystick was odd
        case left:
            if(snake.x != 7)
                snake.x++;
            else
                cleanUp(snake);
        break;

        case right:
            if(snake.x != 0)
                snake.x--;
            else
                cleanUp(snake);
        break;

        case up:
            if(snake.y !=0x01)
                snake.y >>= 1;
            else
                cleanUp(snake);
        break;
        
        case down:
            if(snake.y !=0x80)
                snake.y <<= 1;
            else
                cleanUp(snake);
        break;
    }
    snake.addPartOnMove = false;
}

void collisionDetect(snakeHead &snake){

}

//Resets the game
void cleanUp(snakeHead &snake){
    clearSnakeTail(snake);
    snake.x = 4;
    snake.y = 4;
    snake.direction = 0x00;
    snake.nextPartPtr = nullptr;
    snake.endPartPtr = nullptr;
    snake.addPartOnMove = false;
}

void moveSnakePieces(snakePart &snakePiece, uint8_t x, uint8_t y){
    snakePart *snakePartPtr = snakePiece.nextPartPtr; //get next part
    if(snakePartPtr != nullptr){                     //we arent the last part
        uint8_t curX = snakePiece.x;                //save our positions
        uint8_t curY = snakePiece.y;
        snakePiece.x = x;                           //update to the previous ones
        snakePiece.y = y;
        moveSnakePieces(*snakePartPtr, curX, curY); //go again
    }
    else{
        if(myHead.addPartOnMove) //if we just added one leave it where it is
            return;
        else{
            snakePiece.x = x;
            snakePiece.y = y;
        }
    }
}

//Cleans screenbuffer and adds the lone dot back to it
void updateScreenBuffer(snakeHead &snake){
    for(auto &i : screenBuffer){ //clean the screenBuffer
        i = 0x00;
    }
    screenBuffer[snake.x] = snake.y; //Update column to the head

    snakePart *snakePartPtr = snake.nextPartPtr;
    while(snakePartPtr != nullptr){ //end at nullptr, otherwise iterate over all pieces
        screenBuffer[snakePartPtr->x] = screenBuffer[snakePartPtr->x] | snakePartPtr->y; //bitwise OR
        snakePartPtr = snakePartPtr->nextPartPtr;     //go to next part
    }
}

void addPart(snakeHead &snake){
    if(snake.nextPartPtr == nullptr){
        snakePart* snakePiece = (snakePart*) malloc(sizeof(snakePart)); //malloc returns a void * so we need to cast to our struct
        if(snakePiece == nullptr){      //malloc returns a nullptr if no space to allocate
            Serial.println(F("MEM FULL")); //F() turns it into a string literal
        }
        else{       //update pointers and take the heads position, wont move last dot on the tick its created
            snake.nextPartPtr = snakePiece;
            snake.endPartPtr = snakePiece;
            snakePiece->x = snake.x;
            snakePiece->y = snake.y;
            snakePiece->nextPartPtr = nullptr;
        }
    }
    else{
        snakePart* snakePiece = (snakePart*) malloc(sizeof(snakePart));
        if(snakePiece == nullptr){      //malloc returns a nullptr if no space to allocate
            Serial.println(F("MEM FULL"));
        }
        else{
            snakePiece->x = snake.endPartPtr->x;             //Take the positions of the last element
            snakePiece->y = snake.endPartPtr->y;
            snakePiece->nextPartPtr = nullptr;
            snake.endPartPtr->nextPartPtr = snakePiece;     //endpointer pointing towards last piece set that pointer to the new piece
            snake.endPartPtr = snakePiece;                  //set as new last piece
        }
    }
}

void clearSnakeTail(snakeHead &snake){
    snakePart *snakePartPtr = snake.nextPartPtr;
    while(snakePartPtr != nullptr){                 //end at nullptr, otherwise iterate over all pieces
        snakePart *tempPtr = snakePartPtr;          //save current piece in a temp pointer
        snakePartPtr = snakePartPtr->nextPartPtr;   //Get the next piece
        free(tempPtr);                              //free the current one when we have the next
    }
    free(snakePartPtr);                             //free the last piece and end func
}