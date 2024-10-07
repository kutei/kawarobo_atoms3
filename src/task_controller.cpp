#include "task_controller.hpp"

#include <M5Unified.h>


AbstractRtosTaskContext::AbstractRtosTaskContext(RtosTaskConfigSharedPtr config){
    this->_config = config;
    this->_is_started = false;
}


static void task_executer(void *param){
    auto task_context = (AbstractRtosTaskContextRawPtr)param;
    auto conf = task_context->getConfig();

    // 初期delayを実行
    if(conf->initial != 0) vTaskDelay(conf->initial);

    for(;;){
        task_context->onExecute();
        if(conf->repeated != pdTRUE) break;
        vTaskDelay(conf->period);
    }

    vTaskDelete(conf->thand);
}


int16_t task_start(AbstractRtosTaskContextSharedPtr *task_context, std::size_t size){
    for(int i = 0; i < size; i++){
        auto conf = task_context[i]->getConfig();

        if(conf->start_required){
            task_context[i]->setStarted(true);
            task_context[i]->onActivated();
            xTaskCreatePinnedToCore(
                task_executer,
                conf->name,
                conf->stack_size,
                reinterpret_cast<void *>(task_context[i].get()),
                conf->priority,
                &conf->thand,
                conf->core_id
            );
        }else{
            task_context[i]->setStarted(false);
        }
    }
    return 0;
}
