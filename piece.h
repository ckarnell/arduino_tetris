#ifndef MYFILE_H_
#define MYFILE_H_

/* #include <Vector.h> */
/* #include <string> */
/* #include <random> */
using namespace std;


struct TetrominoStruct {
  int symbolNum;
  int orientations[4][4][4];
  int offsets[4][2][5][2];
};

struct TetrominoStruct TPieceStruct = {
  64 + 20,
  {
    {
        { 0, 1, 0},
        { 1, 1, 1},
        { 0, 0, 0},
    },
    {
        { 0, 1, 0},
        { 0, 1, 1},
        { 0, 1, 0},
    },
    {
        { 0, 0, 0},
        { 1, 1, 1},
        { 0, 1, 0},
    },
    {
        { 0, 1, 0},
        { 1, 1, 0},
        { 0, 1, 0},
    }
  },
  {
    // North
    {
      // Counter clockwise
      {
        {0, 0}, {1, 0}, {1, -1}, {1, 2}, {0, 0},
      },
      // Clockwise
      {
        {0, 0}, {-1, 0}, {-1, -1}, {-1, 2}, {0, 0},
      },
    },
    // East
    {
      // Counter clockwise
      {
        {0, 0}, {1, 0}, {1, 1}, {0, -2}, {1, -2},
      },
      // Clockwise
      {
        {0, 0}, {1, 0}, {1, 1}, {0, -2}, {1, -2},
      },
    },
    // South
    {
      // Counter clockwise
      {
        {0, 0}, {-1, 0}, {0, 2}, {-1, 2}, {0, 0},
      },
      // Clockwise
      {
        {0, 0}, {1, 0}, {0, 2}, {1, 2}, {0, 0},
      },
    },
    // West
    {
      // Counter clockwise
      {
        {0, 0}, {-1, 0}, {-1, 1}, {0, -2}, {-1, -2},
      },
      // Clockwise
      {
        {0, 0}, {-1, 0}, {-1, 1}, {0, -2}, {-1, -2},
      },
    },
  },
};

class Tetromino {
  public:
    /* int coordinates[][2] = { */
    /*   {0, 0}, */
    /*   {0, 0}, */
    /* }; */
    int coordinates[2];
    int dimension = 3;

    virtual int getDimension() {
      return dimension;
    }

    int orientation = 0;
    int symbolNum = 65;

    /* String symbol; */
    /* /1* string color; *1/ */

    virtual void rotate(bool clockwise) {
      if (clockwise) {
        orientation++;
      } else {
        orientation--;
      }
    }

    /* virtual int[][] getSrsOffsetsForRotation(bool clockwise, int rotation) { */
    /*   return {{}, {}}; */
    /* } */

    virtual int getSymbol() {
      return symbolNum;
    }

    virtual void getOrientation(int orientation, int mutableResult[4][4]) {
      mutableResult[0][0] = 0;
      mutableResult[0][1] = 0;
      mutableResult[0][2] = 0;

      mutableResult[1][0] = 0;
      mutableResult[1][1] = 0;
      mutableResult[1][2] = 0;

      mutableResult[2][0] = 0;
      mutableResult[2][1] = 0;
      mutableResult[2][2] = 0;
    }

    /* virtual getPieceOrientation(int orientation) { */
    /*   return orientations[orientation]; */
    /* } */
};

class TPiece: public Tetromino {
  public:
    /* wstring symbol = L"T"; */
    /* string color = "purple"; */
    int symbolNum = 64 + 20;
    int dimension = 3;

    int getDimension() {
      return dimension;
    }

    virtual int getSymbol() {
      return symbolNum;
    }

    /* getPieceOrientation(int orientation) { */
    /*   return orientations[orientation]; */
    /* } */

