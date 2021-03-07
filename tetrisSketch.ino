#include "engine.h"
#include "draw.h"

// Random seed
#define seed A9

/* // Buttons */
#define upButton 23
#define rightButton 22
#define downButton 2
#define leftButton 3
#define counterClockwiseButton 4
#define flipButton 6
#define clockwiseButton 5
#define holdButton 8
#define startButton 12
#define selectButton 13

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
  pinMode(flipButton, INPUT);
  pinMode(startButton, INPUT);
  pinMode(selectButton, INPUT);
}

bool firstIteration = true;
unsigned long gameOverAt;
long timeBeforeSleep = 60000;

int ghostInds[4][2] = {{-1, -1}, {-1, -1}, {-1, -1}, {-1, -1}};


struct Controls controls;
int newGamePushed = false;
int newGameReleased = false;

int ghostSettingsPushed = false;
int ghostSettingsReleased = false;

long long highScore = 0;
bool shouldDrawGhost = true;
bool shouldDrawPiece = true;

void drawGhost() {
  // Undraw the last ghost
  if (!shouldDrawGhost) {
    return;
  }

  if (tetrisEngine.rowsRemovedThisIteration != 0) {
    for (int i = 0; i < 4; i++) {
      drawSquareNew(ghostInds[i][0], ghostInds[i][1], matrix.Color333(0, 0, 0), 3);
      ghostInds[i][0] = -1;
      ghostInds[i][1] = -1;
    }
    return;
  }

  int ghostColor = matrix.Color333(1, 1, 1);
  switch (tetrisEngine.currentPiece -> symbol) {
    case 'J':
      ghostColor = matrix.Color333(0, 0, 1);
      break;
    case 'O':
      ghostColor = matrix.Color333(2, 1, 0);
      break;
    case 'Z':
      ghostColor = matrix.Color333(1, 0, 0);
      break;
    case 'S':
      ghostColor = matrix.Color333(0, 1, 0);
      break;
    case 'I':
      ghostColor = matrix.Color333(0, 1, 2);
      break;
    case 'T':
      ghostColor = matrix.Color333(1, 0, 2);
      break;
    case 'L':
      ghostColor = matrix.Color333(2, 1, 0);
      break;
  }

  int newGhostInds[4][2] = {{-1, -1}, {-1, -1}, {-1, -1}, {-1, -1}};

  // Draw the ghost
  int currentGhostInd = 0;
  int currentGhostY = tetrisEngine.getYModifierAfterHardDrop() + tetrisEngine.currentY;
  for (int y = 0; y < tetrisEngine.currentPiece -> dimension; y++) {
    for (int x = 0; x < tetrisEngine.currentPiece -> dimension; x++) {
      int minoRepresentation = tetrisEngine.currentPiece -> orientations[tetrisEngine.orientation][y][x];
      if (minoRepresentation == 1) {
        if (tetrisEngine.matrixRepresentation[(currentGhostY + y)*tetrisEngine.fieldWidth + (x + tetrisEngine.currentX)] == 0) {
          drawSquareNew(x + tetrisEngine.currentX, currentGhostY + y, ghostColor, 3);
          newGhostInds[currentGhostInd][0] = x + tetrisEngine.currentX;
          newGhostInds[currentGhostInd][1] = currentGhostY + y;
        } else {
          newGhostInds[currentGhostInd][0] = -1;
          newGhostInds[currentGhostInd][1] = -1;
        }
        currentGhostInd++;
      }
    }
  }

  for (int i = 0; i < 4; i++) {
    bool shouldDelete = true;
    for (int j = 0; j < 4; j++) {
      if (ghostInds[i][0] == newGhostInds[j][0] && ghostInds[i][1] == newGhostInds[j][1]) {
        shouldDelete = false;
      }
    }

    if (ghostInds[i][0] != -1 && shouldDelete) {
       if (tetrisEngine.matrixRepresentation[ghostInds[i][1]*tetrisEngine.fieldWidth + ghostInds[i][0]] == 0) {
         drawSquareNew(ghostInds[i][0], ghostInds[i][1], matrix.Color333(0, 0, 0), 3);
       }
       ghostInds[i][0] = -1;
       ghostInds[i][1] = -1;
    }
  }


  // Copy over
  for (int i = 0; i < 4; i++) {
    ghostInds[i][0] = newGhostInds[i][0];
    ghostInds[i][1] = newGhostInds[i][1];
  }
}



