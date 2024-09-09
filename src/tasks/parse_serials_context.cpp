#include "tasks/parse_serials_context.hpp"
#include "global_variables.hpp"


void ParseSerialsContext::onExecute()
{
    g_sbus2.parse();
    g_enc_boom.parse();
}
