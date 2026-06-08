/**
 * ADS1220 驱动程序使用示例
 * 
 * 本文件演示如何使用 ADS1220 ADC 驱动程序
 * 请根据您的平台调整 SPI 和延时的实现
 */

#include "ads1220.h"
#include <stdio.h>

/* 平台特定的实现（应由您的 HAL 提供） */
#include "platform.h"  /* 包含 SPI 和延时实现 */

/* ADS1220 全局句柄 */
ADS1220_Handle ads1220_handle;

/**
 * @brief 初始化 ADS1220 并设置平台特定回调
 */
int32_t ADS1220_PlatformInit(void)
{
    /* 设置回调函数 */
    ads1220_handle.spi_read = HAL_SPI_Read;      /* 从 SPI 总线读取 */
    ads1220_handle.spi_write = HAL_SPI_Write;    /* 写入 SPI 总线 */
    ads1220_handle.delay_ms = HAL_Delay_Ms;     /* 毫秒延时 */

    /* 使用 2.048V 参考电压初始化驱动 */
    if (ADS1220_Init(&ads1220_handle, 2.048f) != ADS1220_OK) {
        printf("ADS1220 初始化失败\n");
        return -1;
    }

    return 0;
}

/**
 * @brief 配置 ADS1220 用于测量差分电压（AIN0-AIN1）
 */
int32_t ADS1220_ConfigureMeasurement(void)
{
    /* 设置多路选择器为 AIN0 和 AIN1 */
    if (ADS1220_SetMux(&ads1220_handle, ADS1220_MUX_AIN0_AIN1) != ADS1220_OK) {
        printf("设置多路选择器失败\n");
        return -1;
    }

    /* 设置增益为 1（更宽的输入范围） */
    if (ADS1220_SetGain(&ads1220_handle, ADS1220_GAIN_1) != ADS1220_OK) {
        printf("设置增益失败\n");
        return -1;
    }

    /* 设置数据速率为 330 SPS（每秒采样数） */
    if (ADS1220_SetDataRate(&ads1220_handle, ADS1220_DR_330) != ADS1220_OK) {
        printf("设置数据速率失败\n");
        return -1;
    }

    /* 设置滤波器为 SINC3（更好的噪声性能） */
    if (ADS1220_SetFilter(&ads1220_handle, ADS1220_FILTER_SINC3) != ADS1220_OK) {
        printf("设置滤波器失败\n");
        return -1;
    }

    return 0;
}

/**
 * @brief 读取单个 ADC 数据（单次转换模式）
 */
int32_t ADS1220_ReadSingleSample(void)
{
    ADS1220_Data data;

    /* 启动转换 */
    if (ADS1220_Start(&ads1220_handle) != ADS1220_OK) {
        printf("启动转换失败\n");
        return -1;
    }

    /* 等待转换完成（根据数据速率和滤波器调整） */
    HAL_Delay_Ms(10);  /* 根据数据速率调整 */

    /* 读取转换后的数据 */
    if (ADS1220_ReadData(&ads1220_handle, &data) != ADS1220_OK) {
        printf("读取数据失败\n");
        return -1;
    }

    /* 打印结果 */
    printf("原始 ADC 值: %d\n", data.raw_data);
    printf("电压: %.6f V\n", data.voltage);

    return 0;
}

/**
 * @brief 连续读取 ADC 数据
 */
int32_t ADS1220_ReadContinuous(uint32_t num_samples)
{
    uint32_t i;
    int32_t raw_data;
    float voltage;
    uint8_t gain_index;

    /* 启动连续转换 */
    if (ADS1220_Start(&ads1220_handle) != ADS1220_OK) {
        printf("启动连续转换失败\n");
        return -1;
    }

    /* 获取增益以进行电压转换 */
    gain_index = (ads1220_handle.config.config0 & ADS1220_GAIN_MASK) >> 1;
    if (gain_index >= 8) {
        gain_index = 0;
    }

    /* 读取多个样本 */
    for (i = 0; i < num_samples; i++) {
        /* 等待新数据可用 */
        HAL_Delay_Ms(5);  /* 根据数据速率调整 */

        if (ADS1220_ReadDataContinuous(&ads1220_handle, &raw_data) != ADS1220_OK) {
            printf("读取连续数据失败\n");
            break;
        }

        /* 转换为电压 */
        voltage = ADS1220_RawToVoltage(raw_data, ads1220_handle.vref, gain_index);

        printf("样本 %u: 原始=%d, 电压=%.6f V\n", i+1, raw_data, voltage);
    }

    /* 停止连续转换 */
    ADS1220_Stop(&ads1220_handle);

    return 0;
}

