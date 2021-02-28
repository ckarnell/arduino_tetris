#include <RGBmatrixPanel.h>

// LED Matrix
#define CLK 11 // USE THIS ON ARDUINO MEGA
#define OE   9
#define LAT 10
#define A   A0
#define B   A1
#define C   A2
#define D   A3

const int SQUARE_WIDTH = 3;
const int PIXEL_OFFSET_X = 1;
const int PIXEL_OFFSET_Y = 4;
const int SCORE_DIGITS = 8;

RGBmatrixPanel matrix(A, B, C, D, CLK, LAT, OE, false, 64);

const int CYAN   = matrix.Color333(0,4,7); // 1183
const int VIOLET = matrix.Color333(4,0,7); // -28641
const int RED    = matrix.Color333(7,0,0); // -2048
const int YELLOW = matrix.Color333(7,4,0); // -896
const int GREEN  = matrix.Color333(0,7,0); // 2016
const int ORANGE = matrix.Color333(7,2,0); // -1472
const int BLUE   = matrix.Color333(0,0,7);
const int WHITE  = matrix.Color333(1, 1, 1);
const int BLACK  = matrix.Color333(0, 0, 0);
const int DIM = matrix.Color333(1, 1, 7);
const int BRIGHT = matrix.Color333(7, 1, 1);

int adjustYCoord(int yCoord, int multiplier) {
  // We want y = 20 to appear at 60 on the led board,
  // but we want y = 40 to appear at 0 on the led board,
  // a linear function.
  // f(BUFFER_HEIGHT) = 60
  // f(24) = 0
  int result = (MAIN_MATRIX_HEIGHT+BUFFER_ZONE_HEIGHT+BORDER_SIZE)*multiplier; // Start with the max height
  result -= yCoord*multiplier;
  return result;
}

void drawSquareNew(int xCoord, int yCoord, int color, int multiplier, int xOffset = 0) {
//  int adjustedYCoord = adjustYCoord(yCoord, multiplier) - multiplier + PIXEL_OFFSET_Y; // Subtract 3 since the origin is seen as lower left instead of top left
  int adjustedYCoord = adjustYCoord(yCoord, multiplier) - SQUARE_WIDTH + PIXEL_OFFSET_Y; // Subtract 3 since the origin is seen as lower left instead of top left
//  if (adjustedYCoord > matrix.height()) {
//    // Nothing to draw if we're outside of the boord
//    return;
//  }
  int adjustedXCoord = ((xCoord - 1) * multiplier) + PIXEL_OFFSET_X + xOffset;
  matrix.fillRect(adjustedYCoord, adjustedXCoord, multiplier, multiplier, color);
}

void clearBottom() {
  matrix.fillRect(0, 0, 6, matrix.height(), matrix.Color333(0, 0, 0));
}

void clearNextPieces() {
  matrix.fillRect(0, 9, 6, matrix.height(), matrix.Color333(0, 0, 0));
}

void clearHeldPiece() {
  matrix.fillRect(0, 0, 6, 8, matrix.Color333(0, 0, 0));
}

void clearMatrix() {
  matrix.fillRect(0, 0, matrix.width(), matrix.height(), matrix.Color333(0, 0, 0));
}

int scoreRep[SCORE_DIGITS] = {0, 0, 0, 0, 0, 0, 0, 0};

void translateScoreIntoScoreRep(long long score) {
  Serial.print("Score: ");
  for (int i = 0; i < SCORE_DIGITS; i++) {
    int currentDigitInd = SCORE_DIGITS - 1 - i;
    int divideBy = pow(10, i);
    score /= divideBy;
    scoreRep[i] = score % 10;
    Serial.print(scoreRep[i]);
//    long long currentDigitValue = score % divideBy;
  }
  Serial.println("");
}

/* void newFillRect(int startX, int startY, int endX, int endY, int color) { */
/*     matrix.fillRect(startY, startX, endY, endX, color); */
/* } */
void newFillRect(int startX, int startY, int width, int height, int color) {
    matrix.fillRect(startY, startX, height, width, color);
}

void newDrawLine(int startX, int startY, int endX, int endY, int color) {
    matrix.drawLine(startY, startX, endY, endX, color);
}

void newDrawPixel(int x, int y, int color) {
  matrix.drawPixel(y, x, color);
}


