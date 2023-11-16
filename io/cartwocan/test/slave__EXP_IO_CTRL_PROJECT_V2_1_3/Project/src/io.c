#include "main.h"

sUnload_Port_Info port_info[PORT_NUM];
sPort_Alarm_Info alarm_info[2];

u16 reg_fast = 25;
u16 reg_slow = 50;
u8  output_state_fast = 0;
u8  output_state_slow = 0;

u16 output_test = 0;
u16 output_flag = 0;

u8 readInputBitState(u8 index)
{
    if(index <= 15)//0~15
    {
        return ((IN_1_16_STATE>>index)&0x1);
    }
    else if((index >= 16)&&(index <= 31))//16~31
    {
        return ((IN_17_32_STATE>>(index-16))&0x1);
    }
    else if((index >= 32)&&(index <= 39))//32~39
    {
        return ((IN_33_40_STATE>>(index-32))&0x1);
    }
    else if(index == 40)
    {
        return IN_41_STATE;
    }
    else if(index == 41)
    {
        return IN_42_STATE;
    }
    else if(index == 42)
    {
        return IN_43_STATE;
    }
    else if(index == 43)
    {
        return IN_44_STATE;
    }
    else if(index == 44)
    {
        return IN_45_STATE;
    }
    else if(index == 45)
    {
        return IN_46_STATE;
    }
    else if(index == 46)
    {
        return IN_47_STATE;
    }
    else if(index == 47)
    {
        return IN_48_STATE;
    }
    else
    {
        return 0;
    }
}

