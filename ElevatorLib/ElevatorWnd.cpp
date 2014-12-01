/**
 * \file ElevatorWnd.cpp
 *
 * \author Charles B. Owen
 *
 * \cond
 * I am disabling documentation for this module so we 
 * will only see the ElevatorController class in the docs.
 */

#include "stdafx.h"
#include "ElevatorLib.h"
#include "ElevatorWnd.h"
#include "DoubleBufferDC.h"

#include <string>
#include <iostream>
#include <sstream>
#include <cassert>

using namespace std;
using namespace Gdiplus;

/// Frame duration in milliseconds
const int FrameDuration = 50;

/// Duration of a time slice for our simulation
const double SimulationTimeslice = 0.001;

const double MaxHeight = 6.88;
const double MinHeight = -0.656;

/* Pixel values */
const int ElevatorWidth = 429;
const int ElevatorHeight = 664;
const int FloorHeight = 200;
const int Car1Left = 236;
const int Car2Left = 136;
const int CarBottom = 653;      // Left left corner of where car starts
const int DoorFrameLeft = 230;
const int DoorUpLeft = 38;
const int DoorDownLeft = 48;
const int DoorIndicatorTop = 4;
const double DoorOpenTime = 1;
const int ButtonRadius = 21;


inline int SQR(int i) { return i * i; }
inline double maxx(double a, double b) { return a > b ? a : b; }

IMPLEMENT_DYNAMIC(CElevatorWnd, CWnd)

BEGIN_MESSAGE_MAP(CElevatorWnd, CWnd)
    ON_WM_PAINT()
    ON_WM_TIMER()
    ON_WM_LBUTTONDOWN()
    ON_WM_CREATE()
END_MESSAGE_MAP()

/// \endcond

/**
* \brief Constructor for elevator simulator
*
* This is the actual simulation of the elevator
*/
CElevatorWnd::CElevatorWnd() 
{
    srand((unsigned int)time(NULL));

    //
    // Initialize the loading
    //
    mPassengers[Sparty] = false;
    mPassengers[Noel] = false;
    mPassengers[Neil] = false;

    mButtonCenters.resize(BButtonCount, Point(-1000, -1000));

    //
    // Initialize each floor 
    //
    for (int i = 0; i < NumFloors; i++)
    {
        mDoors[i].SetUser(this);
        mDoors[i].SetFloor(i);
        mPanelFloorLights[i] = false;
        mDoorLights[i][CElevatorController::Up] = false;
        mDoorLights[i][CElevatorController::Down] = false;
        mCallLights[i][CElevatorController::Up] = false;
        mCallLights[i][CElevatorController::Down] = false;
    }

}

/**
* \brief Destructor
*/
CElevatorWnd::~CElevatorWnd()
{

}

/** \brief The elevator width. Based on the width of the background image
* \return Width */
int CElevatorWnd::GetWidth() 
{ 
    return ElevatorWidth; 
}

/** \brief The elevator height. Based on the height of the background image
* \return Height */
int CElevatorWnd::GetHeight()
{
    return ElevatorHeight;
}


/**
* Set the elevator controller we are using for this elevator.
* @param c Elevator controller
*/
void CElevatorWnd::SetController(std::shared_ptr<CElevatorController> c)
{
    mController = c;
    mController->SetElevator(this);
    mController->Initialize();
}


/**
* \brief Reset a stopwatch available to a user program.
*/
void CElevatorWnd::ResetTimer()
{
    mUserTimer = 0;
}

/**
* \brief Get the stopwatch time.
* @return Time since timer has been reset in seconds
*/
double CElevatorWnd::GetTimer()
{
    return mUserTimer;
}

/**
* \brief Update the simulation based on time
*
* This is normally only called from testing. It advances the simulation
* by some elapsed time. Note that this does not have to be
* real time.
*
* This will advance in millisecond increments only.
*
* \param elapsed Time to advance the simulation in seconds.
*/
void CElevatorWnd::Update(double elapsed)
{
    while (elapsed >= SimulationTimeslice)
    {
        UpdateTimeslice();
        elapsed -= SimulationTimeslice;
    }
}


