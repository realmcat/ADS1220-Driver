#ifndef ADS1220_H
#define ADS1220_H

#include <stdint.h>
#include <stdbool.h>

/* ADS1220 SPI 命令 */
#define ADS1220_RESET           0x06
#define ADS1220_START           0x08
#define ADS1220_STOP            0x0A
#define ADS1220_RDATA           0x10
#define ADS1220_RREG            0x20
#define ADS1220_WREG            0x40
#define ADS1220_SYNC            0x04

/* 寄存器地址 */
#define ADS1220_REG_CONFIG0     0x00
#define ADS1220_REG_CONFIG1     0x01
#define ADS1220_REG_CONFIG2     0x02
#define ADS1220_REG_CONFIG3     0x03
#define ADS1220_REG_IRQ         0x04

/* CONFIG0 寄存器位定义 */
#define ADS1220_MUX_MASK        0xF0
#define ADS1220_MUX_AIN0_AIN1   0x00  /* 差分输入 AIN0 vs AIN1 */
#define ADS1220_MUX_AIN0_AIN2   0x10  /* 差分输入 AIN0 vs AIN2 */
#define ADS1220_MUX_AIN0_AIN3   0x20  /* 差分输入 AIN0 vs AIN3 */
#define ADS1220_MUX_AIN1_AIN2   0x30  /* 差分输入 AIN1 vs AIN2 */
#define ADS1220_MUX_AIN1_AIN3   0x40  /* 差分输入 AIN1 vs AIN3 */
#define ADS1220_MUX_AIN2_AIN3   0x50  /* 差分输入 AIN2 vs AIN3 */
#define ADS1220_MUX_AIN1_GND    0x60  /* 单端输入 AIN1 vs GND */
#define ADS1220_MUX_AIN2_GND    0x70  /* 单端输入 AIN2 vs GND */
#define ADS1220_MUX_AIN3_GND    0x80  /* 单端输入 AIN3 vs GND */

#define ADS1220_GAIN_MASK       0x0E
#define ADS1220_GAIN_1          0x00  /* 增益 = 1 */
#define ADS1220_GAIN_2          0x02  /* 增益 = 2 */
#define ADS1220_GAIN_4          0x04  /* 增益 = 4 */
#define ADS1220_GAIN_8          0x06  /* 增益 = 8 */
#define ADS1220_GAIN_16         0x08  /* 增益 = 16 */
#define ADS1220_GAIN_32         0x0A  /* 增益 = 32 */
#define ADS1220_GAIN_64         0x0C  /* 增益 = 64 */
#define ADS1220_GAIN_128        0x0E  /* 增益 = 128 */

#define ADS1220_PGA_BYPASS      0x01  /* PGA 旁路 */

/* CONFIG1 寄存器位定义 */
#define ADS1220_DR_MASK         0xE0  /* 数据速率掩码 */
#define ADS1220_DR_20           0x00  /* 20 SPS */
#define ADS1220_DR_45           0x20  /* 45 SPS */
#define ADS1220_DR_90           0x40  /* 90 SPS */
#define ADS1220_DR_175          0x60  /* 175 SPS */
#define ADS1220_DR_330          0x80  /* 330 SPS */
#define ADS1220_DR_600          0xA0  /* 600 SPS */
#define ADS1220_DR_1000         0xC0  /* 1000 SPS */

#define ADS1220_MODE_MASK       0x18  /* 工作模式掩码 */
#define ADS1220_MODE_NORMAL     0x00  /* 正常模式 */
#define ADS1220_MODE_DUTY       0x08  /* 占空比模式 */
#define ADS1220_MODE_TURBO      0x10  /* 涡轮模式 */

#define ADS1220_FILTER_MASK     0x06  /* 滤波器掩码 */
#define ADS1220_FILTER_SINC1    0x00  /* SINC1 滤波器 */
#define ADS1220_FILTER_SINC2    0x02  /* SINC2 滤波器 */
#define ADS1220_FILTER_SINC3    0x04  /* SINC3 滤波器 */
#define ADS1220_FILTER_SINC4    0x06  /* SINC4 滤波器 */

#define ADS1220_ENABLE_REF      0x01  /* 使能参考电压 */

/* CONFIG2 寄存器位定义 */
#define ADS1220_REF_SEL_MASK    0xC0  /* 参考源选择掩码 */
#define ADS1220_REF_INTERNAL    0x00  /* 内部参考 */
#define ADS1220_REF_EXTERNAL1   0x40  /* 外部参考1 */
#define ADS1220_REF_EXTERNAL2   0x80  /* 外部参考2 */

#define ADS1220_VREF_MASK       0x20  /* 参考电压掩码 */
#define ADS1220_VREF_2048       0x00  /* 2.048V 参考 */
#define ADS1220_VREF_4096       0x20  /* 4.096V 参考 */

#define ADS1220_TS_MASK         0x10  /* 温度传感器掩码 */
#define ADS1220_PSENSE_MASK     0x08  /* 压力传感器掩码 */
#define ADS1220_IDAC_MASK       0x07  /* IDAC 输出电流掩码 */
#define ADS1220_IDAC_OFF        0x00  /* IDAC 关闭 */
#define ADS1220_IDAC_10         0x01  /* IDAC 10 µA */
#define ADS1220_IDAC_50         0x02  /* IDAC 50 µA */
#define ADS1220_IDAC_100        0x03  /* IDAC 100 µA */
#define ADS1220_IDAC_500        0x04  /* IDAC 500 µA */
#define ADS1220_IDAC_1000       0x05  /* IDAC 1000 µA */
#define ADS1220_IDAC_1500       0x06  /* IDAC 1500 µA */

