#ifndef __INCLUDE_GLOBAL_CONSTANTS_HPP
#define __INCLUDE_GLOBAL_CONSTANTS_HPP


/**********************************************************************
 * Control parameters
 *********************************************************************/
// システム制御パラメータ系
#define MAX_CHARS_DISPLAY_WIDTH 16              // ディスプレイの横幅の最大文字数
#define LOOP_ALIVE_COUNT_THRESHOLD 50          // ループが正常に実行され始めていると判定するための閾値
#define SLEEP_BUTTON_START_POSE_THRESHOLD 500  // スリープボタンでスタート姿勢用長押ししたと判定するための閾値
#define SLEEP_BUTTON_SLEEP_THRESHOLD 1000       // スリープボタンでスリープ用長押ししたと判定するための閾値

// ロボット動作パラメータ系
#define ROLL_DEADZONE 0.15              // ロール入力のデッドゾーン
#define ROLL_ADJ_INPUT_MAX 0.6          // ロール入力で調整中と判定する最大入力値
#define ROLL_ADJ_SPEED 0.15             // ロール入力で調整するときの速度
#define ROLL_ROLLING_SPEED 1.0          // ロール入力で回転するときの速度
#define UNSLEEP_MOVE_SQRT_THRESHOLD 0.1 // スリープ解除するための移動入力量の閾値
#define BOOM_UP_MOVE_SQRT_THRESHOLD 0.55 // ブームアップするための移動入力量の閾値
#define BOOM_NORMAL_POSITION 0              // 通常モードのブーム位置
#define BOOM_NORMAL_STICK_SENSITIVITY 4000  // 通常モードのブームのスティック感度
#define BOOM_UP_POSTION 5300                // ブームアップモードのブーム位置
#define BOOM_UP_STICK_SENSITIVITY 0         // ブームアップモードのブームのスティック感度
#define BOOM_ROLLING_POSITION -3000         // ブームローリングモードのブーム位置
#define BOOM_ROLLING_STICK_SENSITIVITY 2500 // ブームローリングモードのブームのスティック感度
#define BOOM_STARTING_POSITION 22500        // スタート姿勢のブーム位置



/**********************************************************************
 * Macros
 *********************************************************************/
#define SBUS2_BUTON_IS_HIGH(x) ((x) > 1500)
#define SBUS2_BUTON_IS_LOW(x) ((x) < 500)
#define POW2(x) ((x)*(x))



/**********************************************************************
 * Enum
 *********************************************************************/
enum RobotStatus{
    RSTAT_WAITING_STABILIZED,
    RSTAT_COUNTING_DOWN,
    RSTAT_INITIALIZING,
    RSTAT_SLEEPING,
    RSTAT_STARTING_POSE_READY,
    RSTAT_STARTING_POSE,
    RSTAT_NORMAL,
};

enum ControlStatus{
    CSTAT_NORMAL,
    CSTAT_BOOM_UP_MOVING,
    CSTAT_ROLLING,
};



#endif // __INCLUDE_GLOBAL_CONSTANTS_HPP
