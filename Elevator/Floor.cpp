/**
 * \file Floor.cpp
 *
 * \author Stephan Hutecker
 */

#include "stdafx.h"
#include "Floor.h"
#include "Controller.h"


/**
 * \brief constructor
 */
CFloor::CFloor()
{
}

/**
 * \brief destructor
 */
CFloor::~CFloor()
{
}

/**
 * \brief sets the controller
 * \param controller the controller to set
 */
void CFloor::SetController(CController* controller)
{
	mController = controller;
}

/** 
* \brief Set the value of Up for a floor.
* \param s The new value for mUp
*/
void CFloor::SetUp(bool s)
{
	mUp = s;
	mController->SetCallLight(mFloor, CElevatorController::Up, mUp);
}

/**
* \brief Set the value of down for a floor.
* \param s The new value for mDown
*/
void CFloor::SetDown(bool s)
{
	mDown = s;
	mController->SetCallLight(mFloor, CElevatorController::Down, mDown);
}

/**
* \brief Set the value of panel for a floor.
* \param s The new value for mPanel
*/
void CFloor::SetPanel(bool p)
{
	mPanel = p;
	mController->SetPanelFloorLight(mFloor, mPanel);
}