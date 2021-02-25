/* NOTE TO SELF: Run with `g++ -std=c++11 -o engineprog engine.cpp && ./engineprog` */
/* Compile with debugger with `g++ -std=c++11 -g -o engineprog engine.cpp` */
/* Debug with `sudo gdb ./engineprog` */

#include <math.h>
#include "piece.h"
#include "controller.h"
/* using namespace std; */

#include <stdio.h>

const int LOCK_DOWN_TIMER = 2000;
const bool DEBUG = false;
const int INDICES_TO_DRAW_LENGTH = 10;

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
    bool drawAllThisIteration = false;
    bool drawThisIteration = false;

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
    int indicesToDraw[INDICES_TO_DRAW_LENGTH] = {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1};
    int indForIndicesToDraw = 0;
    int pastCoordinates[4] = {-1, -1, -1, -1};
    // Use this to compute drop locations faster
    /* int lowestOccupiedYValues[12] = {-100, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, -100}; */

    void drawPieceOnBoard() {
      int currentDimension = currentPiece.dimension;
      /* int pieceOrientation[4][4]; */
      /* currentPiece -> getOrientation(orientation, pieceOrientation); */

      /* char pieceSym = currentPiece -> getSymbol(); */
      /* if (justLocked) { */
      /*   for (int y = currentDimension - 1; y >= 0; y--) { */
      /*     for (int x = 0; x < currentDimension; x++) { */
      /*       int minoRepresentation = currentPiece.orientations[orientation][y][x]; */
      /*       if (minoRepresentation == 1) { */
      /*         lowestOccupiedYValues[x + currentX] = y + currentY; */
      /*       } */
      /*     } */
      /*   } */
      /* } */

      // Undraw past coordinates for the current piece, if it has them.
      if (pastCoordinates[0] != -1) {
        for (int i = 0; i < 4; i++) {
          matrixRepresentation[pastCoordinates[i]] = 0;
          /* addIndexToDraw(pastCoordinates[i]); */
        }
      }

      // Draw new coordinates and save them as past coordinates.
      int pastCoordInd = 0;
      int newPastCoordinates[4] = {-1, -1, -1, -1};
      for (int y = 0; y < currentDimension; y++) {
        for (int x = 0; x < currentDimension; x++) {
          int minoRepresentation = currentPiece.orientations[orientation][y][x];
          if (minoRepresentation == 1) {
            int charToDraw = justLocked ? currentPiece.symbolNum : CURRENT_PIECE_CHAR;
            matrixRepresentation[(y + currentY)*fieldWidth + (x + currentX)] = charToDraw;
            /* addIndexToDraw((y + currentY)*fieldWidth + (x + currentX)); */
            int pastCoord = justLocked ? -1 : (y + currentY)*fieldWidth + (x + currentX);
            newPastCoordinates[pastCoordInd] = pastCoord;
            pastCoordInd++;
          }
        }
      }

      // Mark the diff between what was on the board before and what is on the board now for printing
      if (pastCoordinates[0] != -1) {
        for (int i = 0; i < 4; i++) {
          bool pastCoordNeedsDrawing = true;
          for (int j = 0; j < 4; j++) {
            if (pastCoordinates[i] == newPastCoordinates[j])
              pastCoordNeedsDrawing = false;
          }
          if (pastCoordNeedsDrawing) 
            addIndexToDraw(pastCoordinates[i]);
        }

        for (int i = 0; i < 4; i++) {
          bool newCoordNeedsDrawing = true;
          for (int j = 0; j < 4; j++) {
            if (newPastCoordinates[i] == pastCoordinates[j])
              newCoordNeedsDrawing = false;
          }
          if (newCoordNeedsDrawing) 
            addIndexToDraw(newPastCoordinates[i]);
        }
      }

      // Finally, copy over
      for (int i = 0; i < 4; i++) {
        pastCoordinates[i] = newPastCoordinates[i];
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
      drawThisIteration = true;
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
      }

      return collisionDetected;
    }

    void removeRows() {
      // Undraw rows queued for removal
      int numRowsBeingRemoved = 0;
      for (int i = 0; i < 4; i++) {
        if (rowsToRemove[i] != -1) {
          /* Serial.println("ABOUT TO REMOVE"); */
          numRowsBeingRemoved++;
          for (int y = rowsToRemove[i]; y >= 0; y--) {
            /* Serial.print("Y: "); */
            /* Serial.println(y); */
            for (int x = BORDER_SIZE; x < fieldWidth - BORDER_SIZE; x++) {
              int indToEdit = y*fieldWidth + x;
              if (y == 0) {
                matrixRepresentation[indToEdit] = 0;
              } else {
                /* Serial.print("X: "); */
                /* Serial.println(x); */
                matrixRepresentation[indToEdit] = matrixRepresentation[(y - 1)*fieldWidth + x];
              }
              /* } */
            }
          }
        }
        rowsToRemove[i] = -1;
      }

      if (numRowsBeingRemoved != 0) {
        /* Serial.print("NUMBER REMOVED: "); */
        Serial.println(numRowsBeingRemoved);
        drawAllThisIteration = true;
      }

      /* for (int i = 0; i < numRowsBeingRemoved; i++) { */
      /*   for (int y = fieldHeight - BORDER_SIZE - 1; y > 4; y--) { */
      /*     for (int x = BORDER_SIZE; x < fieldWidth - BORDER_SIZE; x++) { */
      /*       /1* int minoToWrite = y > numRowsBeingRemoved ? matrixRepresentation[(y - 1)*(fieldWidth) + x] : 0; *1/ */
      /*       int minoToWrite = matrixRepresentation[(y - 1)*(fieldWidth) + x]; */
      /*       matrixRepresentation[y*fieldWidth + x] = minoToWrite; */
      /*     } */
      /*   } */
      /* } */

/*       // Draw all lines down the number of rows that were removed */
/*       for (int y = fieldHeight - 1 - BORDER_SIZE; y > 0; y--) { */
/*         for (int x = BORDER_SIZE; x < fieldWidth - BORDER_SIZE; x++) { */
/*           int minoToWrite = y > numRowsBeingRemoved ? matrixRepresentation[(y - numRowsBeingRemoved)*(fieldWidth) + x] : 0; */
/*           matrixRepresentation[y*fieldWidth + x] = minoToWrite; */
/*         } */
/*       } */

      /* for (int j = 1; j < fieldWidth - 1; j++) { */
      /*   if (lowestOccupiedYValues[j] != fieldHeightWithoutBorder) { */
      /*     lowestOccupiedYValues[j] -= numRowsBeingRemoved; */
      /*   } */
      /* } */
    }

    void queueRowsForRemoval() {
      /* Serial.println("ABOUT TO QUEUE"); */
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

        if (queueThisRow) {
          /* Serial.print("QUEUED: "); */
          /* Serial.println(y); */
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
        bool foundFittingPosition = false;
        int indexOfWorkingRotation = 0;
        int potentialNewOrientation = (orientation + 1) % 4;

        // Iterate through the possible ways we can rotate the piece until we find one or don't
        for (int i = 0; i < 4 && !foundFittingPosition; i++) {
          /* cout << possibleRotations[i][0] << possibleRotations[i][1] << endl; */
          /* , currentX + currentPiece.possibleRotations[i][0], currentY + currentPiece.possibleRotations[i][1] */
          foundFittingPosition = !isCollisionDetected(
              currentX + currentPiece.offsets[potentialNewOrientation][1][i][0],
              currentY + currentPiece.offsets[potentialNewOrientation][1][i][1],
              potentialNewOrientation
          );

          if (foundFittingPosition) {
            indexOfWorkingRotation = i;
          }
        }

        if (foundFittingPosition) {
          orientation = potentialNewOrientation;
          currentX += currentPiece.offsets[potentialNewOrientation][1][indexOfWorkingRotation][0];
          currentY += currentPiece.offsets[potentialNewOrientation][1][indexOfWorkingRotation][1];
        }
      }

      if (gameController.counterClockwisePressed) {
        bool foundFittingPosition = false;
        int indexOfWorkingRotation = 0;

        int potentialNewOrientation = orientation - 1 < 0 ? 3 : orientation - 1;
        if (potentialNewOrientation < 0) {
          potentialNewOrientation = 3;
        }

        // Iterate through the possible ways we can rotate the piece until we find one or don't
        for (int i = 0; i < 4 && !foundFittingPosition; i++) {
          /* cout << possibleRotations[i][0] << possibleRotations[i][1] << endl; */
          /* , currentX + currentPiece.possibleRotations[i][0], currentY + currentPiece.possibleRotations[i][1] */
          foundFittingPosition = !isCollisionDetected(
              currentX + currentPiece.offsets[potentialNewOrientation][0][i][0],
              currentY + currentPiece.offsets[potentialNewOrientation][0][i][1],
              potentialNewOrientation
          );

          if (foundFittingPosition) {
            indexOfWorkingRotation = i;
          }
        }

        if (foundFittingPosition) {
          orientation = potentialNewOrientation;
          currentX += currentPiece.offsets[potentialNewOrientation][0][indexOfWorkingRotation][0];
          currentY += currentPiece.offsets[potentialNewOrientation][0][indexOfWorkingRotation][1];
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
      return gameController.downHeld || currentTime - lastDrop > dropAfter;
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
      drawAllThisIteration = firstIteration;
      drawThisIteration = false;
      indForIndicesToDraw = 0;
      for (int x = 0; x < 10; x++)
        indicesToDraw[x] = -1;

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
        drawAllThisIteration = true;
      }

      updateLockDownTimer();

      gameController.updateControls(controls, currentTime);
      rotateAndMove();

      if (shouldPieceLock()) {
        justLocked = true;

        // TODO: Handle score update
        // The game could be over if we just locked a piece
        drawAllThisIteration = true;
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

        // There may be a game over at this point
        gameOver = isLockedOut();
        if (gameOver) {
          return gameOver;
        }
      }

      if (shouldDebugPrint()) {
        /* for (int i = 0; i < 12; i++) { */
        /*   Serial.print(lowestOccupiedYValues[i]); */
        /*   Serial.print(" "); */
        /* } */
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

    void createNewPlayField() {
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

    void prepareNewGame() {
      gameOver = false;
      createNewPlayField();
      firstIteration = true;
      justLocked = false;
      bag.createNewBagOrder(true);

      score = 0;
      currentLevel = 1;
    }

    TetrisEngine() {
      random();
      bag = PieceBag();

      // Create initial board representation, which is empty except for boarders.
      // Note that this includes a "buffer zone" above the main viewable board,
      // which won't be rendered, but is important for game state management.
      matrixRepresentation = new unsigned char[fieldWidth*fieldHeight]; // Create play field buffer
      /* prepareNewGame(); */
      /* for (int y = 0; y < fieldHeight; y++) { */
      /*   for (int x = 0; x < fieldWidth; x++) { // Board Boundary */
      /*     // "1" represents a border, "0" is anything else. */
      /*     matrixRepresentation[y*fieldWidth + x] = 0; */
      /*     if (x == 0) { */
      /*       // This is the first element of the line, it's a border character. */
      /*       matrixRepresentation[y*fieldWidth + x] = 1; */
      /*     } else if (x == fieldWidth - 1) { */
      /*       // This is the last element of the line, it's a border character. */
      /*       matrixRepresentation[y*fieldWidth + x] = 1; */
      /*     } else if (y == fieldHeight - 1) { */
      /*       // This is the last row of the board, every character on this row is a border character. */
      /*       matrixRepresentation[y*fieldWidth + x] = 1; */
      /*     } */
      /*   } */
      /* } */
    }
};
