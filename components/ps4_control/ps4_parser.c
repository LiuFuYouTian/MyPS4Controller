#include <string.h>
#include <esp_system.h>
#include "esp_log.h"
#include "ps4.h"
#include "ps4_parser.h"

#define PARSER_TAG "PS4_PARSER"

/********************************************************************************/
/*                            L O C A L    T Y P E S */
/********************************************************************************/



/********************************************************************************/
/*              L O C A L    F U N C T I O N     P R O T O T Y P E S */
/********************************************************************************/

ps4_sensor_t parsePacketSensor(uint8_t* packet);
ps4_status_t parsePacketStatus(uint8_t* packet);
ps4_analog_stick_t parsePacketAnalogStick(uint8_t* packet);
ps4_analog_button_t parsePacketAnalogButton(uint8_t* packet);
ps4_button_t parsePacketButtons(uint8_t* packet);
ps4_event_t parseEvent(ps4_t prev, ps4_t cur);

/********************************************************************************/
/*                         L O C A L    V A R I A B L E S */
/********************************************************************************/

static ps4_t ps4;
static ps4_event_callback_t ps4_event_cb = NULL;
static uint8_t ps4_control_packet[PS4_RECV_BUFFER_SIZE];


/*
1、0~2字节为按钮输出1bit
2、3~6字节为摇杆XY输出8bit
3、7~12字节为陀螺仪XYZ输出16bit
4、13~18字节为加速度计XYZ输出16bit
5、19字节为遥控器当前电量8bit
*/
static uint8_t ps4_out[128];


/********************************************************************************/
/*                      P U B L I C    F U N C T I O N S */
/********************************************************************************/
void parserSetEventCb(ps4_event_callback_t cb) {
  ps4_event_cb = cb;
}
/**
  * @brief  PS4数据包解析函数（目前使用不需要解析）
  *
  * @param  packet 收到的数据
  *
  * @return null
  */
void parsePacket(uint8_t* packet) 
{
  ps4_t prev_ps4 = ps4;

  ps4.button = parsePacketButtons(packet);
  ps4.analog.stick = parsePacketAnalogStick(packet);
  ps4.analog.button = parsePacketAnalogButton(packet);
  ps4.sensor = parsePacketSensor(packet);
  ps4.status = parsePacketStatus(packet);
  ps4.latestPacket = packet;

  ps4_event_t ps4Event = parseEvent(prev_ps4, ps4);

  ps4PacketEvent(ps4, ps4Event);

}

/**
  * @brief  PS4数据包拷贝到固定地址上
  *
  * @param  packet 收到的数据
  * @param  len 收到的数据长度
  *
  * @return null
  */
void ps4_control_save_packet(uint8_t* packet,uint8_t len)
{
  if(len == PS4_RECV_BUFFER_SIZE)
  {
    // ESP_LOGE(PARSER_TAG,"ps4_out[22]:%d",ps4_out[22]);
    // ESP_LOGE(PARSER_TAG,"ps4_out[23]:%d",ps4_out[23]);
    // ESP_LOGE(PARSER_TAG,"ps4_out[24]:%d",ps4_out[24]);
    memcpy(ps4_out,packet + PS4_OUT_DATA_OFFSET,PS4_RECV_BUFFER_SIZE - PS4_OUT_DATA_OFFSET);
  }
}

/**
  * @brief  获取PS4手柄指定按钮/传感器当前输出
  *
  * @param  packet 收到的数据
  * @param  len 收到的数据长度
  *
  * @return 指定按钮/传感器输出
  */
