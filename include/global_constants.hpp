#ifndef __INCLUDE_GLOBAL_CONSTANTS_HPP
#define __INCLUDE_GLOBAL_CONSTANTS_HPP


/**********************************************************************
 * Control parameters
 *********************************************************************/
// システム制御パラメータ系
#define MAX_CHARS_DISPLAY_WIDTH 16              // ディスプレイの横幅の最大文字数
#define LOOP_ALIVE_COUNT_THRESHOLD 20           // ループが正常に実行され始めていると判定するための閾値
#define SLEEP_BUTTON_START_POSE_THRESHOLD 2000  // スリープボタンでスタート姿勢用長押ししたと判定するための閾値
#define SLEEP_BUTTON_SLEEP_THRESHOLD 4000       // スリープボタンでスリープ用長押ししたと判定するための閾値

// ロボット動作パラメータ系
#define ROLL_DEADZONE 0.05              // ロール入力のデッドゾーン
#define ROLL_ADJ_INPUT_MAX 0.6          // ロール入力で調整中と判定する最大入力値
#define ROLL_ADJ_SPEED 0.15             // ロール入力で調整するときの速度
#define ROLL_ROLLING_SPEED 1.0          // ロール入力で回転するときの速度
#define UNSLEEP_MOVE_SQRT_THRESHOLD 0.1 // スリープ解除するための移動入力量の閾値
#define BOOM_UP_MOVE_SQRT_THRESHOLD 0.55 // ブームアップするための移動入力量の閾値

// システム動作パラメータ系
// #define SERIAL_OUT_SBUS2_RAW        // SBUS2受信データを生データで出力する場合定義
// #define SERIAL_OUT_CONTROL_STATUS   // 制御状態を出力する場合定義



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
    RSTAT_STARTING_POSE,
    RSTAT_NORMAL,
};

enum ControlStatus{
    CSTAT_NORMAL,
    CSTAT_BOOM_UP_MOVING,
    CSTAT_ROLLING,
};



#endif // __INCLUDE_GLOBAL_CONSTANTS_HPP
