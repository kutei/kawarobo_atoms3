#include "task_controller.hpp"

#include <Sbus2Reciever.hpp>

#include <Arduino.h>
#include <M5Unified.h>

Sbus2Reciever sbus2;
std::array<RtosTaskConfigRawPtr, 2> task_configs;


void task_parse_sbus2(void *param)
{
    sbus2.parse();
}

void task_draw_display(void *param)
{
    Serial.printf(
        "1:%5d, 2:%5d, 3:%5d, 4:%5d, 5:%5d, 6:%5d, "
        "7:%5d, 8:%5d, 9:%5d,10:%5d, "
        "fs:%1d, lf:%1d\r\n",
        sbus2.getChannel(0),
        sbus2.getChannel(1),
        sbus2.getChannel(2),
        sbus2.getChannel(3),
        sbus2.getChannel(4),
        sbus2.getChannel(5),
        sbus2.getChannel(6),
        sbus2.getChannel(7),
        sbus2.getChannel(8),
        sbus2.getChannel(9),
        sbus2.isFailsafe(),
        sbus2.isLostframe()
    );
}


void setup() {
    // begin M5Unified.
    auto cfg = M5.config();
    M5.begin(cfg);

    // set default font size.
    M5.Display.setTextSize(1);
    M5.Display.print("initializing...\n");

    // debug serial
    Serial.begin(921600);

    // Serial1をSBUS2受信用に設定(8E2で25byte)
    Serial1.begin(100000, SERIAL_8E2, 5, -1, true);
    pinMode(5, INPUT_PULLDOWN);
    int16_t sbus_init = sbus2.begin(&Serial1);
    if(sbus_init != 0){
        M5.Display.printf("sbus2 failed: %d\n", sbus_init);
        while(1);
    };


    task_configs[0] = new RtosTaskConfig_typedef{
        "task_parse_sbus2",
        NULL,
        pdTRUE,
        pdMS_TO_TICKS(10),
        pdMS_TO_TICKS(0),
        task_parse_sbus2
    };
    task_configs[1] = new RtosTaskConfig_typedef{
        "task_draw_display",
        NULL,
        pdTRUE,
        pdMS_TO_TICKS(100),
        pdMS_TO_TICKS(0),
        task_draw_display
    };

    task_initialize(task_configs.data(), task_configs.size());
    task_start(task_configs.data(), task_configs.size());

    M5.Display.print("started\n");
}

void loop() {
    // nothing to do.
    vTaskDelay(1000);
}
