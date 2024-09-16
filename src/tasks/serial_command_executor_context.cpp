#include "tasks/serial_command_executor_context.hpp"
#include "global_constants.hpp"
#include "global_variables.hpp"


void SerialCommandExecutorContext::onExecute()
{
    if(this->_stream->available() > 0){
        this->_stream->printf("%d\n", this->_stream->read());
    }

}
