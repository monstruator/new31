#include <conio.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/name.h>
#include <sys/kernel.h>
#include <time.h>
#include <sys/osinfo.h>
#include <process.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/dev.h>
#include <sys/proxy.h>
#include <signal.h>
#include "../include/shared_mem.h"
#include "../include/my_pcs.h"
#define m_sec 1000000
#define Tmax 86400 //кол-во сек в сутках

int COR_T=0; //correction time
int TM=0; //test time mode
int TS=0; //test signal mode
 
int rez=0;
pid_t proxy_DRV2=0; 
chan2=3;

char test_K2[40][15]={{0x55, 0x04, 0x01, 0x0B, 0x65},		// ->+ 0
					{0x55, 0x04, 0x03, 0x00, 0x5C},			// <- (->) квитанция
					{0x55, 0x05, 0x02, 0x03, 0x00, 0x5F},	// <-2
					{0x55, 0x04, 0x01, 0x0C, 0x66},			// ->+3	
					{0x55, 0x05, 0x02, 0x06, 0x00, 0x62},	// <-4
					{0x55, 0x04, 0x01, 0x0D, 0x67},			// ->+5
					{0x55, 0x05, 0x02, 0x18, 0x00, 0x74},	// <-6
					{0x55, 0x04, 0x01, 0x01, 0x5B},         //7
					{0x55, 0x05, 0x02, 0x01, 0x00, 0x5D},   //8
					{0x55, 0x06, 0x01, 0x0A, 0x28, 0x01, 0x8F},//КанЛ 9
					{0x55, 0x04, 0x01, 0x17, 0x71},         //10
					{0x55, 0x07, 0x02, 0x14, 0x00, 0x15, 0x7E, 0x05},
					{0x55, 0x05, 0x01, 0x09, 0x01, 0x65},   //12 ДкР
					//{0x55, 0x05, 0x01, 0x09, 0x02, 0x66},   //12 ДкK
					{0x55, 0x04, 0x01, 0x03, 0x5D},         //13 AG1 03 5D// A1 02 5C
					//{0x55, 0x04, 0x01, 0x02, 0x5C},         //13 AG1 03 5D// A1 02 5C
					{0x55, 0x04, 0x01, 0x1A, 0x74},			//14
					{0x55, 0x05, 0x02, 0x13, 0x00, 0x6F},   //15

					{0x55, 0x05, 0x01, 0x09, 0x02, 0x66}, //Дк.К 16
					{0x55, 0x04, 0x01, 0x27, 0x81},   //КонтрЛК11 17
					{0x55, 0x05, 0x02, 0x6, 0x00, 0x62},  //18

					{0x55, 0x04, 0x01, 0x17, 0x71}, //НУС  19
					{0x55, 0x07, 0x02, 0x14, 0x00, 0x57, 0x7f, 0x48}, //20
					{0x55, 0x05, 0x01, 0x24, 0x03, 0x82}, //21 Ртек com
					{0x55, 0x06, 0x02, 0x11, 0x00, 0x00, 0x72}, //22 Ртек lvl
					{0x55, 0x04, 0x01, 0x18, 0x72}, //23 прер
					//{0x55, 0x08, 0x01, 0x07, 0x00, 0x00, 0x00, 0x00, 0x00}, //24 ДанИ
					{0x55, 0x07, 0x01, 0x07, 0x00, 0x00, 0x00, 0x00}, //24 ДанИ
					{0x55, 0x05, 0x01, 0x04, 0x00, 0x5F}, //25 АКП-ВЫКЛ
					{0x55, 0x05, 0x01, 0x16, 0x01, 0x72}, //26 К2-ВКЛ
					//{0x55, 0x05, 0x01, 0x16, 0x00, 0x71}, //26 К2-ВЫКЛ
					{0x55, 0x0B, 0x01, 0x08, 0xD6, 0xA1, 0x66, 0xC9, 0x00, 0x00, 0x00, 0x00}, //27 DanP.T.R
					//{0x55, 0x0B, 0x01, 0x08, 0x4A, 0x17, 0x15, 0x40, 0xAE, 0x29, 0x00, 0xF6}, //27 DanP.T.R
					{0x55, 0x05, 0x01, 0x25, 0x6B, 0xEB}, //28 ZDR.Z
					{0x55, 0x05, 0x01, 0x2C, 0x02, 0x89}, //29 R4K
					{0x55, 0x05, 0x01, 0x05, 0x02, 0x62}, //30 S4K
					{0x55, 0x06, 0x01, 0x19, 0x28, 0x01, 0x9E}, //31 Сеанс
					{0x55, 0x05, 0x01, 0x2A, 0x06, 0x8B}, //32 ОБР-ТЕСТ
					{0x55, 0x05, 0x01, 0x22, 0x01, 0x7E}, //33 ПРЕР
					{0x55, 0x05, 0x01, 0x24, 0x06, 0x85}}; //34 Выв. КД


