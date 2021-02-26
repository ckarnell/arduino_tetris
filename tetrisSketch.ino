#include "engine.h"
#include <RGBmatrixPanel.h>

// Joystick
//#define joySw 49
//#define joyY A7
//#define joyX A8
#define seed A9

// Buttons
#define counterClockwiseButton 45
#define clockwiseButton 47
#define upButton 40
#define leftButton 38
#define rightButton 36
#define downButton 34

// LED Matrix
#define CLK 11 // USE THIS ON ARDUINO MEGA
#define OE   9
#define LAT 10
#define A   A0
#define B   A1
#define C   A2
#define D   A3

TetrisEngine tetrisEngine = TetrisEngine();
const int SQUARE_WIDTH = 3;

RGBmatrixPanel matrix(A, B, C, D, CLK, LAT, OE, false, 64);

const int PIXEL_OFFSET_X = 1;
const int PIXEL_OFFSET_Y = 4;

const int CYAN   = matrix.Color333(0,4,7); // 1183
const int VIOLET = matrix.Color333(4,0,7); // -28641
const int RED    = matrix.Color333(7,0,0); // -2048
const int YELLOW = matrix.Color333(7,4,0); // -896
const int GREEN  = matrix.Color333(0,7,0); // 2016
const int ORANGE = matrix.Color333(7,2,0); // -1472
const int BLUE   = matrix.Color333(0,0,7);

int adjustYCoord(int yCoord, int multiplier) {
  // We want y = 20 to appear at 60 on the led board,
  // but we want y = 40 to appear at 0 on the led board,
  // a linear function.
  // f(BUFFER_HEIGHT) = 60
  // f(24) = 0
  int result = (MAIN_MATRIX_HEIGHT+BUFFER_ZONE_HEIGHT+BORDER_SIZE)*multiplier; // Start with the max height
  result -= yCoord*multiplier;
  return result;
}

void drawSquareNew(int xCoord, int yCoord, int color, int multiplier) {
  int adjustedYCoord = adjustYCoord(yCoord, multiplier) - SQUARE_WIDTH + PIXEL_OFFSET_Y; // Subtract 3 since the origin is seen as lower left instead of top left
//  if (adjustedYCoord > matrix.height()) {
//    // Nothing to draw if we're outside of the boord
//    return;
//  }
  int adjustedXCoord = ((xCoord - 1) * multiplier) + PIXEL_OFFSET_X;
  matrix.fillRect(adjustedYCoord, adjustedXCoord, multiplier, multiplier, color);
}

const int colorMap[9] = {0, 1, VIOLET, GREEN, RED, CYAN, ORANGE, BLUE, YELLOW};

bool gameOver = false;
bool gameOverDrawn = false;

void setup() {
  matrix.begin();
  Serial.begin(9875);
//  randomSeed(analogRead(3));
  randomSeed(analogRead(seed));
  pinMode(clockwiseButton, INPUT);
  pinMode(counterClockwiseButton, INPUT);
//  pinMode(joySw, INPUT);
  pinMode(upButton, INPUT);
  pinMode(leftButton, INPUT);
  pinMode(rightButton, INPUT);
  pinMode(downButton, INPUT);
}

bool firstIteration = true;
unsigned char *matrixRepresentationCopy = nullptr;
//int iterations = 100;
long gameOverAt;
long timeBeforeSleep = 60000;

struct Controls controls;
int joySwPushed = false;
int joySwReleased = false;

void clearBottom() {
  matrix.fillRect(0, 0, 6, matrix.height(), matrix.Color333(0, 0, 0));
}

void clearMatrix() {
  matrix.fillRect(0, 0, matrix.width(), matrix.height(), matrix.Color333(0, 0, 0));
}

