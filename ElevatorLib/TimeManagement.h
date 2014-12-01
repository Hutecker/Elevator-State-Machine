/**
 * \file TimeManagement.h
 *
 * \author Charles B. Owen
 *
 * \brief This class manages the timing of the simulation
 */


#pragma once

/** \brief Manages the timing of the simulation
 */
class CTimeManagement
{
public:
    /// \cond
    CTimeManagement();
    ~CTimeManagement();

    double Update();

    /** \brief Get the simulation time
     * \returns Time in seconds, starting at zero */
    double GetTime() { return mTime; }

private:
    void Initialize();

    bool   mFirstUpdate = true;
    long long mLastTime = 0;    ///< Last time we read the timer
    double mTimeFreq = 0;       ///< Rate the timer updates
    double mTime = 0;           ///< The current time as far as the system is concerned
    /// \endcond
};