/**
* \brief Press the Open button on the panel in the car
*
* This function is called when the button is pressed.
* It can be used to simulate a press of the button as well.
*/
void CElevatorWnd::PressOpen()
{
    mController->Speak("Open pressed");
    mController->OnOpenPressed();
}


/** \brief Press the Close button on the panel in the car
*
* This function is called when the button is pressed.
* It can be used to simulate a press of the button as well.
*/
void CElevatorWnd::PressClose()
{
    mController->Speak("Close pressed");
    mController->OnClosePressed();
}

/** \brief Press a floor button in the elevator panel
*
* This function is called when the button is pressed.
* It can be used to simulate a press of the button as well.
* \param floor The floor that is pressed, starting at 1
*/
void CElevatorWnd::PressPanelFloor(int floor)
{
    stringstream str;
    str << "Panel floor " << floor << " pressed";
    mController->Speak(str.str());
    mController->OnPanelFloorPressed(floor);
}

/** \brief Press a call up button on a floor
*
* This function is called when the button is pressed.
* It can be used to simulate a press of the button as well.
* \param floor The floor the button is on, starting at 1
*/
void CElevatorWnd::PressCallUp(int floor)
{
    stringstream str;
    str << "Floor " << floor << " call up pressed";
    mController->Speak(str.str());
    mController->OnCallUpPressed(floor);
}


/** \brief Press a call down button on a floor
*
* This function is called when the button is pressed.
* It can be used to simulate a press of the button as well.
* \param floor The floor the button is on, starting at 1
*/
void CElevatorWnd::PressCallDown(int floor)
{
    stringstream str;
    str << "Floor " << floor << " call down pressed";
    mController->Speak(str.str());
    mController->OnCallDownPressed(floor);
}

/** \brief Press the fire mode button
*
* This function is called when the button is pressed.
* It can be used to simulate a press of the button as well.
*/
void CElevatorWnd::PressFireMode()
{
    mFireMode = !mFireMode;
    if (mFireMode)
    {
        mController->Speak("Fire mode enabled");
    }
    else
    {
        mController->Speak("Fire mode disabled");
    }
}


/**
* \brief Set the brake
* \param b true for brake enabled
*/
void CElevatorWnd::SetBrake(bool b)
{
    mBrake = b;
    if (mController != NULL) {
        stringstream str;
        if (mBrake)
        {
            str << "Brake enabled, position " << mCarPosition << ends;
        }
        else
        {
            str << "Brake disabled, position " << mCarPosition << ends;
        }
        mController->Speak(str.str());
    }
}


/**
* \brief Set the state of a floor light in the elevator panel
* \param floor Floor to test, starting at 1
* \param s true for light on
*/
void CElevatorWnd::SetPanelFloorLight(int floor, bool s)
{
    assert(floor >= 1 && floor <= 3);
    if (mController != nullptr)
    {
        stringstream str;
        str << "Panel floor light " << floor << (s ? " on" : " off") << ends;
        mController->Speak(str.str());
    }

    mPanelFloorLights[floor - 1] = s;
}


/**
* \brief Get the state of a floor light in the elevator panel
* \param floor Floor to test, starting at 1
* \returns true if light is on
*/
bool CElevatorWnd::GetPanelFloorLight(int floor) const
{
    assert(floor >= 1 && floor <= 3);
    return mPanelFloorLights[floor - 1];
}


/**
* \brief Set a call light for a floor
* \param floor Floor to test, starting at 1
* \param d Direction selection (which light)
* \param s true for light on
*/
void CElevatorWnd::SetCallLight(int floor, CElevatorController::Direction d, bool s)
{
    assert(floor >= 1 && floor <= 3);
    mCallLights[floor - 1][d] = s;

    if (mController != nullptr)
    {
        stringstream str;
        str << "Floor " << floor << (d == CElevatorController::Up ? " up" : " down") << " call light " << (s ? " on" : " off") << ends;
        mController->Speak(str.str());
    }

}

/** \brief Get the state of a call light for a floor
* \param floor Floor to test, starting at 1
* \param d Direction selection (which light)
* \returns true if light is on */
bool CElevatorWnd::GetCallLight(int floor, CElevatorController::Direction d) const
{
    assert(floor >= 1 && floor <= 3);
    return mCallLights[floor - 1][d];
}

