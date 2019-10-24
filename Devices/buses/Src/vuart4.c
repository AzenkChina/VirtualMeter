/**
 * @brief		
 * @details		
 * @date		2016-08-16
 **/

/* Includes ------------------------------------------------------------------*/
#include "vuart4.h"
#include "cpu.h"
#include "trace.h"

#if defined ( _WIN32 ) || defined ( _WIN64 )
#include <windows.h>
#include "stdio.h"
#elif defined ( __linux )
#include <unistd.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include "stdio.h"
#else

#if defined (DEMO_STM32F091)
#include "stm32f0xx.h"
#endif

#endif


/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#if defined ( _WIN32 ) || defined ( _WIN64 )
#define COMM			L"COM14"
#elif defined ( __linux )
#define COMM			"/dev/ttyS4"
#endif

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
static enum __dev_status status = DEVICE_NOTINIT;

static void(*received_byte)(uint8_t) = (void(*)(uint8_t))0;

static  enum __bus_status bus_status = BUS_IDLE;
static enum __baud uart_baud = BDRT_9600;
static enum __parity uart_parity = PARI_EVEN;
static enum __stop uart_stop = STOP_ONE;

#if defined ( _WIN32 ) || defined ( _WIN64 )
static HANDLE hcomm = INVALID_HANDLE_VALUE;
#elif defined ( __linux )
static int fd = -1;
#else

#if defined (DEMO_STM32F091)
static enum __dev_state drv_state;
static const uint8_t *data = (const uint8_t *)0;
static uint16_t length = 0;
static uint16_t sent = 0;
#endif

#endif

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
#if defined ( _WIN32 ) || defined ( _WIN64 ) || defined ( __linux )
#if defined ( __linux )
static void *ThreadRecvByte(void *arg)
#else
static DWORD CALLBACK ThreadRecvByte(PVOID pvoid)
#endif
{
#if defined ( __linux )
	int cnt;
    int read_size = 0;
    uint8_t buff[512];
    
	while(1)
	{
		usleep(5*1000);
        
		if(fd <= 0)
		{
			continue;
		}
	    
        read_size = read(fd, buff, sizeof(buff));
        
        if(read_size <= 0)
        {
            continue;
        }
	    
	    bus_status = BUS_RECEIVE;
	    
	    for(cnt=0; cnt<read_size; cnt++)
	    {
	    	if(received_byte)
	    	{
	    		received_byte(buff[cnt]);
	    	}
	    }
		
	    read_size = 0;
		
	    bus_status = BUS_IDLE;
	}
	
	return(0);
#else
	DWORD cnt;
    DWORD read_size = 0;
    COMSTAT comstat;
    DWORD error;
    uint8_t buff[512];
    
	while(1)
	{
		Sleep(5);
		
		if(hcomm == INVALID_HANDLE_VALUE)
		{
			continue;
		}
		
	    ClearCommError(hcomm, &error, &comstat);
	    
	    if(error > 0)
	    {
	        PurgeComm(hcomm, PURGE_RXCLEAR | PURGE_RXABORT);
	        continue;
	    }
        
        read_size = 0;
	    
	    if(comstat.cbInQue > 0)
	    {
			if(!ReadFile(hcomm, buff, comstat.cbInQue, &read_size, NULL))
			{
				CancelIo(hcomm);
				continue;
			}
	    }
	    
	    bus_status = BUS_RECEIVE;
	    
	    for(cnt=0; cnt<read_size; cnt++)
	    {
	    	if(received_byte)
	    	{
	    		received_byte(buff[cnt]);
	    	}
	    }
        
	    bus_status = BUS_IDLE;
	}
	
	return(0);
#endif
}
#endif

#if defined (DEMO_STM32F091)
void VUART4_Recv_Handler(void)
{
    uint8_t c = USART_ReceiveData(USART1);
    
    if(received_byte)
    {
        received_byte(c);
    }
}

void VUART4_Trans_Handler(void)
{
    if((data) && (sent < length))
    {
        USART_SendData(USART1, data[sent]);
        sent += 1;
    }
    else
    {
        USART_ITConfig(USART1, USART_IT_TC, DISABLE);
        bus_status = BUS_IDLE;
    }
}
#endif

/**
  * @brief  
  */
static enum __dev_status uart_status(void)
{
    return(status);
}

