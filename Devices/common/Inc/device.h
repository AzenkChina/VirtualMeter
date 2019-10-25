/**
 * @brief		设备驱动模型
 * @details		本文件里将会给所有的设备进行建模，所有的外设都要遵循
 *              设备模型来编写其驱动
 * @date		azenk@2016-11-15
 **/


/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __DEVICE_H__
#define __DEVICE_H__

/* Includes ------------------------------------------------------------------*/
#include "stdint.h"
#include "string.h"
#include "stdbool.h"
#include "device_lowlevel.h"

/* Exported types ------------------------------------------------------------*/
     
//###########################################################################
//设备驱动控制单元（基类）
     
/**
  * @brief  设备当前状态
  */
enum __dev_status
{
    DEVICE_NOTINIT = 0, //设备未初始化
    DEVICE_INIT, //设备已初始化
    DEVICE_SUSPENDED, //设备已挂起
    DEVICE_MISC, //设备状态混乱
    DEVICE_ERROR, //设备出错
};

/**
  * @brief  设备初始化模式
  */
enum __dev_state
{
    DEVICE_NORMAL = 0x03,//正常初始化
    DEVICE_LOWPOWER = 0x0c,//低功耗初始化
};

/**
  * @brief  驱动模型设备控制单元，每种驱动模型中的第一个结构必须是 设备控制单元
  */
struct __device_ctrl
{
    const char						*name;
    enum __dev_status               (*status)(void); //设备当前状态
    void                            (*init)(enum __dev_state state); //初始化本设备
    void                            (*suspend)(void); //挂起本设备
};




//###########################################################################
//通用设备驱动模型

/**
  * @brief  按键状态
  */
enum __key_status
{
    KEY_NONE = 0, //按键没有按下
    KEY_PRESS, //按键按下
    KEY_RELEASE, //按键抬起
    KEY_LONG_PRESS, //按键长按下
    KEY_LONG_RELEASE, //按键长按抬起
};

/**
  * @brief  总线当前状态
  */
enum __bus_status
{
    BUS_IDLE = 0, //总线空闲
    BUS_BUSY, //总线忙碌
    BUS_TRANSFER, //总线发送数据中
    BUS_RECEIVE, //总线接收数据中
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
  * @brief  串行总线工作模式 
  */
enum __serial_mode
{
    SERIAL_IN = 0, //仅输入
    SERIAL_OUT, //仅输出
    SERIAL_AUTO, //自动（双向）
};


/**
  * @brief  LED状态
  */
enum __led_status
{
    LED_OFF = 0, //led关闭
    LED_ON, //led打开
    LED_DONTCARE, //不关心
    LED_UNKNOWN, //led状态未知
};

/**
  * @brief  开关状态
  */
enum __switch_status
{
    SWITCH_OPEN = 0, //断开
    SWITCH_CLOSE, //闭合
    SWITCH_DONTCARE, //不关心
    SWITCH_UNKNOWN, //状态未知
};

/**
  * @brief  电池状态
  */
enum __battery_status
{
    BAT_FULL = 0, //电池满状态
    BAT_LOW, //电池不足
    BAT_EMPTY, //电池空
};

/**
  * @brief  usart设备驱动模型
  */
struct __uart
{
    struct __device_ctrl            control;
    
    uint16_t                        (*write)(uint16_t count, const uint8_t *buffer); //写数据
    enum __bus_status               (*status)(void); //总线状态
    
    struct
    {
    	enum __baud                 (*get)(void); //获取当前波特率
        enum __baud                 (*set)(enum __baud baudrate); //设置波特率
        
    }                               baudrate;
    
    struct
    {
        enum __parity				(*get)(void); //获取当前校验位
        enum __parity				(*set)(enum __parity parity); //设置校验位
        
    }                                parity;
    
    struct
    {
        enum __stop					(*get)(void); //获取当前停止位
        enum __stop					(*set)(enum __stop stop); //设置停止位
        
    }                                stop;
    
    //usart的接收过程是异步的，所以只能通过回调函数来实现数据的接收
    struct
    {
        void                        (*filling)(void(*callback)(uint8_t ch)); //添加字节接收回调函数
        void                        (*remove)(void); //清除字节接收回调函数
        
    }                               handler;
    
};

/**
  * @brief  serial备驱动模型
  */
struct __serial
{
    struct __device_ctrl            control;
    
