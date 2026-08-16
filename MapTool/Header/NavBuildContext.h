#pragma once
#include "Recast/Recast.h"

class CNavBuildContext : public rcContext
{
public:
    virtual void doLog(const rcLogCategory category, const char* msg, const int len) override
    {

    }
    
    virtual void doResetTimers() override {}
    virtual void doStartTimer(const rcTimerLabel label) override {}
    virtual void doStopTimer(const rcTimerLabel label) override {}
    virtual int doGetAccumulatedTime(const rcTimerLabel label) const override { return -1; }
};


