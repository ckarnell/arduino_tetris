/* NOTE TO SELF: Run with `g++ -std=c++11 -o engineprog engine.cpp && ./engineprog` */
/* Compile with debugger with `g++ -std=c++11 -g -o engineprog engine.cpp` */
/* Debug with `sudo gdb ./engineprog` */

#include <math.h>
#include "piece.h"
#include "controller.h"
/* using namespace std; */

#include <stdio.h>

const int LOCK_DOWN_TIMER = 500;
const bool DEBUG = false;

const int MAIN_MATRIX_HEIGHT = 20;
const int MAIN_MATRIX_WIDTH = 10;
const int BUFFER_ZONE_HEIGHT = 4;
const int BORDER_SIZE = 1;
const int CURRENT_PIECE_CHAR = 9;

float getSpeedInMillisecondsByLevel(int level)
{
  int lastLevel = level - 1;
  float timeInMilliseconds = pow((0.8 - (lastLevel*0.007)), lastLevel) * 1000;
  return timeInMilliseconds;
}


class TetrisEngine {
  public:
    /* PieceBag bag; */
    GameController gameController = GameController(300);
    Tetromino currentPiece;
    PieceBag bag;

    int currentTime = millis();
    int rowsToRemove[4] = {-1, -1, -1, -1};
    int lastDraw = currentTime - 1001;
    bool gameOver = false;
    bool firstIteration = true;

    // Piece creation
    bool justLocked = false;

    // Score
    int score = 0;
    int currentLevel = 1;

    // Drop
    int lastDrop = currentTime;
    float dropAfter = getSpeedInMillisecondsByLevel(currentLevel);

    // Lockdown
    int lockDownTimerMs = LOCK_DOWN_TIMER;
    int lockingDownAt = -1;
    int lockDownMaxY = -100;

    // Field
    int fieldWidth = MAIN_MATRIX_WIDTH + (BORDER_SIZE*2);
    int fieldHeight = MAIN_MATRIX_HEIGHT + BORDER_SIZE + BUFFER_ZONE_HEIGHT;

    // Falling piece state
    int currentX;
    int currentY;
    int orientation = 0;

    // Board state
    unsigned char *matrixRepresentation = nullptr;
    unsigned char *indicesToDraw = nullptr;
    int indForIndicesToDraw = 0;
    int pastCoordinates[4] = {-1, -1, -1, -1};
    // Use this to compute drop locations faster
    int lowestOccupiedYValues[12] = {-100, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, -100};

    void drawPieceOnBoard() {
      int currentDimension = currentPiece.dimension;
      /* int pieceOrientation[4][4]; */
      /* currentPiece -> getOrientation(orientation, pieceOrientation); */

      /* char pieceSym = currentPiece -> getSymbol(); */
      if (justLocked) {
        for (int y = currentDimension - 1; y >= 0; y--) {
          for (int x = 0; x < currentDimension; x++) {
            int minoRepresentation = currentPiece.orientations[orientation][y][x];
            if (minoRepresentation == 1) {
              lowestOccupiedYValues[x + currentX] = y + currentY;
            }
          }
        }
      }

      // Undraw past coordinates for the current piece, if it has them.
      if (pastCoordinates[0] != -1) {
        for (int i = 0; i < 4; i++) {
          matrixRepresentation[pastCoordinates[i]] = 0;
        }
      }

      // Draw new coordinates and save them as past coordinates.
      int whichPastCoord = 0;
      for (int y = 0; y < currentDimension; y++) {
        for (int x = 0; x < currentDimension; x++) {
          int minoRepresentation = currentPiece.orientations[orientation][y][x];
          if (minoRepresentation == 1) {
            int charToDraw = justLocked ? currentPiece.symbolNum : CURRENT_PIECE_CHAR;
            matrixRepresentation[(y + currentY)*fieldWidth + (x + currentX)] = charToDraw;
            int pastCoord = justLocked ? -1 : (y + currentY)*fieldWidth + (x + currentX);
            pastCoordinates[whichPastCoord] = pastCoord;
            whichPastCoord++;
          }
        }
      }

    }

    bool shouldDebugPrint() {
      return (DEBUG && currentTime - lastDraw > 1000);
    }

    void renderForCli() {
      for(int y = BUFFER_ZONE_HEIGHT; y < fieldHeight; y++) {
        /* if (y % MAIN_MATRIX_HEIGHT == 0) { */
        /*   Serial.print("\n"); */
        /* } */

        Serial.print(y);
        Serial.print(" ");
        for(int x = 0; x < fieldWidth; x++) {
          Serial.print(matrixRepresentation[y*fieldWidth + x]);
        }
        Serial.println("");
      }
      Serial.println("");
    }

    void addIndexToDraw(int indToDraw) {
      indicesToDraw[indForIndicesToDraw] = indToDraw;
      indForIndicesToDraw++;
    }

