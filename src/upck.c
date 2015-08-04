#include <stdio.h>
#include <malloc.h>
#include "funcu.h"



int main(int argc, char **argv)
{

  FILE *fin, *fout;
  unsigned int *longbuf, *longbuf2, lbuf[8];
  char buf[4];
  unsigned long ttt, t1,t2;
  int j,i_MV,i_FSHS, i_MNU, i2, jmp, tmp, size_malloc;  
  int gw=0, count_gw=0; // начилие информации и счетчик (good word)
  MV_t MV;
  FORM_FShS_K2_t FSHS;
  





  if(argc == 2){
    fin = fopen(argv[1], "r");
  }else{
    fin = fopen("MNRD.dat", "r");
  }

  if(fin == NULL){
    return(1);
  }


  fseek(fin, 0, SEEK_END);
  printf("%d\n", ftell(fin));
  size_malloc = ftell(fin);  

  longbuf = (unsigned int*)  malloc(size_malloc);
  if(longbuf == NULL){
    printf("MALLOC!\n");
    return(1);
  }  





  fseek(fin, 0, SEEK_SET); 
  
  fread(longbuf, size_malloc, 1, fin);

  longbuf2 = longbuf;

  printf("size %d\n", size_malloc/4);

  i2 = 0;
  i_MV = i_FSHS = i_MNU = 0;
  while( i2< size_malloc/4 ){
    

    t2 = *longbuf2;
    ttt = 0;
    t1 = 0x1<<31;

    for(j = 32; j > 0; j--){
      ttt = ttt | (((t2&t1)>>(j-1))  <<  (32 - j));
      t1=t1>>1;
    }
    ttt = ttt >> 12;

    *longbuf2 = ttt;
    i2++;
    longbuf2++;

  }

  i2 = 0;
  longbuf2 = longbuf;

  while( i2< size_malloc/4 ){

//    printf("i2 = %d\n", i2);
    ttt = *longbuf2;        

    if(((ttt& 0xF0000) == 0xA0000)  && (*(longbuf2 +2) == 0x00000))
    {
	  if(!CSumK1(longbuf2, 4)){
        printf("FORM_MV!   %d \n", ++i_MV);
        gt_MV(&MV, longbuf2);
		jmp = i2 + 3;
		gw=1;
      }
	  else printf("FORM_MV error chksum!  \n");
	}

    if( ((ttt&0xF0000) == 0x90000)){
      if(!CSumK1(longbuf2, 8)){
        printf("FORM_FSHS!   %d\n", ++i_FSHS);
        //gt_FSHS(&FSHS, longbuf2);
		jmp = i2 +7;
		gw=1;

      }	  else printf("FORM_FSHS error chksum!  \n");

	}

    if( ((ttt&0xF0000) == 0xD0000)){
      if(!CSumK1(longbuf2, 18)){
        printf("FORM_MNU!   %d\n", ++i_MNU);
        //gt_FSHS(&FSHS, longbuf2);
		jmp = i2 +17;
		gw=1;

      }	  else printf("FORM_MNU error chksum!  \n");

	}

	if (gw==1) count_gw++; 
    printf("%06d  %05X %d\n", i2, ttt , count_gw);

if( i2 == jmp) {printf("\n");gw=0;}
    i2++;
    longbuf2++;
  }
	printf("МВ %d   МНУ %d   ФШС %d   Всего %d   Плохих %d\n", i_MV, i_MNU, i_FSHS, i2, i2-count_gw);
  
  return(0);
}



