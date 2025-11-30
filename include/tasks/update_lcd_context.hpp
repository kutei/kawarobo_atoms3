#ifndef __INCLUDE_UPDATE_LCD_CONTEXT_HPP
#define __INCLUDE_UPDATE_LCD_CONTEXT_HPP

#include "task_controller.hpp"
#include <M5Unified.h>


class UpdateLcdContext : public AbstractRtosTaskContext
{
public:
    enum Menu {
        MENU_CALIBRATE_IMU = 0,
        MENU_DELETE_CALIB_DATA,
        MENU_SIZE
    };
    UpdateLcdContext(RtosTaskConfigSharedPtr config, M5GFX *disp);
    void onExecute();

private:
    M5GFX *_disp;
    M5Canvas _canvas;
    int _brect_x1; // CoRE1のKeep Aliveを示す四角形の座標
    int _brect_x2; // CoRE1のKeep Aliveを示す四角形の座標
    int _brect_y; // CoRE1のKeep Aliveを示す四角形の座標
    int _brect_w; // CoRE1のKeep Aliveを示す四角形の座標
    int _brect_h; // CoRE1のKeep Aliveを示す四角形の座標
    unsigned long _update_counter = 0;

    int _menu_index = 0;
    void _entered();
    void _nextMenu();

    void _pushToDisplay();
};


#endif // __INCLUDE_UPDATE_LCD_CONTEXT_HPP