int16_t ps4_control_get_out(ps4_out_datat_tpye out_type)
{
  uint8_t frontBtnData = ps4_out[packet_index_button_standard - PS4_OUT_DATA_OFFSET];
  uint8_t extraBtnData = ps4_out[packet_index_button_extra - PS4_OUT_DATA_OFFSET];
  uint8_t psBtnData = ps4_out[packet_index_button_ps - PS4_OUT_DATA_OFFSET];
  uint8_t directionBtnsOnly = button_mask_direction & frontBtnData;
  int16_t buff = 0;

  switch(out_type)
  {
    /*肩键状态*/
    case PS4_BUTTON_L1:
        return extraBtnData & button_mask_l1;
    break;
    case PS4_BUTTON_L2:
        return extraBtnData & button_mask_l2;
    break;
    case PS4_BUTTON_L3:
        return extraBtnData & button_mask_l3;
    break;
    case PS4_BUTTON_R1:
        return extraBtnData & button_mask_r1;
    break;
    case PS4_BUTTON_R2:
        return extraBtnData & button_mask_r2;
    break;
    case PS4_BUTTON_R3:
        return extraBtnData & button_mask_r3;
    break;
    /*方向键状态*/
    case PS4_BUTTON_DIR_UP:
        return directionBtnsOnly == button_mask_up;
    break;
    case PS4_BUTTON_DIR_DOWN:
        return directionBtnsOnly == button_mask_down;
    break;
    case PS4_BUTTON_DIR_LEFT:
        return directionBtnsOnly == button_mask_left;
    break;
    case PS4_BUTTON_DIR_RIGHR:
        return directionBtnsOnly == button_mask_right;
    break;
    case PS4_BUTTON_DIR_UP_RIGHT:
        return directionBtnsOnly == button_mask_upright;
    break;
    case PS4_BUTTON_DIR_UP_LEFT:
        return directionBtnsOnly == button_mask_upleft;
    break;
    case PS4_BUTTON_DIR_DOWN_LEFT:
        return directionBtnsOnly == button_mask_downright;
    break;
    case PS4_BUTTON_DIR_DOWN_RIGHR:
        return directionBtnsOnly == button_mask_downleft;
    break;
    /*特殊功能键状态*/
    case PS4_BUTTON_SHAPE_TRIANGLE:
        return frontBtnData & button_mask_triangle;
    break;
    case PS4_BUTTON_SHAPE_CIRCLE:
        return frontBtnData & button_mask_circle;
    break;
    case PS4_BUTTON_SHAPE_CROSS:
        return frontBtnData & button_mask_cross;
    break;
    case PS4_BUTTON_SHAPE_SQUARE:
        return frontBtnData & button_mask_square;
    break;
    case PS4_BUTTON_PS:
        return psBtnData & button_mask_ps;
    break;
    case PS4_BUTTON_SHARE:
        return extraBtnData & button_mask_share;
    break;
    case PS4_BUTTON_OPTIONS:
        return extraBtnData & button_mask_options;
    break;
    case PS4_BUTTON_TOUCH:
        return psBtnData & button_mask_touchpad;
    break;

    /*摇杆模拟量*/
    case PS4_ANALOG_ROCKER_LX:
        return ps4_out[packet_index_analog_stick_lx - PS4_OUT_DATA_OFFSET];
    break;
    case PS4_ANALOG_ROCKER_LY:
        return ps4_out[packet_index_analog_stick_ly - PS4_OUT_DATA_OFFSET];
    break;
    case PS4_ANALOG_ROCKER_L2:
        return ps4_out[packet_index_analog_l2 - PS4_OUT_DATA_OFFSET];
    break;
    case PS4_ANALOG_ROCKER_RX:
        return ps4_out[packet_index_analog_stick_rx - PS4_OUT_DATA_OFFSET];
    break;
    case PS4_ANALOG_ROCKER_RY:
        return ps4_out[packet_index_analog_stick_ry - PS4_OUT_DATA_OFFSET];
    break;
    case PS4_ANALOG_ROCKER_R2:
        return ps4_out[packet_index_analog_r2 - PS4_OUT_DATA_OFFSET];
    break;
    case PS4_ANALOG_TOUCH_X:
        buff = (ps4_out[23] << 8) + ps4_out[22];
        return buff;
    break;
    case PS4_ANALOG_TOUCH_Y:
        return 100;
    break;
    case PS4_ANALOG_GRY_X:
        buff = (ps4_out[packet_index_sensor_gyroscope_x + 1 - PS4_OUT_DATA_OFFSET] << 8) + ps4_out[packet_index_sensor_gyroscope_x - PS4_OUT_DATA_OFFSET];
        return buff;
    break;
    case PS4_ANALOG_GRY_Y:
        buff = (ps4_out[packet_index_sensor_gyroscope_y + 1 - PS4_OUT_DATA_OFFSET] << 8) + ps4_out[packet_index_sensor_gyroscope_y - PS4_OUT_DATA_OFFSET];
        return buff;
    break;
    case PS4_ANALOG_GRY_Z:
        buff = (ps4_out[packet_index_sensor_gyroscope_z + 1 - PS4_OUT_DATA_OFFSET] << 8) + ps4_out[packet_index_sensor_gyroscope_z - PS4_OUT_DATA_OFFSET];
        return buff;
    break;
    case PS4_ANALOG_ACC_X:
        buff = (ps4_out[packet_index_sensor_accelerometer_x + 1 - PS4_OUT_DATA_OFFSET] << 8) + ps4_out[packet_index_sensor_accelerometer_x - PS4_OUT_DATA_OFFSET];
        return buff;
    break;
    case PS4_ANALOG_ACC_Y:
        buff = (ps4_out[packet_index_sensor_accelerometer_y + 1 - PS4_OUT_DATA_OFFSET] << 8) + ps4_out[packet_index_sensor_accelerometer_y - PS4_OUT_DATA_OFFSET];
        return buff;
    break;
    case PS4_ANALOG_ACC_Z:
        buff = (ps4_out[packet_index_sensor_accelerometer_z + 1 - PS4_OUT_DATA_OFFSET] << 8) + ps4_out[packet_index_sensor_accelerometer_z - PS4_OUT_DATA_OFFSET];
        return buff;
    break;
    case PS4_ANALOG_BAT:
        buff = ps4_out[packet_index_status - PS4_OUT_DATA_OFFSET] & ps4_status_mask_battery;
        return buff;
    default:
        return 0;
  };

}

