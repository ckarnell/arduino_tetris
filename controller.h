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
    int drop;
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
      drop = false;
      // -1 is a sentinal value here, implying the button isn't pushed at all
      leftPressedAt = -1;
      rightPressedAt = -1;
    }

    void updateControls(Controls controls, int currentTime) {
      if (controls.left) {
        if (!leftPressed) {
          leftPressed = true;
          leftPressedAt = currentTime;
        } else {
          // Left was not pressed this iteration, set value to false
          leftPressed = false;

          // That means this was held - check for das
          if (currentTime - leftPressedAt > _dasDelay) {
            leftDas = true;
          }
        }
      } else {
        leftDas = false;
        leftPressed = false;
        leftPressedAt = -1;
      }

      if (controls.right) {
        if (!rightPressed) {
          rightPressed = true;
          rightPressedAt = currentTime;
        } else {
          // right was not pressed this iteration, set value to false
          rightPressed = false;

          // That means this was held - check for das
          if (currentTime - rightPressedAt > _dasDelay) {
            rightDas = true;
          }
        }
      } else {
        rightDas = false;
        rightPressed = false;
        rightPressedAt = -1;
      }

      clockwisePressed = controls.rotateClockwise;
      /* if (controls.rotateClockwise) { */
      /*   if (!clockwisePressed) { */
      /*     clockwisePressed = true; */
      /*     clockwiseHeld = true; */
      /*   } else { */
      /*     clockwisePressed = false; */
      /*   } */
      /* } else { */
      /*   clockwisePressed = false; */
      /*   clockwiseHeld = false; */
      /* } */

      if (controls.rotateCounterClockwise) {
        if (!counterClockwisePressed) {
          counterClockwisePressed = true;
          counterClockwiseHeld = true;
        } else {
          counterClockwisePressed = false;
        }
      } else {
        counterClockwisePressed = false;
        counterClockwiseHeld = false;
      }
    }

  private:
    int _currentTime;
    int leftPressedAt;
    int rightPressedAt;
    int clockwiseHeld;
    int counterClockwiseHeld;
    bool dropPressedAt = -1;
    int _dasDelay;
};