    const int orientations[4][3][3] = {
      {
          { 0, 1, 0},
          { 1, 1, 1},
          { 0, 0, 0},
      },
      {
          { 0, 1, 0},
          { 0, 1, 1},
          { 0, 1, 0},
      },
      {
          { 0, 0, 0},
          { 1, 1, 1},
          { 0, 1, 0},
      },
      {
          { 0, 1, 0},
          { 1, 1, 0},
          { 0, 1, 0},
      }
    };

    void getOrientation(int orientation, int mutableResult[4][4]) {
      mutableResult[0][0] = orientations[orientation][0][0];
      mutableResult[0][1] = orientations[orientation][0][1];
      mutableResult[0][2] = orientations[orientation][0][2];

      mutableResult[1][0] = orientations[orientation][1][0];
      mutableResult[1][1] = orientations[orientation][1][1];
      mutableResult[1][2] = orientations[orientation][1][2];

      mutableResult[2][0] = orientations[orientation][2][0];
      mutableResult[2][1] = orientations[orientation][2][1];
      mutableResult[2][2] = orientations[orientation][2][2];
    }

    const int offsets[4][2][5][2] = {
      // North
      {
        // Counter clockwise
        {
          {0, 0}, {1, 0}, {1, -1}, {1, 2}, {0, 0},
        },
        // Clockwise
        {
          {0, 0}, {-1, 0}, {-1, -1}, {-1, 2}, {0, 0},
        },
      },
      // East
      {
        // Counter clockwise
        {
          {0, 0}, {1, 0}, {1, 1}, {0, -2}, {1, -2},
        },
        // Clockwise
        {
          {0, 0}, {1, 0}, {1, 1}, {0, -2}, {1, -2},
        },
      },
      // South
      {
        // Counter clockwise
        {
          {0, 0}, {-1, 0}, {0, 2}, {-1, 2}, {0, 0},
        },
        // Clockwise
        {
          {0, 0}, {1, 0}, {0, 2}, {1, 2}, {0, 0},
        },
      },
      // West
      {
        // Counter clockwise
        {
          {0, 0}, {-1, 0}, {-1, 1}, {0, -2}, {-1, -2},
        },
        // Clockwise
        {
          {0, 0}, {-1, 0}, {-1, 1}, {0, -2}, {-1, -2},
        },
      },
    };

    void getSrsOffsetsForRotation(int clockwise, int rotation, int mutableResult[5][2]) {
      mutableResult[0][0] = offsets[rotation][clockwise][0][0];
      mutableResult[0][1] = offsets[rotation][clockwise][0][1];

      mutableResult[1][0] = offsets[rotation][clockwise][1][0];
      mutableResult[1][1] = offsets[rotation][clockwise][1][1];

      mutableResult[2][0] = offsets[rotation][clockwise][2][0];
      mutableResult[2][1] = offsets[rotation][clockwise][2][1];

      mutableResult[3][0] = offsets[rotation][clockwise][3][0];
      mutableResult[3][1] = offsets[rotation][clockwise][3][1];

      mutableResult[4][0] = offsets[rotation][clockwise][4][0];
      mutableResult[4][1] = offsets[rotation][clockwise][4][1];
    }
};

class SPiece: public Tetromino {
  public:
    /* wstring symbol = L"S"; */
    /* string color = "green"; */
    int symbolNum = 64 + 19;
    int dimension = 3;

    int getDimension() {
      return dimension;
    }

    virtual int getSymbol() {
      return symbolNum;
    }

    /* getPieceOrientation(int orientation) { */
    /*   return orientations[orientation]; */
    /* } */

    const int orientations[4][3][3] = {
      {
          { 0, 1, 1},
          { 1, 1, 0},
          { 0, 0, 0},
      },
      {
          { 0, 1, 0},
          { 0, 1, 1},
          { 0, 0, 1},
      },
      {
          { 0, 0, 0},
          { 0, 1, 1},
          { 1, 1, 0},
      },
      {
          { 1, 0, 0},
          { 1, 1, 0},
          { 0, 1, 0},
      }
    };

