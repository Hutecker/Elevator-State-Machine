/**
 * \file TimeManagement.cpp
 *
 * \author Charles B. Owen
 */

#include "stdafx.h"
#include "TimeManagement.h"

/// \cond

CTimeManagement::CTimeManagement()
{
}


CTimeManagement::~CTimeManagement()
{
}

/** \brief Initialize the time management system.
*
* Called once on first use by Update
*/
void CTimeManagement::Initialize()
{
    /*
    * Initialize the elapsed time system
    */
    LARGE_INTEGER time, freq;
    QueryPerformanceCounter(&time);
    QueryPerformanceFrequency(&freq);

    mTime = 0;
    mLastTime = time.QuadPart;
    mTimeFreq = double(freq.QuadPart);
}


/**
 * \brief Update the time management system.
 * \returns Elapsed tiome since last call to Update in seconds.
 */
double CTimeManagement::Update()
{
    if (mFirstUpdate)
    {
        Initialize();
        mFirstUpdate = false;
    }

    /*
    * Compute the elapsed time since the last draw
    */
    LARGE_INTEGER time;
    QueryPerformanceCounter(&time);
    long long diff = time.QuadPart - mLastTime;
    double elapsed = double(diff) / mTimeFreq;
    mLastTime = time.QuadPart;
    mTime += elapsed;

    return elapsed;
}

/// \endcond
