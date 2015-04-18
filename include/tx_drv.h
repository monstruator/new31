//tx_drv.h
//файл tx_drv.h подключается к пользовательской программе. 
#include <stdio.h>
#include <fcntl.h>
#include <sys/kernel.h>
#include <sys/wait.h>
#include <conio.h>
#include <unistd.h>
#include <sys/types.h>
#include <process.h>
#include <time.h>
#include <math.h>
#include <sys/mouse.h>
#include <i86.h>
#include <ctype.h>
#include <sys/proxy.h>
#include <sys/sched.h>
#include <sys/irqinfo.h>
#include <sys/name.h>
#include <sys/seginfo.h>
#include <sys/inline.h>
#include <sys/vc.h>
#include <stdlib.h>
#include <malloc.h>

#include "tx_common.h"

//FILE *f;
//структура для формирования команды КК...
struct DefCMD
{
 unsigned short cmd; //команда КК
 unsigned short *d;  //- указатель на массив слов данных (в случае,
                     //  если оконечник принимает данные) 
					 // - NULL (в случае, если оконечник передает данные)
};
//--------------------------------------------------
//unsigned short tx_B[N_B];
//pid_t tx_pid,proxyR[3]={0,0,0},proxyERR[3]={0,0,0},proxyMODE[3]={0,0,0};
typedef struct
{
 unsigned short tx_B[N_B];
 pid_t tx_pid;
 pid_t proxyR[3];
 pid_t proxyERR[3];
 pid_t proxyMODE[3];
 pid_t proxyOUT[3];
 unsigned pci_index;
} dev_tx_t;

dev_tx_t* OpenTx(unsigned pci_index)
{
 dev_tx_t *dev;
 int i;
 dev=(dev_tx_t*)malloc(sizeof(dev_tx_t));
 if(dev!=NULL)
 {
  dev->tx_pid=0;
  dev->pci_index=pci_index;
  for(i=0;i<3;i++)
  {	
    dev->proxyR[i]=0;
    dev->proxyERR[i]=0;
    dev->proxyMODE[i]=0;
    dev->proxyOUT[i]=0;
  }
 } 
 return(dev);
}

void CloseTx(dev_tx_t* dev)
{
 int i;
  for(i=0;i<3;i++)
  {	
    if(dev->proxyR[i]!=0)
		qnx_proxy_detach(dev->proxyR[i]);
    if(dev->proxyERR[i]!=0)
		qnx_proxy_detach(dev->proxyERR[i]);
    if(dev->proxyMODE[i]!=0)
		qnx_proxy_detach(dev->proxyMODE[i]);
    if(dev->proxyOUT[i]!=0)
		qnx_proxy_detach(dev->proxyOUT[i]);
  }

 if(dev!=NULL)
 {
  free(dev);
 }
}
//Установка признаков, передаваемых ОУ в ответном слове...
//dev - указатель на структуру типа dev_tx_t
//nm - номер канала
//aw - слово, содержащее признаки ответного слова
//возвращаемые значения:
/*
-1 -ОШИБКА
 0 -НОРМА
 */
int SetAnswer(dev_tx_t* dev, int nm, unsigned short aw)
{
 int ret=0;
 if(dev!=NULL) 
 {
  dev->tx_B[0]=ANSWER; dev->tx_B[1]=nm; dev->tx_B[2]=aw;
  Send(dev->tx_pid,dev->tx_B,dev->tx_B,3*2,N_B*2); 
  ret=dev->tx_B[0];
 }//if(dev!=NULL)
 else
 {
  printf("ОШИБКА! SetAnswer (dev=NULL)\n"); 
  fflush(stdout);
  ret=-1; 
 }
 return(ret);
}

//установка режима ОУ...
//параметры:
//dev - указатель на структуру типа dev_tx_t
//nm - номер канала
//ou - адрес ОУ
//r=true - прерывания от приема оконечником разрешены
//r=false - прерывания от приема оконечником запрещены
//возвращаемые значения:
/*
-1 -ОШИБКА
 0 -НОРМА
 */
