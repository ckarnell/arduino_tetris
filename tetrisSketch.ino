#include "engine.h"
#include <RGBmatrixPanel.h>

// Joystick
#define joySw 49
#define joyY A7
#define joyX A8
#define seed A9

// Buttons
#define counterClockwiseButton 45
#define clockwiseButton 47

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

int adjustYCoord(int yCoord) {
  // We want y = 20 to appear at 60 on the led board,
  // but we want y = 40 to appear at 0 on the led board,
  // a linear function.
  // f(BUFFER_HEIGHT) = 60
  // f(24) = 0
  int result = (MAIN_MATRIX_HEIGHT+BUFFER_ZONE_HEIGHT+BORDER_SIZE)*SQUARE_WIDTH; // Start with the max height
  result -= yCoord*SQUARE_WIDTH;
  return result;
}

void drawSquareNew(int xCoord, int yCoord, int color) {
  int adjustedYCoord = adjustYCoord(yCoord) - SQUARE_WIDTH + PIXEL_OFFSET_Y; // Subtract 3 since the origin is seen as lower left instead of top left
//  if (adjustedYCoord > matrix.height()) {
//    // Nothing to draw if we're outside of the boord
//    return;
//  }
  int adjustedXCoord = ((xCoord - 1) * 3) + PIXEL_OFFSET_X;
//  int adjustedY = yCoord;
//  Serial.print("Adj Y: ");
//  Serial.println(adjustedYCoord);
//  Serial.print("Adj X: ");
//  Serial.println(adjustedXCoord);
  matrix.fillRect(adjustedYCoord, adjustedXCoord, SQUARE_WIDTH, SQUARE_WIDTH, color);
//  Serial.println("DELAYING");
//  delay(500);
}

//void drawSquare(int xCoord, int yCoord, int color) {
//  matrix.fillRect(yCoord, xCoord, SQUARE_WIDTH, SQUARE_WIDTH, color);
//}

const int colorMap[9] = {0, 1, VIOLET, GREEN, RED, CYAN, ORANGE, BLUE, YELLOW};

bool gameOver = false;

void setup() {
  matrix.begin();
  Serial.begin(9875);
//  randomSeed(analogRead(3));
  randomSeed(analogRead(seed));
  pinMode(clockwiseButton, INPUT);
  pinMode(counterClockwiseButton, INPUT);
  pinMode(joySw, INPUT);
}

bool firstIteration = true;
unsigned char *matrixRepresentationCopy = nullptr;
//int iterations = 100;

struct Controls controls;

void loop() {
  int xValue = analogRead(joyX); // > 766 = right, <256 = left
  int yValue = analogRead(joyY); // 511 by default, >766 = down, <100 = up
  int swValue = digitalRead(joySw); // 0 when pushed, random otherwise
  int clockwiseButtonValue = digitalRead(clockwiseButton); // 1 when pushed, 0 otherwise
  int counterClockwiseButtonValue = digitalRead(counterClockwiseButton); // 1 when pushed, 0 otherwise
  
  controls = {
    yValue < 256, // Right
    yValue > 766, // Left
    xValue < 100, // Up
    xValue > 766, // Down
    clockwiseButtonValue == 1, // Clockwise
    counterClockwiseButtonValue == 1, // Counter clockwise
    false, // Rotate 180
    false, // Hold
  };

  if (gameOver) {
//    Serial.println("Game over");
    firstIteration = true;
  }
//  gameOver = true;
  if (!gameOver) {
     if (firstIteration) {
        // Draw border and a placeholder for score
        matrix.drawLine(6, 0, matrix.width()-1, 0, matrix.Color333(2, 2, 2));
        matrix.drawLine(6, 0, 6, matrix.height() - 1, matrix.Color333(2, 2, 2));
        matrix.drawLine(6, matrix.height()-1, matrix.width()-1, matrix.height() - 1, matrix.Color333(2, 2, 2));
        
     }
//    iterations--;
    gameOver = tetrisEngine.loop(controls);

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
            drawSquareNew(x, y, currentColor);
          }// else {
  //          matrix.drawPixel(0, 0, matrix.Color333(7, 7, 7));
  //        }
        }
      }
    } else if (tetrisEngine.drawThisIteration) {
//      return;
      Serial.println("GOT HERE");
      for (int i = 0; i < INDICES_TO_DRAW_LENGTH && tetrisEngine.indicesToDraw[i] != -1; i++) {
         int indexToDraw = tetrisEngine.indicesToDraw[i];
         int x = indexToDraw % tetrisEngine.fieldWidth;
         Serial.print("X: ");
         Serial.println(x);
         int y = (indexToDraw - x) / tetrisEngine.fieldWidth;
         Serial.print("Y: ");
         Serial.println(y);
          
         int currentNum = tetrisEngine.matrixRepresentation[indexToDraw];
         int currentColorInd = currentNum == CURRENT_PIECE_CHAR ? tetrisEngine.currentPiece.symbolNum : currentNum;
         int currentColor = colorMap[currentColorInd];
         drawSquareNew(x, y, currentColor);
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