//scan every 10ms
void InputScanProc()
{
    u8 i;
    u8 input[2];
    //处理第一组的卡包信号
    input[0] = readInputBitState(20);
    if (alarm_info[0].input_block.input_state != input[0]
        && alarm_info[0].input_block.input_confirm_times == 0)
    {
        alarm_info[0].input_block.input_middle_state = input[0];
    }
    if (alarm_info[0].input_block.input_middle_state == input[0]
        && alarm_info[0].input_block.input_middle_state != alarm_info[0].input_block.input_state)
    {
        alarm_info[0].input_block.input_confirm_times++;
        if (alarm_info[0].input_block.input_confirm_times > 100)//卡包光电确认时间1000ms
        {
            alarm_info[0].input_block.input_state = alarm_info[0].input_block.input_middle_state;
            alarm_info[0].input_block.input_confirm_times = 0;
            if (alarm_info[0].input_block.input_state == 1)
            {
                alarm_info[0].input_block.input_trig_mode = INPUT_TRIG_UP;
            }
            else
            {
                alarm_info[0].input_block.input_trig_mode = INPUT_TRIG_DOWN;
            }
        }
    }
    else
    {
        alarm_info[0].input_block.input_middle_state = alarm_info[0].input_block.input_state;
        alarm_info[0].input_block.input_confirm_times = 0;
    }
    if (alarm_info[0].input_block.input_trig_mode == INPUT_TRIG_UP)
    {
        alarm_info[0].block_flag = 1;
    }
    else if (alarm_info[0].input_block.input_trig_mode == INPUT_TRIG_DOWN)
    {
        alarm_info[0].block_flag = 0;
    }
    //处理第二组的卡包信号
    input[0] = readInputBitState(44);
    if (alarm_info[1].input_block.input_state != input[0]
        && alarm_info[1].input_block.input_confirm_times == 0)
    {
        alarm_info[1].input_block.input_middle_state = input[0];
    }
    if (alarm_info[1].input_block.input_middle_state == input[0]
        && alarm_info[1].input_block.input_middle_state != alarm_info[1].input_block.input_state)
    {
        alarm_info[1].input_block.input_confirm_times++;
        if (alarm_info[1].input_block.input_confirm_times > 100)//卡包光电确认时间1000ms
        {
            alarm_info[1].input_block.input_state = alarm_info[1].input_block.input_middle_state;
            alarm_info[1].input_block.input_confirm_times = 0;
            if (alarm_info[1].input_block.input_state == 1)
            {
                alarm_info[1].input_block.input_trig_mode = INPUT_TRIG_UP;
            }
            else
            {
                alarm_info[1].input_block.input_trig_mode = INPUT_TRIG_DOWN;
            }
        }
    }
    else
    {
        alarm_info[1].input_block.input_middle_state = alarm_info[1].input_block.input_state;
        alarm_info[1].input_block.input_confirm_times = 0;
    }
    if (alarm_info[1].input_block.input_trig_mode == INPUT_TRIG_UP)
    {
        alarm_info[1].block_flag = 1;
    }
    else if (alarm_info[1].input_block.input_trig_mode == INPUT_TRIG_DOWN)
    {
        alarm_info[1].block_flag = 0;
    }
    
    //处理满包和按钮输入信号
    for(i=0; i<PORT_NUM; i++)
    {
        //输入分成两组,IN1~IN24为第一组,IN25~IN48为第二组
        if(i < PORT_NUM/2)//第一组
        {
            input[0] = readInputBitState(i*2);
            input[1] = readInputBitState(i*2+1);
        }
        else//第二组
        {
            input[0] = readInputBitState((i+2)*2);
            input[1] = readInputBitState((i+2)*2+1);
        }

        // handle pack_full single
        if (port_info[i].input_pack_full.input_state != input[0]
            && port_info[i].input_pack_full.input_confirm_times == 0)
        {
            port_info[i].input_pack_full.input_middle_state = input[0];
        }
        if (port_info[i].input_pack_full.input_middle_state == input[0]
            && port_info[i].input_pack_full.input_middle_state != port_info[i].input_pack_full.input_state)
        {
            port_info[i].input_pack_full.input_confirm_times++;
            if (port_info[i].input_pack_full.input_confirm_times > 100)//按钮消抖时间20ms
            {
                port_info[i].input_pack_full.input_state = port_info[i].input_pack_full.input_middle_state;
                port_info[i].input_pack_full.input_confirm_times = 0;
                if (port_info[i].input_pack_full.input_state == 1)
                {
                    port_info[i].input_pack_full.input_trig_mode = INPUT_TRIG_UP;
                    port_info[i].full_flag = 1;
                }
                else
                {
                    port_info[i].input_pack_full.input_trig_mode = INPUT_TRIG_DOWN;
                    port_info[i].full_flag = 0;
                }
            }
        }
        else
        {
            port_info[i].input_pack_full.input_middle_state = port_info[i].input_pack_full.input_state;
            port_info[i].input_pack_full.input_confirm_times = 0;
        }

        
        //handle button signal
        if(  port_info[i].input_button.input_state != input[1]
          && port_info[i].input_button.input_confirm_times == 0)
        {
            port_info[i].input_button.input_middle_state = input[1];
        }
        if(  port_info[i].input_button.input_middle_state == input[1]
          && port_info[i].input_button.input_middle_state != port_info[i].input_button.input_state)
        {
            port_info[i].input_button.input_confirm_times++;
            if(port_info[i].input_button.input_confirm_times > 2)//按钮消抖时间20ms
            {
                port_info[i].input_button.input_state = port_info[i].input_button.input_middle_state;
                port_info[i].input_button.input_confirm_times = 0;
                if(port_info[i].input_button.input_state == 1)
                {
                    port_info[i].input_button.input_trig_mode = INPUT_TRIG_UP;
                }
                else
                {
                    port_info[i].input_button.input_trig_mode = INPUT_TRIG_DOWN;
                }
            }
        }
        else
        {
            if(port_info[i].input_button.input_trig_mode == INPUT_TRIG_UP)
            {
                if(input[1] == 1)
                {
                    port_info[i].input_button.input_confirm_times++;
                    if(port_info[i].input_button.input_confirm_times > 300)//按住按键4s以上强制打开
                    {
                        port_info[i].input_button.input_trig_mode = INPUT_TRIG_NULL;
                        port_info[i].light_mode = LIGHT_OFF;
                        port_info[i].port_enable = 0;
                        port_info[i].close_flag =0;
                    }
                }
                else
                {
                    if(port_info[i].input_button.input_confirm_times < 300)
                    {
                        port_info[i].input_button.input_trig_mode = INPUT_TRIG_DOWN;
                        port_info[i].input_button.input_state = 0;
                        port_info[i].input_button.input_middle_state = 0;
                        port_info[i].input_button.input_confirm_times = 0;
                    }
                    else
                    {
                        port_info[i].input_button.input_trig_mode = INPUT_TRIG_NULL;
                        port_info[i].input_button.input_state = 0;
                        port_info[i].input_button.input_middle_state = 0;
                        port_info[i].input_button.input_confirm_times = 0;
                    }
                }
            }
            else
            {
                port_info[i].input_button.input_middle_state = port_info[i].input_button.input_state;
                port_info[i].input_button.input_confirm_times = 0;
            }
        }
        //handle port state
        switch(port_info[i].light_mode)
        {
        case LIGHT_OFF:
            if (port_info[i].full_flag == 1)
            {
                port_info[i].light_mode = LIGHT_FLASH_FAST;
            }
            if(port_info[i].input_button.input_trig_mode == INPUT_TRIG_UP)
            {
                port_info[i].input_button.input_trig_mode = INPUT_TRIG_NULL;              
                port_info[i].light_mode = LIGHT_ON;
                port_info[i].port_enable = 1;
                port_info[i].close_flag = 1;
            }
            break;
        case LIGHT_FLASH_FAST:
            if (port_info[i].full_flag == 0)
            {
                port_info[i].light_mode = LIGHT_OFF;
            }
            if (port_info[i].input_button.input_trig_mode == INPUT_TRIG_UP)
            {
                port_info[i].input_button.input_trig_mode = INPUT_TRIG_NULL;

                port_info[i].light_mode = LIGHT_ON;
                port_info[i].port_enable = 1;
                port_info[i].close_flag = 1;
            }
            break;
        default:
            break;
        }
    }
}

