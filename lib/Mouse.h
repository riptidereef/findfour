#pragma once

class Mouse
{
    int x, y;
    bool left, middle, right;

    int mickeyScale;
    int xLimit, yLimit;

    int xMickeys, yMickeys;
    int leftPresses, leftReleases, rightPresses, rightReleases;

    static Mouse *instance;
    static bool isPresent();

    Mouse();

public:
    static Mouse *getInstance();
    static void shutdown();

    inline int getX() { return x; }
    inline int getY() { return y; }
    inline int getLeft() { return left; }
    inline int getRight() { return right; }
    inline int getMiddle() { return middle; }

    void update();
    void reset(int _xLimit, int _yLimit, int _mickeyScale = 8);
};