/**
* \brief Set the status of a door indicator light
* \param floor Floor to test, starting at 1
* \param d Direction selection (which light)
* \param s true for light on
*/
void CElevatorWnd::SetDoorIndicator(int floor, CElevatorController::Direction d, bool s)
{
    assert(floor >= 1 && floor <= 3);
    mDoorLights[floor - 1][d] = s;

    if (mController != nullptr)
    {
        stringstream str;
        str << "Floor " << floor << (d == CElevatorController::Up ? " up" : " down") << " door indicator " << (s ? " on" : " off") << ends;
        mController->Speak(str.str());
    }
}

/** \brief Get the state of a door indicator light
* \param floor Floor to test, starting at 1
* \param d Direction selection (which light)
* \returns true if light is on */
bool CElevatorWnd::GetDoorIndicator(int floor, CElevatorController::Direction d) const
{
    assert(floor >= 1 && floor <= 3);
    return mDoorLights[floor - 1][d];
}


/**
* \brief Set the motor power.
*
* Limits the power to the normalized range -1 to 1.
* \param p Power in range -1 to 1
*/
void CElevatorWnd::SetMotorSpeed(double p)
{
    /* This is protection from an invalid input */
    assert(p >= -1 && p <= 1);

    if (mController != NULL) {
        stringstream str;
        str << "Motor speed set to " << p << " position " << mCarPosition << ends;
        mController->Speak(str.str());
    }

    mMotorSpeed = p;
}


/**
* \brief Set the door motor speed
* \param floor Floor we are setting, starting at 1
* \param speed Speed in range -1 to 1
*/
void CElevatorWnd::SetDoorMotor(int floor, double speed)
{
    if (mController != NULL) {
        stringstream str;
        str << "Floor " << floor << " door motor set to " << speed << ends;
        mController->Speak(str.str());
    }

    mDoors[floor - 1].SetMotor(speed);
}


/**
* \brief Is the door on a floor open?
* \param floor Floor we are setting, starting at 1
* \returns true if door is open.
*/
bool CElevatorWnd::IsDoorOpen(int floor) const
{
    return mDoors[floor - 1].GetOpen() >= 1;
}



/**
* \brief Is the door on a floor closed?
* \param floor Floor we are setting, starting at 1
* \returns true if door is closed.
*/
bool CElevatorWnd::IsDoorClosed(int floor) const
{
    return mDoors[floor - 1].GetOpen() <= 0;
}

/**
* \brief Change the elevator car loading
*/
void CElevatorWnd::ChangeLoading()
{
    // New passenger count
    int r = rand();
    for (int j = 0; j<PassengersCnt; j++)
    {
        mPassengers[j] = (r & 1) == 1;
        r >>= 1;
    }

    SayPassengers();
}

/**
* \brief Set the loading in passengers.
* \param r A bit field, with one bit for each passenger.
*/
void CElevatorWnd::SetLoading(int r)
{
    // New passenger count
    for (int j = 0; j<PassengersCnt; j++)
    {
        mPassengers[j] = (r & 1) == 1;
        r >>= 1;
    }

    SayPassengers();
}

/// \cond