void drawDigit(int digit, int startingX, int startingY, int color) {
  // Font from here: https://fontstruct.com/fontstructions/show/1422505/5x4-pxl
  // Starting X is from the left and starting Y is from the bottom
  switch (digit) {
    case 0:
      //0000
      //00 0
      //00 0
      //00 0
      //0000

      matrix.drawLine(startingY, startingX, startingY, startingX+3, color);
      matrix.drawLine(startingY+4, startingX, startingY+4, startingX+3, color);

      matrix.drawLine(startingY+1, startingX, startingY+3, startingX, color);
      matrix.drawLine(startingY+1, startingX+1, startingY+3, startingX+1, color);
      matrix.drawLine(startingY+1, startingX+3, startingY+3, startingX+3, color);

      break;
    case 1:
      //1111
      // 111
      // 111
      // 111
      // 111
      // 111

      newDrawPixel(startingX, startingY+4, color);
      newFillRect(startingX + 1, startingY, 3, 5, color);

      break;
    case 2:
      //2222
      //  22
      //2222
      //22
      //2222

      newDrawLine(startingX, startingY, startingX+3, startingY, color);
      newDrawLine(startingX, startingY+1, startingX+1, startingY+1, color);
      newDrawLine(startingX, startingY+2, startingX+3, startingY+2, color);
      newDrawLine(startingX+2, startingY+3, startingX+3, startingY+3, color);
      newDrawLine(startingX, startingY+4, startingX+3, startingY+4, color);

      break;
    case 3:
      //3333
      //  33
      // 333
      //  33
      //3333

      newDrawLine(startingX, startingY, startingX+3, startingY, color);
      newDrawLine(startingX, startingY+4, startingX+3, startingY+4, color);
      newFillRect(startingX+2, startingY+1, 2, 3, color);
      newDrawPixel(startingX+1, startingY+2, color);
      break;
    case 4:
      //4 44
      //4 44
      //4444
      //  44
      //  44

      newFillRect(startingX+2, startingY, 2, 5, color);
      newDrawLine(startingX, startingY+2, startingX, startingY+4, color);
      newDrawPixel(startingX+1, startingY+2, color);
      break;
    case 5:
      //5555
      //55
      //5555
      //  55
      //5555

      newFillRect(startingX, startingY+2, 2, 3, color);
      newFillRect(startingX+2, startingY, 2, 3, color);
      newDrawLine(startingX, startingY, startingX+1, startingY, color);
      newDrawLine(startingX+2, startingY+4, startingX+3, startingY+4, color);
      break;
    case 6:
      //6666
      //66
      //6666
      //66 6
      //6666

      newFillRect(startingX, startingY, 2, 5, color);
      newDrawLine(startingX+2, startingY, startingX+3, startingY, color);
      newDrawLine(startingX+2, startingY+2, startingX+3, startingY+2, color);
      newDrawLine(startingX+2, startingY+4, startingX+3, startingY+4, color);
      newDrawPixel(startingX+3, startingY+1, color);
      break;
    case 7:
      //7777
      //  77
      //  77
      //  77
      //  77

      newFillRect(startingX+2, startingY, 2, 5, color);
      newDrawLine(startingX, startingY+4, startingX+3, startingY+4, color);
      break;
    case 8:
      //8888
      //88 8
      //8888
      //88 8
      //8888

      newFillRect(startingX, startingY, 2, 5, color);
      newDrawLine(startingX+2, startingY, startingX+3, startingY, color);
      newDrawPixel(startingX+3, startingY+1, color);
      newDrawLine(startingX+2, startingY+2, startingX+3, startingY+2, color);
      newDrawPixel(startingX+3, startingY+3, color);
      newDrawLine(startingX+2, startingY+4, startingX+3, startingY+4, color);
      break;
    case 9:
      //9999
      //9 99
      //9999
      //  99
      //9999

      newFillRect(startingX+2, startingY, 2, 5, color);
      newDrawLine(startingX, startingY, startingX+1, startingY, color);
      newDrawLine(startingX, startingY+2, startingX+1, startingY+2, color);
      newDrawPixel(startingX, startingY+3, color);
      newDrawLine(startingX, startingY+4, startingX+1, startingY+4, color);
      break;
  }
}

const int LETTER_WIDTH = 4;

