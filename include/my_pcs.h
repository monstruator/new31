#include <sys/vc.h>
#include <sys/kernel.h>
#include <sys/dev.h>
#include <time.h>
#include <fcntl.h>
#include <sys/psinfo.h>
#include <unistd.h>
#include <sys/proxy.h>

#include "pcs_m.h"
#include "pcs_mm.h"

#define CVS_NODE 9
#define CVS_NODE_STEND 2
#define TIME 3
//#define PARENT_NODE 1
//#define IND_BU 0x4255  //От ts_usoi_pr_BU
//#define IND_R999 0x9099 //От ts_usoi_pr_P

struct _psinfo2 my_info;
INIT_PCS_s init_pcs_s;  //Сообщение-запрос при инициализации канала ПЦС
INIT_PCS_r init_pcs_r;  //Сообщение-ответ при инициализации канала ПЦС
START_PCS_s start_pcs_s;//Сообщение-запрос при старте канала ПЦС
START_PCS_r start_pcs_r;//Сообщение-ответ при старте канала ПЦС
WR_CPCS_s wr_cpcs_s;  //Сообщение-запрос при выдаче данных в канал 
WR_CPCS_r wr_cpcs_r;  //Сообщение-ответ при выдаче данных в канал 
RD_CPCS_s rd_cpcs_s;  //Сообщение-запрос для получения данных из канала
RD_CPCS_r rd_cpcs_r;  //Сообщение-ответ при получении данных из канала
RST_CPCS_s rst_cpcs_s; //Сообщение - запрос на сброс канала модуля
RST_CPCS_r rst_cpcs_r; //Сообщение - ответ про сброс канала модуля

int prizn_cmd,prizn_fled; 
int pid_drv;

int get_pid_process(char *parent_name,nid_t node) {
int prizn=0;
pid_t m_id=1,proxy,vid,m_id1;

	prizn=qnx_name_locate(node,parent_name,0,NULL);
	if (prizn > 0){ 
	 printf("My parent name %s pid %d\n",parent_name,prizn);
	return (prizn);
			  }
	return (0);
		}


unsigned char work_msrs0_write(int cmd,UN int cmd_aring,UN t_out)
	{
int i;
	if (cmd==OFFRadio || cmd==ONRadio || STInpMd || cmd==ONT_IO || 
	cmd==OFFT_IO || cmd==RdVMd || RC_ARING)
 	{wr_cpcs_s.uom.Wr_m.cmd=cmd; wr_cpcs_s.cnt=1;}

	if (cmd>=TLED0 && cmd<=TLED7) {wr_cpcs_s.uom.Wr_m.st_2N.cmd=cmd;wr_cpcs_s.uom.Wr_m.st_2N.ps=0;
								   if (!prizn_fled) wr_cpcs_s.uom.Wr_m.st_2N.q_fled=0x10;
								   else wr_cpcs_s.uom.Wr_m.st_2N.q_fled=0; 
									   wr_cpcs_s.cnt=3;wr_cpcs_s.cnt=3;
								  }
	if (cmd==OC_ARING) {wr_cpcs_s.uom.Wr_m.st_40.cmd=cmd;
						wr_cpcs_s.uom.Wr_m.st_40.cmd_AO=cmd_aring;
						wr_cpcs_s.cnt=5;
//	for (i=0;i<5;i++) printf ("aring %x\n",((char *)&wr_cpcs_s.uom.Wr_m.st_40)[i]);
						}
	if (cmd==T_IO) {wr_cpcs_s.uom.Wr_m.st_C0.cmd=cmd;wr_cpcs_s.uom.Wr_m.st_C0.t_out=t_out; 
					wr_cpcs_s.cnt=2;
				   }

	wr_cpcs_s.type=W_MSRS0; wr_cpcs_s.cnl=MSRS0; 
	//Передача команды в МСРС
	Send(pid_drv,&wr_cpcs_s,&wr_cpcs_r,sizeof(wr_cpcs_s),sizeof(wr_cpcs_r));				
	if (wr_cpcs_r.type==W_MSRS0) return wr_cpcs_r.status;
	if (wr_cpcs_r.type==6 && wr_cpcs_r.status==-1) return (2); 
	return (1);  //Если в Сообщении-ответе при выдаче данных в канал
				 //иное значение wr_cpcs_r.type  
 
	}
