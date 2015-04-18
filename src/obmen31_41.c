  #include <sys/types.h>
  #include <sys/socket.h>
  #include <sys/kernel.h>

  #include <netinet/in.h>
  #include <stdio.h>
  #include <unistd.h>
  #include <stdlib.h>
  #include <netdb.h>
  #include <math.h>
  #include "../include/shared_mem.h"
  #include "../include/gloriya.h"
  #include "../include/IO_DT_SOI.h"
  #include "../include/func_IP.h"

  #define RAD 5729.57	
  #define SRC_PORT41 8208
  #define DST_PORT41 8208
  #define SRC_PORT42 8206
  #define DST_PORT42 8206
  #define pi 	3.14159265
  #define GRADtoRAD  0.00153398
  #define RADtoGRAD 651.898648
  #define Kncu 	12.27
  #define Kq 	0.5

  # define max_len_OUT    4096*8
  # define max_len_IP     4096*8


	float Mc=64.*1852./32384./3600., Dbl, Flt=0;
	int sock, length, i , count_mes=0;
	static Udp_Client_t Uc41,Uc42;
	char bufi[1024];
	char bufo[1024];
	char gloria_start=0;	
	int gloria_count=0;	

	char out_buf[1024];
	obmen_41_31_t from41;	
	obmen_31_41_t to41;	
	simfonia41_t simfonia;	
	int r,bytes,byta4;
	pid_t pid_CEP;
	short MK2[15],Priem[50],buf;
	short byta2,T,len_OUT,sen,j;
    div_t   vol;    // vol.quot - количество полных томов
    char          pack_buf[1500];  // буфер задачи obm_41_31. Выходные данные в Socket
    char                 numb_pack,     // текущий номер пакета
                         numb_vol;      // текущий номер тома в пакете

struct
   { 
     char      out_buf[max_len_OUT];  // данные для Socket'a
   } ip_out; 

	  
