/**
 * @brief		
 * @details		
 * @date		2016-11-15
 **/

/* Includes ------------------------------------------------------------------*/
#include "meter.h"
#include "crc.h"
#include "trace.h"

#if defined ( _WIN32 ) || defined ( _WIN64 )
#include "comm_socket.h"
#include <windows.h>
#elif defined ( __linux )
#include <unistd.h>
#include <pthread.h>
#include "comm_socket.h"
#else

#if defined (BUILD_REAL_WORLD)
#include "stm32f0xx.h"
#include "delay.h"
#include "cpu.h"
#include "vspi1.h"
#endif

#endif

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define devspi      vspi1

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
static enum __dev_status status = DEVICE_NOTINIT;
static void(*meter_callback)(void *buffer) = (void(*)(void *))0;
static uint8_t calibrate = 0;

#if defined ( _WIN32 ) || defined ( _WIN64 ) || defined ( __linux )
static SOCKET sock = INVALID_SOCKET;
static volatile int32_t metering_data[42] = {0};
static volatile uint8_t updating = 0;
#else

#if defined (BUILD_REAL_WORLD)
struct __calibrate_base calibase;
#endif

#endif

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
#if defined ( _WIN32 ) || defined ( _WIN64 ) || defined ( __linux )
#if defined ( __linux )
static void *ThreadRecvMail(void *arg)
#else
static DWORD CALLBACK ThreadRecvMail(PVOID pvoid)
#endif
{
    int32_t buff[42];
	int32_t recv_size;
    uint32_t j;
    
    while(1)
    {
#if defined ( __linux )
    	usleep(8*1000);
#else
    	Sleep(8);
#endif
    	
	    if(sock == INVALID_SOCKET)
	    {
	        continue;
	    }
	    
		recv_size = receiver.read(sock, (uint8_t *)buff, sizeof(buff));
		if (recv_size < (42 * 4))
		{
			continue;
		}

		updating = 0xff;

#if defined ( __linux )
    	usleep(2*1000);
#else
    	Sleep(2);
#endif

		if (status == DEVICE_INIT)
		{
			for (j = 0; j<(int)R_ESC; j++)
			{
				metering_data[j] += buff[j];
			}
		}

		for (j = (int)R_ESC; j<(int)R_FREQ; j++)
		{
			metering_data[j] = buff[j];
		}

		updating = 0;
    }
    
	return(0);
}

/**
  * @brief  ģ�����ʹ�õ�ǰ��ѹ���ж��Ƿ�����
  */
uint8_t is_powered(void)
{
    //ֻҪ�κ�һ���ѹ���ڵ���1V����
	if(metering_data[R_UA - 1] >= 1000 || \
        metering_data[R_UB - 1] >= 1000 || \
        metering_data[R_UC - 1] >= 1000)
    {
        return(0xff);
    }
    
    return(0);
}
#endif

/**
  * @brief  
  */
static enum __dev_status meter_status(void)
{
    //��ȡ��ǰ�豸״̬
    return(status);
}

/**
  * @brief  
  */
static void meter_init(enum __dev_state state)
{
#if defined ( _WIN32 ) || defined ( _WIN64 ) || defined ( __linux )
	sock = receiver.open(50002);
    
    (void)(meter_callback);

	if(sock == INVALID_SOCKET)
	{
		TRACE(TRACE_INFO, "Create receiver for metering failed.");
	}
    
    if(status == DEVICE_NOTINIT)
    {
#if defined ( __linux )
        pthread_t thread;
        pthread_attr_t thread_attr;
        
        pthread_attr_init(&thread_attr);
        pthread_attr_setdetachstate(&thread_attr, PTHREAD_CREATE_DETACHED);
        pthread_create(&thread, &thread_attr, ThreadRecvMail, NULL);
        pthread_attr_destroy(&thread_attr);
#else
        HANDLE hThread;
        hThread = CreateThread(NULL, 0, ThreadRecvMail, 0, 0, NULL);
        CloseHandle(hThread);
#endif
    }
	
	meter_callback= (void(*)(void *))0;
	status = DEVICE_INIT;
#else

#if defined (BUILD_REAL_WORLD)
    GPIO_InitTypeDef GPIO_InitStruct;
    
    (void)(meter_callback);
    
    meter_callback= (void(*)(void *))0;
    
    if(state == DEVICE_NORMAL)
	{
		devspi.control.init(state);
		
		if(devspi.control.status() != DEVICE_INIT)
		{
			status = DEVICE_ERROR;
			return;
		}
		
		//PD12 �̱����/�¼����
		RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOD, ENABLE);
		
		GPIO_InitStruct.GPIO_Pin = GPIO_Pin_12;
		GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN;
		GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_UP;
		GPIO_Init(GPIOD, &GPIO_InitStruct);
        
		//PC5 Power
		RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOC, ENABLE);
		
		GPIO_InitStruct.GPIO_Pin = GPIO_Pin_5;
		GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
        GPIO_InitStruct.GPIO_OType = GPIO_OType_OD;
        GPIO_InitStruct.GPIO_Speed = GPIO_Speed_Level_1;
		GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
		GPIO_Init(GPIOC, &GPIO_InitStruct);
        GPIO_ResetBits(GPIOC, GPIO_Pin_5);
        
        mdelay(10);
        
		//PE15 Reset
		RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOE, ENABLE);
		
		GPIO_InitStruct.GPIO_Pin = GPIO_Pin_15;
		GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
        GPIO_InitStruct.GPIO_OType = GPIO_OType_OD;
        GPIO_InitStruct.GPIO_Speed = GPIO_Speed_Level_1;
		GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
		GPIO_Init(GPIOE, &GPIO_InitStruct);
        
        GPIO_ResetBits(GPIOE, GPIO_Pin_15);
        mdelay(1);
        GPIO_SetBits(GPIOE, GPIO_Pin_15);
        mdelay(10);
        
        calibrate = 0;
		status = DEVICE_INIT;
	}
#endif

#endif
}

/**
  * @brief  
  */
static void meter_suspend(void)
{
#if defined ( _WIN32 ) || defined ( _WIN64 ) || defined ( __linux )
	if(sock != INVALID_SOCKET)
	{
		receiver.close(sock);
		sock = INVALID_SOCKET;
	}
    meter_callback = (void(*)(void *))0;
#else
    
#if defined (BUILD_REAL_WORLD)
    meter_callback = (void(*)(void *))0;
    devspi.control.suspend();
#endif
    
#endif
    
    calibrate = 0;
    status = DEVICE_SUSPENDED;
}

#if defined (BUILD_REAL_WORLD)
/**
  * @brief  �� __metering_meta ת��Ϊ 702x ��������
  */
