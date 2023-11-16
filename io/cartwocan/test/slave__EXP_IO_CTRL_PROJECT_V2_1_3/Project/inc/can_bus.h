#ifndef __CAN_BUS_H
#define __CAN_BUS_H
#pragma pack (1)

#define MANUAL_VALID   0
#define MANUAL_INVAILD 1

#define OPERATION_INVALID 0
#define OPERATION_VALID   1 

#define CAN_PACK_DATA_LEN 8

enum
{
    CAN_FUNC_ID_PORT_STATE  = 0x2,
    CAN_FUNC_ID_PRINT_STATE = 0x3,
    CAN_FUNC_ID_MANUAL_STATUS = 0xD,
    CAN_FUNC_ID_BOOT_MODE   = 0xF
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

extern u32 port_enable_state;
extern u32 port_print_state;
extern u32 port_full_state;
extern sPort_Alarm_Info alarm_info[2];
extern u16 manual_flag;
extern u16 manual_operation;

void can_bus_send_port_enable_state();

void can_bus_frame_receive(CanRxMsg rxMsg);


//CAN2
void vcanbus_initcansendqueue_two(void);
void vcanbus_addto_cansendqueue_two(sCanFrameExt x);
void vcanbus_framereceive_two(CanRxMsg rxMsg);
void vcan_sendmsg_two(u8* buff, u16 send_total_len, u8 type, u8 dst);
void vcan_sendframe_process_two(void);

#endif