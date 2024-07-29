#include "task_controller.hpp"


int16_t task_initialize(RtosTaskConfigRawPtr *task_configs, std::size_t size){
    for(int i = 0; i < size; i++){
        auto conf = task_configs[i];
        if(conf == NULL){
            return -1;
        }
        conf->thand = xTimerCreate(
            conf->name,
            conf->period,
            conf->repeated,
            NULL,
            conf->callback
        );
        if(conf->thand == NULL){
            return -2;
        }
    }
    return 0;
}


int16_t task_start(RtosTaskConfigRawPtr *task_configs, std::size_t size){
    for(int i = 0; i < size; i++){
        auto conf = task_configs[i];
        xTimerStart(conf->thand, conf->initial);
    }
    return 0;
}