// Print next pieces
void printNextPieces() {
  drawGhost();
  clearNextPieces();
  for (int i = 0; i < 5; i++) {
    Tetromino* nextPiece = tetrisEngine.bag.getFuturePiece(i + 1);

    for (int y = 0; y < nextPiece -> dimension; y++) {
      for (int x = 0; x < nextPiece -> dimension; x++) {
        if (nextPiece -> orientations[0][y][x] == 1) {
          int adjustedX = x + matrix.height() - 5 - 5*i;
          newDrawPixel(adjustedX, 1-y, colorMap[nextPiece -> symbolNum]);
          /* int xOffset = 5; */
          /* if (nextPiece -> symbolNum == 8) */
          /*   xOffset = -1; */
          /* drawSquareNew(x + 13 - 4*i, y + tetrisEngine.fieldHeight - 1, colorMap[nextPiece -> symbolNum], 1, xOffset); */
        /* } */
        }
      }
    }
  }
}

void printWholeBoard() {
  for(int y = BUFFER_ZONE_HEIGHT; y < tetrisEngine.fieldHeight; y++) {
    for(int x = 0; x < tetrisEngine.fieldWidth; x++) {
      int currentNum = tetrisEngine.matrixRepresentation[y*tetrisEngine.fieldWidth + x];


      int currentColorInd = currentNum;
      if (currentNum == CURRENT_PIECE_CHAR) {
        if (shouldDrawPiece || tetrisEngine.gameController.dropPressed || firstIteration) {
          currentColorInd = tetrisEngine.currentPiece -> symbolNum;
        } else {
          // Draw black if we don't want to draw the current piece
          currentColorInd = 0;
        }
      }
      /* int currentColorInd = currentNum == CURRENT_PIECE_CHAR ? tetrisEngine.currentPiece -> symbolNum : currentNum; */
      int currentColor = colorMap[currentColorInd];
      
      if (currentColor != 1) { // Don't draw borders
        drawSquareNew(x, y, currentColor, 3);
      }// else {
  //          matrix.drawPixel(0, 0, matrix.Color333(7, 7, 7));
  //        }
    }
  }
}

