
//---------------------------------------------------------------------
//		IO_DT_SOI.H
//     ���� ������� ��६����� � �㭪権 ����� ��-���
//		� ��� ࠡ��� ࠧ���� ᨣ����� 
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

//०��� ࠡ��� 
#define GET_IN		1   //������� ��-� ॣ. �室��� ᨣ�����
#define GET_OUT		9   //������� ��-� ॣ. ��室��� ᨣ�����

//------
//��⠭���� ०��� � ON ࠧ�蠥� ࠡ��� ������ �/�
//�ய�᪠��� ��室�� ᨣ����, OFF - ����頥�
//�ਧ���᪥ �ࠩ��� �� ��室�  ����饭�
#define ED_B_OUT	2	//ࠧ����/������� �� ��室�� ᨣ����
#define ED_B_POST	10	//ࠧ�./����. ��室�� ᨣ���� ���� ���
#define ED_B_SOI	12  //ࠧ�./����. ��室�� ᨣ���� ��� 
#define ED_B_PRIB1	13	//ࠧ�./����. ��室�� ᨣ���� ����1
						
//---------
//��⠭��������/���뢠�� �ࠧ� ��᪮�쪮 㪠������ ��⮢  
//� ॣ���� ��室��� ᨣ����� 
#define S_OUT		3	//��⠭����� ��室�� ᨣ���� �� ��᪥
#define OFF_OUT		4	//����� ��室�� ᨣ���� �� ��᪥

//-----------
//��⠭��������/���뢠�� ᮮ⢥������e ����  
//� ॣ���� ��室��� ᨣ����� 
#define BT_POST_KRK	20	//���./��. ᨣ��� ���� ��� O_DT0
#define BT_POST_RDY	21	//���./��. ᨣ��� ���� ��⮢ O_DT1
#define BT_XOROUT1_1 22	//���./��. १��. ᨣ��� ��� O_DT2
#define BT_XOROUT1_2 23	//���./��. १��. ᨣ��� ��� O_DT3
#define BT_XOROUT1_3 24	//���./��. १��. ᨣ��� ��� O_DT4
#define BT_XOROUT1_4 25	//���./��. १��. ᨣ��� ��� O_DT5
#define BT_XOROUT1_5 26	//���./��. १��. ᨣ��� ��� O_DT6
#define BT_XOROUT1_6 27	//���./��. १��. ᨣ��� ��� O_DT7
#define BT_MD 		28	//���./��. ᨣ��� ��� O_DT8
#define BT_WRK 		29	//���./��. ᨣ��� ��� O_DT9
#define BT_K_1P 	36	//���./��. ᨣ��� ��� O_DT16
#define BT_K_2P 	37	//���./��. ᨣ��� ��� O_DT17
#define BT_K_3P 	38	//���./��. ᨣ��� ��� O_DT18
#define BT_K_4P 	39	//���./��. ᨣ��� ��� O_DT19
#define BT_K_5P 	40	//���./��. ᨣ��� ��� O_DT20
#define BT_XO_SOI_RSV 41	//���./��. १��. ᨣ��� ��� O_DT21
#define BT_XO_PRIB1_KRK 44	//���./��. ᨣ��� ��� O_DT24

//-------
//�� �믮������ OFF ᨣ��� �ନ����� �� ⠩���� 2
//�� ON ᨣ��� ������ �� ��� 0 ॣ���� ��室���
//ᨣ����� 
#define POST_KRK 	58	//�ࠢ����� �ନ஢����� ᨣ���� ���� ���

//�� �믮������ OFF ᨣ��� �ନ����� �� ��� 8 ॣ����
//�室��� ᨣ����� 
//�� ON ᨣ��� ������ �� ��� 1 ॣ���� ��室���
//ᨣ����� 
#define POST_SOI 	66	//�ࠢ����� �ନ஢����� ᨣ���� ���� ���

//�� �믮������ OFF ᨣ��� �ନ����� �� ⠩���� 2
//�� ON ᨣ��� ������ �� ��� 24 ॣ���� ��室���
//ᨣ����� 
#define PRIB1_KRK 	74	//�ࠢ����� �ନ஢����� ᨣ���� ����.1 ���

//-------
#define	ERR_IO  	5	//�訡��

//��।������ ��� ࠡ��� � ॣ���ࠬ�
//������ ���� ॣ���஢ �롨ࠥ� �� ॣ���� 0�10
//���䨣��樮����� ����࠭�⢠ PCI ����� ��-���
//�⮡ࠦ���� ॣ���஢ �ந�������� � ������� 
//�����/�뢮��
//ॣ���� ᨣ����� ���뢠���  4�	0x1000
#define R_INTR		(BA_MPL+0x00)

