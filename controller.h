// Struct of all the actual inputs to the game
struct Controls {
  bool right;
  bool left;
  bool up;
  bool down;
  bool rotateClockwise;
  bool rotateCounterClockwise;
  bool rotate180;
  bool hold;
};

class GameController {
  public:
    bool leftPressed;
    bool rightPressed;
    bool leftDas;
    bool rightDas;
    int dropPressed;
    int downHeld;
    bool clockwisePressed = false;
    bool holdPressed = false;
    bool counterClockwisePressed = false;

    GameController(int dasDelay, int dasSpeed, int dropSpeed) {
      _dasDelay = dasDelay;
      _dasSpeed = dasSpeed;
      _currentTime = millis();
      _dropSpeed = dropSpeed;

      // "Pressed" means pressed THIS ITERATION - i.e. newly pressed
      leftPressed = false;
      rightPressed = false;
      leftDas = false;
      rightDas = false;
      dropPressed = false;
      holdPressed = false;
      // -1 is a sentinal value here, implying the button isn't pushed at all
      leftPressedAt = -1;
      rightPressedAt = -1;
    }

    void updateControls(Controls controls, int currentTime) {
      if (controls.down) {
        if (lastDropAt == -1 || (currentTime - lastDropAt) > _dropSpeed) {
          lastDropAt = currentTime;
          downHeld = true;
        } else {
          downHeld = false;
        }
      } else {
        downHeld = false;
      }

      if (controls.left) {
        if (leftPressedAt == -1) {
          // It was pressed this iteration
          leftPressed = true;
          leftPressedAt = currentTime;
        } else {
          // Left was not pressed this iteration, set value to false
          leftPressed = false;

          // That means this was held - check for das
          if (leftPressedAt != -1 && (currentTime - leftPressedAt > _dasDelay)) {
            // If both right and left are held, but right was pushed later, don't turn
            // on left das
            if (!(rightPressedAt != -1 && rightPressedAt > leftPressedAt)) {
              if (leftDasAt == -1 || (currentTime - leftDasAt) > _dasSpeed) {
                leftDasAt = currentTime;
                leftDas = true;
              } else {
                leftDas = false;
              }
            }
          }
        }
      } else {
        leftDas = false;
        leftPressed = false;
        leftPressedAt = -1;
      }

      if (controls.right) {
        if (rightPressedAt == -1) {
          // It was pressed this iteration
          rightPressed = true;
          rightPressedAt = currentTime;
        } else {
          // Right was not pressed this iteration, set value to false
          rightPressed = false;

          // That means this was held - check for das
          if (rightPressedAt != -1 && (currentTime - rightPressedAt > _dasDelay)) {
            // If both right and left are held, but left was pushed later, don't turn
            // on right das
            if (!(leftPressedAt != -1 && leftPressedAt > rightPressedAt)) {
              if (rightDasAt == -1 || currentTime - rightDasAt > _dasSpeed) {
                rightDasAt = currentTime;
                rightDas = true;
              } else {
                rightDas = false;
              }
            }
          }
        }
      } else {
        rightDas = false;
        rightPressed = false;
        rightPressedAt = -1;
      }

      if (controls.rotateClockwise) {
        if (!clockwiseHeld) {
          clockwisePressed = true;
          clockwiseHeld = true;
        } else {
          clockwisePressed = false;
        }
      } else {
        clockwisePressed = false;
        clockwiseHeld = false;
      }


      if (controls.rotateCounterClockwise) {
        if (!counterClockwiseHeld) {
          counterClockwisePressed = true;
          counterClockwiseHeld = true;
        } else {
          counterClockwisePressed = false;
        }
      } else {
        counterClockwisePressed = false;
        counterClockwiseHeld = false;
      }


      if (controls.up) {
        if (!dropHeld) {
          dropHeld = true;
          dropPressed = true;
        } else {
          dropPressed = false;
        }
      } else {
        dropHeld = false;
        dropPressed = false;
      }

      if (controls.hold) {
        if (!holdHeld) {
          holdPressed = true;
          holdHeld = true;
        } else {
          holdPressed = false;
        }
      } else {
        holdPressed = false;
        holdHeld = false;
      }
    }

  private:
    int _currentTime;
    int leftPressedAt;
    int rightPressedAt;
    int clockwiseHeld;
    int leftDasAt = -1;
    int rightDasAt = -1;
    int holdHeld = false;
    int counterClockwiseHeld;
    bool dropHeld = false;
    int _dasDelay;
    int _dasSpeed;
    int _dropSpeed;
    int lastDropAt = -1;
};