int err=0, i,i1;
int time_out=1;
char buffer[1000];
char name_dev1[20] = "//1/dev/ser1"; //Имя у-ва N1 (для чтения на стенде)
char name_dev2[20] = "/dev/ser2"; //Имя у-ва N2 (для записи на стенде)

pid_t proxy1, proxy2, pid;
int fd_1, fd_2;
unsigned int    Tcount, 		//счетчик таймера
				Tcount_com,		//время  отправки команды
				Ncount;		//кол-во повторений
unsigned state;
unsigned char c;
div_t msec; 
time_t time_of_day;
char b[80];
int N_COM=1;//,KVIT=0;//,ILS=0,LK11=0;
short comOK[50];


void Init_K2()
{
   int i,i1;
// Проверка запуска драйвера модуля ПЦС ЦВС-3-1
	pid_drv=get_pid_process("PCS",0);
	if (!pid_drv) {
		printf("!!! Драйвер не запущен\n"); err++; rez=2;
		exit(0);
	} 

	wr_cpcs_s.type=7;
	wr_cpcs_s.cnl=chan2;
	Send(pid_drv,&wr_cpcs_s,&wr_cpcs_r,sizeof(wr_cpcs_s),sizeof(wr_cpcs_r));

// Инициализация канала ПЦС для связи с каналом chan2 (ПОСТ-3Ц)
	init_pcs_s.type=1;
	init_pcs_s.cnl=chan2;			// номер канала ПЦС
	init_pcs_s.speed=19200;			// скорость передачи данных
	init_pcs_s.b_info=8;			// кол-во бит в информационном байте
	init_pcs_s.b_StrStp=1;			// кол-во стоп-бит
	init_pcs_s.b_prt=0;			// наличие и тип паритета (нет) ПОСТ-3Ц
	init_pcs_s.dev=0; 				// тип у-ва (0 - обычный RS канал) 
	init_pcs_s.lvl_inp=45;			// уровень заполнения FIFO до прерывания

	i=Send(pid_drv,&init_pcs_s,&init_pcs_r,sizeof(init_pcs_s),sizeof(init_pcs_r));
	if (i==-1) printf("Нет общения с драйвером\n");
	if (init_pcs_r.type==1) 
		if (init_pcs_r.status!=0) {
			printf("!!! Инициализация канала %d не прошла\n", 3);
			exit(2);
		}
	printf("Инициализация канала %d прошла\n", 3);

// Старт канала chan2
	start_pcs_s.type=2;
	start_pcs_s.cnl=chan2;
	start_pcs_s.ID_P=getpid();

	//printf("my_id= %d\n",start_pcs_s.ID_P);
	Send(pid_drv,&start_pcs_s,&start_pcs_r,sizeof(start_pcs_s),sizeof(start_pcs_r));
	if (start_pcs_r.type==2) {
		if (start_pcs_r.status==-1) {
			printf("!!! Старт канала %d не выполнен\n", 3);
			exit(2);
		} 
		else { proxy_DRV2=start_pcs_r.Proxy;
		  printf("Старт канала %d выполнен, proxy=%d\n", 3, proxy_DRV2);
		}
	}       

	for(i=0;i<10;i++)
	{
		rd_cpcs_s.type=4;
		rd_cpcs_s.cnl=chan2;
		Send(pid_drv,&rd_cpcs_s,&rd_cpcs_r,sizeof(rd_cpcs_s),sizeof(rd_cpcs_r));
		delay(50);
	}   
}