afx_msg int CElevatorWnd::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
    //
    // Load all of the images
    //
    LoadImage(mBackground, L"ElevatorLib/images/ElevatorBackground.png");
    LoadImage(mCar, L"ElevatorLib/images/Car.png");
    LoadImage(mDoorFrame, L"ElevatorLib/images/DoorFrame.png");
    LoadImage(mDoorUp, L"ElevatorLib/images/DoorUp.png");
    LoadImage(mDoorDown, L"ElevatorLib/images/DoorDown.png");
    LoadImage(mDoorLeft, L"ElevatorLib/images/DoorLeft.png");
    LoadImage(mDoorRight, L"ElevatorLib/images/DoorRight.png");
    LoadImage(mElevatorPanel, L"ElevatorLib/images/ElevatorPanel.png");
    LoadImage(mButtonLight, L"ElevatorLib/images/ButtonLight.png");
    LoadImage(mCallButton, L"ElevatorLib/images/CallButton.png");
    LoadImage(mCallButton1, L"ElevatorLib/images/CallButton1.png");
    LoadImage(mMotorRed, L"ElevatorLib/images/MotorRed.png");
    LoadImage(mMotorSmoke, L"ElevatorLib/images/MotorSmoke.png");
    LoadImage(mMotorFire, L"ElevatorLib/images/MotorFire.png");
    LoadImage(mSparty, L"ElevatorLib/images/Sparty.png");
    LoadImage(mNoel, L"ElevatorLib/images/Noel.png");
    LoadImage(mNeil, L"ElevatorLib/images/Neil.png");
    LoadImage(mFireActive, L"ElevatorLib/images/FireActive.png");

    return CWnd::OnCreate(lpCreateStruct);
}


/** \brief Load an image into a local bitmap pointer 
 * \param bitmap Reference to unique_ptr we load into
 * \param name The filename to load */
void CElevatorWnd::LoadImage(std::unique_ptr<Gdiplus::Bitmap> &bitmap, const wchar_t *name)
{
    bitmap = unique_ptr<Bitmap>(Bitmap::FromFile(name));
    if (bitmap->GetLastStatus() != Ok)
    {
        wstring msg(L"Failed to open: ");
        msg += name;
        AfxMessageBox(msg.c_str());
    }
}


/** \brief Load an image into a local bitmap pointer
* \param bitmap Reference to shared_ptr we load into
* \param name The filename to load */
void CElevatorWnd::LoadImage(std::shared_ptr<Gdiplus::Bitmap> &bitmap, const wchar_t *name)
{
    bitmap = shared_ptr<Bitmap>(Bitmap::FromFile(name));
    if (bitmap->GetLastStatus() != Ok)
    {
        wstring msg(L"Failed to open: ");
        msg += name;
        AfxMessageBox(msg.c_str());
    }
}



