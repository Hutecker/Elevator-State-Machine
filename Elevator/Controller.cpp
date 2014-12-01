/**
 * \file Controller.cpp
 *
 * \author Stephan Hutecker
 */

#include "stdafx.h"
#include "Controller.h"

/// The time the door remains open
const double DoorOpenTime = 2.0;

/**
* \brief constructor
*/
CController::CController()
{
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
		break;

	case DoorOpen:
		SetDoorMotor(mFloor, 0);
		break;

	case DoorClosing:
		SetDoorMotor(mFloor, -1);
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

	default:
		break;
	}
}