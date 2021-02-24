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
    bool clockwisePressed = false;
    bool counterClockwisePressed = false;

    GameController(int dasDelay) {
      _dasDelay = dasDelay;
      _currentTime = millis();

      // "Pressed" means pressed THIS ITERATION - i.e. newly pressed
      leftPressed = false;
      rightPressed = false;
      leftDas = false;
      rightDas = false;
      dropPressed = false;
      // -1 is a sentinal value here, implying the button isn't pushed at all
      leftPressedAt = -1;
      rightPressedAt = -1;
    }

    void updateControls(Controls controls, int currentTime) {
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
            leftDas = true;
          }
        }
      } else {
        leftDas = false;
        leftPressed = false;
        leftPressedAt = -1;
      }

      if (controls.right) {
        if (leftPressedAt == -1) {
          // It was pressed this iteration
          rightPressed = true;
          rightPressedAt = currentTime;
        } else {
          // Right was not pressed this iteration, set value to false
          rightPressed = false;

          // That means this was held - check for das
          if (rightPressedAt != -1 && currentTime - rightPressedAt > _dasDelay) {
            rightDas = true;
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
    }

  private:
    int _currentTime;
    int leftPressedAt;
    int rightPressedAt;
    int clockwiseHeld;
    int counterClockwiseHeld;
    bool dropHeld = false;
    int _dasDelay;
};