/** \brief Paint the elevator window */
void CElevatorWnd::OnPaint()
{
    if (mFirstDraw)
    {
        SetTimer(1, FrameDuration, nullptr);
        mFirstDraw = false;
    }

    // Handle time management for the simulation
    TimeManagement();

    CPaintDC paintDC(this); // device context for painting

    CDoubleBufferDC dbDC(&paintDC);

    Graphics graphics(dbDC.m_hDC);    // Create GDI+ graphics context

    if (mFirstDraw)
    {
        mFirstDraw = false;
    }

    const double RedTemp = 300;
    const double SmokeTemp = 700;
    const double FireTemp = 1000;

    int y;

    int w, h;
    CRect rect;
    GetClientRect(&rect);
    w = rect.Width();
    h = rect.Height();
    int bottom = mBackground->GetHeight();

    //
    // Background
    //

    Draw(&graphics, mBackground, 0, 0);

    //
    // Motor temperature
    //

    if (mMotorTemperature > FireTemp)
    {
        Draw(&graphics, mMotorFire, 128, 0);
    }
    else if (mMotorTemperature > SmokeTemp)
    {
        Draw(&graphics, mMotorSmoke, 128, 0);
    }
    else if (mMotorTemperature > RedTemp)
    {
        Draw(&graphics, mMotorRed, 128, 0);
    }

    //
    // Car
    //

    y = int(CarBottom - mCarPosition / CElevatorController::FloorSpacing * FloorHeight);
    Draw(&graphics, mCar, Car1Left, y - mCar->GetHeight());
    Draw(&graphics, mCar, Car2Left, y - mCar->GetHeight());

    //
    // Passengers
    //

    for (int i = 0; i < PassengersCnt; i++)
    {
        if (mPassengers[i])
        {
            Gdiplus::Bitmap *bm = NULL;
            int x;
            switch (i)
            {
            case Sparty:
                bm = mSparty.get();
                x = 15;
                break;

            case Neil:
                bm = mNeil.get();
                x = 7;
                break;

            case Noel:
                bm = mNoel.get();
                x = 48;
                break;
            }

            if (bm != NULL)
            {
                int h = bm->GetHeight();
                graphics.DrawImage(bm, Car1Left + x, y - h - 10, bm->GetWidth(), bm->GetHeight());
                graphics.DrawImage(bm, Car2Left + x, y - h - 10, bm->GetWidth(), bm->GetHeight());
            }
        }
    }


    //
    // Elevator panel
    //

    int x = 0;
    if (mCarPosition < 0)
    {
        y = bottom;
    }
    else if (mCarPosition > CElevatorController::FloorSpacing * 2)
    {
        y = int(bottom - 2 * FloorHeight);
    }
    else
    {
        y = int(bottom - mCarPosition / CElevatorController::FloorSpacing * FloorHeight);
    }

    int top = y - mElevatorPanel->GetHeight();
    Draw(&graphics, mElevatorPanel, x, top);
    mButtonCenters[BClose] = Point(x + 74 + ButtonRadius, top + 145 + ButtonRadius);
    mButtonCenters[BOpen] = Point(x + 11 + ButtonRadius, top + 145 + ButtonRadius);

    if (mDoorCloseLight)
    {
        DrawButtonLight(&graphics, BClose);
    }

    if (mDoorOpenLight)
    {
        DrawButtonLight(&graphics, BOpen);
    }

    static int ButtonY[] = { 98, 52, 7 };

    for (int i = 0; i<NumFloors; i++)
    {
        mButtonCenters[BEP1 + i] = Point(x + 43 + ButtonRadius, top + ButtonY[i] + ButtonRadius);
        if (mPanelFloorLights[i])
        {
            DrawButtonLight(&graphics, BEP1 + i);
        }
    }


    //
    // Doors
    //

    for (int i = 0; i<NumFloors; i++)
    {

        mDoors[i].Draw(&graphics, DoorFrameLeft, bottom - FloorHeight * i);
    }

    //
    // Door Frames
    //

    for (int i = 0; i<NumFloors; i++)
    {
        int y = bottom - mDoorFrame->GetHeight() - FloorHeight * i;
        Draw(&graphics, mDoorFrame, DoorFrameLeft, y);

        if (mDoorLights[i][CElevatorController::Up])
        {
            Draw(&graphics, mDoorUp, DoorFrameLeft + DoorUpLeft, y + DoorIndicatorTop);
        }

        if (mDoorLights[i][CElevatorController::Down])
        {
            Draw(&graphics, mDoorDown, DoorFrameLeft + DoorDownLeft, y + DoorIndicatorTop);
        }
    }

    //
    // Call buttons
    //

    for (int i = 0; i<NumFloors; i++)
    {
        int y = 484 - i * FloorHeight;
        int x = 342;
        int b = BCall1Up + i * 2;
        if (i == 0)
        {
            Draw(&graphics, mCallButton1, x, y);
            mButtonCenters[b] = Point(x + 16 + ButtonRadius, y + 10 + ButtonRadius);
            mButtonCenters[b + 1] = Point(x + 16 + ButtonRadius, y + 60 + ButtonRadius);
            mButtonCenters[BFire] = Point(x + 16 + ButtonRadius, y + 111 + ButtonRadius);

            if (mFireMode)
            {
                Draw(&graphics, mFireActive, x + 28, y + 122);
            }

            //DrawButtonLight(&dc, BFire);
        }
        else
        {
            Draw(&graphics, mCallButton, x, y);
            mButtonCenters[b] = Point(x + 16 + ButtonRadius, y + 20 + ButtonRadius);
            mButtonCenters[b + 1] = Point(x + 16 + ButtonRadius, y + 89 + ButtonRadius);
        }

        if (mCallLights[i][CElevatorController::Up])
        {
            DrawButtonLight(&graphics, b);
        }

        if (mCallLights[i][CElevatorController::Down])
        {
            DrawButtonLight(&graphics, b + 1);
        }
    }

    //
    // Gauges
    //

    DrawGaugeLine(&graphics, Point(34, 27), mCarVelocity, 1.33333);
    DrawGaugeLine(&graphics, Point(92, 27), mMotorTemperature, SmokeTemp * 1.33333);

    if (mBrakeFail)
    {
        FontFamily fontFamily(L"Arial");
        Gdiplus::Font font(&fontFamily, 18, FontStyle::FontStyleBold, Unit::UnitPixel);

        RectF bbox;
        graphics.MeasureString(L"Brake Failure", -1, &font, PointF(0, 0), &bbox);
        int w = (int)bbox.Width;
        int h = (int)bbox.Height;

        SolidBrush  black(Color(255, 0, 0));
        graphics.DrawString(L"Brake Failure",  // String to draw
            -1,         // String length, -1 means it figures it out on its own
            &font,      // The font to use
            PointF((float)(64 - w / 2), (float)(27 - h / 2)),   // Where to draw (top left corner)
            &black);    // The brush to draw the text with
    }

}