    void getOrientation(int orientation, int mutableResult[4][4]) {
      mutableResult[0][0] = orientations[orientation][0][0];
      mutableResult[0][1] = orientations[orientation][0][1];
      mutableResult[0][2] = orientations[orientation][0][2];

      mutableResult[1][0] = orientations[orientation][1][0];
      mutableResult[1][1] = orientations[orientation][1][1];
      mutableResult[1][2] = orientations[orientation][1][2];

      mutableResult[2][0] = orientations[orientation][2][0];
      mutableResult[2][1] = orientations[orientation][2][1];
      mutableResult[2][2] = orientations[orientation][2][2];
    }

    const int offsets[4][2][5][2] = {
      // North
      {
        // Counter clockwise
        {
          {0, 0}, {1, 0}, {1, -1}, {0, 2}, {1, 2},
        },
        // Clockwise
        {
          {0, 0}, {-1, 0}, {-1, -1}, {0, 2}, {-1, 2},
        },
      },
      // East
      {
        // Counter clockwise
        {
          {0, 0}, {1, 0}, {1, 1}, {0, -2}, {1, -2},
        },
        // Clockwise
        {
          {0, 0}, {1, 0}, {1, 1}, {0, -2}, {1, -2},
        },
      },
      // South
      {
        // Counter clockwise
        {
          {0, 0}, {-1, 0}, {-1, 1}, {0, 2}, {-1, 2},
        },
        // Clockwise
        {
          {0, 0}, {1, 0}, {1, 1}, {0, 2}, {1, 2},
        },
      },
      // West
      {
        // Counter clockwise
        {
          {0, 0}, {-1, 0}, {-1, 1}, {0, -2}, {-1, -2},
        },
        // Clockwise
        {
          {0, 0}, {-1, 0}, {-1, 1}, {0, -2}, {-1, -2},
        },
      },
    };

    void getSrsOffsetsForRotation(int clockwise, int rotation, int mutableResult[5][2]) {
      mutableResult[0][0] = offsets[rotation][clockwise][0][0];
      mutableResult[0][1] = offsets[rotation][clockwise][0][1];

      mutableResult[1][0] = offsets[rotation][clockwise][1][0];
      mutableResult[1][1] = offsets[rotation][clockwise][1][1];

      mutableResult[2][0] = offsets[rotation][clockwise][2][0];
      mutableResult[2][1] = offsets[rotation][clockwise][2][1];

      mutableResult[3][0] = offsets[rotation][clockwise][3][0];
      mutableResult[3][1] = offsets[rotation][clockwise][3][1];

      mutableResult[4][0] = offsets[rotation][clockwise][4][0];
      mutableResult[4][1] = offsets[rotation][clockwise][4][1];
    }
};

class ZPiece: public Tetromino {
  public:
    /* wstring symbol = L"Z"; */
    /* string color = "red"; */
    int symbolNum = 64 + 26;
    int dimension = 3;

    int getDimension() {
      return dimension;
    }

    virtual int getSymbol() {
      return symbolNum;
    }

    /* getPieceOrientation(int orientation) { */
    /*   return orientations[orientation]; */
    /* } */

    const int orientations[4][3][3] = {
      {
          { 1, 1, 0},
          { 0, 1, 1},
          { 0, 0, 0},
      },
      {
          { 0, 0, 1},
          { 0, 1, 1},
          { 0, 1, 0},
      },
      {
          { 0, 0, 0},
          { 1, 1, 0},
          { 0, 1, 1},
      },
      {
          { 0, 1, 0},
          { 1, 1, 0},
          { 1, 0, 0},
      }
    };

    void getOrientation(int orientation, int mutableResult[4][4]) {
      mutableResult[0][0] = orientations[orientation][0][0];
      mutableResult[0][1] = orientations[orientation][0][1];
      mutableResult[0][2] = orientations[orientation][0][2];

      mutableResult[1][0] = orientations[orientation][1][0];
      mutableResult[1][1] = orientations[orientation][1][1];
      mutableResult[1][2] = orientations[orientation][1][2];

      mutableResult[2][0] = orientations[orientation][2][0];
      mutableResult[2][1] = orientations[orientation][2][1];
      mutableResult[2][2] = orientations[orientation][2][2];
    }