/********************************************************************************/
/*                      L O C A L    F U N C T I O N S */
/********************************************************************************/

/******************/
/*    E V E N T   */
/******************/
ps4_event_t parseEvent(ps4_t prev, ps4_t cur) {
  ps4_event_t ps4Event;

  /* Button down events */
  ps4Event.button_down.right = !prev.button.right && cur.button.right;
  ps4Event.button_down.down = !prev.button.down && cur.button.down;
  ps4Event.button_down.up = !prev.button.up && cur.button.up;
  ps4Event.button_down.left = !prev.button.left && cur.button.left;

  ps4Event.button_down.square = !prev.button.square && cur.button.square;
  ps4Event.button_down.cross = !prev.button.cross && cur.button.cross;
  ps4Event.button_down.circle = !prev.button.circle && cur.button.circle;
  ps4Event.button_down.triangle = !prev.button.triangle && cur.button.triangle;

  ps4Event.button_down.upright = !prev.button.upright && cur.button.upright;
  ps4Event.button_down.downright = !prev.button.downright && cur.button.downright;
  ps4Event.button_down.upleft = !prev.button.upleft && cur.button.upleft;
  ps4Event.button_down.downleft = !prev.button.downleft && cur.button.downleft;

  ps4Event.button_down.l1 = !prev.button.l1 && cur.button.l1;
  ps4Event.button_down.r1 = !prev.button.r1 && cur.button.r1;
  ps4Event.button_down.l2 = !prev.button.l2 && cur.button.l2;
  ps4Event.button_down.r2 = !prev.button.r2 && cur.button.r2;

  ps4Event.button_down.share = !prev.button.share && cur.button.share;
  ps4Event.button_down.options = !prev.button.options && cur.button.options;
  ps4Event.button_down.l3 = !prev.button.l3 && cur.button.l3;
  ps4Event.button_down.r3 = !prev.button.r3 && cur.button.r3;

  ps4Event.button_down.ps = !prev.button.ps && cur.button.ps;
  ps4Event.button_down.touchpad = !prev.button.touchpad && cur.button.touchpad;

  /* Button up events */
  ps4Event.button_up.right = prev.button.right && !cur.button.right;
  ps4Event.button_up.down = prev.button.down && !cur.button.down;
  ps4Event.button_up.up = prev.button.up && !cur.button.up;
  ps4Event.button_up.left = prev.button.left && !cur.button.left;

  ps4Event.button_up.square = prev.button.square && !cur.button.square;
  ps4Event.button_up.cross = prev.button.cross && !cur.button.cross;
  ps4Event.button_up.circle = prev.button.circle && !cur.button.circle;
  ps4Event.button_up.triangle = prev.button.triangle && !cur.button.triangle;

  ps4Event.button_up.upright = prev.button.upright && !cur.button.upright;
  ps4Event.button_up.downright = prev.button.downright && !cur.button.downright;
  ps4Event.button_up.upleft = prev.button.upleft && !cur.button.upleft;
  ps4Event.button_up.downleft = prev.button.downleft && !cur.button.downleft;

  ps4Event.button_up.l1 = prev.button.l1 && !cur.button.l1;
  ps4Event.button_up.r1 = prev.button.r1 && !cur.button.r1;
  ps4Event.button_up.l2 = prev.button.l2 && !cur.button.l2;
  ps4Event.button_up.r2 = prev.button.r2 && !cur.button.r2;

  ps4Event.button_up.share = prev.button.share && !cur.button.share;
  ps4Event.button_up.options = prev.button.options && !cur.button.options;
  ps4Event.button_up.l3 = prev.button.l3 && !cur.button.l3;
  ps4Event.button_up.r3 = prev.button.r3 && !cur.button.r3;

  ps4Event.button_up.ps = prev.button.ps && !cur.button.ps;
  ps4Event.button_up.touchpad = prev.button.touchpad && !cur.button.touchpad;

  ps4Event.analog_move.stick.lx = cur.analog.stick.lx != 0;
  ps4Event.analog_move.stick.ly = cur.analog.stick.ly != 0;
  ps4Event.analog_move.stick.rx = cur.analog.stick.rx != 0;
  ps4Event.analog_move.stick.ry = cur.analog.stick.ry != 0;

  return ps4Event;
}

