/**
 * \file Controller.cpp
 *
 * \author Stephan Hutecker
 */

#include "stdafx.h"
#include "Controller.h"

/// The time the door remains open
const double DoorOpenTime = 2.0;
/// Time to wait to open the door
const double WaitTime = 1.0;

/**
* \brief constructor
*/
CController::CController()
{
	for (int f = 1; f <= NumFloors; f++)
	{
		mFloors[f - 1].SetController(this);
		mFloors[f - 1].SetFloor(f);
	}
}

/**
 * \brief destructor
 */
CController::~CController()
{
}

/** \brief This function is called when the open button is pressed.
*/
void CController::OnOpenPressed()
{
	switch (mState)
	{
	case Idle:
		// Transition to the DoorOpening state
		SetState(DoorOpening);
		break;

	case DoorOpen:
		SetState(DoorOpen);
		break;

	case DoorClosing:
		SetState(DoorOpening);
		break;

	default:
		break;
	}
}

/** \brief Set a state
* \param state The new state to set
*/
void CController::SetState(States state)
{
	mState = state;
	mStateTime = 0;

	// Entry activities for states
	switch (mState)
	{
	case Idle:
		SetDoorMotor(mFloor, 0);
		break;

	case DoorOpening:
		SetDoorMotor(mFloor, 1);

		WhatFloorToGoTo();
		mFloors[mFloor - 1].SetPanel(false);
		if (mGoingUp)
		{
			mFloors[mFloor - 1].SetUp(false);
		}
		else
		{
			mFloors[mFloor - 1].SetDown(false);
		}

		break;

	case DoorOpen:
		SetDoorMotor(mFloor, 0);
		break;

	case DoorClosing:
		SetDoorMotor(mFloor, -1);
		break;

	case Moving:
		SetBrake(false);
		SetMotorSpeed(mGoingUp ? 1 : -1);
		break;

	case Stop:
		SetMotorSpeed(0);
		SetBrake(true);
		break;

	case FiremanDoorOpen:
		SetDoorMotor(mFloor, 0);
		break;

	case FiremanDoorClosing:
		SetDoorMotor(mFloor, -1);
		break;

	case FiremanMoving:
		SetBrake(false);
		SetMotorSpeed(-1);
		break;

	case FiremanDoorOpening:
		SetDoorMotor(mFloor, 1);
		break;

	case FiremanStop:
		SetMotorSpeed(0);
		SetBrake(true);
		break;

	default:
		break;
	}
}

/** \brief Elevator service function
*
* This function is called once every 0.001 seconds and
* allows us to control elevator functionality.
*/
void CController::Service()
{
	// Increment state time by 1 millisecond
	mStateTime += 0.001;

	IsFireMode() ? mFireMode = true : mFireMode = false;

	switch (mState)
	{
	case DoorOpening:
		if (IsDoorOpen(mFloor))
		{
			SetState(DoorOpen);
		}
		break;

	case DoorClosing:
		if (IsDoorClosed(mFloor))
		{
			SetState(Idle);
		}
		break;

	case DoorOpen:
		if (mStateTime >= DoorOpenTime)
		{
			SetState(DoorClosing);
		}
		else if (mFireMode && mFloor == 1)
		{
			SetState(FiremanDoorOpen);
		}
		else if (mFireMode && mFloor != 1)
		{
			SetState(FiremanDoorClosing);
		}
		break;

	case Idle:
	{
		if (mFireMode && mFloor != 1)
		{
			SetState(FiremanMoving);
		}
		else if (mFireMode && mFloor == 1)
		{
			SetState(FiremanDoorOpening);
		}

		int floor = WhatFloorToGoTo();
		if (floor == mFloor)
		{
			// Button pressed on this floor. Open the door
			SetState(DoorOpening);
		}
		else if (floor != 0)
		{
			SetState(Moving);
		}
	}

	case Moving:
	{
		if (mFireMode)
		{
			SetState(FiremanMoving);
		}

		int floor = WhatFloorToGoTo();
		//assert(floor != 0); This was breaking my program

		// What's the position for that floor?
		double floorPosition = (floor - 1) * FloorSpacing;
		if (fabs(GetPosition() - floorPosition) < FloorTolerance)
		{
			mFloor = floor;
			SetState(Stop);
		}
	}
		break;

	case Stop:
	{
		if (mStateTime >= WaitTime)
		{
			SetState(DoorOpening);
		}
		else if (mFireMode)
		{
			SetState(FiremanMoving);
		}
	}
		break;

	case FiremanDoorOpen:
	{
		if (!mFireMode)
		{
			SetState(DoorClosing);
		}
	}
		break;

	case FiremanDoorClosing:
	{
		if (!mFireMode)
		{
			SetState(DoorClosing);
		}
		else if (IsDoorClosed(mFloor))
		{
			SetState(FiremanMoving);
		}
	}
		break;

	case FiremanMoving:
	{
		int floor = 1;
		//assert(floor != 0); This was breaking my program

		// What's the position for that floor?
		double floorPosition = (floor - 1) * FloorSpacing;
		if (fabs(GetPosition() - floorPosition) < FloorTolerance)
		{
			mFloor = floor;
			SetState(Stop);
		}

		if (!mFireMode)
		{
			SetState(Moving);
		}
		else if (fabs(GetPosition() - floorPosition) < FloorTolerance)
		{
			mFloor = floor;
			SetState(FiremanStop);
		}
	}
		break;

	case FiremanDoorOpening:
	{
		if (IsDoorOpen(mFloor))
		{
			SetState(FiremanDoorOpen);
		}
	}
		break;

	case FiremanStop:
	{
		if (mStateTime >= WaitTime)
		{
			SetState(FiremanDoorOpening);
		}
	}
		break;

	default:
		break;
	}
}