/**
  * @brief  
  */
static void uart_init(enum __dev_state state)
{
#if defined ( _WIN32 ) || defined ( _WIN64 )
	COMMTIMEOUTS timeouts;
	DCB dcb = {0};
	enum __interrupt_status intr_status;
	
	intr_status = cpu.interrupt.status();
	
	if(intr_status == INTR_ENABLED)
	{
		cpu.interrupt.disable();
	}
	
	received_byte = (void(*)(uint8_t))0;
	
	if(intr_status == INTR_ENABLED)
	{
		cpu.interrupt.enable();
	}
	
	hcomm = CreateFileW(COMM,
						GENERIC_READ | GENERIC_WRITE,
						0,
						NULL,
						OPEN_EXISTING,
						0,
						0);

    if(hcomm == INVALID_HANDLE_VALUE)
    {
    	TRACE(TRACE_INFO, "UART4 open failed.");
    	return;
    }
    
    if(!SetupComm(hcomm, 2048, 2048))
    {
        CancelIo(hcomm);
        CloseHandle(hcomm);
        hcomm = INVALID_HANDLE_VALUE;
		TRACE(TRACE_INFO, "UART4 set attributes failed.");
		return;
	}
    
	timeouts.ReadIntervalTimeout = 0; 
	timeouts.ReadTotalTimeoutMultiplier = 0;
	timeouts.ReadTotalTimeoutConstant = 0;
	timeouts.WriteTotalTimeoutMultiplier = 0;
	timeouts.WriteTotalTimeoutConstant = 0;
    
    if(!SetCommTimeouts(hcomm, &timeouts))
    {
        CancelIo(hcomm);
        CloseHandle(hcomm);
        hcomm = INVALID_HANDLE_VALUE;
		TRACE(TRACE_INFO, "UART4 set attributes failed.");
		return;
	}

	if(!GetCommState(hcomm, &dcb))
	{
        CancelIo(hcomm);
        CloseHandle(hcomm);
        hcomm = INVALID_HANDLE_VALUE;
		TRACE(TRACE_INFO, "UART4 set attributes failed.");
		return;
	}
	
    //CBR_300
    //CBR_600
    //CBR_1200
	//CBR_2400
	//CBR_4800
	//CBR_9600
	//CBR_19200
	dcb.BaudRate = CBR_9600;
	
 	//EVENPARITY
  	//MARKPARITY
  	//NOPARITY
  	//ODDPARITY
	dcb.Parity = EVENPARITY;
	
	//ONESTOPBIT
  	//ONE5STOPBITS
  	//TWOSTOPBITS
	dcb.StopBits = ONESTOPBIT;
	
	dcb.ByteSize = 8;
    dcb.fDtrControl = DTR_CONTROL_DISABLE;
    dcb.fRtsControl = RTS_CONTROL_DISABLE;
	
	dcb.DCBlength = sizeof(DCB);
	
    if(!SetCommState(hcomm, &dcb))
	{
        CancelIo(hcomm);
        CloseHandle(hcomm);
        hcomm = INVALID_HANDLE_VALUE;
		return;
	}
	
    if(!PurgeComm(hcomm, PURGE_RXCLEAR | PURGE_RXABORT | PURGE_TXCLEAR | PURGE_TXABORT))
    {
        CancelIo(hcomm);
        CloseHandle(hcomm);
        hcomm = INVALID_HANDLE_VALUE;
		return;
	}
	
    if(status == DEVICE_NOTINIT)
    {
        HANDLE hThread;
        hThread = CreateThread(NULL, 0, ThreadRecvByte, 0, 0, NULL);
        CloseHandle(hThread);
    }
#elif defined ( __linux )
    enum __interrupt_status intr_status;
    struct termios options;
	
	intr_status = cpu.interrupt.status();
	
	if(intr_status == INTR_ENABLED)
	{
		cpu.interrupt.disable();
	}
	
	received_byte = (void(*)(uint8_t))0;
	
	if(intr_status == INTR_ENABLED)
	{
		cpu.interrupt.enable();
	}
    
    fd = open(COMM, O_RDWR | O_NOCTTY | O_NDELAY);
    if (fd < 0)
    {
    	TRACE(TRACE_INFO, "UART4 open failed.");
    	return;
    }
    
    if(tcgetattr(fd, &options) != 0)
    {
        close(fd);
        fd = -1;
        TRACE(TRACE_INFO, "UART4 get attributes failed.");
        return;
    }
    
    //B300
    //B600
    //B1200
	//B2400
	//B4800
	//B9600
	//B19200
    cfsetispeed(&options, B9600);
    cfsetospeed(&options, B9600);
    
    //PARENB clear: no parity
    //PARENB set: have parity
    options.c_cflag |= PARENB;
    //PARODD clear: even parity
    //PARODD set: odd parity
    options.c_cflag &= ~PARODD;
    
    //1 stop bits
    //clear: 1 stop bits
    //set: 2 stop bits
    options.c_cflag &= ~CSTOPB;
    
    //CS7
    //CS8
    options.c_cflag &= ~CSIZE;//enable setting
    options.c_cflag |=  CS8;
    
    //no hardware flow control
    options.c_cflag &= ~CRTSCTS;
    //enable receiver,ignore modem control lines
    options.c_cflag |= CREAD | CLOCAL;
    
    //disable XON/XOFF flow control both i/p and o/p
    options.c_iflag &= ~(IXON | IXOFF | IXANY);
    
    //raw input
    options.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);
    //raw output
    options.c_oflag &= ~OPOST;
    
    //return immediately even no any data received
    options.c_cc[VMIN] = 0;
    options.c_cc[VTIME] = 0;
    
    if((tcsetattr(fd, TCSANOW, &options)) != 0)
    {
        close(fd);
        fd = -1;
        TRACE(TRACE_INFO, "UART4 set attributes failed.");
        return;
    }
    
    //discards old data in the rx buffer
    tcflush(fd, TCIFLUSH);
    
    if(status == DEVICE_NOTINIT)
    {
        pthread_t thread;
        pthread_attr_t thread_attr;
        
        pthread_attr_init(&thread_attr);
        pthread_attr_setdetachstate(&thread_attr, PTHREAD_CREATE_DETACHED);
        pthread_create(&thread, &thread_attr, ThreadRecvByte, NULL);
        pthread_attr_destroy(&thread_attr);
    }
