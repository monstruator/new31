#include <unistd.h>
#include <sys/osinfo.h>
#include <time.h>
#include <process.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/dev.h>
#include "IO_DT_SOI.h"
int current_key=0;

//*********************************************************************
int gloriya(UN char work, UN char mode, UN char k_r) {

UN char k_1r, k_2r, k_3r, k_4r, k_5r;
pid_t pid_O=0;
int i, rez=0, err=0, rez1=0;

current_key=k_r;

printf("							������ �����  K%d \n",mode+1);

k_1r=k_r&1; k_2r=(k_r>>1)&1; k_3r=(k_r>>2)&1;
k_4r=(k_r>>3)&1; k_5r=(k_r>>4)&1;
//printf("work=%d  mode=%d  K_1R=%d  K_2R=%d  K_3R=%d  K_4R=%d  K_5R=%d\n",
//work, mode, k_1r, k_2r, k_3r, k_4r, k_5r);

//��।��塞 ����稥 �ࢥ� ࠧ���� ᨣ����� 
pid_O=qnx_name_locate(0,"MS_SOI_IO", 0, NULL);
if(pid_O==-1) {
	printf("�ࠩ��� ࠧ���� ᨣ����� �� ��⮢!\n");
	return(-1);
}

//ࠧ���� ���. ���� ���
msg_i.type=ED_B_SOI; // ���. ���� ����
msg_i.act_s.t_act=ON;
rez1=Send(pid_O, &msg_i, &msg_o, sizeof(msg_i),sizeof(msg_o));
//printf("����襭�� ���. ���� SOI = %d\n", rez1);

//ࠧ���� ���. ���� ����
	    msg_i.type=ED_B_POST; // ���. ���� ����
//	    msg_i.type=ED_B_OUT; // ���. ����
	    msg_i.act_s.t_act=ON; 
	    rez1=Send(pid_O, &msg_i, &msg_o, sizeof(msg_i),sizeof(msg_o));
//printf("����襭�� ���. ���� ���� = %d\n", rez1);

// ��४����� �ନ஢���� ���. ᨣ����� � ���ਨ
	    msg_i.type=POST_SOI;
	    msg_i.act_s.t_act=ON;
	    rez1=Send(pid_O, &msg_i, &msg_o, sizeof(msg_i),sizeof(msg_o));

// �뤠�� ᨣ��� "�����"
msg_i.type=BT_WRK;
msg_i.act_s.t_act=work;
rez1=Send(pid_O, &msg_i, &msg_o, sizeof(msg_i),sizeof(msg_o));
printf("�뤠� ᨣ���� ����� \n");

// �뤠�� ᨣ��� "�����"
msg_i.type=BT_MD;
msg_i.act_s.t_act=mode;
rez1=Send(pid_O, &msg_i, &msg_o, sizeof(msg_i),sizeof(msg_o));
printf("�뤠� ᨣ���� ����� \n");

// �뤠�� ᨣ��� "K-1P"
msg_i.type=BT_K_1P;
msg_i.act_s.t_act=k_1r;
rez1=Send(pid_O, &msg_i, &msg_o, sizeof(msg_i),sizeof(msg_o));
printf("�뤠� ᨣ���� K-1P ");

// �뤠�� ᨣ��� "K-2P"
msg_i.type=BT_K_2P;
msg_i.act_s.t_act=k_2r;
rez1=Send(pid_O, &msg_i, &msg_o, sizeof(msg_i),sizeof(msg_o));
printf(" K-2P ");

// �뤠�� ᨣ��� "K-3P"
msg_i.type=BT_K_3P;
msg_i.act_s.t_act=k_3r;
rez1=Send(pid_O, &msg_i, &msg_o, sizeof(msg_i),sizeof(msg_o));
printf(" K-3P ");

// �뤠�� ᨣ��� "K-4P"
msg_i.type=BT_K_4P;
msg_i.act_s.t_act=k_4r;
rez1=Send(pid_O, &msg_i, &msg_o, sizeof(msg_i),sizeof(msg_o));
printf(" K-4P ");

// �뤠�� ᨣ��� "K-5P"
msg_i.type=BT_K_5P;
msg_i.act_s.t_act=k_5r;
rez1=Send(pid_O, &msg_i, &msg_o, sizeof(msg_i),sizeof(msg_o));
printf(" K-5P \n");

sleep(2);

msg_i.type=GET_IN; 
rez1=Send(pid_O, &msg_i, &msg_o, sizeof(msg_i),sizeof(msg_o));
printf("��. ᨣ����=%x\n", msg_o.inp_r.u_inp.i_d);
printf("����-�=%x ��-�=%x ��-�=%x ��⮢=%x\n",msg_o.inp_r.u_inp.st_in.ISPR_K, 
msg_o.inp_r.u_inp.st_in.NI_K, msg_o.inp_r.u_inp.st_in.NI_R,
msg_o.inp_r.u_inp.st_in.SOI_RDy);

if ((((msg_o.inp_r.u_inp.i_d>>8)&0xff)==9)	&&	((msg_o.inp_r.u_inp.i_d>>16)==k_r)) {printf("����� ��ଠ\n");return(1);}
else {printf("����� �� ��ଠ\n");return(0);}

//return(msg_o.inp_r.u_inp.i_d);
}