static uint8_t meter_cmd_translate(enum __metering_meta id)
{
    uint8_t result = 0xff;
    
    switch(id)
    {
        case R_EPA:
            result = 0x1E;
            break;
        case R_EPB:
            result = 0x1F;
            break;
        case R_EPC:
            result = 0x20;
            break;
        case R_EPT:
            result = 0x21;
            break;
        case R_EQA:
            result = 0x22;
            break;
        case R_EQB:
            result = 0x23;
            break;
        case R_EQC:
            result = 0x24;
            break;
        case R_EQT:
            result = 0x25;
            break;
        case R_ESA:
            result = 0x35;
            break;
        case R_ESB:
            result = 0x36;
            break;
        case R_ESC:
            result = 0x37;
            break;
        case R_EST:
            result = 0x38;
            break;
        case R_PA:
            result = 0x01;
            break;
        case R_PB:
            result = 0x02;
            break;
        case R_PC:
            result = 0x03;
            break;
        case R_PT:
            result = 0x04;
            break;
        case R_QA:
            result = 0x05;
            break;
        case R_QB:
            result = 0x06;
            break;
        case R_QC:
            result = 0x07;
            break;
        case R_QT:
            result = 0x08;
            break;
        case R_SA:
            result = 0x09;
            break;
        case R_SB:
            result = 0x0A;
            break;
        case R_SC:
            result = 0x0B;
            break;
        case R_ST:
            result = 0x0C;
            break;
        case R_PFA:
            result = 0x14;
            break;
        case R_PFB:
            result = 0x15;
            break;
        case R_PFC:
            result = 0x16;
            break;
        case R_PFT:
            result = 0x17;
            break;
        case R_UA:
            result = 0x0D;
            break;
        case R_UB:
            result = 0x0E;
            break;
        case R_UC:
            result = 0x0F;
            break;
        case R_IA:
            result = 0x10;
            break;
        case R_IB:
            result = 0x11;
            break;
        case R_IC:
            result = 0x12;
            break;
        case R_IT:
            result = 0x13;
            break;
        case R_YIA:
            result = 0x18;
            break;
        case R_YIB:
            result = 0x19;
            break;
        case R_YIC:
            result = 0x1A;
            break;
        case R_YUAUB:
            result = 0x26;
            break;
        case R_YUAUC:
            result = 0x27;
            break;
        case R_YUBUC:
            result = 0x28;
            break;
        case R_FREQ:
            result = 0x1C;
            break;
        case R_I0:
            result = 0x29;
            break;
    }
    
    return(result);
}
#endif

static void meter_runner(uint16_t msecond)
{
#if defined ( _WIN32 ) || defined ( _WIN64 ) || defined ( __linux )

#else

#if defined (BUILD_REAL_WORLD)

#endif

#endif
}



static int32_t meter_data_read(enum __metering_meta id)
{
#if defined ( _WIN32 ) || defined ( _WIN64 ) || defined ( __linux )
    int32_t result;
    
    if(updating)
    {
#if defined ( __linux )
        usleep(5*1000);
#else
        Sleep(5);
#endif
    }
    
    switch(id)
    {
        case R_EPA:
        case R_EPB:
        case R_EPC:
        case R_EPT:
        case R_EQA:
        case R_EQB:
        case R_EQC:
        case R_EQT:
        case R_ESA:
        case R_ESB:
        case R_ESC:
        case R_EST:
            result = metering_data[(uint16_t)id - 1];
            metering_data[(uint16_t)id - 1] = 0;
            break;
        case R_PA:
        case R_PB:
        case R_PC:
        case R_PT:
        case R_QA:
        case R_QB:
        case R_QC:
        case R_QT:
        case R_SA:
        case R_SB:
        case R_SC:
        case R_ST:
        case R_PFA:
        case R_PFB:
        case R_PFC:
        case R_PFT:
        case R_UA:
        case R_UB:
        case R_UC:
        case R_IA:
        case R_IB:
        case R_IC:
        case R_IT:
        case R_YIA:
        case R_YIB:
        case R_YIC:
        case R_YUAUB:
        case R_YUAUC:
        case R_YUBUC:
        case R_FREQ:
            result = metering_data[(uint16_t)id - 1];
            break;
        default:
            result = 0;
    }
    
    return(result);
#else
    
#if defined (BUILD_REAL_WORLD)
    uint8_t addr;
    float val;
    union {
        int32_t i;
        uint32_t u;
    } result;
    
    if((status == DEVICE_INIT) && (calibrate))
    {
        addr = meter_cmd_translate(id);
		if(addr == 0xff)
		{
			return(0);
		}
        devspi.select(0);
        devspi.octet.write(addr);
        udelay(10);
        result.u = devspi.octet.read();
        result.u <<= 8;
        result.u += devspi.octet.read();
        result.u <<= 8;
        result.u += devspi.octet.read();
        devspi.release(0);
		
        //��ѹ
		if((id >= R_UA) && (id <= R_UC))
		{
			val = (((float)result.u) * ((float)1000) / ((float)8192));
            result.u = (uint32_t)val;
			return(result.i);
		}
		
		if(calibase.check != crc32(&calibase, (sizeof(calibase) - sizeof(uint32_t)), 0))
		{
			return(0);
		}
		
        //����
		if((id >= R_IA) && (id <= R_IC))
		{
			val = (((float)result.u) * ((float)1000) / ((float)4096));
			val = ((float)val) / ((float)calibase.nrate);
			result.u = (uint32_t)val;
		}
        //���ߵ���
		else if(id == R_I0)
		{
			val = (((float)result.u) * ((float)1000) / ((float)4096));
			val = ((float)val) / ((float)calibase.nrate);
			result.u = (uint32_t)val;
		}
        //����ʸ����
		else if(id == R_IT)
		{
			val = (((float)result.u) * ((float)1000) / ((float)4096));
			val = ((float)val) / ((float)calibase.nrate);
			result.u = (uint32_t)val;
		}
        //�й�����
		else if((id >= R_PT) && (id <= R_PC))
		{
            result.u <<= 8;
            result.i /= 256;
            
			//K=2.592*10^10/(HFconst*EC*2^23)
            val = 25920000000 * 1000 / 8388608;
			val /= calibase.hfconst;
			val /= calibase.pulse;
            
            if(id == R_PT)
            {
                result.i = (int32_t)(result.i * val * 2);
            }
            else
            {
                result.i = (int32_t)(result.i * val * 1);
            }
		}
        //�޹�����
		else if((id >= R_QT) && (id <= R_QC))
		{
            result.u <<= 8;
            result.i /= 256;
            
			//K=2.592*10^10/(HFconst*EC*2^23)
            val = 25920000000 * 1000 / 8388608;
			val /= calibase.hfconst;
			val /= calibase.pulse;
            
            if(id == R_QT)
            {
                result.i = (int32_t)(result.i * val * 2);
            }
            else
            {
                result.i = (int32_t)(result.i * val * 1);
            }
		}
        //���ڹ���
		else if((id >= R_ST) && (id <= R_SC))
		{
            result.u <<= 8;
            result.i /= 256;
            
			//K=2.592*10^10/(HFconst*EC*2^23)
            val = 25920000000 * 1000 / 8388608;
			val /= calibase.hfconst;
			val /= calibase.pulse;
            
            if(id == R_ST)
            {
                result.i = (int32_t)(result.i * val * 2);
            }
            else
            {
                result.i = (int32_t)(result.i * val * 1);
            }
		}
    }
    else
    {
        result.i = 0;
    }
    
    return(result.i);
#endif
    
#endif
}



/**
  * @brief  
  */
