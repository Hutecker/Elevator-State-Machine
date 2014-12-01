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
	/** \brief Default constructor disabled */
	CController() = delete;
	/** \brief Copy constructor disabled */
	CController(const CController &) = delete;
	/** \brief Assignment operator disabled */
	void operator=(const CController &) = delete;
	virtual ~CController();

};