/********************/
/*    A N A L O G   */
/********************/
ps4_analog_stick_t parsePacketAnalogStick(uint8_t* packet) {
  ps4_analog_stick_t ps4AnalogStick;

  const uint8_t offset = 128;//左右摇杆
  const int8_t minJ = -127;

  if (packet[packet_index_analog_stick_lx] == 0) 
    ps4AnalogStick.lx = minJ;
  else 
    ps4AnalogStick.lx = packet[packet_index_analog_stick_lx] + offset;

  if (packet[packet_index_analog_stick_ly] == 0) 
    ps4AnalogStick.ly = minJ;
  else 
    ps4AnalogStick.ly = packet[packet_index_analog_stick_ly] + offset;

  if (packet[packet_index_analog_stick_rx] == 0) 
    ps4AnalogStick.rx = minJ;
  else 
    ps4AnalogStick.rx = packet[packet_index_analog_stick_rx] + offset;

  if (packet[packet_index_analog_stick_ry] == 0) 
    ps4AnalogStick.ry = minJ;
  else 
    ps4AnalogStick.ry = packet[packet_index_analog_stick_ry] + offset;

  return ps4AnalogStick;
}

ps4_analog_button_t parsePacketAnalogButton(uint8_t* packet) {
  ps4_analog_button_t ps4AnalogButton;

  ps4AnalogButton.l2 = packet[packet_index_analog_l2];//L2 R2
  ps4AnalogButton.r2 = packet[packet_index_analog_r2];

  return ps4AnalogButton;
}

/*********************/
/*   B U T T O N S   */
/*********************/

