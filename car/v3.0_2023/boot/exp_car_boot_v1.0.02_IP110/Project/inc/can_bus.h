#ifndef _CAN2_BUS_H
#define _CAN2_BUS_H


enum
{
    CAN_FUNC_ID_BOOT_MODE = 0xF
};

enum
{
    CAN_SEG_POLO_NONE = 0,
    CAN_SEG_POLO_FIRST = 1,
    CAN_SEG_POLO_MIDDLE = 2,
    CAN_SEG_POLO_FINAL = 3
};

/*
|    28~22    |    21~20      |    19~12     |     11~7     |    6~0      |    ExtID
|  dst_id(7)  |  seg_polo(2)  |  seg_num(8)  |  func_id(5)  |  src_id(7)  |
*/

#pragma pack (1)


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




#define CAN_RECV_PREFRAME     8


#define CAN_PACK_DATA_LEN       8
#define CAN_SEND_DATA_PK_SIZE   256

#define CAN_RX_DATA_PK_SIZE     256
#define CAN_RX_BUFF_SIZE        300

#define DLOAD_CMD_NULL          0x00
#define DLOAD_CMD_ENTER_BOOT    0xB0
#define DLOAD_CMD_ERASE_FLASH   0xB1
#define DLOAD_CMD_TRAN_DATA     0xB2


//CAN1

extern u8  cur_tran_station;
extern u16 pk_tot_num;
extern u16 pk_cur_num;
extern u16 pk_len;
extern u8  trans_flag;
extern u16 trans_timeout;

extern u16 can_resend_delay;
extern u8  can_resend_cnt;
extern u8  can_send_flag;


extern u8  can_send_buff[];
extern u16 can_send_tot;
extern u16 can_send_len;

extern u8  can_recv_buff[];
extern u16 can_recv_len;
extern u8  g_SegPolo ;
extern u8  g_SegNum ;
extern u16 g_SegBytes ;

extern u16 slave_reply_cmd ;
extern u16 slave_recv_pk_num ;

extern u8 SlaveApp_UpdateFlag ;

extern sCanFrameQueue   canframequeue_one;

void vcan_sendmsg_one(u8* buff, u16 send_total_len, u8 type, u8 dst);

void vcanbus_initcansendqueue_one(void);
void vcanbus_addto_cansendqueue_one(sCanFrameExt x);
void vcanbus_framereceive_one(CanRxMsg rxMsg);
void vcan_sendmsg_one(u8* buff, u16 send_total_len, u8 type, u8 dst);
void vcan_sendframe_process_one(void);
void can_recv_timeout();
void host_transmit_process(void);
u8 sum_check(u8* pdata, u16 len);


void master_can_bus_frame_receive(CanRxMsg rxMsg);


//CAN2
void vcanbus_initcansendqueue_two(void);
void vcanbus_addto_cansendqueue_two(sCanFrameExt x);
void vcanbus_framereceive_two(CanRxMsg rxMsg);
void vcan_sendmsg_two(u8* buff, u8 send_total_len, u8 type, u8 dst);
void vcan_sendframe_process_two(void);


u8 slave_recv_frame_check(void);
void slave_recv_process(void);
void can_bus_reply_process(void);

#endif
