/**
 * \file Controller.h
 *
 * \author Stephan Hutecker
 *
 * \brief controls our elevator
 */

#pragma once
#include "ElevatorController.h"

/** \brief controls our elevator */
class CController : public CElevatorController
{
public:
	CController();
	/** \brief Copy constructor disabled */
	CController(const CController &) = delete;
	/** \brief Assignment operator disabled */
	void operator=(const CController &) = delete;
	virtual ~CController();

	virtual void OnOpenPressed() override;
	virtual void OnClosePressed() override;
	virtual void Service() override;
	/// The state machine state1s
	enum States { Idle, DoorOpening, DoorOpen, DoorClosing };

private:
	int    mFloor = 1;      ///< The current floor
	States mState = Idle;   ///< The current state
	double mStateTime = 0;  ///< The time in a current state
	void SetState(States state);
};

