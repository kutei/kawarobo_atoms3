#include "task_controller.hpp"
#include "pwm_out.hpp"
#include "enc_reciever.hpp"

#include <Sbus2Reciever.hpp>

#include <Arduino.h>
#include <M5Unified.h>


/**********************************************************************
 * Control parameters
 *********************************************************************/
#define ROLL_ATTAKING_THRESHOLD 1550


/**********************************************************************
 * Global variables
 *********************************************************************/
Sbus2Reciever sbus2;
EncReciever enc_boom;
PwmOutServo motor_boom;
PwmOutServo motor_roll;

int lcd_bottom_rect_x1, lcd_bottom_rect_x2, lcd_bottom_rect_x3;
int lcd_bottom_rect_y1, lcd_bottom_rect_y2;



/**********************************************************************
 * Task functions
 *********************************************************************/
void task_parse_sbus2(void *param)
{
    sbus2.parse();
}
void task_parse_enc(void *param)
{
    enc_boom.parse();
}

void task_draw_infomation(void *param)
{
    Serial.printf(
        "1:%5d, 2:%5d, 3:%5d, 4:%5d, 5:%5d, 6:%5d, "
        "7:%5d, 8:%5d, 9:%5d,10:%5d, fs:%1d, lf:%1d, enc:%d.%d.%d\r\n",
        sbus2.getChannel(0), sbus2.getChannel(1),
        sbus2.getChannel(2), sbus2.getChannel(3),
        sbus2.getChannel(4), sbus2.getChannel(5),
        sbus2.getChannel(6), sbus2.getChannel(7),
        sbus2.getChannel(8), sbus2.getChannel(9),
        sbus2.isFailsafe(), sbus2.isLostframe(),
        enc_boom.is_initialized(), enc_boom.is_on_upper_side(), enc_boom.get_angle()
    );
}

// 制御ループ
void task_control_loop(void *param)
{
    // rollはそのまま出力
    int16_t roll = sbus2.getChannel(0);
    float roll_std = (roll / 1024.0 - 1.0) * 1.5;
    motor_roll.out(roll_std);

    // rollでの攻撃判定
    bool attacking = roll > ROLL_ATTAKING_THRESHOLD;
}



/**********************************************************************
 * intialize and loop
 *********************************************************************/
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

    // エンコーダ用にSerial2を設定
    Serial2.begin(921600, SERIAL_8N1, 8, -1);
    if(enc_boom.begin(&Serial2) != true){
        M5.Display.printf("enc_boom failed\n");
        while(1);
    }

    // モーター出力を初期化
    motor_boom.begin(38, 0);
    motor_roll.begin(39, 1);

    std::array<RtosTaskConfigRawPtr, 4> task_configs;
    task_configs[0] = new RtosTaskConfig_typedef{
        "task_parse_sbus2",
        NULL,
        pdTRUE,
        pdMS_TO_TICKS(10),
        pdMS_TO_TICKS(0),
        task_parse_sbus2
    };
    task_configs[1] = new RtosTaskConfig_typedef{
        "task_parse_enc",
        NULL,
        pdTRUE,
        pdMS_TO_TICKS(1),
        pdMS_TO_TICKS(0),
        task_parse_enc
    };
    task_configs[2] = new RtosTaskConfig_typedef{
        "task_draw_infomation",
        NULL,
        pdTRUE,
        pdMS_TO_TICKS(200),
        pdMS_TO_TICKS(0),
        task_draw_infomation
    };
    task_configs[3] = new RtosTaskConfig_typedef{
        "task_control_loop",
        NULL,
        pdTRUE,
        pdMS_TO_TICKS(10),
        pdMS_TO_TICKS(0),
        task_control_loop
    };

    task_initialize(task_configs.data(), task_configs.size());

    vTaskDelay(pdMS_TO_TICKS(1000));
    M5.Display.print("5");
    vTaskDelay(pdMS_TO_TICKS(1000));
    M5.Display.print(" 4");
    vTaskDelay(pdMS_TO_TICKS(1000));
    M5.Display.print(" 3");
    vTaskDelay(pdMS_TO_TICKS(1000));
    M5.Display.print(" 2");
    vTaskDelay(pdMS_TO_TICKS(1000));
    M5.Display.print(" 1\n");
    vTaskDelay(pdMS_TO_TICKS(1000));

    task_start(task_configs.data(), task_configs.size());
    M5.Display.print("started\n");

    lcd_bottom_rect_x1 = M5.Lcd.width()*0.9;
    lcd_bottom_rect_x2 = M5.Lcd.width()*0.95;
    lcd_bottom_rect_x3 = M5.Lcd.width()*1.0;
    lcd_bottom_rect_y1 = M5.Lcd.height()*0.95;
    lcd_bottom_rect_y2 = M5.Lcd.height()*1.0;

}




void loop() {
    // nothing to do.
    vTaskDelay(100);
    M5.Display.fillRect(lcd_bottom_rect_x1, lcd_bottom_rect_y1, lcd_bottom_rect_x2, lcd_bottom_rect_y2, WHITE);
    M5.Display.fillRect(lcd_bottom_rect_x2, lcd_bottom_rect_y1, lcd_bottom_rect_x3, lcd_bottom_rect_y2, BLACK);
    vTaskDelay(100);
    M5.Display.fillRect(lcd_bottom_rect_x1, lcd_bottom_rect_y1, lcd_bottom_rect_x2, lcd_bottom_rect_y2, BLACK);
    M5.Display.fillRect(lcd_bottom_rect_x2, lcd_bottom_rect_y1, lcd_bottom_rect_x3, lcd_bottom_rect_y2, WHITE);
}
