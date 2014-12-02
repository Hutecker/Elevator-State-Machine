/**
 * \file Floor.h
 *
 * \author Stephan Hutecker
 *
 * \brief Controls which floor we are on
 */

#pragma once

class CController;

/** \brief Controls which floor we are on */
class CFloor
{
public:
	CFloor();
	/** \brief Copy constructor disabled */
	CFloor(const CFloor &) = delete;
	/** \brief Assignment operator disabled */
	void operator=(const CFloor &) = delete;
	virtual ~CFloor();

	/**
	* \brief get mup
	* \returns mup
	*/
	bool IsUp() { return mUp; }
	void SetUp(bool up);

	/**
	* \brief get mdown
	* \returns mdown
	*/
	bool IsDown() { return mDown; }
	void SetDown(bool down);

	/**
	* \brief get mpanel
	* \returns mpanel
	*/
	bool IsPanel() { return mPanel; }
	void SetPanel(bool panel);

	/**
	* \brief get mfloor
	* \returns mfloor
	*/
	int  GetFloor() { return mFloor; }
	/**
	* \brief set mfloor
	* \param the value of floor
	*/
	void SetFloor(int floor) { mFloor = floor; }

	void SetController(CController* controller);
	void SetUp();

private:
	/// whether we are going up
	bool mUp = false;
	/// whether we are going down
	bool mDown = false;
	/// The elevator panel
	bool mPanel = false;
	/// The floor we are on
	int mFloor = 0;
	CController *mController;   ///< Controller for this object
};

