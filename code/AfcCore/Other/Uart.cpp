/// @file Uart.cpp
/// @brief 
/// @author Jesse Xu <zhimo811@gmail.com>
/// @version V1.0.0
/// @date 2018-01-05

#include <termios.h>
#include <fcntl.h>
#include "LtyTypeDef.h"
#include "LtyError.h"
#include "LtyCommon.h"
#include "ev.h"
#include "LtyTimer.h"
#include "Uart.h"

LTY_U32 speed_arr[] =
{ 
    B115200, B38400, B19200, B9600, B4800, B2400, B1200, B300,
    B38400, B19200, B9600,  B4800, B2400, B1200,B600, B300,
};

LTY_U32 name_arr[] =
{
    115200, 38400, 19200, 9600, 4800, 2400, 1200, 300,
    38400, 19200, 9600, 4800, 2400,  1200,600, 300,
};

CUart::CUart()
{
    
}

CUart::~CUart()
{

}

LTY_U32 CUart::Init(struct ev_loop *loop, LTY_U32 *pUartFd, LTY_U8 *pUartDev, LTY_U32 u32Speed)
{
    LTY_U32 u32Ret = LTY_SUCCESS;
    LTY_U32 u32Fd = 0;

    u32Fd = UartInit(pUartDev, u32Speed);
    if ( u32Fd == LTY_FAILURE )
    {
        ///_ERR_("Uart Init %s Error", pUartDev); 
        return LTY_FAILURE;
    }
    tcflush(u32Fd,TCIFLUSH);
	*pUartFd = u32Fd;
       
    return u32Ret; 
}

LTY_VOID CUart::Release()
{
     
}

LTY_U64 CUart::GetTimeUsec(void)
{
    struct timeval tv;
    gettimeofday(&tv, NULL);

    return (((1000*1000) * tv.tv_sec) + tv.tv_usec);
}

LTY_U32 CUart::UartRead(LTY_U32 u32UartFd, LTY_U8 *pBuff, LTY_U32 u32Len)
{
    LTY_U32 u32RdLen = 0;
    LTY_S32 s32Ret = 0;
    fd_set rfds;
    LTY_S32 s32RetVal = 0;
    struct timeval tv;
    LTY_U8 *pRecvBuf = NULL;
	
	
    pRecvBuf = pBuff;
    memset(&rfds, 0, sizeof(rfds));
	//_DBG_("UartRead start\n");
    while(u32RdLen < u32Len)
    {
        FD_ZERO(&rfds);
        FD_SET(u32UartFd, &rfds);
        tv.tv_sec = 6;
        tv.tv_usec = 0;
     	 s32Ret = select(u32UartFd + 1, &rfds, NULL, NULL, &tv);
        if(s32Ret < 0)
        {			
            if ( errno != EINTR )
            {									
                ///_ERR_( "Error: Select the fd(%d) error (%s)", u32UartFd, strerror(errno) );
                break;
            }	
            else 
            {	
                ///_ERR_("Error: Select the fd(%d) receive signal (%s)", u32UartFd, strerror(errno) );
                continue;
            }			
        }
        else if ( 0 == s32Ret )
        {
        	//_DBG_("s32Ret=0 break\n");
            break;
        }
		
        if (FD_ISSET( u32UartFd, &rfds ))
        {			
            s32RetVal = read(u32UartFd, pRecvBuf+u32RdLen, u32Len - u32RdLen);
            if(s32RetVal < 0)
            {
                if ( errno != EWOULDBLOCK )
                {
                    ///_ERR_("Error: read(%d) error (%s)", u32UartFd, strerror(errno));
                    break;
                }
                ///_ERR_("Error: read(%d) continue (%s)", u32UartFd, strerror(errno));
                continue;
            }
            else if( 0 == s32RetVal)
            {
            	//_DBG_("s32RetVal=0 break\n");
                break;
            }
            u32RdLen += s32RetVal;
        }
    }
    if (0 == u32RdLen)
    {
        return LTY_FAILURE;
    }	
	//_DBG_("u32RdLen=%d\n",u32RdLen);
    return u32RdLen;
}
	
LTY_U32 CUart::UartWrite(LTY_U32 u32UartFd, LTY_U8 *pBuff, LTY_U32 u32Len)
{
	LTY_U32 u32HasWrite = 0;
    LTY_U32 u32WLen = 0;
    LTY_S32 s32Ret = 0;
	LTY_S32 s32RetVal = 0; 
	fd_set wfds;
    struct timeval tv;
    long long u64STime = 0;

    memset(&wfds, 0, sizeof(wfds));
    memset(&tv, 0, sizeof(tv));
    if((NULL == pBuff) || (u32Len <= 0))
    {
        return LTY_FAILURE;
    }
    u32HasWrite = u32Len;
    u64STime = GetTimeUsec();

    while((GetTimeUsec() - u64STime) < 1000*300)
    {
        //usleep(10);
        FD_ZERO(&wfds);
        FD_SET(u32UartFd, &wfds);
        tv.tv_sec = 0;
        tv.tv_usec = 1000;
        s32Ret = select(u32UartFd + 1, NULL, &wfds, NULL, &tv);
        if (0 < s32Ret)
        {
            s32RetVal = write(u32UartFd, pBuff, (u32Len - u32WLen));
            if (s32RetVal <= 0)
            {
                ///_ERR_( "Dev_Uart_write write ERROR! Error code = 0x%x\n", errno); 
                return LTY_FAILURE;
            }
            u32WLen += s32RetVal;
            u32HasWrite -= s32RetVal;
            if(u32HasWrite == 0)
            {
                return u32WLen;
            }
            pBuff += u32WLen;
        }
    }
    return LTY_FAILURE;
}