write_com (Ncom)
{
	wr_cpcs_s.type=5;
	wr_cpcs_s.cnl=chan2;
	wr_cpcs_s.cnt=test_K2[Ncom][1]+1;
	for(i=0;i<test_K2[Ncom][1]+1;i++) wr_cpcs_s.uom.dt[i]=test_K2[Ncom][i];
	Send(pid_drv,&wr_cpcs_s,&wr_cpcs_r,sizeof(wr_cpcs_s),sizeof(wr_cpcs_r));

	//printf("время : %d  <---  (",Tcount);
	//1time_of_day=time(NULL);
	//1msec=div(Tcount,10);
	//1strftime(b, 40 , "%T", localtime(&time_of_day));//D T
	//1printf("%s:%03d <-- (",b,msec.rem*100);			
	printf("%02x:%02x:%02x ", p->CEB[2]>>8,p->CEB[3]>>8,p->CEB[3]&0x00ff);

	for(i1=0;i1<test_K2[Ncom][1]+1;i1++) printf("%x.",test_K2[Ncom][i1]);
	N_COM++;Ncount++;
	Tcount_com=Tcount;
}

//посылка команды ДанИ.V.dV не проверяет ответную квитaнцию
write_com24 (Ncom)
{
	int i1;
	short V=0;
	int i;
	V=(short)(p->from41.Vr);
	if (TS) V=0;
	test_K2[24][4]=V&0x00ff;
	test_K2[24][5]=V>>8;
	printf(" V=%d",V);
	//V=(short)(p->from41.Ar/1.5625);
	V=(short)(p->from41.Ar);
	if (TS) V=0;
	test_K2[24][6]=V&0x00ff;
	test_K2[24][7]=V>>8; 
	printf(" dV=%d\n",V);
	test_K2[24][8]=0; //check sum
	for (i1=0;i1<8;i1++) test_K2[24][8]+=test_K2[24][i1]; //test sum
	wr_cpcs_s.type=5;	wr_cpcs_s.cnl=chan2;	wr_cpcs_s.cnt=9;

	//test_K2[24][7]=0; //check sum
	//for (i1=0;i1<7;i1++) test_K2[24][7]+=test_K2[24][i1]; //test sum
	//wr_cpcs_s.type=5;	wr_cpcs_s.cnl=chan2;	wr_cpcs_s.cnt=8;

	for(i=0;i<test_K2[24][1]+1;i++) wr_cpcs_s.uom.dt[i]=test_K2[24][i];
	Send(pid_drv,&wr_cpcs_s,&wr_cpcs_r,sizeof(wr_cpcs_s),sizeof(wr_cpcs_r));
	printf("%02x:%02x:%02x ", p->CEB[2]>>8,p->CEB[3]>>8,p->CEB[3]&0x00ff);
	for(i1=0;i1<8;i1++) printf("%x.",wr_cpcs_s.uom.dt[i1]);printf("\n");


//	printf("!! V=%d\n",V);
}


write_com27 (Ncom)
{
	int i1;
	short D=0;
	int i;
	D=(short)(p->from41.D/1000);
	if (TS) D=0;
	test_K2[27][8]=D&0x00ff;
	test_K2[27][9]=D>>8;
	test_K2[27][11]=0; //check sum
	for (i1=0;i1<11;i1++) test_K2[27][11]+=test_K2[27][i1]; //test sum
	wr_cpcs_s.type=5;	wr_cpcs_s.cnl=chan2;	wr_cpcs_s.cnt=12;

	for(i=0;i<test_K2[27][1]+1;i++) wr_cpcs_s.uom.dt[i]=test_K2[27][i];
	Send(pid_drv,&wr_cpcs_s,&wr_cpcs_r,sizeof(wr_cpcs_s),sizeof(wr_cpcs_r));
	printf("%02x:%02x:%02x ", p->CEB[2]>>8,p->CEB[3]>>8,p->CEB[3]&0x00ff);
	for(i1=0;i1<12;i1++) printf("%x.",wr_cpcs_s.uom.dt[i1]);printf("\n");
	printf("\n D=%d \n",D);

}


