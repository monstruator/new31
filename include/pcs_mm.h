//---------------------------------------------------------------------
//		PCS_MM.H
//     Файл объявления переменных, структур для 
//		пересылок команд и ретрансляции данных
//		в/из БУ1, БУ2, МСРС1, МСРС2 
//		в драйвере ПЦС для Моряны
//---------------------------------------------------------------------
#ifndef UN
#define UN unsigned
#endif

//		для модулей БУ
//================================
//шаблон маркера пакета управления (ПУ)
//на передачу в БУ
UN short otpu_mrk=0x3231;
//на прием из БУ
UN short itpu_mrk=0x3332;

//шаблон маркера пакета ретрансляции (ПР)
//на передачу в БУ
UN short otr_mrk=0x3534;
//на прием из БУ
UN short itpr_mrk=0x3635;

//маркер  
#define imrk	ui_BU[i1].mrk		//прием из БУ
#define omrk	uo_BU[i_c].mrk		//передача в БУ

//массив байт всего пакета 
#define imbu	ui_BU[i1].mbu
#define ombu	uo_BU[i_c].mbu

//ПАКЕТЫ УПРАВЛЕНИЯ
//код команды в пакетах управления
#define imd_cmd ui_BU[i1].st_cp.md_cmd
#define omd_cmd uo_BU[i_c].st_cp.md_cmd

//данные в пакетах управления
#define is_dt ui_BU[i1].st_cp.dt.d1		
#define ic1_dt ui_BU[i1].st_cp.dt.d2[0]		//1-й Б
#define ic2_dt ui_BU[i1].st_cp.dt.d2[1]		//2-й Б

#define os_dt uo_BU[i_c].st_cp.dt.d1		
#define oc1_dt uo_BU[i_c].st_cp.dt.d2[0]		//1-й Б
#define oc2_dt uo_BU[i_c].st_cp.dt.d2[1]		//2-й Б

//контрольная сумма в пакетах управления
#define icsum ui_BU[i1].st_cp.csum
#define ocsum uo_BU[i_c].st_cp.csum

//ПАКЕТЫ РЕТРАНСЛЯЦИИ
//код ошибки в пакетах ретрансляции
#define imd_err ui_BU[i1].st_rpi.md_err
#define omd_err uo_BU[i_c].st_rpo.md_err

//размер пакета ретрансляции
#define isz ui_BU[i1].st_rpi.sz
#define osz uo_BU[i_c].st_rpo.sz

//массив данных пакета ретрансляции
#define idt ui_BU[i1].st_rpi.dt
#define odt uo_BU[i_c].st_rpo.dt

//-----------------------------------------
//ДЛЯ ЗАПРОСОВ НА ЧТЕНИЕ ПОЛУЧЕННЫХ ПАКЕТОВ
//при обмене сообщениями с клиентом
//массив данных форматов
//#define dt_BU msg_oc.rd_r.uim.dt
#define Rd_dt msg_oc.rd_r.uim.dt


//ПАКЕТЫ УПРАВЛЕНИЯ 
//код команды в пакетах управления
#define pu8_cmd msg_oc.rd_r.uim.st_cpp.cd_cmd
#define pu8o_cmd msg_ic.wr_s.uom.st_cpo.cd_cmd

//данные в пакетах управления
#define pu8_dt msg_oc.rd_r.uim.st_cpp.dt.d1		
#define pu81_dt msg_oc.rd_r.uim.st_cpp.d2[0]		//1-й Б
#define pu82_dt msg_oc.rd_r.uim.st_cpp.d2[1]		//2-й Б

#define pu8o_dt msg_ic.wr_s.uom.st_cpo.im.d1
#define pu81o_dt msg_ic.wr_s.uom.st_cpo.im.d2[0]
#define pu82o_dt msg_ic.wr_s.uom.st_cpo.im.d2[1]


//ПАКЕТЫ РЕТРАНСЛЯЦИИ
//код ошибки в пакетах ретрансляции
#define pr9_err msg_oc.rd_r.uim.st_rpp.err

//размер данных пакета ретрансляции в байтах
#define pr9_sz msg_oc.rd_r.uim.st_rpp.sz_dt
#define pr9o_sz msg_ic.wr_s.uom.st_rpo.sz_dt

//массив данных пакета ретрансляции
#define pr9_dt msg_oc.rd_r.uim.st_rpp.dt
#define pr9o_dt msg_ic.wr_s.uom.st_rpo.dt



//		для модулей  МСРС 
//------------------------------------
// при работе с каналами
//===================================
//массив байт всего сообщения 
#define im_dt	ui_MSRS[i1].m_dt
#define om_dt	uo_MSRS[i_c].m_dt

//код команды в пакетах МСРС
#define i_cmd ui_MSRS[i1].Rd_m.cmd
#define o_cmd uo_MSRS[i_c].Wr_m.cmd

//размер принятых данных из радиостанции (1_0x90)
#define  sz_dtRdo ui_MSRS[i1].Rd_m.st1_90.sz
//массив данных из радиостанции (1_0x90)
#define  dt_Rdo ui_MSRS[i1].Rd_m.st1_90.dt


