//---------------------------------------------------------------------
//		PCS_M.H
//     ���� ������� ��६����� � �㭪権 ����� PCS 
//---------------------------------------------------------------------
#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
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


#ifndef __PCS_H__
#define __PCS_H__

//=============================================================
//�����䨪���� �������
#define	BU1		1
#define	BU2		2
#define	IK1		3
#define	IK2		4
#define	REZ1 	5
#define	REZ2 	6
#define	MSRS0	7
#define	MSRS1	8

//???????????????????
//??#define	MSRS0	3
//??#define	MSRS1	4
//???????????????????

//०��� ࠡ���  �ࢥ� - ������
//������� ���⮩�� � �ࠢ����� ���
//⨯� ᮮ�饭��
#define INIT_PCS	1   //���樠������ ���
#define START_PCS	2	//����᪠�� �� ������ ��� � ࠡ���
#define STOP_PCS	3	//��⠭�������� ������ ���
#define RD_CPCS		4	//���뢠��� �� ������ ��� ������
#define WR_CPCS		5 	//������ � ����� ��� ������
#define	ERR_PCS		6	//�訡�� �� ࠡ�� � ���
#define	RST_CPCS	7	//��� ������ ���

//⨯ ᮮ�饭�� ��/ ������� �� 
//-----------------------------
#define	MSG_PBU_P	8	//����� ��� �� ࠡ�� � 
						//��1, ��2 �ࠢ���騬�
						//����⠬
#define	MSG_PBU_R	9	//����� ��� �� ࠡ�� � ��1, ��2
						//� ०��� ��࠭��樨

//⨯ ᮮ�饭�� � ������ �� 
#define	MSG_WBU_P	18	//����� ��� �� ࠡ�� � 
						//��1, ��2 �ࠢ���騬�
						//����⠬
#define	MSG_WBU_R	19	//����� ��� �� ࠡ�� � ��1, ��2
						//� ०��� ��࠭��樨

//⨯ ᮮ�饭�� ��/� ������ ���� 
//-----------------------------
#define	R_MSRS0	10		
#define	R_MSRS1	11

#define	W_MSRS0	12		
#define	W_MSRS1	13

//०��� ࠡ���  parent - child 
//����ᮢ �ࠩ���
//⨯� ᮮ�饭��
#define INT_PRX		21   //����� Proxy ��� ���뢠���
#define PRX_CNL		22	 //��।�� Proxy ��� ���ନ஢����
						 //� ��室� ᮮ�饭�� � ������
#define NPRX_CNL	23	 //�⪫�祭�� Proxy ��� ���ନ஢����
						 //� ��室� ᮮ�饭�� � ������

//--------------------------------
//������� �ࠢ����� ������� ��
//---------------------
#define	BU_UR		0x71		//����祭�� ���祭�� �஢��
#define BU_K1K6		0x72		//�ࠢ����� ५� �1 - �6
#define BU1_K7		0x74		//�ࠢ����� ५� �7 ��� ��1
#define BU2_K7		0x73		//�ࠢ����� ५� �7 ��� ��2
#define BU_RES90	0x75		//�ࠢ����� ५� ���90
#define BU_BKU		0x76		//����祭�� ���祭�� ᨣ����� ���
#define BU_ONCBU	0x77		//����祭�� ०��� ����஫�
								//��ࠢ���� ��
#define BU_VR		0x78	  	//����祭�� ���ᨨ �����
#define BU_ERR		0x66		//����� �� �訡��

//================================================================

//������� ��� ������ ����0
//----------------
#define OFFRadio	0x10	//�몫���� ࠤ���⠭��
#define ONRadio		0x11	//������� ࠤ���⠭��
#define	TLED0		0x20	//������� ᢥ⮤����� 0
#define	TLED1		0x21	//������� ᢥ⮤����� 1
#define	TLED2		0x22	//������� ᢥ⮤����� 2
#define	TLED3		0x23	//������� ᢥ⮤����� 3
#define	TLED4		0x24	//������� ᢥ⮤����� 4
#define	TLED5		0x25	//������� ᢥ⮤����� 5
#define	TLED6		0x26	//������� ᢥ⮤����� 6
#define	TLED7		0x27	//������� ᢥ⮤����� 7
#define STInpMd		0x30	//㧭��� ���ﭨ� �室�� �����
#define	OC_ARING	0x40	//�뤠�� ������� � ����䥩� �����
#define RC_ARING	0x41	//������ ��᫥���� �ਭ����
							//�� ࠤ���⠭樨 ������� �� ������ 