/**
 * \brief Draw a bitmap it's native size.
 * \param bitmap Bitmap declared as a unique pointer to a Gdiplus::Bitmap
 * \param x X location in pixels
 * \param y Y location in pixels
 */
void CElevatorWnd::Draw(Gdiplus::Graphics *graphics, std::unique_ptr<Gdiplus::Bitmap> &bitmap, int x, int y)
{
    graphics->DrawImage(bitmap.get(), x, y, bitmap->GetWidth(), bitmap->GetHeight());
}



/** \brief Handle once-per-draw time management 
 *
 * Handles updating the time management component and
 * calls to the local Update function.
 */
void CElevatorWnd::TimeManagement()
{
    mTimeManagement.Update();

    while (mTime < mTimeManagement.GetTime())
    {
        UpdateTimeslice();
        mTime += SimulationTimeslice;
    }

}



/** \brief Update in time
 */
void CElevatorWnd::UpdateTimeslice()
{
    double elapsed = SimulationTimeslice;

    mUserTimer += elapsed;

    if (mController != nullptr)
    {
        mController->Service();
    }

    MotorSimulation(elapsed);

    // Passenger and door management
    for (int i = 0; i < NumFloors; i++)
    {
        if (mDoors[i].Advance(elapsed))
        {
            // Door has been opened all of the way.
            // Are we on a floor?
            if (fabs(mCarPosition / CElevatorController::FloorSpacing - i) <=
                CElevatorController::FloorTolerance / CElevatorController::FloorSpacing)
            {
                // New passenger count
                ChangeLoading();
            }
        }
    }

}

/*
* Name :        CElevator::OnTimer()
* Description : Handle a timer event
* Parameters :  nIDEvent the timer event id
* Returns :     none
*/
void CElevatorWnd::OnTimer(UINT_PTR nIDEvent)
{
    Invalidate();
    UpdateWindow();

    CWnd::OnTimer(nIDEvent);
}




/// \cond



/**
 * \brief Handle a left mouse button press
 * \param nFlags Flags associated with the mouse
 * \param point Where the mouse was clicked.
 */
void CElevatorWnd::OnLButtonDown(UINT nFlags, CPoint point)
{
    // Top right area for loading change
    if (point.x > 328 && point.y < 48)
    {
        ChangeLoading();
        return;
    }

    for (int b = 0; b<BButtonCount; b++)
    {
        if ((SQR(mButtonCenters[b].X - point.x) + SQR(mButtonCenters[b].Y - point.y)) <
            (ButtonRadius * ButtonRadius))
        {
            OnButton(b);
            break;
        }
    }

    CWnd::OnLButtonDown(nFlags, point);
}





/**
* Handle an actual button press
*
* This function maps a button press to an appropriate
* controller press function.
* @param b The button code
*/
void CElevatorWnd::OnButton(int b)
{
    if (mController != NULL)
    {
        switch (b)
        {
        case BOpen:
            PressOpen();
            break;

        case BClose:
            PressClose();
            break;

        case BEP1:
            PressPanelFloor(1);
            break;

        case BEP2:
            PressPanelFloor(2);
            break;

        case BEP3:
            PressPanelFloor(3);
            break;

        case BCall1Up:
            PressCallUp(1);
            break;


        case BCall2Up:
            PressCallUp(2);
            break;


        case BCall3Up:
            PressCallUp(3);
            break;

        case BCall1Down:
            PressCallDown(1);
            break;

        case BCall2Down:
            PressCallDown(2);
            break;

        case BCall3Down:
            PressCallDown(3);
            break;

        case BFire:
            PressFireMode();
            break;
        }
    }
}



