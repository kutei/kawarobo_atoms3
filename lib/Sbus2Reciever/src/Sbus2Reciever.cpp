#include <Sbus2Reciever.hpp>

int16_t Sbus2Reciever::begin(Stream *stream)
{
    if(stream == NULL) return -1;
    this->_stream = stream;

    this->_sem = xSemaphoreCreateMutex();
    if(this->_sem == NULL) return -2;

    return 0;
}

int16_t Sbus2Reciever::parse()
{
    bool recieved_flag = false;
    uint16_t err_cnt = 1;

    while(this->_stream->available() > 0){
        uint8_t buf = this->_stream->read();

        // スタートバイトがエラーかチェック
        if(this->_parse_counter == 0 && buf != 0x0F){
            err_cnt++;
            continue;
        }

        // 受信バッファに格納
        this->_recieve_buf[this->_parse_counter] = buf;
        this->_parse_counter++;

        if(this->_parse_counter >= Sbus2Reciever::SBUS2_DATA_SIZE){
            this->_parse_counter = 0;

            // エンドバイトがエラーかチェック
            if(this->_recieve_buf[24] & 0x8F != 0xC4){
                err_cnt++;
                continue;
            }

            // 受信データをパース(11bitごと)
            if(this->_parseOnce() == true){
                err_cnt = 0;
                recieved_flag = true;
            }
        }
    }

    return err_cnt;
}

int16_t Sbus2Reciever::getChannel(int ch)
{
    if(ch < 0 || ch >= Sbus2Reciever::SBUS2_CHANNELS) return -1;

    int16_t ret;
    if(xSemaphoreTake(this->_sem, portMAX_DELAY) != pdTRUE) return -1;
    ret = this->_recieved[ch];
    xSemaphoreGive(this->_sem);

    return ret;
}
bool Sbus2Reciever::getVtailChannel(int16_t *ch0, int16_t *ch1, int16_t *ch2, int16_t *ch3)
{
    if(ch0 == NULL || ch1 == NULL || ch2 == NULL || ch3 == NULL) return false;

    if(xSemaphoreTake(this->_sem, portMAX_DELAY) != pdTRUE) return false;
    *ch0 = this->_recieved[0];
    *ch1 = this->_recieved_vtail[0];
    *ch2 = this->_recieved[2];
    *ch3 = this->_recieved_vtail[1];
    xSemaphoreGive(this->_sem);

    return true;
}
bool Sbus2Reciever::isFailsafe()
{
    bool ret;
    if(xSemaphoreTake(this->_sem, portMAX_DELAY) != pdTRUE) return false;
    ret = this->_failsafe;
    xSemaphoreGive(this->_sem);

    return ret;
}
bool Sbus2Reciever::isLostframe()
{
    bool ret;
    if(xSemaphoreTake(this->_sem, portMAX_DELAY) != pdTRUE) return false;
    ret = this->_lostframe;
    xSemaphoreGive(this->_sem);

    return ret;
}

bool Sbus2Reciever::_parseOnce()
{
    uint8_t *buf = this->_recieve_buf;
    uint16_t ch[Sbus2Reciever::SBUS2_CHANNELS];
    bool failsafe, lostframe;

    ch[0]  = ((buf[1]     | buf[2]<<8)                & 0x07FF);
    ch[1]  = ((buf[2]>>3  | buf[3]<<5)                & 0x07FF);
    ch[2]  = ((buf[3]>>6  | buf[4]<<2  | buf[5]<<10)  & 0x07FF);
    ch[3]  = ((buf[5]>>1  | buf[6]<<7)                & 0x07FF);
    ch[4]  = ((buf[6]>>4  | buf[7]<<4)                & 0x07FF);
    ch[5]  = ((buf[7]>>7  | buf[8]<<1  | buf[9]<<9)   & 0x07FF);
    ch[6]  = ((buf[9]>>2  | buf[10]<<6)               & 0x07FF);
    ch[7]  = ((buf[10]>>5 | buf[11]<<3)               & 0x07FF);
    ch[8]  = ((buf[12]    | buf[13]<<8)               & 0x07FF);
    ch[9]  = ((buf[13]>>3 | buf[14]<<5)               & 0x07FF);
    ch[10] = ((buf[14]>>6 | buf[15]<<2 | buf[16]<<10) & 0x07FF);
    ch[11] = ((buf[16]>>1 | buf[17]<<7)               & 0x07FF);
    ch[12] = ((buf[17]>>4 | buf[18]<<4)               & 0x07FF);
    ch[13] = ((buf[18]>>7 | buf[19]<<1 | buf[20]<<9)  & 0x07FF);
    ch[14] = ((buf[20]>>2 | buf[21]<<6)               & 0x07FF);
    ch[15] = ((buf[21]>>5 | buf[22]<<3)               & 0x07FF);

    if((buf[23])      & 0x0001){
        ch[16] = 2047;
    }else{
        ch[16] = 0;
    }

    if((buf[23] >> 1) & 0x0001){
        ch[17] = 2047;
    }else{
        ch[17] = 0;
    }

    if((buf[23] >> 3) & 0x0001){
        failsafe = true;
    }else{
        failsafe = false;
    }

    if ((buf[23] >> 2) & 0x0001) {
        lostframe = true;
    }else{
        lostframe = false;
    }

    // VTAILのチャンネルを計算
    this->_recieved_vtail[0] = ((int)ch[1] - ch[3]) / 2 + 1024;
    this->_recieved_vtail[1] = ((int)ch[1] + ch[3]) / 2;

    // 公開用のバッファにコピー
    if(xSemaphoreTake(this->_sem, portMAX_DELAY) != pdTRUE) return false;
    for(int i = 0; i < Sbus2Reciever::SBUS2_CHANNELS; i++){
        this->_recieved[i] = ch[i];
    }
    this->_failsafe = failsafe;
    this->_lostframe = lostframe;
    xSemaphoreGive(this->_sem);

    return true;
}