void loop() {
//  int xValue = analogRead(joyX); // > 766 = right, <256 = left
//  int yValue = analogRead(joyY); // 511 by default, >766 = down, <100 = up
  int swValue = digitalRead(counterClockwiseButton); // 0 when pushed, random otherwise
  if (gameOver && joySwPushed && !swValue) {
    joySwReleased = true;
  }
  int upValue = digitalRead(upButton);
  int leftValue = digitalRead(leftButton);
  int rightValue = digitalRead(rightButton);
  int downValue = digitalRead(downButton);
  joySwPushed = swValue;
  
  int clockwiseButtonValue = digitalRead(clockwiseButton); // 1 when pushed, 0 otherwise
  int counterClockwiseButtonValue = digitalRead(counterClockwiseButton); // 1 when pushed, 0 otherwise
//  Serial.println(counterClockwiseButtonValue);
  
  controls = {
    rightValue, // Right
    leftValue, // Left
    upValue, // Up
    downValue, // Down
    clockwiseButtonValue == 1, // Clockwise
    counterClockwiseButtonValue == 1, // Counter clockwise
    false, // Rotate 180
    false, // Hold
  };
  
  if (gameOver && !gameOverDrawn) {
    clearBottom();
    gameOverDrawn = true;
    
    // G
    matrix.drawPixel(1, 0, CYAN);
    matrix.drawPixel(2, 0, CYAN);
    matrix.drawPixel(3, 0, CYAN);
    matrix.drawPixel(4, 0, CYAN);
    matrix.drawPixel(1, 1, CYAN);
    matrix.drawPixel(4, 1, CYAN);
    matrix.drawPixel(1, 2, CYAN);
    matrix.drawPixel(4, 2, CYAN);
    matrix.drawPixel(1, 3, CYAN);
    matrix.drawPixel(2, 3, CYAN);
    matrix.drawPixel(4, 3, CYAN);

    // A
    matrix.drawPixel(1, 4, VIOLET);
    matrix.drawPixel(2, 4, VIOLET);
    matrix.drawPixel(3, 4, VIOLET);
    matrix.drawPixel(4, 4, VIOLET);

    matrix.drawPixel(2, 5, VIOLET);
    matrix.drawPixel(4, 5, VIOLET);
    
    matrix.drawPixel(2, 6, VIOLET);
    matrix.drawPixel(4, 6, VIOLET);

    matrix.drawPixel(1, 7, VIOLET);
    matrix.drawPixel(2, 7, VIOLET);
    matrix.drawPixel(3, 7, VIOLET);
    matrix.drawPixel(4, 7, VIOLET);
    

    // M
    matrix.drawPixel(1, 8, RED);
    matrix.drawPixel(2, 8, RED);
    matrix.drawPixel(3, 8, RED);
    matrix.drawPixel(4, 8, RED);
    matrix.drawPixel(4, 9, RED);
    matrix.drawPixel(3, 10, RED);
    matrix.drawPixel(4, 10, RED);

    matrix.drawPixel(1, 11, RED);
    matrix.drawPixel(2, 11, RED);
    matrix.drawPixel(3, 11, RED);
    matrix.drawPixel(4, 11, RED);

    // E
    matrix.drawPixel(1, 12, YELLOW);
    matrix.drawPixel(2, 12, YELLOW);
    matrix.drawPixel(3, 12, YELLOW);
    matrix.drawPixel(4, 12, YELLOW);

    matrix.drawPixel(1, 13, YELLOW);
    matrix.drawPixel(3, 13, YELLOW);
    matrix.drawPixel(4, 13, YELLOW);
    
    matrix.drawPixel(1, 14, YELLOW);
    matrix.drawPixel(3, 14, YELLOW);
    matrix.drawPixel(4, 14, YELLOW);

    matrix.drawPixel(1, 15, YELLOW);
    matrix.drawPixel(4, 15, YELLOW);

    // O
    matrix.drawPixel(1, 16, GREEN);
    matrix.drawPixel(2, 16, GREEN);
    matrix.drawPixel(3, 16, GREEN);
    matrix.drawPixel(4, 16, GREEN);

    matrix.drawPixel(1, 17, GREEN);
    matrix.drawPixel(4, 17, GREEN);
    matrix.drawPixel(1, 18, GREEN);
    matrix.drawPixel(4, 18, GREEN);

    matrix.drawPixel(1, 19, GREEN);
    matrix.drawPixel(2, 19, GREEN);
    matrix.drawPixel(3, 19, GREEN);
    matrix.drawPixel(4, 19, GREEN);

    // V
    matrix.drawPixel(2, 20, ORANGE);
    matrix.drawPixel(3, 20, ORANGE);
    matrix.drawPixel(4, 20, ORANGE);

    matrix.drawPixel(1, 21, ORANGE);
    matrix.drawPixel(1, 22, ORANGE);

    matrix.drawPixel(2, 23, ORANGE);
    matrix.drawPixel(3, 23, ORANGE);
    matrix.drawPixel(4, 23, ORANGE);

    //E
    matrix.drawPixel(1, 24, BLUE);
    matrix.drawPixel(2, 24, BLUE);
    matrix.drawPixel(3, 24, BLUE);
    matrix.drawPixel(4, 24, BLUE);

    matrix.drawPixel(1, 25, BLUE);
    matrix.drawPixel(3, 25, BLUE);
    matrix.drawPixel(4, 25, BLUE);
    
    matrix.drawPixel(1, 26, BLUE);
    matrix.drawPixel(3, 26, BLUE);
    matrix.drawPixel(4, 26, BLUE);

    matrix.drawPixel(1, 27, BLUE);
    matrix.drawPixel(4, 27, BLUE);

    // R
    matrix.drawPixel(1, 28, CYAN);
    matrix.drawPixel(2, 28, CYAN);
    matrix.drawPixel(3, 28, CYAN);
    matrix.drawPixel(4, 28, CYAN);

    matrix.drawPixel(2, 29, CYAN);
    matrix.drawPixel(4, 29, CYAN);

    matrix.drawPixel(1, 30, CYAN);
    matrix.drawPixel(2, 30, CYAN);
    matrix.drawPixel(4, 30, CYAN);

    matrix.drawPixel(2, 31, CYAN);
    matrix.drawPixel(3, 31, CYAN);
    matrix.drawPixel(4, 31, CYAN);
  }
  
  if (gameOver) {
//    Serial.println("Game over");
    firstIteration = true;
    if (joySwReleased) {
      joySwReleased = false;
      gameOver = false;
    }
    if (millis() - gameOverAt > timeBeforeSleep) {
      // Go into a "sleep mode" after a few minutes to save energy
      clearMatrix();
    }
  }
//  gameOver = true;
  if (!gameOver) {
     gameOverDrawn = false;
     if 
     (firstIteration) {
        tetrisEngine.prepareNewGame();
        // Draw border and a placeholder for score
        matrix.drawLine(6, 0, matrix.width()-1, 0, matrix.Color333(2, 2, 2));
        matrix.drawLine(6, 0, 6, matrix.height() - 1, matrix.Color333(2, 2, 2));
        matrix.drawLine(6, matrix.height()-1, matrix.width()-1, matrix.height() - 1, matrix.Color333(2, 2, 2));

        clearBottom();
     }
    gameOver = tetrisEngine.loop(controls);
    if (gameOver) {
      gameOverAt = millis();
    }

    // Print board
    if (tetrisEngine.drawAllThisIteration) {
      for(int y = BUFFER_ZONE_HEIGHT; y < tetrisEngine.fieldHeight; y++) {
        for(int x = 0; x < tetrisEngine.fieldWidth; x++) {
          int currentNum = tetrisEngine.matrixRepresentation[y*tetrisEngine.fieldWidth + x];
  
          // Only draw if the value has changed or if it's the first iteration.
  //        if (!firstIteration && currentNum == matrixRepresentationCopy[y*tetrisEngine.fieldWidth + x]) {
  //          continue;
  //        }
  
          int currentColorInd = currentNum == CURRENT_PIECE_CHAR ? tetrisEngine.currentPiece.symbolNum : currentNum;
          int currentColor = colorMap[currentColorInd];
          
          if (currentColor != 1) { // Don't draw borders
            drawSquareNew(x, y, currentColor, 3);
          }// else {
  //          matrix.drawPixel(0, 0, matrix.Color333(7, 7, 7));
  //        }
        }
      }
      
      // Print next piece
      clearBottom();
      for (int i = 0; i < 3; i++) {
        Tetromino* nextPiece = tetrisEngine.bag.getFuturePiece(i + 1);
        for (int y = 0; y < nextPiece -> dimension; y++) {
          for (int x = 0; x < nextPiece -> dimension; x++) {
  
            tetrisEngine.bag.getFuturePiece(1);
            if (nextPiece -> orientations[0][y][x] == 1) {
              drawSquareNew(x + 12 - 5*i, y + tetrisEngine.fieldHeight - 1, colorMap[nextPiece -> symbolNum], 2);
            }
          }
        }
      }
      
    } else if (tetrisEngine.drawThisIteration) {
//      return;
//      Serial.println("GOT HERE");
      for (int i = 0; i < INDICES_TO_DRAW_LENGTH && tetrisEngine.indicesToDraw[i] != -1; i++) {
         int indexToDraw = tetrisEngine.indicesToDraw[i];
         int x = indexToDraw % tetrisEngine.fieldWidth;
//         Serial.print("X: ");
//         Serial.println(x);
         int y = (indexToDraw - x) / tetrisEngine.fieldWidth;
//         Serial.print("Y: ");
//         Serial.println(y);
          
         int currentNum = tetrisEngine.matrixRepresentation[indexToDraw];
         int currentColorInd = currentNum == CURRENT_PIECE_CHAR ? tetrisEngine.currentPiece.symbolNum : currentNum;
         int currentColor = colorMap[currentColorInd];
         drawSquareNew(x, y, currentColor, 3);
      }
    }
//    tetrisEngine.loop(controls);
    
    firstIteration = false;
//    for(int y = BUFFER_ZONE_HEIGHT; y < tetrisEngine.fieldHeight; y++) {
////      Serial.println("HERE");
////      delay(5000);
//      for(int x = 1; x < tetrisEngine.fieldWidth; x++) {
//        matrixRepresentationCopy[y*tetrisEngine.fieldWidth + x] = tetrisEngine.matrixRepresentation[y*tetrisEngine.fieldWidth + x];
//      }
//    }
    
//    Serial.println("NEW ITERATION");  
  }

  ////  if (gameOver) {
//    oneLoop = false;
////    Serial.println("");
//    gameOver = tetrisEngine.loop(controls);
////    delay(1000);
//  }
//  //  Serial.println();
}