#define ONT_IO		0x80	//��३� � ०�� ���஢���� ��./���.
#define OFFT_IO		0x81	//��� �� ०��� ���஢���� ��./���.
#define	T_IO		0xC0	//�஢��� ���஢���� ��./���.
//----������� ������㥬� ����0 
#define YDt_ARING	0xF8	//�ਭ��� ����� �� ������ ����� �� ࠤ����.
#define	MdfS_Cnd	0xF9	//���������� ���ﭨ� ᨣ���� "��ࠢ�����"

//������� ��� ������ ����1
//----------------
#define TrRadio		0x40	//��।��� ����� �� ���� � ࠤ����.
#define	NTrRadio	0x41	//����� �� ���. �� ��।���, �� �����祭�
							//�।���� ��।��
#define	FillBuf		0x80	//��������� ����묨 ���� ��।��
#define	RdBuf		0x90	//������ ���� �ਭ���� ������
#define	StrB		0xB0	//������ �ॠ���� � ���⮢� ���
#define SpTr		0xC0	//������ ᪮���� ��।�� ������
//-----������� ������㥬� ����1
#define YDtST2		0xF0	//�ਭ��� ����� �� ������ ���� �2
#define	ErrDtST2	0xF1	//��९������� ���� �� �ਭ�⨨ 
							//������ �� ������ ����-�2

//���� ������� ��� ����0 � ����1
#define RdVMd		0xE0		//������ ����� ����� ����

//����⠭�� ��⠭���� ᪮��� ��।�� ������ 
#define S1200	00
#define S2400	01
#define S4800	02
#define S16000	03
 


//==================================
//		���������
//==================================
//��ꥤ������ ��� ����⥫� ᪮���
union 
	{UN short s_dlt;		
	 UN char uc_dlt[2];
	}dlt;	




// �������� ᮮ�饭�� ��।�� ���ଠ樨 ����� ����ᠬ� 
//----------------------------------------------------------
//��ࢥ�- ������
//==========================================================
//���樠������ ���
//������� ᮮ�饭�� ���뫠���� �ࠩ���� (�ࢥ��)
typedef struct 	
	{UN short type;			//⨯ ᮮ�饭��
	 int 	cnl;			//����� ������ (�� 1 �� 8)
	 						//=0, � ���樠���������� �� ������ �ࠧ�
	 int 	speed;		    //᪮����
	 int 	b_info;			//���-�� ���ଠ樮���� ���
	 int 	b_StrStp;		//���-�� ����/�⮯ ���
	 UN char b_prt;			//����� =0 - ��� �����
	 						//'O'- ����
							//'E'- ��
	 UN int   dev;			//⨯ ���९������� ���ன�⢠
	 						//=0 - ࠡ�� � ०��� ���筮�� RS
							//��� �.�. BU1, BU2, MSRS0, MSRS1  
	 UN int   lvl_inp;		//�஢��� ���������� FIFO ������
	 						//�� ���뢠��� (�� 1 �� 63)
	}INIT_PCS_s;

//	�ਬ�砭��. �� ���樠����樨 ������ ���९������� �� ���ன�⢮�
//				�ॡ���� ��⠭����� ⮫쪮 ��ࠬ���� type � dev

//������� ᮮ�饭�� ����祬�� �����⮬ �� �ࢥ� � �⢥�
typedef struct 
	{UN short type;		//⨯ ᮮ�饭��
	 short status;		//१���� �믮������ ������� 
	 					//=0 - ��ଠ
	}INIT_PCS_r;

//----------
//���� ���
typedef struct 
	{UN short type;		//⨯ ᮮ�饭��
	 int	cnl;		//����� ������ (1 - 8)
	 pid_t 	ID_P;		//ID �����, ����� �㤥� ��।�����/
	 					//�ਭ����� ������ �� ������ (����砥� 
	 					//�㭪�ﬨ qnx_getids(...) ��� geteuid())
	}START_PCS_s;

