#ifndef __CAN_BUS_H
#define __CAN_BUS_H

#include "main.h"

#pragma pack (1)

enum
{
    CAN_FUNC_ID_HEART_TYPE = 0x1,                         //心跳信息
    CAN_FUNC_ID_MOTO_ERR_TYPE = 0x2,                      //小车报警信息
    CAN_FUNC_ID_UNLOAD_TYPE = 0x3,                        //3包合1数据
    CAN_FUNC_ID_FIXED_TYPE = 0x4,                         //支持异常格口配置
    CAN_FUNC_ID_MOTO_RUN_STATE_TYPE = 0x5,                //小车没有动作  
    CAN_FUNC_ID_SLAVER_CAR_FIND_INIT = 0x06,              //小车寻零
    CAN_FUNC_ID_SLAVER_CONFIG = 0x07,                     //从板卡相关配置  
    CAN_FUNC_ID_SLAVER_PHOTO_CNT_TYPE = 0x08,             //从板卡光电计数
    CAN_FUNC_ID_ERR_HEART_TYPE = 0x09,                    //从板初始化信息丢失 
    CAN_FUNC_ID_PHOTO_TYPE = 0xA,                         //最终的光电数据
    CAN_FUNC_ID_UNLOADMSG_REPLY = 0xB,                    //反馈4包合1
    CAN_FUNC_ID_ENTER_BOOT = 0xF,
};

//一条指令就可以发完数据的

enum
{
    CAN_FUNC_ID_SLAVER_PHOTO_CONF = 0x01,                 //从板卡光电配置 
    CAN_FUNC_ID_SLAVER_CAR_RESET_CONFIG = 0x02,           //配置小车反转复位  用于NC交叉带
    CAN_FUNC_ID_REPAIRE_TYPE = 0x3,                       //点动控制小车
    CAN_FUNC_ID_CONFIG_TYPE = 0x4,                        //配置信息
    CAN_FUNC_ID_FORBID_TYPE = 0x5,                        //禁止分拣数据
};


enum
{
    CAN_SEG_POLO_NONE = 0,
    CAN_SEG_POLO_FIRST = 1,
    CAN_SEG_POLO_MIDDLE = 2,
    CAN_SEG_POLO_FINAL = 3
};

/*
|28~22|    21~20      |    19~12     |     11~8     |    7~0      |    ExtID
|dst_id锛?锛墊  seg_polo(2)  |  seg_num(8)  |  func_id(5)  |  mac_id(7)  |
*/

typedef struct
{
    u8 seg_polo;
    u8 seg_num;
    u8 func_id;
    u8 mac_id;
    u8 dst_id;
} sCanFrameExtID;

typedef struct
{
    sCanFrameExtID extId;
    u8 data_len;
    u8 data[8];
} sCanFrameExt;

typedef struct
{
    u8  g_SegPolo;
    u8  g_SegNum;
    u16 g_SegBytes;
}sCanFrameCtrl;

#pragma pack ()

typedef struct
{
    sCanFrameExt* queue;
    u16 front, rear, len;
    u16 maxsize;
}sCanFrameQueue;



#define CAN_PACK_DATA_LEN     8
#define CAN_MAX_FRAME_LEN     1024

#define CAN_SEND_DATA_LEN     100

#define CAN_RECV_PREFRAME     8

#define ALL_SLAVER            0x7F

#define TSR_TERR0    ((uint32_t)0x00000008)
#define TSR_TERR1    ((uint32_t)0x00000800)
#define TSR_TERR2    ((uint32_t)0x00080000)
#define TSR_TME0     ((uint32_t)0x04000000)
#define TSR_TME1     ((uint32_t)0x08000000)
#define TSR_TME2     ((uint32_t)0x10000000)
#define TMIDxR_TXRQ  ((uint32_t)0x00000001)

#define CAN_RX_BUFF_SIZE_TWO      1024
#define CAN_RX_CONFIG_SIZE_TWO    200




extern u8  can_recv_buff_two[];
extern u16 can_recv_len_two;
extern sCanFrameCtrl   canctrl_two;
//
extern u8  can_send_buff_two[];
extern u16 can_send_tot_two;

extern u8 can_recv_config_msg[];
extern u16 can_recconf_len;
extern sCanFrameCtrl   canconf_two;

extern sCanFrameQueue   canframequeue;

//can 2
void vcanbus_upload_heart_msg(void);
void vcanbus_upload_motor_err_msg(void);
void vcanbus_upload_motor_run_abnorable_msg(void);
void vcanbus_upload_err_heart_process(void);


//can2
void vcanbus_init_cansendqueue_two(void);
void vcanbus_addto_cansendqueue_two(sCanFrameExt x);
u8 u8canbus_send_oneframe_two(sCanFrameExt sTxMsg);

void vcanbus_frame_receive_two(CanRxMsg rxMsg);
void vcan_sendmsg_two(u8* buff, u8 send_total_len, u8 type, u8 dst);
void vcan_send_frame_process(void);

//CAN1

extern  sCanFrameCtrl       canctrl_one;

void vcanbus_initcansendqueue_one(void);
void vcanbus_addto_cansendqueue_one(sCanFrameExt x);
u8 u8canbus_send_oneframe_one(sCanFrameExt sTxMsg);
void vcanbus_framereceive_one(CanRxMsg rxMsg);
void vcan_sendmsg_one(u8* buff, u8 send_total_len, u8 type, u8 dst);
void vcan_sendframe_process_one(void);

void vcan2_send_check_slaver(void);
void vcan2_reply_check_slaver(void);
void vcan1_send_check_slaver(void);
void vcan1_reply_check_slaver(void);
void udp_send_can_bus_test(void);

#endif