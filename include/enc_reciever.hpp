#ifndef __INCLUDE_MAIN_ENC_RECIEVER_HPP
#define __INCLUDE_MAIN_ENC_RECIEVER_HPP

#include <Arduino.h>
#include <M5Unified.h>

#include <freertos/FreeRTOS.h>
#include <freertos/semphr.h>

class EncReciever
{
public:
    static const int ENC_DATA_LENGTH = 5;
    bool begin(Stream *stream, bool invert_enc = false, bool invert_sw = false);
    bool parse();
    void flush_rx();
    int32_t get_angle();
    bool is_initialized();
    bool is_on_upper_side();
    Stream *stream;

private:
    SemaphoreHandle_t _sem;
    int32_t _angle = 0;
    bool _is_initialized = false;
    bool _is_on_upper_side = false;
    uint8_t _recieve_buf[ENC_DATA_LENGTH];
    uint8_t _parse_counter = 0;
    bool _invert_enc = false;
    bool _invert_sw = false;
    bool _parse_from_buffer_once();
};

#endif // __INCLUDE_MAIN_ENC_RECIEVER_HPP