//ॣ���� ��᪨ ᨣ����� ���뢠���  4�	0x1004
#define R_M_INTR	(BA_MPL+0x04)

//ॣ���� ��砫쭮�� ���祭�� ⠩��� 0  4�	0x1008
#define R_B_TMR0	(BA_MPL+0x08)

//ॣ���� ⥪�饣� ���祭�� ⠩��� 0  4�	0x100C
#define R_C_TMR0	(BA_MPL+0x0C)

//ॣ���� ��砫쭮�� ���祭�� ⠩��� 1  4�	0x1010
#define R_B_TMR1	(BA_MPL+0x10)

//ॣ���� ⥪�饣� ���祭�� ⠩��� 1  4�	0x1014
#define R_C_TMR1	(BA_MPL+0x14)

//ॣ���� ��砫쭮�� ���祭�� ⠩��� 2  4�	0x1018
#define R_B_TMR2	(BA_MPL+0x18)

//ॣ���� ⥪�饣� ���祭�� ⠩��� 0  4�	0x101C
#define R_C_TMR2	(BA_MPL+0x1C)

//ॣ���� �ࠢ�����  4�	0x1020
#define R_CNTR		(BA_MPL+0x20)

//ॣ���� �ࠢ�����  ⠩��ࠬ� 2�	0x1020
#define R_TMR_CNTR	(BA_MPL+0x20)

//ॣ���� ⥪�饩 ���ᨨ � ������ 2� 0x1022
#define RLED_MS_SOI		(BA_MPL+0x22)

//----------------
//�������⥫�� ॣ���� �ࠢ�����  4�	0x1024
//� �ଠ� DD
#define R_CNTR_ADD	(BA_MPL+0x24)

//� �����⭮� �ଠ� 1�
#define R_CNTR_0ADD	(BA_MPL+0x24)
#define R_CNTR_1ADD	(BA_MPL+0x25)
#define R_CNTR_2ADD	(BA_MPL+0x26)
#define R_CNTR_3ADD	(BA_MPL+0x27)
//---------------

//ॣ���� �室��� ������  4�	0x1028
#define R_I_DT		(BA_MPL+0x28)

//ॣ���� ��室��� ������  4�	0x102C
#define R_O_DT		(BA_MPL+0x2C)

//==================================
//		���������
//==================================

// �������� ᮮ�饭�� ��।�� ���ଠ樨 ����� ����ᠬ� 
//----------------------------------------------------------
//������� �室��� ᨣ�����
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
//������� ��室��� ᨣ�����
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

//��ꥤ������ ��� ࠡ��� � �室�묨/��室�묨 ᨣ������
union IN_ 
	{UN int	i_d;
	 R_INP 	st_in;
	 R_OUTP st_out;
	};
//===============================
//������� ��-� �室��� ᨣ�����
typedef struct 	
	{UN short type;
	}INP_S_s;

typedef struct 
	{UN short type;
	 short status;
	 union IN_ u_inp;
	}INP_S_r;
//-----
//������� ��� ��⠭���� ��室��� 
//ᨣ����� �� ��᪥
typedef struct 
	{UN short type;
	 UN int msk;		//��᪠. �� �믮������ ��⠭���� - ����,
	 					//����� �.�. ��⠭������ �ਭ����� ���祭��
						//1, ����� �� �������� 0.
						//�� �믮������ ��� - ����,
	 					//����� �.�. ��襭� �ਭ����� ���祭��
						//0, ����� �� �������� - 1.

	}MSK_A_s;

typedef struct 
	{UN short type;
	 short status;	
	}MSK_A_r;

//---- 
//�������� ��� �믮������ ����⢨� � 
//������묨 ᨣ������ � �ࠢ����� ���
//(���/��⠭����, ��४��祭�� ०��� �ࠢ����� � �.�.)
typedef struct 
	{UN short type;
	 UN char t_act;		//����� �ਭ����� ��-� ON=1 � OFF=0
	 				    //�� ᮮ⢥����� ���ﭨ� ��।�������� ���
						//� ॣ����
	}ACT_S_s;

typedef struct 
	{UN short type;
	 short status;	
	}ACT_S_r;

//-------------
//---------------
//�⢥� �� �訡��
typedef struct 
	{UN short type;		//⨯ ᮮ�饭��
	 short status;		//=1 - ����祭� �� ������
	 					//������� ������᭮�� ⨯�
	}ERR_IO_r;


//��ꥤ������ ��� ࠡ���
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