/** \brief This function is called when the door close button is pressed.
*/
void CController::OnClosePressed()
{
	switch (mState)

	{
	case DoorOpen:
		SetState(DoorClosing);
		break;

	case DoorOpening:
		SetState(DoorClosing);
		break;

	default:
		break;
	}
}

/**
 * \brief controlls what happens when the panel is pressed
 * \param floor what floor was pressed
 */
void CController::OnPanelFloorPressed(int floor)
{
	mFloors[floor - 1].SetPanel(true);
}

/**
 * \brief controlls what happened when up is pressed
 * \param floor the floor up was pressed on
 */
void CController::OnCallUpPressed(int floor)
{
	mFloors[floor - 1].SetUp(true);
}

/**
 * \brief controlls what happens when down is pressed
 * \param floor the floor down was pressed on
 */
void CController::OnCallDownPressed(int floor)
{
	mFloors[floor - 1].SetDown(true);
}

/**
 * \brief what floor we want to go to
 * \returns the floor to go to
 */
int CController::WhatFloorToGoTo()
{
	if (mGoingUp)
	{
		// We are going up, so try for a floor in that direction
		int floor = WhatFloorUp();
		if (floor != 0)
			return floor;

		// Guess we can't go up, so see if we need to go down
		floor = WhatFloorDown();
		if (floor != 0)
		{
			// Reverse the direction
			mGoingUp = false;
			return floor;
		}
	}
	else
	{
		// We are going down, so try for a floor in that direction
		int floor = WhatFloorDown();
		if (floor != 0)
			return floor;

		// Guess we can't go down, so see if we need to go up
		floor = WhatFloorUp();
		if (floor != 0)
		{
			// Reverse the direction
			mGoingUp = true;
			return floor;
		}
	}

	return 0;
}

/**
 * \brief what floor above ours
 * \returns the floor number
 */
int CController::WhatFloorUp()
{
	// What floor are we currently on?
	// We stop with FloorTolerance of a floor. Suppose I am at position
	// 3.42. That's just above 3.42 - 3.28 = 0.14 above floor 2, but it's within
	// the tolerance, so we think of it as on floor 2.
	int floor = int((GetPosition() + FloorTolerance) / FloorSpacing) + 1;

	// Is there a floor to goto in the up direction that has the panel
	// or the up button pressed?
	for (int f = floor; f <= NumFloors; f++)
	{
		if (mFloors[f - 1].IsUp() || mFloors[f - 1].IsPanel())
			return f;
	}

	// Is there a floor to go to in the up direction that has the down
	// button pressed. We don't look at the current floor, though.
	for (int f = NumFloors; f>floor; f--)
	{
		if (mFloors[f - 1].IsDown())
			return f;
	}

	// If nothing, return 0;
	return 0;
}

/**
 * \brief what floor be low ours
 * \returns the floor number
 */
int CController::WhatFloorDown()
{
	// What floor are we currently on?
	// We stop with FloorTolerance of a floor. Suppose I am at position
	// 3.42. That's just above 3.42 - 3.28 = 0.14 above floor 2, but it's within
	// the tolerance, so we think of it as on floor 2.
	int floor = int((GetPosition() + FloorTolerance) / FloorSpacing) + 1;

	// Is there a floor to goto in the down direction that has the panel
	// or the down button pressed?
	for (int f = floor; f > 0; f--)
	{
		if (mFloors[f - 1].IsDown() || mFloors[f - 1].IsPanel())
			return f;
	}

	// Is there a floor to go to in the down direction that has the up
	// button pressed. We don't look at the current floor, though.
	for (int f = floor - 1; f> 0; f--)
	{
		if (mFloors[f - 1].IsUp())
			return f;
	}

	// If nothing, return 0;
	return 0;
}

/**
 * \brief initializes the elevator
 */
void CController::Initialize()
{

}