int regim_ou ( dev_tx_t* dev, int nm, int ou, int r )
{
 int ret=0;
 int i=0;
 char name[80];
 if(dev!=NULL)
 {
  sprintf(name,"/potok_%02d",dev->pci_index);
  if((nm!=0)&&(nm!=1)&&(nm!=2))
   { 
    printf("Номер канала=%d?  допустимые значения =0,1,2\n",nm);
	fflush(stdout);
	return(-1); 
   }
  while((dev->tx_pid<=0)&&(i<1000))
   {
    dev->tx_pid=qnx_vc_name_attach(0,100,name); i++; 
   }
  if(dev->tx_pid<=0)
   { 
    printf("ОШИБКА! regim_ou (подсоединение к tx_drv)\n"); 
	fflush(stdout);
	return(-1); 
   }

  if(r)
  {
  	if(dev->proxyR[nm]==0)
	    dev->proxyR[nm]=qnx_proxy_attach(0,0,0,-1);
  	if(dev->proxyMODE[nm]==0)
	    dev->proxyMODE[nm]=qnx_proxy_attach(0,0,0,-1);
  	if(dev->proxyOUT[nm]==0)
	    dev->proxyOUT[nm]=qnx_proxy_attach(0,0,0,-1);
  }

//
    dev->tx_B[3]=dev->proxyR[nm];
	dev->tx_B[4]=dev->proxyOUT[nm];
	dev->tx_B[5]=dev->proxyMODE[nm];

  dev->tx_B[0]=REGIM; dev->tx_B[1]=nm; dev->tx_B[2]=(ou|REG_OU);
  Send(dev->tx_pid,dev->tx_B,dev->tx_B,6*2,N_B*2); 
  ret=dev->tx_B[0];
 }//if(dev!=NULL)
 else
 {
  printf("ОШИБКА! regim_ou (dev=NULL)\n"); 
  fflush(stdout);
  ret=-1; 
 }
 return(ret);
}
//--------------------------------------------------

//установка режима КК...
//параметры:
//dev - указатель на структуру типа dev_tx_t
//nm - номер канала
//p=true - прерывания по завершению выполнения команд разрешены
//p=false - прерывания по завершению выполнения команд запрещены
//возвращаемые значения:
/*
-1 -ОШИБКА
 0 -НОРМА
 */
int regim_kk( dev_tx_t* dev, int nm, int p )
{
 int ret=0;
 int i=0;
 char name[80];
 if(dev!=0)
 {
  sprintf(name,"/potok_%02d",dev->pci_index);
  if((nm!=0)&&(nm!=1)&&(nm!=2))
   { 
    printf("Номер канала=%d?  допустимые значения =0,1,2\n",nm); 
	fflush(stdout);
	return(-1); 
   }
  while((dev->tx_pid<=0)&&(i<1000))
   {
    dev->tx_pid=qnx_vc_name_attach(0,100,name); i++;
   }
  if(dev->tx_pid<=0)
   { 
    printf("ОШИБКА! regim_kk (подсоединение к tx_drv)\n"); 
	fflush(stdout);
	return(-1); 
   }
  if(p)
  {
  	if(dev->proxyR[nm]==0)
	    dev->proxyR[nm]=qnx_proxy_attach(0,0,0,-1);
  	if(dev->proxyERR[nm]==0)
	    dev->proxyERR[nm]=qnx_proxy_attach(0,0,0,-1); 
  }
//
  dev->tx_B[3]=dev->proxyR[nm]; dev->tx_B[4]=dev->proxyERR[nm];

  dev->tx_B[0]=REGIM; dev->tx_B[1]=nm; dev->tx_B[2]=0;
  Send(dev->tx_pid,dev->tx_B,dev->tx_B,5*2,N_B*2);
  ret=dev->tx_B[0];
 }//if(dev!=NULL)
 else
 {
  printf("ОШИБКА! regim_kk (dev=NULL)\n"); 
  fflush(stdout);
  ret=-1; 
 } 
 return(ret);
}
//--------------------------------------------------

//формирование сообщения для реализации чтения из ОУ или записи в ОУ...
//параметры:
//dev - указатель на структуру типа dev_tx_t
//nm - номер канала
//u=RT_READ, RT_INF_OUT - чтение из ОУ
//u=RT_WRITE - запись в ОУ
//pa - подадрес  
//dl - количество слов 
//D - указатель на массив слов данных (NULL - в случае чтения из оконечника)
//возвращаемые значения:
/*
-1 -ОШИБКА
 0 -НОРМА
 */
