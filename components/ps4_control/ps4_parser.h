#ifndef PS4_PARSER_H
#define PS4_PARSER_H

#define PS4_OUT_DATA_OFFSET 13 //有效数据相对原始数据偏移

/*PS4手柄输出数据索引定义*/
typedef enum {
  PS4_BUTTON_L1,
  PS4_BUTTON_L2,
  PS4_BUTTON_L3,
  PS4_BUTTON_R1,
  PS4_BUTTON_R2,
  PS4_BUTTON_R3,

  PS4_BUTTON_DIR_UP,
  PS4_BUTTON_DIR_DOWN,
  PS4_BUTTON_DIR_LEFT,
  PS4_BUTTON_DIR_RIGHR,

  PS4_BUTTON_DIR_UP_RIGHT,
  PS4_BUTTON_DIR_UP_LEFT,
  PS4_BUTTON_DIR_DOWN_LEFT,
  PS4_BUTTON_DIR_DOWN_RIGHR,

  PS4_BUTTON_SHAPE_TRIANGLE,
  PS4_BUTTON_SHAPE_CIRCLE,
  PS4_BUTTON_SHAPE_CROSS,
  PS4_BUTTON_SHAPE_SQUARE,

  PS4_BUTTON_PS,
  PS4_BUTTON_SHARE,
  PS4_BUTTON_OPTIONS,
  PS4_BUTTON_TOUCH,

  PS4_ANALOG_ROCKER_LX,
  PS4_ANALOG_ROCKER_LY,
  PS4_ANALOG_ROCKER_L2,
  PS4_ANALOG_ROCKER_RX,
  PS4_ANALOG_ROCKER_RY,
  PS4_ANALOG_ROCKER_R2,

  PS4_ANALOG_TOUCH_X,
  PS4_ANALOG_TOUCH_Y,

  PS4_ANALOG_GRY_X,
  PS4_ANALOG_GRY_Y,
  PS4_ANALOG_GRY_Z,

  PS4_ANALOG_ACC_X,
  PS4_ANALOG_ACC_Y,
  PS4_ANALOG_ACC_Z,

  PS4_ANALOG_BAT,
} ps4_out_datat_tpye;

enum ps4_packet_index {
  packet_index_analog_stick_lx = 13,
  packet_index_analog_stick_ly = 14,
  packet_index_analog_stick_rx = 15,
  packet_index_analog_stick_ry = 16,

  packet_index_button_standard = 17,
  packet_index_button_extra = 18,
  packet_index_button_ps = 19,

  packet_index_analog_l2 = 20,
  packet_index_analog_r2 = 21,

  packet_index_sensor_gyroscope_x = 25,
  packet_index_sensor_gyroscope_y = 27,
  packet_index_sensor_gyroscope_z = 29,

  packet_index_sensor_accelerometer_x = 31,
  packet_index_sensor_accelerometer_y = 33,
  packet_index_sensor_accelerometer_z = 35,

  packet_index_status = 42
};


enum ps4_button_mask {
  button_mask_up = 0,
  button_mask_right = 0b00000010,
  button_mask_down = 0b00000100,
  button_mask_left = 0b00000110,

  button_mask_upright = 0b00000001,
  button_mask_downright = 0b00000011,
  button_mask_upleft = 0b00000111,
  button_mask_downleft = 0b00000101,

  button_mask_direction = 0b00001111,

  button_mask_square = 0b00010000,
  button_mask_cross = 0b00100000,
  button_mask_circle = 0b01000000,
  button_mask_triangle = 0b10000000,

  button_mask_l1 = 0b00000001,
  button_mask_r1 = 0b00000010,
  button_mask_l2 = 0b00000100,
  button_mask_r2 = 0b00001000,

  button_mask_share = 0b00010000,
  button_mask_options = 0b00100000,

  button_mask_l3 = 0b01000000,
  button_mask_r3 = 0b10000000,

  button_mask_ps = 0b01,
  button_mask_touchpad = 0b10
};

enum ps4_status_mask {
  ps4_status_mask_battery = 0b00001111,
  ps4_status_mask_charging = 0b00010000,
  ps4_status_mask_audio = 0b00100000,
  ps4_status_mask_mic = 0b01000000,
};

/**
  * @brief  PS4数据包解析函数（目前使用不需要解析）
  *
  * @param  packet 收到的数据
  *
  * @return null
  */
void parsePacket(uint8_t* packet);

/**
  * @brief  PS4数据包拷贝到固定地址上
  *
  * @param  packet 收到的数据
  * @param  len 收到的数据长度
  *
  * @return null
  */
void ps4_control_save_packet(uint8_t* packet,uint8_t len);

/**
  * @brief  获取PS4手柄指定按钮/传感器当前输出
  *
  * @param  packet 收到的数据
  * @param  len 收到的数据长度
  *
  * @return 指定按钮/传感器输出
  */
int16_t ps4_control_get_out(ps4_out_datat_tpye out_type);

#endif