static bool meter_calibrate_load(uint32_t size, const void *param)
{
#if defined ( _WIN32 ) || defined ( _WIN64 ) || defined ( __linux )
	if(size < sizeof(struct __calibrate_data))
	{
		return(false);
	}
	
	if(((struct __calibrate_data *)param)->check != crc32(param, (sizeof(struct __calibrate_data) - sizeof(uint32_t)), 0))
	{
		return(false);
	}
	
    calibrate = 0xff;
	return(calibrate);
#else

#if defined (BUILD_REAL_WORLD)
    uint8_t loop;
    
	if(size < sizeof(struct __calibrate_data))
	{
		return(false);
	}
	
	if(((struct __calibrate_data *)param)->check != crc32(param, (sizeof(struct __calibrate_data) - sizeof(uint32_t)), 0))
	{
		return(false);
	}
	
	calibase = ((struct __calibrate_data *)param)->base;
	calibase.check = crc32(&calibase, (sizeof(calibase) - sizeof(uint32_t)), 0);
    
    //��У������
    devspi.select(0);
    devspi.octet.write(0xc3);
    devspi.octet.write(0);
    devspi.octet.write(0);
    devspi.octet.write(0);
    devspi.release(0);
    mdelay(1);
    
    //ʹ��д��������
    devspi.select(0);
    devspi.octet.write(0xc9);
    devspi.octet.write(0);
    devspi.octet.write(0);
    devspi.octet.write(0x5a);
    devspi.release(0);
    mdelay(1);
    
    //д����
    for(loop=0; loop<59; loop++)
    {
        devspi.select(0);
        devspi.octet.write(0x80 + (((struct __calibrate_data *)param)->reg[loop].address & 0x7f));
        devspi.octet.write(0);
        devspi.octet.write((((struct __calibrate_data *)param)->reg[loop].value >> 8) & 0xff);
        devspi.octet.write((((struct __calibrate_data *)param)->reg[loop].value >> 0) & 0xff);
        devspi.release(0);
        mdelay(1);
    }
    
    //ʹ�ܶ����ݲ���
    devspi.select(0);
    devspi.octet.write(0xc6);
    devspi.octet.write(0);
    devspi.octet.write(0);
    devspi.octet.write(0);
    devspi.release(0);
    mdelay(1);
    
    //�ر�д��������
    devspi.select(0);
    devspi.octet.write(0xc9);
    devspi.octet.write(0);
    devspi.octet.write(0);
    devspi.octet.write(0);
    devspi.release(0);
    mdelay(1);
    
    mdelay(500);
	
    calibrate = 0xff;
	return(calibrate);
#endif

#endif
}

/**
  * @brief  
  */
