#ifndef __CAN_BUS_H
#define __CAN_BUS_H

#define CAN_PACK_DATA_LEN 8

#pragma pack (1)

enum
{
    CAN_FUNC_ID_BOOT_MODE  = 0xF
};

enum
{
    CAN_SEG_POLO_NONE   = 0,
    CAN_SEG_POLO_FIRST  = 1,
    CAN_SEG_POLO_MIDDLE = 2,
    CAN_SEG_POLO_FINAL  = 3
};
/*
|    28~22    |    21~20      |    19~12     |     11~8     |    7~0      |    ExtID
|  dst_id(7)  |  seg_polo(2)  |  seg_num(8)  |  func_id(4)  |  src_id(8)  |
*/
typedef struct
{
    u8 seg_polo;
    u8 seg_num;
    u8 func_id;
    u8 src_id;
    u8 dst_id;
} sCanFrameExtID;

typedef struct
{
    sCanFrameExtID extId;
    u8 data_len;
    u8 data[8];
} sCanFrameExt;

#pragma pack ()


typedef struct
{
    sCanFrameExt* queue;
    u16 front, rear, len;
    u16 maxsize;
}sCanFrameQueue;

#define   CAN_RX_DATA_PK_SIZE     256
#define   CAN_RX_BUFF_SIZE        300

#define   DLOAD_CMD_NULL          0x00
#define   DLOAD_CMD_ENTER_BOOT    0xB0
#define   DLOAD_CMD_ERASE_FLASH   0xB1
#define   DLOAD_CMD_TRAN_DATA     0xB2

void can_bus_frame_receive(CanRxMsg rxMsg);
void can_bus_reply_process(void);

//CAN1
void vcanbus_initcansendqueue_one(void);
void vcanbus_addto_cansendqueue_one(sCanFrameExt x);
void vcanbus_framereceive_one(CanRxMsg rxMsg);
void vcan_sendmsg_one(u8* buff, u16 send_total_len, u8 type, u8 dst);
void vcan_sendframe_process_one(void);

//CAN2
void vcanbus_initcansendqueue_two(void);
void vcanbus_addto_cansendqueue_two(sCanFrameExt x);
void vcanbus_framereceive_two(CanRxMsg rxMsg);
void vcan_sendmsg_two(u8* buff, u16 send_total_len, u8 type, u8 dst);
void vcan_sendframe_process_two(void);

void slave_recv_process(void);

void slave_reply_boot_cmd();

#endif