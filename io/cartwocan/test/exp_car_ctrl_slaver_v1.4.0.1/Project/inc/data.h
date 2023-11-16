#ifndef _DATA_H
#define _DATA_H

#include "stm32f107.h"

#define   TEN_MS                    10

#define   USART2_DRIVER_GRP_NUM     30
#define   USART2_DRIVER_MAX_NUM     3

#define   USART1_DRIVER_GRP_NUM     30
#define   USART1_DRIVER_MAX_NUM     3

#define   UART4_DRIVER_GRP_NUM      30
#define   UART4_DRIVER_MAX_NUM      3

#define   MAX_LOAD_PLATFORM_NUM     48
#define   MAX_UNLOAD_CAR_NUM        48

#define   MAX_CAR_SPD               127
#define   MAX_CAR_DELAY             255
#define   MAX_CAR_LEN               127

//data len
#define   UNLOAD_CAR_NODE_LEN       60
#define   LOAD_CAR_NODE_LEN         60

#define   UNLOAD_FRAME_LEN          12
#define   LOAD_FRAME_LEN            10 

/* usart status*/
#define	  RECV_DATA		            1
#define	  SEND_DATA		            2
#define   SENDING_DATA              3
#define	  RECV_DATA_END	            4

/* moto ctrl process */
#define SEND_PARA_STATUE            1
#define WAIT_RECV_PARA              2
#define WAIT_RECV_RUN               3
#define SEND_ASK_RUN_STATE          4
#define RECV_RUN_ACK_STATE          5

/* motor ctrl status*/
#define  SNED_CMD_NONE              1
#define  SNED_PARA_CMD              2 
#define  SEND_RUN_CMD               3
#define  SEND_ASKRUN_CMD            4

#define  MAX_IO_NUM                1000

#define MOTO_RUNASK_SHUT_TIMER     1500;


typedef enum {
    COM_ERR = 0x01,                    //ͨѶ����
    CURRENT_PROTECTION = 0x02,         //����
    MOTOR_BELT = 0x04,                 //���Ͳ�쳣
    OVER_VOLTAGE = 0x08,               //��ѹ
    COM_LOST = 0x01,                   //ͨѶ�쳣
} eDrive_err_TypeDef;

typedef enum {
    COM_TWO_LOST = 0xFF,              //δ�ظ���Ϣ
    COM_TWO_ERR = 0xFE,               //�ص���Ϣ��ʽ����ȷ
}eDrive_errtwo_Typedef;

/* send run msg or not*/
#define  MSG_VALID                 1
#define  MSG_INVALID               0

#define  VALUE                     1
#define  INVALID                   0

// ����Ƿ�ر�
#define  IO_CLOSE                  1
#define  IO_OPEN                   0

// �Ƿ�������֡
#define  NOERR_RUN                 0           //����������֡
#define  ERR_RUN                   1           //δ��������֡

// �·�ָ��� �����ڼ���  �رո�� 
#define  RUN_CLOSE                 2



// �Ƿ���������
#define     LICENSE_NO             0
#define     LICENSE_OK             1

// ����������
#define     DRIVER_TYPE_ONE        1
#define     DRIVER_TYPE_TWO        2
#define     DRIVER_TYPE_THREE      3         //NC�����������


//���� or һ��˫��
#define     ONE_CAR_ONE_TIME       1
#define     TWO_CAR_ONE_TIME       2




typedef struct
{
    u8     state;               //��Ҫ�������ʲô����
    u8     runerr;              //δ������֡����
    u8     num;
    u8     dir;
    u8     speed;
    u8     dely;
    u8     juli;
    u16    chutdown;            //������ֱ��ת�����Ͳ
    u16    waitrun;             //�ȴ���ѯС���Ƿ�ת��
    u8     runcnt;              //ת���ڼ��ж��Ƿ�رո��
    u16    chuteindex;          //�������
    u16    zeroinit;            //С��Ѱ��
    u16    juliextern;          //�˶�������չ
    u16    carresetcnt;         //С����ת����
    u16    cartestcnt;          //�㶯С����ת
}moto_para_struct;

