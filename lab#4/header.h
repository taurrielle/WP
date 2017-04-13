#define ID_TIMER 333

class Objects
{
protected:
    COLORREF color;
public:
    POINT leftCorner;
    POINT dimensions;

    int xSpeed, ySpeed;
    int counter = 0;

    Objects(POINT leftCorner, POINT rightCorner, const int &speed);

    bool Move(const HDC &hdc,const RECT& rect,HBRUSH &hBrush);

};