read_kvit()
{
	//printf("Tcount-Tcount_com=%d\n",Tcount-Tcount_com);
	if (comOK[1]>0) {comOK[1]=Ncount=0;N_COM++;return;}
	if (Tcount>Tcount_com+50) {printf("Timeout 5 sec (квитанция)\n");N_COM--;}
	if (Ncount>10) {printf("TPO not responding\n");N_COM=0;}
}

read_kvit_NUS()
{
	//printf("Tcount-Tcount_com=%d\n",Tcount-Tcount_com);

	if (comOK[1]>0) {comOK[1]=Ncount=0;N_COM++;return;}
	//if (Tcount>Tcount_com+2000) {printf("Timeout 5 sec (квитанция)\n");N_COM--;}
	//if (Ncount>10) {printf("TPO not responding\n");N_COM=0;}
}

main(int argc, char *argv[]) {
	unsigned short cr_com=0;
	unsigned int Tpr=0;
	unsigned int Tpr1=0;
	unsigned int Tstart=0;
	short OC4=0;
    pid_t proxy;
	FILE 		 	*out_fp = NULL;
    timer_t id;
	unsigned char chkSUM=0,N=0;
	struct itimerspec timer;
    struct sigevent event;

	int K2count=0;//счетчик бафтов в буфере К2
	while( (i=getopt(argc, argv, "t:s:") )!=-1)	{
		switch(i){
			case 't' :	TM=1; break;
			case 's' :	TS=1; break;
			//case 'c' :	sscanf(optarg,"%d",COR_T); break;
		}//switch
	}//while

	delay(500);
	open_shmem();

	p->to41.PrM_K2=0;
	if (TS) printf("		УСТАНОВЛЕН РЕЖИМ РАБОТЫ ПО СИГНАЛУ ТВК\n");
	if (TM) printf("		УСТАНОВЛЕН РЕЖИМ РАБОТЫ БЕЗ ОЖИДАНИЯ КОМАНДЫ НАЧАЛА СС\n");
	if (COR_T!=0) printf("		УСТАНОВЛЕНА КОРРЕКЦИЯ ПО ВРЕМЕНИ %d МИНУТ\n",COR_T);

//	printf("TM=%d TS=%d COR_T=%d\n\n",TM,TS,COR_T);
	COR_T=COR_T*60;

	if (!TM)
	{
		//if ( ( out_fp = fopen( "//1/home/seversk/new31/k2_log", "w" ) ) == NULL ) 
		//		fprintf( stderr, "Couldn't create/open file. %s\n",  strerror( errno ) );
		printf("ОЖИДАНИЕ КОМАНДЫ НАЧАЛА СС \n");
		while((p->from41.num_com!=1)&&(p->from41.num_com!=2)) delay(500);
	}	

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

    timer.it_value.tv_sec     = 3L; //start after X sec
    timer.it_value.tv_nsec    = 0L;
    timer.it_interval.tv_sec  = 0;
    timer.it_interval.tv_nsec = 100*m_sec;
    timer_settime( id, 0, &timer, NULL );

	delay(1000);	
	Init_K2();
	delay(500);
	printf("			НАСТРОЙКА К2	команда %d\n",p->from41.num_com);
	
	while(1)
	{
		pid=Receive(0,0,0); //получение всех системных сообщений
		if (pid==proxy_DRV2) 
		{  //получили сообщение чтения данных из ПОСТ-3Ц
			rd_cpcs_s.type=4;
			rd_cpcs_s.cnl=chan2;
		    // выдача команды и прием данных
			i=Send(pid_drv,&rd_cpcs_s,&rd_cpcs_r,sizeof(rd_cpcs_s),sizeof(rd_cpcs_r));
			//printf("Ответ =");
			//for(i=0;i<rd_cpcs_r.cnt;i++) printf(" %02x",rd_cpcs_r.uim.dt[i]);printf("\n");
    		// переформатирование
		    for (i=0;i<rd_cpcs_r.cnt;i++)  buffer[i+K2count]=rd_cpcs_r.uim.dt[i];//дополним массив

			//if ( out_fp != NULL ) 
			//	if ( fwrite( buffer+K2count, rd_cpcs_r.cnt, 1, out_fp ) != 1 )
			//		fprintf( stderr, "Failed to write to file. %s\n", strerror( errno ) );

			K2count=K2count+rd_cpcs_r.cnt;//увеличим кол-во байт в буфере
		}

		if (pid==proxy) 
		{  //срабатывание таймера
			Tcount++;
			//for(i1=0;i1<K2count;i1++) printf("%x ",buffer[i1]);printf("\n");         
			//printf("com1=%2x ", c);  //печать байта
			//printf("count=%d\n",K2count);
			if (K2count>4) 
			for(i=0;i<K2count;i++)
			if (buffer[i]==0x55) //найден заголовок
			{	
				N=buffer[i+1]; //кол-во байт в пакете без КС
				if (K2count>N+i) //достаточное кол-во байт в буфере
				{
					//printf("N=%d \n",N);					
					//1_of_day=time(NULL);
					//1strftime(b, 40 , "%T", localtime(&time_of_day));//D T
					//1msec=div(Tcount,10);
					//1printf("%s:%03d -->",b,msec.rem*100);			
					printf("%02x:%02x:%02x ", p->CEB[2]>>8,p->CEB[3]>>8,p->CEB[3]&0x00ff);
					for(i1=0;i1<N;i1++) chkSUM+=buffer[i+i1]; //подсчет контр суммы         
					//printf("i=%d chkSUM=%x\n",i,chkSUM);
					if (chkSUM!=buffer[N+i]) //если не совпадает контр сумма
					{
						printf(" error CHKSUM\n");//проверка контрольной суммы
						K2count=chkSUM=0;		//очистка буфера
						break;					//выходим и ждем дальше
					}
					//пришел правильный пакет
					printf(" (");			
					for(i1=0;i1<N+1;i1++) printf("%x.",buffer[i1+i]);printf(")");         
					//анализ полученного пакета
					switch(buffer[i+1])
					{
 						case 4: if(buffer[i+2]==3)
								{
									//printf(" Получена квитанция - команда принята ");		
									//if(buffer[i+3]==0) {comOK[1]++;printf("ПРАВИЛЬНО");break;}
									//else printf("неправильно");
									if(buffer[i+3]==0) {comOK[1]++;printf("");break;}
									else printf("");
								}
								else printf(" Получен неизвестный пакет");		
								break;
 						case 5: if(buffer[i+2]==2)
								{
									switch(buffer[i+3])
									{
										case 0x03 : printf(" ИЛС Сообщение принято - ");
													if (buffer[i+4]==0) {printf("норма");comOK[0]=1;}
													else printf("ненорма");
													break;
										case 0x06 : printf(" ЛК1 Сообщение принято - ");
													if (buffer[i+4]==0) {printf("норма");comOK[17]=1;}
													else printf("ненорма");
													break;
										case 0x18 : printf(" ЛК2 Сообщение принято - ");
													if (buffer[i+4]==0) {printf("норма");comOK[5]=1;}
													else printf("ненорма");
													break;
										case 0x01 : printf(" АвтК Сообщение принято - ");
													if (buffer[i+4]==0) {printf("норма");comOK[7]=1;}
													else printf("ненорма");
													break;
										case 0x02 : printf("  Дан.Н.НН - ");
													if (buffer[i+4]&1) printf(" нет-ДанИ");
													if (buffer[i+4]&2) printf(" нет-ДанП");
													//printf("\n");
													break;
										case 0x010: printf("  ПРМ - ");
													if (buffer[i+4]&0x01) {printf(" ПРС");p->to41.PrM_K2=1;}else p->to41.PrM_K2=0;
													if (buffer[i+4]&0x02) printf(" СС");
													if (buffer[i+4]&0x04) printf(" ЗС");
													if (buffer[i+4]&0x08) printf(" СБ");
													if (buffer[i+4]&0x10) {printf(" ИДК");p->to41.Pr_ZI_K2=1;}else p->to41.Pr_ZI_K2=0;
													if (buffer[i+4]&0x20) printf(" ИДД");
													if (buffer[i+4]&0x40) printf(" ВИ");
													if (buffer[i+4]&0x80) printf(" ПС");
													//printf("\n");
													break;
																
									}
								}
								else printf(" Получен неизвестный пакет");								
								break;
 						case 6: if(buffer[i+2]==2)
								{
									switch(buffer[i+3])
									{
										case 0x07 : OC4=buffer[i+5];OC4=(OC4<<8)|buffer[i+4];
													printf(" ОСЧ - %6.3f м/с ",OC4*0.582);break;
													//printf(" ОСЧ - %d м/с ",OC4);break;
										case 0x11 : printf(" Pтек - %d ",buffer[i+4]);
													if (buffer[i+4]>0)	p->to41.UR_sign_K2=buffer[i+4]*3;
													else p->to41.UR_sign_K2=0;
													break;
										case 0x73 : printf(" ПРМ1 - ");
													if (buffer[i+4]&0x01) {printf(" ПРС");p->to41.PrM_K2=1;}else p->to41.PrM_K2=0;
													if (buffer[i+4]&0x02) printf(" СС");
													if (buffer[i+4]&0x04) printf(" ЗС");
													if (buffer[i+4]&0x08) printf(" СБ");
													if (buffer[i+4]&0x10) {printf(" ИДК");p->to41.Pr_ZI_K2=1;}else p->to41.Pr_ZI_K2=0;
													if (buffer[i+4]&0x20) printf(" ИДД");
													if (buffer[i+4]&0x40) printf(" ВИ");
													if (buffer[i+4]&0x80) printf(" ПС");	

													if (buffer[i+5]&0x01) printf(" 27М1н");
													if (buffer[i+5]&0x02) printf(" ПФ");
													if (buffer[i+5]&0x04) printf(" РНС");
													if (buffer[i+5]&0x08) printf(" РПС");
													if (buffer[i+5]&0x10) printf(" РСС");
													if (buffer[i+5]&0x20) printf(" ЗКС");
													if (buffer[i+5]&0x40) printf(" ОСС");
													if (buffer[i+5]&0x80) printf(" НС");
													//printf("\n");
													break;
									}
								}
								else printf(" Получен неизвестный пакет");								
								break;
 						case 7: if(buffer[i+2]==2)
								{
									switch(buffer[i+3])
									{
										case 0x014: printf(" НУС Сообщение принято - ");
													if (buffer[i+4]==0) {printf("норма");comOK[10]=1;}
													else printf("ненорма");
													comOK[10]=1;//!!!
													break;
									}
								}
								else printf(" Получен неизвестный пакет");								
								break;
 						case 0x1b:
								if(buffer[i+2]==2)
								{
									printf("\n		");
									if (buffer[i+5]||buffer[i+6]) printf("ГР=%02x%02x ",buffer[i+6],buffer[i+5]);
									if (buffer[i+7]||buffer[i+8]) printf("НДИ=%02x%02x ",buffer[i+8],buffer[i+7]);
									if (buffer[i+9]||buffer[i+10])printf("МН=%02x%02x ",buffer[i+10],buffer[i+9]);
									if (buffer[i+19]||buffer[i+20]||buffer[i+21]) printf("ИИ=%02x%02x%02x ",buffer[i+21],buffer[i+20],buffer[i+19]);
									if (buffer[i+24]) {printf("СИГ=%x ",buffer[i+24]);p->to41.GL_priem=1;} else p->to41.GL_priem=0;
									if (buffer[i+25]) {printf("СРГ=%x ",buffer[i+25]);p->to41.GL_CP=1;} else p->to41.GL_CP=0;
									if (buffer[i+26]) printf("СбСИГ=%x ",buffer[i+26]);
								}
								else printf(" Получен неизвестный пакет");								
								break;


					}									
					printf("\n");
					chkSUM=0;
					K2count-=N+i+1; // сдвигаем данные в буфере
					for(i1=0;i1<K2count;i1++) buffer[i1]=buffer[i+N+i1+1];
				}			
			}

			//printf("%d\n",Tcount);
			switch (N_COM) //порядок выполнения команс в сеансе
			{
				case 0:	close(fd_1);        timer_delete(id);
						printf("EXIT\n");   exit(0);
 				case 1: write_com(0);printf(")  Команда ИЛС отправлена\n");break;
				case 2: read_kvit();break;
				case 3: if (comOK[0]>0) {comOK[0]=Ncount=0;N_COM++;break;}
						if (Tcount>Tcount_com+50) {printf("Timeout 5 sec\n");N_COM=-2;}
						if (Ncount>10) {printf("TPO not responding\n");N_COM=0;}
						break;
 				case 4: write_com(17);printf(")  Команда КонтрЛК11 отправлена\n");break;
				case 5: read_kvit();break;
				case 6: if (comOK[17]>0) {comOK[17]=Ncount=0;N_COM++;break;}
						if (Tcount>Tcount_com+50) {printf("Timeout 5 sec\n");N_COM=-2;}
						if (Ncount>10) {printf("TPO not responding\n");N_COM=0;}
						break;
 				case 7: write_com(5);printf(")  Команда ЛК2 отправлена\n");break;
				case 8: read_kvit();break;
/**/			case 9: if (comOK[5]>0) {comOK[5]=Ncount=0;N_COM+=4;break;}
						if (Tcount>Tcount_com+50) {printf("Timeout 5 sec\n");N_COM=-2;}
						if (Ncount>10) {printf("TPO not responding\n");N_COM=0;}
						break;
 			   case 10: write_com(7);printf(")  Команда АвтК отправлена\n");break;
			   case 11: read_kvit();break;
			   case 12: if (comOK[7]>0) {comOK[7]=Ncount=0;N_COM++;break;}
						if (Tcount>Tcount_com+500) {printf("Timeout 50 sec\n");N_COM=-2;}
						if (Ncount>10) {printf("TPO not responding\n");N_COM=0;}
						break;
 			   case 13: write_com(25);printf(")  Команда АКП-ВЫКЛ отправлена\n");break;
			   case 14: read_kvit();break;
 			   case 15: write_com(12);printf(")  Команда ДК-Р отправлена\n");break;
			   case 16: read_kvit();break;
 			   case 17: write_com(13);printf(")  Команда Реж-А1 отправлена\n");break;
			   case 18: read_kvit();break;
 			   case 19: write_com(26);printf(")  Команда К2-ВЫКЛ отправлена\n");break;
			   case 20: read_kvit();break;
 			   case 21: write_com(9);printf(")  Команда КАН-Л отправлена\n");break;
			   case 22: read_kvit();break;

			   case 23: //if (TS) //!!!!!!!!!!!!!!!!!!!!!!!!
						{	
							printf("\n---- ВКЛЮЧЕН ТВК С МАХ ОСЛАБЛЕНИЕМ -----\n");
							//p->toPR1[3]=0xFC00;//0xFC00;
							//p->toPR1[3]=0x0000;//0xFC00;
							p->toPR1[3]=0xBC00;//0xFC00;
						}
						N_COM++;break;
			   case 24:	N_COM++;break;
						//if(kbhit()) {N_COM++;getch();}break;
			   case 25:	write_com(10);printf(")  Команда НУС отправлена\n");break;
			   case 26: read_kvit_NUS();break;
			   case 27: if (comOK[10]>0) {comOK[10]=Ncount=0;N_COM+=9;break;}//9
						if (Tcount>Tcount_com+500) {printf("Timeout 50 sec\n");N_COM=-2;}
						if (Ncount>10) {printf("TPO not responding\n");N_COM=0;}
						
						break;
 			   case 28: N_COM+=8;
						//write_com(27);printf(")  Команда ДанП.Т.Р отправлена\n");
						break;
			   case 29: read_kvit();break;
 			   case 30: write_com(28);printf(")  Команда ЗДР.З отправлена\n");break;
			   case 31: read_kvit();break;
 			   case 32: write_com(29);printf(")  Команда РЧК отправлена\n");break;
			   case 33: read_kvit();break;
 			   case 34: write_com(30);printf(")  Команда СЧК отправлена\n");break;
			   case 35: read_kvit();break;
 			   case 36: write_com24(24);printf("  Начата посылка команды ДанИ.V.dV\n");
						comOK[24]=1;N_COM++;
						//if (TS) p->toPR1[3]=0xFC00;//8000-onn 0 dBm 0000-off TVK 
						if (TS) p->toPR1[3]=0x8000;//8000-onn 0 dBm 0000-off TVK 
						else p->toPR1[3]=0x0000;//8000-onn 0 dBm 0000-off TVK 
						Tstart=p->from41.T_SS-10; //время старта за 10 сек до сеанса
						//Tpr=p->Dout41[30]*3600+p->Dout41[31]*60+p->Dout41[32]; //время прибора из СЕВ
						Tpr=p->Dout41[30]*3600+p->Dout41[31]*60+p->Dout41[32]+COR_T; //время прибора из СЕВ
						if (Tstart<Tpr) //если "проспали" сеанс -> переводим на след цикл
							if ((Tpr-Tstart)<1200)	while(Tstart<Tpr) Tstart+=30;					
						break;
//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
			   case 37: if (TM) {N_COM++;break;}
						Tpr=p->Dout41[30]*3600+p->Dout41[31]*60+p->Dout41[32]+COR_T; //время прибора из СЕВ
						if (Tpr1!=Tpr) printf("---- ОЖИДАНИЕ НАЧАЛА СС   Ts=%d  Tpr=%d-----\n",Tstart,Tpr);
						Tpr1=Tpr;
						//Tpr=p->Dout41[30]*3600+p->Dout41[31]*60+p->Dout41[32]; //время прибора из СЕВ
						if (Tpr==Tstart) //если время начала сеанса совпало
						{
							printf("			СТАРТ Ts=%d Tpr=%d\n",Tstart,Tpr);
							Tstart+=30;			
							N_COM++;//выход из цикла
						}				
						break;
			//-----------------------------------
 			   case 38: write_com27(27);printf(")  Команда ДанП.Т.Р отправлена\n");
						N_COM++;				
						break;
			   case 39: read_kvit();break;
			//-----------------------------------
 			   case 40: write_com(31);printf(")  			Команда Сеанс.L отправлена\n\n");
						break;
			   case 41: read_kvit();break;
 			   case 42: if (TS) {if (Tcount>Tcount_com+40) {write_com(32);printf("\n\nОБР ТЕСТ\n\n");}}
						else N_COM++;
						break;			   
 			   case 43: if (TS)	
						{
							if (Tcount>Tcount_com+100) {write_com(23);write_com(33);N_COM=37;}
							else if (comOK[24]>0) {comOK[24]++;if (comOK[24]>10) {comOK[24]=1;write_com24(24);}} 
						}
						else 
						{
							if (Tcount>Tcount_com+240) {write_com(23);write_com(33);N_COM=37;}
							else if (Tcount>Tcount_com+40) {comOK[24]++;if (comOK[24]>10) {comOK[24]=1;write_com24(24);}} 
						}
						break;
			   case 44: read_kvit();break;
			}
			//if (comOK[24]>0) {comOK[24]++;if (comOK[24]>10) {comOK[24]=1;write_com24(24);}}
		}
		if (kbhit()) 
		{
			getch();
			write_com(23);
			write_com(33);
			N_COM=0;
			p->toPR1[3]=0x0000;//8000-onn 0 dBm 0000-off TVK 
		}//printf("%d",getch());
	}
	close(fd_1);
	fclose( out_fp );
	timer_delete(id);
	printf("EXIT\n");
	exit(0);
}

