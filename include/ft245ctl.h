/***************************************************************************
 *                                                                         *
 * File      : ft245ctl.h                                                  *
 * Comment   : ft245rl driver ioctl requests                               *
 * Copyright : (c) CBD BC, Pavel Kozlov, 2011                              *
 *                                                                         *
 ***************************************************************************/

#ifndef FT245CTL_H_INCLUDED
#define FT245CTL_H_INCLUDED

#include <ioctl.h>

#define IOCTL_FT245_FIFO_RESET			  _IO( 'T', 1 )						// reset device and driver rx buffer
#define IOCTL_FT245_RXSTATUS			  _IOR( 'T', 2, unsigned long )     // get number of bytes currently in the driver rx buffer
#define IOCTL_FT245_RXTIMEOUT			  _IOW( 'T', 3, unsigned long )     // set read timeout in ms
#define IOCTL_FT245_GETLATENCY			  _IOR( 'T', 4, unsigned char )     // get device latency timer value
#define IOCTL_FT245_SETLATENCY			  _IOW( 'T', 5, unsigned char )     // set device latency timer value

#endif