    void                            (*runner)(uint16_t msecond); //输入上次调用与本次调用之间的间隔时间，单位毫秒
    uint16_t                        (*read)(uint16_t size, uint8_t *buffer); //读一帧数据
    uint16_t                        (*write)(uint16_t count); //写一帧数据
    enum __bus_status               (*status)(void); //总线状态
    
    struct
    {
        uint16_t					(*get)(uint8_t **buffer); //获取当前发送缓冲区首地址
        void                        (*set)(uint16_t size, uint8_t *buffer); //设置接收缓冲区
        void                        (*remove)(void); //删除缓冲区
        
    }                               rxbuff;
    
    struct
    {
		uint16_t					(*get)(uint8_t **buffer); //获取当前发送缓冲区首地址
		void                        (*set)(uint16_t size, uint8_t *buffer); //设置缓冲区
        void                        (*remove)(void); //删除缓冲区
        
    }                               txbuff;
    
    struct
    {
        enum __serial_mode			(*get)(void); //读取当前工作模式
        enum __serial_mode			(*set)(enum __serial_mode mode); //设置工作模式
        
    }                               mode;
    
    struct
    {
        uint16_t                    (*get)(void); //获取当前帧超时时间
        uint16_t                    (*set)(uint16_t msecond); //设置帧超时时间
        
    }                               timeout;
    
    const struct __uart			*uart; //serial对应的usart
};

#if defined ( _WIN32 ) || defined ( _WIN64 ) || defined ( __linux )
/**
  * @brief  console设备驱动模型
  */
struct __console
{
    struct __device_ctrl            control;
    
    struct
    {
        void                        (*filling)(void(*)(const char *)); //添加回调函数
        void                        (*remove)(void); //清除回调函数
        
    }                               handler;
};
#endif

/**
  * @brief  iic总线设备驱动模型
  */
struct __iic
{
    struct __device_ctrl            control;
    
    struct
    {
        uint8_t                     (*read)(void); //从总线读一个字节
        uint8_t                     (*write)(uint8_t ch); //写一个字节到总线
        
    }                               octet;
    
    struct
    {
        uint16_t                    (*read)(uint16_t count, uint8_t * buffer); //从总线读 count 个数据
        uint16_t                    (*write)(uint16_t count, const uint8_t *buffer); //写 count 个数据到总线
		
    }                               sequent;
    
    struct
    {
        uint16_t                    (*read)(uint16_t addr, uint32_t reg, uint8_t reglen, uint16_t count, uint8_t * buffer); //从 addr 设备的 reg 地址读 count 个数据
        uint16_t                    (*write)(uint16_t addr, uint32_t reg, uint8_t reglen, uint16_t count, const uint8_t *buffer); //写 count 个数据到 addr 设备的 reg 地址处
        
    }                               bus;
    
    enum __bus_status               (*status)(void); //总线状态
    
    struct
    {
        uint32_t                    (*get)(void); //获取当前总线速度
        uint32_t                    (*set)(uint32_t rate); //设置当前总线速度
        
    }                                speed;
};

/**
  * @brief  spi总线设备驱动模型
  */
struct __spi
{
    struct __device_ctrl            control;
    
    uint32_t                        (*read)(uint32_t count, uint8_t * buffer); //从总线读 count 个字节
    uint32_t                        (*write)(uint32_t count, const uint8_t *buffer); //写 count 个字节到总线
    uint32_t                        (*exchange)(uint32_t count, const uint8_t *wbuffer, uint8_t * rbuffer); //交换 count 个字节到总线
    uint8_t                         (*select)(uint8_t cs); //片选
    uint8_t                         (*release)(uint8_t cs); //片选解除
    enum __bus_status               (*status)(void); //总线状态
    
    struct
    {
        uint32_t                    (*read)(void); //从总线读一个字节
        uint32_t                    (*write)(uint32_t ch); //写一个字节到总线
        uint32_t                    (*exchange)(uint32_t ch); //交换一个字节到总线
        
    }                               octet;
    
    struct
    {
        uint32_t                    (*get)(void); //获取总线频率
        uint32_t                    (*set)(uint32_t rate); //设置总线频率
        
    }                                speed;
};

/**
  * @brief  eeprom设备驱动模型
  */
struct __eeprom
{
    struct __device_ctrl            control;
    
    struct
    {
        uint32_t                    (*read)(uint32_t page, uint16_t offset, uint16_t size, uint8_t *buffer); //读一个页内地址
        uint32_t                    (*write)(uint32_t page, uint16_t offset, uint16_t size, const uint8_t *buffer); //写一个页内地址
        
    }                               page;
    
