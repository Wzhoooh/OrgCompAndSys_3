#include <windows.h>
#include <math.h>

#ifdef UNICODE
    #define tsprintf swprintf
    #define tstrlen wcslen
#else
    #define tsprintf snprintf
    #define tstrlen strlen
#endif

#define MAX_COVERAGE 0.95

#define X_FROM 3.0 * M_PI / 2.0
#define X_TO 18.0 * M_PI
#define Y_FROM -5
#define Y_TO 10

#define Y_AXIS_COORD 40

#define X_NUM_MARKS 20
#define Y_NUM_MARKS 7

// for calculating of max function value
#define STEP_SIZE 0.001

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

double func(double x);

double getNumPixInMeas(double xFrameFrom, double xFrameTo, double yFrameFrom, double yFrameTo, RECT winRect);
COORDInt getCenterCoord(double xFrameFrom, double xFrameTo, double yFrameFrom, double yFrameTo, RECT winRect);
FrameInfo getFrameInfo(double xFrameFrom, double xFrameTo, double yFrameFrom, double yFrameTo, RECT winRect);
COORDInt getPixCoord(COORDDouble coord, FrameInfo info);
COORDDouble getMeasCoord(COORDInt coord, FrameInfo info);
BOOL isFitIntoFrame(COORDInt coord, FrameInfo info);

HPEN drawFrame(HDC hdc, FrameInfo info, COLORREF pen);
HPEN drawAxes(HDC hdc, double yAxis, FrameInfo info, COLORREF pen);
HPEN drawMarkOnX(HDC hdc, COORDInt coord, double val, COLORREF pen);
HPEN drawMarkOnY(HDC hdc, COORDInt coord, double val, COLORREF pen);
HPEN drawGraph(HDC hdc, double (*func)(double), FrameInfo info, COLORREF pen);

COORDDouble findMaxPoint(double (*func)(double), FrameInfo info);