#include "task_controller.hpp"

#include <M5Unified.h>


#define MAX_CHARS_TASK_NAME 16

static void task_executer(void *param){
    auto conf = (RtosTaskConfigRawPtr)param;

    // 初期delayを実行
    if(conf->initial != 0) vTaskDelay(conf->initial);

    for(;;){
        conf->callback(NULL);
        if(conf->repeated != pdTRUE) break;
        vTaskDelay(conf->period);
    }

    vTaskDelete(conf->thand);
}


int16_t task_start(RtosTaskConfigRawPtr *task_configs, std::size_t size){
    for(int i = 0; i < size; i++){
        auto conf = task_configs[i];

        if(conf->start_required){
            M5.Display.printf("-[T]%.*s\n", MAX_CHARS_TASK_NAME, conf->name);
            xTaskCreatePinnedToCore(
                task_executer,
                conf->name,
                conf->stack_size,
                reinterpret_cast<void *>(conf),
                conf->priority,
                &conf->thand,
                conf->core_id
            );
        }else{
            M5.Display.printf("-[ ]%.*s\n", MAX_CHARS_TASK_NAME, conf->name);
        }
    }
    return 0;
}
