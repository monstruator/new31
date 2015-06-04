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
	printf(" sr=%d",log_shot.GL_CP);			//	наличие сигнала "ср" на выходе Глории
//	printf(" p=%3.1f",log_shot.P_FACT*RAD);			//азимут, рад
//	printf(" b=%3.1f",log_shot.beta_FACT*RAD);		//угол места, рад
//	printurf(" b4=%3.1f",log_shot.beta*RAD);		//угол места антенны, рад

//	printf(" d_p=%3.1f ",(log_shot.P_FACT-log_shot.P_ANT)*RAD);			//азимут, рад
//	printf(" d_b=%3.1f ",(log_shot.beta_FACT-log_shot.beta)*RAD);		//угол места, рад
//	printf(" cr=%d ",log_shot.cr_SEANCE);		//порядковый номер массива данных цикла передачи 
//	printf(" sDCP=%d ",log_shot.sum_DCP);		//количество принятых слов с массиве дцп
	printf(" c_OI=%d ",log_shot.c_OI);		//количество принятых слов с массиве дцп
//	printf(" dpl=%d ",log_shot.DPL_1*244+(short)log_shot.Fd*1000);
//	printf(" dpl3=%d ",log_shot.DPL_1*244);
//	printf(" dpl4=%4.0f ",log_shot.Fd*1000);
	printf(" inf=%1.1e ",log_shot.SUM_20);
	printf(" %d ",log_shot.UR_sign_K2);		//уровень сигнала по каналу К2 (0-100) -табло сигнала К2


/*	log_shot.SUM_20=p->U.SUM_20;
	log_shot.DPL_1=p->U.DPL_1;
	log_shot.DPL_2=p->U.DPL_2;
	log_shot.c_OI=p->U.c_OI; //кол-во ОИ

	//v 4.1-----------------------------------------------
	log_shot.P_FACT=p->to41.P_FACT;			//азимут, рад
	log_shot.beta_FACT=p->to41.beta_FACT;		//угол места, рад

	log_shot.UR_sign_K1=p->to41.UR_sign_K1;		//уровень сигнала по каналу К1 (0-100) -табло сигнала К1
	log_shot.UR_sign_K2=p->to41.UR_sign_K2;		//уровень сигнала по каналу К2 (0-100) -табло сигнала К2
	log_shot.GL_priem=p->to41.GL_priem;		//наличие сигнала "прием" на выходе Глории
									//(1-есть сигнал, 0- нет сигнала)
									//(табло Глория-прием)	
	log_shot.GL_CP=p->to41.GL_CP;			//	наличие сигнала "ср" на выходе Глории
									//(1-есть сигнал, 0- нет сигнала)
									//(табло Глория-ср)	
	log_shot.cr_SEANCE=p->to41.cr_SEANCE;		//порядковый номер массива данных цикла передачи 
									//0-USHRT_MAX
	log_shot.sum_DCP=p->to41.sum_DCP;		//количество принятых слов с массиве дцп
	//v 4.2-----------------------------------------------
	log_shot.Mispr=p->to42.Mispr;		//Маска исправности аппаратуры пр 3.1
	//iz 4.1----------------------------------------------
	log_shot.cr_com=p->from41.cr_com;	//порядковый номер последней команды, выданной из пр.4.1 в пр.3.1 
							//0-USHRT_MAX
	log_shot.num_com=p->from41.num_com;//номер последней выданной команды
							//команда 1 - подготовка к сеансу связи
							//команда 2 - начало сеанса связи
							//команда 3 - завершение  сеанса связи

							//параметры команды 1	
	log_shot.num_KS=p->from41.num_KS;	//номер канала связи 1 - нр-к1, 2 - пр-к2 
	log_shot.Nkey_SHAKR=p->from41.Nkey_SHAKR;	// номер ключа ШАКР 
	log_shot.P_ANT_1=p->from41.P_ANT_1;	//азимут антенны (в момент начала сеанса), рад
	log_shot.beta_1=p->from41.beta_1;		//угол места антенны (в момент начала сеанса), рад
	log_shot.Fd_1=p->from41.Fd_1;		//сдвиг частоты (в момент начала сеанса), Кгц 

	log_shot.T_SS=p->from41.T_SS;		//время начала СС сек в формате СЕВ
								//(время первого включения КПИ после входа КА в круг связи)

								//положение антенны и сдвиг частоты на каждом такте
	log_shot.P_ANT=p->from41.P_ANT;		//азимут антенны, рад
	log_shot.beta=p->from41.beta;		//угол места антенны, рад
	log_shot.Fd=p->from41.Fd;			//сдвиг частоты, Кгц 
*/
	printf("\n");
}