    const int offsets[4][2][5][2] = {
      // North
      {
        // Counter clockwise
        {
          {0, 0}, {1, 0}, {1, -1}, {0, 2}, {1, 2},
        },
        // Clockwise
        {
          {0, 0}, {-1, 0}, {-1, -1}, {0, 2}, {-1, 1},
        },
      },
      // East
      {
        // Counter clockwise
        {
          {0, 0}, {1, 0}, {1, 1}, {0, -2}, {1, -2},
        },
        // Clockwise
        {
          {0, 0}, {1, 0}, {1, 1}, {0, -2}, {1, -2},
        },
      },
      // South
      {
        // Counter clockwise
        {
          {0, 0}, {-1, 0}, {-1, -1}, {0, 2}, {-1, 2},
        },
        // Clockwise
        {
          {0, 0}, {1, 0}, {1, -1}, {0, 2}, {1, 2},
        },
      },
      // West
      {
        // Counter clockwise
        {
          {0, 0}, {-1, 0}, {-1, 1}, {0, -2}, {-1, -2},
        },
        // Clockwise
        {
          {0, 0}, {-1, 0}, {-1, 1}, {0, -2}, {-1, -2},
        },
      }
    };

    void getSrsOffsetsForRotation(int clockwise, int rotation, int mutableResult[5][2]) {
      mutableResult[0][0] = offsets[rotation][clockwise][0][0];
      mutableResult[0][1] = offsets[rotation][clockwise][0][1];

      mutableResult[1][0] = offsets[rotation][clockwise][1][0];
      mutableResult[1][1] = offsets[rotation][clockwise][1][1];

      mutableResult[2][0] = offsets[rotation][clockwise][2][0];
      mutableResult[2][1] = offsets[rotation][clockwise][2][1];

      mutableResult[3][0] = offsets[rotation][clockwise][3][0];
      mutableResult[3][1] = offsets[rotation][clockwise][3][1];

      mutableResult[4][0] = offsets[rotation][clockwise][4][0];
      mutableResult[4][1] = offsets[rotation][clockwise][4][1];
    }
};

class IPiece: public Tetromino {
  public:
    /* wstring symbol = L"I"; */
    int symbolNum = 64 + 9;
    int dimension = 4;

    int getDimension() {
      return dimension;
    }
    /* /1* string color = "cyan"; *1/ */

    virtual int getSymbol() {
      return symbolNum;
    }

    /* getPieceOrientation(int orientation) { */
    /*   return orientations[orientation]; */
    /* } */

    const int orientations[4][4][4] = {
      {
          { 0, 0, 0, 0 },
          { 1, 1, 1, 1 },
          { 0, 0, 0, 0 },
          { 0, 0, 0, 0 }
      },
      {
          { 0, 0, 1, 0 },
          { 0, 0, 1, 0 },
          { 0, 0, 1, 0 },
          { 0, 0, 1, 0 }
      },
      {
          { 0, 0, 0, 0 },
          { 0, 0, 0, 0 },
          { 1, 1, 1, 1 },
          { 0, 0, 0, 0 }
      },
      {
          { 0, 1, 0, 0 },
          { 0, 1, 0, 0 },
          { 0, 1, 0, 0 },
          { 0, 1, 0, 0 }
      }
    };

