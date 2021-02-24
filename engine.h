/* NOTE TO SELF: Run with `g++ -std=c++11 -o engineprog engine.cpp && ./engineprog` */
/* Compile with debugger with `g++ -std=c++11 -g -o engineprog engine.cpp` */
/* Debug with `sudo gdb ./engineprog` */

#include <math.h>
#include "piece.h"
#include "controller.h"
using namespace std;

#include <stdio.h>

const int LOCK_DOWN_TIMER = 500;
const bool DEBUG = true;

const int MAIN_MATRIX_HEIGHT = 20;
const int MAIN_MATRIX_WIDTH = 10;
const int BUFFER_ZONE_HEIGHT = 20;
const int BORDER_SIZE = 1;
const int CURRENT_PIECE_CHAR = 2;

float getSpeedInMillisecondsByLevel(int level)
{
  int lastLevel = level - 1;
  float timeInMilliseconds = pow((0.8 - (lastLevel*0.007)), lastLevel) * 1000;
  return timeInMilliseconds;
}

class TetrisEngine {
  public:
    PieceBag bag;
    GameController gameController = GameController(0);
    Tetromino* currentPiece;

    int currentTime = millis();
    bool firstIteration = true;

    // Piece creation
    bool justLocked = false;

    // Score
    int score = 0;
    int currentLevel = 8;

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

    unsigned char *matrixRepresentation = nullptr;
    int pastCoordinates[4] = {-1, -1, -1, -1};

    void drawPieceOnBoard() {
      /* char pieceSym = currentPiece -> getSymbol(); */
      if (justLocked) {
        if (pastCoordinates[0] != -1) {
          for (int i = 0; i < 4; i++) {
            matrixRepresentation[pastCoordinates[i]] = 1; // TODO: Replace with better symbol
            pastCoordinates[i] = -1;
          }
        }
        return;
      }

      // Undraw past coordinates for the current piece, if it has them.
      if (pastCoordinates[0] != -1) {
        for (int i = 0; i < 4; i++) {
          matrixRepresentation[pastCoordinates[i]] = 0;
        }
      }

      // Draw new coordinates and save them as past coordinates.
      int currentDimension = currentPiece -> getDimension();
      int pieceOrientation[4][4];
      currentPiece -> getOrientation(orientation, pieceOrientation);

      int whichPastCoord = 0;
      for (int y = 0; y < currentDimension; y++) {
        for (int x = 0; x < currentDimension; x++) {
          int minoRepresentation = pieceOrientation[y][x];
          if (minoRepresentation == 1) {
            matrixRepresentation[(y + currentY)*fieldWidth + (x + currentX)] = CURRENT_PIECE_CHAR;
            pastCoordinates[whichPastCoord] = (y + currentY)*fieldWidth + (x + currentX);
            whichPastCoord++;
          }
        }
      }

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
        Serial.print("\n");
      }
    }

    void generation() {
      if (DEBUG) {
        Serial.println("GENERATING");
      }
      currentPiece = bag.getNextPiece();
      orientation = 0;
      currentX = 4;
      currentY = 40 - 21;
      lockDownTimerMs = LOCK_DOWN_TIMER;
      lockDownMaxY = -100;
      lockingDownAt = -1;
    }

    bool isCollisionDetected(int newX, int newY, int newOrientation) {
      bool collisionDetected = false;
      int currentDimension = currentPiece -> getDimension();


      int newPieceOrientation[4][4];
      currentPiece -> getOrientation(newOrientation, newPieceOrientation);

      /* Serial.println(newX); */
      /* Serial.println(newY); */
      for (int y = 0; y < currentDimension; y++) {
        for (int x = 0; x < currentDimension; x++) {
          /* int oldMinoRepresentation = currentPieceOrientation[y][x]; */
          int newMinoRepresentation = newPieceOrientation[y][x];
          //  Check for 0 so we're not comparing squares that are occupied by the current piece
          int thisY = (y + newY)*fieldWidth;
          int thisX = x + newX;
          int matrixMino = matrixRepresentation[thisY + thisX];

          if (matrixMino != 0 && matrixMino != CURRENT_PIECE_CHAR && newMinoRepresentation == 1) {
            // This is a part of the matrix that is actually occupied
            if (DEBUG) {
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

    void rotateAndMove() {
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
        int potentialNewOrientation = (orientation - 1) % 4;
        if (!isCollisionDetected(currentX, currentY, potentialNewOrientation)) {
          orientation = potentialNewOrientation;
        }
      }
    }

    bool shouldPieceLock() {
      return lockDownTimerMs <= 0;
    }

    void handlePieceLock() {
      justLocked = true;
    }

    bool shouldPieceTryToFall() {
      return currentTime - lastDrop > dropAfter;
    }

    void handlePieceTryToFall() {
      bool collisionDetected = isCollisionDetected(currentX, currentY + 1, orientation);
      if (!collisionDetected) {
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

      // Update global state
      currentTime = millis();
      Serial.print("LOCK: ");
      Serial.println(lockDownTimerMs);

      if (justLocked || firstIteration) {
        generation();
        justLocked = false;
        firstIteration = false;
      }

      updateLockDownTimer();

      gameController.updateControls(controls, currentTime);
      rotateAndMove();

      if (shouldPieceLock()) {
        handlePieceLock();
      } else if (shouldPieceTryToFall()) {
        Serial.println("FALLING");
        handlePieceTryToFall();
      }

      drawPieceOnBoard();

      if (DEBUG) {
        Serial.print("Piece: ");
        char pieceSym = currentPiece -> getSymbol();
        Serial.println(pieceSym);
        Serial.print("X: ");
        Serial.println(currentX);
        Serial.print("Y: ");
        Serial.println(currentY);
        renderForCli();
      }

      // input // TODO: Handle input
      // update state
      // render?

      return true;
    }

    TetrisEngine() {
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
