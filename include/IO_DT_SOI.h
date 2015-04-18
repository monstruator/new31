
//---------------------------------------------------------------------
//		IO_DT_SOI.H
//     Файл объявления переменных и функций модуля МС-СОИ
//		в части работы разовых сигналов 
//---------------------------------------------------------------------
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <signal.h>
#include <sys/irqinfo.h>
#include <sys/pci.h>
#include <sys/mman.h>
#include <sys/proxy.h>
#include <sys/kernel.h>
#include <sys/sched.h>
#include <sys/name.h>

#ifndef UN
#define UN unsigned
#endif


#ifndef __IO_DT_SOI_H__
#define __IO_DT_SOI_H__

//
#define		OFF		0
#define		ON		1

//режимы работы 
#define GET_IN		1   //получить зн-е рег. входных сигналов
#define GET_OUT		9   //получить зн-е рег. выходных сигналов

//------
//Установка режима в ON разрешает работу буферных м/с
//пропускающих выходные сигналы, OFF - запрещает
//призапуске драйвера все выходы  запрещены
#define ED_B_OUT	2	//разрешить/запретить все выходные сигналы
#define ED_B_POST	10	//разр./запр. выходные сигналы ПОСТ СОИ
#define ED_B_SOI	12  //разр./запр. выходные сигналы СОИ 
#define ED_B_PRIB1	13	//разр./запр. выходные сигналы ПРИБ1
						
//---------
//Устанавливают/сбрасывают сразу несколько указанных битов  
//в регистре выходных сигналов 
#define S_OUT		3	//установить выходные сигналы по маске
#define OFF_OUT		4	//сбросить выходные сигналы по маске

//-----------
//Устанавливают/сбрасывают соответствующиe биты  
//в регистре выходных сигналов 
#define BT_POST_KRK	20	//уст./сбр. сигнал ПОСТ КРК O_DT0
#define BT_POST_RDY	21	//уст./сбр. сигнал ПОСТ Готов O_DT1
#define BT_XOROUT1_1 22	//уст./сбр. резерв. сигнал бит O_DT2
#define BT_XOROUT1_2 23	//уст./сбр. резерв. сигнал бит O_DT3
#define BT_XOROUT1_3 24	//уст./сбр. резерв. сигнал бит O_DT4
#define BT_XOROUT1_4 25	//уст./сбр. резерв. сигнал бит O_DT5
#define BT_XOROUT1_5 26	//уст./сбр. резерв. сигнал бит O_DT6
#define BT_XOROUT1_6 27	//уст./сбр. резерв. сигнал бит O_DT7
#define BT_MD 		28	//уст./сбр. сигнал бит O_DT8
#define BT_WRK 		29	//уст./сбр. сигнал бит O_DT9
#define BT_K_1P 	36	//уст./сбр. сигнал бит O_DT16
#define BT_K_2P 	37	//уст./сбр. сигнал бит O_DT17
#define BT_K_3P 	38	//уст./сбр. сигнал бит O_DT18
#define BT_K_4P 	39	//уст./сбр. сигнал бит O_DT19
#define BT_K_5P 	40	//уст./сбр. сигнал бит O_DT20
#define BT_XO_SOI_RSV 41	//уст./сбр. резерв. сигнал бит O_DT21
#define BT_XO_PRIB1_KRK 44	//уст./сбр. сигнал бит O_DT24

//-------
//при выполнении OFF сигнал формируется по таймеру 2
//при ON сигнал зависит от бита 0 регистра выходных
//сигналов 
#define POST_KRK 	58	//управление формированием сигнала ПОСТ КРК

//при выполнении OFF сигнал формируется по бита 8 регистра
//входных сигналов 
//при ON сигнал зависит от бита 1 регистра выходных
//сигналов 
#define POST_SOI 	66	//управление формированием сигнала ПОСТ СОИ

//при выполнении OFF сигнал формируется по таймеру 2
//при ON сигнал зависит от бита 24 регистра выходных
//сигналов 
#define PRIB1_KRK 	74	//управление формированием сигнала ПРИБ.1 КРК

//-------
#define	ERR_IO  	5	//ошибка

//определения для работы с регистрами
//базовый адрес регистров выбираем из регистра 0х10
//конфигурационного пространства PCI модуля МС-СОИ
//отображение регистров производится в область 
//ввода/вывода
//регистр сигналов прерываний  4Б	0x1000
#define R_INTR		(BA_MPL+0x00)

//регистр маски сигналов прерываний  4Б	0x1004
#define R_M_INTR	(BA_MPL+0x04)

//регистр начального значения таймера 0  4Б	0x1008
#define R_B_TMR0	(BA_MPL+0x08)

//регистр текущего значения таймера 0  4Б	0x100C
#define R_C_TMR0	(BA_MPL+0x0C)

//регистр начального значения таймера 1  4Б	0x1010
#define R_B_TMR1	(BA_MPL+0x10)

//регистр текущего значения таймера 1  4Б	0x1014
#define R_C_TMR1	(BA_MPL+0x14)

