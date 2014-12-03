/**
 * \file Controller.h
 *
 * \author Stephan Hutecker
 *
 * \brief controls our elevator
 */

#pragma once
#include "ElevatorController.h"
#include "Floor.h"

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

	void Initialize();
	virtual void OnOpenPressed() override;
	virtual void OnClosePressed() override;
	virtual void OnPanelFloorPressed(int floor) override;
	virtual void OnCallUpPressed(int floor) override;
	virtual void OnCallDownPressed(int floor) override;
	virtual void Service() override;
	int WhatFloorToGoTo();
	int WhatFloorUp();
	int WhatFloorDown();

	/// The state machine state1s
	enum States {
		Idle, DoorOpening, DoorOpen, DoorClosing, Moving, Stop,
		FiremanDoorOpen, FiremanDoorClosing, FiremanMoving, FiremanDoorOpening,
		FiremanStop};

private:
	int    mFloor = 1;      ///< The current floor
	States mState = Idle;   ///< The current state
	double mStateTime = 0;  ///< The time in a current state
	void SetState(States state);
	/// An object for each floor
	CFloor mFloors[NumFloors];
	bool mGoingUp = true; ///< Whether we are going up or not
	bool mFireMode = false; ///<Whether fireman mode is active or not
};

