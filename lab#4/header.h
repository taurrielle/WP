#define ID_TIMER 333

class Objects
{
public:
    COLORREF color;
    POINT leftCorner;
    POINT dimensions;


    int xSpeed, ySpeed;
    int squareFlag = 0;

    Objects(POINT leftCorner, POINT rightCorner, const int &speed,  COLORREF color);
    void accelerate (const int & delta);
    void decelerate(const int &delta);
    void Move(const HDC &hdc,const RECT& rect,HBRUSH &hBrush);

    void changeColor();

};
    void onInteraction(Objects &obj1, Objects &obj2, int &nrObj);
