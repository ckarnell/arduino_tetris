#include "engine.h"
#include "draw.h"

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
#define holdButton 53

TetrisEngine tetrisEngine = TetrisEngine();

const int colorMap[10] = {0, 1, VIOLET, GREEN, RED, CYAN, ORANGE, BLUE, YELLOW, WHITE};

bool gameOver = false;
bool gameOverDrawn = false;

void setup() {
  matrix.begin();
  Serial.begin(9875);
  randomSeed(analogRead(seed));
  pinMode(clockwiseButton, INPUT);
  pinMode(counterClockwiseButton, INPUT);
//  pinMode(joySw, INPUT);
  pinMode(upButton, INPUT);
  pinMode(leftButton, INPUT);
  pinMode(rightButton, INPUT);
  pinMode(downButton, INPUT);
  pinMode(holdButton, INPUT);
}

bool firstIteration = true;
unsigned char *matrixRepresentationCopy = nullptr;
long gameOverAt;
long timeBeforeSleep = 60000;


struct Controls controls;
int joySwPushed = false;
int joySwReleased = false;
long long highScore = 0;


void loop() {
  int swValue = digitalRead(counterClockwiseButton); // 0 when pushed, random otherwise
  if (gameOver && joySwPushed && !swValue) {
    joySwReleased = true;
  }
  int upValue = digitalRead(upButton);
  int leftValue = digitalRead(leftButton);
  int rightValue = digitalRead(rightButton);
  int downValue = digitalRead(downButton);
  int holdValue = digitalRead(holdButton);
  joySwPushed = swValue;
  
  int clockwiseButtonValue = digitalRead(clockwiseButton); // 1 when pushed, 0 otherwise
  int counterClockwiseButtonValue = digitalRead(counterClockwiseButton); // 1 when pushed, 0 otherwise
  
  controls = {
    rightValue, // Right
    leftValue, // Left
    upValue, // Up
    downValue, // Down
    clockwiseButtonValue == 1, // Clockwise
    counterClockwiseButtonValue == 1, // Counter clockwise
    false, // Rotate 180
    holdValue, // Hold
  };
  
  if (gameOver && !gameOverDrawn) {
    highScore = tetrisEngine.score > highScore ? tetrisEngine.score : highScore;
    clearBottom();
    gameOverDrawn = true;
    drawGameOver();
    int scoreHeight = 28;
    int scoreTitleOffset = 7;
    newFillRect(0, scoreHeight, matrix.height(), 7 + 6, matrix.Color333(0, 0, 0));
    newDrawLine(0, scoreHeight + scoreTitleOffset + 6, matrix.height() - 1, scoreHeight + scoreTitleOffset + 6, matrix.Color333(2, 2, 2));
    newDrawLine(0, scoreHeight - 1, matrix.height() - 1, scoreHeight - 1, matrix.Color333(2, 2, 2));
    drawLetter('S', 0, scoreHeight + scoreTitleOffset, matrix.Color333(5, 5, 5));
    drawLetter('C', LETTER_WIDTH + 1, scoreHeight + scoreTitleOffset, matrix.Color333(5, 5, 5));
    drawLetter('O', LETTER_WIDTH*2 + 2, scoreHeight + scoreTitleOffset, matrix.Color333(5, 5, 5));
    drawLetter('R', LETTER_WIDTH*3 + 3, scoreHeight + scoreTitleOffset, matrix.Color333(5, 5, 5));
    drawLetter('E', LETTER_WIDTH*4 + 4, scoreHeight + scoreTitleOffset, matrix.Color333(5, 5, 5));
    drawNumber(&tetrisEngine.score, scoreHeight + 1);
  }
  
  if (gameOver) {
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
  
  if (!gameOver) {
     gameOverDrawn = false;
     if 
     (firstIteration) {
        long long fakeScore = 12345678; // DEBUG
//        translateScoreIntoScoreRep(fakeScore); // DEBUG
        tetrisEngine.prepareNewGame();
        // Draw border and a placeholder for score
        matrix.drawLine(6, 0, matrix.width()-1, 0, matrix.Color333(2, 2, 2));
        matrix.drawLine(6, 0, 6, matrix.height() - 1, matrix.Color333(2, 2, 2));
        matrix.drawLine(6, matrix.height()-1, matrix.width()-1, matrix.height() - 1, matrix.Color333(2, 2, 2));

        clearBottom();
        
        // Draw line separating new pieces from help piece display
        matrix.drawLine(0, 8, 6, 8, matrix.Color333(2, 2, 2));
     }
    gameOver = tetrisEngine.loop(controls);
    if (gameOver) {
      gameOverAt = millis();
    }

    if (tetrisEngine.generationThisIteration) {
      // Print next pieces
      clearNextPieces();
      for (int i = 0; i < 3; i++) {
        Tetromino* nextPiece = tetrisEngine.bag.getFuturePiece(i + 1);
        
        for (int y = 0; y < nextPiece -> dimension; y++) {
          for (int x = 0; x < nextPiece -> dimension; x++) {
            if (nextPiece -> orientations[0][y][x] == 1) {
              int xOffset = nextPiece -> symbolNum == 5 && i == 0 ? -1 : 0;
              if (nextPiece -> symbolNum == 8)
                xOffset = -1;
              drawSquareNew(x + 13 - 4*i, y + tetrisEngine.fieldHeight - 1, colorMap[nextPiece -> symbolNum], 2, xOffset);
            }
          }
        }
      }
    }

    if (tetrisEngine.pieceHeldThisIteration) {
      clearHeldPiece();
      Tetromino* heldPiece = tetrisEngine.heldPiece;
      int xOffset = heldPiece -> symbolNum == 5 ? 1 : 2;
      for (int y = 0; y < heldPiece -> dimension; y++) {
        for (int x = 0; x < heldPiece -> dimension; x++) {
          if (heldPiece -> orientations[0][y][x] == 1) {
            drawSquareNew(x, y + tetrisEngine.fieldHeight - 1, colorMap[heldPiece -> symbolNum], 2, xOffset);
          }
        }
      }
    }

    // Print board
    if (tetrisEngine.drawAllThisIteration) {
      for(int y = BUFFER_ZONE_HEIGHT; y < tetrisEngine.fieldHeight; y++) {
        for(int x = 0; x < tetrisEngine.fieldWidth; x++) {
          int currentNum = tetrisEngine.matrixRepresentation[y*tetrisEngine.fieldWidth + x];

  
          int currentColorInd = currentNum == CURRENT_PIECE_CHAR ? tetrisEngine.currentPiece -> symbolNum : currentNum;
          int currentColor = colorMap[currentColorInd];
          
          if (currentColor != 1) { // Don't draw borders
            drawSquareNew(x, y, currentColor, 3);
          }// else {
  //          matrix.drawPixel(0, 0, matrix.Color333(7, 7, 7));
  //        }
        }
      }
      
      
    } else if (tetrisEngine.drawThisIteration) {
      // Draw the ghost
      
      // Draw the new piece area
      for (int i = 0; i < INDICES_TO_DRAW_LENGTH && tetrisEngine.indicesToDraw[i] != -1; i++) {
         int indexToDraw = tetrisEngine.indicesToDraw[i];
         int x = indexToDraw % tetrisEngine.fieldWidth;
         int y = (indexToDraw - x) / tetrisEngine.fieldWidth;
          
         int currentNum = tetrisEngine.matrixRepresentation[indexToDraw];
         int currentColorInd = currentNum == CURRENT_PIECE_CHAR ? tetrisEngine.currentPiece -> symbolNum : currentNum;
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
