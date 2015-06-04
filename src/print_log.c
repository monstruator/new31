#include <stdio.h>
#include <signal.h>
#include <time.h>
#include <sys/proxy.h>
#include <sys/kernel.h>
#include <stdlib.h>
#include "../include/shared_mem.h"
#define RAD 57.2957	

void read_shot();

	LOG_MEM log_shot;

void main( void )
  {
	FILE *out_fp = NULL;
    timer_t id;
	pid_t proxy;
	unsigned int i,i2=0;
	char j[2]={2,1};
    struct itimerspec timer;
    struct sigevent event;
	short cr_SEANCE=0;	
	char log_f[30] = "//1/log/logs_000000_0000";
	char data_f[30] ="//1/log/logs";

    /* Get a proxy for the timer to kick */
    proxy = qnx_proxy_attach( 0, 0, 0, -1 );
    if( proxy == -1 ) {
      printf( "Unable to attach proxy." );
      return;
    }
	
    /* Attach to the timer */
    event.sigev_signo = -proxy;
    id = timer_create( CLOCK_REALTIME, &event );
    if( id == -1 ) {
      printf( "Unable to attach timer." );
      return;
    }

    timer.it_value.tv_sec     = 1L; //start after X sec
    timer.it_value.tv_nsec    = 0L;
    timer.it_interval.tv_sec  = 0;
    timer.it_interval.tv_nsec = 1000000000;
    timer_settime( id, 0, &timer, NULL );


	if ( ( out_fp = fopen( "//1/log/logs", "r" ) ) == NULL ) 
			fprintf( stderr, "Couldn't create/open file %s.\n", strerror( errno ) );

	if ( out_fp != NULL ) 
		while(fread(&log_shot, sizeof(log_shot),1,out_fp))
		{
			//printf("%d \n",log_shot.UR_sign_K1);
			read_shot();
	
		}


/*
    while(1) 
	{
    	Receive( proxy, 0, 0 );

		//printf("lvl = %d data=%d Nmas=%d\n",p->to41.UR_sign_K1,p->U.c_OI,p->to41.cr_SEANCE);
		//printf("%d\n",p->from41.num_KS); 

//		if (p->from41.num_KS==1) 
		{	
			printf("SUM_4=%1.1e ",p->U.SUM_4);
//			printf("ANT_1=%f          ANT_2=%f\n",p->from41.P_ANT_1,p->from41.P_ANT_2);
//			printf("    Fd=%2.1f  ",p->from41.Fd);

//			printf("SUM_4=%05e  SUM_20=%05e \nDPL_1=%x DPL_2=%x OI=%x c_OI=%d\n",
//			//!p->U.RAZN_0,p->U.RAZN_1,
//			p->U.SUM_4,p->U.SUM_20,
//			//p->U.DPL_1,p->U.DPL_2,
//			p->U.OI[p->U.c_OI],p->U.c_OI);
//			printf("    R_0=%3.3f     R_1=%3.3f \n",p->U.RAZN_0,p->U.RAZN_1);


//			printf("ZI_DATA=%x ZI_DOST=%x\n",p->U.ZI_DATA,p->U.ZI_DOST);
//			printf("\n");
		}
		if (p->from41.num_com==2) printf("%3d  ",i2++); else printf("  0  ");
		//	printf("V=%f A=%f D=%f T=%d \n",p->from41.Vr,p->from41.Ar,p->from41.D,p->from41.T_SS);
//-------------------------------------------------------------------------------------
		printf("to41=%2.2f fr41=%2.2f fact=%2.2f Pr1=%d ",p->to41.P_FACT,p->from41.P_ANT,p->to41.P_FACT-p->to41.simfonia41.Kg,p->Dout41[16]);
		printf(" LVL4=%d  DPL=%d\n",(short)p->U.SUM_20,p->U.DPL_1*244);

//			printf("H=%d M=%d S=%d T41=%d T31=%d\n",p->Dout41[30],p->Dout41[31],p->Dout41[32],p->from41.T_SS,p->Dout41[30]*3600+p->Dout41[31]*60+p->Dout41[32]);
//			printf("  D=%d \n",p->from41.T_SS-10-(p->Dout41[30]*3600+p->Dout41[31]*60+p->Dout41[32]));
			
//		printf("cr_seance=%d\n",p->to41.cr_SEANCE);
//    	printf(" size=%d\n",sizeof(obmen_31_41_t)); ///!!!!!!

		if ( out_fp != NULL ) 
		{
			//fwrite( &p->to41.DCP_K1[i], sizeof(LOG_MEM), 1, out_fp_log );
		}
		
	
		if (kbhit()) break;
    }
*/
	fclose( out_fp );
  }


