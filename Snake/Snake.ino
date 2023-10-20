#include "dotMatrix.h"

/* 
TODO Add apples to make it grow
TODO Collision detection for hitting the player
TODO Consider if I want to loop around or no
 */

//Some defines for clearer code
#define up 0x00
#define down 0x01
#define right 0x02
#define left 0x04
#define gameSpeed 50

//Joystick pins
const int xAxisPin = 0;
const int yAxisPin = 1;
const int zAxisPin = 10;    //TODO remove once apples are here
int limiter = 0;

//Only used for the apple
struct position{
    uint8_t x;
    uint8_t y;
};

//Basic struct, I probably want to link them together and iterate util a nullPTR
struct snakePart : position{
    snakePart *nextPartPtr;
};

//extend the base struct with some extra features needed for head
struct snakeHead : snakePart{
    bool addPartOnMove;
    uint8_t direction;
    snakePart *endPartPtr;
};

//Our "player" and apple
snakeHead myHead;
position apple;

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
    pinMode(zAxisPin, INPUT_PULLUP);    //TODO remove once apples are here
    Serial.begin(9600);
    cleanUp(myHead);                    //just running the cleanup func to init to remove code duplication
    randomSeed(analogRead(2));
}

void loop(){
    limiter++;
    dotMatrix::displayOnMatrix(screenBuffer);   //Update the screen needs to run every loop otherwise it flickers
    setDirection(myHead);                       //Gets the joystick direction
    if(limiter % gameSpeed == 0){               //every gameSpeed actually do the logic
        move(myHead);                           //moves the snake
        updateScreenBuffer(myHead);             //updates the screenbuffer
    }  
}

//1024 max 0 min
//512 avg
//Sets the directions
//TODO another pass on this function, kinda dirty
void setDirection(snakeHead &snake){
    int xVal, yVal, zVal;           //TODO remove zVal once apples are here
    xVal = analogRead(xAxisPin);
    yVal = analogRead(yAxisPin);
    zVal = digitalRead(zAxisPin);   //TODO remove once apples are here

    if(zVal == 0)                   //TODO remove once apples are here
        snake.addPartOnMove = true;

    if(xVal < 100)
        snake.direction = left;

    if(xVal > 900)
        snake.direction = right;
    
    if(yVal < 100)
        snake.direction = up;
    
    if(yVal > 900)
        snake.direction = down;
    
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

//TODO implement, run after moving head
//I can just check the screenBuffer after its made for the bit I'm void setup()
//Might come with some other issues if I do it like that, cheap though
//screenBuffer[snake.x] byte on pos y = snake.y
void collisionDetect(snakeHead &snake){

}

//Resets the game
void cleanUp(snakeHead &snake){
    clearSnakeTail(snake);
    snake.x = 4;
    snake.y = 4;
    snake.direction = down;
    snake.nextPartPtr = nullptr;
    snake.endPartPtr = nullptr;
    snake.addPartOnMove = false;
}

//moves across the pointers to move all snake pieces
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
        if(!myHead.addPartOnMove){ //if we just added one leave it where it is
            snakePiece.x = x;
            snakePiece.y = y;
        }
    }
}

//updates the screenBuffer with the position of everything
void updateScreenBuffer(snakeHead &snake){
    for(auto &i : screenBuffer){ //clean the screenBuffer
        i = 0x00;
    }
    screenBuffer[snake.x] = snake.y; //Insert head, simple equal works since its first added

    snakePart *snakePartPtr = snake.nextPartPtr;        //get ptr for first part
    while(snakePartPtr != nullptr){                     //end at nullptr, otherwise iterate over all pieces
        screenBuffer[snakePartPtr->x] = screenBuffer[snakePartPtr->x] | snakePartPtr->y; //bitwise OR so we can handle several pieces on a column
        snakePartPtr = snakePartPtr->nextPartPtr;       //set the ptr to next part
    }
}

void addPart(snakeHead &snake){
    if(snake.nextPartPtr == nullptr){       //Only runs as first piece is created
        snakePart* snakePiece = (snakePart*) malloc(sizeof(snakePart)); //malloc returns a void * so we need to cast it to a ptr for our struct
        if(snakePiece == nullptr){          //malloc returns a nullptr if no space to allocate
            Serial.println(F("MEM FULL"));  //F() turns it into a string literal
            cleanUp(snake);                 //malloc failed so cleanup the game (Should never happen)
        }
        else{       //update pointers and take the heads position, wont move last dot on the tick its created
            snake.nextPartPtr = snakePiece;
            snake.endPartPtr = snakePiece;
            snakePiece->x = snake.x;
            snakePiece->y = snake.y;
            snakePiece->nextPartPtr = nullptr;              //making sure its a nullptr for other logic
        }
    }
    else{       //runs for all other pieces created
        snakePart* snakePiece = (snakePart*) malloc(sizeof(snakePart));
        if(snakePiece == nullptr){          //malloc returns a nullptr if no space to allocate
            Serial.println(F("MEM FULL"));
            cleanUp(snake);                 //malloc failed so we reset game (Should never happen)
        }
        else{
            snakePiece->x = snake.endPartPtr->x;            //Take the positions of the last element
            snakePiece->y = snake.endPartPtr->y;
            snakePiece->nextPartPtr = nullptr;              //set the next piece as a nullptr since we are last
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