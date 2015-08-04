#include <stdio.h>
#include <signal.h>
#include <time.h>
#include <sys/proxy.h>
#include <sys/kernel.h>
#include <stdlib.h>
#include "../include/K1.h"

#include <math.h>
#define MS 1000000

 struct ispr_mo3k *ispr;

	LOG_MEM log_shot;
 	FILE *out_fp_data = NULL;
	FILE *out_fp_log = NULL;

void read_shot();
int num_data=0;


void main( int argc, char *argv[] )
{
 	char log_f[30] = "//1/log/log";
	char data_f[30] ="//1/log/data";

	unsigned short cr_com,data_count=0;
	unsigned int TCount=0,DTTM=0; //��騩 ⠩���, ��᫥���� ��室 ������
	int TC10=0; //⠩��� �� 100 ��
	int N;//���-�� �⥭�� ����
	int N1;//���-�� �⥭�� ���� �� ��諮� ⠪�	
	int Mes,reset_u=0;
    pid_t proxy,pid;
    timer_t id;
	unsigned int i,i2=0;
    struct itimerspec timer;
    struct sigevent event;
	short i_p=0;
	float Dopler1,m_porog[2],test_dpl;
	time_t start_time,end_time;

	while( (i=getopt(argc, argv, "mis:") )!=-1)	{
		switch(i){
			case 'p' :  break;
			case 's' :	break;//sscanf(optarg,"%f",&porog_sf); printf("Yaa Zdes!!\n"); break;
			case 'i' :	break;//sscanf(optarg,"%f",&porog_df); break;
		}//switch
	}//while
	
	porog_sf = atof(argv[2]);
	porog_df = atof(argv[3]);

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

	if ( ( out_fp_data = fopen( data_f, "r" ) ) == NULL ) 
			fprintf( stderr, "Couldn't create/open file %s.\n", strerror( errno ) );

	if ( ( out_fp_log = fopen( log_f, "r" ) ) == NULL ) 
			fprintf( stderr, "Couldn't create/open file %s.\n", strerror( errno ) );


	//create_shmem();
	delay(500);
	open_shmem();
	delay(500);

	ispr = (struct ispr_mo3k *) & p->to42.Mispr;

	//----------------------------------------------------
//	while((p->from41.num_com==0)&&(p->from42.num_com==0)) delay(500);
	cr_com=p->from41.cr_com; //��������� ����� �������

	
//	start_time=clock();
	if ((p->from41.num_com==1)||(p->from41.num_com==2)||(p->from41.num_com==3)) 	
	{
		Init_K1(p->from41.num_KS-1);
	} //�᫨ ���� ������� �� 4.1
	else Init_K1(1); //�᫨ ���� ������� �� 4.2 � �2

	Dopler1=60000;
	writeDopler(-Dopler1);
	
	printf("\n��⠭���� �㫥��� ��ண��\n");
	writePorogs(porog_sf, porog_df);

	printf("����� �।���� ���祭�� �஢�� �㬬�୮�� ������ ... \n");
	while(f <= 8) {
		Write_K1(SUM4);
		Write_K1(RAZN0);
		delay(200);
		pid=Receive( 0 , 0, 0 );				
		if (pid==proxy_DRV1) DDRead_K1();
		pid=Receive( 0 , 0, 0 );				
		if (pid==proxy_DRV1) DDRead_K1();
		Dopler1-=15000;
		writeDopler(-Dopler1);
	}
	SREDN = floatSUM_4/f;
	printf("����७ �஢��� �㬠 %3.3e \n",SREDN);			
	//writePorogs(0, SREDN*2);
	writePorogs(1e2, SREDN*2);

	Dopler1=(float)p->from41.Fd*1000;
	//printf("d_from41=%e\n",p->from41.Fd);
	writeDopler(-Dopler1);
	//test_dpl=Dopler1;
	//	Ubran test Glorii do uto4neni9 algoritma
	//	ispr->k1=TestGloriya(0);
    
	timer.it_value.tv_sec     = 2L; //start after X sec
    timer.it_value.tv_nsec    = 0L;
    timer.it_interval.tv_sec  = 0;
    timer.it_interval.tv_nsec = 100*MS;  //100
    timer_settime( id, 0, &timer, NULL );

    while(1) 
	{
    	pid=Receive( 0 , 0, 0 );
		if (pid==proxy) //��ࠡ��稪 ⠩���
		{
			TC10++;
			if (TC10==10) TC10=0;


			if ( (p->from41.num_com==2) && (TC10==1) ) 
			{
				fread(&log_shot, sizeof(log_shot),1,out_fp_log);
				printf("%02x:%02x:%02x \n", log_shot.CEB[2]>>8,log_shot.CEB[3]>>8,log_shot.CEB[3]&0x00ff);

				p->to41.GL_priem=log_shot.GL_priem;	
				p->to41.GL_CP=log_shot.GL_CP;			//	����稥 ᨣ���� "��" �� ��室� ���ਨ
				p->to41.UR_sign_K1=log_shot.UR_sign_K1;
				p->U.DPL_1=log_shot.DPL_1;


				if (num_data!=log_shot.cr_SEANCE)
				{
					//fread(&p->to41.DCP_K1, log_shot.sum_DCP*4,1,out_fp_data);
					//for(i=0;i<log_shot.sum_DCP;i++) 	
					fread(&p->to41.DCP_K1[0], 4,log_shot.sum_DCP,out_fp_data);
	
					p->to41.cr_SEANCE=log_shot.cr_SEANCE;
					p->to41.sum_DCP=log_shot.sum_DCP;


					for(i=0;i<10;i++) printf(" %08x",p->to41.DCP_K1[i]);printf("\n");
					num_data=log_shot.cr_SEANCE;
					printf(" cr=%d data=%d \n", p->to41.cr_SEANCE, p->to41.sum_DCP);
					
				}
				//printf("%d \n",log_shot.);
				//read_shot();
			}

			switch (TC10)
			{
				//case 0 : Write_K1(SUM4); break;
				//case 1 : Write_K1(ZI); break;
				//case 2 : Write_K1(SUM20); break;
				//case 3 : Write_K1(YP); break;
				//case 4 : Write_K1(DPL1); break;						
				case 5 : Write_K1(RAZN0); break;						
				case 6 : Write_K1(RAZN1); break;						
				case 9 : //ࠧ � ��� ᥪ �믮��塞 �ࢨ�� ����樨
						test_dpl=(p->from41.Fd-2)*1000; //���४�஢�� �訡�� ��।������ ������ � �����
						//if (p->U.SUM_4>1e+8) p->to41.UR_sign_K1=(short)((log10(p->U.SUM_4)-8)*16);	else p->to41.UR_sign_K1=0;
						//if (abs(p->from41.Fd*1000-Dopler1) > 2000) 
						//	if ((abs(p->from41.Fd<4.5))&&(abs(p->from41.Fd)>5.5))
						if (abs(test_dpl-Dopler1) > 2000) 
							//if ((abs(test_dpl*1000<4500))||(abs(test_dpl*1000)>5500)) //���� �� ����ᠭ�� ������
							if ((test_dpl<-5500)||(test_dpl>5500)||((test_dpl>-4500)&&(test_dpl<4500))) //���� �� ����ᠭ�� ������
							{
								//Dopler1=(float)p->from41.Fd*1000;
								Dopler1=test_dpl;
								//printf("-- %5.0f -- ",test_dpl);
								writeDopler(-Dopler1);
							}
						//printf("lvl = %f data=%d\n",p->U.SUM_20,data_count);
						//printf("n_com_from_k1 = %x \n",p->from41.num_com);
						//printf("SUM_4=%3.3e LVL20=%1.1e DPL=%d hz\n",p->U.SUM_4,p->U.SUM_20,p->U.DPL_1*244);
					
						//printf("OI=%x c_OI=%x\n",p->U.OI,p->U.c_OI);				
						//printf("ZI_DATA=%x	 ZI_DOST=%x\n",p->U.ZI_DATA,p->U.ZI_DOST);

						//if((m_porog[0]==p->U.SUM_4)&&(reset_u==0)) i2++;else i2=0;
						//m_porog[0]=p->U.SUM_4;
						//if(m_porog[0]==p->U.SUM_4) {p->U.SUM_20=0;} 
						//m_porog[0]=p->U.SUM_4;
						break;
			}

			//if (N==N1) printf("��� �⥭��\n");
			N1=N;
			if ((cr_com!=p->from41.cr_com)&&(p->from41.num_com==1))	Init_K1(p->from41.num_KS-1);
			//if ((cr_com!=p->from41.cr_com)&&(p->from41.num_com==77))	writePorogs(SREDN,SREDN);
			/*if ((cr_com!=p->from42.cr_com)&&(p->from42.num_com==6))	
			{
				Dopler1=-p->from42.Fd*1000;
				writeDopler(Dopler1);
			}
			*/
			cr_com=p->from41.cr_com;
		}
//----------------------------�⥭�� �1-----------------------------
		if (pid==proxy_DRV1) {Read_K1();N++;}
    }

    timer_delete( id );
  }


void read_shot()
{
	int i=0;

//	for(i=0;i<6;i++) printf("%x ",log_shot.CEB[i]);
	printf("%02x:%02x:%02x ", log_shot.CEB[2]>>8,log_shot.CEB[3]>>8,log_shot.CEB[3]&0x00ff);
	// UPOS-----------------------------------------------
	printf(" s=%d ",log_shot.UR_sign_K1);
	printf(" s1=%1.2e ",log_shot.SUM_20);
	printf(" s2=%1.2e ",log_shot.SUM_4);

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
	printf(" cr=%d ",log_shot.cr_SEANCE);		//���浪��� ����� ���ᨢ� ������ 横�� ��।�� 
	printf(" sDCP=%d ",log_shot.sum_DCP);		//������⢮ �ਭ���� ᫮� � ���ᨢ� ��
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