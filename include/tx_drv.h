//tx_drv.h
//䠩� tx_drv.h ������砥��� � ���짮��⥫�᪮� �ணࠬ��. 
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
//������� ��� �ନ஢���� ������� ��...
struct DefCMD
{
 unsigned short cmd; //������� ��
 unsigned short *d;  //- 㪠��⥫� �� ���ᨢ ᫮� ������ (� ��砥,
                     //  �᫨ �����筨� �ਭ����� �����) 
					 // - NULL (� ��砥, �᫨ �����筨� ��।��� �����)
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
//��⠭���� �ਧ�����, ��।������� �� � �⢥⭮� ᫮��...
//dev - 㪠��⥫� �� �������� ⨯� dev_tx_t
//nm - ����� ������
//aw - ᫮��, ᮤ�ঠ饥 �ਧ���� �⢥⭮�� ᫮��
//�����頥�� ���祭��:
/*
-1 -������
 0 -�����
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
  printf("������! SetAnswer (dev=NULL)\n"); 
  fflush(stdout);
  ret=-1; 
 }
 return(ret);
}

//��⠭���� ०��� ��...
//��ࠬ����:
//dev - 㪠��⥫� �� �������� ⨯� dev_tx_t
//nm - ����� ������
//ou - ���� ��
//r=true - ���뢠��� �� �ਥ�� �����筨��� ࠧ�襭�
//r=false - ���뢠��� �� �ਥ�� �����筨��� ����饭�
//�����頥�� ���祭��:
/*
-1 -������
 0 -�����
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
    printf("����� ������=%d?  �����⨬� ���祭�� =0,1,2\n",nm);
	fflush(stdout);
	return(-1); 
   }
  while((dev->tx_pid<=0)&&(i<1000))
   {
    dev->tx_pid=qnx_vc_name_attach(0,100,name); i++; 
   }
  if(dev->tx_pid<=0)
   { 
    printf("������! regim_ou (���ᮥ������� � tx_drv)\n"); 
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
  printf("������! regim_ou (dev=NULL)\n"); 
  fflush(stdout);
  ret=-1; 
 }
 return(ret);
}
//--------------------------------------------------

//��⠭���� ०��� ��...
//��ࠬ����:
//dev - 㪠��⥫� �� �������� ⨯� dev_tx_t
//nm - ����� ������
//p=true - ���뢠��� �� �����襭�� �믮������ ������ ࠧ�襭�
//p=false - ���뢠��� �� �����襭�� �믮������ ������ ����饭�
//�����頥�� ���祭��:
/*
-1 -������
 0 -�����
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
    printf("����� ������=%d?  �����⨬� ���祭�� =0,1,2\n",nm); 
	fflush(stdout);
	return(-1); 
   }
  while((dev->tx_pid<=0)&&(i<1000))
   {
    dev->tx_pid=qnx_vc_name_attach(0,100,name); i++;
   }
  if(dev->tx_pid<=0)
   { 
    printf("������! regim_kk (���ᮥ������� � tx_drv)\n"); 
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
  printf("������! regim_kk (dev=NULL)\n"); 
  fflush(stdout);
  ret=-1; 
 } 
 return(ret);
}
//--------------------------------------------------

//�ନ஢���� ᮮ�饭�� ��� ॠ����樨 �⥭�� �� �� ��� ����� � ��...
//��ࠬ����:
//dev - 㪠��⥫� �� �������� ⨯� dev_tx_t
//nm - ����� ������
//u=RT_READ, RT_INF_OUT - �⥭�� �� ��
//u=RT_WRITE - ������ � ��
//pa - �������  
//dl - ������⢮ ᫮� 
//D - 㪠��⥫� �� ���ᨢ ᫮� ������ (NULL - � ��砥 �⥭�� �� �����筨��)
//�����頥�� ���祭��:
/*
-1 -������
 0 -�����
 */
int tx_send( dev_tx_t* dev, int nm, char u, int pa, int dl, unsigned short D[] )
{
 int ret=0;
 int i;
 if(dev!=0)
 {
  if(dev->tx_pid<=0)
   {
    printf("��� ����㯠 � tx_drv"); 
	fflush(stdout);
	return(-1); 
   }
  if((pa<(((u==RT_READ)||(u==RT_INF_OUT))?0:1))||(pa>=31))
   { 
    printf("����୮� ���祭�� �������=%d\n",pa); 
	return(-1); 
   }
  if(D!=NULL)
   if((dl<1)||(dl>32))
   { 
    printf("�����=%d?  �����⨬� ���祭��=1-32\n",dl); 
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
  printf("������! tx_send (dev=NULL)\n"); 
  fflush(stdout);
  ret=-1; 
 }
 return(ret);
}
//--------------------------------------------------
//����� "�३�� ������"...
//��ࠬ����:
//dev - 㪠��⥫� �� �������� ⨯� dev_tx_t
//nm - ����� ������ (nm|RBUS -� ��砥 १�ࢭ��� ������)
//n - ������⢮ ����⮢ � ���ᨢ� ������� ⨯� "struct DefCMD" 
//    ("n" ࠢ�� �������� ������ �� "�३��")
//p - 㪠��⥫� �� 1-� ����� ���ᨢ� ������� ⨯� "struct DefCMD"
//    p[i].cmd - BC COMMAND
//	  p[i].D   - 㪠��⥫� �� 1-� ����� ���ᨢ� ᫮� ������

//�����頥�� ���祭��:
/*
-1 -������
 0 -�����
� ��砥 ࠡ��� ��� ���뢠���:
tx_B[0]    - ��� ��।�� ������ �� �ᯮ������
tx_B[1]      - ������⢮ ������ �� "�३��"
tx_B[2]      - BC CONTROL WORD   | - 1-�� �������
tx_B[3]      - BC COMMAND        |
tx_B[4]      - STATUS WORD       |
tx_B[5]      - ������⢮ ᫮�   |
               ������            |
               (0 - � ��������   |
			    �ਥ�� ������    |
			    �����筨���)     |
tx_B[6]...   - DATA WORDS        | 		

tx_B[X]      - BC CONTROL WORD   |- 2-�� �������
tx_B[X+1]    - BC COMMAND        |
tx_B[X+2]    - STATUS WORD       |
tx_B[X+3]    - ������⢮ ᫮�   |
               ������ 
               (0 - � ��������   |
			    �ਥ�� ������    |
			    �����筨���)     |
tx_B[X+4]... - DATA WORDS        |

.............................
                                 |- n-�� ������� 

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
    printf("��� ����㯠 � tx_drv"); 
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
  printf("������! KK_frame (dev=NULL)\n"); 
  fflush(stdout);
  ret=-1; 
 }
 return(ret);
}
//--------------------------------------------------
//����祭�� १���� �� "n" �������� �� ��砫� "�३��" � ��砥
//ࠡ��� �� �� ���뢠���...
//��ࠬ����:
//dev - 㪠��⥫� �� �������� ⨯� dev_tx_t
//nm - ����� ������
//n - ������⢮ ������ �� ��砫� �३��, �� ����� ���� �������  
//    १����

//�����頥�� ���祭��:
/*
-1 -������
 0 -�����

tx_B[0]    - ��� ��।�� ������ �� �ᯮ������
tx_B[1]      - ������⢮ ������ �� "�३��"
tx_B[2]      - BC CONTROL WORD   | - 1-�� �������
tx_B[3]      - BC COMMAND        |
tx_B[4]      - STATUS WORD       |
tx_B[5]      - ������⢮ ᫮�   |
               ������            |
               (0 - � ��������   |
			    �ਥ�� ������    |
			    �����筨���)     |
tx_B[6]...   - DATA WORDS        | 		

tx_B[X]      - BC CONTROL WORD   |- 2-�� �������
tx_B[X+1]    - BC COMMAND        |
tx_B[X+2]    - STATUS WORD       |
tx_B[X+3]    - ������⢮ ᫮�   |
               ������ 
               (0 - � ��������   |
			    �ਥ�� ������    |
			    �����筨���)     |
tx_B[X+4]... - DATA WORDS        |

.............................
                                 |- n-�� ������� 
 */								 
int KK_end( dev_tx_t* dev, int nm, int n )
{
 int ret=0;
 if(dev!=NULL)
 {
  if(dev->tx_pid<=0)
   {
    printf("��� ����㯠 � tx_drv"); 
	fflush(stdout);
	return(-1); 
   }
  dev->tx_B[0]=BC_RESULT; dev->tx_B[1]=nm;dev->tx_B[2]=n;
  Send(dev->tx_pid,dev->tx_B,dev->tx_B,3*2,N_B*2);
  ret=dev->tx_B[0];  
 }//if(dev!=NULL)
 else
 {
  printf("������! KK_end (dev=NULL)\n"); 
  fflush(stdout);
  ret=-1; 
 }
 return(ret);
}
//--------------------------------------------------
//����祭�� १���� �� ������� � ����஬ "ncmd" �� ��砫� "�३��"
//� ��砥 ࠡ��� �� �� ���뢠���...
//��ࠬ����:
//dev - 㪠��⥫� �� �������� ⨯� dev_tx_t
//nm - ����� ������
//ncmd - ����� ������� �� ��砫� �३��, �� ���ன ���� �������  
//       १����

//�����頥�� ���祭��:
/*
-1 -������
 0 -�����

tx_B[0]    - ��� ��।�� ������ �� �ᯮ������
tx_B[1]      - BC CONTROL WORD   
tx_B[2]      - BC COMMAND        
tx_B[3]      - STATUS WORD       
tx_B[4]      - ������⢮ ᫮�   
               ������            
               (0 - � ��������   |
			    �ਥ�� ������    |
			    �����筨���)     |
tx_B[5]...   - DATA WORDS         		
 */
int KK_end_CMD( dev_tx_t* dev, int nm, int ncmd )
{
 int ret=0;
 if(dev!=NULL)
 {
  if(dev->tx_pid<=0)
   {
    printf("��� ����㯠 � tx_drv"); 
	fflush(stdout);
	return(-1); 
   }
  dev->tx_B[0]=BC_RESULT_CMD; dev->tx_B[1]=nm;dev->tx_B[2]=ncmd;
  Send(dev->tx_pid,dev->tx_B,dev->tx_B,3*2,N_B*2);
  ret=dev->tx_B[0];  
 }//if(dev!=NULL)
 else
 {
  printf("������! KK_end_CMD (dev=NULL)\n"); 
  fflush(stdout);
  ret=-1; 
 }
 return(ret);
}

//--------------------------------------------------
//������ � �����筨�...
//��ࠬ����:
//dev - 㪠��⥫� �� �������� ⨯� dev_tx_t
//nm - ����� ������
//pa - �������  
//dl - ������⢮ ᫮� 
//D - 㪠��⥫� �� ���ᨢ ᫮� ������ 
//�����頥�� ���祭��:
/*
-1 -������
 0 -�����
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
  printf("������! ou_write (dev=NULL)\n"); 
  fflush(stdout);
  ret=-1; 
 }
 return(ret);
}
//--------------------------------------------------
//�⥭�� �� �����筨��...
//��ࠬ����:
//dev - 㪠��⥫� �� �������� ⨯� dev_tx_t
//nm - ����� ������
//pa - ������� (�᫨ pa=0, � �⥭�� �� ��।���� �������,
//     �� ���஬� ��諨 �����) 
//�����頥�� ���祭��:
/*
-1 -������
-2 -������ (�� �� �ਭ����� ������ ��� ��� 㦥 �뫨 ���⠭� ࠭��
           �ணࠬ��� ���짮��⥫�)
 0 -�����

tx_B[0]    - ��� ��।�� ������ �� �ᯮ������
tx_B[1]    - �������
tx_B[2]    - RT DATA TABLE TAG WORD
tx_B[3]    - ������⢮ ᫮� ������
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
  printf("������! ou_read (dev=NULL)\n"); 
  fflush(stdout);
  ret=-1; 
 }
 return(ret);
}
//--------------------------------------------------
//����祭�� ���ଠ樨 � ��।�� ������ �� �����筨��...
//��ࠬ����:
//dev - 㪠��⥫� �� �������� ⨯� dev_tx_t
//nm - ����� ������
//pa - �������, �� ���஬� ���� ������� ���ଠ�� 
//     (�᫨ pa=0, � ����祭�� ���ଠ樨 
//               � ��।�� ������ �� ��।���� �������) 
//�����頥�� ���祭��:
/*
-1 -������
-2 -������ (����� �� �� ��।�������)
 0 -�����

tx_B[0]    - ��� ��।�� ������ �� �ᯮ������
tx_B[1]    - �������
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
  printf("������! ou_inf_out (dev=NULL)\n"); 
  fflush(stdout);
  ret=-1; 
 }
 return(ret);
}
//--------------------------------------------------
//������ � �����筨� ᫮�� ������ ��� ��।�� ����஫����
//�� ������� �ࠢ�����...
//��ࠬ����:
//dev - 㪠��⥫� �� �������� ⨯� dev_tx_t
//nm - ����� ������
//mode - ��� ������� �ࠢ�����  
//d - ᫮�� ������ 
//�����頥�� ���祭��:
/*
-1 -������
 0 -�����
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
  printf("������! ou_mode_write (dev=NULL)\n"); 
  fflush(stdout);
  ret=-1; 
 }
 return(ret);
}
//--------------------------------------------------
//�⥭�� �� �����筨�� ᫮�� ������, ��।������ �� ����஫���
//�� ������� �ࠢ�����...
//��ࠬ����:
//dev - 㪠��⥫� �� �������� ⨯� dev_tx_t
//nm - ����� ������
//mode - ��� ������� �ࠢ����� (�᫨ mode=0, � �⥭�� ᫮�� ������ 
//       ��।��� ������� �ࠢ�����) 
//�����頥�� ���祭��:
/*
-1 -������
-2   ������(�� �� �ਭ����� ������ ��� ��� 㦥 �뫨 ���⠭� ࠭��
            �ணࠬ��� ���짮��⥫�)
 0 -�����
tx_B[0]    - ��� ��।�� ������ �� �ᯮ������
tx_B[1]    - ��� ������� �ࠢ�����
tx_B[2]    - RT DATA TABLE TAG WORD
tx_B[3]    - 1
tx_B[4]    - ᫮�� ������
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
  printf("������! ou_mode_read (dev=NULL)\n"); 
  fflush(stdout);
  ret=-1; 
 }
 return(ret);
}



//--------------------------------------------------
/*
 �ࠩ��� �ନ��� ᫥���騥 ᨣ���� "proxy" :
           proxyR[0]   - ��: ��।�� �३�� ������ �����襭� (����� 0)
                         ��: ����祭� ����� (����� 0)
		   proxyR[1]   - ��: ��।�� �३�� ������ �����襭� (����� 1)
                         ��: ����祭� ����� (����� 1)		 
						 
           proxyERR[0] - ��: ������ �� ��।�� ᮮ�饭�� (����� 0)
		   proxyERR[1] - ��: ������ �� ��।�� ᮮ�饭�� (����� 1)

           proxyMODE[0] - ��: ����祭� ����� �� ������� �ࠢ����� (����� 0)
		   proxyMODE[1] - ��: ����祭� ����� �� ������� �ࠢ����� (����� 1)

           proxyOUT[0]   - ��: ��।��� ����� �� ������� �� (����� 0)
		   proxyOUT[1]   - ��: ��।��� ����� �� ������� �� (����� 1)		 
*/

