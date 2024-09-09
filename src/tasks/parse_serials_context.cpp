#include "tasks/parse_serials_context.hpp"
#include "global_variables.hpp"


void ParseSerialsContext::onExecute()
{
    sbus2.parse();
    enc_boom.parse();
}