void read_shot()
{
	int i=0;

//	for(i=0;i<6;i++) printf("%x ",log_shot.CEB[i]);
	printf("%02x:%02x:%02x ", log_shot.CEB[2]>>8,log_shot.CEB[3]>>8,log_shot.CEB[3]&0x00ff);
	// UPOS-----------------------------------------------
	printf(" s=%d ",log_shot.UR_sign_K1);
/*	if (log_shot.RAZN_0>=0) printf(" r0=+");
	else 				   printf(" r0=");
	printf("%1.3f ",log_shot.RAZN_0);
	if (log_shot.RAZN_1>=0) printf(" r1=+");
	else 				   printf(" r1=");
	printf("%1.3f ",log_shot.RAZN_1);
*/
	printf(" pr=%d",log_shot.GL_priem);	
	printf(" sr=%d",log_shot.GL_CP);			//	����稥 ᨣ���� "��" �� ��室� ���ਨ
//	printf(" p=%3.1f",log_shot.P_FACT*RAD);			//������, ࠤ
//	printf(" b=%3.1f",log_shot.beta_FACT*RAD);		//㣮� ����, ࠤ
//	printurf(" b4=%3.1f",log_shot.beta*RAD);		//㣮� ���� ��⥭��, ࠤ

//	printf(" d_p=%3.1f ",(log_shot.P_FACT-log_shot.P_ANT)*RAD);			//������, ࠤ
//	printf(" d_b=%3.1f ",(log_shot.beta_FACT-log_shot.beta)*RAD);		//㣮� ����, ࠤ
//	printf(" cr=%d ",log_shot.cr_SEANCE);		//���浪��� ����� ���ᨢ� ������ 横�� ��।�� 
//	printf(" sDCP=%d ",log_shot.sum_DCP);		//������⢮ �ਭ���� ᫮� � ���ᨢ� ��
	printf(" c_OI=%d ",log_shot.c_OI);		//������⢮ �ਭ���� ᫮� � ���ᨢ� ��
//	printf(" dpl=%d ",log_shot.DPL_1*244+(short)log_shot.Fd*1000);
//	printf(" dpl3=%d ",log_shot.DPL_1*244);
//	printf(" dpl4=%4.0f ",log_shot.Fd*1000);
	printf(" inf=%1.1e ",log_shot.SUM_20);
	printf(" %d ",log_shot.UR_sign_K2);		//�஢��� ᨣ���� �� ������ �2 (0-100) -⠡�� ᨣ���� �2


/*	log_shot.SUM_20=p->U.SUM_20;
	log_shot.DPL_1=p->U.DPL_1;
	log_shot.DPL_2=p->U.DPL_2;
	log_shot.c_OI=p->U.c_OI; //���-�� ��

	//v 4.1-----------------------------------------------
	log_shot.P_FACT=p->to41.P_FACT;			//������, ࠤ
	log_shot.beta_FACT=p->to41.beta_FACT;		//㣮� ����, ࠤ

	log_shot.UR_sign_K1=p->to41.UR_sign_K1;		//�஢��� ᨣ���� �� ������ �1 (0-100) -⠡�� ᨣ���� �1
	log_shot.UR_sign_K2=p->to41.UR_sign_K2;		//�஢��� ᨣ���� �� ������ �2 (0-100) -⠡�� ᨣ���� �2
	log_shot.GL_priem=p->to41.GL_priem;		//����稥 ᨣ���� "�ਥ�" �� ��室� ���ਨ
									//(1-���� ᨣ���, 0- ��� ᨣ����)
									//(⠡�� �����-�ਥ�)	
	log_shot.GL_CP=p->to41.GL_CP;			//	����稥 ᨣ���� "��" �� ��室� ���ਨ
									//(1-���� ᨣ���, 0- ��� ᨣ����)
									//(⠡�� �����-��)	
	log_shot.cr_SEANCE=p->to41.cr_SEANCE;		//���浪��� ����� ���ᨢ� ������ 横�� ��।�� 
									//0-USHRT_MAX
	log_shot.sum_DCP=p->to41.sum_DCP;		//������⢮ �ਭ���� ᫮� � ���ᨢ� ��
	//v 4.2-----------------------------------------------
	log_shot.Mispr=p->to42.Mispr;		//��᪠ ��ࠢ���� ��������� �� 3.1
	//iz 4.1----------------------------------------------
	log_shot.cr_com=p->from41.cr_com;	//���浪��� ����� ��᫥���� �������, �뤠���� �� ��.4.1 � ��.3.1 
							//0-USHRT_MAX
	log_shot.num_com=p->from41.num_com;//����� ��᫥���� �뤠���� �������
							//������� 1 - �����⮢�� � ᥠ��� �裡
							//������� 2 - ��砫� ᥠ�� �裡
							//������� 3 - �����襭��  ᥠ�� �裡

							//��ࠬ���� ������� 1	
	log_shot.num_KS=p->from41.num_KS;	//����� ������ �裡 1 - ��-�1, 2 - ��-�2 
	log_shot.Nkey_SHAKR=p->from41.Nkey_SHAKR;	// ����� ���� ���� 
	log_shot.P_ANT_1=p->from41.P_ANT_1;	//������ ��⥭�� (� ������ ��砫� ᥠ��), ࠤ
	log_shot.beta_1=p->from41.beta_1;		//㣮� ���� ��⥭�� (� ������ ��砫� ᥠ��), ࠤ
	log_shot.Fd_1=p->from41.Fd_1;		//ᤢ�� ����� (� ������ ��砫� ᥠ��), ��� 

	log_shot.T_SS=p->from41.T_SS;		//�६� ��砫� �� ᥪ � �ଠ� ���
								//(�६� ��ࢮ�� ����祭�� ��� ��᫥ �室� �� � ��� �裡)

								//��������� ��⥭�� � ᤢ�� ����� �� ������ ⠪�
	log_shot.P_ANT=p->from41.P_ANT;		//������ ��⥭��, ࠤ
	log_shot.beta=p->from41.beta;		//㣮� ���� ��⥭��, ࠤ
	log_shot.Fd=p->from41.Fd;			//ᤢ�� �����, ��� 
*/
	printf("\n");
}