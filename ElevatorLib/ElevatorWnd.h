/** \file ElevatorWnd.h
* \brief Elevator simulation class
*
* \author Charles B. Owen
* \version 1.01 11-08-2011 Initial development
* \version 1.02 11-16-2013 Revisions for wxWidgets 2.9.5
* \version 1.03 10-22-2014 New MFC version
*/
#pragma once

#include <vector>
#include <memory>
#include "ElevatorController.h"
#include "TimeManagement.h"


/** \brief Elevator simulation class.
*
* This class is a window that simulates an elevator.
*/
class CElevatorWnd : public CWnd
{

    DECLARE_DYNAMIC(CElevatorWnd)

public:
    CElevatorWnd();
    virtual ~CElevatorWnd();

    /// The number of floors
    static const int NumFloors = 3;

    void Update(double elapsed);

    void SetController(std::shared_ptr<CElevatorController> c);
    void ResetTimer();
    double GetTimer();

    // Button Presses
    void PressOpen();
    void PressClose();
    void PressPanelFloor(int floor);
    void PressCallUp(int floor);
    void PressCallDown(int floor);
    void PressFireMode();

    int GetWidth();
    int GetHeight();

    void SetBrake(bool b);

    // Lights

    /** \brief Set the state of the door open light 
     * \param s New state to set, true = on */
    void SetOpenDoorLight(bool s) { mDoorOpenLight = s; }

    /** \brief Get the state of the door open light
     * \returns true for on */
    bool GetOpenDoorLight() const { return mDoorOpenLight; }

    /** \brief Set the state of the door close light
    * \param s New state to set, true = on */
    void SetCloseDoorLight(bool s) { mDoorCloseLight = s; }

    /** \brief Get the state of the door close light
    * \returns true for on */
    bool GetCloseDoorLight() const { return mDoorCloseLight; }

    void SetPanelFloorLight(int floor, bool s);
    bool GetPanelFloorLight(int floor) const;

    void SetCallLight(int floor, CElevatorController::Direction d, bool s);
    bool GetCallLight(int floor, CElevatorController::Direction d) const;
    void SetDoorIndicator(int floor, CElevatorController::Direction d, bool s);
    bool GetDoorIndicator(int floor, CElevatorController::Direction d) const;

    void SetDoorMotor(int floor, double speed);
    bool IsDoorOpen(int floor) const;
    bool IsDoorClosed(int floor) const;

    void SetMotorSpeed(double p);

    /** \brief Get the current car position
     * \return Position - 0 = first floor, 2 = third floor */
    double GetPosition() const { return mCarPosition; }

    /** \brief Get the current car velocity 
     * \return Current car velocity */
    double GetVelocity() const { return mCarVelocity; }

    /** \brief Is fire mode set?
     * \return true if fire mode is set */
    bool IsFireMode() const { return mFireMode; }

    void ChangeLoading();
    void SetLoading(int i);

    /* 
     * \cond
     * I am disabling some of the documentation for 
     * this class since it is internal to the library
     * and not needed.
     */

    enum Ids { ID_None, ID_Exit = 1, ID_CloseMe };
    enum MotorStates { Normal = 0, Red = 1, Smoke = 2, Fire = 3 };
    enum Passengers { Noel = 0, Neil = 1, Sparty = 2, PassengersCnt = 3 };

    afx_msg void OnPaint();
    afx_msg void OnTimer(UINT_PTR nIDEvent);

protected:
    DECLARE_MESSAGE_MAP()

private:
    void OnButton(int b);
    void TimeManagement();
    void UpdateTimeslice();
    double GetCarMass();
    void MotorSimulation(double delta);
    void Draw(Gdiplus::Graphics *graphics, std::unique_ptr<Gdiplus::Bitmap> &bitmap, int x, int y);

    void DrawButtonLight(Gdiplus::Graphics *graphics, int button);
    void DrawGaugeLine(Gdiplus::Graphics *graphics, Gdiplus::Point center, double value, double max);

    void LoadImage(std::unique_ptr<Gdiplus::Bitmap> &bitmap, const wchar_t *name);
    void LoadImage(std::shared_ptr<Gdiplus::Bitmap> &bitmap, const wchar_t *name);