void drawLetter(char letter, int startingX, int startingY, int color) {
  switch (letter) {
    case 'A':
      //AAAA
      //A  A
      //AAAA
      //A  A
      //A  A

      newDrawLine(startingX, startingY, startingX, startingY+4, color);
      newDrawLine(startingX+3, startingY, startingX+3, startingY+4, color);
      newDrawLine(startingX+1, startingY+2, startingX+2, startingY+2, color);
      newDrawLine(startingX+1, startingY+4, startingX+2, startingY+4, color);
      break;
    case 'C':
      //CCCC
      //C
      //C
      //C
      //CCCC

      newDrawLine(startingX, startingY, startingX+3, startingY, color);
      newDrawLine(startingX, startingY+1, startingX, startingY+3, color);
      newDrawLine(startingX, startingY+4, startingX+3, startingY+4, color);
      break;
    case 'E':
      //EEEE
      //E
      //EEEE
      //E
      //EEEE

      newDrawLine(startingX, startingY, startingX+3, startingY, color);
      newDrawLine(startingX, startingY+2, startingX+3, startingY+2, color);
      newDrawLine(startingX, startingY+4, startingX+3, startingY+4, color);
      newDrawPixel(startingX, startingY+1, color);
      newDrawPixel(startingX, startingY+3, color);
      break;
    case 'G':
      //GGGG
      //G
      //G GG
      //G  G
      //GGGG

      newDrawLine(startingX, startingY, startingX+3, startingY, color);
      newDrawLine(startingX, startingY+1, startingX, startingY+3, color);
      newDrawLine(startingX+3, startingY+1, startingX+3, startingY+2, color);
      newDrawPixel(startingX+2, startingY+2, color);
      newDrawLine(startingX, startingY+4, startingX+3, startingY+4, color);
      break;
    case 'M':
      //MMMM
      //MMMM
      //M  M
      //M  M
      //M  M

      newFillRect(startingX, startingY+3, 4, 2, color);
      newDrawLine(startingX, startingY, startingX, startingY+2, color);
      newDrawLine(startingX+3, startingY, startingX+3, startingY+2, color);
      break;
    case 'O':
      //OOOO
      //O  O
      //O  O
      //O  O
      //OOOO

      newDrawLine(startingX, startingY, startingX+3, startingY, color);
      newDrawLine(startingX, startingY+4, startingX+3, startingY+4, color);
      newDrawLine(startingX, startingY+1, startingX, startingY+3, color);
      newDrawLine(startingX+3, startingY+1, startingX+3, startingY+3, color);
      break;
    case 'R':
      //RRR
      //R  R
      //RRR
      //R  R
      //R  R

      newDrawLine(startingX, startingY, startingX, startingY+4, color);
      newDrawLine(startingX+1, startingY+2, startingX+2, startingY+2, color);
      newDrawLine(startingX+1, startingY+4, startingX+2, startingY+4, color);
      newDrawLine(startingX+3, startingY, startingX+3, startingY+1, color);
      newDrawPixel(startingX+3, startingY+3, color);
      break;
    case 'S':
      //SSSS
      //S
      //SSSS
      //   S
      //SSSS

      newDrawLine(startingX, startingY, startingX+3, startingY, color);
      newDrawLine(startingX, startingY+2, startingX+3, startingY+2, color);
      newDrawLine(startingX, startingY+4, startingX+3, startingY+4, color);
      newDrawPixel(startingX+3, startingY+1, color);
      newDrawPixel(startingX, startingY+3, color);
      break;
    case 'V':
      //V  V
      //V  V
      //V  V
      //V V
      // V

      newDrawLine(startingX, startingY+1, startingX, startingY+4, color);
      newDrawPixel(startingX+1, startingY, color);
      newDrawPixel(startingX+2, startingY+1, color);
      newDrawLine(startingX+3, startingY+2, startingX+3, startingY+4, color);
      break;
  }
}

void drawGameOver() {
  drawLetter('G', 0, 0, CYAN);
  drawLetter('A', LETTER_WIDTH, 0, VIOLET);
  drawLetter('M', LETTER_WIDTH*2, 0, RED);
  drawLetter('E', LETTER_WIDTH*3, 0, YELLOW);
  drawLetter('O', LETTER_WIDTH*4, 0, GREEN);
  drawLetter('V', LETTER_WIDTH*5, 0, ORANGE);
  drawLetter('E', LETTER_WIDTH*6, 0, BLUE);
  drawLetter('R', LETTER_WIDTH*7, 0, CYAN);
}

void drawNumber(long long* num, int y) {
  int startX = matrix.height() - 4;
  int color = BRIGHT;
  int numToDraw;

  while (startX >= 0) {
    numToDraw = *num % 10;
    Serial.print("Drawing num: ");
    Serial.println(numToDraw);
    color = color == DIM ? BRIGHT : DIM;
    drawDigit(numToDraw, startX, y, color);
    *num /= 10;
    startX -= 4;
  }
}
