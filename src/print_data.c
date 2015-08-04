#include <stdio.h>
#include <signal.h>
#include <time.h>
#include <sys/proxy.h>
#include <sys/kernel.h>
#include <stdlib.h>
#include "../include/shared_mem.h"
#include "../include/funcu.h"
#define RAD 57.2957	

void read_shot();

	LOG_MEM log_shot;

  unsigned int *longbuf, *longbuf2, lbuf[8];
  unsigned int data_in[1250];
  char buf[4];
  unsigned long ttt, t1,t2;
  int j,i_MV,i_FSHS, i_MNU, i2, jmp, tmp, size_malloc;  
  int all_i_MV,all_i_FSHS, all_i_MNU;  
  int gw=0, count_gw=0; // начилие информации и счетчик (good word)
  int all_count_gw=0, all_data=0, all_cicle=0; //
  
  MV_t MV;
  FORM_FShS_K2_t FSHS;
  int old_c_OI=0;

void main(int argc, char **argv)
  {
	FILE *out_fp = NULL;
	FILE *out_fp1 = NULL;
	FILE *out_fp2 = NULL;
	FILE *fin, *fout;

    timer_t id;
	pid_t proxy;
	unsigned int i,i2=0;
	
	//char j[2]={2,1};
    struct itimerspec timer;
    struct sigevent event;
	short cr_SEANCE=0;	
	char itog_f[30];
	char log_f[30] = "//1/log/logs_000000_0000";
	char data_f[30] ="//1/log/data_000000_0000";
	int old_DCP=0;
	int c;

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


/*	if(argc == 2)
	{
    	fin = fopen(argv[1], "r");
        sscanf( optarg, "%s", itog_f);
		for (i=0;i<30;i++) printf("%c", itog_f[i]);printf("\n");

	}

  if(fin == NULL){
	printf("file not exist\n\n");
    return;
  }
*/

   while( ( c = getopt( argc, argv, "A:B:" ) ) != -1 ) 
    switch( c ) 
	{
      case 'A':
         sscanf( optarg, "%s", itog_f);
  		for (i=0;i<16;i++) printf("%c", itog_f[i]);printf("\n");

         break;
	}

//	if ( ( out_fp = fopen( "//1/log/logs", "r" ) ) == NULL ) 
	if ( ( out_fp = fopen( itog_f, "r" ) ) == NULL ) 
			{fprintf( stderr, "log.Couldn't create/open file %s.\n", strerror( errno ) );return;}

	itog_f[0]='d';
	itog_f[1]='a';
	itog_f[2]='t';
	itog_f[3]='a';

	for (i=0;i<16;i++) printf("%c", itog_f[i]);printf("\n\n");

//	if ( ( out_fp1 = fopen( "//1/log/data", "r" ) ) == NULL ) 
	if ( ( out_fp1 = fopen( itog_f, "r" ) ) == NULL ) 
			{fprintf( stderr, "data.Couldn't create/open file %s.\n", strerror( errno ) );return;}

	itog_f[0]='i';
	itog_f[1]='t';
	itog_f[2]='o';
	itog_f[3]='g';

	if ( ( out_fp2 = fopen( itog_f, "w" ) ) == NULL ) 
			{fprintf( stderr, "data.Couldn't create/open file %s.\n", strerror( errno ) );return;}

	for (i=5;i<16;i++) fprintf(out_fp2,"%c", itog_f[i]);fprintf(out_fp2,"\n\n");


		while(fread(&log_shot, sizeof(log_shot),1,out_fp))
		{
			//read_shot();
			if ((old_c_OI==0)&&(log_shot.c_OI!=0)) //если пришли первые данные в цикле
			{
				//printf("%02x:%02x:%02x - ", log_shot.CEB[2]>>8,log_shot.CEB[3]>>8,log_shot.CEB[3]&0x00ff);
				fprintf(out_fp2,"%02x:%02x:%02x - ", log_shot.CEB[2]>>8,log_shot.CEB[3]>>8,log_shot.CEB[3]&0x00ff);
			}
			
			if ((old_c_OI!=0)&&(log_shot.c_OI==0)) //если пришли последние данные в цикле
			//if (old_DCP!=log_shot.cr_SEANCE) //пришел новый массив
			{
				fprintf(out_fp2,"%02x:%02x:%02x пришло %d слов \n", log_shot.CEB[2]>>8,log_shot.CEB[3]>>8,log_shot.CEB[3]&0x00ff, log_shot.sum_DCP);
				all_cicle++;
				old_DCP=log_shot.cr_SEANCE;
				//printf(" s_DCP=%d \n", log_shot.sum_DCP);
			    fread(data_in, log_shot.sum_DCP*4 , 1, out_fp1);
				
				for( i2=0;i2<log_shot.sum_DCP;i2++ ) // переворот битов в словах
				{
				    ttt = 0;
				    t1 = 0x1<<31;
				    for(j = 32; j > 0; j--)
					{
					     ttt = ttt | (((data_in[i2]&t1)>>(j-1))  <<  (32 - j));
					     t1=t1>>1;
				    }
				    ttt = ttt >> 12;
				    data_in[i2] = ttt;
				}
				
				for( i2=0;i2<log_shot.sum_DCP;i2++ ) // поиск слов
				{

				    if(((data_in[i2]& 0xF0000) == 0xA0000)  && (data_in[i2+2] == 0x00000))
				    {
						if(!CSumK1(&data_in[i2], 4))
						{
							i_MV++;
        					//fprintf(out_fp,"FORM_MV!   %d \n", i_MV);
        					//gt_MV(&MV, &data_in[i2]);
							jmp = i2 + 3;
							gw=1;
      					}
	  					//else printf("FORM_MV error chksum!  \n");
					}

				    if( ((data_in[i2]&0xF0000) == 0x90000))
					{
				    	if(!CSumK1(&data_in[i2], 8))
						{
							i_FSHS++;
        					//fprintf(out_fp,"FORM_FSHS!   %d\n", i_FSHS);
        					//gt_FSHS(&FSHS, longbuf2);
							jmp = i2 +7;
							gw=1;
      					}	  //else printf("FORM_FSHS error chksum!  \n");
					}

    				if( ((data_in[i2]&0xF0000) == 0xD0000))
					{
      					if(!CSumK1(&data_in[i2], 18))
						{
							i_MNU++;
        					//printf("FORM_MNU!   %d\n", i_MNU);
        					//gt_FSHS(&FSHS, longbuf2);
							jmp = i2 +17;
							gw=1;
      					}  //else printf("FORM_MNU error chksum!  \n");

					}

					if (gw==1) count_gw++; 
   	 				//fprintf(out_fp2,"%06d  %05X %d\n", i2, data_in[i2] , count_gw);

					if ( i2 == jmp) {gw=0;}
				}
				//печатаем итег по циклу
				fprintf(out_fp2,"МВ %d   МНУ %d   ФШС %d  Плохих слов %d ( %d процента )\n\n", i_MV, i_MNU, i_FSHS, i2-count_gw, (i2-count_gw)*100 / i2);
		    	all_count_gw+=count_gw; all_data+=log_shot.sum_DCP; //  информации счетчик 
				all_i_MV+=i_MV;
				all_i_MNU+=i_MNU;
				all_i_FSHS+=i_FSHS;
			
				i_MV = i_MNU = i_FSHS = gw = count_gw = 0;
				
			}
			old_c_OI=log_shot.c_OI;

		}

			//printf("\n\n ИТОГО :   слов - %d , плохих - %d , %d процентов\n", all_data, all_count_gw, all_count_gw*100/all_data);
			fprintf(out_fp2,"\n\n ИТОГО :   слов - %d , плохих - %d ( %d процента )\n", all_data, all_data-all_count_gw, (all_data-all_count_gw)*100/all_data);
			fprintf(out_fp2," ЦИКЛОВ - %d , МВ - %d, МНУ - %d, ФШС - %d\n",all_cicle,all_i_MV, all_i_MNU, all_i_FSHS);

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
			
		printf("cr_seance=%d ",p->to41.cr_SEANCE);
//    	printf(" size=%d\n",sizeof(obmen_31_41_t)); ///!!!!!!

		if ( out_fp != NULL ) 
		{
			//fwrite( &p->to41.DCP_K1[i], sizeof(LOG_MEM), 1, out_fp_log );
		}
		
	
		if (kbhit()) break;
    }
*/
	fclose( out_fp );
	return;
  }


void read_shot()
{
	int i=0;

//	for(i=0;i<6;i++) printf("%x ",log_shot.CEB[i]);
	printf("%02x:%02x:%02x ", log_shot.CEB[2]>>8,log_shot.CEB[3]>>8,log_shot.CEB[3]&0x00ff);
	// UPOS-----------------------------------------------
	printf(" s=%d ",log_shot.UR_sign_K1);
	//printf(" s1=%1.2e ",log_shot.SUM_20);
	//printf(" s2=%1.2e ",log_shot.SUM_4);

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
	printf(" cr=%d ",log_shot.cr_SEANCE);		//порядковый номер массива данных цикла передачи 
	printf(" sDCP=%d ",log_shot.sum_DCP);		//количество принятых слов с массиве дцп
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