#else
    
#if defined (DEMO_STM32F091)
    GPIO_InitTypeDef GPIO_InitStruct;
    NVIC_InitTypeDef NVIC_InitStruct;
    USART_InitTypeDef USART_InitStruct;
    
    if(state == DEVICE_NORMAL)
    {
        RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);
		GPIO_PinAFConfig(GPIOA, GPIO_PinSource9, GPIO_AF_1);
		GPIO_PinAFConfig(GPIOA, GPIO_PinSource10, GPIO_AF_1);
		GPIO_InitStruct.GPIO_Pin = GPIO_Pin_9 | GPIO_Pin_10;
		GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF;
		GPIO_InitStruct.GPIO_Speed = GPIO_Speed_Level_3;
		GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
		GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
		GPIO_Init(GPIOA, &GPIO_InitStruct);
        
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
        NVIC_InitStruct.NVIC_IRQChannel = USART1_IRQn;
        NVIC_InitStruct.NVIC_IRQChannelPriority = 0x01;
        NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
        NVIC_Init(&NVIC_InitStruct);
        
        USART_DeInit(USART1);
        
        USART_InitStruct.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
        USART_InitStruct.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
        
        USART_InitStruct.USART_BaudRate = ((uint32_t)uart_baud) * 100;
        if(uart_stop == STOP_TWO)
        {
            USART_InitStruct.USART_StopBits = USART_StopBits_2;
        }
        else if(uart_stop == STOP_ONE5)
        {
            USART_InitStruct.USART_StopBits = USART_StopBits_1_5;
        }
        else
        {
            USART_InitStruct.USART_StopBits = USART_StopBits_1;
        }
        
        if(uart_parity == PARI_EVEN)
        {
            USART_InitStruct.USART_Parity = USART_Parity_Even;
            USART_InitStruct.USART_WordLength = USART_WordLength_9b;
        }
        else if(uart_parity == PARI_ODD)
        {
            USART_InitStruct.USART_Parity = USART_Parity_Odd;
            USART_InitStruct.USART_WordLength = USART_WordLength_9b;
        }
        else
        {
            USART_InitStruct.USART_Parity = USART_Parity_No;
            USART_InitStruct.USART_WordLength = USART_WordLength_8b;
        }
        
        USART_Init(USART1, &USART_InitStruct);
        
        USART_SendData(USART1, USART_ReceiveData(USART1));
        USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
        USART_Cmd(USART1, ENABLE);
    }
    
    drv_state = state;
