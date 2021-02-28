#include "engine.h"
#include "draw.h"

// Random seed
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
int newGamePushed = false;
int newGameReleased = false;
long long highScore = 0;


void loop() {
  int upValue = digitalRead(upButton);
  int leftValue = digitalRead(leftButton);
  int rightValue = digitalRead(rightButton);
  int downValue = digitalRead(downButton);
  int holdValue = digitalRead(holdButton);
  
  int clockwiseButtonValue = digitalRead(clockwiseButton); // 1 when pushed, 0 otherwise
  int counterClockwiseButtonValue = digitalRead(counterClockwiseButton); // 1 when pushed, 0 otherwise

  if (!newGamePushed) {
    newGamePushed = counterClockwiseButtonValue == 1;
  }

  if (gameOver && newGamePushed && counterClockwiseButtonValue == 0) {
    newGameReleased = true;
    newGamePushed = false;
  }

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
    gameOverDrawn = true;

    int wordHeightOffset = 6; // Height of font + 1 space + 1 to set next draw location
    int currentY = 18;
    int lineOffset = 1;
    int highestY = currentY + lineOffset*4 + wordHeightOffset*5;

    newDrawLine(0, currentY - 1, matrix.height() - 1, currentY - 1, matrix.Color333(2, 2, 2));
    newDrawLine(0, highestY, matrix.height() - 1, highestY, matrix.Color333(2, 2, 2));
    newFillRect(0, currentY, matrix.height(), highestY - currentY, matrix.Color333(0, 0, 0));

    currentY += lineOffset;

    drawNumber(&highScore, currentY);
    currentY += wordHeightOffset;

    drawHigh(currentY);
    currentY += wordHeightOffset + lineOffset;

    drawNumber(&tetrisEngine.score, currentY);
    currentY += wordHeightOffset;

    drawScore(currentY);
    currentY += wordHeightOffset + lineOffset;


    drawGameOver(currentY);
    /* currentY += wordHeightOffset; */

    /* newDrawLine(0, currentY, matrix.height() - 1, currentY, matrix.Color333(2, 2, 2)); */
  }

  /* if (gameOver && !gameOverDrawn) { */
  /*   highScore = tetrisEngine.score > highScore ? tetrisEngine.score : highScore; */
  /*   gameOverDrawn = true; */

  /*   clearBottom(); // TODO: Move game over and delete */
  /*   drawGameOver(0); */
  /*   int scoreHeight = 32; */
  /*   int scoreTitleOffset = 7; */
  /*   int currentY = 32; */
  /*   drawNumber(&tetrisEngine.score, scoreHeight + 1); */
  /*   newFillRect(0, scoreHeight, matrix.height(), 7 + 6, matrix.Color333(0, 0, 0)); */
  /*   newDrawLine(0, scoreHeight + scoreTitleOffset + 6, matrix.height() - 1, scoreHeight + scoreTitleOffset + 6, matrix.Color333(2, 2, 2)); */
  /*   newDrawLine(0, scoreHeight - 1, matrix.height() - 1, scoreHeight - 1, matrix.Color333(2, 2, 2)); */
  /*   drawScore(scoreHeight + scoreTitleOffset); */
  /* } */

  if (gameOver) {
    firstIteration = true;
    if (newGameReleased) {
      newGameReleased = false;
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

    firstIteration = false;
  }
}
