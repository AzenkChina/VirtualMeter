/**
 * @brief		�豸����ģ��
 * @details		���ļ��ｫ������е��豸���н�ģ�����е����趼Ҫ��ѭ
 *              �豸ģ������д������
 * @date		azenk@2016-11-15
 **/


/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __DEVICE_H__
#define __DEVICE_H__

/* Includes ------------------------------------------------------------------*/
#include "stdint.h"
#include "device_lowlevel.h"

/* Exported types ------------------------------------------------------------*/

/**
  * @brief  �豸��ǰ״̬
  */
enum __dev_status
{
    DEVICE_NOTINIT = 0, //�豸δ��ʼ��
    DEVICE_INIT, //�豸�ѳ�ʼ��
    DEVICE_SUSPENDED, //�豸�ѹ���
    DEVICE_MISC, //�豸״̬����
    DEVICE_ERROR, //�豸����
};

/**
  * @brief  �豸��ʼ��ģʽ
  */
enum __dev_state
{
    DEVICE_NORMAL = 0x03,//������ʼ��
    DEVICE_LOWPOWER = 0x0c,//�͹��ĳ�ʼ��
};

/**
  * @brief  ����ģ���豸���Ƶ�Ԫ��ÿ������ģ���еĵ�һ���ṹ������ �豸���Ƶ�Ԫ
  */
struct __device_ctrl
{
    const char						*name;
    enum __dev_status               (*status)(void); //�豸��ǰ״̬
    void                            (*init)(enum __dev_state state); //��ʼ�����豸
    void                            (*suspend)(void); //�����豸
};




//###########################################################################
//ͨ���豸����ģ��

/**
  * @brief  ����״̬
  */
enum __key_status
{
    KEY_NONE = 0, //����û�а���
    KEY_PRESS, //��������
    KEY_RELEASE, //����̧��
    KEY_LONG_PRESS, //����������
    KEY_LONG_RELEASE, //��������̧��
};

/**
  * @brief  ���ߵ�ǰ״̬
  */
enum __bus_status
{
    BUS_IDLE = 0, //���߿���
    BUS_BUSY, //����æµ
    BUS_TRANSFER, //���߷���������
    BUS_RECEIVE, //���߽���������
};

/**
  * @brief  baudrate
  */
enum __baud
{
    BDRT_300 = 3,
    BDRT_600 = 6,
    BDRT_1200 = 12,
	BDRT_2400 = 24,
	BDRT_4800 = 48,
	BDRT_9600 = 96,
	BDRT_19200 = 192,
	BDRT_38400 = 384,
};

/**
  * @brief  parity
  */
enum __parity
{
    PARI_NONE = 0,
    PARI_EVEN,
    PARI_ODD,
	PARI_MARK,
};

/**
  * @brief  stop
  */
enum __stop
{
    STOP_ONE = 0,
	STOP_ONE5,
	STOP_TWO,
};

/**
  * @brief  �������߹���ģʽ 
  */
enum __serial_mode
{
    SERIAL_IN = 0, //������
    SERIAL_OUT, //�����
    SERIAL_AUTO, //�Զ���˫��
};


/**
  * @brief  LED״̬
  */
enum __led_status
{
    LED_OFF = 0, //led�ر�
    LED_ON, //led��
    LED_DONTCARE, //������
    LED_UNKNOWN, //led״̬δ֪
};

/**
  * @brief  ����״̬
  */
enum __switch_status
{
    SWITCH_OPEN = 0, //�Ͽ�
    SWITCH_CLOSE, //�պ�
    SWITCH_DONTCARE, //������
    SWITCH_UNKNOWN, //״̬δ֪
};

/**
  * @brief  ���״̬
  */
enum __battery_status
{
    BAT_FULL = 0, //�����״̬
    BAT_LOW, //��ز���
    BAT_EMPTY, //��ؿ�
};

/**
  * @brief  usart�豸����ģ��
  */
struct __uart
{
    struct __device_ctrl            control;
    
    uint16_t                        (*write)(uint16_t count, const uint8_t *buffer); //д����
    enum __bus_status               (*status)(void); //����״̬
    
    struct
    {
    	enum __baud                 (*get)(void); //��ȡ��ǰ������
        enum __baud                 (*set)(enum __baud baudrate); //���ò�����
        
    }                               baudrate;
    
