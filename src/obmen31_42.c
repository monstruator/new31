  #include <sys/types.h>
  #include <sys/socket.h>
  #include <sys/kernel.h>

  #include <netinet/in.h>
  #include <stdio.h>
  #include <unistd.h>
  #include <stdlib.h>
  #include <netdb.h>
  #include <math.h>
  #include "../include/func_IP.h"
  #include "../include/shared_mem.h"
  //#include "../include/gloriya.h"



  #define pi 	3.14159265
  #define Kncu 	12.27
  #define Kq 	0.5
  #define RAD 5729.57
  #define GRADtoRAD  0.00153398
  #define RADtoGRAD 651.898648
  #define SRC_PORT41 8208
  #define DST_PORT41 8208
  #define SRC_PORT42 8206
  #define DST_PORT42 8206


typedef struct  //обмен с задачей 31_42
{
	unsigned short Mispr; //сотояние аппаратуры прибора 3.1	
	short PR1[8];//данные первого прибора
	UPOS  upos;  //состояние УПОС
} toT42;

main()
{//--- KOHCTAHTbl npu6opa 1.0 ---//
 float 	C1,C2,C3,C4,C5,C6,C7,C8;
	  int sock, length, i , count_mes=0, i1 , i_p=0 , i2=0;
	  static Udp_Client_t Uc42;
	  char bufi[1024];
	  char bufo[1024];
	  char out_buf[1024];
	  obmen_42_31_t from42;	
	  obmen_31_42_t to42;	
	  simfonia41_t simfonia;	
	  int r,bytes,COM=0;
	  pid_t pid_CEP;
	  toT42 fromServ;
	  unsigned short Priem[48],SendToServ[18],buf;
	  float m_porog[10],m_sr,q_s,beta_s;
 C1=2048./pi;C2=4096.0/360.0;C3=180./pi;C4=C1*Kncu;
 C5=C2*Kncu;C6=C1*Kq;C7=C3;C8=C2*Kq;
///////////////
//поиск сервера
qnx_name_attach(0,"4.2");
//for(i=0;i<50;i++){pid_CEP=qnx_name_locate(0,"CEPBEP",0,NULL);if(pid_CEP>0) break;delay(100);}
//if(i==50) {printf("\nHET CEPBEP\n\n");fflush(stdout);}
//else {printf("\nECTb CEPBEP\n\n");fflush(stdout);}    

//инициализация канала UDP
	i = Udp_Client_Ini(&Uc42,"194.1.1.2",SRC_PORT42,DST_PORT42);
	printf(" Udp_Init=%d	\n", i);

	delay(3000);
	open_shmem();
	delay(1000);
	
//инициализация К1
	
	while(1)
	{
    bytes = Udp_Client_Read(&Uc42,bufi,4096);
	//printf(" Udp_READ=%d	\n", bytes);
	//for(i=0;i<28;i++) printf("%x ",bufi[i]);printf("\n");
	
	memcpy(&p->from42,&bufi[4],sizeof(obmen_42_31_t));

	//Состояние 1го прибора    
	buf=0;
	for(i1=0;i1<16;i1++) {buf+=((p->from42.M1>>i1)&1)<<(15-i1);} p->from42.M1=buf;buf=0;
	for(i1=0;i1<16;i1++) {buf+=((p->from42.M2>>i1)&1)<<(15-i1);} p->from42.M2=buf;buf=0;
	for(i1=0;i1<16;i1++) {buf+=((p->from42.M3>>i1)&1)<<(15-i1);} p->from42.M3=buf;buf=0;
	for(i1=0;i1<16;i1++) {buf+=((p->from42.M4>>i1)&1)<<(15-i1);} p->from42.M4=buf;buf=0;

	//printf("num_com2=%d\n",p->from42.num_com);
	//Для СЕРЕЖИ
	if (p->from42.num_com==9) p->toPR1[6]=0xFA30;

	//printf("cr_com=%x n_com=%x NKS=%d NSHKR=%x Nd_FR4=%d N_FR4=%x ZUNf=%x N_psp=%x Vr=%f Ar=%f\n",
	//p->from41.cr_com,p->from41.num_com,p->from41.num_KS,
	//p->from41.Nkey_SHAKR,p->from41.Nd_FRCH,p->from41.N_FRCH,
	//p->from41.ZUNf,p->from41.Nans_PSP,p->from41.Vr,p->from41.Ar);
		
//	printf(" %04x   %04x   %04x   %04x ",p->from42.M1,p->from42.M2,p->from42.M3,p->from42.M4);
//	printf(" CR=%d NC=%d \n",p->from42.cr_com,p->from42.num_com);
//  printf("\n");  
  
	p->to42.Ms1=p->PR1[3];   //состояние прибора 1.0
	p->to42.Ms2=p->PR1[4];
	p->to42.Ms3=p->PR1[5];
	p->to42.USign=p->PR1[6]; //уровень сигнала ПРД из сост ПР1.0

	p->to42.sum_K1=p->U.SUM_4;

	p->to42.D_K1=(float)p->U.DPL_1*244.14;

	//printf("DPL=%x\n",p->U.DPL_1);
    //УГЛЫ
	p->to42.q=(p->PR1[0]-1991)*2/RADtoGRAD; //АЗИМУТ
	if (p->PR1[2]&0x800) p->to42.beta=(360-p->PR1[2]/C2)/C3; //УГОЛ МЕСТА
	else p->to42.beta=-p->PR1[2]/C1;
	if (p->PR1[1]&0x800) p->to42.alfa=(p->PR1[1]-0xFFF)/12.27/RADtoGRAD;//КРЕН
	else p->to42.alfa=p->PR1[1]/C4; 

	for(i1=0;i1<16;i1++) {buf+=((p->to42.Ms1>>i1)&1)<<(15-i1);} p->to42.Ms1=buf;buf=0;
	for(i1=0;i1<16;i1++) {buf+=((p->to42.Ms2>>i1)&1)<<(15-i1);} p->to42.Ms2=buf;buf=0;
	for(i1=0;i1<16;i1++) {buf+=((p->to42.Ms3>>i1)&1)<<(15-i1);} p->to42.Ms3=buf;buf=0;

//	printf("1=%04x 2=%04x 3=%04x\n",Priem[1],Priem[2],Priem[3]);
//	for(i1=0;i1<16;i1++) {buf+=((Priem[7]>>i1)&1)<<(15-i1);} Priem[8]=buf;buf=0;

	p->to42.Ms1=p->to42.Ms1|0x5;   //состояние прибора 1.0
//    printf("%04x ",p->to42.Ms1);
// printf("%04x ",p->to42.Ms2);
// printf("%04x ",p->to42.Ms3);printf("\n");

//	printf(" q=%8.4f a=%8.4f b=%8.4f ",p->to42.q,p->to42.alfa,p->to42.beta);
//	p->to42.q=p->to42.alfa=p->to42.beta=1;
    
    memcpy(out_buf,&p->to42,sizeof(obmen_31_42_t));
  
    bufo[0]=80;
    bufo[1]=count_mes++;
    bufo[2]=bufo[3]=1;
    for(i=0;i<sizeof(obmen_31_42_t);i++) bufo[i+4]=out_buf[i]; 

     i = Udp_Client_Send(&Uc42,bufo,(sizeof(obmen_31_42_t)+4));

	//	 printf("Send i = %d\n ",i);
	 //	printf("\n=====> ");for(i=0;i<28;i++) printf(" %d ", bufo[i]);
	}   
}
