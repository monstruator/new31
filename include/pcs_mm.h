//---------------------------------------------------------------------
//		PCS_MM.H
//     ���� ������� ��६�����, ������� ��� 
//		����뫮� ������ � ��࠭��樨 ������
//		�/�� ��1, ��2, ����1, ����2 
//		� �ࠩ��� ��� ��� �����
//---------------------------------------------------------------------
#ifndef UN
#define UN unsigned
#endif

//		��� ���㫥� ��
//================================
//蠡��� ��થ� ����� �ࠢ����� (��)
//�� ��।��� � ��
UN short otpu_mrk=0x3231;
//�� �ਥ� �� ��
UN short itpu_mrk=0x3332;

//蠡��� ��થ� ����� ��࠭��樨 (��)
//�� ��।��� � ��
UN short otr_mrk=0x3534;
//�� �ਥ� �� ��
UN short itpr_mrk=0x3635;

//��થ�  
#define imrk	ui_BU[i1].mrk		//�ਥ� �� ��
#define omrk	uo_BU[i_c].mrk		//��।�� � ��

//���ᨢ ���� �ᥣ� ����� 
#define imbu	ui_BU[i1].mbu
#define ombu	uo_BU[i_c].mbu

//������ ����������
//��� ������� � ������ �ࠢ�����
#define imd_cmd ui_BU[i1].st_cp.md_cmd
#define omd_cmd uo_BU[i_c].st_cp.md_cmd

//����� � ������ �ࠢ�����
#define is_dt ui_BU[i1].st_cp.dt.d1		
#define ic1_dt ui_BU[i1].st_cp.dt.d2[0]		//1-� �
#define ic2_dt ui_BU[i1].st_cp.dt.d2[1]		//2-� �

#define os_dt uo_BU[i_c].st_cp.dt.d1		
#define oc1_dt uo_BU[i_c].st_cp.dt.d2[0]		//1-� �
#define oc2_dt uo_BU[i_c].st_cp.dt.d2[1]		//2-� �

//����஫쭠� �㬬� � ������ �ࠢ�����
#define icsum ui_BU[i1].st_cp.csum
#define ocsum uo_BU[i_c].st_cp.csum

//������ ������������
//��� �訡�� � ������ ��࠭��樨
#define imd_err ui_BU[i1].st_rpi.md_err
#define omd_err uo_BU[i_c].st_rpo.md_err

//ࠧ��� ����� ��࠭��樨
#define isz ui_BU[i1].st_rpi.sz
#define osz uo_BU[i_c].st_rpo.sz

//���ᨢ ������ ����� ��࠭��樨
#define idt ui_BU[i1].st_rpi.dt
#define odt uo_BU[i_c].st_rpo.dt

//-----------------------------------------
//��� �������� �� ������ ���������� �������
//�� ������ ᮮ�饭�ﬨ � �����⮬
//���ᨢ ������ �ଠ⮢
//#define dt_BU msg_oc.rd_r.uim.dt
#define Rd_dt msg_oc.rd_r.uim.dt


//������ ���������� 
//��� ������� � ������ �ࠢ�����
#define pu8_cmd msg_oc.rd_r.uim.st_cpp.cd_cmd
#define pu8o_cmd msg_ic.wr_s.uom.st_cpo.cd_cmd

//����� � ������ �ࠢ�����
#define pu8_dt msg_oc.rd_r.uim.st_cpp.dt.d1		
#define pu81_dt msg_oc.rd_r.uim.st_cpp.d2[0]		//1-� �
#define pu82_dt msg_oc.rd_r.uim.st_cpp.d2[1]		//2-� �

#define pu8o_dt msg_ic.wr_s.uom.st_cpo.im.d1
#define pu81o_dt msg_ic.wr_s.uom.st_cpo.im.d2[0]
#define pu82o_dt msg_ic.wr_s.uom.st_cpo.im.d2[1]


//������ ������������
//��� �訡�� � ������ ��࠭��樨
#define pr9_err msg_oc.rd_r.uim.st_rpp.err

//ࠧ��� ������ ����� ��࠭��樨 � �����
#define pr9_sz msg_oc.rd_r.uim.st_rpp.sz_dt
#define pr9o_sz msg_ic.wr_s.uom.st_rpo.sz_dt

//���ᨢ ������ ����� ��࠭��樨
#define pr9_dt msg_oc.rd_r.uim.st_rpp.dt
#define pr9o_dt msg_ic.wr_s.uom.st_rpo.dt



//		��� ���㫥�  ���� 
//------------------------------------
// �� ࠡ�� � ��������
//===================================
//���ᨢ ���� �ᥣ� ᮮ�饭�� 
#define im_dt	ui_MSRS[i1].m_dt
#define om_dt	uo_MSRS[i_c].m_dt

//��� ������� � ������ ����
#define i_cmd ui_MSRS[i1].Rd_m.cmd
#define o_cmd uo_MSRS[i_c].Wr_m.cmd

//ࠧ��� �ਭ���� ������ �� ࠤ���⠭樨 (1_0x90)
#define  sz_dtRdo ui_MSRS[i1].Rd_m.st1_90.sz
//���ᨢ ������ �� ࠤ���⠭樨 (1_0x90)
#define  dt_Rdo ui_MSRS[i1].Rd_m.st1_90.dt


//�� ������ ᮮ�饭�ﬨ � �����⮬
//================================
//��� ������� � ������ ����
#define omsrs_cmd msg_oc.rd_r.uim.Rd_m.cmd
#define imsrs_cmd msg_ic.wr_s.uom.Wr_m.cmd

//����� ��� ��।�� � ����
#define Wr_dt msg_ic.wr_s.uom.dt

