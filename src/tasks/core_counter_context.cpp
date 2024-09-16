#include "tasks/core_counter_context.hpp"
#include "global_constants.hpp"
#include "global_variables.hpp"

void Core1CounterContext::onExecute()
{
    g_core1_alive_count++;
}