unsigned char work_msrs1_write(int cmd,char *dt,int size)
{
int i;
	if (cmd==FillBuf)
		{
		 wr_cpcs_s.uom.Wr_m.st1_80.cmd=cmd;
		 if (size==256) wr_cpcs_s.uom.Wr_m.st1_80.sz=0; 
		 else wr_cpcs_s.uom.Wr_m.st1_80.sz=size;
		 memcpy(wr_cpcs_s.uom.Wr_m.st1_80.dt,dt,size);wr_cpcs_s.cnt=size+1;
		} else
	if (cmd==SpTr) {wr_cpcs_s.uom.Wr_m.st1_C0.cmd=cmd;wr_cpcs_s.uom.Wr_m.st1_C0.spd=*dt;wr_cpcs_s.cnt=2;}
	else 
	if (cmd==TrRadio) {wr_cpcs_s.uom.Wr_m.cmd=cmd;wr_cpcs_s.cnt=1;} 
	else
	if (cmd==StrB) {wr_cpcs_s.uom.Wr_m.st1_B0.cmd=cmd;
	wr_cpcs_s.uom.Wr_m.st1_B0.prmb=*dt;
	wr_cpcs_s.uom.Wr_m.st1_B0.strb=*(dt+1);wr_cpcs_s.cnt=3;
	printf("PREAM %x STB %x\n",wr_cpcs_s.uom.Wr_m.st1_B0.prmb,wr_cpcs_s.uom.Wr_m.st1_B0.strb);
				   }

	wr_cpcs_s.type=W_MSRS1; wr_cpcs_s.cnl=MSRS1; 
	
	//Передача команды в МСРС
	Send(pid_drv,&wr_cpcs_s,&wr_cpcs_r,sizeof(wr_cpcs_s),sizeof(wr_cpcs_r));				
	printf("wr_cpcs_r.type=%d\n",wr_cpcs_r.type); 
	if (wr_cpcs_r.type==W_MSRS1) return wr_cpcs_r.status;

	if (wr_cpcs_r.type==6 && wr_cpcs_r.status==-1) return (2); 
	return (1);  //Если в Сообщении-ответе при выдаче данных в канал
				 //иное значение wr_cpcs_r.type  
}		
char work_msrs0_read(pid_t proxy_DRV, UN char cmd_last )//UN char cmd_last - ранее выданная команда
	{
int St_time,Fin_time,St_time_all,get_proxy=0;
	prizn_cmd=0;
				//Ожидание 
St_time_all=clock();
while(1){
				St_time=Fin_time=clock();
	 			while ((Fin_time - St_time)/CLOCKS_PER_SEC < TIME) {
				Fin_time=clock();
				if (Creceive(0,0,0)==proxy_DRV){get_proxy=1;break;}
																    } //while time	
	if (!get_proxy) return(3);
	//Передача запроса для получения данных от МСРС0
	rd_cpcs_s.type=RD_CPCS;
	rd_cpcs_s.cnl=MSRS0;
	Send(pid_drv,&rd_cpcs_s,&rd_cpcs_r,sizeof(rd_cpcs_s),sizeof(rd_cpcs_r));	
	printf("Read MSRS %d status=%d\n",rd_cpcs_r.type,rd_cpcs_r.status); 			
	if (cmd_last==rd_cpcs_r.uim.Rd_m.cmd || cmd_last==rd_cpcs_r.uim.Rd_m.st_E0.cmd || 
		cmd_last==rd_cpcs_r.uim.Rd_m.st_30.cmd || cmd_last==rd_cpcs_r.uim.Rd_m.st_41.cmd) break;
		Fin_time=clock();
if (((Fin_time - St_time_all)/CLOCKS_PER_SEC)  > TIME*20) break;
		} //while  
	if (rd_cpcs_r.type==R_MSRS0 && rd_cpcs_r.status==0 ) {
		prizn_cmd++;
		switch(cmd_last){
		case OFFRadio: 
		case ONRadio: 
		case ONT_IO:
		case OC_ARING:
        case YDt_ARING:
		case OFFT_IO: return rd_cpcs_r.uim.Rd_m.cmd; 
		case RdVMd:   return rd_cpcs_r.uim.Rd_m.st_E0.cmd;
		case STInpMd: return rd_cpcs_r.uim.Rd_m.st_30.cmd;
		case RC_ARING:return rd_cpcs_r.uim.Rd_m.st_41.cmd; 
						}                               }
	else return rd_cpcs_r.status;
 	return (0);
	}
