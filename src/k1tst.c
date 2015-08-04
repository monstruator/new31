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


void main( int argc, char *argv[] )
{
	unsigned short cr_com,data_count=0;
	unsigned int TCount=0,DTTM=0; //общий таймер, последний приход данных
	int TC10=0; //таймер по 100 мс
	int N;//кол-во чтений УПОС
	int N1;//кол-во чтений УПОС на прошлом такте	
	int Mes,reset_u=0,test_count=0;
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

	create_shmem();
	delay(500);
	open_shmem();
	delay(300);

	ispr = (struct ispr_mo3k *) & p->to42.Mispr;

	//----------------------------------------------------
//	while((p->from41.num_com==0)&&(p->from42.num_com==0)) delay(500);
	cr_com=p->from41.cr_com; //запомнили номер команды

	
//	start_time=clock();
	if ((p->from41.num_com==1)||(p->from41.num_com==2)||(p->from41.num_com==3)) 	
	{
		Init_K1(p->from41.num_KS-1);
	} //если есть команда от 4.1
	else Init_K1(0); //если есть команда от 4.2 в К2

//	end_time=clock();
//	printf("time=%d %d \n",end_time-start_time,CLOCKS_PER_SEC);

//	start_time=clock();
//	nastr_upos(0);
//	end_time=clock();
//	printf("time1=%d %d \n",end_time-start_time,CLOCKS_PER_SEC);

	Dopler1=60000;
	writeDopler(-Dopler1);
	
	//Init_K1(0);	
	
	printf("\nУстановка нулевых порогов\n");
	writePorogs(porog_sf, porog_df);

/*	while(f <= 480) {
		test_count++;
		Write_K1(SUM4);
		Write_K1(DPL1);
		delay(200);
		pid=Receive( 0 , 0, 0 );				
		if (pid==proxy_DRV1) DDRead_K1();
		pid=Receive( 0 , 0, 0 );				
		if (pid==proxy_DRV1) DDRead_K1();
		//if (test_count>5)
		{
			Dopler1-=250;
			test_count=0;
			//if (Dopler1<-10000) Dopler1=4000;
			writeDopler(-Dopler1);
		}
		printf(" DPL=%d \n",p->U.DPL_1*244);
	}
*/

	printf("Расчет среднего значения уровня суммарного канала ... \n");
	while(f <= 8) {
		Write_K1(SUM4);
		Write_K1(RAZN0);
		delay(50);
		pid=Receive( 0 , 0, 0 );				
		if (pid==proxy_DRV1) DDRead_K1();
		pid=Receive( 0 , 0, 0 );				
		if (pid==proxy_DRV1) DDRead_K1();
		Dopler1-=15000;
		writeDopler(-Dopler1);
	}
	SREDN = floatSUM_4/f;
	printf("измерен уровень шума %3.3e \n",SREDN);			
	//writePorogs(0, SREDN*2);
	writePorogs(0, SREDN*2);

	Dopler1=(float)p->from41.Fd*1000;
	//printf("d_from41=%e\n",p->from41.Fd);
	//writeDopler(-Dopler1);
	writeDopler(4750);

	//	Ubran test Glorii do uto4neni9 algoritma
	//	ispr->k1=TestGloriya(0);

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
    
	timer.it_value.tv_sec     = 2L; //start after X sec
    timer.it_value.tv_nsec    = 0L;
    timer.it_interval.tv_sec  = 0;
    timer.it_interval.tv_nsec = 100*MS;  //100
    timer_settime( id, 0, &timer, NULL );

    while(1) 
	{
    	pid=Receive( 0 , 0, 0 );
		if (pid==proxy) //обработчик таймера
		{
			TCount++;
			TC10++;
			if (TC10==10) TC10=0;
			if (p->U.c_OI!=data_count)//пришли данные
			{
				DTTM=TCount;			//запомнили время прихода
				data_count=p->U.c_OI;   //запомним кол-во
			}

			//if (((TCount-DTTM>40)||(data_count>1249))&&(data_count>0)) //тайм-аут данных при их наличии
			if ((TCount-DTTM>40)&&(data_count>0)) //тайм-аут данных при их наличии
			{
				if ((p->from41.num_com==2)&&(p->from41.num_KS==1))//если это сеансК1
				{
					if(data_count>1250) data_count=1250;
					for(i=0;i<data_count;i++) p->to41.DCP_K1[i]=p->U.OI[i];//копируем в вых буфер
					p->to41.sum_DCP=data_count;		//передаем кол-во данных
					p->to41.cr_SEANCE++; //увеличили порядковый номер массива
					printf("Сформирован массив К1 N%d. Кол-во данных %d слов\n",p->to41.cr_SEANCE,p->to41.sum_DCP);
					//reset_upos();
					//nastr_upos(p->from41.num_KS-1);

				}
				data_count=p->U.c_OI=0;  //обнуление буфера УПОС
			}

			switch (TC10)
			{
				case 0 : Write_K1(SUM4); break;
				//case 1 : Write_K1(ZI); break;
				//case 2 : Write_K1(SUM20); break;
				case 3 : Write_K1(YP); break;
				case 4 : Write_K1(DPL1); break;						
				case 5 : Write_K1(RAZN0); break;						
				case 6 : Write_K1(RAZN1); break;						
				case 9 : //раз в пол сек выполняем сервисные операции
						test_count++;
						if (test_count>5)
						{
							Dopler1-=250;
							test_count=0;
							if (Dopler1<-10000) Dopler1=4000;
							writeDopler(-Dopler1);
						}


						test_dpl=p->from41.Fd-2;
						if (p->U.SUM_4>1e+8) p->U.SUM_20=(log10(p->U.SUM_4)-8)*16;	else p->U.SUM_20=0;
						//if (abs(p->from41.Fd*1000-Dopler1) > 2000) 
						
/*						if (abs(test_dpl*1000-Dopler1) > 2000) 
						{
						//	Dopler1=(float)p->from41.Fd*1000;
						//	writeDopler(-Dopler1);
							Dopler1=(float)test_dpl*1000;
							writeDopler(-Dopler1);
						}
*/
						//printf("lvl = %f data=%d\n",p->U.SUM_20,data_count);
						//printf("n_com_from_k1 = %x \n",p->from41.num_com);
						//printf("SUM_4=%3.3e  	LVL4=%d 	DPL=%d hz\n",p->U.SUM_4,(short)p->U.SUM_20,p->U.DPL_1*244);
					
						//printf("OI=%x c_OI=%x\n",p->U.OI,p->U.c_OI);				
						//printf("ZI_DATA=%x	 ZI_DOST=%x\n",p->U.ZI_DATA,p->U.ZI_DOST);

						if((m_porog[0]==p->U.SUM_4)&&(reset_u==0)) i2++;else i2=0;
						m_porog[0]=p->U.SUM_4;
						if (i2>10) 
						{
							//reset_u=1;
							//reset_upos();
							Init_K1(p->from41.num_KS-1);
							//reset_u=0;
							//p->U.SUM_4=1e8;
							//nastr_upos(p->from41.num_KS-1);
							i2=0;

						} 
						//if(m_porog[0]==p->U.SUM_4) {p->U.SUM_20=0;} 
						//m_porog[0]=p->U.SUM_4;

	
						break;
			}

			//if (N==N1) printf("нет чтения\n");
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
//----------------------------чтение К1-----------------------------
		if (pid==proxy_DRV1) {Read_K1();N++;}
    }

    timer_delete( id );
  }