    void getOrientation(int orientation, int mutableResult[4][4]) {
      mutableResult[0][0] = orientations[orientation][0][0];
      mutableResult[0][1] = orientations[orientation][0][1];
      mutableResult[0][2] = orientations[orientation][0][2];
      mutableResult[0][3] = orientations[orientation][0][3];

      mutableResult[1][0] = orientations[orientation][1][0];
      mutableResult[1][1] = orientations[orientation][1][1];
      mutableResult[1][2] = orientations[orientation][1][2];
      mutableResult[1][3] = orientations[orientation][1][3];

      mutableResult[2][0] = orientations[orientation][2][0];
      mutableResult[2][1] = orientations[orientation][2][1];
      mutableResult[2][2] = orientations[orientation][2][2];
      mutableResult[2][3] = orientations[orientation][2][3];

      mutableResult[3][0] = orientations[orientation][3][0];
      mutableResult[3][1] = orientations[orientation][3][1];
      mutableResult[3][2] = orientations[orientation][3][2];
      mutableResult[3][3] = orientations[orientation][3][3];
    }

    const int offsets[4][2][5][2] = {
      // North
      {
        // Counter clockwise
        {
          {0, 0}, {-1, 0}, {2, 0}, {-1, -2}, {2, 1},
        },
        // Clockwise
        {
          {0, 0}, {-2, 0}, {1, 0}, {-2, 1}, {1, -2},
        },
      },
      // East
      {
        // Counter clockwise
        {
          {0, 0}, {1, 0}, {-1, 0}, {2, -1}, {-1, 2},
        },
        // Clockwise
        {
          {0, 0}, {-1, 0}, {2, 0}, {-1, -2}, {2, 2},
        },
      },
      // South
      {
        // Counter clockwise
        {
          {0, 0}, {1, 0}, {-2, 0}, {1, 2}, {-2, -1},
        },
        // Clockwise
        {
          {0, 0}, {2, 0}, {-1, 0}, {2, -1}, {-1, 2},
        },
      },
      // West
      {
        // Counter clockwise
        {
          {0, 0}, {-2, 0}, {1, 0}, {-2, 2}, {1, -2},
        },
        // Clockwise
        {
          {0, 0}, {1, 0}, {-2, 0}, {1, 2}, {-2, -1},
        },
      },
    };

    void getSrsOffsetsForRotation(int clockwise, int rotation, int mutableResult[5][2]) {
      mutableResult[0][0] = offsets[rotation][clockwise][0][0];
      mutableResult[0][1] = offsets[rotation][clockwise][0][1];

      mutableResult[1][0] = offsets[rotation][clockwise][1][0];
      mutableResult[1][1] = offsets[rotation][clockwise][1][1];

      mutableResult[2][0] = offsets[rotation][clockwise][2][0];
      mutableResult[2][1] = offsets[rotation][clockwise][2][1];

      mutableResult[3][0] = offsets[rotation][clockwise][3][0];
      mutableResult[3][1] = offsets[rotation][clockwise][3][1];

      mutableResult[4][0] = offsets[rotation][clockwise][4][0];
      mutableResult[4][1] = offsets[rotation][clockwise][4][1];
    }
};

class LPiece: public Tetromino {
  public:
    /* wstring symbol = L"L"; */
    /* /1* string color = "orange"; *1/ */
    int symbolNum = 64 + 12;
    int dimension = 3;

    int getDimension() {
      return dimension;
    }

    virtual int getSymbol() {
      return symbolNum;
    }

    /* Elements<Elements<int> > getPieceOrientation(int orientation) { */
    /*   return orientations[orientation]; */
    /* } */

    const int orientations[4][3][3] = {
      {
          { 0, 0, 1},
          { 1, 1, 1},
          { 0, 0, 0},
      },
      {
          { 0, 1, 0},
          { 0, 1, 0},
          { 0, 1, 1},
      },
      {
          { 0, 0, 0},
          { 1, 1, 1},
          { 1, 0, 0},
      },
      {
          { 1, 1, 0},
          { 0, 1, 0},
          { 0, 1, 0},
      },
    };

