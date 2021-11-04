#include <stdio.h>
#include <string.h>

#include "drawing.h"

double func(double x)
{
    return pow(cos(x / 0.5), 2.0) + pow(x, 0.5);
}

double getNumPixInMeas(double xFrameFrom, double xFrameTo, double yFrameFrom, double yFrameTo, RECT winRect)
{
    double xFrameSize = xFrameTo - xFrameFrom;
    double yFrameSize = yFrameTo - yFrameFrom;
    int xWindowSize = winRect.right - winRect.left;
    int yWindowSize = winRect.bottom - winRect.top;
    double pixInMeas;

    // trying to make xFrame size of xWindow
    pixInMeas = (double)xWindowSize * MAX_COVERAGE / xFrameSize;
    if (pixInMeas * yFrameSize > yWindowSize * MAX_COVERAGE)
        // xFrame has not fit into xWindow
        pixInMeas = (double)yWindowSize * MAX_COVERAGE / yFrameSize;

    return pixInMeas;
}

COORDInt getCenterCoord(double xFrameFrom, double xFrameTo, double yFrameFrom, double yFrameTo, RECT winRect)
{
    double xFrameSize = xFrameTo - xFrameFrom;
    double yFrameSize = yFrameTo - yFrameFrom;
    int xWindowSize = winRect.right - winRect.left;
    int yWindowSize = winRect.bottom - winRect.top;

    COORDInt centerCoord;
    double pixInMeas = getNumPixInMeas(xFrameFrom, xFrameTo, yFrameFrom, yFrameTo, winRect);
    centerCoord.X = (int)(-xFrameFrom * pixInMeas) + (xWindowSize - xFrameSize * pixInMeas) / 2;
    centerCoord.Y = (int)(yFrameTo * pixInMeas) + (yWindowSize - yFrameSize * pixInMeas) / 2;

    return centerCoord;
}

FrameInfo getFrameInfo(double xFrameFrom, double xFrameTo, double yFrameFrom, double yFrameTo, RECT winRect)
{
    FrameInfo info;
    info.topLeftMeas.X = xFrameFrom;
    info.topLeftMeas.Y = yFrameTo;
    info.bottomRightMeas.X = xFrameTo;
    info.bottomRightMeas.Y = yFrameFrom;

    info.numPixInMeas = getNumPixInMeas(xFrameFrom, xFrameTo, yFrameFrom, yFrameTo, winRect);

    COORDInt centerPix = getCenterCoord(xFrameFrom, xFrameTo, yFrameFrom, yFrameTo, winRect);
    info.centerPix.X = centerPix.X;
    info.centerPix.Y = centerPix.Y;

    info.topLeftPix.X = info.centerPix.X + info.topLeftMeas.X * info.numPixInMeas;
    info.topLeftPix.Y = info.centerPix.Y - info.topLeftMeas.Y * info.numPixInMeas;
    info.bottomRightPix.X = info.centerPix.X + info.bottomRightMeas.X * info.numPixInMeas;
    info.bottomRightPix.Y = info.centerPix.Y - info.bottomRightMeas.Y * info.numPixInMeas;
    
    return info;
}

COORDInt getPixCoord(COORDDouble coord, FrameInfo info)
{
    COORDInt res;
    res.X = info.centerPix.X + coord.X * info.numPixInMeas;
    res.Y = info.centerPix.Y - coord.Y * info.numPixInMeas;
    return res;
}

COORDDouble getMeasCoord(COORDInt coord, FrameInfo info)
{
    COORDDouble res;
    res.X = (double)(coord.X - info.centerPix.X) / info.numPixInMeas;
    res.Y = (double)(info.centerPix.Y - coord.Y) / info.numPixInMeas;
    return res;
}

BOOL isFitIntoFrame(COORDInt coord, FrameInfo info)
{
    return coord.X >= info.topLeftPix.X && coord.X <= info.bottomRightPix.X &&
        coord.Y >= info.topLeftPix.Y && coord.Y <= info.bottomRightPix.Y;
}