    struct
    {
        uint32_t                    (*pagesize)(void); //页大小
        uint32_t                    (*pageamount)(void); //芯片大小
        uint32_t                    (*chipsize)(void); //芯片大小
        
    }                               info;
    
    uint32_t                        (*erase)(void); //擦除整个芯片
};

/**
  * @brief  flash设备驱动模型
  */
struct __flash
{
    struct __device_ctrl            control;
    
    struct
    {
        uint32_t                    (*read)(uint32_t block, uint16_t offset, uint16_t size, uint8_t *buffer); //读一个块
        uint32_t                    (*write)(uint32_t block, uint16_t offset, uint16_t size, const uint8_t *buffer); //写一个块
        uint32_t                    (*erase)(uint32_t block); //擦除一个块
        
    }                               block;
    
    struct
    {
        uint32_t                    (*blocksize)(void); //块大小
        uint32_t                    (*blockcount)(void); //块数量
        uint32_t                    (*chipsize)(void); //芯片大小
        
    }                               info;
    
    uint32_t                        (*erase)(void); //擦除整个芯片
};

/**
  * @brief  led设备驱动模型
  */
struct __led
{
    struct __device_ctrl            control;
    
    enum __led_status               (*get)(void); //获取LED当前状态
    uint8_t                         (*set)(enum __led_status value); //设置LED状态
};

/**
  * @brief  按键设备驱动模型
  */
struct __keys
{
    struct __device_ctrl            control;
    
    void                            (*runner)(uint16_t msecond); //输入上次调用与本次调用之间的间隔时间，单位毫秒
    uint16_t                        (*get)(void); //获取键盘当前状态
    
    //按键变化是异步的，所以通常通过回调来实现而不是用 get 方法
    struct
    {
        void                        (*filling)(void(*callback)(uint16_t id, enum __key_status status)); //设置按键变化回调函数
        void                        (*remove)(void); //清除按键变化回调函数
        
    }                               handler;
};

/**
  * @brief  继电器设备驱动模型
  */
struct __switch
{
    struct __device_ctrl            control;
    
    void                            (*runner)(uint16_t msecond); //输入上次调用与本次调用之间的间隔时间，单位毫秒
    enum __switch_status             (*get)(void); //获取当前开关状态
    uint8_t                         (*set)(enum __switch_status status); //设置开关状态
};

/**
  * @brief  时钟设备驱动模型
  */
struct __rtc
{
    struct __device_ctrl            control;
    
    uint64_t                        (*read)(void); //获取当前硬件时间
    uint64_t                        (*write)(uint64_t stamp); //设置当前硬件时间
	
    struct
    {
        uint8_t                    (*read)(uint8_t addr, uint8_t count, uint8_t *param); //读参数
        uint8_t                    (*write)(uint8_t addr, uint8_t count, const uint8_t *param); //写参数
        
    }                               config;
};

/**
  * @brief  电池设备驱动模型
  */
struct __battery
{
    struct __device_ctrl            control;
    
    struct
    {
        uint32_t                    (*capacity)(void); //额定电池容量mA·H
        uint32_t                    (*voltage)(void); //额定电池电压mV
        
    }                               rated;
    
    uint32_t                        (*voltage)(void); //电池电压mV
	enum __battery_status           (*status)(void); //获取电池剩余百分百
};

//###########################################################################
//专用设备驱动模型


/** LCD 设备模型 */

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
    //只有年月日
	LCD_DATE_YYMMDD = 0x01,
    LCD_DATE_DDMMYY = 0x02,
    LCD_DATE_MMDDYY = 0x04,
    
    //只有时分秒
	LCD_DATE_hhmmss = 0x08,
    LCD_DATE_ssmmhh = 0x10,
	LCD_DATE_mmsshh = 0x20,
    
    //只有月日时分
	LCD_DATE_MMDDhhmm = 0x41,
    LCD_DATE_DDMMhhmm = 0x42,
    LCD_DATE_MMDDmmhh = 0x43,
    LCD_DATE_DDMMmmhh = 0x44,
};

/**
  * @brief  LCD设备驱动模型
  */
struct __lcd
{
    struct __device_ctrl            control;
    
    void                            (*runner)(uint16_t msecond); //输入上次调用与本次调用之间的间隔时间，单位毫秒
    
    struct
    {
        void                        (*none)(void); //写数据
        void                        (*all)(void); //写数据
        
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
            void					(*bin)(uint8_t channel, uint16_t val, enum __lcd_dot dot, enum __lcd_unit unit); //写数据
            void					(*dec)(uint8_t channel, int32_t val, enum __lcd_dot dot, enum __lcd_unit unit); //写数据
            void					(*hex)(uint8_t channel, uint32_t val, enum __lcd_dot dot, enum __lcd_unit unit); //写数据
            