#endif
    
#endif
    status = DEVICE_INIT;
}

/**
  * @brief  
  */
static void uart_suspend(void)
{
#if defined ( _WIN32 ) || defined ( _WIN64 ) || defined ( __linux )
	enum __interrupt_status intr_status = cpu.interrupt.status();
	
	if(intr_status == INTR_ENABLED)
	{
		cpu.interrupt.disable();
	}
	
	received_byte = (void(*)(uint8_t))0;
	
	if(intr_status == INTR_ENABLED)
	{
		cpu.interrupt.enable();
	}
	
#if defined ( __linux )
    if(fd > 0)
    {
        close(fd);
        fd = -1;
    }
#else
    if(hcomm != INVALID_HANDLE_VALUE)
    {
        CancelIo(hcomm);
        CloseHandle(hcomm);
        hcomm = INVALID_HANDLE_VALUE;
    }
#endif

#else
    
#if defined (DEMO_STM32F091)
    USART_DeInit(USART1);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, DISABLE);
    NVIC_DisableIRQ(USART1_IRQn);
    NVIC_ClearPendingIRQ(USART1_IRQn);
    
	data = (const uint8_t *)0;
	length = 0;
	sent = 0;
    bus_status = BUS_IDLE;
#endif
    
#endif
    status = DEVICE_SUSPENDED;
}



/**
  * @brief  
  */
static uint16_t uart_write(uint16_t count, const uint8_t *buffer)
{
#if defined ( _WIN32 ) || defined ( _WIN64 )
	DWORD write_size = 0;
	DWORD error;
    COMSTAT comstat;
    
    if(hcomm == INVALID_HANDLE_VALUE)
    {
    	return(0);
    }
    
    if(!ClearCommError(hcomm, &error, &comstat))
	{
		return(0);
	}
	
    if(error > 0)
    {
        PurgeComm(hcomm, PURGE_TXCLEAR | PURGE_TXABORT);
        return(0);
    }
    
    bus_status = BUS_TRANSFER;
    WriteFile(hcomm, buffer, count, &write_size, NULL);
    bus_status = BUS_IDLE;
    
    return((uint16_t)write_size);
#elif defined ( __linux )
    int write_size = 0;
    
    if(fd <= 0)
    {
    	return(0);
    }
    
    bus_status = BUS_TRANSFER;
    write_size = write(fd, (void *)buffer, (size_t)count);
    bus_status = BUS_IDLE;
    
    return((uint16_t)(write_size>=0? write_size : 0));
#else
    
#if defined (DEMO_STM32F091)
    if((!buffer) || (!count))
    {
        return(0);
    }
    
    if(status != DEVICE_INIT)
    {
        return(0);
    }
    
    data = buffer;
    length = count;
    sent = 0;
    
    bus_status = BUS_TRANSFER;
    USART_ITConfig(USART1, USART_IT_TC, ENABLE);
    USART_SendData(USART1, data[sent]);
    sent += 1;
    
    return(count);
#endif
    
#endif
}

/**
  * @brief  
  */
static enum __bus_status uart_bus_status(void)
{
    return(bus_status);
}

/**
  * @brief  
  */
static enum __baud uart_baudrate_set(enum __baud baudrate)
{
#if defined ( _WIN32 ) || defined ( _WIN64 )
	DCB dcb = {0};
	
    if(hcomm == INVALID_HANDLE_VALUE)
    {
    	return(uart_baud);
    }
	
	if(!GetCommState(hcomm, &dcb))
	{
		return(uart_baud);
	}
	
	dcb.BaudRate = ((uint16_t)baudrate) * 100;
	
    if(!SetCommState(hcomm, &dcb))
	{
		return(uart_baud);
	}
	
    if(!PurgeComm(hcomm, PURGE_RXCLEAR | PURGE_RXABORT | PURGE_TXCLEAR | PURGE_TXABORT))
    {
		return(uart_baud);
	}
	