//при обмене сообщениями с клиентом
//================================
//код команды в пакетах МСРС
#define omsrs_cmd msg_oc.rd_r.uim.Rd_m.cmd
#define imsrs_cmd msg_ic.wr_s.uom.Wr_m.cmd

//данные для передачи в МСРС
#define Wr_dt msg_ic.wr_s.uom.dt

//переменная состояния входов модуля (0x30)
#define i_stt30 msg_oc.rd_r.uim.Rd_m.st_30.stt_in

//последняя принятая команда по интерфейсу
//Аринг (0x41)
#define in_dtA msg_oc.rd_r.uim.Rd_m.st_30.cmd_AI

//состояния входов модуля в тестовом режиме
//Аринг (0xС0)
#define i_sttC0 msg_oc.rd_r.uim.Rd_m.st_С0.t_in

//версия модуля МСРС (0xE0)
#define msrs_v msg_oc.rd_r.uim.Rd_m.st_E0.vr

//размер принятых данных из радиостанции (1_0x90)
#define  isz_Rdo msg_oc.rd_r.uim.Rd_m.st1_90.sz
//данные принятые из радиостанции (1_0x90)
#define  idtRdo msg_oc.rd_r.uim.Rd_m.st1_90.dt

//-----------
//кол-во миганий светодиода (0x20 -0x27)
#define q_f_led msg_ic.wr_s.uom.Wr_m.st_2N.q_fled
//код паузы  (0x20 -0x27)
#define ps_led msg_ic.wr_s.uom.Wr_m.st_2N.ps

//команда для выдачи по интерфейсу
//Аринг (0x40)
#define out_dtA msg_ic.wr_s.uom.Wr_m.st_40.cmd_AO

//код маски на выходах модуля в тестовом режиме
//Аринг (0xС0)
#define o_sttC0 msg_ic.wr_s.uim.Wr_m.st_С0.t_out

//размер данных для передачи в радиостанцию (1_0x80)
#define  osz_Rdo msg_ic.wr_s.uom.Wr_m.st1_80.sz
//данные для передачи в радиостанцию (1_0x80)
#define  odtRdo msg_ic.wr_s.uom.Wr_m.st1_80.dt

//код перамбулы (1_0xB0) !!!пока отсутствует!!!
#define  prmbB0 msg_ic.wr_s.uom.Wr_m.st1_B0.prmb
//стартовый бит (1_0xB0) !!!пока отсутствует!!!
#define  strB0 msg_ic.wr_s.uom.Wr_m.st1_B0.strb

//скорость передачи данных (1_0xС0)
#define tr_spd msg_ic.wr_s.uim.Wr_m.st1_С0.spd


//==================================
//		СТРУКТУРЫ
//==================================

// Структуры сообщений БУ 
//----------------------------------------------------------
//структура пакета управления
typedef struct 	
	{UN short mrk;		//2Б - маркер
	 UN char md_cmd;	//1Б - код команды
	 union	DT_ dt;		//2Б - байтовые данные
	 UN char csum;		//1Б - контрольная сумма
	}PU_BU;				//всего 6Б

//структура пакета ретрансляции по приему
//из модуля БУ
typedef struct 	
	{UN short mrk;		//2Б - маркер
	 UN char md_err;	//1Б - код ошибки
	 UN char sz;	    //1Б - размер данных ретрансляции
	 UN char dt[29];	//до 28Б - данные +1Б контрольная сумма
	}PRT_BUi;				//всего до 33Б


//структура пакета ретрансляции по передачи
//в модуль БУ
typedef struct 	
	{UN short mrk;		//2Б - маркер
	 UN char sz;	    //1Б - размер данных ретрансляции
	 UN char dt[29];	//до 28Б - данные +1Б контрольная сумма
	}PRT_BUo;				//всего до 33Б



//объединение для приема пакетов из БУ
union U_BUi
	{UN char mbu[100];		//весь пакет как массив байт
	 UN short mrk;			//маркер пакета
	 PU_BU	st_cp;			//структура ПУ
	 PRT_BUi st_rpi;		//структура ПР по приему
	};

//объединение для передачи пакетов в БУ
union U_BUo
	{UN char mbu[100];		//весь пакет как массив байт
	 UN short mrk;			//маркер пакета
	 PU_BU	st_cp;			//структура ПУ
	 PRT_BUo st_rpo;		//структура ПР по пердаче
	};


//массив переменных объединений для 
//передачи пакетов в БУ по 2 каналам
union U_BUo uo_BU[2];	

//массив переменных объединений для 
//пакетов приема из БУ по 2 каналам
union U_BUi ui_BU[2];	

//=============================
//объединение для приема сообщений из МСРС
union U_MSRSi
	{UN char m_dt[300];		//весь пакет как массив байт
	 union	MSRS_Rd Rd_m;	//для работы с МСРС
	};

//объединение для передачи сообщений в МСРС
union U_MSRSo
	{UN char m_dt[300];		//весь пакет как массив байт
	 union MSRS_Wr Wr_m;	//для работы с МСРС
	};


//массив переменных объединений для 
//передачи пакетов в МСРС по 2 каналам
union U_MSRSo uo_MCRS[2];	

//массив переменных объединений для 
//пакетов приема из МСРС по 2 каналам
union U_MSRSi ui_MSRS[2];	