int tx_send( dev_tx_t* dev, int nm, char u, int pa, int dl, unsigned short D[] )
{
 int ret=0;
 int i;
 if(dev!=0)
 {
  if(dev->tx_pid<=0)
   {
    printf("Нет доступа к tx_drv"); 
	fflush(stdout);
	return(-1); 
   }
  if((pa<(((u==RT_READ)||(u==RT_INF_OUT))?0:1))||(pa>=31))
   { 
    printf("Неверное значение подадреса=%d\n",pa); 
	return(-1); 
   }
  if(D!=NULL)
   if((dl<1)||(dl>32))
   { 
    printf("Длина=%d?  допустимые значения=1-32\n",dl); 
	fflush(stdout);
	return(-1); 
   }
  dev->tx_B[0]=u; dev->tx_B[1]=nm; dev->tx_B[2]=pa; dev->tx_B[3]=dl;
  if(u==RT_WRITE)
   for(i=0;i<dl;i++) dev->tx_B[4+i]=D[i];
  Send(dev->tx_pid,dev->tx_B,dev->tx_B,(4+i)*2,N_B*2);
  ret=dev->tx_B[0]; 
 }//if(dev!=NULL)
 else
 {
  printf("ОШИБКА! tx_send (dev=NULL)\n"); 
  fflush(stdout);
  ret=-1; 
 }
 return(ret);
}
//--------------------------------------------------
//запуск "фрейма команд"...
//параметры:
//dev - указатель на структуру типа dev_tx_t
//nm - номер канала (nm|RBUS -в случае резервного канала)
//n - количество элементов в массиве структур типа "struct DefCMD" 
//    ("n" равно количеству команд во "фрейме")
//p - указатель на 1-ый элемент массива структур типа "struct DefCMD"
//    p[i].cmd - BC COMMAND
//	  p[i].D   - указатель на 1-ый элемент массива слов данных

//возвращаемые значения:
/*
-1 -ОШИБКА
 0 -НОРМА
В случае работы без прерывания:
tx_B[0]    - для передачи данных не используется
tx_B[1]      - количество команд во "фрейме"
tx_B[2]      - BC CONTROL WORD   | - 1-ая команда
tx_B[3]      - BC COMMAND        |
tx_B[4]      - STATUS WORD       |
tx_B[5]      - количество слов   |
               данных            |
               (0 - в командах   |
			    приема данных    |
			    оконечником)     |
tx_B[6]...   - DATA WORDS        | 		

tx_B[X]      - BC CONTROL WORD   |- 2-ая команда
tx_B[X+1]    - BC COMMAND        |
tx_B[X+2]    - STATUS WORD       |
tx_B[X+3]    - количество слов   |
               данных 
               (0 - в командах   |
			    приема данных    |
			    оконечником)     |
tx_B[X+4]... - DATA WORDS        |

.............................
                                 |- n-ая команда 

 */
int KK_frame( dev_tx_t* dev, int nm, int n, struct DefCMD *p)
{
 int ret=0;
 unsigned short *q,cmd,*d;
 int i,j,len;
 if(dev!=NULL)
 {
  if(dev->tx_pid<=0)
   {
    printf("Нет доступа к tx_drv"); 
	fflush(stdout);
	return(-1); 
   }
  dev->tx_B[0]=BC_FRAME; dev->tx_B[1]=nm; dev->tx_B[2]=n;q=&(dev->tx_B[3]);
  for(i=0;i<n;i++)
   {
    cmd=*q++=p->cmd;
	if(!(cmd&0x400))
	  {//RECEIVE
	   len=cmd&0x1F;
	   if(len==0)
	     len=32;
	   d=p->d;
	   for(j=0;j<len;j++)
	     {
		  *q++=*d++;
		 } 
	  }
    p++;
   }  
  i=(q-dev->tx_B)*2;

  Send(dev->tx_pid,dev->tx_B,dev->tx_B,i,N_B*2);
  ret=dev->tx_B[0];  
 }//if(dev!=NULL)
 else
 {
  printf("ОШИБКА! KK_frame (dev=NULL)\n"); 
  fflush(stdout);
  ret=-1; 
 }
 return(ret);
}
//--------------------------------------------------
//получение результата по "n" комондам от начала "фрейма" в случае
//работы КК по прерыванию...
//параметры:
//dev - указатель на структуру типа dev_tx_t
//nm - номер канала
//n - количество команд от начала фрейма, по которым надо получить  
//    результат

