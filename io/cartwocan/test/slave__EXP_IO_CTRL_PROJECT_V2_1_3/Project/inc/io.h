#ifndef _IO_H
#define _IO_H

#define  PORT_NUM           24

#define  INPUT_TRIG_NULL    0
#define  INPUT_TRIG_UP      1
#define  INPUT_TRIG_DOWN    2

#define  LIGHT_OFF          0
#define  LIGHT_FLASH_FAST   1
#define  LIGHT_FLASH_SLOW   2
#define  LIGHT_ON           3


#define  IO_ALLCTRL_NONE    0
#define  IO_ALLCTRL_CLOSE   1
#define  IO_ALLCTRL_OPEN    2

#pragma pack (1) 

typedef struct {
    u8  input_state;
    u8  input_middle_state;
    u16 input_confirm_times;
    u8  input_trig_mode; //null,up,down
}sInput_Info;

typedef struct {
    sInput_Info  input_pack_full;
    sInput_Info  input_button;
    u8           light_mode;  //on,off,flash(fast,slow)
    u8           port_enable; //open(0),close(1)
    u8           print_flag;
    u8           full_flag;
    u8           button_flag;
    u8           close_flag;
    u8           clear_flag;
    u16          hold_delay_time;
}sUnload_Port_Info;

typedef struct {
    sInput_Info  input_block;
    u8  yellow_alarm_mode; //on,off,flash  green
    u8  red_alarm_mode; //on,off,flash
    u8  buzzer_alarm_mode;
    u8  block_flag;
}sPort_Alarm_Info;

#pragma pack () 

extern u16 output_test;

void InputScanProc();
void OutPutCtrlProc();
void WritePortStateBit();
void port_state_ctrl(u8* data);
void io_open_all(void);
void handleInputScan();
void io_outtest_void(void);

#endif 