void handleInputScan()
{
    u8 i;
    u8 input[2];

    

    //处理满包和按钮输入信号
    for (i = 0; i < PORT_NUM; i++)
    {
        //输入分成两组,IN1~IN24为第一组,IN25~IN48为第二组
          input[0] = readInputBitState(i);
          input[1] = readInputBitState(i + 24);
        
        

        // handle pack_full single
        if (port_info[i].input_pack_full.input_state != input[0]
            && port_info[i].input_pack_full.input_confirm_times == 0)
        {
            port_info[i].input_pack_full.input_middle_state = input[0];
        }
        if (port_info[i].input_pack_full.input_middle_state == input[0]
            && port_info[i].input_pack_full.input_middle_state != port_info[i].input_pack_full.input_state)
        {
            port_info[i].input_pack_full.input_confirm_times++;
            if (port_info[i].input_pack_full.input_confirm_times > 100)//按钮消抖时间20ms
            {
                port_info[i].input_pack_full.input_state = port_info[i].input_pack_full.input_middle_state;
                port_info[i].input_pack_full.input_confirm_times = 0;
                if (port_info[i].input_pack_full.input_state == 1)
                {
                    port_info[i].input_pack_full.input_trig_mode = INPUT_TRIG_UP;
                    port_info[i].full_flag = 1;
                }
                else
                {
                    port_info[i].input_pack_full.input_trig_mode = INPUT_TRIG_DOWN;
                    port_info[i].full_flag = 0;
                }
            }
        }
        else
        {
            port_info[i].input_pack_full.input_middle_state = port_info[i].input_pack_full.input_state;
            port_info[i].input_pack_full.input_confirm_times = 0;
        }
        //handle button signal
        if (port_info[i].input_button.input_state != input[1]
            && port_info[i].input_button.input_confirm_times == 0)
        {
            port_info[i].input_button.input_middle_state = input[1];
        }
        if (port_info[i].input_button.input_middle_state == input[1]
            && port_info[i].input_button.input_middle_state != port_info[i].input_button.input_state)
        {
            port_info[i].input_button.input_confirm_times++;
            if (port_info[i].input_button.input_confirm_times > 2)//按钮消抖时间20ms
            {
                port_info[i].input_button.input_state = port_info[i].input_button.input_middle_state;
                port_info[i].input_button.input_confirm_times = 0;
                if (port_info[i].input_button.input_state == 1)
                {
                    port_info[i].input_button.input_trig_mode = INPUT_TRIG_UP;
                    port_info[i].button_flag = 1;
                }
                else
                {
                    port_info[i].input_button.input_trig_mode = INPUT_TRIG_DOWN;
                    port_info[i].button_flag = 0;
                }
            }
        }
        else
        {
                port_info[i].input_button.input_middle_state = port_info[i].input_button.input_state;
                port_info[i].input_button.input_confirm_times = 0;
        }

    }
}