    struct
    {
        enum __parity				(*get)(void); //��ȡ��ǰУ��λ
        enum __parity				(*set)(enum __parity parity); //����У��λ
        
    }                                parity;
    
    struct
    {
        enum __stop					(*get)(void); //��ȡ��ǰֹͣλ
        enum __stop					(*set)(enum __stop stop); //����ֹͣλ
        
    }                                stop;
    
    //usart�Ľ��չ������첽�ģ�����ֻ��ͨ���ص�������ʵ�����ݵĽ���
    struct
    {
        void                        (*filling)(void(*callback)(uint8_t ch)); //����ֽڽ��ջص�����
        void                        (*remove)(void); //����ֽڽ��ջص�����
        
    }                               handler;
    
};

/**
  * @brief  serial������ģ��
  */
struct __serial
{
    struct __device_ctrl            control;
    
    void                            (*runner)(uint16_t msecond); //�����ϴε����뱾�ε���֮��ļ��ʱ�䣬��λ����
    uint16_t                        (*read)(uint8_t *buffer, uint16_t size); //��һ֡����
    uint16_t                        (*write)(uint16_t count); //дһ֡����
    enum __bus_status               (*status)(void); //����״̬
    
    struct
    {
        uint16_t					(*get)(uint8_t **buffer); //��ȡ��ǰ���ͻ������׵�ַ
        void                        (*set)(uint8_t *buffer, uint16_t size); //���ý��ջ�����
        void                        (*remove)(void); //ɾ��������
        
    }                               rxbuff;
    
    struct
    {
		uint16_t					(*get)(uint8_t **buffer); //��ȡ��ǰ���ͻ������׵�ַ
		void                        (*set)(uint8_t *buffer, uint16_t size); //���û�����
        void                        (*remove)(void); //ɾ��������
        
    }                               txbuff;
    
    struct
    {
        enum __serial_mode			(*get)(void); //��ȡ��ǰ����ģʽ
        enum __serial_mode			(*set)(enum __serial_mode mode); //���ù���ģʽ
        
    }                               mode;
    
    struct
    {
        uint16_t                    (*get)(void); //��ȡ��ǰ֡��ʱʱ��
        uint16_t                    (*set)(uint16_t msecond); //����֡��ʱʱ��
        
    }                               timeout;
    
    const struct __uart			*uart; //serial��Ӧ��usart
};

#if defined ( _WIN32 ) || defined ( _WIN64 ) || defined ( __linux )
/**
  * @brief  console�豸����ģ��
  */
struct __console
{
    struct __device_ctrl            control;
    
    struct
    {
        void                        (*filling)(void(*)(const char *)); //��ӻص�����
        void                        (*remove)(void); //����ص�����
        
    }                               handler;
};
#endif

/**
  * @brief  iic�����豸����ģ��
  */
struct __iic
{
    struct __device_ctrl            control;
    
    struct
    {
        uint8_t                     (*read)(void); //�����߶�һ���ֽ�
        uint8_t                     (*write)(uint8_t ch); //дһ���ֽڵ�����
        
    }                               octet;
    
    struct
    {
        uint16_t                    (*read)(uint16_t count, uint8_t * buffer); //�����߶� count ������
        uint16_t                    (*write)(uint16_t count, const uint8_t *buffer); //д count �����ݵ�����
		
    }                               sequent;
    
    struct
    {
        uint16_t                    (*read)(uint16_t addr, uint16_t reg, uint16_t count, uint8_t * buffer); //�� addr �豸�� reg ��ַ�� count ������
        uint16_t                    (*write)(uint16_t addr, uint16_t reg, uint16_t count, const uint8_t *buffer); //д count �����ݵ� addr �豸�� reg ��ַ��
        
    }                               bus;
    
    enum __bus_status               (*status)(void); //����״̬
    
    struct
    {
        uint32_t                    (*get)(void); //��ȡ��ǰ�����ٶ�
        uint32_t                    (*set)(uint32_t rate); //���õ�ǰ�����ٶ�
        
    }                                speed;
};

/**
  * @brief  spi�����豸����ģ��
  */
struct __spi
{
    struct __device_ctrl            control;
    