ps4_button_t parsePacketButtons(uint8_t* packet) {
  ps4_button_t ps4_button;
  uint8_t frontBtnData = packet[packet_index_button_standard];      //正面按键 △◻x◯
  uint8_t extraBtnData = packet[packet_index_button_extra];         //额外按键L1 L2 R1 R2 L3 R3 SAHARE OPTIONS (L3 R3 摇杆按下)
  uint8_t psBtnData = packet[packet_index_button_ps];               //PS按键：PS及触摸板按键
  uint8_t directionBtnsOnly = button_mask_direction & frontBtnData; //方向按键:上下左右

  ps4_button.up = directionBtnsOnly == button_mask_up;
  ps4_button.right = directionBtnsOnly == button_mask_right;
  ps4_button.down = directionBtnsOnly == button_mask_down;
  ps4_button.left = directionBtnsOnly == button_mask_left;

  ps4_button.upright = directionBtnsOnly == button_mask_upright;
  ps4_button.upleft = directionBtnsOnly == button_mask_upleft;
  ps4_button.downright = directionBtnsOnly == button_mask_downright;
  ps4_button.downleft = directionBtnsOnly == button_mask_downleft;

  ps4_button.triangle = (frontBtnData & button_mask_triangle) ? true : false;
  ps4_button.circle = (frontBtnData & button_mask_circle) ? true : false;
  ps4_button.cross = (frontBtnData & button_mask_cross) ? true : false;
  ps4_button.square = (frontBtnData & button_mask_square) ? true : false;

  ps4_button.l1 = (extraBtnData & button_mask_l1) ? true : false;
  ps4_button.r1 = (extraBtnData & button_mask_r1) ? true : false;
  ps4_button.l2 = (extraBtnData & button_mask_l2) ? true : false;
  ps4_button.r2 = (extraBtnData & button_mask_r2) ? true : false;

  ps4_button.share = (extraBtnData & button_mask_share) ? true : false;
  ps4_button.options = (extraBtnData & button_mask_options) ? true : false;
  ps4_button.l3 = (extraBtnData & button_mask_l3) ? true : false;
  ps4_button.r3 = (extraBtnData & button_mask_r3) ? true : false;

  ps4_button.ps = (psBtnData & button_mask_ps) ? true : false;
  ps4_button.touchpad = (psBtnData & button_mask_touchpad) ? true : false;

  return ps4_button;
}

/*******************************/
/*   S T A T U S   F L A G S   */
/*******************************/
ps4_status_t parsePacketStatus(uint8_t* packet) {
  ps4_status_t ps4Status;

  ps4Status.battery = packet[packet_index_status] & ps4_status_mask_battery;//电池
  ps4Status.charging = packet[packet_index_status] & ps4_status_mask_charging ? true : false;
  ps4Status.audio = packet[packet_index_status] & ps4_status_mask_audio ? true : false;
  ps4Status.mic = packet[packet_index_status] & ps4_status_mask_mic ? true : false;

  return ps4Status;
}

/********************/
/*   S E N S O R S  */
/********************/
ps4_sensor_t parsePacketSensor(uint8_t* packet) {
  ps4_sensor_t ps4Sensor;

  ps4Sensor.gyroscope.x = (packet[packet_index_sensor_gyroscope_x + 1] << 8) + packet[packet_index_sensor_gyroscope_x];//陀螺仪
  ps4Sensor.gyroscope.y = (packet[packet_index_sensor_gyroscope_y + 1] << 8) + packet[packet_index_sensor_gyroscope_y];
  ps4Sensor.gyroscope.z = (packet[packet_index_sensor_gyroscope_z + 1] << 8) + packet[packet_index_sensor_gyroscope_z];

  ps4Sensor.accelerometer.x = (packet[packet_index_sensor_accelerometer_x + 1] << 8) + packet[packet_index_sensor_accelerometer_x];//加速度计
  ps4Sensor.accelerometer.y = (packet[packet_index_sensor_accelerometer_y + 1] << 8) + packet[packet_index_sensor_accelerometer_y];
  ps4Sensor.accelerometer.z = (packet[packet_index_sensor_accelerometer_z + 1] << 8) + packet[packet_index_sensor_accelerometer_z];

  //ESP_LOGE(PARSER_TAG,"gyro.x:%d,gyro.y:%d,gyro.z:%d,acce.x:%d,acce.y:%d,acce.z:%d\r\n",ps4Sensor.gyroscope.x,ps4Sensor.gyroscope.y,ps4Sensor.gyroscope.z,ps4Sensor.accelerometer.x,ps4Sensor.accelerometer.y,ps4Sensor.accelerometer.z);

 return ps4Sensor;
}