void loop() {
  int upValue = digitalRead(upButton);
  int leftValue = digitalRead(leftButton);
  int rightValue = digitalRead(rightButton);
  int downValue = digitalRead(downButton);
  int holdValue = digitalRead(holdButton);
  int flipValue = digitalRead(flipButton);
  int startValue = digitalRead(startButton);
  int selectValue = digitalRead(selectButton);

  int clockwiseButtonValue = digitalRead(clockwiseButton); // 1 when pushed, 0 otherwise
  int counterClockwiseButtonValue = digitalRead(counterClockwiseButton); // 1 when pushed, 0 otherwise

  if (gameOver && !ghostSettingsPushed && flipValue == 1) {
    ghostSettingsPushed = true;
  }

  if (gameOver && ghostSettingsPushed && flipValue == 0) {
    ghostSettingsPushed = false;
    ghostSettingsReleased = true;
  }

  if (gameOver && !newGamePushed && counterClockwiseButtonValue == 1) {
    newGamePushed = true;
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
    flipValue, // Rotate 180
    holdValue, // Hold
    selectValue, // Select
    startValue, // Start
  };

  if (gameOver && !gameOverDrawn) {
    highScore = tetrisEngine.score > highScore ? tetrisEngine.score : highScore;
    gameOverDrawn = true;

    int wordHeightOffset = 6; // Height of font + 1 space + 1 to set next draw location
    int currentY = 18;
    int lineOffset = 1;
    int highestY = currentY + lineOffset*4 + wordHeightOffset*5;

    newFillRect(0, currentY, matrix.height(), highestY - currentY, matrix.Color333(0, 0, 0));
    newDrawLine(0, currentY - 1, matrix.height() - 1, currentY - 1, matrix.Color333(2, 2, 2));
    newDrawLine(0, highestY-1, matrix.height() - 1, highestY-1, matrix.Color333(2, 2, 2));

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

    if (!shouldDrawGhost) {
      newDrawPixel(matrix.height()-1, matrix.width()-1, matrix.Color333(7, 0, 3));
    }
    /* currentY += wordHeightOffset; */

    /* newDrawLine(0, currentY, matrix.height() - 1, currentY, matrix.Color333(2, 2, 2)); */
  }

  if (gameOver) {
    firstIteration = true;
    if (newGameReleased) {
      newGameReleased = false;
      gameOver = false;
    } else if (ghostSettingsReleased) {
      ghostSettingsReleased = false;
      shouldDrawGhost = !shouldDrawGhost;
      shouldDrawPiece = !shouldDrawPiece;

      if (!shouldDrawGhost) {
        newDrawPixel(matrix.height()-1, matrix.width()-1, matrix.Color333(7, 0, 3));
      } else {
        newDrawPixel(matrix.height()-1, matrix.width()-1, matrix.Color333(2, 2, 2));
      }
    }
    if (millis() - gameOverAt > timeBeforeSleep) {
      // Go into a "sleep mode" after some time
      clearMatrix();
    }
  }

  if (!gameOver) {
     gameOverDrawn = false;
     if 
     (firstIteration) {
        tetrisEngine.prepareNewGame(shouldDrawPiece);

        // Draw border
        newDrawLine(0, 4, 0, matrix.width()-1, matrix.Color333(2, 2, 2));
        matrix.drawLine(3, 0, 3, matrix.height() - 1, matrix.Color333(2, 2, 2));
        matrix.drawLine(3, matrix.height()-1, matrix.width()-1, matrix.height() - 1, matrix.Color333(2, 2, 2));

        clearBottom();

        // Draw line separating new pieces from help piece display
        newDrawLine(5, 0, 5, 2, matrix.Color333(2, 2, 2));
        /* matrix.drawLine(0, 8, 6, 8, matrix.Color333(2, 2, 2)); */
     }
    gameOver = tetrisEngine.loop(controls);
    if (gameOver) {
      gameOverAt = millis();
    }

    if (tetrisEngine.generationThisIteration || (tetrisEngine.pieceHeldThisIteration && !tetrisEngine.pieceHeldThisGame)) {
      printNextPieces();
    }

    if (tetrisEngine.pieceHeldThisIteration) {
      clearHeldPiece();
      Tetromino* heldPiece = tetrisEngine.heldPiece;
      /* int xOffset = heldPiece -> symbolNum == 5 ? 1 : 2; */
      for (int y = 0; y < heldPiece -> dimension; y++) {
        for (int x = 0; x < heldPiece -> dimension; x++) {
          if (heldPiece -> orientations[0][y][x] == 1) {
            newDrawPixel(x+1, 1-y, colorMap[heldPiece -> symbolNum]);
            /* drawSquareNew(x, y + tetrisEngine.fieldHeight - 1, colorMap[heldPiece -> symbolNum], 2, xOffset); */
          }
        }
      }

      /* if (!tetrisEngine.pieceHeldThisGame) { */
      /*   // We had to generate a new piece since this is the first hold of the game, */
      /*   // so we need to redraw held pieces. */
      /*   printNextPieces(); */
      /* } */
    }

    // Print board
    if (tetrisEngine.drawAllThisIteration) {
      printWholeBoard();
      /* for(int y = BUFFER_ZONE_HEIGHT; y < tetrisEngine.fieldHeight; y++) { */
      /*   for(int x = 0; x < tetrisEngine.fieldWidth; x++) { */
      /*     int currentNum = tetrisEngine.matrixRepresentation[y*tetrisEngine.fieldWidth + x]; */

  
      /*     int currentColorInd = currentNum; */
      /*     if (currentNum == CURRENT_PIECE_CHAR) { */
      /*       if (shouldDrawPiece || tetrisEngine.gameController.dropPressed || firstIteration) { */
      /*         currentColorInd = tetrisEngine.currentPiece -> symbolNum; */
      /*       } else { */
      /*         // Draw black if we don't want to draw the current piece */
      /*         currentColorInd = 0; */
      /*       } */
      /*     } */
      /*     /1* int currentColorInd = currentNum == CURRENT_PIECE_CHAR ? tetrisEngine.currentPiece -> symbolNum : currentNum; *1/ */
      /*     int currentColor = colorMap[currentColorInd]; */
          
      /*     if (currentColor != 1) { // Don't draw borders */
      /*       drawSquareNew(x, y, currentColor, 3); */
      /*     }// else { */
  /* //          matrix.drawPixel(0, 0, matrix.Color333(7, 7, 7)); */
  /* //        } */
      /*   } */
      /* } */
      drawGhost();
    } else if (tetrisEngine.drawThisIteration) {

      // Draw the new piece area
      for (int i = 0; i < INDICES_TO_DRAW_LENGTH && tetrisEngine.indicesToDraw[i] != -1; i++) {
         int indexToDraw = tetrisEngine.indicesToDraw[i];
         int x = indexToDraw % tetrisEngine.fieldWidth;
         int y = (indexToDraw - x) / tetrisEngine.fieldWidth;
          
         int currentNum = tetrisEngine.matrixRepresentation[indexToDraw];
         int currentColorInd = currentNum;
         if (currentNum == CURRENT_PIECE_CHAR) {
           if (shouldDrawPiece || tetrisEngine.gameController.dropPressed || firstIteration) {
             currentColorInd = tetrisEngine.currentPiece -> symbolNum;
           } else {
             // Draw black if we don't want to draw the current piece
             currentColorInd = 0;
           }
         }

         /* int currentColorInd = currentNum == CURRENT_PIECE_CHAR ? tetrisEngine.currentPiece -> symbolNum : currentNum; */
         /* int currentColor = colorMap[currentColorInd]; */
         /* int currentColorInd = currentNum == CURRENT_PIECE_CHAR ? tetrisEngine.currentPiece -> symbolNum : currentNum; */
         int currentColor = colorMap[currentColorInd];
         drawSquareNew(x, y, currentColor, 3);
      }

      drawGhost();
    }

    firstIteration = false;
  }
}