    uint32_t                        (*read)(uint32_t count, uint8_t * buffer); //�����߶� count ���ֽ�
    uint32_t                        (*write)(uint32_t count, const uint8_t *buffer); //д count ���ֽڵ�����
    uint32_t                        (*exchange)(uint32_t count, const uint8_t *wbuffer, uint8_t * rbuffer); //���� count ���ֽڵ�����
    uint8_t                         (*select)(uint8_t cs); //Ƭѡ
    uint8_t                         (*release)(uint8_t cs); //Ƭѡ���
    enum __bus_status               (*status)(void); //����״̬
    
    struct
    {
        uint32_t                    (*read)(void); //�����߶�һ���ֽ�
        uint32_t                    (*write)(uint32_t ch); //дһ���ֽڵ�����
        uint32_t                    (*exchange)(uint32_t ch); //����һ���ֽڵ�����
        
    }                               octet;
    
    struct
    {
        uint32_t                    (*get)(void); //��ȡ����Ƶ��
        uint32_t                    (*set)(uint32_t rate); //��������Ƶ��
        
    }                                speed;
};

/**
  * @brief  eeprom�豸����ģ��
  */
struct __eeprom
{
    struct __device_ctrl            control;
    
    struct
    {
        uint32_t                    (*read)(uint32_t page, uint8_t *buffer); //��һ��ҳ
        uint32_t                    (*write)(uint32_t page, const uint8_t *buffer); //дһ��ҳ
        
    }                               page;
    
    struct
    {
        uint32_t                    (*read)(uint32_t addr, uint32_t count, uint8_t *buffer); //�� addr ��ַ�� count ������
        uint32_t                    (*write)(uint32_t addr, uint32_t count, const uint8_t *buffer); //д count �����ݵ� addr ��ַ
        
    }                               random;
    
    struct
    {
        uint32_t                    (*pagesize)(void); //ҳ��С
        uint32_t                    (*pageamount)(void); //оƬ��С
        uint32_t                    (*chipsize)(void); //оƬ��С
        
    }                               info;
    
    uint32_t                        (*erase)(void); //��������оƬ
};

/**
  * @brief  flash�豸����ģ��
  */
struct __flash
{
    struct __device_ctrl            control;
    
    struct
    {
        uint32_t                    (*read)(uint32_t block, uint8_t page, uint8_t *buffer); //��һ��ҳ
        uint32_t                    (*write)(uint32_t block, uint8_t page, const uint8_t *buffer); //дһ��ҳ
        
    }                               page;
    
    struct
    {
        uint32_t                    (*read)(uint32_t block, uint8_t *buffer); //��һ����
        uint32_t                    (*write)(uint32_t block, const uint8_t *buffer); //дһ����
        uint32_t                    (*erase)(uint32_t block); //����һ����
        
    }                               block;
    
    struct
    {
        uint32_t                    (*pagesize)(void); //ҳ��С
        uint32_t                    (*blocksize)(void); //���С
        uint32_t                    (*blockmount)(void); //������
        uint32_t                    (*chipsize)(void); //оƬ��С
        
    }                               info;
    
    uint32_t                        (*erase)(void); //��������оƬ
};

/**
  * @brief  led�豸����ģ��
  */
struct __led
{
    struct __device_ctrl            control;
    
    enum __led_status               (*get)(void); //��ȡLED��ǰ״̬
    uint8_t                         (*set)(enum __led_status value); //����LED״̬
};

/**
  * @brief  �����豸����ģ��
  */
struct __keys
{
    struct __device_ctrl            control;
    
    void                            (*runner)(uint16_t msecond); //�����ϴε����뱾�ε���֮��ļ��ʱ�䣬��λ����
    uint16_t                        (*get)(void); //��ȡ���̵�ǰ״̬
    
    //�����仯���첽�ģ�����ͨ��ͨ���ص���ʵ�ֶ������� get ����
    struct
    {
        void                        (*filling)(void(*callback)(uint16_t id, enum __key_status status)); //���ð����仯�ص�����
        void                        (*remove)(void); //��������仯�ص�����
        
    }                               handler;
};

/**
  * @brief  �̵����豸����ģ��
  */
struct __switch
{
    struct __device_ctrl            control;
    
    enum __switch_status             (*get)(void); //��ȡ��ǰ����״̬
    uint8_t                         (*set)(enum __switch_status status); //���ÿ���״̬
};

/**
  * @brief  ʱ���豸����ģ��
  */
struct __rtc
{
    struct __device_ctrl            control;
    