typedef struct
{
    moto_para_struct* queue;
    u16 front, rear, len;
    u16 maxsize;
}moto_para_queue;

// pc2bd load node
typedef struct {
    struct xLIST_ITEM* index;
    u16 car_load_index;
    u16 car_load_position;
    u16 car_load_length;                   //mm
    u16 car_load_delay;                    //ms
    u8  car_load_dir;
    u8  car_load_speed;
}sData_pc2bd_load_node;

//pc2bd unload node
typedef struct {
    struct xLIST_ITEM* index;
    u16 car_unload_index;
    u16 car_unload_exitno;
    u16 car_rotate_length;
    u16 car_unload_delay;
    u8  car_unload_dir;
    u8  car_exit_speed;
    u16 car_chute_index;
}sData_pc2bd_unload_node;


//pc2bd io state
typedef struct {
    u16  io_info[MAX_IO_NUM];
    u16  num;
}sData_pc2bd_io_state;

/* ��ʼ����Ϣ*/
typedef struct {
    u8   car_mode;                        //������1��˫��
    u16  car_num;                         //С��������
    u16  mainlinespd;                     //�����ٶ�
    u8   drive_type;                      //���Ͳ����������
    u8   onegroupcarnum;                  //һ���Ӱ忨���Ƶ�С��������
}sDate_PC_CONFIG_Data;

typedef  struct {
    u8   load_forbid;                      //��ֹ����
    u16  curposition;                      //ͷ��λ��
    u8   calculate;                        //�����ϰ�����
    u8   runflag;                          //�·�����ָ��
    u32  can_prerecv_index;                //��һ��can��֡��� 
    u32  can_currecv_index;                //����can��֡���
    u32  unload_pre_index;                 //��һ��3����1��֡���
}sDate_SYS_MSG;

typedef struct {
    u16 lostnum;                           //��ʧ4����1�Ĵ���
    u32 unload_last_index;                 //��һ��3����1��֡���
    u16 photoerr;                          //����������
}sData_sys_err;

typedef struct {
    u16 fixed_position;
    u8  fixed_dir;
    u8  fixed_spd;
    u16 fixed_len;
    u16 fixed_delay;
}sDate_FIXED_MSG;

typedef struct {
    sDate_FIXED_MSG  fixedmsg[4];
    u8 num;
}sDate_FIEXD_QUEUE;


extern sDate_PC_CONFIG_Data    datapcconfig;
extern sDate_SYS_MSG           datasysmsg;
extern sData_sys_err           datasyserr;
extern sDate_FIEXD_QUEUE       datafixedqueue;

extern u16  car_resetcal;

extern moto_para_queue   motooneprarqueue[];
extern moto_para_queue   mototwoprarqueue[];
extern moto_para_queue   motothreeprarqueue[];

extern sData_pc2bd_io_state  iostate;
extern u16  lastslaver_station;

// pc unload data
extern struct xLIST_ITEM       unloadcar_listitem[];
extern sData_pc2bd_unload_node   unloadcar_dataitem[];

// pc load data
extern struct xLIST_ITEM       loadcar_listitem[];
extern sData_pc2bd_load_node   loadcar_dataitem[];


void Data_Init_struct(void);
//motor
void Data_Init_motor_para_queue(void);
void Data_add_to_moto_para_queue(moto_para_queue* q, moto_para_struct x);
moto_para_struct* getmsgfrommotoparaqueue(moto_para_queue* q);

// list
void vdata_list_item_init(void);
void vdata_list_int(void);
void vdata_addto_list_unload(sData_pc2bd_unload_node x);
void vdata_addto_list_load(sData_pc2bd_load_node x);
u8   u8data_check_io_close_state(u16 x);

#endif