    void generation() {
      if (shouldDebugPrint()) {
        Serial.println("GENERATING");
      }
      currentPiece = *bag.getNextPiece();
      orientation = 0;
      currentX = 4;
      currentY = BUFFER_ZONE_HEIGHT;
      lockDownTimerMs = LOCK_DOWN_TIMER;
      lockDownMaxY = -100;
      lockingDownAt = -1;

      // The game can end now if the newly generated piece overlaps with another one
      gameOver = isBlockedOut();
    }

    bool isCollisionDetected(int newX, int newY, int newOrientation) {
      bool collisionDetected = false;
      int currentDimension = currentPiece.dimension;

      /* int newPieceOrientation[4][4] = currentPiece.orientations[newOrientation]; */
      /* /1* currentPiece -> getOrientation(newOrientation, newPieceOrientation); *1/ */

      for (int y = 0; y < currentDimension; y++) {
        for (int x = 0; x < currentDimension; x++) {
          /* int oldMinoRepresentation = currentPieceOrientation[y][x]; */
          int newMinoRepresentation = currentPiece.orientations[newOrientation][y][x];
          //  Check for 0 so we're not comparing squares that are occupied by the current piece
          int thisY = (y + newY)*fieldWidth;
          int thisX = x + newX;
          int matrixMino = matrixRepresentation[thisY + thisX];

          if (matrixMino != 0 && matrixMino != CURRENT_PIECE_CHAR && newMinoRepresentation == 1) {
            // This is a part of the matrix that is actually occupied
            if (shouldDebugPrint()) {
              Serial.println(y + newY);
              Serial.println(x + newX);
            }
            return true;
          }
        }
        /* Serial.println(""); */
      }

      return collisionDetected;
    }

    void removeRows() {
      // Undraw rows queued for removal
      int numRowsBeingRemoved = 0;
      for (int i = 0; i < 4; i++) {
        if (rowsToRemove[i] != -1) {
          numRowsBeingRemoved++;
          for (int x = BORDER_SIZE; x < fieldWidth - BORDER_SIZE; x++) {
            matrixRepresentation[rowsToRemove[i]*fieldWidth + x] = 0;
          }
        }
        rowsToRemove[i] = -1;
      }

      // Draw all lines down the number of rows that were removed
      for (int y = fieldHeight - 1 - BORDER_SIZE; y > 0; y--) {
        for (int x = BORDER_SIZE; x < fieldWidth - BORDER_SIZE; x++) {
          int minoToWrite = y > numRowsBeingRemoved ? matrixRepresentation[(y - numRowsBeingRemoved)*(fieldWidth) + x] : 0;
          matrixRepresentation[y*fieldWidth + x] = minoToWrite;
        }
      }
    }

    void queueRowsForRemoval() {
      int currentInd = 0;
      for (int y = currentY; y < currentY + currentPiece.dimension; y++) {
        /* Serial.print("Y: "); */
        /* Serial.println(y); */
        bool queueThisRow = true;
        for (int x = BORDER_SIZE; x < fieldWidth - BORDER_SIZE && queueThisRow; x++) {
          /* Serial.print("X: "); */
          /* Serial.println(x); */
          /* Serial.print("Rep: "); */
          /* Serial.println(matrixRepresentation[y*fieldWidth + x]); */
          int minoRep = matrixRepresentation[y*fieldWidth + x];
          if (minoRep == 0 || minoRep == 1) {
            // This slot is empty, so we don't queue this row
            queueThisRow = false;
          }
        }
        Serial.println("");

        if (queueThisRow) {
          rowsToRemove[currentInd] = y;
          currentInd++;
        }
      }
    }

    void rotateAndMove() {
      if (gameController.dropPressed) {
        // Set currentY to the lowest Y below the current piece that is not occupied
        int yToDrop = 0;
        while (!isCollisionDetected(currentX, currentY + yToDrop + 1, orientation)) {
          yToDrop++;
        }
        currentY += yToDrop;
        return;
      }

      if (gameController.leftPressed || gameController.leftDas) {
        if (!isCollisionDetected(currentX - 1, currentY, orientation)) {
          currentX--; // TODO: Use hit detection here
        /* } */
        }
      }

      if (gameController.rightPressed || gameController.rightDas) {
        if (!isCollisionDetected(currentX + 1, currentY, orientation)) {
        /* if (!rightIsHeld) { */
          currentX++; // TODO: Use hit detection here
        /* } */
        }
      }

      if (gameController.clockwisePressed) {
        int potentialNewOrientation = (orientation + 1) % 4;
        if (!isCollisionDetected(currentX, currentY, potentialNewOrientation)) {
          orientation = potentialNewOrientation;
        }
      }

      if (gameController.counterClockwisePressed) {
        int potentialNewOrientation = orientation - 1 < 0 ? 3 : orientation - 1;
        if (potentialNewOrientation < 0) {
          potentialNewOrientation = 3;
        }
        if (!isCollisionDetected(currentX, currentY, potentialNewOrientation)) {
          orientation = potentialNewOrientation;
        }
      }
    }

    bool isBlockedOut() {
      // This is a game over condition that occurs when a newly generated piece overlaps
      // with another piece.
      return isCollisionDetected(currentX, currentY, orientation);
    }