static bool meter_calibrate_enter(uint32_t size, void *args)
{
#if defined ( _WIN32 ) || defined ( _WIN64 ) || defined ( __linux )
	if(size < sizeof(struct __calibrates))
	{
		return(false);
	}
	
	memset((void *)&(((struct __calibrates *)args)->data), \
           0, \
           sizeof(((struct __calibrates *)args)->data));
    
	((struct __calibrates *)args)->data.check = crc32((const void *)&(((struct __calibrates *)args)->data), \
                                                      sizeof(((struct __calibrates *)args)->data) - sizeof(uint32_t), 0);
    
    calibrate = 0xff;
    
    return(true);
#else

#if defined (BUILD_REAL_WORLD)
	if(size < sizeof(struct __calibrates))
	{
		return(false);
	}
	
	//����һ ��ʼ��
	if(((struct __calibrates *)args)->param.step == 1)
	{
		double value;
		
		memset((void *)&(((struct __calibrates *)args)->data), \
			   0, \
			   sizeof(((struct __calibrates *)args)->data));
		
		//��Чֵoffset��ÿ������У��һ�Σ�
		((struct __calibrates *)args)->data.reg[6].address = 0x24;
		((struct __calibrates *)args)->data.reg[6].value = 0x0000;
		((struct __calibrates *)args)->data.reg[7].address = 0x25;
		((struct __calibrates *)args)->data.reg[7].value = 0x0000;
		((struct __calibrates *)args)->data.reg[8].address = 0x26;
		((struct __calibrates *)args)->data.reg[8].value = 0x0000;
		((struct __calibrates *)args)->data.reg[9].address = 0x27;
		((struct __calibrates *)args)->data.reg[9].value = 0x0007;
		((struct __calibrates *)args)->data.reg[10].address = 0x28;
		((struct __calibrates *)args)->data.reg[10].value = 0x0007;
		((struct __calibrates *)args)->data.reg[11].address = 0x29;
		((struct __calibrates *)args)->data.reg[11].value = 0x0007;
		
		//ADC offset��ÿ������У��һ�Σ�
		((struct __calibrates *)args)->data.reg[12].address = 0x2a;
		((struct __calibrates *)args)->data.reg[12].value = 0x0000;
		((struct __calibrates *)args)->data.reg[13].address = 0x2b;
		((struct __calibrates *)args)->data.reg[13].value = 0x0000;
		((struct __calibrates *)args)->data.reg[14].address = 0x2c;
		((struct __calibrates *)args)->data.reg[14].value = 0x0000;
		((struct __calibrates *)args)->data.reg[15].address = 0x2d;
		((struct __calibrates *)args)->data.reg[15].value = 0x0000;
		((struct __calibrates *)args)->data.reg[16].address = 0x2e;
		((struct __calibrates *)args)->data.reg[16].value = 0x0000;
		((struct __calibrates *)args)->data.reg[17].address = 0x2f;
		((struct __calibrates *)args)->data.reg[17].value = 0x0000;
		
		//����offset����ѡÿ̨����ҪУ����
		((struct __calibrates *)args)->data.reg[0].address = 0x13;
		((struct __calibrates *)args)->data.reg[0].value = 0x0000;
		((struct __calibrates *)args)->data.reg[1].address = 0x14;
		((struct __calibrates *)args)->data.reg[1].value = 0x0000;
		((struct __calibrates *)args)->data.reg[2].address = 0x15;
		((struct __calibrates *)args)->data.reg[2].value = 0x0000;
		((struct __calibrates *)args)->data.reg[3].address = 0x21;
		((struct __calibrates *)args)->data.reg[3].value = 0x0000;
		((struct __calibrates *)args)->data.reg[4].address = 0x22;
		((struct __calibrates *)args)->data.reg[4].value = 0x0000;
		((struct __calibrates *)args)->data.reg[5].address = 0x23;
		((struct __calibrates *)args)->data.reg[5].value = 0x0000;
		
		//�������油����ÿ̨����ҪУ����
		((struct __calibrates *)args)->data.reg[18].address = 0x04;
		((struct __calibrates *)args)->data.reg[18].value = 0x0000;
		((struct __calibrates *)args)->data.reg[19].address = 0x05;
		((struct __calibrates *)args)->data.reg[19].value = 0x0000;
		((struct __calibrates *)args)->data.reg[20].address = 0x06;
		((struct __calibrates *)args)->data.reg[20].value = 0x0000;
		((struct __calibrates *)args)->data.reg[21].address = 0x07;
		((struct __calibrates *)args)->data.reg[21].value = 0x0000;
		((struct __calibrates *)args)->data.reg[22].address = 0x08;
		((struct __calibrates *)args)->data.reg[22].value = 0x0000;
		((struct __calibrates *)args)->data.reg[23].address = 0x09;
		((struct __calibrates *)args)->data.reg[23].value = 0x0000;
		((struct __calibrates *)args)->data.reg[24].address = 0x0a;
		((struct __calibrates *)args)->data.reg[24].value = 0x0000;
		((struct __calibrates *)args)->data.reg[25].address = 0x0b;
		((struct __calibrates *)args)->data.reg[25].value = 0x0000;
		((struct __calibrates *)args)->data.reg[26].address = 0x0c;
		((struct __calibrates *)args)->data.reg[26].value = 0x0000;
		
		//��λУ����ÿ̨����ҪУ����
		((struct __calibrates *)args)->data.reg[27].address = 0x0d;
		((struct __calibrates *)args)->data.reg[27].value = 0x0000;
		((struct __calibrates *)args)->data.reg[28].address = 0x0e;
		((struct __calibrates *)args)->data.reg[28].value = 0x0000;
		((struct __calibrates *)args)->data.reg[29].address = 0x0f;
		((struct __calibrates *)args)->data.reg[29].value = 0x0000;
		((struct __calibrates *)args)->data.reg[30].address = 0x10;
		((struct __calibrates *)args)->data.reg[30].value = 0x0000;
		((struct __calibrates *)args)->data.reg[31].address = 0x11;
		((struct __calibrates *)args)->data.reg[31].value = 0x0000;
		((struct __calibrates *)args)->data.reg[32].address = 0x12;
		((struct __calibrates *)args)->data.reg[32].value = 0x0000;
		
		//��ѹ����У����ÿ̨����ҪУ����
		((struct __calibrates *)args)->data.reg[33].address = 0x17;
		((struct __calibrates *)args)->data.reg[33].value = 0x0000;
		((struct __calibrates *)args)->data.reg[34].address = 0x18;
		((struct __calibrates *)args)->data.reg[34].value = 0x0000;
		((struct __calibrates *)args)->data.reg[35].address = 0x19;
		((struct __calibrates *)args)->data.reg[35].value = 0x0000;
		
		//��������У����ÿ̨����ҪУ����
		((struct __calibrates *)args)->data.reg[36].address = 0x1a;
		((struct __calibrates *)args)->data.reg[36].value = 0x0000;
		((struct __calibrates *)args)->data.reg[37].address = 0x1b;
		((struct __calibrates *)args)->data.reg[37].value = 0x0000;
		((struct __calibrates *)args)->data.reg[38].address = 0x1c;
		((struct __calibrates *)args)->data.reg[38].value = 0x0000;
		((struct __calibrates *)args)->data.reg[39].address = 0x20;
		((struct __calibrates *)args)->data.reg[39].value = 0x0000;
		
		//���Ʋ���
		((struct __calibrates *)args)->data.reg[40].address = 0x01;//ģʽ���üĴ���
		((struct __calibrates *)args)->data.reg[40].value = 0xbd7f;
		((struct __calibrates *)args)->data.reg[41].address = 0x02;//ADC�������ã���ѹ2��������1��������8����
		((struct __calibrates *)args)->data.reg[41].value = 0x0102;
		((struct __calibrates *)args)->data.reg[42].address = 0x03;//EMU��Ԫ����
		((struct __calibrates *)args)->data.reg[42].value = 0x79c4;
		((struct __calibrates *)args)->data.reg[43].address = 0x16;//�����޹���λУ��
		((struct __calibrates *)args)->data.reg[43].value = 0x0000;
		((struct __calibrates *)args)->data.reg[47].address = 0x30;//�ж�ʹ��
		((struct __calibrates *)args)->data.reg[47].value = 0x0001;
		((struct __calibrates *)args)->data.reg[48].address = 0x31;//ģ��ģ��ʹ��
		((struct __calibrates *)args)->data.reg[48].value = 0x3837;
		((struct __calibrates *)args)->data.reg[49].address = 0x32;//ȫͨ������
		((struct __calibrates *)args)->data.reg[49].value = 0x0000;
		((struct __calibrates *)args)->data.reg[50].address = 0x33;//����ӱ�
		((struct __calibrates *)args)->data.reg[50].value = 0x0000;
		((struct __calibrates *)args)->data.reg[51].address = 0x35;//IO״̬
		((struct __calibrates *)args)->data.reg[51].value = 0x000f;
		((struct __calibrates *)args)->data.reg[53].address = 0x37;//��λ�������򣬲��ֶ�У����0
		((struct __calibrates *)args)->data.reg[53].value = 0x0000;
		((struct __calibrates *)args)->data.reg[58].address = 0x6b;//ToffsetУ��
		((struct __calibrates *)args)->data.reg[58].value = 0x0000;
		((struct __calibrates *)args)->data.reg[55].address = 0x6d;//Vrefgain�Ĳ�������ϵ�� TCcoffA
		((struct __calibrates *)args)->data.reg[55].value = 0xff00;
		((struct __calibrates *)args)->data.reg[56].address = 0x6e;//Vrefgain�Ĳ�������ϵ�� TCcoffB
		((struct __calibrates *)args)->data.reg[56].value = 0x0db8;
		((struct __calibrates *)args)->data.reg[57].address = 0x6f;//Vrefgain�Ĳ�������ϵ�� TCcoffC
		((struct __calibrates *)args)->data.reg[57].value = 0xd1da;
		((struct __calibrates *)args)->data.reg[54].address = 0x70;//�㷨����
		((struct __calibrates *)args)->data.reg[54].value = 0x0002;
		
		//������Ϣ
		((struct __calibrates *)args)->data.base.voltage = ((struct __calibrates *)args)->param.base.voltage;
		((struct __calibrates *)args)->data.base.current = ((struct __calibrates *)args)->param.base.current;
		((struct __calibrates *)args)->data.base.pulse = ((struct __calibrates *)args)->param.base.ppulse;
		//����ϵ��N���壺�����Ib���뵽оƬ��ȡ����ѹΪ50mVʱ����Ӧ�ĵ�����Чֵ�Ĵ���ֵΪ
		//Vrms��Vrms/2^13Լ����60����ʱN=60/Ib�� Ib=1.5A��N=60/1.5=40��Ib=6A��N=60/6=10
		//ͬ�������뵽оƬ��ȡ����ѹΪ25mVʱ��Vrms/2^13Լ����30��Ib=1.5A��N=30/1.5=20��Ib=6A��
		//N=30/6=5���ɸ��ݵ�ǰIb������ʵ��ֵ������Nֵ��
		//����Ӳ����·�˴�Ib=1.5A ȡ����ѹ50mV
		((struct __calibrates *)args)->data.base.nrate = 40;
		((struct __calibrates *)args)->data.base.check = 0;
		
		//���� HFConst
		//HFConst��INT[25920000000*G*G*Vu*Vi/(EC*Un*Ib)]������G=1.163��INTΪȡ������
		value = 35058588480; //25920000000*G*G
		value *= ((struct __calibrates *)args)->param.base.voltage;//Vu(��ѹ����δΪ1500:1������Ŵ�������Ϊ2)
		value *= 2;
		value /= 1500;
		value *= ((struct __calibrates *)args)->param.base.current;//Vi(����1.5A��ViΪ50mV������������)
		value *= 50;
		value /= 1500;
		value /= ((struct __calibrates *)args)->param.base.ppulse;//EC
		value /= ((struct __calibrates *)args)->param.base.voltage;//Un
		value /= ((struct __calibrates *)args)->param.base.current;//Ib
		
		((struct __calibrates *)args)->data.reg[45].address = 0x1e;//��Ƶ���峣��
		((struct __calibrates *)args)->data.reg[45].value = (uint16_t)value;
		
		((struct __calibrates *)args)->data.base.hfconst = (uint16_t)value;
		calibase = ((struct __calibrates *)args)->data.base;
		calibase.check = crc32(&calibase, (sizeof(calibase) - sizeof(uint32_t)), 0);
		
		//�����𶯵��� Istartup
		//Istartup=INT[0.8*Io*2^13]
		//����Io=Ib*N*�������õ�
		value = ((struct __calibrates *)args)->param.base.current;//Ib
		value *= ((struct __calibrates *)args)->data.base.nrate;//N
		value *= 8192;//2^13
		value /= 1000;//0.1% �������� ǧ��֮һIb
		value *= 0.8;//0.8
		value /= 1000;
		((struct __calibrates *)args)->data.reg[44].address = 0x1d;
		((struct __calibrates *)args)->data.reg[44].value = (uint16_t)value;
		
		//�����𶯹��� Pstartup
		//Pstartup=INT[0.6*Ub*Ib*HFconst*EC*k��*2^23/(2.592*10^10)]
		//���ж��ѹUb����������Ib������������k��
		value = ((struct __calibrates *)args)->param.base.voltage;//Ub
		value *= ((struct __calibrates *)args)->param.base.current;//Ib
		value *= ((struct __calibrates *)args)->param.base.ppulse;//EC
		value *= ((struct __calibrates *)args)->data.reg[45].value;//HFconst
		value *= 0.6;//0.6
		value /= 1000;//k�� 0.1%
		value *= 8388608;//2^23
		value /= 25920000000;//2.592*10^10
		value /= 1000;
		value /= 1000;
		((struct __calibrates *)args)->data.reg[52].address = 0x36;
		((struct __calibrates *)args)->data.reg[52].value = (uint16_t)value;
		
		//����ʧѹ��ֵ FailVoltage
		//FailVoltage=Un*2^5*D
		//D��ʾʧѹ��ѹ�ٷֱ�
		value = ((struct __calibrates *)args)->param.base.voltage;//Ub
		value *= 32;//2^5
		value *= 0.6;//60%
		value /= 1000;
		((struct __calibrates *)args)->data.reg[46].address = 0x1f;
		((struct __calibrates *)args)->data.reg[46].value = (uint16_t)value;
		
		cpu.watchdog.feed();
		
		//��У������
		devspi.select(0);
		devspi.octet.write(0xc3);
		devspi.octet.write(0);
		devspi.octet.write(0);
		devspi.octet.write(0);
		devspi.release(0);
		mdelay(1);
		
		//ʹ��д����
		devspi.select(0);
		devspi.octet.write(0xc9);
		devspi.octet.write(0);
		devspi.octet.write(0);
		devspi.octet.write(0x5a);
		devspi.release(0);
		mdelay(1);
		
		//ģʽ���üĴ���(0x01)д�룺0xB97E
		devspi.select(0);
		devspi.octet.write(0x80 + 0x01);
		devspi.octet.write(0);
		devspi.octet.write((0xb97e >> 8) & 0xff);
		devspi.octet.write((0xb97e >> 0) & 0xff);
		devspi.release(0);
		mdelay(1);
		
		//EMU ��Ԫ���üĴ���(0x03)д�룺0xF804
		devspi.select(0);
		devspi.octet.write(0x80 + 0x03);
		devspi.octet.write(0);
		devspi.octet.write((0xf804 >> 8) & 0xff);
		devspi.octet.write((0xf804 >> 0) & 0xff);
		devspi.release(0);
		mdelay(1);
		
		//ģ��ģ��ʹ�ܼĴ���(0x31)д�룺0x3427
		devspi.select(0);
		devspi.octet.write(0x80 + 0x31);
		devspi.octet.write(0);
		devspi.octet.write((0x3427 >> 8) & 0xff);
		devspi.octet.write((0x3427 >> 0) & 0xff);
		devspi.release(0);
		mdelay(1);
		
		//ADC�������ã���ѹ2��������1��������8����
		devspi.select(0);
		devspi.octet.write(0x80 + 0x02);
		devspi.octet.write(0);
		devspi.octet.write((0x0102 >> 8) & 0xff);
		devspi.octet.write((0x0102 >> 0) & 0xff);
		devspi.release(0);
		mdelay(1);
		
		//���ø�Ƶ���峣��
		devspi.select(0);
		devspi.octet.write(0x80 + ((struct __calibrates *)args)->data.reg[45].address);
		devspi.octet.write(0);
		devspi.octet.write((((struct __calibrates *)args)->data.reg[45].value >> 8) & 0xff);
		devspi.octet.write((((struct __calibrates *)args)->data.reg[45].value >> 0) & 0xff);
		devspi.release(0);
		mdelay(1);
		
		//������λ��������
		devspi.select(0);
		devspi.octet.write(0x80 + ((struct __calibrates *)args)->data.reg[53].address);
		devspi.octet.write(0);
		devspi.octet.write((((struct __calibrates *)args)->data.reg[53].value >> 8) & 0xff);
		devspi.octet.write((((struct __calibrates *)args)->data.reg[53].value >> 0) & 0xff);
		devspi.release(0);
		mdelay(1);
	}
	//����� A�๦������У��
	else if(((struct __calibrates *)args)->param.step == 2)
	{
		//Pgain = -err% / (1 + err%)
		double value;
		
		//ʹ��д����
		devspi.select(0);
		devspi.octet.write(0xc9);
		devspi.octet.write(0);
		devspi.octet.write(0);
		devspi.octet.write(0x5a);
		devspi.release(0);
		mdelay(1);
		
		//P
		value = -((struct __calibrates *)args)->param.value[0];
		value = value / (1000000 + ((struct __calibrates *)args)->param.value[0]);
		if(value >= 0)
		{
			value = value * 32768;
		}
		else
		{
			value = 65536 + value * 32768;
		}
		((struct __calibrates *)args)->data.reg[18].address = 0x04;//P
		((struct __calibrates *)args)->data.reg[18].value = (uint16_t)value;
		
		//Q
		value = -((struct __calibrates *)args)->param.value[1];
		value = value / (1000000 + ((struct __calibrates *)args)->param.value[1]);
		if(value >= 0)
		{
			value = value * 32768;
		}
		else
		{
			value = 65536 + value * 32768;
		}
		((struct __calibrates *)args)->data.reg[21].address = 0x07;//Q
		((struct __calibrates *)args)->data.reg[21].value = (uint16_t)value;
		
		//S
		value = -((struct __calibrates *)args)->param.value[2];
		value = value / (1000000 + ((struct __calibrates *)args)->param.value[1]);
		if(value >= 0)
		{
			value = value * 32768;
		}
		else
		{
			value = 65536 + value * 32768;
		}
		((struct __calibrates *)args)->data.reg[24].address = 0x0a;//S
		((struct __calibrates *)args)->data.reg[24].value = (uint16_t)value;
		
		devspi.select(0);
		devspi.octet.write(0x80 + ((struct __calibrates *)args)->data.reg[18].address);
		devspi.octet.write(0);
		devspi.octet.write((((struct __calibrates *)args)->data.reg[18].value >> 8) & 0xff);
		devspi.octet.write((((struct __calibrates *)args)->data.reg[18].value >> 0) & 0xff);
		devspi.release(0);
		mdelay(1);
		
		devspi.select(0);
		devspi.octet.write(0x80 + ((struct __calibrates *)args)->data.reg[21].address);
		devspi.octet.write(0);
		devspi.octet.write((((struct __calibrates *)args)->data.reg[21].value >> 8) & 0xff);
		devspi.octet.write((((struct __calibrates *)args)->data.reg[21].value >> 0) & 0xff);
		devspi.release(0);
		mdelay(1);
		
		devspi.select(0);
		devspi.octet.write(0x80 + ((struct __calibrates *)args)->data.reg[24].address);
		devspi.octet.write(0);
		devspi.octet.write((((struct __calibrates *)args)->data.reg[24].value >> 8) & 0xff);
		devspi.octet.write((((struct __calibrates *)args)->data.reg[24].value >> 0) & 0xff);
		devspi.release(0);
		mdelay(1);
	}
	//������ A����λУ��
	else if(((struct __calibrates *)args)->param.step == 3)
	{
		//Q = -err% / 1.732
		double value;
		
		//ʹ��д����
		devspi.select(0);
		devspi.octet.write(0xc9);
		devspi.octet.write(0);
		devspi.octet.write(0);
		devspi.octet.write(0x5a);
		devspi.release(0);
		mdelay(1);
		
		value = -((struct __calibrates *)args)->param.value[0];
		value = value / 1732000;
		if(value >= 0)
		{
			value = value * 32768;
		}
		else
		{
			value = 65536 + value * 32768;
		}
		((struct __calibrates *)args)->data.reg[27].address = 0x0d;
		((struct __calibrates *)args)->data.reg[27].value = (uint16_t)value;
		((struct __calibrates *)args)->data.reg[30].address = 0x10;
		((struct __calibrates *)args)->data.reg[30].value = (uint16_t)value;
		
		devspi.select(0);
		devspi.octet.write(0x80 + ((struct __calibrates *)args)->data.reg[27].address);
		devspi.octet.write(0);
		devspi.octet.write((((struct __calibrates *)args)->data.reg[27].value >> 8) & 0xff);
		devspi.octet.write((((struct __calibrates *)args)->data.reg[27].value >> 0) & 0xff);
		devspi.release(0);
		mdelay(1);
		
		devspi.select(0);
		devspi.octet.write(0x80 + ((struct __calibrates *)args)->data.reg[30].address);
		devspi.octet.write(0);
		devspi.octet.write((((struct __calibrates *)args)->data.reg[30].value >> 8) & 0xff);
		devspi.octet.write((((struct __calibrates *)args)->data.reg[30].value >> 0) & 0xff);
		devspi.release(0);
		mdelay(1);
	}
	//������ B�๦������У��
	else if(((struct __calibrates *)args)->param.step == 4)
	{
		//Pgain = -err% / (1 + err%)
		double value;
		
		//ʹ��д����
		devspi.select(0);
		devspi.octet.write(0xc9);
		devspi.octet.write(0);
		devspi.octet.write(0);
		devspi.octet.write(0x5a);
		devspi.release(0);
		mdelay(1);
		
		//P
		value = -((struct __calibrates *)args)->param.value[0];
		value = value / (1000000 + ((struct __calibrates *)args)->param.value[0]);
		if(value >= 0)
		{
			value = value * 32768;
		}
		else
		{
			value = 65536 + value * 32768;
		}
		((struct __calibrates *)args)->data.reg[19].address = 0x05;//P
		((struct __calibrates *)args)->data.reg[19].value = (uint16_t)value;
		
		//Q
		value = -((struct __calibrates *)args)->param.value[1];
		value = value / (1000000 + ((struct __calibrates *)args)->param.value[1]);
		if(value >= 0)
		{
			value = value * 32768;
		}
		else
		{
			value = 65536 + value * 32768;
		}
		((struct __calibrates *)args)->data.reg[22].address = 0x08;//Q
		((struct __calibrates *)args)->data.reg[22].value = (uint16_t)value;
		
		//S
		value = -((struct __calibrates *)args)->param.value[2];
		value = value / (1000000 + ((struct __calibrates *)args)->param.value[1]);
		if(value >= 0)
		{
			value = value * 32768;
		}
		else
		{
			value = 65536 + value * 32768;
		}
		((struct __calibrates *)args)->data.reg[25].address = 0x0b;//S
		((struct __calibrates *)args)->data.reg[25].value = (uint16_t)value;
		
		devspi.select(0);
		devspi.octet.write(0x80 + ((struct __calibrates *)args)->data.reg[19].address);
		devspi.octet.write(0);
		devspi.octet.write((((struct __calibrates *)args)->data.reg[19].value >> 8) & 0xff);
		devspi.octet.write((((struct __calibrates *)args)->data.reg[19].value >> 0) & 0xff);
		devspi.release(0);
		mdelay(1);
		
		devspi.select(0);
		devspi.octet.write(0x80 + ((struct __calibrates *)args)->data.reg[22].address);
		devspi.octet.write(0);
		devspi.octet.write((((struct __calibrates *)args)->data.reg[22].value >> 8) & 0xff);
		devspi.octet.write((((struct __calibrates *)args)->data.reg[22].value >> 0) & 0xff);
		devspi.release(0);
		mdelay(1);
		
		devspi.select(0);
		devspi.octet.write(0x80 + ((struct __calibrates *)args)->data.reg[25].address);
		devspi.octet.write(0);
		devspi.octet.write((((struct __calibrates *)args)->data.reg[25].value >> 8) & 0xff);
		devspi.octet.write((((struct __calibrates *)args)->data.reg[25].value >> 0) & 0xff);
		devspi.release(0);
		mdelay(1);
	}
	//������ B����λУ��
	else if(((struct __calibrates *)args)->param.step == 5)
	{
		//Q = -err% / 1.732
		double value;
		
		//ʹ��д����
		devspi.select(0);
		devspi.octet.write(0xc9);
		devspi.octet.write(0);
		devspi.octet.write(0);
		devspi.octet.write(0x5a);
		devspi.release(0);
		mdelay(1);
		
		value = -((struct __calibrates *)args)->param.value[0];
		value = value / 1732000;
		if(value >= 0)
		{
			value = value * 32768;
		}
		else
		{
			value = 65536 + value * 32768;
		}
		((struct __calibrates *)args)->data.reg[28].address = 0x0e;
		((struct __calibrates *)args)->data.reg[28].value = (uint16_t)value;
		((struct __calibrates *)args)->data.reg[31].address = 0x11;
		((struct __calibrates *)args)->data.reg[31].value = (uint16_t)value;
		
		devspi.select(0);
		devspi.octet.write(0x80 + ((struct __calibrates *)args)->data.reg[28].address);
		devspi.octet.write(0);
		devspi.octet.write((((struct __calibrates *)args)->data.reg[28].value >> 8) & 0xff);
		devspi.octet.write((((struct __calibrates *)args)->data.reg[28].value >> 0) & 0xff);
		devspi.release(0);
		mdelay(1);
		
		devspi.select(0);
		devspi.octet.write(0x80 + ((struct __calibrates *)args)->data.reg[31].address);
		devspi.octet.write(0);
		devspi.octet.write((((struct __calibrates *)args)->data.reg[31].value >> 8) & 0xff);
		devspi.octet.write((((struct __calibrates *)args)->data.reg[31].value >> 0) & 0xff);
		devspi.release(0);
		mdelay(1);
	}
	//������ C�๦������У��
	else if(((struct __calibrates *)args)->param.step == 6)
	{
		//Pgain = -err% / (1 + err%)
		double value;
		
		//ʹ��д����
		devspi.select(0);
		devspi.octet.write(0xc9);
		devspi.octet.write(0);
		devspi.octet.write(0);
		devspi.octet.write(0x5a);
		devspi.release(0);
		mdelay(1);
		
		//P
		value = -((struct __calibrates *)args)->param.value[0];
		value = value / (1000000 + ((struct __calibrates *)args)->param.value[0]);
		if(value >= 0)
		{
			value = value * 32768;
		}
		else
		{
			value = 65536 + value * 32768;
		}
		((struct __calibrates *)args)->data.reg[20].address = 0x06;//P
		((struct __calibrates *)args)->data.reg[20].value = (uint16_t)value;
		
		//Q
		value = -((struct __calibrates *)args)->param.value[1];
		value = value / (1000000 + ((struct __calibrates *)args)->param.value[1]);
		if(value >= 0)
		{
			value = value * 32768;
		}
		else
		{
			value = 65536 + value * 32768;
		}
		((struct __calibrates *)args)->data.reg[23].address = 0x09;//Q
		((struct __calibrates *)args)->data.reg[23].value = (uint16_t)value;
		
		//S
		value = -((struct __calibrates *)args)->param.value[2];
		value = value / (1000000 + ((struct __calibrates *)args)->param.value[1]);
		if(value >= 0)
		{
			value = value * 32768;
		}
		else
		{
			value = 65536 + value * 32768;
		}
		((struct __calibrates *)args)->data.reg[26].address = 0x0c;//S
		((struct __calibrates *)args)->data.reg[26].value = (uint16_t)value;
		
		devspi.select(0);
		devspi.octet.write(0x80 + ((struct __calibrates *)args)->data.reg[20].address);
		devspi.octet.write(0);
		devspi.octet.write((((struct __calibrates *)args)->data.reg[20].value >> 8) & 0xff);
		devspi.octet.write((((struct __calibrates *)args)->data.reg[20].value >> 0) & 0xff);
		devspi.release(0);
		mdelay(1);
		
		devspi.select(0);
		devspi.octet.write(0x80 + ((struct __calibrates *)args)->data.reg[23].address);
		devspi.octet.write(0);
		devspi.octet.write((((struct __calibrates *)args)->data.reg[23].value >> 8) & 0xff);
		devspi.octet.write((((struct __calibrates *)args)->data.reg[23].value >> 0) & 0xff);
		devspi.release(0);
		mdelay(1);
		
		devspi.select(0);
		devspi.octet.write(0x80 + ((struct __calibrates *)args)->data.reg[26].address);
		devspi.octet.write(0);
		devspi.octet.write((((struct __calibrates *)args)->data.reg[26].value >> 8) & 0xff);
		devspi.octet.write((((struct __calibrates *)args)->data.reg[26].value >> 0) & 0xff);
		devspi.release(0);
		mdelay(1);
	}
	//������ C����λУ��
	else if(((struct __calibrates *)args)->param.step == 7)
	{
		//Q = -err% / 1.732
		double value;
		
		//ʹ��д����
		devspi.select(0);
		devspi.octet.write(0xc9);
		devspi.octet.write(0);
		devspi.octet.write(0);
		devspi.octet.write(0x5a);
		devspi.release(0);
		mdelay(1);
		
		value = -((struct __calibrates *)args)->param.value[0];
		value = value / 1732000;
		if(value >= 0)
		{
			value = value * 32768;
		}
		else
		{
			value = 65536 + value * 32768;
		}
		((struct __calibrates *)args)->data.reg[29].address = 0x0f;
		((struct __calibrates *)args)->data.reg[29].value = (uint16_t)value;
		((struct __calibrates *)args)->data.reg[32].address = 0x12;
		((struct __calibrates *)args)->data.reg[32].value = (uint16_t)value;
		
		devspi.select(0);
		devspi.octet.write(0x80 + ((struct __calibrates *)args)->data.reg[29].address);
		devspi.octet.write(0);
		devspi.octet.write((((struct __calibrates *)args)->data.reg[29].value >> 8) & 0xff);
		devspi.octet.write((((struct __calibrates *)args)->data.reg[29].value >> 0) & 0xff);
		devspi.release(0);
		mdelay(1);
		
		devspi.select(0);
		devspi.octet.write(0x80 + ((struct __calibrates *)args)->data.reg[32].address);
		devspi.octet.write(0);
		devspi.octet.write((((struct __calibrates *)args)->data.reg[32].value >> 8) & 0xff);
		devspi.octet.write((((struct __calibrates *)args)->data.reg[32].value >> 0) & 0xff);
		devspi.release(0);
		mdelay(1);
	}
	//����� ��ѹУ��
	else if(((struct __calibrates *)args)->param.step == 8)
	{
		//ʵ�������ѹ��ЧֵUr
		//������ѹ��ЧֵUrms=DataU/2^13
		//���㹫ʽ��Ugain=Ur/Urms-1
		//���Ugain��0����Ugain=INT[Ugain*2^15]
		//���Ugain<0����Ugain=INT[2^16+ Ugain*2^15]
		double value;
		int32_t Urms[3];
		
		Urms[0] = meter_data_read(R_UA);
		Urms[1] = meter_data_read(R_UB);
		Urms[2] = meter_data_read(R_UC);
		
		value = ((double)(((struct __calibrates *)args)->param.value[0]) / (double)Urms[0]) - 1;
		if(value >= 0)
		{
			value = value * 32768;
		}
		else
		{
			value = 65536 + value * 32768;
		}
		((struct __calibrates *)args)->data.reg[33].address = 0x17;//A
		((struct __calibrates *)args)->data.reg[33].value = (uint16_t)value;
		
		value = ((double)(((struct __calibrates *)args)->param.value[1]) / (double)Urms[1]) - 1;
		if(value >= 0)
		{
			value = value * 32768;
		}
		else
		{
			value = 65536 + value * 32768;
		}
		((struct __calibrates *)args)->data.reg[34].address = 0x18;//B
		((struct __calibrates *)args)->data.reg[34].value = (uint16_t)value;
		
		value = ((double)(((struct __calibrates *)args)->param.value[2]) / (double)Urms[2]) - 1;
		if(value >= 0)
		{
			value = value * 32768;
		}
		else
		{
			value = 65536 + value * 32768;
		}
		((struct __calibrates *)args)->data.reg[35].address = 0x19;//C
		((struct __calibrates *)args)->data.reg[35].value = (uint16_t)value;
		
		//ʹ��д����
		devspi.select(0);
		devspi.octet.write(0xc9);
		devspi.octet.write(0);
		devspi.octet.write(0);
		devspi.octet.write(0x5a);
		devspi.release(0);
		mdelay(1);
		
		devspi.select(0);
		devspi.octet.write(0x80 + ((struct __calibrates *)args)->data.reg[33].address);
		devspi.octet.write(0);
		devspi.octet.write((((struct __calibrates *)args)->data.reg[33].value >> 8) & 0xff);
		devspi.octet.write((((struct __calibrates *)args)->data.reg[33].value >> 0) & 0xff);
		devspi.release(0);
		mdelay(1);
		
		devspi.select(0);
		devspi.octet.write(0x80 + ((struct __calibrates *)args)->data.reg[34].address);
		devspi.octet.write(0);
		devspi.octet.write((((struct __calibrates *)args)->data.reg[34].value >> 8) & 0xff);
		devspi.octet.write((((struct __calibrates *)args)->data.reg[34].value >> 0) & 0xff);
		devspi.release(0);
		mdelay(1);
		
		devspi.select(0);
		devspi.octet.write(0x80 + ((struct __calibrates *)args)->data.reg[35].address);
		devspi.octet.write(0);
		devspi.octet.write((((struct __calibrates *)args)->data.reg[35].value >> 8) & 0xff);
		devspi.octet.write((((struct __calibrates *)args)->data.reg[35].value >> 0) & 0xff);
		devspi.release(0);
		mdelay(1);
	}
	//����� ����У��
	else if(((struct __calibrates *)args)->param.step == 9)
	{
		//��֪��ʵ�����������ЧֵIr
		//������ѹ��ЧֵIrms=(DataI/2^13)/N
		//���㹫ʽ��Igain=Ir/Irms-1
		//���Igain��0����Igain=INT[Igain*2^15]
		//���Igain��0����Igain=INT[2^16+ Igain*2^15]
		double value;
		int32_t Irms[3];
		
		Irms[0] = meter_data_read(R_IA);
		Irms[1] = meter_data_read(R_IB);
		Irms[2] = meter_data_read(R_IC);
		
		value = ((double)(((struct __calibrates *)args)->param.value[0]) / (double)Irms[0]) - 1;
		if(value >= 0)
		{
			value = value * 32768;
		}
		else
		{
			value = 65536 + value * 32768;
		}
		((struct __calibrates *)args)->data.reg[36].address = 0x1a;//A
		((struct __calibrates *)args)->data.reg[36].value = (uint16_t)value;
		
		value = ((double)(((struct __calibrates *)args)->param.value[1]) / (double)Irms[1]) - 1;
		if(value >= 0)
		{
			value = value * 32768;
		}
		else
		{
			value = 65536 + value * 32768;
		}
		((struct __calibrates *)args)->data.reg[37].address = 0x1b;//B
		((struct __calibrates *)args)->data.reg[37].value = (uint16_t)value;
		
		value = ((double)(((struct __calibrates *)args)->param.value[2]) / (double)Irms[2]) - 1;
		if(value >= 0)
		{
			value = value * 32768;
		}
		else
		{
			value = 65536 + value * 32768;
		}
		((struct __calibrates *)args)->data.reg[38].address = 0x1c;//C
		((struct __calibrates *)args)->data.reg[38].value = (uint16_t)value;
		
		//ʹ��д����
		devspi.select(0);
		devspi.octet.write(0xc9);
		devspi.octet.write(0);
		devspi.octet.write(0);
		devspi.octet.write(0x5a);
		devspi.release(0);
		mdelay(1);
		
		devspi.select(0);
		devspi.octet.write(0x80 + ((struct __calibrates *)args)->data.reg[36].address);
		devspi.octet.write(0);
		devspi.octet.write((((struct __calibrates *)args)->data.reg[36].value >> 8) & 0xff);
		devspi.octet.write((((struct __calibrates *)args)->data.reg[36].value >> 0) & 0xff);
		devspi.release(0);
		mdelay(1);
		
		devspi.select(0);
		devspi.octet.write(0x80 + ((struct __calibrates *)args)->data.reg[37].address);
		devspi.octet.write(0);
		devspi.octet.write((((struct __calibrates *)args)->data.reg[37].value >> 8) & 0xff);
		devspi.octet.write((((struct __calibrates *)args)->data.reg[37].value >> 0) & 0xff);
		devspi.release(0);
		mdelay(1);
		
		devspi.select(0);
		devspi.octet.write(0x80 + ((struct __calibrates *)args)->data.reg[38].address);
		devspi.octet.write(0);
		devspi.octet.write((((struct __calibrates *)args)->data.reg[38].value >> 8) & 0xff);
		devspi.octet.write((((struct __calibrates *)args)->data.reg[38].value >> 0) & 0xff);
		devspi.release(0);
		mdelay(1);
	}
	//����ʮ ���ߵ���У��
	else if(((struct __calibrates *)args)->param.step == 10)
	{
		//��֪��ʵ�����������ЧֵIr
		//������ѹ��ЧֵIrms=(DataI/2^13)/N
		//���㹫ʽ��Igain=Ir/Irms-1
		//���Igain��0����Igain=INT[Igain*2^15]
		//���Igain��0����Igain=INT[2^16+ Igain*2^15]
		double value;
		int32_t Irms;
		
		Irms = meter_data_read(R_I0);
		
		value = ((double)(((struct __calibrates *)args)->param.value[0]) / (double)Irms) - 1;
		if(value >= 0)
		{
			value = value * 32768;
		}
		else
		{
			value = 65536 + value * 32768;
		}
		((struct __calibrates *)args)->data.reg[39].address = 0x1a;
		((struct __calibrates *)args)->data.reg[39].value = (uint16_t)value;
		
		//ʹ��д����
		devspi.select(0);
		devspi.octet.write(0xc9);
		devspi.octet.write(0);
		devspi.octet.write(0);
		devspi.octet.write(0x5a);
		devspi.release(0);
		mdelay(1);
		
		devspi.select(0);
		devspi.octet.write(0x80 + ((struct __calibrates *)args)->data.reg[39].address);
		devspi.octet.write(0);
		devspi.octet.write((((struct __calibrates *)args)->data.reg[39].value >> 8) & 0xff);
		devspi.octet.write((((struct __calibrates *)args)->data.reg[39].value >> 0) & 0xff);
		devspi.release(0);
		mdelay(1);
	}
	else
	{
		return(false);
	}
	
	//ʹ�ܶ�����
	devspi.select(0);
	devspi.octet.write(0xc6);
	devspi.octet.write(0);
	devspi.octet.write(0);
	devspi.octet.write(0);
	devspi.release(0);
	mdelay(1);
	
	//�ر�д����
	devspi.select(0);
	devspi.octet.write(0xc9);
	devspi.octet.write(0);
	devspi.octet.write(0);
	devspi.octet.write(0);
	devspi.release(0);
	mdelay(1);
	
	cpu.watchdog.feed();
	mdelay(500);
	cpu.watchdog.feed();
	
	//���У��
	((struct __calibrates *)args)->data.check = crc32((const void *)&(((struct __calibrates *)args)->data), \
								sizeof(((struct __calibrates *)args)->data) - sizeof(uint32_t), 0);
    calibrate = 0xff;
    return(calibrate);
#endif

#endif
}

