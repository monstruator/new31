#include <stdio.h>
#include <malloc.h>
#include <math.h>


#include "type_form.h"




void addbyte(unsigned int*, unsigned int);
int CSumK1(unsigned int*, int);
void gt_MV(MV_t*, unsigned int*);
void gt_FSHS(FORM_FShS_K2_t*, unsigned int*);


unsigned int TIME_SEC_MV;


int CSumK1(unsigned int *m5, int len){
  unsigned long tmp = 0x0;
  int i;

  for( i = 0; i < len -1; i++ ){
    if(!(i%2)){
      tmp += (m5[i] & 0xFF000) >> 12;
      tmp += (m5[i] & 0x00FF0) >> 4; 
      tmp += (m5[i] & 0x0000F) << 4;
    }else{
      tmp += (m5[i] & 0xF0000) >> 16;
      tmp += (m5[i] & 0x0FF00) >> 8; 
      tmp += (m5[i] & 0x000FF);
    }
  }
  tmp += (m5[len -1] & 0xF0000) >> 16;
  tmp += (m5[len -1] & 0x0FF00) >> 8; 

  while( tmp >= 0x100){
    tmp = (tmp & 0x000FF) + (tmp >> 8); 
  }


  if((m5[len -1] & 0xFF) == tmp){
    return(0);
  }
  return(1);
}




void gt_MV(MV_t* MV, unsigned int* m){
  
  MV->N_4year = (m[0] & 0x0F000) >> 12;
  MV->N_day   = (m[0] & 0x00FFE) >> 1;
  MV->hour    = ((m[0] & 0x00001) << 4) + ((m[1] & 0x78000) >> 15); 
  MV->min     = (m[1] & 0x07E00) >> 9;
  MV->sec     = (m[1] & 0x001F8) >> 3; 

  printf("N_4year %d,  N_day  %d,  hour  %d  min  %d,  sec  %d   \n",MV->N_4year, MV->N_day, MV->hour, MV->min, MV->sec);

  TIME_SEC_MV = MV->hour*3600 + MV->min*60 + MV->sec;
}




float delta_T( unsigned long temp2){

  float c0,c1,c2, res;

  c0=4.88024;
  c1=29.88024;
  c2=8.000027 *pow(10, -3);

  if(temp2!=1023){ 
    res = c0+c1*(exp(c2*((float)(temp2) + 0.5))-1);
  }else{     
    res = (-1.0);								//1 во всех разрядах
  }
  return(res);

}

void gt_FSHS(FORM_FShS_K2_t *FSHS, unsigned int *m){

  float c0,c1,c2, temp1;
  unsigned int T_PR;
  

  FSHS->M       = 4*((m[0] & 0x0E000) >> 13);    
  FSHS->SKO_BPO = 1.25*((m[0] & 0x01FC0) >> 6);
  FSHS->la      =  10./3600.*PI/180.*(((m[0] & 0x0001F)  << 11) + ((m[1] & 0x7FF00) >> 8));
  if(m[0] & 0x00020){ FSHS->la *= (-1);}
  FSHS->SKO_MPO = 1.25*((m[1] & 0x000F8) >> 3);
  FSHS->kvm_1    = (m[1] & 0x00007);
  FSHS->fi      = 10./3600.*PI/180.*((m[2] & 0x3FFF8) >> 3);
  if(m[2] & 0x40000){ FSHS->fi *= (-1);}
  FSHS->as_BPO  = 2.8125*PI/180.*(((m[2] & 0x00007) << 3) + ((m[3] & 0x70000) >> 16));
//  FSHS->tau1    = ((m[3] & 0x0FF80) >> 7);
//   FSHS->PM1     = (m[3] & 0x0007F);

  FSHS->F_n      = (((m[4] & 0x7FFFB) >> 2)*100 + 100000)*pow(10, -6);
  FSHS->T_pr    = (((m[4] & 0x00003) << 9) + ((m[5] & 0x7FC00) >> 10));


  if((FSHS->kvm_1==0) || (FSHS->kvm_1==4) || (FSHS->kvm_1==5) || (FSHS->kvm_1==6)){
    c0=7.821567;
    c1=107.8216;
    c2=4.000534/100.0;

  }else if((FSHS->kvm_1==1) || (FSHS->kvm_1==2) || (FSHS->kvm_1==3) ){
    c0=5.880644;
    c1=272.5474;
    c2=3.000225/100.0;
  }

  temp1 = m[3]&0x0007F;		

  if((temp1 != 0x7F) && (FSHS->kvm_1 != 7)){
    FSHS->PM_1=c0+c1*(exp(c2*(((float) temp1) + 0.5))-1);
  }
  if((temp1 == 0x7F) || (FSHS->kvm_1 == 7)){
    FSHS->PM_1=-1.0; 		//1 во всех разрядах или неопознанный вид модуляции
  }

  c0=0.1982064;
  c1=6.448207;
  c2=1.600034/100.0;

  temp1=((m[3] & 0x0FF80) >> 7);


  if(temp1 !=511){
    FSHS->tau_1=c0+c1*(exp(c2*(((float)temp1) + 0.5))-1);
  }else{ 
    FSHS->tau_1=-1.0;			//1 во всех разрядах
  }

  FSHS->delta_T_1      = delta_T((m[5] & 0x003FF));
  FSHS->delta_T_2      = delta_T((m[6] & 0x7FE00) >> 9);
  FSHS->delta_T_3      = delta_T(((m[6] & 0x001FF) << 1) + ((m[7] & 0x40000) >> 18));
  FSHS->delta_T_4      = delta_T((m[7] & 0x3FF00) >> 8);


 T_PR = FSHS->T_pr + TIME_SEC_MV;

  printf("%02d:%02d:%02d   Fn %.3f   T_pr %d  fi %.2f   la %.2f, kvm %d, PM_1 %.1f\n",  T_PR/3600, T_PR%3600/60, T_PR%60, FSHS->F_n*1000, FSHS->T_pr, FSHS->fi*180./PI, FSHS->la*180./PI, FSHS->kvm_1, FSHS->PM_1);
  printf(" tau %.2f  T1 %.1f T2 %.1f T3 %.1f T4 %.1f, BPO %.1f,  MPO %.1f, A %.1f\n",
    FSHS->tau_1, FSHS->delta_T_1, FSHS->delta_T_2, FSHS->delta_T_3, FSHS->delta_T_4,
    FSHS->SKO_BPO, FSHS->SKO_MPO, FSHS->as_BPO/180*PI); 


}