    uint64_t                        (*read)(void); //��ȡ��ǰӲ��ʱ��
    uint64_t                        (*write)(uint64_t stamp); //���õ�ǰӲ��ʱ��
	
    struct
    {
        uint8_t                    (*read)(uint8_t *param); //������
        uint8_t                    (*write)(const uint8_t *param); //д����
        
    }                               config;
};

/**
  * @brief  ����豸����ģ��
  */
struct __battery
{
    struct __device_ctrl            control;
    
	enum __battery_status			(*status)(void); //��ȡ���״̬
};

/**
  * @brief  �����豸����ģ��
  */
struct __misc
{
    struct __device_ctrl            control;
    
    void                            (*runner)(uint16_t msecond); //�����ϴε����뱾�ε���֮��ļ��ʱ�䣬��λ����
    
    struct
    {
        uint32_t                    (*read)(uint32_t addr, uint32_t count, void *buffer); //������
        uint32_t                    (*write)(uint32_t addr, uint32_t count, const void *buffer); //д����
		
    }                               data;
    
    struct
    {
        uint32_t                    (*read)(uint32_t addr, uint32_t count, void *buffer); //���Ĵ���
        uint32_t                    (*write)(uint32_t addr, uint32_t count, const void *buffer); //д�Ĵ���
        
    }                               config;
    
    //�����豸�Ļص�����
    struct
    {
        void                        (*filling)(void(*callback)(void *buffer)); //���û����豸�ص�����
        void                        (*remove)(void); //��������豸�ص�����
        
    }                               handler;
};

//###########################################################################
//ר���豸����ģ��

/** ����оƬ������ֻ��������Ԫ��оƬ�����ӿ�ʹ�� �����豸����ģ�� ��������Ҫ�� */

/**
  * @brief  ��������Ԫ
  */
enum __metering_meta
{
    R_EPT	 = 0x01, //�����й�����
    R_EPA	 = 0x02, //A���й�����
    R_EPB	 = 0x03, //B���й�����
    R_EPC	 = 0x04, //C���й�����

    R_EQT	 = 0x05, //�����޹�����
    R_EQA	 = 0x06, //A���޹�����
    R_EQB	 = 0x07, //B���޹�����
    R_EQC	 = 0x08, //C���޹�����

    R_EST	 = 0x09, //�������ڵ���
    R_ESA	 = 0x0A, //A�����ڵ���
    R_ESB	 = 0x0B, //B�����ڵ���
    R_ESC	 = 0x0C, //C�����ڵ���

    R_PT	 = 0x0D, //�����й�����
    R_PA	 = 0x0E, //A���й�����
    R_PB	 = 0x0F, //B���й�����
    R_PC	 = 0x10, //C���й�����

    R_QT	 = 0x11, //�����޹�����
    R_QA	 = 0x12, //A���޹�����
    R_QB	 = 0x13, //B���޹�����
    R_QC	 = 0x14, //C���޹�����

    R_ST	 = 0x15, //�������ڹ���
    R_SA	 = 0x16, //A�����ڹ���
    R_SB	 = 0x17, //B�����ڹ���
    R_SC	 = 0x18, //C�����ڹ���

    R_PFT	 = 0x19, //���๦������
    R_PFA	 = 0x1A, //A�๦������
    R_PFB	 = 0x1B, //B�๦������
    R_PFC	 = 0x1C, //C�๦������


    R_UARMS	 = 0x1D, //A���ѹ��Чֵ
    R_UBRMS	 = 0x1E, //B���ѹ��Чֵ
    R_UCRMS	 = 0x1F, //C���ѹ��Чֵ

    R_ITRMS	 = 0x20, //�������ʸ���͵���Чֵ
    R_IARMS	 = 0x21, //A�������Чֵ
    R_IBRMS	 = 0x22, //B�������Чֵ
    R_ICRMS	 = 0x23, //C�������Чֵ


    R_PGA	 = 0x24, //A��������ѹ���
    R_PGB	 = 0x25, //B��������ѹ���
    R_PGC	 = 0x26, //C��������ѹ���

    R_YUAUB	 = 0x27, //Ua��Ub�ĵ�ѹ�н�
    R_YUAUC	 = 0x28, //Ua��Uc�ĵ�ѹ�н�
    R_YUBUC	 = 0x29, //Ub��Uc�ĵ�ѹ�н�

    R_FREQ	 = 0x2A, //Ƶ��
};