//возвращаемые значения:
/*
-1 -ОШИБКА
 0 -НОРМА

tx_B[0]    - для передачи данных не используется
tx_B[1]      - количество команд во "фрейме"
tx_B[2]      - BC CONTROL WORD   | - 1-ая команда
tx_B[3]      - BC COMMAND        |
tx_B[4]      - STATUS WORD       |
tx_B[5]      - количество слов   |
               данных            |
               (0 - в командах   |
			    приема данных    |
			    оконечником)     |
tx_B[6]...   - DATA WORDS        | 		

tx_B[X]      - BC CONTROL WORD   |- 2-ая команда
tx_B[X+1]    - BC COMMAND        |
tx_B[X+2]    - STATUS WORD       |
tx_B[X+3]    - количество слов   |
               данных 
               (0 - в командах   |
			    приема данных    |
			    оконечником)     |
tx_B[X+4]... - DATA WORDS        |

.............................
                                 |- n-ая команда 
 */								 
int KK_end( dev_tx_t* dev, int nm, int n )
{
 int ret=0;
 if(dev!=NULL)
 {
  if(dev->tx_pid<=0)
   {
    printf("Нет доступа к tx_drv"); 
	fflush(stdout);
	return(-1); 
   }
  dev->tx_B[0]=BC_RESULT; dev->tx_B[1]=nm;dev->tx_B[2]=n;
  Send(dev->tx_pid,dev->tx_B,dev->tx_B,3*2,N_B*2);
  ret=dev->tx_B[0];  
 }//if(dev!=NULL)
 else
 {
  printf("ОШИБКА! KK_end (dev=NULL)\n"); 
  fflush(stdout);
  ret=-1; 
 }
 return(ret);
}
//--------------------------------------------------
//получение результата по команде с номером "ncmd" от начала "фрейма"
//в случае работы КК по прерыванию...
//параметры:
//dev - указатель на структуру типа dev_tx_t
//nm - номер канала
//ncmd - номер команды от начала фрейма, по которой надо получить  
//       результат

//возвращаемые значения:
/*
-1 -ОШИБКА
 0 -НОРМА

tx_B[0]    - для передачи данных не используется
tx_B[1]      - BC CONTROL WORD   
tx_B[2]      - BC COMMAND        
tx_B[3]      - STATUS WORD       
tx_B[4]      - количество слов   
               данных            
               (0 - в командах   |
			    приема данных    |
			    оконечником)     |
tx_B[5]...   - DATA WORDS         		
 */
int KK_end_CMD( dev_tx_t* dev, int nm, int ncmd )
{
 int ret=0;
 if(dev!=NULL)
 {
  if(dev->tx_pid<=0)
   {
    printf("Нет доступа к tx_drv"); 
	fflush(stdout);
	return(-1); 
   }
  dev->tx_B[0]=BC_RESULT_CMD; dev->tx_B[1]=nm;dev->tx_B[2]=ncmd;
  Send(dev->tx_pid,dev->tx_B,dev->tx_B,3*2,N_B*2);
  ret=dev->tx_B[0];  
 }//if(dev!=NULL)
 else
 {
  printf("ОШИБКА! KK_end_CMD (dev=NULL)\n"); 
  fflush(stdout);
  ret=-1; 
 }
 return(ret);
}

//--------------------------------------------------
//запись в оконечник...
//параметры:
//dev - указатель на структуру типа dev_tx_t
//nm - номер канала
//pa - подадрес  
//dl - количество слов 
//D - указатель на массив слов данных 
//возвращаемые значения:
/*
-1 -ОШИБКА
 0 -НОРМА
 */	   
int ou_write( dev_tx_t* dev, int nm, int pa, int dl, unsigned short D[] )
{
 int ret=0;
 if(dev!=NULL)
 {
  ret=tx_send(dev,nm,RT_WRITE,pa,dl,D);
 }//if(dev!=NULL)
 else
 {
  printf("ОШИБКА! ou_write (dev=NULL)\n"); 
  fflush(stdout);
  ret=-1; 
 }
 return(ret);
}
//--------------------------------------------------
//чтение из оконечника...
//параметры:
//dev - указатель на структуру типа dev_tx_t
//nm - номер канала
//pa - подадрес (если pa=0, то чтение из очередного подадреса,
//     по которому пришли данные) 
//возвращаемые значения:
/*
-1 -ОШИБКА
-2 -ОШИБКА (ОУ не принимало данных или они уже были прочитаны ранее
           программой пользователя)
 0 -НОРМА

tx_B[0]    - для передачи данных не используется
tx_B[1]    - подадрес
tx_B[2]    - RT DATA TABLE TAG WORD
tx_B[3]    - количество слов данных
tx_B[4]... - DATA WORDS 		
 */

