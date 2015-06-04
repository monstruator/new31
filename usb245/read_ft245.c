/***************************************************************************
 *                                                                         *
 * File      : read_ft245.c                                                *
 * Comment   : devu-ft245 test program                                     * 
 * Copyright : (c) CBD BC, 2011                                            *
 *                                                                         *
 ***************************************************************************/

#ifdef __USAGE
%C	 -	Test program for the devu-ft245 driver

%C [options] &

Options are:
   -d name             Device name. Default /dev/ft245.
   -f fname            Save data to file. By default print to the stdout.
   -n                  Open device on nonblock mode.
   -v                  Be verbose.
   -i                  Get dev info.
   -r                  Reset device and driver rx buffer before data read.
   -l                  Set device latency in msec (range 2-255).
   -s psize            Set packet size. Default 4096.
   -p num              Number of packets to read. Default 1.
   -t 				   Test mode	
	
Examples:
    read_ft245 
    read_ft245 -f /home /data.dat -n
    read_ft245 -d /dev/ft245 -f /home/data.dat -s 1024 -p 8
    read_ft245  -t 6000 -s 8192 -l50

#endif

#include <stdio.h>
#include <conio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <i86.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/dev.h>
#include <sys/qioctl.h>
#include "../include/shared_mem.h"
#include <ft245ctl.h>

int sum_ret=0;
int TM=0;//test mode

typedef struct _optparams	{
	char				*dev_name;
	char				*fname;
	int 				reset;
	int					nonblock;
	int 				devinfo;
	int					verbose;
	unsigned long		rx_timeout;
	unsigned long		psize;
	unsigned long		npackets;
	unsigned char		latency;
} optparams_t;


int get_dev_info( int fd )
{
	struct _dev_info_entry		info;
	int							ret = 0;
	
	if ( dev_info( fd, &info ) == 0 ) {
		printf( "Dev Info form ft245rl driver:\n" );
		printf( "  Driver nid :%d\n", info.nid  );
		printf( "  Driver pid :%d\n", info.driver_pid  );
		printf( "  Open count :%d\n", info.open_count  );
		printf( "  Driver type :%s\n", info.driver_type );
		printf( "  Driver prefix :%s\n", info.tty_name ); 
	}
	else {
		fprintf( stderr, "dev_info() failed. %s\n", strerror( errno ) );
		ret = -1;
	}
	
	return ( ret );
}

int set_rx_timeout( int fd, unsigned long tval )
{
	if ( qnx_ioctl( fd, IOCTL_FT245_RXTIMEOUT, &tval, sizeof(tval), NULL, 0 ) == -1 ) {
		fprintf( stderr, "qnx_ioctl(IOCTL_FT245_FIFO_RXTIMEOUT) is failed. %s\n", strerror( errno ) );
		return ( -1 );
	}
	
	printf( "Set rx timeout timer to %ldms\n", tval );
	return ( 0 );
}

int set_latency( int fd, unsigned char latency )
{
	if ( qnx_ioctl( fd, IOCTL_FT245_SETLATENCY, &latency, sizeof(latency), NULL, 0 ) == -1 ) {
		fprintf( stderr, "qnx_ioctl(IOCTL_FT245_FIFO_SETLATENCY) is failed. %s\n", strerror( errno ) );
		return ( -1 );
	}
	
	return ( 0 );
}

unsigned char get_latency( int fd )
{
	unsigned char 	latency;
	
	if ( qnx_ioctl( fd, IOCTL_FT245_GETLATENCY, NULL, 0, &latency, sizeof(latency) ) == -1 ) {
		fprintf( stderr, "qnx_ioctl(IOCTL_FT245_FIFO_SETLATENCY) is failed. %s\n", strerror( errno ) );
		return ( -1 );
	}
	
	return ( latency );
}


int reset_device( int fd )
{
	unsigned long		in_buf;
	
	if ( qnx_ioctl( fd, IOCTL_FT245_RXSTATUS, NULL, 0,
					&in_buf, sizeof(in_buf) ) == -1 ) {
		fprintf( stderr, "qnx_ioctl(IOCTL_FT245_RXSTATUS) is failed. %s\n", strerror( errno ) );
	}
	else
		printf( "In driver rx buffer before reset: %d bytes\n", in_buf );
	
	if ( qnx_ioctl( fd, IOCTL_FT245_FIFO_RESET, NULL, 0,
					NULL, 0 ) == -1 ) {
		fprintf( stderr, "qnx_ioctl(IOCTL_FT245_FIFO_RESET) is failed. %s\n", strerror( errno ) );
		return ( -1 );
	}

	if ( qnx_ioctl( fd, IOCTL_FT245_RXSTATUS, NULL, 0,
					&in_buf, sizeof(in_buf) ) == -1 ) {
		fprintf( stderr, "qnx_ioctl(IOCTL_FT245_RXSTATUS) is failed. %s\n", strerror( errno ) );
	}
	else
		printf( "In driver rx buffer after reset: %d bytes\n", in_buf );


	return ( 0 );
}