/**
  * @brief  
  */
static bool meter_calibrate_status(void)
{
#if defined ( _WIN32 ) || defined ( _WIN64 ) || defined ( __linux )
	return(calibrate);
#else

#if defined (BUILD_REAL_WORLD)
	return(calibrate);
#endif

#endif
}

/**
  * @brief  
  */
static bool meter_calibrate_exit(void)
{
#if defined ( _WIN32 ) || defined ( _WIN64 ) || defined ( __linux )
	return(true);
#else

#if defined (BUILD_REAL_WORLD)
	return(true);
#endif

#endif
}



/**
  * @brief  handler ���������׳��쳣
  */
static void meter_handler_filling(void(*callback)(void *buffer))
{
	if(callback)
    {
        meter_callback = callback;
    }
}

static void meter_handler_remove(void)
{
    meter_callback= (void(*)(void *))0;
}










/**
  * @brief  
  */
const struct __meter meter = 
{
    .control        = 
    {
        .name       = "att7022e 3p4w",
        .status     = meter_status,
        .init       = meter_init,
        .suspend    = meter_suspend,
    },
    
    .runner         = meter_runner,
    
    .read           = meter_data_read,
    
    .calibrate      = 
    {
        .load       = meter_calibrate_load,
        .enter      = meter_calibrate_enter,
        .status     = meter_calibrate_status,
        .exit       = meter_calibrate_exit,
    },
    
    .handler		= 
    {
		.filling	= meter_handler_filling,
		.remove		= meter_handler_remove,
    },
};
