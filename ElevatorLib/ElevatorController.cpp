/**
* \file ElevatorController.cpp
*
* \author Charles B. Owen
*/

#include "stdafx.h"
#include "ElevatorController.h"
#include "ElevatorWnd.h"

#include <cassert>

const double CElevatorController::FloorTolerance = 0.08;
const double CElevatorController::FloorSpacing = 3.28;


/** \cond */
CElevatorController::CElevatorController()
{
}

CElevatorController::~CElevatorController()
{
}


void CElevatorController::SetElevator(CElevatorWnd *elevator)
{
    mElevator = elevator;
}
/** \endcond */

bool CElevatorController::IsFireMode()
{
    assert(mElevator != NULL);
    return mElevator->IsFireMode();
}

void CElevatorController::SetDoorMotor(int floor, double speed)
{
    assert(mElevator != NULL);
    mElevator->SetDoorMotor(floor, speed);
}

bool CElevatorController::IsDoorOpen(int floor) const
{
    assert(mElevator != NULL);
    return mElevator->IsDoorOpen(floor);
}

bool CElevatorController::IsDoorClosed(int floor) const
{
    assert(mElevator != NULL);
    return mElevator->IsDoorClosed(floor);
}

double CElevatorController::GetPosition(void) const
{
    assert(mElevator != NULL);
    return mElevator->GetPosition();
}

double CElevatorController::GetVelocity(void) const
{
    assert(mElevator != NULL);
    return mElevator->GetVelocity();
}

void CElevatorController::ChangeLoading(void)
{
    assert(mElevator != NULL);
    mElevator->ChangeLoading();
}

void CElevatorController::SetLoading(int r)
{
    assert(mElevator != NULL);
    mElevator->SetLoading(r);
}


void CElevatorController::SetOpenDoorLight(bool s)
{
    assert(mElevator != NULL);
    mElevator->SetOpenDoorLight(s);
}

void CElevatorController::SetCloseDoorLight(bool s)
{
    assert(mElevator != NULL);
    mElevator->SetCloseDoorLight(s);
}

bool CElevatorController::GetCloseDoorLight(void) const
{
    assert(mElevator != NULL);
    return mElevator->GetCloseDoorLight();
}

bool CElevatorController::GetOpenDoorLight(void) const
{
    assert(mElevator != NULL);
    return mElevator->GetOpenDoorLight();
}

void CElevatorController::SetPanelFloorLight(int floor, bool s)
{
    assert(mElevator != NULL);
    assert(floor >= 1 && floor <= 3);
    mElevator->SetPanelFloorLight(floor, s);
}

bool CElevatorController::GetPanelFloorLight(int floor) const
{
    assert(mElevator != NULL);
    assert(floor >= 1 && floor <= 3);
    return mElevator->GetPanelFloorLight(floor);
}

void CElevatorController::SetCallLight(int floor, Direction d, bool s)
{
    assert(mElevator != NULL);
    assert(floor >= 1 && floor <= 3);
    mElevator->SetCallLight(floor, d, s);
}

bool CElevatorController::GetCallLight(int floor, Direction d) const
{
    assert(mElevator != NULL);
    assert(floor >= 1 && floor <= 3);
    return mElevator->GetCallLight(floor, d);
}

void CElevatorController::SetDoorIndicator(int floor, Direction d, bool s)
{
    assert(mElevator != NULL);
    assert(floor >= 1 && floor <= 3);
    mElevator->SetDoorIndicator(floor, d, s);
}

bool CElevatorController::GetDoorIndicator(int floor, Direction d) const
{
    assert(mElevator != NULL);
    assert(floor >= 1 && floor <= 3);
    return mElevator->GetDoorIndicator(floor, d);
}



void CElevatorController::SetMotorSpeed(double s)
{
    assert(mElevator != NULL);
    mElevator->SetMotorSpeed(s);
}


void CElevatorController::SetBrake(bool b)
{
    assert(mElevator != NULL);
    mElevator->SetBrake(b);
}