char work_msrs1_read(pid_t proxy_DRV, UN char cmd_last )//UN char cmd_last - ранее выданная команда
	{
int St_time,Fin_time,get_proxy=0,i,rez;
	prizn_cmd=0;
	//Ожидание 
	printf("Wait Proxy %d \n",proxy_DRV);
	St_time=Fin_time=clock();
	while ((Fin_time - St_time)/CLOCKS_PER_SEC < TIME*5) {
	Fin_time=clock();

	if ((rez=Creceive(0,0,0))==proxy_DRV){get_proxy=1; printf("Proxy OK %d \n",rez);break;}
	if (rez!=-1) printf("Proxy %d \n",rez);
													    } //while time	
	if (!get_proxy) return(3);
	//Передача запроса для получения данных от МСРС1
	rd_cpcs_s.type=RD_CPCS;
	rd_cpcs_s.cnl=MSRS1;
	Send(pid_drv,&rd_cpcs_s,&rd_cpcs_r,sizeof(rd_cpcs_s),sizeof(rd_cpcs_r));	
	printf("Read MSRS %d status=%d\n",rd_cpcs_r.type,rd_cpcs_r.status); 
/*
	if(cmd_last==RdBuf)	{
					printf("BEGIN\n");
					for(i=0;i<256;i++) printf("%x ",rd_cpcs_r.uim.Rd_m.st1_90.dt[i]);
					printf("END\n");		
						}
*/
	if (rd_cpcs_r.type==R_MSRS1 && rd_cpcs_r.status==0) {
		prizn_cmd++;
		switch(cmd_last){
		case TrRadio:
		case StrB:
		case SpTr:
		case YDtST2:
		case FillBuf:return rd_cpcs_r.uim.Rd_m.cmd; 
		case RdBuf:printf("BEGIN\n");
					for(i=0;i<256;i++) printf("%x ",rd_cpcs_r.uim.Rd_m.st1_90.dt[i]);
					printf("\nEND %d\n",i);
                   return rd_cpcs_r.uim.Rd_m.st1_90.cmd;
		case RdVMd:return rd_cpcs_r.uim.Rd_m.st_E0.cmd;
						}                               }
	else return rd_cpcs_r.status;
 	return (0);
	}

short 
work_bu_write(int cmd_dt,int po_type, int cnl_bu, UN char *point) {
//int cmd_dt - код пакета управления или кол-во байт данных при ретрансляции
//int po_type - Пакеты управления или Пакеты ретрансляции
//int cnl_bu - канал БУ1 или БУ2
//UN char *point - адрес массива данных 
int St_time,Fin_time,i;

	wr_cpcs_s.type=po_type;
	
//Запись в БУ пакетов управления или ретрансляции
//Код пакета управления соответствует системе команд модулей БУ
	if (po_type==MSG_WBU_P) { wr_cpcs_s.uom.st_cpo.cd_cmd=cmd_dt;  
							  if (point) 
							  memcpy(&wr_cpcs_s.uom.st_cpo.im.d2,point,2);
							  wr_cpcs_s.cnt=3;
printf("cmd =%x oper=%x data= %x\n",wr_cpcs_s.uom.st_cpo.cd_cmd,wr_cpcs_s.uom.st_cpo.im.d2[0],wr_cpcs_s.uom.st_cpo.im.d2[1]);							  
							}
	else
//Количество байт при ретрансляции
	if (po_type==MSG_WBU_R) { 
						      wr_cpcs_s.uom.st_rpo.sz_dt=cmd_dt; 
							  if (point) 
							  memcpy(&wr_cpcs_s.uom.st_rpo.dt,point,cmd_dt);
							  wr_cpcs_s.cnt=cmd_dt+1;
//							  wr_cpcs_s.cnt=cmd_dt;
printf("num of data =%x data=%x data= %x data= %x data %x\n",
wr_cpcs_s.uom.st_rpo.sz_dt,wr_cpcs_s.uom.st_rpo.dt[0],wr_cpcs_s.uom.st_rpo.dt[1],
wr_cpcs_s.uom.st_rpo.dt[2],wr_cpcs_s.uom.st_rpo.dt[3]);							  
							}
//Номер канала БУ1 или БУ2
	wr_cpcs_s.cnl=cnl_bu; 
	if (point==NULL) {wr_cpcs_s.cnt=1;
					  for (i=0;i<2;i++) wr_cpcs_s.uom.st_cpo.im.d2[i]=0;
					  for (i=0;i<28;i++) wr_cpcs_s.uom.st_rpo.dt[i]=0;
printf("num of data =%x data=%x data= %x data= %x\n",
wr_cpcs_s.uom.st_rpo.sz_dt,wr_cpcs_s.uom.st_rpo.dt[0],wr_cpcs_s.uom.st_rpo.dt[1],wr_cpcs_s.uom.st_rpo.dt[2]);							  
						 }
//Передача команды
	Send(pid_drv,&wr_cpcs_s,&wr_cpcs_r,sizeof(wr_cpcs_s),sizeof(wr_cpcs_r));
	printf ("From PCS_M wr_cpcs_r.type=%d wr_cpcs_r.status= %d\n",wr_cpcs_r.type,wr_cpcs_r.status);
	fflush(stdout);
	if (wr_cpcs_r.type==MSG_WBU_P || wr_cpcs_r.type==MSG_WBU_R) 				
	return wr_cpcs_r.status;
	if (wr_cpcs_r.type==6 && wr_cpcs_r.status==-1) return (2); 
	return (1);  //Если в Сообщении-ответе при выдаче данных в канал
				 //иное значение wr_cpcs_r.type  
	}


