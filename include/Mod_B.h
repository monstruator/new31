//--- Ynoc K1 ---// 21.06.11
#include <unistd.h>
#include <sys/osinfo.h>
#include <time.h>
#include <process.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/dev.h>
#include "../include/my_pcs.h"
#include "../include/shared_mem.h"
#define TIMEOUT 500

//-------k2----
pid_t proxy_DRV2=0,pid,
	  proxy_RS; // Mod_B 
unsigned short Din_ModB[24];  
char masK2[]={0x55, 0x04, 0x03, 0x01, 0x5d};
int rez=0, err=0 ;
int Ptec,lvlK2;
unsigned char massiv[9];
//-------------

pid_t proxy_DRV1=0; 
int msg=25,rmsg,cntl_code=0,ii=1;
int chan1=8, chan2=2, chan_rd, chan_wr; // chan2 - связь со стендом 			// chan1 - связь с устройством (УПОС(8) или ПОСТ-3Ц(3))
//-----мои данные-----

void Init_ModB()
{
	int i;
// Проверка запуска драйвера модуля ПЦС ЦВС-3-1
	
	pid_drv=get_pid_process("PCS",0);
	if (!pid_drv) {
		printf("!!! Драйвер не запущен\n"); err++; rez=2;
		exit(rez);
	} 

	init_pcs_s.type=1;
	init_pcs_s.cnl=1;	
	init_pcs_s.speed=19200;
	init_pcs_s.b_info=8;
	init_pcs_s.b_StrStp=1;
	init_pcs_s.b_prt='E';
	init_pcs_s.dev=0; 
	init_pcs_s.lvl_inp=48;

	printf("pid_K1=%d\n",pid_drv);

	i=Send(pid_drv,&init_pcs_s,&init_pcs_r,sizeof(init_pcs_s),sizeof(init_pcs_r));
	if (i==-1) printf("ERROR\n");
	if (init_pcs_r.type==1) 
		if (init_pcs_r.status!=0) {
			printf("!!! \n");
			exit(-1);
		}
	start_pcs_s.type=2;
	start_pcs_s.cnl=1;
	start_pcs_s.ID_P=getpid();

	printf("my_id= %d\n",start_pcs_s.ID_P);
	Send(pid_drv,&start_pcs_s,&start_pcs_r,sizeof(start_pcs_s),sizeof(start_pcs_r));
	if (start_pcs_r.type==2) {
		if (start_pcs_r.status==-1) {
			printf("!!!ърэрыр1\n");
			exit(-1);
		} 
		else proxy_RS=start_pcs_r.Proxy;
	}       

}

void Write_ModB ()
{
	wr_cpcs_s.type=5;
	wr_cpcs_s.cnl=1;
	wr_cpcs_s.cnt=2;
	wr_cpcs_s.uom.dt[0]=0xab;
	wr_cpcs_s.uom.dt[1]=0xba;	
	Send(pid_drv,&wr_cpcs_s,&wr_cpcs_r,sizeof(wr_cpcs_s),sizeof(wr_cpcs_r));
} 

int Read_ModB ()
{	
	int i;
	rd_cpcs_s.type=4;
	rd_cpcs_s.cnl=1;
	Send(pid_drv,&rd_cpcs_s,&rd_cpcs_r,sizeof(rd_cpcs_s),sizeof(rd_cpcs_r));
//	printf ("From PCS_M rd_cpcs_r.type=%d rd_cpcs_r.status= %d\n",rd_cpcs_r.type,rd_cpcs_r.status);
//	printf ("rd_cpcs_s.cnl=%d   rd_cpcs_r.cnt= %d\n", rd_cpcs_s.cnl, rd_cpcs_r.cnt);
//	for(i=0;i<rd_cpcs_r.cnt;i++) printf(" %2x",rd_cpcs_r.uim.dt[i]); printf("\n");
	memcpy(&Din_ModB, &rd_cpcs_r.uim.dt, sizeof(Din_ModB));
	//for(i=0;i<24;i++) printf(" %d %4x",i, Din_ModB[i]); printf("\n");	
	return rd_cpcs_r.cnt; //вернем кол-во прочитанных байт
}