typedef struct 
	{UN short type;		//⨯ ᮮ�饭��
	 pid_t 	Proxy;		//ID Proxy ����� ���砥� ������
	 					//�� ��室� ����� ������ � ���⮢��襬
						//������
	 short status;		//१���� �믮������ ������� 
	 					//=0 - ��ଠ	
	}START_PCS_r;

//----------------- 
//�⮯ ���
//��⠭�������� 㪠����� ����� ���
//���뢠�� ���� ���������� �� �ਥ��
//��ᮥ����� Proxy ������ �� ����� ������
typedef struct 
	{UN short type;		//⨯ ᮮ�饭��
	 int	cnl;		//����� ������ (1 - 8)
	}STOP_PCS_s;

typedef struct 
	{UN short type;		//⨯ ᮮ�饭��
	 short status;		//१���� �믮������ ������� 
	 					//=0 - ��ଠ
	}STOP_PCS_r;

//---------------
//�⢥� �� �訡��
typedef struct 
	{UN short type;		//⨯ ᮮ�饭��
	 short status;		//=1 - ����祭� �� ������
	 					//������� ������᭮�� ⨯�
	}ERR_PCS_r;

	
//------------------------------------
//����� ����� ���������� � ������ ����� 
//___________
//��� ᮮ�饭�� �� ������ ����� 
union DT_ 
	{UN short	d1;
	 UN char 	d2[2];
	};

//===================================================
//�������� ��� ࠡ��� � ��
//---------------------------
//������� ���ଠ樮���� ��� 
//��� �⢥� ����� �ࠢ����� �� ��
typedef struct
	{UN char cd_cmd;	//1� ��� ����� �ࠢ�����
	 union DT_ 	im;     //2� ����� ����� �ࠢ����� 
	}C_PCK;
	
//������� ���ଠ樮���� ��� 
//��� ����� ��࠭��樨 �� ��� � ��
typedef struct
	{UN char err;		//1� - �訡��
	 UN char sz_dt;		//1� - ���-�� ���� ������
	 UN char dt[28];	//�� 28� - �����
	}R_PCK;

//===================================================
//�������� ��� ࠡ��� � ����
//-----------------------------
typedef struct
	{UN char cmd;		//��� �������
	 UN short stt_in:9;	//���ﭨ� ��. �����
	 UN short :7;		//
	}R_MSRS0_30;

typedef struct
	{UN char cmd;		//��� �������
	 UN int cmd_AI;		//����� � �ਭ��
	}R_MSRS0_41;

typedef struct
	{UN char cmd;		//��� �������
	 UN char t_in;		//��᪠ ᨣ����� �� ��. �����
	}R_MSRS0_C0;

typedef struct
	{UN char cmd;		//��� �������
	 UN char vr;		//����� ����� ����
	}R_MSRS_E0;		//� ��⮢�� ०���

typedef struct
	{UN char cmd;		//��� �������
	 UN char sz;		//ࠧ��� �ਭ���� ������
	 UN char dt[256];	//���ᨢ �ਭ���� ������
	}R_MSRS1_90;		

union MSRS_Rd			
	{UN char 	cmd;
	 R_MSRS0_30 st_30;
	 R_MSRS0_41 st_41;
	 R_MSRS0_C0 st_C0;
	 R_MSRS_E0 	st_E0;
	 R_MSRS1_90 st1_90; 
	};	
//ࠡ�� �ࠧ� �१ ��ꥤ������
//�� ᫥���騬 ��������
//��� ����0: 0x10, 0x11, 0x20 - 0x27, 0x40, 0x80
//			 0x81, 0xF8, 0xF9
//��� ����1: 0x40, 0x41, 0x80,0xB0, 0xC0, 0xF0, 0xF1 
//===================================================
	
//��ꥤ������ ��� �ਭ���� ������
union IM_ 
	{C_PCK st_cpp;			//����� �ࠢ�����
	 R_PCK st_rpp;			//����� ��࠭��樨
	 union	MSRS_Rd Rd_m;	//��� ࠡ��� � ����
	 UN char dt[300];		//���ᨢ �ਭ���� ����
	};