//регистр начального значения таймера 2  4Б	0x1018
#define R_B_TMR2	(BA_MPL+0x18)

//регистр текущего значения таймера 0  4Б	0x101C
#define R_C_TMR2	(BA_MPL+0x1C)

//регистр управления  4Б	0x1020
#define R_CNTR		(BA_MPL+0x20)

//регистр управления  таймерами 2Б	0x1020
#define R_TMR_CNTR	(BA_MPL+0x20)

//регистр текущей версии и диодов 2Б 0x1022
#define RLED_MS_SOI		(BA_MPL+0x22)

//----------------
//дополнительный регистр управления  4Б	0x1024
//в формате DD
#define R_CNTR_ADD	(BA_MPL+0x24)

//в побайтном формате 1Б
#define R_CNTR_0ADD	(BA_MPL+0x24)
#define R_CNTR_1ADD	(BA_MPL+0x25)
#define R_CNTR_2ADD	(BA_MPL+0x26)
#define R_CNTR_3ADD	(BA_MPL+0x27)
//---------------

//регистр входных данных  4Б	0x1028
#define R_I_DT		(BA_MPL+0x28)

//регистр выходных данных  4Б	0x102C
#define R_O_DT		(BA_MPL+0x2C)

//==================================
//		СТРУКТУРЫ
//==================================

// Структуры сообщений передачи информации между процессами 
//----------------------------------------------------------
//структура входных сигналов
typedef struct
	{UN int RIN1_1:1;
	 UN int RIN1_2:1;		
	 UN int RIN1_3:1;
	 UN int RIN1_4:1;
	 UN int :4;
	 UN int SOI_RDy:1;
	 UN int NI_R:1;
	 UN int NI_K:1;
	 UN int ISPR_K:1;
	 UN int :4;
	 UN int R1_K:1;	 		
	 UN int R2_K:1;	 		
	 UN int R3_K:1;	 		
	 UN int R4_K:1;	 		
	 UN int R5_K:1;	 		
     UN int RIN2_1:1;
	 UN int RIN2_2:1;		
	 UN int RIN2_3:1;
	 UN int RIN2_4:1;
	 UN int RIN2_5:1;
	 UN int RIN2_6:1;
	 UN int RIN2_7:1;
	 UN int :4;
	}R_INP;
//структура выходных сигналов
typedef struct
	{UN int SOI_KRK:1;
	 UN int PST_SOI:1;		
	 UN int ROUT1_1:1;
	 UN int ROUT1_2:1;
	 UN int ROUT1_3:1;
	 UN int ROUT1_4:1;
	 UN int ROUT1_5:1;
	 UN int ROUT1_6:1;
	 UN int SOI_MD:1;
	 UN int SOI_WRK:1;
	 UN int :6;
	 UN int KR1:1;	 		
	 UN int KR2:1;	 		
	 UN int KR3:1;	 		
	 UN int KR4:1;	 		
	 UN int KR5:1;	 		
     UN int SOI_RSV:1;
	 UN int :2;		
	 UN int PRB1_KRK:1;
	 UN int :7;
	}R_OUTP;

//объединение для работы с входными/выходными сигналами
union IN_ 
	{UN int	i_d;
	 R_INP 	st_in;
	 R_OUTP st_out;
	};
//===============================
//получить зн-е входных сигналов
typedef struct 	
	{UN short type;
	}INP_S_s;

typedef struct 
	{UN short type;
	 short status;
	 union IN_ u_inp;
	}INP_S_r;
//-----
//структура сброса установки выходных 
//сигналов по маске
typedef struct 
	{UN short type;
	 UN int msk;		//маска. При выполнении установки - биты,
	 					//которые д.б. установлены принимают значение
						//1, которые не меняются 0.
						//При выполнении сброса - биты,
	 					//которые д.б. сброшены принимают значение
						//0, которые не меняются - 1.

	}MSK_A_s;

typedef struct 
	{UN short type;
	 short status;	
	}MSK_A_r;

//---- 
//структуры для выполнения действий с 
//конкретными сигналами и управления ими
//(сброс/установка, переключение режима управления и т.п.)
typedef struct 
	{UN short type;
	 UN char t_act;		//может принимать зн-я ON=1 и OFF=0
	 				    //что соответствует состоянию определенного бита
						//в регистре
	}ACT_S_s;

typedef struct 
	{UN short type;
	 short status;	
	}ACT_S_r;

//-------------
//---------------
//ответ при ошибке
typedef struct 
	{UN short type;		//тип сообщения
	 short status;		//=1 - получена от клиента
	 					//команда неизвесного типа
	}ERR_IO_r;


//объединения для работы
//receive
union 
	{UN short type;
	 INP_S_s 		inp_s;
	 MSK_A_s  		msk_s;
	 ACT_S_s  		act_s;
	}msg_i;	

//replay
union 
	{UN short type;
	 INP_S_r 		inp_r;
	 MSK_A_r  		msk_r;
	 ACT_S_r  		act_r;
	 ERR_IO_r  	er_r;
	}msg_o;	

#endif