//��६����� ���ﭨ� �室�� ����� (0x30)
#define i_stt30 msg_oc.rd_r.uim.Rd_m.st_30.stt_in

//��᫥���� �ਭ��� ������� �� ����䥩��
//�ਭ� (0x41)
#define in_dtA msg_oc.rd_r.uim.Rd_m.st_30.cmd_AI

//���ﭨ� �室�� ����� � ��⮢�� ०���
//�ਭ� (0x�0)
#define i_sttC0 msg_oc.rd_r.uim.Rd_m.st_�0.t_in

//����� ����� ���� (0xE0)
#define msrs_v msg_oc.rd_r.uim.Rd_m.st_E0.vr

//ࠧ��� �ਭ���� ������ �� ࠤ���⠭樨 (1_0x90)
#define  isz_Rdo msg_oc.rd_r.uim.Rd_m.st1_90.sz
//����� �ਭ��� �� ࠤ���⠭樨 (1_0x90)
#define  idtRdo msg_oc.rd_r.uim.Rd_m.st1_90.dt

//-----------
//���-�� ������� ᢥ⮤���� (0x20 -0x27)
#define q_f_led msg_ic.wr_s.uom.Wr_m.st_2N.q_fled
//��� ����  (0x20 -0x27)
#define ps_led msg_ic.wr_s.uom.Wr_m.st_2N.ps

//������� ��� �뤠� �� ����䥩��
//�ਭ� (0x40)
#define out_dtA msg_ic.wr_s.uom.Wr_m.st_40.cmd_AO

//��� ��᪨ �� ��室�� ����� � ��⮢�� ०���
//�ਭ� (0x�0)
#define o_sttC0 msg_ic.wr_s.uim.Wr_m.st_�0.t_out

//ࠧ��� ������ ��� ��।�� � ࠤ���⠭�� (1_0x80)
#define  osz_Rdo msg_ic.wr_s.uom.Wr_m.st1_80.sz
//����� ��� ��।�� � ࠤ���⠭�� (1_0x80)
#define  odtRdo msg_ic.wr_s.uom.Wr_m.st1_80.dt

//��� ��ࠬ��� (1_0xB0) !!!���� ���������!!!
#define  prmbB0 msg_ic.wr_s.uom.Wr_m.st1_B0.prmb
//���⮢� ��� (1_0xB0) !!!���� ���������!!!
#define  strB0 msg_ic.wr_s.uom.Wr_m.st1_B0.strb

//᪮���� ��।�� ������ (1_0x�0)
#define tr_spd msg_ic.wr_s.uim.Wr_m.st1_�0.spd


//==================================
//		���������
//==================================

// �������� ᮮ�饭�� �� 
//----------------------------------------------------------
//������� ����� �ࠢ�����
typedef struct 	
	{UN short mrk;		//2� - ��થ�
	 UN char md_cmd;	//1� - ��� �������
	 union	DT_ dt;		//2� - ���⮢� �����
	 UN char csum;		//1� - ����஫쭠� �㬬�
	}PU_BU;				//�ᥣ� 6�

//������� ����� ��࠭��樨 �� �ਥ��
//�� ����� ��
typedef struct 	
	{UN short mrk;		//2� - ��થ�
	 UN char md_err;	//1� - ��� �訡��
	 UN char sz;	    //1� - ࠧ��� ������ ��࠭��樨
	 UN char dt[29];	//�� 28� - ����� +1� ����஫쭠� �㬬�
	}PRT_BUi;				//�ᥣ� �� 33�


//������� ����� ��࠭��樨 �� ��।��
//� ����� ��
typedef struct 	
	{UN short mrk;		//2� - ��થ�
	 UN char sz;	    //1� - ࠧ��� ������ ��࠭��樨
	 UN char dt[29];	//�� 28� - ����� +1� ����஫쭠� �㬬�
	}PRT_BUo;				//�ᥣ� �� 33�



//��ꥤ������ ��� �ਥ�� ����⮢ �� ��
union U_BUi
	{UN char mbu[100];		//���� ����� ��� ���ᨢ ����
	 UN short mrk;			//��થ� �����
	 PU_BU	st_cp;			//������� ��
	 PRT_BUi st_rpi;		//������� �� �� �ਥ��
	};

//��ꥤ������ ��� ��।�� ����⮢ � ��
union U_BUo
	{UN char mbu[100];		//���� ����� ��� ���ᨢ ����
	 UN short mrk;			//��થ� �����
	 PU_BU	st_cp;			//������� ��
	 PRT_BUo st_rpo;		//������� �� �� ��ठ�
	};


//���ᨢ ��६����� ��ꥤ������ ��� 
//��।�� ����⮢ � �� �� 2 �������
union U_BUo uo_BU[2];	

//���ᨢ ��६����� ��ꥤ������ ��� 
//����⮢ �ਥ�� �� �� �� 2 �������
union U_BUi ui_BU[2];	

//=============================
//��ꥤ������ ��� �ਥ�� ᮮ�饭�� �� ����
union U_MSRSi
	{UN char m_dt[300];		//���� ����� ��� ���ᨢ ����
	 union	MSRS_Rd Rd_m;	//��� ࠡ��� � ����
	};

//��ꥤ������ ��� ��।�� ᮮ�饭�� � ����
union U_MSRSo
	{UN char m_dt[300];		//���� ����� ��� ���ᨢ ����
	 union MSRS_Wr Wr_m;	//��� ࠡ��� � ����
	};


//���ᨢ ��६����� ��ꥤ������ ��� 
//��।�� ����⮢ � ���� �� 2 �������
union U_MSRSo uo_MCRS[2];	

//���ᨢ ��६����� ��ꥤ������ ��� 
//����⮢ �ਥ�� �� ���� �� 2 �������
union U_MSRSi ui_MSRS[2];	