//___________
typedef struct 
	{UN short type;			//⨯ ᮮ�饭��		    
	 int 	cnl;			//����� ������ (1 - 8)
	}RD_CPCS_s;

typedef struct 
	{UN short type;			//type==4 - �⥭�� ������ ��� �ଠ�		    
							//type==8 - ����� �ࠢ����� ��
	 						//type==9 - ����� ��࠭��樨 ��
	 						//type==10 - ᮮ�饭�� �� ����0
	 						//type==11 - ᮮ�饭�� �� ����1
	 short status;			//१���� �믮������ ������� 
	 						//=0 - ��ଠ
							//-1 - ������ ���
							//-2 -����� �� ���⮢��
	 UN int cnt_msg;		//���-�� �ਭ���� ᮮ�饭��
	 				        //(�᫨ ��।����� ࠡ�� � ���ன�⢮�) 
	 UN int  cnt;			//���-�� �ਭ���� ����
	 union IM_ 	uim;		//��ꥤ������ �ਭ���� ����
	}RD_CPCS_r;

//-------------------------------------
//������� ����� � ����� ����� 

//===================================================
//�������� ��� ࠡ��� � ��
//---------------------------
//������� ���ଠ樮���� ��� 
//��� �⢥� ����� �ࠢ����� ��� ��
typedef struct
	{UN char cd_cmd;	//1� ��� ����� �ࠢ�����
	 union DT_ 	im;     //2� ����� ����� �ࠢ����� 
	}C_PK;
	
//������� ���ଠ樮���� ��� 
//��� ����� ��࠭��樨 �� �� � ���
typedef struct
	{UN char sz_dt;		//1� - ���-�� ���� ������
	 UN char dt[28];	//�� 28� - �����
	}R_PK;

//===================================================
//�������� ��� ࠡ��� � ����
//-----------------------------
typedef struct
	{UN char cmd;		//��� �������  (�� 0x20 �� 0x27 )
						//�⮤���� �� 0 �� 7
	 UN char q_fled:5;	//���-�� �������  
	 UN char ps:3;		//��� ����
	}W_MSRS0_2N;

typedef struct
	{UN char cmd;		//��� ������� 
	 UN int cmd_AO;		//������� � �ਭ�  
	}W_MSRS0_40;

typedef struct
	{UN char cmd;		//��� ������� 
	 UN char t_out;		//��� ��᪨ �� ��室�� ����� 
	}W_MSRS0_C0;

typedef struct
	{UN char cmd;		//��� ������� 
	 UN char sz;		//ࠧ��� ���ᨢ� ��� ��।�� 
	 UN char dt[256];	//���ᨢ ��।�� 
	}W_MSRS1_80;

typedef struct
	{UN char cmd;		//��� ������� 
	 UN char prmb;		//��� �ॠ���� 
	 UN char strb;		//��� ���⮢��� ��� 
	}W_MSRS1_B0;

typedef struct
	{UN char cmd;		//��� ������� 
	 UN char spd;		//᪮���� ��।�� ������ 
	}W_MSRS1_C0;

union MSRS_Wr			
	{UN char 	cmd;
	 W_MSRS0_2N st_2N;
	 W_MSRS0_40 st_40;
	 W_MSRS0_C0 st_C0;
	 W_MSRS1_80 st1_80; 
	 W_MSRS1_B0 st1_B0; 
	 W_MSRS1_C0 st1_C0; 
	};	
//ࠡ�� �ࠧ� �१ ��ꥤ������
//�� ᫥���騬 �������� (������� ��⮨� �� ����� ���� 
//�������)
//��� ����0: 0x10, 0x11, 0x30, 0x41, 0x80
//			 0x81, 0xE0
//��� ����1: 0x40, 0x90, 0xE0. 
	
//===================================================
	
union OM_ 
	{C_PK st_cpo;			//����� �ࠢ�����
	 R_PK st_rpo;			//����� ��࠭��樨
	 union MSRS_Wr Wr_m;	//��� ࠡ��� � ����
	 UN char dt[300];		//���ᨢ �ਭ���� ����
	};