short 
work_bu_read(int cnl_bu, pid_t proxy_BU, UN char *point) {
//int cnl_bu - канал БУ1 или БУ2
//UN char *point - адрес массива , в который надо поместить данные от БУ  
int St_time,Fin_time,get_proxy=0;
int msg;
	//Ожидание прокси от драйвера 
	printf("proxy_BU =%d\n",proxy_BU);
	St_time=Fin_time=clock()/CLOCKS_PER_SEC;
	while (Fin_time - St_time < TIME){ 
	if (proxy_BU==Creceive(0,&msg,sizeof(msg))) {get_proxy=1; break;}
	Fin_time=clock()/CLOCKS_PER_SEC;
										}
	//Передача запроса для получения данных от БУ
	if (!get_proxy) return (2);       //Вышло время ожидания ответа
	rd_cpcs_s.type=RD_CPCS;
	rd_cpcs_s.cnl=cnl_bu;
rd_cpcs_r.status=0xAA;
	Send(pid_drv,&rd_cpcs_s,&rd_cpcs_r,sizeof(rd_cpcs_s),sizeof(rd_cpcs_r));				
	printf ("From PCS_M rd_cpcs_r.type=%d rd_cpcs_r.status %d\n",rd_cpcs_r.type,rd_cpcs_r.status);
	fflush(stdout);
	if (rd_cpcs_r.type==MSG_PBU_P)
	printf("Rez cd_cmd %x d2[0] %x d2[1] %x\n",	rd_cpcs_r.uim.st_cpp.cd_cmd,
	rd_cpcs_r.uim.st_cpp.im.d2[0],rd_cpcs_r.uim.st_cpp.im.d2[1]);
	fflush(stdout);
	if (rd_cpcs_r.type==MSG_PBU_R)
	printf("Rez err %x sz_dt %x \n",	rd_cpcs_r.uim.st_rpp.err,
	rd_cpcs_r.uim.st_rpp.sz_dt);
	fflush(stdout);

	if (rd_cpcs_r.type==MSG_PBU_P && rd_cpcs_r.status==0) 
	memcpy(point,rd_cpcs_r.uim.st_cpp.im.d2,2);

	if (rd_cpcs_r.type==MSG_PBU_R && rd_cpcs_r.status==0) 
	memcpy(point,rd_cpcs_r.uim.st_rpp.dt,rd_cpcs_r.uim.st_rpp.sz_dt);

	if (rd_cpcs_r.type==MSG_PBU_P)  {
		if (rd_cpcs_r.uim.st_cpp.cd_cmd != 0x66) return rd_cpcs_r.status; 
		else return(3);     		}
	if (rd_cpcs_r.type==MSG_PBU_R)  {
		if (!rd_cpcs_r.uim.st_rpp.err) return rd_cpcs_r.status; 
		else return(3);     		}

	return (1);  //Если в Сообщении-ответе при чтении данных из канала
				 //иное значение rd_cpcs_r.type  

	}