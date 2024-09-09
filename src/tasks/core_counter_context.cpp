#include "tasks/core_counter_context.hpp"
#include "global_constants.hpp"
#include "global_variables.hpp"

void Core1CounterContext::onExecute()
{
    if(core1_alive_count < LOOP_ALIVE_COUNT_THRESHOLD){
        core1_alive_count++;
    }
}