typedef struct 
	{UN short type;			//⨯ ᮮ�饭��
	 int 	cnl;			//����� ������ (1 - 8)
	 UN int  cnt;			//���-�� ���� ��� ��।��
	 union OM_ 	uom;		//��ꥤ������ �����뢠���� ������
	}WR_CPCS_s;

typedef struct 
	{UN short type;			//⨯ ᮮ�饭��
	 short status;			//१���� �믮������ ������� 
	 						//=0 - ��ଠ
							//-1 - ����� �� ���⮢��
							//-2 - ����� �� ���९��� �� ��
							//-3 - �� ��ୠ� ������� �ࠢ����� ५� �7 ��
							//-4 - ����0 �� � ��⮢�� ०���
							//-5 - ����ଠ�� ����� � UART
	}WR_CPCS_r;

//--------------------------------------
//��� ������ ����� 
//���뢠�� ���� ���������� �� �ਥ��
//� FIFO ������(��)
typedef struct 
	{UN short type;			//⨯ ᮮ�饭��
	 int 	cnl;			//����� ������ (1 - 8)
	 						//�� cnl=0, ���뢠���� �� ������ ����� 
	}RST_CPCS_s;

typedef struct 
	{UN short type;			//⨯ ᮮ�饭��
	 short status;			//१���� �믮������ ������� 
	 						//=0 - ��ଠ
	}RST_CPCS_r;


//=================================
//�������� ��� ��।�� ᮮ�饭�� 
//����� ����ᠬ� �ࠩ���
//Parent - Child
//---------------------------------
//����� Proxy �易����� � ���뢠����
//�� ����� 
typedef struct 	
	{UN short type;			//⨯ ᮮ�饭��
	}INT_PRX_s;

typedef struct 
	{UN short type;			//⨯ ᮮ�饭��
	 pid_t 	Proxy;			//ID Proxy
	 short status;			//
	}INT_PRX_r;

//����ன�� Proxy ��� ���ନ஢����
//� ��室� ᮮ�饭�� � ������
//(�� ������� ���� ���)
typedef struct 	
	{UN short type;		//⨯ ᮮ�饭��
	 int cnl;			//����� ������ �� 0 �� 7
	 pid_t 	ID_PRC;		//ID �����, ����� ������ 
	 					//(����᪠�� � ࠡ���) �����
	}PRX_CNL_s;

typedef struct 
	{UN short type;		//⨯ ᮮ�饭��
	 pid_t 	Proxy;		//ID Proxy �ਪ९�������
	 					//� ������
	 short status;
	}PRX_CNL_r;

//�⪫�祭�� Proxy ��� ���ନ஢����
//� ��室� ᮮ�饭�� � ������
//(�� ������� �⮯ ���)
typedef struct 	
	{UN short type;		//⨯ ᮮ�饭��
	 int cnl;			//����� ������ �� 0 �� 7
	}NPRX_CNL_s;

typedef struct 
	{UN short type;		//⨯ ᮮ�饭��
	 short status;
	}NPRX_CNL_r;

//=====================================

//��ꥤ������ ��� ࠡ���
union 
	{UN short 		type;		//⨯ ᮮ�饭��
	 INIT_PCS_s 	ini_s;
	 START_PCS_s  	str_s;
	 STOP_PCS_s  	stp_s;
	 RD_CPCS_s 		rd_s;
	 WR_CPCS_s 		wr_s;
	 RST_CPCS_s		rstc_s;
	 INT_PRX_s 		int_s;
	 PRX_CNL_s  	prc_s;
	 NPRX_CNL_s  	nprc_s;
	}msg_ic;	

union 
	{UN short 		type;		//⨯ ᮮ�饭��
	 INIT_PCS_r 	ini_r;
	 START_PCS_r  	str_r;
	 STOP_PCS_r  	stp_r;
	 RD_CPCS_r 		rd_r;
	 WR_CPCS_r 		wr_r;
	 RST_CPCS_r		rstc_r;
	 ERR_PCS_r  	er_r;
	 INT_PRX_r 	    int_r;
	 PRX_CNL_r      prc_r;
	 NPRX_CNL_r     nprc_r;
	}msg_oc;	

#endif