void LightOutPut(u8 index,BitAction BitVal)
{
    switch(index)
    {
    case 0:
        OUT_1_(BitVal);
        break;
    case 1:
        OUT_2_(BitVal);
        break;
    case 2:
        OUT_3_(BitVal);
        break;
    case 3:
        OUT_4_(BitVal);
        break;
    case 4:
        OUT_5_(BitVal);
        break;
    case 5:
        OUT_6_(BitVal);
        break;
    case 6:
        OUT_7_(BitVal);
        break;
    case 7:
        OUT_8_(BitVal);
        break;
    case 8:
        OUT_9_(BitVal);
        break;
    case 9:
        OUT_10_(BitVal);
        break;
    case 10:
        OUT_11_(BitVal);
        break;
    case 11:
        OUT_12_(BitVal);
        break;
    case 12:
        OUT_13_(BitVal);
        break;
    case 13:
        OUT_14_(BitVal);
        break;
    case 14:
        OUT_15_(BitVal);
        break;
    case 15:
        OUT_16_(BitVal);
        break;
    case 16:
        OUT_17_(BitVal);
        break;
    case 17:
        OUT_18_(BitVal);
        break;
    case 18:
        OUT_19_(BitVal);
        break;
    case 19:
        OUT_20_(BitVal);
        break;
    case 20:
        OUT_21_(BitVal);
        break;
    case 21:
        OUT_22_(BitVal);
        break;
    case 22:
        OUT_23_(BitVal);
        break;
    case 23:
        OUT_24_(BitVal);
        break;
    default:
        break;
    }
}
void HandleLightFast(BitAction BitVal)
{
    u8 i;
    
    if(alarm_info[0].block_flag == 0)
    {
        for(i=0; i<10; i++)
        {
            if(port_info[i].light_mode == LIGHT_FLASH_FAST)
            {
                LightOutPut(i,BitVal);
            }
        }
    }
    if(alarm_info[1].block_flag == 0)
    {
        for(i=10; i<PORT_NUM; i++)
        {
            if(port_info[i].light_mode == LIGHT_FLASH_FAST)
            {
                LightOutPut(i,BitVal);
            }
        }
    }
    if(alarm_info[0].yellow_alarm_mode == LIGHT_FLASH_FAST)
    {
        LightOutPut(20,BitVal);
    }
    if (alarm_info[0].red_alarm_mode == LIGHT_FLASH_FAST)
    {
        LightOutPut(21, BitVal);
    }
    if (alarm_info[0].buzzer_alarm_mode == LIGHT_FLASH_FAST)
    {
        LightOutPut(22, BitVal);
    }

    if(alarm_info[1].yellow_alarm_mode == LIGHT_FLASH_FAST)
    {
        LightOutPut(20,BitVal);
    }
    if(alarm_info[1].red_alarm_mode == LIGHT_FLASH_FAST)
    {
        LightOutPut(21,BitVal);
    }
    if(alarm_info[1].buzzer_alarm_mode == LIGHT_FLASH_FAST)
    {
        LightOutPut(22,BitVal);
    }
}

