#ifndef __INCLUDE_TASKS_PARSE_SERIALS_CONTEXT_HPP
#define __INCLUDE_TASKS_PARSE_SERIALS_CONTEXT_HPP

#include "task_controller.hpp"


class ParseSerialsContext : public AbstractRtosTaskContext
{
public:
    ParseSerialsContext(RtosTaskConfigSharedPtr config) : AbstractRtosTaskContext(config) {};
    void onExecute();
};


#endif // __INCLUDE_TASKS_PARSE_SERIALS_CONTEXT_HPP
