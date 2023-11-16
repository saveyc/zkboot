#include "data.h"
#include "main.h"

/* usart1 queue*/
moto_para_struct  motooneparabuff[USART1_DRIVER_GRP_NUM][USART1_DRIVER_MAX_NUM];
moto_para_queue   motooneprarqueue[USART1_DRIVER_GRP_NUM];

/* usart2 queue*/
moto_para_struct  mototwoparabuff[USART2_DRIVER_GRP_NUM][USART2_DRIVER_MAX_NUM];
moto_para_queue   mototwoprarqueue[USART2_DRIVER_GRP_NUM];

/* uart4 queue*/
moto_para_struct  motothreeparabuff[UART4_DRIVER_GRP_NUM][UART4_DRIVER_MAX_NUM];
moto_para_queue   motothreeprarqueue[UART4_DRIVER_GRP_NUM];


sData_pc2bd_io_state  iostate;

// pc unload data
struct xLIST_ITEM       unloadcar_listitem[UNLOAD_CAR_NODE_LEN];
sData_pc2bd_unload_node   unloadcar_dataitem[UNLOAD_CAR_NODE_LEN];

// pc load data
struct xLIST_ITEM       loadcar_listitem[LOAD_CAR_NODE_LEN];
sData_pc2bd_load_node   loadcar_dataitem[LOAD_CAR_NODE_LEN];

//pc config
sDate_PC_CONFIG_Data    datapcconfig;
//system msg
sDate_SYS_MSG           datasysmsg;
//system err msg
sData_sys_err           datasyserr;

sDate_FIEXD_QUEUE       datafixedqueue;

u16  car_resetcal = 0;

u16  lastslaver_station = 0;

void Data_Init_struct(void)
{
	datapcconfig.car_mode = ONE_CAR_ONE_TIME;
	datapcconfig.car_num = 0;
	datapcconfig.drive_type = DRIVER_TYPE_ONE;
	datapcconfig.mainlinespd = 0;
	datapcconfig.onegroupcarnum = 32;

	datasysmsg.calculate = 0;
	datasysmsg.can_currecv_index = 0;
	datasysmsg.can_prerecv_index = 0;
	datasysmsg.curposition = 0;
	datasysmsg.load_forbid = LICENSE_NO;
	datasysmsg.runflag = 0;
	datasysmsg.unload_pre_index = 0;

	datasyserr.lostnum = 0;
	datasyserr.photoerr = 0;
	datasyserr.unload_last_index = 0;

	datafixedqueue.num = 0;

	iostate.num = 0;

}


void Data_Init_motor_para_queue(void)
{
	u8 i;
	moto_para_queue* q = NULL;

	for (i = 0; i < USART1_DRIVER_GRP_NUM; i++) {
		q = &motooneprarqueue[i];
		q->queue = motooneparabuff[i];
		q->maxsize = USART1_DRIVER_MAX_NUM;
		q->front = q->rear = 0;
	}

	for (i = 0; i < USART2_DRIVER_GRP_NUM; i++) {
		q = &mototwoprarqueue[i];
		q->queue = mototwoparabuff[i];
		q->maxsize = USART2_DRIVER_MAX_NUM;
		q->front = q->rear = 0;
	}

	for (i = 0; i < UART4_DRIVER_GRP_NUM; i++) {
		q = &motothreeprarqueue[i];
		q->queue = motothreeparabuff[i];
		q->maxsize = UART4_DRIVER_MAX_NUM;
		q->front = q->rear = 0;
	}
}

void Data_add_to_moto_para_queue(moto_para_queue* q, moto_para_struct x)
{
	if ((q->rear + 1) % q->maxsize == q->front)
	{
		return;
	}
	q->rear = (q->rear + 1) % q->maxsize;

	q->queue[q->rear] = x;
}

moto_para_struct* getmsgfrommotoparaqueue(moto_para_queue* q)
{
	if (q->front == q->rear) {
		return NULL;
	}
	q->front = (q->front + 1) % q->maxsize;
	return (moto_para_struct*)(&(q->queue[q->front]));
}


void vdata_list_item_init(void)
{
	u16 i = 0;
	sData_pc2bd_unload_node* unloadnode = NULL;
	sData_pc2bd_load_node* loadnode = NULL;

	for (i = 0; i < UNLOAD_CAR_NODE_LEN; i++) {
		unloadcar_dataitem[i].car_unload_exitno = 0xFFFF;
		unloadnode = &unloadcar_dataitem[i];
		unloadnode->index = &unloadcar_listitem[i];
		unloadnode->index->pvOwner = &unloadcar_dataitem[i];
		unloadnode->index->xItemValue = i;
		vListInitialiseItem(unloadnode->index);
	}

	for (i = 0; i < LOAD_CAR_NODE_LEN; i++) {
		loadcar_dataitem[i].car_load_position = 0xFFFF;
		loadnode = &loadcar_dataitem[i];
		loadnode->index = &loadcar_listitem[i];
		loadnode->index->pvOwner = &loadcar_dataitem[i];
		loadnode->index->xItemValue = i;
		vListInitialiseItem(loadnode->index);
	}
}


void vdata_list_int(void)
{
	vListInitialise(&list_unload);
	vListInitialise(&list_load);
}

u16 u16data_searchemptyitem_fromlistunload(void)
{
	u16 i = 0;
	for (i = 0; i < UNLOAD_CAR_NODE_LEN; i++) {
		if (unloadcar_dataitem[i].index->pvContainer == NULL)  return (u16)(i);
	}
	return UNLOAD_CAR_NODE_LEN;
}


u16 u16data_searchemptyitem_fromlistload(void)
{
	u16 i = 0;
	for (i = 0; i < LOAD_CAR_NODE_LEN; i++) {
		if (loadcar_dataitem[i].index->pvContainer == NULL)  return (u16)(i);
	}
	return LOAD_CAR_NODE_LEN;
}


void vdata_addto_list_unload(sData_pc2bd_unload_node x)
{
	u16 nodeindex;

	nodeindex = u16data_searchemptyitem_fromlistunload();

	if (nodeindex == UNLOAD_CAR_NODE_LEN) return;

	unloadcar_dataitem[nodeindex] = x;
	unloadcar_dataitem[nodeindex].index = &(unloadcar_listitem[nodeindex]);
	unloadcar_dataitem[nodeindex].index->pvOwner = &(unloadcar_dataitem[nodeindex]);

	vListInsert(&list_unload, unloadcar_dataitem[nodeindex].index);
}


void vdata_addto_list_load(sData_pc2bd_load_node x)
{
	u16 nodeindex;

	nodeindex = u16data_searchemptyitem_fromlistload();

	if (nodeindex == LOAD_CAR_NODE_LEN) return;

	loadcar_dataitem[nodeindex] = x;
	loadcar_dataitem[nodeindex].index = &(loadcar_listitem[nodeindex]);
	loadcar_dataitem[nodeindex].index->pvOwner = &(loadcar_dataitem[nodeindex]);


	vListInsert(&list_load, loadcar_dataitem[nodeindex].index);
}


u8  u8data_check_io_close_state(u16 x)
{
	u16 i = 0;
	u8 value = IO_OPEN;
	for (i = 0; i < iostate.num; i++) {
		if (iostate.io_info[i] == x) {
			value = IO_CLOSE;
			return value;
		}
	}

	return value;
}

















 