/* 数据结构 */
typedef struct {
    uint8_t config0;
    uint8_t config1;
    uint8_t config2;
    uint8_t config3;
} ADS1220_Config;

/* ADC 数据结构 */
typedef struct {
    int32_t raw_data;    /* 原始ADC数据 */
    float voltage;       /* 转换后的电压值 */
} ADS1220_Data;

/* SPI 通信回调函数类型 */
typedef int32_t (*ads1220_spi_read)(uint8_t* buffer, uint32_t length);
typedef int32_t (*ads1220_spi_write)(uint8_t* buffer, uint32_t length);
typedef void (*ads1220_delay_ms)(uint32_t ms);

/* 驱动句柄结构 */
typedef struct {
    ads1220_spi_read spi_read;      /* SPI 读函数 */
    ads1220_spi_write spi_write;    /* SPI 写函数 */
    ads1220_delay_ms delay_ms;      /* 延时函数 */
    ADS1220_Config config;          /* 配置寄存器 */
    float vref;                     /* 参考电压 */
} ADS1220_Handle;

/* API 函数声明 */
/**
 * @brief 初始化 ADS1220 驱动
 * @param handle 驱动句柄指针
 * @param vref 参考电压（通常为 2.048V 或 4.096V）
 * @return 成功返回 0，失败返回 -1
 */
int32_t ADS1220_Init(ADS1220_Handle* handle, float vref);

/**
 * @brief 复位 ADS1220 芯片
 * @param handle 驱动句柄指针
 * @return 成功返回 0，失败返回 -1
 */
int32_t ADS1220_Reset(ADS1220_Handle* handle);

/**
 * @brief 写寄存器
 * @param handle 驱动句柄指针
 * @param reg 寄存器地址
 * @param value 寄存器值
 * @return 成功返回 0，失败返回 -1
 */
int32_t ADS1220_WriteReg(ADS1220_Handle* handle, uint8_t reg, uint8_t value);

/**
 * @brief 读寄存器
 * @param handle 驱动句柄指针
 * @param reg 寄存器地址
 * @param value 寄存器值指针
 * @return 成功返回 0，失败返回 -1
 */
int32_t ADS1220_ReadReg(ADS1220_Handle* handle, uint8_t reg, uint8_t* value);

/**
 * @brief 写所有配置寄存器
 * @param handle 驱动句柄指针
 * @return 成功返回 0，失败返回 -1
 */
int32_t ADS1220_WriteConfig(ADS1220_Handle* handle);

/**
 * @brief 设置多路选择通道
 * @param handle 驱动句柄指针
 * @param mux 多路选择值
 * @return 成功返回 0，失败返回 -1
 */
int32_t ADS1220_SetMux(ADS1220_Handle* handle, uint8_t mux);

/**
 * @brief 设置 PGA 增益
 * @param handle 驱动句柄指针
 * @param gain 增益值
 * @return 成功返回 0，失败返回 -1
 */
int32_t ADS1220_SetGain(ADS1220_Handle* handle, uint8_t gain);

/**
 * @brief 设置数据速率
 * @param handle 驱动句柄指针
 * @param dr 数据速率值
 * @return 成功返回 0，失败返回 -1
 */
int32_t ADS1220_SetDataRate(ADS1220_Handle* handle, uint8_t dr);

/**
 * @brief 设置数字滤波器
 * @param handle 驱动句柄指针
 * @param filter 滤波器类型
 * @return 成功返回 0，失败返回 -1
 */
int32_t ADS1220_SetFilter(ADS1220_Handle* handle, uint8_t filter);

/**
 * @brief 启动转换
 * @param handle 驱动句柄指针
 * @return 成功返回 0，失败返回 -1
 */
int32_t ADS1220_Start(ADS1220_Handle* handle);

/**
 * @brief 停止转换
 * @param handle 驱动句柄指针
 * @return 成功返回 0，失败返回 -1
 */
int32_t ADS1220_Stop(ADS1220_Handle* handle);

/**
 * @brief 读取 ADC 数据（单次模式）
 * @param handle 驱动句柄指针
 * @param data ADC 数据指针
 * @return 成功返回 0，失败返回 -1
 */
int32_t ADS1220_ReadData(ADS1220_Handle* handle, ADS1220_Data* data);

/**
 * @brief 读取 ADC 原始数据（连续模式）
 * @param handle 驱动句柄指针
 * @param raw_data 原始数据指针
 * @return 成功返回 0，失败返回 -1
 */
int32_t ADS1220_ReadDataContinuous(ADS1220_Handle* handle, int32_t* raw_data);

/**
 * @brief 将原始 ADC 数据转换为电压值
 * @param raw_data 原始 ADC 数据
 * @param vref 参考电压
 * @param gain 增益索引（0-7）
 * @return 电压值（单位：伏）
 */
float ADS1220_RawToVoltage(int32_t raw_data, float vref, uint8_t gain);

#endif /* ADS1220_H */
