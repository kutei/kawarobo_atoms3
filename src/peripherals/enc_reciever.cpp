#include "peripherals/enc_reciever.hpp"

bool EncReciever::begin(Stream *stream_, bool invert_enc, bool invert_sw)
{
    if(stream_ == NULL) return false;
    this->stream = stream_;

    this->_sem = xSemaphoreCreateMutex();
    if(this->_sem == NULL) return false;

    this->_invert_enc = invert_enc;
    this->_invert_sw = invert_sw;

    return true;
}

bool EncReciever::parse()
{
    uint16_t err_cnt = 1;

    this->flush_rx();

    while(this->stream->available() > 0){

        uint8_t buf = this->stream->read();

        // スタートバイトがエラーかチェック
        if(this->_parse_counter == 0){
            if(buf != 's' && buf != 'p' && buf != 'n'){
                err_cnt++;
                continue;
            }
        }

        // 受信バッファに格納
        this->_recieve_buf[this->_parse_counter] = buf;
        this->_parse_counter++;


        if(this->_parse_counter >= EncReciever::ENC_DATA_LENGTH){
            this->_parse_counter = 0;

            // 受信データをパース
            if(this->_parse_from_buffer_once() == true){
                err_cnt = 0;
            }
        }
    }

    return err_cnt;
}

void EncReciever::flush_rx()
{
    int rest_over = this->stream->available() - EncReciever::ENC_DATA_LENGTH;
    if(rest_over > 0){
        for(int i = 0; i < rest_over; i++) this->stream->read();
    }
}

int32_t EncReciever::get_angle()
{
    if(xSemaphoreTake(this->_sem, portMAX_DELAY) != pdTRUE) return 0;
    int32_t ret = this->_angle;
    xSemaphoreGive(this->_sem);

    return ret;
}
bool EncReciever::is_recieved()
{
    if(xSemaphoreTake(this->_sem, portMAX_DELAY) != pdTRUE) return false;
    bool ret = this->_is_recieved;
    xSemaphoreGive(this->_sem);

    return ret;
}
bool EncReciever::is_initialized()
{
    if(xSemaphoreTake(this->_sem, portMAX_DELAY) != pdTRUE) return false;
    bool ret = this->_is_initialized;
    xSemaphoreGive(this->_sem);

    return ret;
}
bool EncReciever::is_on_upper_side()
{
    if(xSemaphoreTake(this->_sem, portMAX_DELAY) != pdTRUE) return false;
    bool ret = this->_is_on_upper_side;
    xSemaphoreGive(this->_sem);

    return ret;
}

bool EncReciever::_parse_from_buffer_once()
{
    int32_t agl = 0;
    bool init = false;
    bool side = false;

    // ステータスをパース
    if(this->_recieve_buf[0] == 's')
    {
        init = true;
        side = false;
    }
    else if(this->_recieve_buf[0] == 'p')
    {
        init = false;
        side = true;
    }
    else if(this->_recieve_buf[0] == 'n')
    {
        init = false;
        side = false;
    }

    // 回転角をパース
    agl = (int32_t)this->_recieve_buf[1] << 24;
    agl |= (int32_t)this->_recieve_buf[2] << 16;
    agl |= (int32_t)this->_recieve_buf[3] << 8;
    agl |= (int32_t)this->_recieve_buf[4];

    // 反転設定を反映
    if(this->_invert_enc) agl *= -1;
    if(this->_invert_enc) side = !side;

    // 公開用変数に格納
    if(xSemaphoreTake(this->_sem, portMAX_DELAY) != pdTRUE) return false;
    this->_angle = agl;
    this->_is_recieved = true;
    this->_is_initialized = init;
    this->_is_on_upper_side = side;
    xSemaphoreGive(this->_sem);

    return false;
}