    bool isLockedOut() {
      // This is a game over condition that occurs when a piece locks without any of its
      // squares being in the game field, i.e. they're all in the buffer zone.
      if (!justLocked) {
        return false;
      }

      int currentDimension = currentPiece.dimension;
      /* int currentPieceOrientation[4][4]; */ // TODO: Delete these
      /* currentPiece -> getOrientation(orientation, currentPieceOrientation); */
      /* currentPiece -> getOrientation(orientation, currentPieceOrientation); */

      for (int y = 0; y < currentDimension; y++) {
        for (int x = 0; x < currentDimension; x++) {
          if (currentPiece.orientations[orientation][y][x] == 1) {
            if (currentY + y > BUFFER_ZONE_HEIGHT) {
              // This mino is in the game field, so there's no lockout
              return false;
            }
          }
        }
      }

      return true;
    }

    bool canPieceFall() {
      return !isCollisionDetected(currentX, currentY + 1, orientation);
    }

    bool shouldPieceLock() {
      if (gameController.dropPressed) {
        // Drop is pressed - we need to drop and lock on this iteration
        return true;
      } else if (canPieceFall()) {
        // Don't lock if there's nothing below the current piece - it has to fall
        return false;
      }
      return lockDownTimerMs <= 0;
    }

    bool shouldPieceTryToFall() {
      return currentTime - lastDrop > dropAfter;
    }

    void handlePieceTryToFall() {
      if (canPieceFall()) {
        // No collision, actually drop
        currentY++;
        lastDrop = currentTime;
        lockingDownAt = -1;
        return;
      }

      // We can't move down, so start the lockdown timer
      if (currentY > lockDownMaxY) {
        // Reset the clock if we fell since the last time the lockdown timer started
        lockDownTimerMs = LOCK_DOWN_TIMER;
      }
      lockingDownAt = currentTime;
      lockDownMaxY = currentY;
    }

    void updateLockDownTimer() {
      if (lockingDownAt != -1) {
        lockDownTimerMs -=  currentTime - lockingDownAt;
        lockingDownAt = currentTime;
      }
    }

    bool loop(Controls controls) {
      // Returns true if the game continues after the loop, false if there's a game over

      indForIndicesToDraw = 0;
      indicesToDraw = nullptr;

      currentTime = millis();
      if (shouldDebugPrint()) {
        Serial.print("LOCK: ");
        Serial.println(lockDownTimerMs);
      }

      if (justLocked || firstIteration) {
        generation();
        if (gameOver) {
          return gameOver;
        }
        justLocked = false;
        firstIteration = false;
      }

      updateLockDownTimer();

      gameController.updateControls(controls, currentTime);
      rotateAndMove();

      if (shouldPieceLock()) {
        justLocked = true;

        // TODO: Handle line removal. REMEMBER TO CHANGE lowestOccupiedYValues
        // TODO: Handle score update
        // The game could be over if we just locked a piece
        gameOver = isLockedOut();
        if (gameOver) {
          return gameOver;
        }
      } else if (shouldPieceTryToFall()) {
        if (shouldDebugPrint()) {
          Serial.println("FALLING");
        }
        handlePieceTryToFall();
      }

      drawPieceOnBoard();

      if (justLocked) {
        queueRowsForRemoval();
        removeRows();
      }

      if (shouldDebugPrint()) {
        for (int i = 0; i < 12; i++) {
          Serial.print(lowestOccupiedYValues[i]);
          Serial.print(" ");
        }
        Serial.println("");

        Serial.print("Piece: ");
        char pieceSym = currentPiece.symbolNum;
        Serial.println(pieceSym);
        Serial.print("X: ");
        Serial.println(currentX);
        Serial.print("Y: ");
        Serial.println(currentY);
        renderForCli();
      }

      if (shouldDebugPrint()) {
        lastDraw = currentTime; // For CLI debugging
      }
      return gameOver;
    }

    TetrisEngine() {
      random();
      bag = PieceBag();

      // Create initial board representation, which is empty except for boarders.
      // Note that this includes a "buffer zone" above the main viewable board,
      // which won't be rendered, but is important for game state management.
      matrixRepresentation = new unsigned char[fieldWidth*fieldHeight]; // Create play field buffer
      for (int y = 0; y < fieldHeight; y++) {
        for (int x = 0; x < fieldWidth; x++) { // Board Boundary
          // "1" represents a border, "0" is anything else.
          matrixRepresentation[y*fieldWidth + x] = 0;
          if (x == 0) {
            // This is the first element of the line, it's a border character.
            matrixRepresentation[y*fieldWidth + x] = 1;
          } else if (x == fieldWidth - 1) {
            // This is the last element of the line, it's a border character.
            matrixRepresentation[y*fieldWidth + x] = 1;
          } else if (y == fieldHeight - 1) {
            // This is the last row of the board, every character on this row is a border character.
            matrixRepresentation[y*fieldWidth + x] = 1;
          }
        }
      }
    }
};