    void getOrientation(int orientation, int mutableResult[4][4]) {
      mutableResult[0][0] = orientations[orientation][0][0];
      mutableResult[0][1] = orientations[orientation][0][1];
      mutableResult[0][2] = orientations[orientation][0][2];

      mutableResult[1][0] = orientations[orientation][1][0];
      mutableResult[1][1] = orientations[orientation][1][1];
      mutableResult[1][2] = orientations[orientation][1][2];

      mutableResult[2][0] = orientations[orientation][2][0];
      mutableResult[2][1] = orientations[orientation][2][1];
      mutableResult[2][2] = orientations[orientation][2][2];
    }

    const int offsets[4][2][5][2] = {
      // North
      {
        // Counter clockwise
        {
          {0, 0}, {1, 0}, {1, -1}, {0, 2}, {1, 2},
        },
        // Clockwise
        {
          {0, 0}, {-1, 0}, {-1, -1}, {0, 2}, {-1, 2},
        },
      },
      // East
      {
        // Counter clockwise
        {
          {0, 0}, {1, 0}, {1, 1}, {0, -2}, {1, -2},
        },
        // Clockwise
        {
          {0, 0}, {1, 0}, {1, 1}, {0, -2}, {1, -2},
        },
      },
      // South
      {
        // Counter clockwise
        {
          {0, 0}, {-1, 0}, {-1, -1}, {0, 2}, {-1, 2},
        },
        // Clockwise
        {
          {0, 0}, {1, 0}, {1, -1}, {0, 2}, {1, 2},
        },
      },
      // West
      {
        // Counter clockwise
        {
          {0, 0}, {-1, 0}, {-1, 1}, {0, 2}, {-1, 2},
        },
        // Clockwise
        {
          {0, 0}, {-1, 0}, {-1, 1}, {0, 2}, {-1, 2},
        },
      },
    };

    void getSrsOffsetsForRotation(int clockwise, int rotation, int mutableResult[5][2]) {
      mutableResult[0][0] = offsets[rotation][clockwise][0][0];
      mutableResult[0][1] = offsets[rotation][clockwise][0][1];

      mutableResult[1][0] = offsets[rotation][clockwise][1][0];
      mutableResult[1][1] = offsets[rotation][clockwise][1][1];

      mutableResult[2][0] = offsets[rotation][clockwise][2][0];
      mutableResult[2][1] = offsets[rotation][clockwise][2][1];

      mutableResult[3][0] = offsets[rotation][clockwise][3][0];
      mutableResult[3][1] = offsets[rotation][clockwise][3][1];

      mutableResult[4][0] = offsets[rotation][clockwise][4][0];
      mutableResult[4][1] = offsets[rotation][clockwise][4][1];
    }
};

class JPiece: public Tetromino {
  public:
    /* wstring symbol = L"J"; */
    /* /1* string color = "blue"; *1/ */
    int symbolNum = 64 + 10;
    int dimension = 3;

    int getDimension() {
      return dimension;
    }

    virtual int getSymbol() {
      return symbolNum;
    }

    /* getPieceOrientation(int orientation) { */
    /*   return orientations[orientation]; */
    /* } */

    int const orientations[4][3][3] = {
      {
          { 1, 0, 0 },
          { 1, 1, 1 },
          { 0, 0, 0 },
      },
      {
          { 0, 1, 1 },
          { 0, 1, 0 },
          { 0, 1, 0 },
      },
      {
          { 0, 0, 0 },
          { 1, 1, 1 },
          { 0, 0, 1 },
      },
      {
          { 0, 1, 0 },
          { 0, 1, 0 },
          { 1, 1, 0 },
      },
    };

    void getOrientation(int orientation, int mutableResult[4][4]) {
      mutableResult[0][0] = orientations[orientation][0][0];
      mutableResult[0][1] = orientations[orientation][0][1];
      mutableResult[0][2] = orientations[orientation][0][2];

      mutableResult[1][0] = orientations[orientation][1][0];
      mutableResult[1][1] = orientations[orientation][1][1];
      mutableResult[1][2] = orientations[orientation][1][2];

      mutableResult[2][0] = orientations[orientation][2][0];
      mutableResult[2][1] = orientations[orientation][2][1];
      mutableResult[2][2] = orientations[orientation][2][2];
    }