/**
 * \brief Draw a line on a gauge
 * \param graphics Graphics device to draw on
 * \param center Center of the guage
 * \param value Value to draw
 * \param max Maximum value for this gauge
 */
void CElevatorWnd::DrawGaugeLine(Gdiplus::Graphics *graphics, Gdiplus::Point center, double value, double max)
{
    value = fabs(value);
    if (value > max)
        value = max;

    Pen pen(Color::Black);
    double angle = -3.665 + 4.1888 * value / max;
    double len = 20;
    int tx = int(center.X + len * cos(angle));
    int ty = int(center.Y + len * sin(angle));
    graphics->DrawLine(&pen, center.X, center.Y, tx, ty);
}


/**
 * \brief Draw a button light
 * \param graphics Graphics device to draw on
 * \param button Button code to draw
 */
void CElevatorWnd::DrawButtonLight(Gdiplus::Graphics *graphics, int button)
{
    Point &point = mButtonCenters[button];
    Draw(graphics, mButtonLight, point.X - ButtonRadius, point.Y - ButtonRadius);
}



double CElevatorWnd::GetCarMass()
{
    // Weight of the car
    const double CarMass = 453;             // Kilograms
    static const double PassengerMass[PassengersCnt] = { 61, 91, 136 };

    double weight = CarMass;
    for (int i = 0; i<PassengersCnt; i++)
    {
        if (mPassengers[i])
            weight += PassengerMass[i];
    }

    return weight;
}


void CElevatorWnd::SayPassengers()
{
    if (mController != nullptr) {
        stringstream str;
        str << "Passenger load ";
        bool any = false;
        if (mPassengers[Sparty])
        {
            str << "Sparty ";
            any = true;
        }
        if (mPassengers[Neil])
        {
            str << "Neil ";
            any = true;
        }
        if (mPassengers[Noel])
        {
            str << "Noel ";
            any = true;
        }
        if (!any)
        {
            str << "Empty";
        }
        str << ends;
        mController->Speak(str.str());
    }
}


void CElevatorWnd::MotorSimulation(double delta)
{
    const double CounterweightMass = 679;   // Kilograms
    const double MotorVelocity = 1;       // m / sec

    // Force of gravity
    const double Gravity = -9.81;    // m / sec^2

    // Drag coefficient
    const double DragCoeff = 100;

    // Motor resistance if brake is on
    const double MaxMotorResistance = 50000;

    // Normal motor resistance
    const double NormalMotorResistance = 5000;

    //
    // Car physics
    //

    double motorResistance = 0;
    if (mBrake && !mBrakeFail)
    {
        mCarVelocity = 0;
        motorResistance = MaxMotorResistance;
    }
    else
    {
        if (mMotorSpeed != 0)
        {
            // Fixed motor speed
            mCarVelocity = mMotorSpeed * MotorVelocity;
            motorResistance = NormalMotorResistance;
        }
        else
        {
            // Brake off an no motor running
            double carMass = GetCarMass();

            // Force due to mass of the car
            // Newtons
            double force = carMass * Gravity;

            // Force due to the counterweight
            // Opposite direction, of course
            force -= CounterweightMass * Gravity;

            // Force due to drag
            force += mCarVelocity * -DragCoeff;

            // Acceleration
            double a = force / (carMass + CounterweightMass);

            // Euler step
            mCarVelocity += a * delta;
        }

        // Update the position
        mCarPosition += (mCarVelocity * delta);

        if (mCarPosition > MaxHeight)
        {
            // We have hit the top
            mCarVelocity = 0;
            mCarPosition = MaxHeight;
            motorResistance = MaxMotorResistance;
        }
        else if (mCarPosition < MinHeight)
        {
            // We have hit the bottom
            mCarVelocity = 0;
            mCarPosition = MinHeight;
            motorResistance = MaxMotorResistance;
        }
    }

    //
    // Motor Temperature
    //

    const double PowerFactor = 0.005;
    const double Dissipation = 0.1;
    double power = fabs(mMotorSpeed) * fabs(motorResistance) * PowerFactor;
    double disp = 0;
    if (mMotorTemperature > 30)
    {
        disp = (mMotorTemperature - 30) * Dissipation;
    }

    mMotorTemperature += (power - disp) * delta;

    //
    // Motion reporting to the speak interface
    //
    if (!mBrake || mBrakeFail)
    {
        mMotionReport += delta;
        if (mMotionReport > 1.5)
        {
            stringstream str;
            str << "Car position " << mCarPosition << ends;
            if (mController != NULL)
            {
                mController->Speak(str.str());
            }

            mMotionReport = 0;
        }

    }
    else
    {
        mMotionReport = 0;
    }
}