HPEN drawFrame(HDC hdc, FrameInfo info, COLORREF pen)
{
    HPEN hPen = CreatePen(PS_SOLID, 1, pen);
    HPEN hOldPen = SelectObject(hdc, hPen);

    MoveToEx(hdc, info.topLeftPix.X, info.topLeftPix.Y, NULL);
    LineTo(hdc, info.bottomRightPix.X, info.topLeftPix.Y);

    MoveToEx(hdc, info.bottomRightPix.X, info.topLeftPix.Y, NULL);
    LineTo(hdc, info.bottomRightPix.X, info.bottomRightPix.Y);

    MoveToEx(hdc, info.bottomRightPix.X, info.bottomRightPix.Y, NULL);
    LineTo(hdc, info.topLeftPix.X, info.bottomRightPix.Y);

    MoveToEx(hdc, info.topLeftPix.X, info.bottomRightPix.Y, NULL);
    LineTo(hdc, info.topLeftPix.X, info.topLeftPix.Y);

    return hOldPen;
}

HPEN drawAxes(HDC hdc, double yAxis, FrameInfo info, COLORREF pen)
{
    double xLength = info.bottomRightMeas.X - info.topLeftMeas.X;
    double yLength = info.topLeftMeas.Y - info.bottomRightMeas.Y;

    HPEN hPen = CreatePen(PS_SOLID, 1, pen);
    HPEN hOldPen = SelectObject(hdc, hPen);

    COORDDouble verticPointMeas = { yAxis, 0};
    COORDInt newCenter = getPixCoord(verticPointMeas, info);
    COORDInt verticPoint = { newCenter.X, info.topLeftPix.Y };
    COORDInt horizPoint = { info.topLeftPix.X, info.centerPix.Y };
    // vertical line
    if (isFitIntoFrame(verticPoint, info))
    {
        MoveToEx(hdc, newCenter.X, info.topLeftPix.Y, NULL);
        LineTo(hdc, newCenter.X, info.bottomRightPix.Y);

        // marks
        if (isFitIntoFrame(newCenter, info))
        {
            int numMarksInTopPart = Y_NUM_MARKS * 
                (info.topLeftMeas.Y - 0.0) / (info.topLeftMeas.Y - info.bottomRightMeas.Y);
            int numMarksInBottomPart = Y_NUM_MARKS * 
                (0.0 - info.bottomRightMeas.Y) / (info.topLeftMeas.Y - info.bottomRightMeas.Y);
            
            // drawing in right part
            for (int i = 1; i <= numMarksInTopPart; i++)
            {
                COORDDouble markVal = { yAxis, 
                    0.0 + (info.topLeftMeas.Y - info.bottomRightMeas.Y) / Y_NUM_MARKS * i };
                COORDInt point = getPixCoord(markVal, info);
                if (isFitIntoFrame(point, info))
                    drawMarkOnY(hdc, point, markVal.Y, pen);
            }
            // drawing in left part
            for (int i = 1; i <= numMarksInBottomPart; i++)
            {
                COORDDouble markVal = { yAxis, 
                    0.0 - (info.topLeftMeas.Y - info.bottomRightMeas.Y) / Y_NUM_MARKS * i};
                COORDInt point = getPixCoord(markVal, info);
                if (isFitIntoFrame(point, info))
                    drawMarkOnY(hdc, point, markVal.Y, pen);
            }
        }
        else
            for (int i = 0; i < Y_NUM_MARKS; i++)
            {
                COORDDouble markVal = { yAxis, 
                    info.bottomRightMeas.Y + (info.topLeftMeas.Y - info.bottomRightMeas.Y) / Y_NUM_MARKS * i };
                COORDInt point = getPixCoord(markVal, info);
                if (isFitIntoFrame(point, info))
                    drawMarkOnY(hdc, point, markVal.Y, pen);
            }
    }
    // horizontal line
    if (isFitIntoFrame(horizPoint, info))
    {
        MoveToEx(hdc, info.topLeftPix.X, newCenter.Y, NULL);
        LineTo(hdc, info.bottomRightPix.X, newCenter.Y);

        // marks
        if (isFitIntoFrame(newCenter, info))
        {
            int numMarksInRightPart = X_NUM_MARKS * 
                (info.bottomRightMeas.X - yAxis) / (info.bottomRightMeas.X - info.topLeftMeas.X);
            int numMarksInLeftPart = X_NUM_MARKS * 
                (yAxis - info.topLeftMeas.X) / (info.bottomRightMeas.X - info.topLeftMeas.X);
            
            // drawing in right part
            for (int i = 0; i <= numMarksInRightPart; i++)
            {
                COORDDouble markVal = { 
                    yAxis + (info.bottomRightMeas.X - info.topLeftMeas.X) / X_NUM_MARKS * i, 0.0 };
                COORDInt point = getPixCoord(markVal, info);
                if (isFitIntoFrame(point, info))
                    drawMarkOnX(hdc, point, markVal.X, pen);
            }
            // drawing in left part
            for (int i = 1; i <= numMarksInLeftPart; i++)
            {
                COORDDouble markVal = { 
                    yAxis - (info.bottomRightMeas.X - info.topLeftMeas.X) / X_NUM_MARKS * i, 0.0 };
                COORDInt point = getPixCoord(markVal, info);
                if (isFitIntoFrame(point, info))
                    drawMarkOnX(hdc, point, markVal.X, pen);
            }
        }
        else
            for (int i = 0; i < X_NUM_MARKS; i++)
            {
                COORDDouble markVal = { 
                    info.topLeftMeas.X + (info.bottomRightMeas.X - info.topLeftMeas.X) / X_NUM_MARKS * i, 0 };
                COORDInt point = getPixCoord(markVal, info);
                if (isFitIntoFrame(point, info))
                    drawMarkOnX(hdc, point, markVal.X, pen);
            }
    }
    return hOldPen;
}