void HandleLightSlow(BitAction BitVal)
{
    u8 i;
    
    if(alarm_info[0].block_flag == 0)
    {
        for(i=0; i<10; i++)
        {
            if(port_info[i].light_mode == LIGHT_FLASH_SLOW)
            {
                LightOutPut(i,BitVal);
            }
        }
    }
    if(alarm_info[1].block_flag == 0)
    {
        for(i=10; i<PORT_NUM; i++)
        {
            if(port_info[i].light_mode == LIGHT_FLASH_SLOW)
            {
                LightOutPut(i,BitVal);
            }
        }
    }
}
void HandleLightOnOff(void)
{
    u8 i;
    
    if(alarm_info[0].block_flag)
    {
        for(i=0; i<10; i++)
        {
            LightOutPut(i,Bit_SET);
        }
    }
    else
    {
        for(i=0; i<10; i++)
        {
            if(port_info[i].light_mode == LIGHT_ON)
            {
                LightOutPut(i,Bit_SET);
            }
            if(port_info[i].light_mode == LIGHT_OFF)
            {
                LightOutPut(i,Bit_RESET);
            }
        }
    }
    if(alarm_info[1].block_flag)
    {
        for(i=10; i<PORT_NUM; i++)
        {
            LightOutPut(i,Bit_SET);
        }
    }
    else
    {
        for(i=10; i<PORT_NUM; i++)
        {
            if(port_info[i].light_mode == LIGHT_ON)
            {
                LightOutPut(i,Bit_SET);
            }
            if(port_info[i].light_mode == LIGHT_OFF)
            {
                LightOutPut(i,Bit_RESET);
            }
        }
    }
    
    if(alarm_info[0].yellow_alarm_mode == LIGHT_ON)
    {
        LightOutPut(20,Bit_SET);
    }
    if(alarm_info[0].yellow_alarm_mode == LIGHT_OFF)
    {
        LightOutPut(20,Bit_RESET);
    }
    if (alarm_info[0].red_alarm_mode == LIGHT_ON)
    {
        LightOutPut(21, Bit_SET);
    }
    if (alarm_info[0].red_alarm_mode == LIGHT_OFF)
    {
        LightOutPut(21, Bit_RESET);
    }
    if (alarm_info[0].buzzer_alarm_mode == LIGHT_ON)
    {
        LightOutPut(22, Bit_SET);
    }
    if (alarm_info[0].buzzer_alarm_mode == LIGHT_OFF)
    {
        LightOutPut(22, Bit_RESET);
    }

    if(alarm_info[1].yellow_alarm_mode == LIGHT_ON)
    {
        LightOutPut(20,Bit_SET);
    }
    if(alarm_info[1].yellow_alarm_mode == LIGHT_OFF)
    {
        LightOutPut(20,Bit_RESET);
    }
    if (alarm_info[1].red_alarm_mode == LIGHT_ON)
    {
        LightOutPut(21, Bit_SET);
    }
    if (alarm_info[1].red_alarm_mode == LIGHT_OFF)
    {
        LightOutPut(21, Bit_RESET);
    }
    if (alarm_info[1].buzzer_alarm_mode == LIGHT_ON)
    {
        LightOutPut(22, Bit_SET);
    }
    if (alarm_info[1].buzzer_alarm_mode == LIGHT_OFF)
    {
        LightOutPut(22, Bit_RESET);
    }

}
void HandleAlarmLight(void)
{
    u8 i;

    alarm_info[0].red_alarm_mode = LIGHT_ON;
    alarm_info[0].yellow_alarm_mode = LIGHT_OFF;
    alarm_info[0].buzzer_alarm_mode = LIGHT_OFF;
    alarm_info[1].red_alarm_mode = LIGHT_ON;
    alarm_info[1].yellow_alarm_mode = LIGHT_OFF;
    alarm_info[1].buzzer_alarm_mode = LIGHT_OFF;

    for (i = 0; i < PORT_NUM; i++)
    {
        if (port_info[i].full_flag)
        {
            alarm_info[0].red_alarm_mode = LIGHT_OFF;
            alarm_info[0].yellow_alarm_mode = LIGHT_FLASH_FAST;
            alarm_info[0].buzzer_alarm_mode = LIGHT_FLASH_FAST;
            alarm_info[1].red_alarm_mode = LIGHT_OFF;
            alarm_info[1].yellow_alarm_mode = LIGHT_FLASH_FAST;
            alarm_info[1].buzzer_alarm_mode = LIGHT_FLASH_FAST;
            break;
        }
    }
}