bool CElevatorWnd::Door::Advance(double delta)
{
    // Set true if a door is opened all of the way
    bool alltheway = false;;

    if (mMotor != 0)
    {
        // We are opening the door
        mOpen += delta / DoorOpenTime * mMotor;
        if (mOpen >= 1)
        {
            mOpen = 1;
            alltheway = true;
            mMotor = 0;
        }
        else if (mOpen <= 0)
        {
            mOpen = 0;
            mMotor = 0;
        }
    }

    return alltheway;
}


/**
* \brief Draw an elevator door
* @param graphics The graphics device to draw on
* @param x x location of door
* @param y y location of door
*/
void CElevatorWnd::Door::Draw(Gdiplus::Graphics *graphics, int x, int y)
{
    double open = mOpen;

    if (open < 1)
    {
        /*
         * Left door
         */
        int wid = mUser->mDoorLeft->GetWidth();
        int hit = mUser->mDoorLeft->GetHeight();
        int move = int(open * wid);          // How much we move the door left

        auto save = graphics->Save();
            
        Rect clip(x + 7, y - 9 - hit, wid - move, hit);
        graphics->SetClip(clip);

        graphics->DrawImage(mUser->mDoorLeft.get(),
            x + 7 - move, y - 9 - hit,
            wid, hit);

        graphics->Restore(save);

        /*
         * Right door
         */
        wid = mUser->mDoorRight->GetWidth();
        hit = mUser->mDoorRight->GetHeight();
        move = int(open * wid);          // How much we move the door left

        save = graphics->Save();

        Rect clipR(x + 88 - wid + move, y - 9 - hit, wid - move, hit);
        graphics->SetClip(clipR);

        graphics->DrawImage(mUser->mDoorRight.get(),
            x + 88 - wid + move, y - 9 - hit,
            wid, hit);

        graphics->Restore(save);
    }

}

#if 0

/**
* \brief Handle a key down message
* @param event Key event
*/
void CElevator::OnKeyDown(wxKeyEvent &event)
{
    // The keys used are:
    //
    // O - Open
    // C - Close
    // 1 - Panel floor 1
    // 2 - Panel floor 2
    // 3 - Panel floor 3
    // 4 - Call up floor 1
    // 5 - Call down floor 1
    // 6 - Call up floor 2
    // 7 - Call down floor 2
    // 8 - Call up floor 3
    // 9 - Call down floor 3
    // F - Fire mode
    // L - Change loading
    // ESC - Closes the program
    //

    switch (event.GetKeyCode())
    {
    case 'O':
        OnButton(BOpen);
        break;

    case 'C':
        OnButton(BClose);
        break;

    case '1':
        OnButton(BEP1);
        break;

    case '2':
        OnButton(BEP2);
        break;

    case '3':
        OnButton(BEP3);
        break;

    case '4':
        OnButton(BCall1Up);
        break;

    case '6':
        OnButton(BCall2Up);
        break;

    case '8':
        OnButton(BCall3Up);
        break;

    case '5':
        OnButton(BCall1Down);
        break;

    case '7':
        OnButton(BCall2Down);
        break;

    case '9':
        OnButton(BCall3Down);
        break;

    case 'F':
        OnButton(BFire);
        break;

    case 'L':
        ChangeLoading();
        break;

    case WXK_ESCAPE:
        GetParent()->Close();
        break;
    }
}

#endif

/// \endcond