	uart_baud = baudrate;
#elif defined ( __linux )
    static const int bnames[] = {115200, 38400, 19200, 9600, 4800, 2400, 1200, 300};
    static const int bspeeds[] = {B115200, B38400, B19200, B9600, B4800, B2400, B1200, B300};
    struct termios options;
    int i;
    
    if(fd <= 0)
    {
    	return(uart_baud);
    }
    
    if(tcgetattr(fd, &options) != 0)
    {
        close(fd);
        fd = -1;
        TRACE(TRACE_INFO, "UART4 get attributes failed.");
        return(uart_baud);
    }
    
    for(i=0; i<sizeof(bspeeds)/sizeof(int); i++)
    {
        if((((uint16_t)baudrate) * 100) == bnames[i])
        {
            tcflush(fd, TCIOFLUSH);
            cfsetispeed(&options, bspeeds[i]);
            cfsetospeed(&options, bspeeds[i]);
            if((tcsetattr(fd, TCSANOW, &options)) != 0)
            {
                close(fd);
                fd = -1;
                TRACE(TRACE_INFO, "UART4 set attributes failed.");
                return(uart_baud);
            }
            
            uart_baud = baudrate;
            tcflush(fd, TCIOFLUSH);
        }
    }
#else
    
#if defined (DEMO_STM32F091)
    uart_baud = baudrate;
    if(status == DEVICE_INIT)
    {
        uart_init(drv_state);
    }
#endif
    
#endif

	return(uart_baud);
}

/**
  * @brief  
  */
static enum __baud uart_baudrate_get(void)
{
	return(uart_baud);
}

/**
  * @brief  
  */
static enum __parity uart_parity_set(enum __parity parity)
{
#if defined ( _WIN32 ) || defined ( _WIN64 )
	DCB dcb = {0};
	
    if(hcomm == INVALID_HANDLE_VALUE)
    {
    	return(uart_parity);
    }
	
	if(!GetCommState(hcomm, &dcb))
	{
		return(uart_parity);
	}
	
 	//EVENPARITY
  	//MARKPARITY
  	//NOPARITY
  	//ODDPARITY
  	switch(parity)
  	{
    	case PARI_NONE:
		{
			dcb.Parity = NOPARITY;
			break;
		}
    	case PARI_EVEN:
		{
			dcb.Parity = EVENPARITY;
			break;
		}
    	case PARI_ODD:
		{
			dcb.Parity = ODDPARITY;
			break;
		}
		case PARI_MARK:
		{
			dcb.Parity = MARKPARITY;
			break;
		}
  	}
	
    if(!SetCommState(hcomm, &dcb))
	{
		return(uart_parity);
	}
	
    if(!PurgeComm(hcomm, PURGE_RXCLEAR | PURGE_RXABORT | PURGE_TXCLEAR | PURGE_TXABORT))
    {
		return(uart_parity);
	}
	
	uart_parity = parity;

	return(uart_parity);
#elif defined ( __linux )
    struct termios options;
    
    if(fd <= 0)
    {
    	return(uart_parity);
    }
    
    if(tcgetattr(fd, &options) != 0)
    {
        close(fd);
        fd = -1;
        TRACE(TRACE_INFO, "UART4 get attributes failed.");
        return(uart_parity);
    }
    
 	//EVENPARITY
  	//MARKPARITY
  	//NOPARITY
  	//ODDPARITY
  	switch(parity)
  	{
    	case PARI_NONE:
		{
            options.c_cflag &= ~PARENB;
            uart_parity = parity;
			break;
		}
    	case PARI_EVEN:
		{
            options.c_cflag |= PARENB;
            options.c_cflag &= ~PARODD;
            uart_parity = parity;
			break;
		}
    	case PARI_ODD:
		{
            options.c_cflag |= PARENB;
            options.c_cflag |= PARODD;
            uart_parity = parity;
			break;
		}
  	}
    
    if((tcsetattr(fd, TCSANOW, &options)) != 0)
    {
        close(fd);
        fd = -1;
        TRACE(TRACE_INFO, "UART4 set attributes failed.");
        return(uart_baud);
    }
    
    tcflush(fd, TCIOFLUSH);
    
	return(uart_parity);
#else
    
#if defined (DEMO_STM32F091)
    uart_parity = parity;
    if(status == DEVICE_INIT)
    {
        uart_init(drv_state);
    }
    return(uart_parity);
#endif
    
#endif
}