//------------------------------------------------------------------------
int gloriya_read() {

pid_t pid_O=0;
int i, rez=0, err=0, rez1=0;

if (current_key==0) return(1);

//��।��塞 ����稥 �ࢥ� ࠧ���� ᨣ����� 
pid_O=qnx_name_locate(0,"MS_SOI_IO", 0, NULL);
if(pid_O==-1) {
	printf("�ࠩ��� ࠧ���� ᨣ����� �� ��⮢!\n");
	return(-1);
}

//ࠧ���� ���. ���� ���
msg_i.type=ED_B_SOI; // ���. ���� ����
msg_i.act_s.t_act=ON;
rez1=Send(pid_O, &msg_i, &msg_o, sizeof(msg_i),sizeof(msg_o));
//printf("����襭�� ���. ���� SOI = %d\n", rez1);
/*
//ࠧ���� ���. ���� ����
	    msg_i.type=ED_B_POST; // ���. ���� ����
//	    msg_i.type=ED_B_OUT; // ���. ����
	    msg_i.act_s.t_act=ON; 
	    rez1=Send(pid_O, &msg_i, &msg_o, sizeof(msg_i),sizeof(msg_o));
//printf("����襭�� ���. ���� ���� = %d\n", rez1);

// ��४����� �ନ஢���� ���. ᨣ����� � ���ਨ
	    msg_i.type=POST_SOI;
	    msg_i.act_s.t_act=ON;
	    rez1=Send(pid_O, &msg_i, &msg_o, sizeof(msg_i),sizeof(msg_o));
*/
//sleep(300);

msg_i.type=GET_IN; 
rez1=Send(pid_O, &msg_i, &msg_o, sizeof(msg_i),sizeof(msg_o));
printf("��. ᨣ����=%x\n", msg_o.inp_r.u_inp.i_d);
printf("����-�=%x ��-�=%x ��-�=%x ��⮢=%x\n",msg_o.inp_r.u_inp.st_in.ISPR_K, 
msg_o.inp_r.u_inp.st_in.NI_K, msg_o.inp_r.u_inp.st_in.NI_R,
msg_o.inp_r.u_inp.st_in.SOI_RDy);
 
if ((((msg_o.inp_r.u_inp.i_d>>8)&0xff)==9)	&&	((msg_o.inp_r.u_inp.i_d>>16)==current_key)) {printf("����� ��ଠ\n");return(1);}
else {printf("����� �� ��ଠ\n");return(0);}

//return(msg_o.inp_r.u_inp.i_d);
}
