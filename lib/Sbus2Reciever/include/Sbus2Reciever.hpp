#ifndef __SBUS2RECIEVER_HPP__
#define __SBUS2RECIEVER_HPP__

#include <freertos/FreeRTOS.h>
#include <freertos/semphr.h>
#include <Stream.h>

class Sbus2Reciever
{
public:
    static const int SBUS2_CHANNELS = 18;
    static const int SBUS2_DATA_SIZE = 25;

    int16_t begin(Stream *stream);
    int16_t parse();
    int16_t getChannel(int ch);
    bool isFailsafe();
    bool isLostframe();

private:
    Stream *_stream;
    int16_t _recieved[SBUS2_CHANNELS];
    uint8_t _recieve_buf[SBUS2_DATA_SIZE];
    uint8_t _parse_counter = 0;
    bool _failsafe = false;
    bool _lostframe = false;
    SemaphoreHandle_t _sem;

    bool _parseOnce();
};

#endif // __SBUS2RECIEVER_HPP__