void OutPutCtrlProc()
{
    HandleAlarmLight();
    HandleLightOnOff();
    
    if(reg_fast > 0)
    {
        reg_fast--;
    }
    if(reg_fast == 0)
    {
        reg_fast = 25;
        if(output_state_fast)
        {
            HandleLightFast(Bit_RESET);
            output_state_fast = 0;
        }
        else
        {
            HandleLightFast(Bit_SET);
            output_state_fast = 1;
        }
    }
    
    if(reg_slow > 0)
    {
        reg_slow--;
    }
    if(reg_slow == 0)
    {
        reg_slow = 50;
        if(output_state_slow)
        {
            HandleLightSlow(Bit_RESET);
            output_state_slow = 0;
        }
        else
        {
            HandleLightSlow(Bit_SET);
            output_state_slow = 1;
        }
    }
}

void WritePortStateBit()
{
    u8 i;
    
    for(i=0; i<PORT_NUM; i++)
    {
        if(port_info[i].full_flag == 1)
        {
            port_full_state |= (1<<i);
        }
        else
        {
            port_full_state &= ~(1<<i);
        }
        if (port_info[i].button_flag == 1)
        {
            port_enable_state |= (1 << i);
        }
        else
        {
            port_enable_state &= ~(1 << i);
        }
    }
}

void port_state_ctrl(u8* data)
{
    u8  close_cmd   = data[1];
    u32 close_state = data[2]|(data[3]<<8)|(data[4]<<16);
    u8  i;
    
    if(close_state)
    {
        for(i=0; i<PORT_NUM; i++)
        {
            if((close_state>>i)&0x1)
            {
                if(close_cmd == 0)//close
                {
                    port_info[i].light_mode = LIGHT_ON;
                    port_info[i].port_enable = 1;
                    port_info[i].close_flag  = 1;
                }
                else//open
                {
                    port_info[i].light_mode = LIGHT_OFF;
                    port_info[i].port_enable = 0;
                    port_info[i].close_flag  = 0;
                }
            }
        }
    }
}

void io_open_all(void)
{
    u8 i;
    
    for(i=0; i<PORT_NUM; i++)
    {

        port_info[i].light_mode = LIGHT_OFF;
        port_info[i].port_enable = 0;
    }
}

void io_outtest_void(void)
{
    u16 i = 0;
    output_test++;
    if (output_test > 1000) {
        output_test = 0;
        if (output_flag == 0) {
            output_flag = 1;
            for (i = 0; i < PORT_NUM; i++) {
                LightOutPut(i, Bit_SET);
            }
            return;

        }
        if (output_flag == 1) {
            output_flag = 0;
            for (i = 0; i < PORT_NUM; i++) {
                LightOutPut(i, Bit_RESET);
            }
            return;

        }
    }

}