    const int offsets[4][2][5][2] = {
      // North
      {
        // Counter clockwise
        {
          {0, 0}, {1, 0}, {1, -1}, {0, 2}, {1, 2},
        },
        // Clockwise
        {
          {0, 0}, {-1, 0}, {-1, -1}, {0, 2}, {-1, 2},
        },
      },
      // East
      {
        // Counter clockwise
        {
          {0, 0}, {1, 0}, {1, 1}, {0, -2}, {1, -2},
        },
        // Clockwise
        {
          {0, 0}, {1, 0}, {1, 1}, {0, -2}, {1, -2},
        },
      },
      // South
      {
        // Counter clockwise
        {
          {0, 0}, {-1, 0}, {-1, -1}, {0, 2}, {-1, 2},
        },
        // Clockwise
        {
          {0, 0}, {1, 0}, {1, -1}, {0, 2}, {1, 2},
        },
      },
      // West
      {
        // Counter clockwise
        {
          {0, 0}, {-1, 0}, {-1, 1}, {0, -2}, {-1, -2},
        },
        // Clockwise
        {
          {0, 0}, {-1, 0}, {-1, 1}, {0, -2}, {-1, -2},
        },
      },
    };

    void getSrsOffsetsForRotation(int clockwise, int rotation, int mutableResult[5][2]) {
      mutableResult[0][0] = offsets[rotation][clockwise][0][0];
      mutableResult[0][1] = offsets[rotation][clockwise][0][1];

      mutableResult[1][0] = offsets[rotation][clockwise][1][0];
      mutableResult[1][1] = offsets[rotation][clockwise][1][1];

      mutableResult[2][0] = offsets[rotation][clockwise][2][0];
      mutableResult[2][1] = offsets[rotation][clockwise][2][1];

      mutableResult[3][0] = offsets[rotation][clockwise][3][0];
      mutableResult[3][1] = offsets[rotation][clockwise][3][1];

      mutableResult[4][0] = offsets[rotation][clockwise][4][0];
      mutableResult[4][1] = offsets[rotation][clockwise][4][1];
    }
};

class OPiece: public Tetromino {
  public:
    /* wstring symbol = L"O"; */
    int symbolNum = 64 + 15;
    int dimension = 3;

    int getDimension() {
      return dimension;
    }
    /* string color = "yellow"; */

    virtual int getSymbol() {
      return symbolNum;
    }

    /* getPieceOrientation(int orientation) { */
    /*   return orientations[orientation]; */
    /* } */

    const int orientations[4][3][3] = {
      {
          { 0, 1, 1 },
          { 0, 1, 1 },
          { 0, 0, 0 },
      },
      {
          { 0, 1, 1 },
          { 0, 1, 1 },
          { 0, 0, 0 },
      },
      {
          { 0, 1, 1 },
          { 0, 1, 1 },
          { 0, 0, 0 },
      },
      {
          { 0, 1, 1 },
          { 0, 1, 1 },
          { 0, 0, 0 },
      }
    };

    void getOrientation(int orientation, int mutableResult[4][4]) {
      mutableResult[0][0] = orientations[orientation][0][0];
      mutableResult[0][1] = orientations[orientation][0][1];
      mutableResult[0][2] = orientations[orientation][0][2];

      mutableResult[1][0] = orientations[orientation][1][0];
      mutableResult[1][1] = orientations[orientation][1][1];
      mutableResult[1][2] = orientations[orientation][1][2];

      mutableResult[2][0] = orientations[orientation][2][0];
      mutableResult[2][1] = orientations[orientation][2][1];
      mutableResult[2][2] = orientations[orientation][2][2];
    }