    void SayPassengers();


    enum Buttons {
        BOpen = 0, BClose = 1, BEP1 = 2, BEP2 = 3, BEP3 = 4, BCall1Up = 5, BCall1Down = 6,
        BCall2Up = 7, BCall2Down = 8, BCall3Up = 9, BCall3Down = 10, BFire = 11,
        BButtonCount = 12
    };

    class Door
    {
    public:
        Door() { mOpen = 0;  mMotor = 0; }

        void SetUser(CElevatorWnd *user) { mUser = user; }
        void SetFloor(int f) { mFloor = f; }

        void SetMotor(double m) { mMotor = m; }

        double GetOpen() const { return mOpen; }

        void Draw(Gdiplus::Graphics *graphics, int x, int y);
        bool Advance(double delta);

    private:
        CElevatorWnd *mUser;
        int mFloor;

        /* Protect */
        double mMotor;          // -1 for closing, 0 for off, 1 for opening
        double mOpen;           // 0 = closed, 1 = open
    };

    std::vector<Gdiplus::Point> mButtonCenters;

    // A controller for this elevator
    std::shared_ptr<CElevatorController> mController;

    bool mFirstDraw = true;


    // Physics simulation
    double  mCarPosition = 0;       ///< 0 = first floor, 2 = third floor
    double  mCarVelocity = 0;       ///< Current car velocity
    double  mMotorSpeed = 0;        ///< Current motor speed -1 to 1
    bool    mBrake = true;          ///< Brakes engaged?
    bool    mBrakeFail = false;     ///< Brake failure
    double  mMotorTemperature = 30; ///< Current motor temperature (Celsius)

    double  mUserTimer = 0;         ///< Timer available to users

    Door mDoors[NumFloors];

    MotorStates mMotorState = Red;  ///< The current motor state

    /// Manage the simulation timing
    CTimeManagement mTimeManagement;

    double   mTime = 0;             ///< Local time
    double   mLastDoorTime = 0;

    // Fire mode
    bool     mFireMode = false;             ///< True if in the fire mode

    // Lights
    bool     mDoorOpenLight = false;        ///< Door open light on panel in car
    bool     mDoorCloseLight = false;       ///< Door close light on panel in car
    bool     mPanelFloorLights[NumFloors];  // Panel in car floor buttons
    bool     mDoorLights[NumFloors][2];     // Lights above the doors
    bool     mCallLights[NumFloors][2];     // Lights in call panel on floors

    bool mPassengers[PassengersCnt];

    std::unique_ptr<Gdiplus::Bitmap>  mBackground;
    std::unique_ptr<Gdiplus::Bitmap>  mCar;
    std::unique_ptr<Gdiplus::Bitmap>  mDoorFrame;
    std::unique_ptr<Gdiplus::Bitmap>  mDoorUp;
    std::unique_ptr<Gdiplus::Bitmap>  mDoorDown;
    std::unique_ptr<Gdiplus::Bitmap>  mElevatorPanel;
    std::unique_ptr<Gdiplus::Bitmap>  mButtonLight;
    std::unique_ptr<Gdiplus::Bitmap>  mCallButton;
    std::unique_ptr<Gdiplus::Bitmap>  mCallButton1;
    std::unique_ptr<Gdiplus::Bitmap>  mFireActive;
    std::unique_ptr<Gdiplus::Bitmap>  mMotorRed;
    std::unique_ptr<Gdiplus::Bitmap>  mMotorSmoke;
    std::unique_ptr<Gdiplus::Bitmap>  mMotorFire;
    std::unique_ptr<Gdiplus::Bitmap>  mSparty;
    std::unique_ptr<Gdiplus::Bitmap>  mNoel;
    std::unique_ptr<Gdiplus::Bitmap>  mNeil;
    std::unique_ptr<Gdiplus::Bitmap>  mDoorLeft;
    std::unique_ptr<Gdiplus::Bitmap>  mDoorRight;

    double   mMotionReport;
public:
    afx_msg void OnLButtonDown(UINT nFlags, CPoint point);

protected:
    afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);

    /// \endcond
};