            void					(*date)(uint8_t channel, uint64_t val, enum __lcd_date_format fmt); //写数据
            
            uint8_t					(*msg)(uint8_t channel, uint8_t from, const char *msg); //写数据
            
            void                    (*none)(uint8_t channel); //写数据
            void                    (*all)(uint8_t channel); //写数据
            
        }                           show;
        
        void                        (*read)(uint8_t channel, void *dat); //读数据
        
    }                               window;
    
    struct
    {
        void                        (*on)(uint8_t channel, uint8_t state);
        void                        (*off)(uint8_t channel);
        void                        (*flash)(uint8_t channel);
        
    }                               label;
};






/** EMU 模型 */

/**
  * @brief  计量数据元
  */
enum __metering_meta
{
    R_EPT	 = 0x01, //合相有功电能
    R_EPA	 = 0x02, //A相有功电能
    R_EPB	 = 0x03, //B相有功电能
    R_EPC	 = 0x04, //C相有功电能

    R_EQT	 = 0x05, //合相无功电能
    R_EQA	 = 0x06, //A相无功电能
    R_EQB	 = 0x07, //B相无功电能
    R_EQC	 = 0x08, //C相无功电能

    R_EST	 = 0x09, //合相视在电能
    R_ESA	 = 0x0A, //A相视在电能
    R_ESB	 = 0x0B, //B相视在电能
    R_ESC	 = 0x0C, //C相视在电能

    R_PT	 = 0x0D, //合相有功功率
    R_PA	 = 0x0E, //A相有功功率
    R_PB	 = 0x0F, //B相有功功率
    R_PC	 = 0x10, //C相有功功率

    R_QT	 = 0x11, //合相无功功率
    R_QA	 = 0x12, //A相无功功率
    R_QB	 = 0x13, //B相无功功率
    R_QC	 = 0x14, //C相无功功率

    R_ST	 = 0x15, //合相视在功率
    R_SA	 = 0x16, //A相视在功率
    R_SB	 = 0x17, //B相视在功率
    R_SC	 = 0x18, //C相视在功率

    R_PFT	 = 0x19, //合相功率因数
    R_PFA	 = 0x1A, //A相功率因数
    R_PFB	 = 0x1B, //B相功率因数
    R_PFC	 = 0x1C, //C相功率因数


    R_UA	 = 0x1D, //A相电压
    R_UB	 = 0x1E, //B相电压
    R_UC	 = 0x1F, //C相电压

    R_IT	 = 0x20, //三相电流矢量和
    R_IA	 = 0x21, //A相电流
    R_IB	 = 0x22, //B相电流
    R_IC	 = 0x23, //C相电流


    R_YIA	 = 0x24, //A相电流与电压相角
    R_YIB	 = 0x25, //B相电流与电压相角
    R_YIC	 = 0x26, //C相电流与电压相角

    R_YUAUB	 = 0x27, //Ua与Ub的电压夹角
    R_YUAUC	 = 0x28, //Ua与Uc的电压夹角
    R_YUBUC	 = 0x29, //Ub与Uc的电压夹角

    R_FREQ	 = 0x2A, //频率
};

/**
  * @brief  EMU设备驱动模型
  */
struct __meter
{
    struct __device_ctrl            control;
    
    void                            (*runner)(uint16_t msecond); //输入上次调用与本次调用之间的间隔时间，单位毫秒
    
	int32_t							(*read)(enum __metering_meta id); //读数据
    
    struct
    {
		bool						(*load)(uint32_t size, const void *param); //加载校准参数
		
		bool						(*enter)(uint32_t size, void *args); //进入校准
		bool						(*status)(void); //校准是否完成（校准模式下）/ 校准参数是否正常（运行模式下）
		bool						(*exit)(void); //退出校准
		
    }                               calibrate;
    
    //回调函数，用于抛出异常
    struct
    {
        void                        (*filling)(void(*callback)(void *buffer)); //设置混杂设备回调函数
        void                        (*remove)(void); //清除混杂设备回调函数
        
    }                               handler;
};

/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
#define DEVICE_MATCH(dev, s)     ((strstr(dev.control.name, s) == NULL)?false:true)

/* Exported function prototypes ----------------------------------------------*/

#endif /* __DEVICE_H__ */