/**
  * @brief  backlight
  */
enum __lcd_backlight
{
    LCD_BACKLIGHT_OFF = 0,
    LCD_BACKLIGHT_ON = 0xff,
};

/**
  * @brief  dot
  */
enum __lcd_dot
{
    LCD_DOT_NONE = 0,
    LCD_DOT_1,
	LCD_DOT_2,
	LCD_DOT_3,
	LCD_DOT_4,
	LCD_DOT_5,
	LCD_DOT_6,
	LCD_DOT_7,
	LCD_DOT_8,
    LCD_DOT_9,
    LCD_DOT_10,
    LCD_DOT_11,
    LCD_DOT_12,
};

/**
  * @brief  unit
  */
enum __lcd_unit
{
	LCD_UNIT_NONE = 0,
	
	LCD_UNIT_V,
	LCD_UNIT_KV,
	LCD_UNIT_MV,
	
	LCD_UNIT_A,
	LCD_UNIT_KA,
	LCD_UNIT_MA,
	
	LCD_UNIT_W,
	LCD_UNIT_KW,
	LCD_UNIT_MW,
	
	LCD_UNIT_VAR,
	LCD_UNIT_KVAR,
	LCD_UNIT_MVAR,
	
	LCD_UNIT_VA,
	LCD_UNIT_KVA,
	LCD_UNIT_MVA,
	
	LCD_UNIT_WH,
	LCD_UNIT_KWH,
	LCD_UNIT_MWH,
	
	LCD_UNIT_VARH,
	LCD_UNIT_KVARH,
	LCD_UNIT_MVARH,
	
	LCD_UNIT_VAH,
	LCD_UNIT_KVAH,
	LCD_UNIT_MVAH,
    
    LCD_UNIT_HZ,
    LCD_UNIT_KHZ,
    LCD_UNIT_MHZ,
};

/**
  * @brief  format of date
  */
enum __lcd_date_format
{
    //ֻ��������
	LCD_DATE_YYMMDD = 0x01,
    LCD_DATE_DDMMYY = 0x02,
    LCD_DATE_MMDDYY = 0x04,
    
    //ֻ��ʱ����
	LCD_DATE_hhmmss = 0x08,
    LCD_DATE_ssmmhh = 0x10,
	LCD_DATE_mmsshh = 0x20,
    
    //ֻ������ʱ��
	LCD_DATE_MMDDhhmm = 0x41,
    LCD_DATE_DDMMhhmm = 0x42,
    LCD_DATE_MMDDmmhh = 0x43,
    LCD_DATE_DDMMmmhh = 0x44,
};

/**
  * @brief  LCD�豸����ģ��
  */
struct __lcd
{
    struct __device_ctrl            control;
    
    void                            (*runner)(uint16_t msecond); //�����ϴε����뱾�ε���֮��ļ��ʱ�䣬��λ����
    
    struct
    {
        void                        (*none)(void); //д����
        void                        (*all)(void); //д����
        
    }                               show;
    
    struct
    {
        enum __lcd_backlight        (*open)(void);
        enum __lcd_backlight        (*close)(void);
        enum __lcd_backlight        (*status)(void);
        
    }                               backlight;
    
    struct
    {
        struct
        {
            void					(*bin)(uint8_t channel, uint16_t val, enum __lcd_dot dot, enum __lcd_unit unit); //д����
            void					(*dec)(uint8_t channel, int32_t val, enum __lcd_dot dot, enum __lcd_unit unit); //д����
            void					(*hex)(uint8_t channel, uint32_t val, enum __lcd_dot dot, enum __lcd_unit unit); //д����
            
            void					(*date)(uint8_t channel, uint64_t val, enum __lcd_date_format fmt); //д����
            
            uint8_t					(*msg)(uint8_t channel, const char *msg); //д����
            
            void                    (*none)(uint8_t channel); //д����
            void                    (*all)(uint8_t channel); //д����
            
        }                           show;
        
        void                        (*read)(uint8_t channel, void *dat); //������
        
    }                               window;
    
    struct
    {
        void                        (*on)(uint8_t channel, uint8_t state);
        void                        (*off)(uint8_t channel);
        void                        (*flash)(uint8_t channel);
        
    }                               label;
};

/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported function prototypes ----------------------------------------------*/

#endif /* __DEVICE_H__ */