    const int offsets[4][2][5][2] = {
      // North
      {
        // Clockwise
        {
          {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0},
        },
        // Counter clockwise
        {
          {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0},
        },
      },
      // East
      {
        // Clockwise
        {
          {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0},
        },
        // Counter clockwise
        {
          {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0},
        },
      },
      // South
      {
        // Clockwise
        {
          {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0},
        },
        // Counter clockwise
        {
          {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0},
        },
      },
      // West
      {
        // Clockwise
        {
          {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0},
        },
        // Counter clockwise
        {
          {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0},
        },
      },
    };

    void getSrsOffsetsForRotation(int clockwise, int rotation, int mutableResult[5][2]) {
      mutableResult[0][0] = offsets[rotation][clockwise][0][0];
      mutableResult[0][1] = offsets[rotation][clockwise][0][1];

      mutableResult[1][0] = offsets[rotation][clockwise][1][0];
      mutableResult[1][1] = offsets[rotation][clockwise][1][1];

      mutableResult[2][0] = offsets[rotation][clockwise][2][0];
      mutableResult[2][1] = offsets[rotation][clockwise][2][1];

      mutableResult[3][0] = offsets[rotation][clockwise][3][0];
      mutableResult[3][1] = offsets[rotation][clockwise][3][1];

      mutableResult[4][0] = offsets[rotation][clockwise][4][0];
      mutableResult[4][1] = offsets[rotation][clockwise][4][1];
    }
};

/* /1* int randomNumber (int i) { return std::rand()%i;} *1/ */

void shuffleArray(int * array, int size)
{
  int last = 0;
  int temp = array[last];
  for (int i=0; i<size; i++)
  {
    int index = random(size);
    array[last] = array[index];
    last = index;
  }
  array[last] = temp;
}

const int NUMBER_OF_TETROMINOS = 7;

class PieceBag {
  public:
    int currentIndex = 0;
    int bagOrder[14];
    Tetromino* tetrominos[NUMBER_OF_TETROMINOS];

    void createNewBagOrder() 
    {
      // Shuffle two bags separately, then append them to each other.
      // We create two at once so we can always see what pieces are coming up.

      int orderedBag1[NUMBER_OF_TETROMINOS] = {0, 1, 2, 3, 4, 5, 6};
      shuffleArray(orderedBag1, NUMBER_OF_TETROMINOS);
      /* std::random_shuffle (orderedBag1.begin(), orderedBag1.end(), random); */

      int orderedBag2[NUMBER_OF_TETROMINOS] = {0, 1, 2, 3, 4, 5, 6};
      shuffleArray(orderedBag2, NUMBER_OF_TETROMINOS);
      /* std::random_shuffle (orderedBag2.begin(), orderedBag2.end(), random); */

      for (int i = 0; i < NUMBER_OF_TETROMINOS; i++) {
        bagOrder[i] = orderedBag1[i];
      }

      for (int i = 0; i < NUMBER_OF_TETROMINOS; i++) {
        bagOrder[i] = orderedBag2[i];
      }
    }

    Tetromino* getNextPiece() {
      /* cout << currentIndex << endl; */
      if (currentIndex == sizeof(bagOrder))
      {
        currentIndex = 0;
        createNewBagOrder(); // Mutates bagOrder
      }

      Tetromino* tetrominoToReturn = tetrominos[bagOrder[currentIndex]];
      currentIndex++;
      return tetrominoToReturn;
    }

    PieceBag() {
      // This next line seems necessary to "kick off" actual randomness,
      // otherwise the first piece is always the same. Very weird.
      /* srand (time(NULL)); */
      /* randomNumber(7); */

      /* random(10); */
      /* random(10); */
      tetrominos[0] = new TPiece;
      tetrominos[1] = new SPiece;
      tetrominos[2] = new ZPiece;
      tetrominos[3] = new JPiece;
      tetrominos[4] = new LPiece;
      tetrominos[5] = new IPiece;
      tetrominos[6] = new OPiece;

      createNewBagOrder();
    }
};

#endif
