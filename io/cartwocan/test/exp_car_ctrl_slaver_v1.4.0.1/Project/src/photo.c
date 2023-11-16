#include "photo.h"


sInput_Info  port_info[5];
sBD_SYS_MSG  sysmsg;

u8  photovalue = INVALUE;

void vphoto_init_msg(void)
{
    sysmsg.photoreset = INVALUE;
    sysmsg.valuecnt = 0;
    sysmsg.cntlost = 0;
    photovalue = INVALUE;
    sysmsg.lostcount = 0;
    sysmsg.resetlost = 0;
}

// reset photo
void vphoto_deal_with_reset_photo(void)
{
    u8 input = INPUT3_STATUE;

    if (port_info[0].input_state != input
        && port_info[0].input_confirm_times == 0)
    {
        port_info[0].input_middle_state = input;
    }
    if (port_info[0].input_middle_state == input
        && port_info[0].input_middle_state != port_info[0].input_state)
    {
        port_info[0].input_confirm_times++;
        if (port_info[0].input_confirm_times > PHOTO_RESET_FILTER)// 5ms
        {
            port_info[0].input_state = port_info[0].input_middle_state;
            port_info[0].input_confirm_times = 0;
            if (port_info[0].input_state == 1)
            {
                port_info[0].input_flag = INPUT_TRIG_HIGH;
                port_info[0].input_trig_mode = INPUT_TRIG_UP;
            }
            else
            {
                port_info[0].err_cnt = 0;
                port_info[0].input_flag = INPUT_TRIG_LOW;
            }
        }
    }
    else
    {
        port_info[0].input_middle_state = port_info[0].input_state;
        port_info[0].input_confirm_times = 0;
    }

    //  可以判断 光电报警 预留
    if (port_info[0].input_state == 1) {
        port_info[0].err_cnt++;
        if (port_info[0].err_cnt >= 5000) {
            port_info[0].err_cnt = 5000;
        }
    }

}


// cnt photo
void vphoto_deal_with_cnt_photo(void)
{
    u8 input = INPUT4_STATUE;
    u8 buf[10] = { 0 };
    u8 sendlen = 0;

    sysmsg.interval++;
    if (sysmsg.valuecnt != 0) {
        sysmsg.valuecnt--;
        return;
    }

    if (port_info[1].input_state != input
        && port_info[1].input_confirm_times == 0)
    {
        port_info[1].input_middle_state = input;
    }
    if (port_info[1].input_middle_state == input
        && port_info[1].input_middle_state != port_info[1].input_state)
    {
        port_info[1].input_confirm_times++;
        if (port_info[1].input_confirm_times > PHOTO_CNT_FILTER)// 5ms
        {
            port_info[1].input_state = port_info[1].input_middle_state;
            port_info[1].input_confirm_times = 0;
            if (port_info[1].input_state == 1)
            {
                port_info[1].input_trig_mode = INPUT_TRIG_UP;
            }
            else
            {
                port_info[1].err_cnt = 0;
            }
        }
    }
    else
    {
        port_info[1].input_middle_state = port_info[1].input_state;
        port_info[1].input_confirm_times = 0;
    }

    //光电报警预留  
    if (port_info[1].input_state == 1) {
        port_info[1].err_cnt++;
        if (port_info[1].err_cnt >= 5000) {
            port_info[1].err_cnt = 5000;
        }
    }
}


void photo_test(void)
{

    u8 send_len = 0;
    char str[80];

    if (port_info[0].input_state != INPUT1_STATUE
        && port_info[0].input_confirm_times == 0)
    {
        port_info[0].input_middle_state = INPUT1_STATUE;
    }
    if (port_info[0].input_middle_state == INPUT1_STATUE
        && port_info[0].input_middle_state != port_info[0].input_state)
    {
        port_info[0].input_confirm_times++;
        if (port_info[0].input_confirm_times > PHOTO_CNT_FILTER)// 5ms
        {
            port_info[0].input_state = port_info[0].input_middle_state;
            port_info[0].input_confirm_times = 0;
        }
    }
    else
    {
        port_info[0].input_middle_state = port_info[0].input_state;
        port_info[0].input_confirm_times = 0;
    }


    if (port_info[1].input_state != INPUT2_STATUE
        && port_info[1].input_confirm_times == 0)
    {
        port_info[1].input_middle_state = INPUT2_STATUE;
    }
    if (port_info[1].input_middle_state == INPUT2_STATUE
        && port_info[1].input_middle_state != port_info[1].input_state)
    {
        port_info[1].input_confirm_times++;
        if (port_info[1].input_confirm_times > PHOTO_CNT_FILTER)// 5ms
        {
            port_info[1].input_state = port_info[1].input_middle_state;
            port_info[1].input_confirm_times = 0;

        }
    }
    else
    {
        port_info[1].input_middle_state = port_info[1].input_state;
        port_info[1].input_confirm_times = 0;
    }

    if (port_info[2].input_state != INPUT3_STATUE
        && port_info[2].input_confirm_times == 0)
    {
        port_info[2].input_middle_state = INPUT3_STATUE;
    }
    if (port_info[2].input_middle_state == INPUT3_STATUE
        && port_info[2].input_middle_state != port_info[2].input_state)
    {
        port_info[2].input_confirm_times++;
        if (port_info[2].input_confirm_times > PHOTO_CNT_FILTER)// 5ms
        {
            port_info[2].input_state = port_info[2].input_middle_state;
            port_info[2].input_confirm_times = 0;

        }
    }
    else
    {
        port_info[2].input_middle_state = port_info[2].input_state;
        port_info[2].input_confirm_times = 0;
    }


    if (port_info[3].input_state != INPUT4_STATUE
        && port_info[3].input_confirm_times == 0)
    {
        port_info[3].input_middle_state = INPUT4_STATUE;
    }
    if (port_info[3].input_middle_state == INPUT4_STATUE
        && port_info[3].input_middle_state != port_info[3].input_state)
    {
        port_info[3].input_confirm_times++;
        if (port_info[3].input_confirm_times > PHOTO_CNT_FILTER)// 5ms
        {
            port_info[3].input_state = port_info[3].input_middle_state;
            port_info[3].input_confirm_times = 0;

        }
    }
    else
    {
        port_info[3].input_middle_state = port_info[3].input_state;
        port_info[3].input_confirm_times = 0;
    }

    send_len = sprintf(str, "INPUT1:%d INPUT2:%d INPUT3:%d INPUT4:%d\n", port_info[0].input_state, port_info[1].input_state, port_info[2].input_state, port_info[3].input_state);
    DEBUG_process((u8*)str, send_len);

}