LTY_U32 CUart::UartInit(const LTY_U8 *pCom, LTY_U32 u32Speed)
{
    LTY_U32 u32Ret = LTY_FAILURE;
    LTY_U8 szSerial[16] = "/dev/";
    strcat((char*)szSerial, (char*)pCom);
		
    LTY_U32 u32Fd = UartOpen(szSerial);
    if (u32Fd > 0)
    {
        UartSetSpeed(u32Fd, u32Speed);
        u32Ret = UartSetParity(u32Fd, 8,1,'n');
        if ( u32Ret == LTY_FAILURE )
        {	
            close(u32Fd);
            ///_ERR_("uart set_parity error\n");
        }
        return u32Fd;
    }
	
    return LTY_FAILURE;
}
    
LTY_U32 CUart::UartOpen(LTY_U8 *pUartDev)
{
    LTY_S32 s32Fd = 0;
        
    s32Fd = open((char*)pUartDev, O_RDWR|O_NOCTTY|O_NONBLOCK);
    if (s32Fd == -1)
    {
        ///_ERR_("Uart_Readdata open()");
        return LTY_FAILURE;
    }
   
    return s32Fd;
}

LTY_VOID CUart::UartSetSpeed(LTY_U32 u32UartFd, LTY_U32 u32Speed)
{
    LTY_U32 i = 0;
    LTY_U32 u32Status = 0;
    struct termios Opt;
    
    memset(&Opt, 0, sizeof(Opt));
    tcgetattr(u32UartFd, &Opt);
   
    for (i = 0; i < sizeof (speed_arr) / sizeof(int); i++)
    {
        if(u32Speed == name_arr[i])
        {
            tcflush(u32UartFd, TCIOFLUSH);
            int ret = cfsetispeed(&Opt, speed_arr[i]);
            if( ret == -1 )
            {
                ///_ERR_("set speed failed");
                return;
            }
            cfsetospeed (&Opt, speed_arr[i]);
            u32Status = tcsetattr(u32UartFd, TCSANOW, &Opt);
            if (u32Status != 0)
            {
                ///_ERR_("tcsetattr fd1");
                return;
            }
            tcflush(u32UartFd, TCIOFLUSH);
        }
    }
}

LTY_U32 CUart::UartSetParity(LTY_U32 u32UartFd, LTY_U32 u32DataBits, LTY_U32 u32StopBits, LTY_U32 u32Parity)
{
    LTY_U32 u32Ret = LTY_FAILURE;
    struct termios options;
    
    memset(&options, 0, sizeof(options));
 
    if (tcgetattr (u32UartFd, &options) != 0)
    {
        ///_ERR_("uart Setup Serial 1");
        return u32Ret;
    }														
    options.c_cflag &= ~CSIZE;
    switch (u32DataBits)
    {
    case 7:
        options.c_cflag |= CS7;
        break;
    case 8:
        options.c_cflag |= CS8;
        break;
    default:
        ///_ERR_("uart Unsupported data size\n");
        return u32Ret;
    }	
    switch (u32Parity)
    {
    case 'n':
    case 'N':
        options.c_cflag &= ~PARENB;		/* Clear parity enable */
        options.c_iflag &= ~INPCK;		/* Enable parity checking */
        break;
    case 'o':
    case 'O':
        options.c_cflag |= (PARODD | PARENB);
        options.c_iflag |= INPCK;		/* Disnable parity checking */
        break;
    case 'e':
    case 'E':
        options.c_cflag |= PARENB;		/* Enable parity */
        options.c_cflag &= ~PARODD;
        options.c_iflag |= INPCK;		/* Disnable parity checking */
        break;
    case 'S':
    case 's':					/*as no parity */
        options.c_cflag &= ~PARENB;
        options.c_cflag &= ~CSTOPB;
        break;
    default:
        ///_ERR_("uart Unsupported parity\n");
        return u32Ret;
    }
    switch (u32StopBits)
    {
    case 1:
        options.c_cflag &= ~CSTOPB;
        break;

    case 2:
        options.c_cflag |= CSTOPB;
        break;

    default:
        ///_ERR_("uart Unsupported stop bits\n");
        return u32Ret;
    }
    if (u32Parity != 'n')
        options.c_iflag |= INPCK;
    tcflush (u32UartFd, TCIFLUSH);
    options.c_cc[VTIME] = 50;
    options.c_cc[VMIN] = 0;				/* Update the options and do it NOW */

    options.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);
    options.c_iflag &= ~( ICRNL | IXON);
    options.c_oflag &= ~( ICRNL | IXON);
    options.c_oflag &= ~OPOST;
    if (tcsetattr (u32UartFd, TCSANOW, &options) != 0)
    {
        ///_ERR_ ("uart SetupSerial 3");
        return u32Ret;
    }

    return LTY_SUCCESS;
}

LTY_VOID CUart::UartTcFlush(LTY_U32 u32UartFd)
{
	tcflush(u32UartFd, TCIOFLUSH);
}
