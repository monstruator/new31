//=====================================================================================
//=============      функции    приемо-передачи   IP / UDP     ========================
//=====================================================================================
#include <limits.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/uio.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>

#if (LONG_MAX !=0x7fffffffUL)
#error "Unsupported platform, using long type isn't 32 bits"
#endif

//=====================================================================================
//=============   конфигурация сокета и портов приемо-передачи ========================
//=====================================================================================
#define UDP_KADR_MAX 1200  // ???? - уточнить

typedef struct
         {
          int soc;        //идентификатор socket-а
          int port_out;   //номер порта на передачу запросов
          int port_inp;   //номер порта на прием ответов
          int y;          //состояние клиента
          char *serv_name;//указатель на имя сервера
          struct hostent *host;    //указатель для поиска
                                  //IP-адреса по имени сервера 
          struct sockaddr_in serv;      //адрес сервера
          struct sockaddr_in serv_from; //адрес, по которому слушаем 
          struct sockaddr_in from;      //адрес, откуда фактически получили 
         }Udp_Client_t;
//=====================================================================================
//=============   функции  приемо-передачи сервера и клиента   ========================
//=====================================================================================

//---------------------------------------------------
int Udp_Client_Ini(Udp_Client_t *Uc,char *serv_name,int port_out,int port_inp)
 {
  int broadcast;
  int dl;
  int x; 
  Uc->port_out=port_out;
  Uc->port_inp=port_inp;
  Uc->y=1;
  Uc->serv_name=serv_name;
  Uc->soc=socket(AF_INET,SOCK_DGRAM,0);
  if(Uc->soc<0)Uc->y=-1; //неудача при создании socket-а
  else
    {//socket создан  
     Uc->host=(struct hostent*)gethostbyname(Uc->serv_name);
     if(Uc->host==NULL)
      {//неудача при определении IP адреса
       Uc->y=-2;
      }//неудача при определении IP адреса
      else
       {//IP-адрес наиден
        memcpy(&Uc->serv.sin_addr,Uc->host->h_addr,
                                  Uc->host->h_length);
        Uc->serv.sin_family=AF_INET;
        Uc->serv.sin_port=htons(Uc->port_out);
        broadcast=1; //-разрешаем широковещательую передачу
        if(setsockopt(Uc->soc,SOL_SOCKET,SO_BROADCAST,
                     &broadcast,sizeof(broadcast))<0)
              {//неудача при установке BROADCAST
               Uc->y=-3;
              }//неудача при установке BROADCAST
      }//IP-адрес наиден
     }//socket создан

  if(Uc->y==1)
     {//настройка приемника
      Uc->serv_from.sin_family=AF_INET;
      Uc->serv_from.sin_addr.s_addr=INADDR_ANY;
//      Uc->serv_from.sin_port=0;//htons(Uc->port_inp);
      Uc->serv_from.sin_port=htons(Uc->port_inp);
      x=bind(Uc->soc,(struct sockaddr*)&Uc->serv_from,
                             sizeof(Uc->serv_from));
    if(x<0)
     {//неудача при назначении порта
      printf("Udp_Client_Ini : bind(...)=%d\n",x);
      fflush(stdout); 
      Uc->y=-4;
     }//неудача при назначении порта
     else
     {//порт назначен нормально
      dl=sizeof(struct sockaddr_in);
      x=getsockname(Uc->soc,(struct sockaddr*)&Uc->serv_from,&dl);
//      printf("gethostname=%d\n",x);
//      printf("Udp_Client_Ini fr_port=%d\n",ntohs(Uc->serv_from.sin_port));
//      fflush(stdout);
     }//порт назначен нормально     
     }//настройка приемника
  return(Uc->y);
 }
//---------------------------------------------------
int Udp_Client_Send(Udp_Client_t *Uc,void *otkuda,int skolko)
 {
  int x; 
//  if((otkuda!=NULL)&&(skolko<=UDP_KADR_MAX)&&(skolko>0))
  if((otkuda!=NULL)&&(skolko>0))
   {//данные корректны, можно передавать
    x=sendto(Uc->soc,otkuda,skolko,0,
                (struct sockaddr*)&Uc->serv,sizeof(Uc->serv));
   }//данные корректны, можно передавать
  return(x);
 }
//---------------------------------------------------
int Udp_Client_Read(Udp_Client_t *Uc,void *kuda,int limit)
 {//блокирующее чтение ответа
  int x,dfr;
  if(Uc->y==1)
     {//ожидание пакета по UDP
       dfr=sizeof(Uc->serv_from);
       memcpy(&Uc->from,&Uc->serv_from,sizeof(Uc->serv_from));
       x=recvfrom(Uc->soc,kuda,limit,0,(struct sockaddr*)&Uc->from,&dfr);
     //  printf("sin_port=%d\n",ntohs(Uc->from.sin_port)); 
//      printf("Udp_Client_Read from_addr=%d\n",ntohs(Uc->from.sin_addr.s_addr));
     //  fflush(stdout);
     }//ожидание пакета по UDP
  else
  {//приемник не настроен
    printf("Udp_Client_Read: ожидание данных от ненастроенного приемника\n");
    fflush(stdout);
    delay(1000);
    x=0;   
  }//приемник не настроен 
  return(x);
 }//блокирующее чтение ответа
//-----------------------------------------------------