main ()
{
float 	C1,C2,C3,C4,C5,C6,C7,C8;
unsigned short cr_com; //порядковый номер предыдущей команды
		short		V,dV;
int cnt=0;
int i1=0;
int rez;
unsigned short buf[4];
float Angle0;
 C1=2048./pi;C2=4096.0/360.0;C3=180./pi;C4=C1*Kncu;
 C5=C2*Kncu;C6=C1*Kq;C7=C3;C8=C2*Kq;
//поиск сервера
qnx_name_attach(0,"4.1");
//инициализация канала UDP
	i = Udp_Client_Ini(&Uc41,"194.1.1.5",SRC_PORT41,DST_PORT41);
	printf(" Udp_Init=%d	\n", i);
			
	//gloriya(1,1,31);//test K2 по умолчанию
	//gloriya(1,1,1);//work K2
	//gloriya(1,0,1);//work K1
	gloriya(1,1,31);//test K2 по умолчанию

	delay(3000);
	open_shmem();
	delay(10000);

	//Angle0=4;
	//p->jump=-1;

while(1)
  {
	//for(i=0;i<sizeof(obmen_41_31_t);i++) bufi[i]=0;

	bytes = Udp_Client_Read(&Uc41,bufi,4096);

    memcpy(&p->from41,&bufi[4],sizeof(obmen_41_31_t));

	//printf("V=%d\n",V);
	//printf("V=%f\n",p->from41.Vr);
	//for(i=4;i<20;i++) printf("%x ",bufi[i]);printf("\n");
	//printf("crcom=%x n_com=%x NKS=%x NSHKR=%x Nd_FR4=%x N_FR4=%x ZUNf=%x N_psp=%x Vr=%f Ar=%f\n",
	//p->from41.cr_com,p->from41.num_com,p->from41.num_KS,
	//p->from41.Nkey_SHAKR,p->from41.Nd_FRCH,p->from41.N_FRCH,
	//p->from41.ZUNf,p->from41.Nans_PSP,p->from41.Vr,p->from41.Ar);

	//SIMFONIA
	buf[0]=p->Dout41[7]; 
	buf[1]=p->Dout41[6];
 	buf[2]=p->Dout41[9];
	buf[3]=p->Dout41[8];		
 	if (p->from41.num_com==0) p->Kg41=p->from41.D;  //курс из прибора 4.1 при 0 команде
	p->to41.simfonia41.status1=p->Dout41[24]; // 0, если нет обмена
    memcpy(&byta2,&p->Dout41[13],2); p->to41.simfonia41.status2=byta2;	//1, если ендостоверно
	
	//if ((p->to41.simfonia41.status1==0)||(p->to41.simfonia41.status2>0)) p->to41.simfonia41.Kg=p->Kg41;
	//else 
    {memcpy(&byta2,&p->Dout41[0],2);	 Flt=byta2*pi/(1<<14); p->to41.simfonia41.Kg=Flt;}
	
	p->to41.simfonia41.Kg=1.579; //временно
    
	memcpy(&byta2,&p->Dout41[4],2);	 Flt=byta2*Mc;p->to41.simfonia41.V=Flt;	
    memcpy(&byta4,&buf[0],4);	 Flt=byta4*pi/(1<<31);p->to41.simfonia41.fi=-Flt;// printf("Fi=%f \n",Flt*180/pi);
    memcpy(&byta4,&buf[2],4);	 p->to41.simfonia41.la=-(byta4*pi/(1<<31));	

    memcpy(&byta2,&p->Dout41[10],2); p->to41.simfonia41.a=byta2*32000./(1<<15);	
    memcpy(&byta2,&p->Dout41[11],2); p->to41.simfonia41.b=byta2*32000./(1<<15);	
    memcpy(&byta2,&p->Dout41[12],2); p->to41.simfonia41.tau=byta2*pi/(1<<15);	

//	printf("%02x%02x	",p->Dout41[6],p->Dout41[7]);
//  printf("Kg=%4.3f V=%4.3f fi=%4.3f ",p->to41.simfonia41.Kg, p->to41.simfonia41.V,p->to41.simfonia41.fi);
//	printf("la=%4.3f a=%4.3f b=%4.3f ",p->to41.simfonia41.la,p->to41.simfonia41.a,p->to41.simfonia41.b);
//	printf("tau=%4.3f st=%d \n",p->to41.simfonia41.tau,p->to41.simfonia41.status2);
// 	printf("st1=%x st2=%x\n",p->to41.simfonia41.status1,p->to41.simfonia41.status2);
//	 printf("Kypc=%8.4f \n",p->to41.simfonia41.Kg);

	//if (p->U.SUM_20==0) p->to41.Fd_FACT=0;
	//	else 
	p->to41.Fd_FACT=-(float)0.24414*p->U.DPL_1;
	//printf("d=%d\n",p->U.DPL_1);

	//p->to41.UR_sign_K1=(short)p->U.SUM_20;	
	if (p->to41.UR_sign_K1>32) p->to41.PrM_K1=1;else p->to41.PrM_K1=0;

//	printf("sum4=%f ur_sign=%f\n",p->U.SUM_4,p->to41.UR_sign_K1);
//	printf(" Angle_Pr1  b=%4.3f p=%4.3f   ",p->to41.beta_FACT*57.3,p->to41.P_FACT*57.3);
//	printf(" Angle_PR4  b=%4.3f p=%4.3f\n",p->from41.beta*57.3,p->from41.P_ANT*57.3);
//	printf("					 P_ANT = %4.3f", p->from41.P_ANT*57.3);
//	printf(" P_FACT = %4.3f  com42=%d \n",p->to41.P_FACT*57.3,p->from42.num_com);

	//printf("n_com1=%d cr_com=%d\n",p->from41.num_com,p->from41.cr_com);
	//printf("Angl 2=%02f r=%f NK%d  \n",from41.P_ANT,p->to41.P_FACT,from41.num_com);

	//Готовность к сеансу связи
	if (p->from41.num_com==1) i1++; //считаем время от первой ком нач сеанса
	if ((gloria_start==0)&&((p->from41.num_com==1)||(p->from41.num_com==2)))
	{
		if ((p->from41.Nkey_SHAKR<=31)&&(p->from41.Nkey_SHAKR>=0)) 
			{rez=gloriya(1,p->from41.num_KS-1,p->from41.Nkey_SHAKR);gloria_start=1;}

		else printf("Error Gloriya Key %d \n",p->from41.Nkey_SHAKR);
		if (rez) p->to42.Mispr=p->to42.Mispr&0xFEFF;else p->to42.Mispr=p->to42.Mispr|0x0100;
	}

	if (p->from41.num_com==2) 
	{
		gloria_count++;
		if (gloria_count>60)
		{
			rez=gloriya_read();
			if (rez) p->to42.Mispr=p->to42.Mispr&0xFEFF;else p->to42.Mispr=p->to42.Mispr|0x0100;
			gloria_count=0;
		}
	}

	if (i1>30) 	{p->to41.pr_GSS=1;i1=0;}
	if (p->from41.num_com==3) gloria_start=0;
	///printf("GSS=%d ",p->to41.pr_GSS);

	cr_com=p->from41.cr_com;
	//признак автосопровождения по К1
	//p->to41.pr_AS=1;
	//призник приема данных по К2
	//p->to41.pr_PD=1;
	
    len_OUT = sizeof(obmen_31_41_t); //!!!!

    memcpy(ip_out.out_buf,&p->to41,sizeof(obmen_31_41_t)); ///!!!!!!

    if (len_OUT <= 1400) 
    {
         pack_buf[0] = 80 ;               
         pack_buf[1] = numb_pack;
         pack_buf[2] = 1;
         pack_buf[3] = 1;
         for (j = 0; j<1400; j++) pack_buf[j+4] = ip_out.out_buf[j];
         sen = Udp_Client_Send(&Uc41,pack_buf,(len_OUT+4));
    }
    else
    {
         pack_buf[0] = 81 ;               
         vol = div(len_OUT,1400);
         pack_buf[1] = numb_pack;
         pack_buf[2] = 1;
         pack_buf[3] = vol.quot + 1;
         for (numb_vol = 1;numb_vol <=vol.quot; numb_vol++)
         {
           for (j = 0; j<1400; j++) {pack_buf[j+4] = ip_out.out_buf[(1400*(numb_vol-1))+j]; }
           Udp_Client_Send(&Uc41,pack_buf,1404);
		   if (numb_vol==3) delay(2);//5 10
		   if (numb_vol==6) delay(2);
		   if (numb_vol==9) delay(2);
		   if (numb_vol==12) delay(2);

           pack_buf[2] ++ ;
         }
                
         for (j = 0; j<vol.rem; j++) pack_buf[j+4] = ip_out.out_buf[(1400*(vol.quot))+j]; 
          Udp_Client_Send(&Uc41,pack_buf,(vol.rem + 4));
    }
          numb_pack ++;
  }   
}
