#ifndef TYPUPDATERUSERBASE_H
#define TYPUPDATERUSERBASE_H

#include <stdint.h>

class TypUpdaterUserBase
{
public:
    TypUpdaterUserBase();
    virtual bool abortTransaction(void) = 0;
    virtual void updateProgress(uint32_t ps) = 0;

};

#endif // TYPUPDATERUSERBASE_H
