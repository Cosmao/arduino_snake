const int latchPin = 12;
const int clockPin = 13;
const int dataPin = 11;
const int LEDPin[] = {2, 3, 4, 5, 6, 7, 8, 9};
uint8_t interval = 0;

byte screenBuffer[] = {
    0b10000000, 0b01000000,
    0b00100000, 0b00010000,
    0b00001000, 0b00000100,
    0b00000010, 0b00000001
};

void setup(){
    pinMode(latchPin, OUTPUT);
    pinMode(clockPin, OUTPUT);
    pinMode(dataPin, OUTPUT);
    for(int i = 0; i < 8; i++){
        pinMode(LEDPin[i], OUTPUT);
    }
}

void loop(){
    interval ++;
    displayOnMatrix(screenBuffer);
    if(interval % 50 == 0)
        wave();
}

void wave(){
    for(auto &a : screenBuffer){
        a <<= 1;
        if(a == NULL){
            a = 0x1;
        }
    }
}

void displayOnMatrix(byte screenBuffer[]){
    int cols = 0x01;
    for(int i = 0; i < 8; i++){
        matrixCol(cols);
        matrixRows(screenBuffer[i]);
        delay(1);
        matrixRows(0x00); //Null terminated?
        cols <<= 1;
    }
}

void matrixCol(byte value){
    byte cols = 0x01;
    for(int i = 0; i < 8; i++){
        digitalWrite(LEDPin[i], ((value & cols) == cols) ? LOW : HIGH);
        cols <<= 1;
    }
}

void matrixRows(byte value){
    digitalWrite(latchPin, LOW);
    shiftOut(dataPin, clockPin, LSBFIRST, value);
    digitalWrite(latchPin, HIGH);
}