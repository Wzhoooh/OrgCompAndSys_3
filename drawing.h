#include <windows.h>

#ifdef UNICODE
    #define tsprintf swprintf
    #define tstrlen wcslen
#else
    #define tsprintf snprintf
    #define tstrlen strlen
#endif

#define MAX_COVERAGE 0.95

#define X_FROM -20
#define X_TO 10
#define Y_FROM -6
#define Y_TO 1

#define X_NUM_MARKS 10
#define Y_NUM_MARKS 7

typedef struct
{
    int X;
    int Y;
} COORDInt;

typedef struct
{
    double X;
    double Y;
} COORDDouble;

typedef struct
{
    COORDInt centerPix;
    COORDInt topLeftPix;
    COORDInt bottomRightPix;
    COORDDouble topLeftMeas;
    COORDDouble bottomRightMeas;
    double numPixInMeas;
} FrameInfo;

double getNumPixInMeas(double xFrameFrom, double xFrameTo, double yFrameFrom, double yFrameTo, RECT winRect);
COORDInt getCenterCoord(double xFrameFrom, double xFrameTo, double yFrameFrom, double yFrameTo, RECT winRect);
FrameInfo getFrameInfo(double xFrameFrom, double xFrameTo, double yFrameFrom, double yFrameTo, RECT winRect);
COORDInt getCoord(COORDDouble coord, FrameInfo info);
BOOL isFitIntoFrame(COORDInt coord, FrameInfo info);

HPEN drawFrame(HDC hdc, FrameInfo info, COLORREF pen);
HPEN drawAxes(HDC hdc, FrameInfo info, COLORREF pen);
HPEN drawMarkOnX(HDC hdc, COORDInt coord, double val, COLORREF pen);
HPEN drawMarkOnY(HDC hdc, COORDInt coord, double val, COLORREF pen);