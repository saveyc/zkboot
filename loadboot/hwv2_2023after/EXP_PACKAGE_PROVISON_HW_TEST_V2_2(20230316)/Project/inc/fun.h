
typedef struct {
        u16 *queue; /* ָ��洢���е�����ռ� */
        u16 front, rear, len; /* ����ָ�루�±꣩����βָ�루�±꣩�����г��ȱ��� */
        u16 maxSize; /* queue���鳤�� */
}MSG_SEND_QUEUE;

void InitSendMsgQueue(void);
void AddSendMsgToQueue(u16 msg);
u16 GetSendMsgFromQueue(void);
void recv_message_from_sever(u8 *point,u16 *len);
void send_message_to_sever(void);
void udp_send_message(u8 *p_data, u16 len);