int ou_read( dev_tx_t* dev, int nm, int pa )
{
 int ret=0;
 if(dev!=0)
 {
  ret=tx_send(dev,nm,RT_READ,pa,0,0); 
 }//if(dev!=NULL)
 else
 {
  printf("ОШИБКА! ou_read (dev=NULL)\n"); 
  fflush(stdout);
  ret=-1; 
 }
 return(ret);
}
//--------------------------------------------------
//Получение информации о передаче данных из оконечника...
//параметры:
//dev - указатель на структуру типа dev_tx_t
//nm - номер канала
//pa - подадрес, по которому надо получить информацию 
//     (если pa=0, то получение информации 
//               о передаче данных из очередного подадреса) 
//возвращаемые значения:
/*
-1 -ОШИБКА
-2 -ОШИБКА (Данные еще не передавались)
 0 -НОРМА

tx_B[0]    - для передачи данных не используется
tx_B[1]    - подадрес
tx_B[2]    - RT DATA TABLE TAG WORD
 */

int ou_inf_out( dev_tx_t* dev, int nm, int pa )
{
 int ret=0;
 if(dev!=0)
 {
  ret=tx_send(dev,nm,RT_INF_OUT,pa,0,0); 
 }//if(dev!=NULL)
 else
 {
  printf("ОШИБКА! ou_inf_out (dev=NULL)\n"); 
  fflush(stdout);
  ret=-1; 
 }
 return(ret);
}
//--------------------------------------------------
//запись в оконечник слова данных для передачи контроллеру
//по команде управления...
//параметры:
//dev - указатель на структуру типа dev_tx_t
//nm - номер канала
//mode - код команды управления  
//d - слово данных 
//возвращаемые значения:
/*
-1 -ОШИБКА
 0 -НОРМА
 */	   
int ou_mode_write( dev_tx_t* dev, int nm, int mode, unsigned short d )
{
 int ret=0;
 if(dev!=0)
 {
  dev->tx_B[0]=RT_MODE_WRITE; dev->tx_B[1]=nm;
  dev->tx_B[2]=mode; dev->tx_B[3]=d;
  Send(dev->tx_pid,dev->tx_B,dev->tx_B,4*2,N_B*2);
  ret=dev->tx_B[0];
 }//if(dev!=NULL)
 else
 {
  printf("ОШИБКА! ou_mode_write (dev=NULL)\n"); 
  fflush(stdout);
  ret=-1; 
 }
 return(ret);
}
//--------------------------------------------------
//чтение из оконечника слова данных, переданного из контроллера
//по команде управления...
//параметры:
//dev - указатель на структуру типа dev_tx_t
//nm - номер канала
//mode - код команды управления (если mode=0, то чтение слова данных 
//       очередной команды управления) 
//возвращаемые значения:
/*
-1 -ОШИБКА
-2   ОШИБКА(ОУ не принимало данных или они уже были прочитаны ранее
            программой пользователя)
 0 -НОРМА
tx_B[0]    - для передачи данных не используется
tx_B[1]    - код команды управления
tx_B[2]    - RT DATA TABLE TAG WORD
tx_B[3]    - 1
tx_B[4]    - слово данных
 */

int ou_mode_read( dev_tx_t* dev, int nm, int mode )
{
 int ret=0;
 if(dev!=NULL)
 {
  dev->tx_B[0]=RT_MODE_READ; dev->tx_B[1]=nm; dev->tx_B[2]=mode; 
  Send(dev->tx_pid,dev->tx_B,dev->tx_B,3*2,N_B*2);
  ret=dev->tx_B[0];
 }//if(dev!=NULL)
 else
 {
  printf("ОШИБКА! ou_mode_read (dev=NULL)\n"); 
  fflush(stdout);
  ret=-1; 
 }
 return(ret);
}



//--------------------------------------------------
/*
 драйвер формирует следующие сигналы "proxy" :
           proxyR[0]   - КК: передача фрейма команд завершена (канал 0)
                         ОУ: получены данные (канал 0)
		   proxyR[1]   - КК: передача фрейма команд завершена (канал 1)
                         ОУ: получены данные (канал 1)		 
						 
           proxyERR[0] - КК: ОШИБКА при передаче сообщения (канал 0)
		   proxyERR[1] - КК: ОШИБКА при передаче сообщения (канал 1)

           proxyMODE[0] - ОУ: получены данные по команде управления (канал 0)
		   proxyMODE[1] - ОУ: получены данные по команде управления (канал 1)

           proxyOUT[0]   - ОУ: переданы данные по команде КК (канал 0)
		   proxyOUT[1]   - ОУ: переданы данные по команде КК (канал 1)		 
*/