HPEN drawMarkOnX(HDC hdc, COORDInt coord, double val, COLORREF pen)
{
    HPEN hPen = CreatePen(PS_SOLID, 1, pen);
    HPEN hOldPen = SelectObject(hdc, hPen);

    COORDInt from = coord, to = coord;
    from.Y -= 4;
    to.Y += 4;
    MoveToEx(hdc, from.X, from.Y, NULL);
    LineTo(hdc, to.X, to.Y);

    TCHAR sVal[10] = { 0 };
    tsprintf(sVal, 10, TEXT("%.1f"), val);
    SetBkMode(hdc, TRANSPARENT);
    TextOut(hdc, to.X - 5, to.Y + 1, sVal, tstrlen(sVal));

    return hOldPen;
}

HPEN drawMarkOnY(HDC hdc, COORDInt coord, double val, COLORREF pen)
{
    HPEN hPen = CreatePen(PS_SOLID, 1, pen);
    HPEN hOldPen = SelectObject(hdc, hPen);

    COORDInt from = coord, to = coord;
    from.X -= 4;
    to.X += 4;
    MoveToEx(hdc, from.X, from.Y, NULL);
    LineTo(hdc, to.X, to.Y);

    TCHAR sVal[10] = { 0 };
    tsprintf(sVal, 10, TEXT("%.1f"), val);
    SetBkMode(hdc, TRANSPARENT);
    TextOut(hdc, to.X + 1, to.Y - 5, sVal, tstrlen(sVal));

    return hOldPen;
}

HPEN drawGraph(HDC hdc, double (*func)(double), FrameInfo info, COLORREF pen)
{
    HPEN hPen = CreatePen(PS_SOLID, 1, pen);
    HPEN hOldPen = SelectObject(hdc, hPen);

    COORDInt prev = { -1, -1 };
    for (int i = info.topLeftPix.X; i <= info.bottomRightPix.X; i++)
    {
        COORDInt xPix = { i, info.centerPix.Y };
        COORDDouble xMeas = getMeasCoord(xPix, info);
        COORDDouble yMeas = { xMeas.X, func(xMeas.X) };
        COORDInt yPix = getPixCoord(yMeas, info);
        if (isFitIntoFrame(yPix, info))
        {
            if (prev.X != -1)
            {
                MoveToEx(hdc, prev.X, prev.Y, NULL);
                LineTo(hdc, yPix.X, yPix.Y);
            }
            prev.X = yPix.X;
            prev.Y = yPix.Y;
        }
        else
        {
            prev.X = -1;
            prev.Y = -1;
        }
    }

    return hOldPen;
}

COORDDouble findMaxPoint(double (*func)(double), FrameInfo info)
{
    COORDDouble maxPoint = { -1, pow(10.0, -305.0) };
    for (double x = info.topLeftMeas.X; x <= info.bottomRightMeas.X; x += STEP_SIZE)
    {
        double y = func(x);
        if (y <= info.topLeftMeas.Y && y > maxPoint.Y)
        {
            maxPoint.X = x;
            maxPoint.Y = y;
        }
    }
    return maxPoint;
}