//--------------------------------------------------------------------------
int read_data_nonblock1( int fd, unsigned long psize, unsigned long npackets, char *fname )
{
//	FILE 		 	*out_fp = NULL;
	char		 	*data_buff;
	unsigned long	in_buf = 0;
	int				i = 1,i1=0;
	//int				j = 0;
	//short b=0;
	int priem=0;	//счетчик циклов без приема  информации
	unsigned long   ret;
	unsigned char	DCP_K2[16400];	//массив дцп для пр-к2


	if ( ( data_buff = malloc( 17000 ) ) == NULL ) {
		fprintf( stderr, "Failed to allocate mem for the data buffer. %s\n", strerror( errno ) );
		return ( -1 );
	}

//	if (TM)
//		if ( ( out_fp = fopen( "//1/home/seversk/new31/src/usb_log", "w" ) ) == NULL ) 
//			fprintf( stderr, "Couldn't create/open file %s. %s\n", fname, strerror( errno ) );
	

	while (i) 
	{
		if ( qnx_ioctl( fd, IOCTL_FT245_RXSTATUS, NULL, 0,
					&in_buf, sizeof(in_buf) ) == -1 ) 
		fprintf( stderr, "qnx_ioctl(IOCTL_FT245_RXSTATUS) is failed. %s\n", strerror( errno ) );
		else 
			if ( in_buf > 0 ) 
			{	//printf("in_buf=%d\n",in_buf);
				if ( ( ret = read( fd, data_buff, in_buf ) ) == -1 ) 
					fprintf( stderr, "read() error. %s\n", strerror( errno ) );
				else //если прочитали данные
				{
					if ((sum_ret+ret)>SUM_DCP_K2*2) {ret=SUM_DCP_K2*2-sum_ret;printf("переполнение\n");} //обрежем лишнее
					priem=0;	//был прием
					for(i1=0;i1<ret;i1++) DCP_K2[i1+sum_ret]=data_buff[i1];
					sum_ret+=ret;
					printf("\n sum_ret=%d\n\n",sum_ret);					
				}
			}
		
		if (kbhit()&&TM) i=0;
		
		priem++;	//не было приема 4 cek

		if ( ((priem>40)&&(sum_ret>0)) || (sum_ret==12500)) //{printf("reset\n");priem=0;reset_device(fd);}
		{
			//if ((TM)&&(out_fp != NULL)) //если тестовый режим пишем в файл
			//	if ( fwrite( DCP_K2, sum_ret, 1, out_fp ) != 1 )
			//		fprintf( stderr, "Failed to write to file. %s\n", strerror( errno ) );
			//if ((p->from41.num_com==2)&&(p->from41.num_KS==2))//если это сеанс К2
			{
				if (sum_ret>SUM_DCP_K2*2) sum_ret=SUM_DCP_K2*2; //обрежем лишнее
		    	memcpy(&p->to41.DCP_K2,&DCP_K2,sum_ret);
				p->to41.sum_DCP=sum_ret/2;			
				p->to41.cr_SEANCE++;
				if (TM) 
				{
					printf("%02x:%02x:%02x ", p->CEB[2]>>8,p->CEB[3]>>8,p->CEB[3]&0x00ff);
					printf("Сформирован массив К2 N%d. Кол-во данных %d слов\n",p->to41.cr_SEANCE,p->to41.sum_DCP);				
				}
			}
			sum_ret=0;
		}
		delay(300);	
//		printf("1\n");
	}

	printf( "\n" );
	free( data_buff );
	//fclose( out_fp );
	
	return ( 0 );
}


int main( int argc, char *argv[] )
{
//	unsigned long		in_buf;
//	char		 	*data_buff;
//	int ret;
 	int							c;
	optparams_t					oparams;
	int							fd;
	int							oflag;

	printf( "read_ft245 test program is started\n" );
	
	oparams.dev_name = strdup( "//9/dev/ft245" ); //!!!!
	oparams.fname = NULL;
	oparams.nonblock = 0;
	oparams.devinfo = 0;
	oparams.rx_timeout = 0;
	oparams.psize = 8;
	oparams.npackets = 30;
	oparams.verbose = 0;
	oparams.reset = 0;
	oparams.latency = 0;
	
	while ( ( c = getopt( argc, argv, "d:f:nivrs:p:t:l:" ) ) != -1 ) {
		switch( c )  {
			case 'd':
				oparams.dev_name = optarg;
				break;
			case 'f':
				oparams.fname = optarg;
				break;
			case 'n':
				oparams.nonblock = 1;
				break;
			case 'i':
				oparams.devinfo = 1;
				break;
			case 'v':
				oparams.verbose++;
				break;
			case 's':
				oparams.psize = strtoul( optarg, NULL, 0 );
				break;
			case 'p':
				oparams.npackets = strtoul( optarg, NULL, 0 );
				break;
			case 't':
				TM = 1;
				break;
			case 'r':
				oparams.reset = 1;
				break;
			case 'l':
				oparams.latency = strtoul( optarg, NULL, 0 );
				if ( oparams.latency == 0 ) {
					fprintf( stderr, "Command line: Bad latency value. Correct range is 1 - 255\n" );
					oparams.latency = 0;
				}
				break;
			default:
				fprintf( stderr, "Unsupported comand line option\n" );
				break;
		}
	}
	
	open_shmem();
	// open device in block or nonblock mode
	oflag = O_RDONLY | O_NONBLOCK;
		
	if ( ( fd = open( oparams.dev_name, oflag ) ) == -1 ) {
		fprintf( stderr, "open() failed. %s\n", strerror( errno ) );
		return ( -1 );
	}
	
	//if ( oparams.reset )		reset_device( fd );

	if ( oparams.devinfo ) 
		get_dev_info( fd );
	
	// timeout test
	if ( oparams.rx_timeout ) 
		set_rx_timeout( fd, oparams.rx_timeout );

	if ( oparams.latency ) 
		set_latency( fd, oparams.latency ); 
	
	printf( "Device latency time is %d\n", get_latency( fd ) );
	
	//if ( oparams.reset )	
	//reset_device( fd );
	
	printf( "Start data read. \n");

	read_data_nonblock1( fd, oparams.psize, oparams.npackets, oparams.fname );
	
	printf( "read_ft245 test program is finished %d\n",sum_ret );	

	close( fd );
	return( 0 );
}
