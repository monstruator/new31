//tx_common.h
#define LED 0x326

#define N_B 1200

#define REG_OU 0x0020

//Для формирования ОС
#define RT_QUERY    0x0100 //(11)запрос на обслуживание
#define RT_BUSY     0x0008 //(16)абонент занят
#define RT_ABONENT  0x0004 //(17)неисправность абонента
#define RT_CONTROL  0x0002 //(18)принято управление интерфейсом
#define RT_TERMINAL 0x0001 //(19)неисправность ОУ
#define RT_MASK     0x010F

#define REGIM			1
#define BC_FRAME		2
#define BC_RESULT		3
#define BC_RESULT_CMD	4

#define RT_READ			5
#define RT_WRITE		6
#define RT_MODE_WRITE	7
#define RT_MODE_READ	8
#define ANSWER          9
#define RT_INF_OUT		10
//коды ошибок...
#define ERR			-1
#define ERR_EMPTY  	-2 //нет данных
#define ERR_2       -3 //данные не переданы

#define RBUS	0x8000

//Выдать информацию по очередной выполненной команде управления:
#define NEXT_MODE	0x8000

//#define BYTE_SWAP(x)    ((((x) & 0x00ff) << 8) | (((x) & 0xff00) >> 8))
#define BYTE_SWAP(x)    (x)

typedef enum {false,true} boolean;