/**
  * @brief  
  */
static enum __parity uart_parity_get(void)
{
	return(uart_parity);
}

/**
  * @brief  
  */
static enum __stop uart_stop_set(enum __stop stop)
{
#if defined ( _WIN32 ) || defined ( _WIN64 )
	DCB dcb = {0};
	
    if(hcomm == INVALID_HANDLE_VALUE)
    {
    	return(uart_stop);
    }
	
	if(!GetCommState(hcomm, &dcb))
	{
		return(uart_stop);
	}
	
	//ONESTOPBIT
  	//ONE5STOPBITS
  	//TWOSTOPBITS
  	switch(stop)
  	{
    	case STOP_ONE:
		{
			dcb.StopBits = ONESTOPBIT;
			break;
		}
    	case STOP_ONE5:
		{
			dcb.StopBits = ONE5STOPBITS;
			break;
		}
    	case STOP_TWO:
		{
			dcb.StopBits = TWOSTOPBITS;
			break;
		}
  	}
	
    if(!SetCommState(hcomm, &dcb))
	{
		return(uart_stop);
	}
	
    if(!PurgeComm(hcomm, PURGE_RXCLEAR | PURGE_RXABORT | PURGE_TXCLEAR | PURGE_TXABORT))
    {
		return(uart_stop);
	}
	
	uart_stop =  stop;

	return(uart_stop);
#elif defined ( __linux )
    struct termios options;
    
    if(fd <= 0)
    {
    	return(uart_stop);
    }
    
    if(tcgetattr(fd, &options) != 0)
    {
        close(fd);
        fd = -1;
        TRACE(TRACE_INFO, "UART4 get attributes failed.");
        return(uart_stop);
    }
    
  	switch(stop)
  	{
    	case STOP_ONE:
		{
			options.c_cflag &= ~CSTOPB;
            uart_stop = stop;
			break;
		}
    	case STOP_TWO:
		{
			options.c_cflag |= CSTOPB;
            uart_stop = stop;
			break;
		}
  	}
    
    if((tcsetattr(fd, TCSANOW, &options)) != 0)
    {
        close(fd);
        fd = -1;
        TRACE(TRACE_INFO, "UART4 set attributes failed.");
        return(uart_stop);
    }
    
    tcflush(fd, TCIOFLUSH);
    
	return(uart_stop);
#else
    
#if defined (DEMO_STM32F091)
    uart_stop = stop;
    if(status == DEVICE_INIT)
    {
        uart_init(drv_state);
    }
    return(uart_stop);
#endif
    
#endif
}

/**
  * @brief  
  */
static enum __stop uart_stop_get(void)
{
	return(uart_stop);
}

/**
  * @brief  
  */
static void uart_handler_filling(void(*callback)(uint8_t ch))
{
	enum __interrupt_status intr_status = cpu.interrupt.status();
	
	if(intr_status == INTR_ENABLED)
	{
		cpu.interrupt.disable();
	}
	
	received_byte = callback;
	
	if(intr_status == INTR_ENABLED)
	{
		cpu.interrupt.enable();
	}
}

/**
  * @brief  
  */
static void uart_handler_clear(void)
{
	enum __interrupt_status intr_status = cpu.interrupt.status();
	
	if(intr_status == INTR_ENABLED)
	{
		cpu.interrupt.disable();
	}
	
	received_byte = (void(*)(uint8_t))0;
	
	if(intr_status == INTR_ENABLED)
	{
		cpu.interrupt.enable();
	}
}



/**
  * @brief  
  */
const struct __uart vuart4 = 
{
    .control        = 
    {
        .name       = "virtual uart 4",
        .status     = uart_status,
        .init       = uart_init,
        .suspend    = uart_suspend,
    },
    
    
	.write			= uart_write,
	.status			= uart_bus_status,
    
    .baudrate		= 
    {
		.get		= uart_baudrate_get,
		.set		= uart_baudrate_set,
    },
    
    .parity			= 
    {
		.get		= uart_parity_get,
		.set		= uart_parity_set,
    },
    
    .stop			= 
    {
		.get		= uart_stop_get,
		.set		= uart_stop_set,
    },
    
    .handler		= 
    {
		.filling	= uart_handler_filling,
		.remove		= uart_handler_clear,
    },
};