/**
 * @brief 示例：使用内置温度传感器测量温度
 */
int32_t ADS1220_MeasureTemperature(void)
{
    ADS1220_Data data;

    /* 在 config2 中使能温度传感器 */
    ads1220_handle.config.config2 |= 0x10;  /* TS 位 */
    ADS1220_WriteReg(&ads1220_handle, ADS1220_REG_CONFIG2, ads1220_handle.config.config2);

    /* 设置多路选择为 AIN3/GND 用于温度读取 */
    ADS1220_SetMux(&ads1220_handle, ADS1220_MUX_AIN3_GND);

    /* 设置适当的增益 */
    ADS1220_SetGain(&ads1220_handle, ADS1220_GAIN_1);

    /* 启动和读取 */
    ADS1220_Start(&ads1220_handle);
    HAL_Delay_Ms(10);

    if (ADS1220_ReadData(&ads1220_handle, &data) != ADS1220_OK) {
        printf("读取温度失败\n");
        return -1;
    }

    /* 温度传感器输出约为 1 LSB / 0.0625°C */
    /* 使用 VREF = 2.048V 和增益 = 1 时，LSB = 2.048V / (2^23) */
    float temperature = (float)data.raw_data / 128.0f;  /* 简化转换 */

    printf("温度: %.2f °C\n", temperature);

    return 0;
}

/**
 * @brief 示例：依次读取所有通道
 */
int32_t ADS1220_ReadAllChannels(void)
{
    ADS1220_Data data;
    uint8_t channels[] = {
        ADS1220_MUX_AIN0_GND,
        ADS1220_MUX_AIN1_GND,
        ADS1220_MUX_AIN2_GND,
        ADS1220_MUX_AIN3_GND
    };
    const char* channel_names[] = {"AIN0", "AIN1", "AIN2", "AIN3"};
    uint32_t i;

    for (i = 0; i < 4; i++) {
        /* 切换到下一个通道 */
        ADS1220_SetMux(&ads1220_handle, channels[i]);

        /* 等待多路选择器稳定时间 */
        HAL_Delay_Ms(5);

        /* 启动转换 */
        ADS1220_Start(&ads1220_handle);

        /* 等待转换 */
        HAL_Delay_Ms(10);

        /* 读取数据 */
        if (ADS1220_ReadData(&ads1220_handle, &data) != ADS1220_OK) {
            printf("读取通道 %s 失败\n", channel_names[i]);
            continue;
        }

        printf("%s: %.6f V\n", channel_names[i], data.voltage);
    }

    return 0;
}

/**
 * @brief 主函数，演示使用方法
 */
int main(void)
{
    /* 初始化平台（SPI、GPIO 等） */
    if (HAL_Init() != 0) {
        printf("平台初始化失败\n");
        return 1;
    }

    /* 初始化 ADS1220 驱动 */
    if (ADS1220_PlatformInit() != 0) {
        printf("ADS1220 初始化失败\n");
        return 1;
    }

    /* 配置测量参数 */
    if (ADS1220_ConfigureMeasurement() != 0) {
        printf("ADS1220 配置失败\n");
        return 1;
    }

    printf("=== ADS1220 驱动程序示例 ===\n\n");

    /* 示例 1：读取单个样本 */
    printf("--- 单个样本读取 ---\n");
    ADS1220_ReadSingleSample();
    printf("\n");

    /* 示例 2：连续读取多个样本 */
    printf("--- 连续读取（10 个样本） ---\n");
    ADS1220_ReadContinuous(10);
    printf("\n");

    /* 示例 3：读取所有通道 */
    printf("--- 多通道读取 ---\n");
    ADS1220_ReadAllChannels();
    printf("\n");

    /* 示例 4：读取温度传感器 */
    printf("--- 温度传感器读取 ---\n");
    ADS1220_MeasureTemperature();
    printf("\n");

    /* 清理资源 */
    HAL_Deinit();

    return 0;
}
