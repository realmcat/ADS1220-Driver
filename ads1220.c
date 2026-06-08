#include "ads1220.h"

/* 错误代码 */
#define ADS1220_OK          0   /* 成功 */
#define ADS1220_ERROR      -1   /* 错误 */
#define ADS1220_NULL_PTR   -2   /* 空指针 */

/* 增益值数组 */
static const float gain_values[] = {1.0, 2.0, 4.0, 8.0, 16.0, 32.0, 64.0, 128.0};

/**
 * @brief 初始化 ADS1220 驱动
 * @param handle 指向 ADS1220_Handle 结构的指针
 * @param vref 参考电压（通常为 2.048V 或 4.096V）
 * @return 成功返回 ADS1220_OK，失败返回 ADS1220_ERROR
 */
int32_t ADS1220_Init(ADS1220_Handle* handle, float vref)
{
    if (handle == NULL) {
        return ADS1220_NULL_PTR;
    }

    handle->vref = vref;

    /* 初始化默认配置 */
    handle->config.config0 = ADS1220_MUX_AIN0_AIN1 | ADS1220_GAIN_1;
    handle->config.config1 = ADS1220_DR_330 | ADS1220_MODE_NORMAL | ADS1220_FILTER_SINC1;
    handle->config.config2 = ADS1220_REF_INTERNAL | ADS1220_VREF_2048;
    handle->config.config3 = 0x00;

    /* 复位设备 */
    if (ADS1220_Reset(handle) != ADS1220_OK) {
        return ADS1220_ERROR;
    }

    /* 写入配置 */
    if (ADS1220_WriteConfig(handle) != ADS1220_OK) {
        return ADS1220_ERROR;
    }

    return ADS1220_OK;
}

/**
 * @brief 复位 ADS1220 设备
 * @param handle 指向 ADS1220_Handle 结构的指针
 * @return 成功返回 ADS1220_OK，失败返回 ADS1220_ERROR
 */
int32_t ADS1220_Reset(ADS1220_Handle* handle)
{
    uint8_t cmd = ADS1220_RESET;

    if (handle == NULL || handle->spi_write == NULL) {
        return ADS1220_NULL_PTR;
    }

    if (handle->spi_write(&cmd, 1) != 1) {
        return ADS1220_ERROR;
    }

    /* 等待复位完成 */
    if (handle->delay_ms != NULL) {
        handle->delay_ms(1);
    }

    return ADS1220_OK;
}

/**
 * @brief 写寄存器值
 * @param handle 指向 ADS1220_Handle 结构的指针
 * @param reg 寄存器地址
 * @param value 寄存器值
 * @return 成功返回 ADS1220_OK，失败返回 ADS1220_ERROR
 */
int32_t ADS1220_WriteReg(ADS1220_Handle* handle, uint8_t reg, uint8_t value)
{
    uint8_t cmd[3];

    if (handle == NULL || handle->spi_write == NULL) {
        return ADS1220_NULL_PTR;
    }

    cmd[0] = ADS1220_WREG | (reg << 2);
    cmd[1] = 0;  /* 写入寄存器个数 - 1 */
    cmd[2] = value;

    if (handle->spi_write(cmd, 3) != 3) {
        return ADS1220_ERROR;
    }

    return ADS1220_OK;
}

/**
 * @brief 读寄存器值
 * @param handle 指向 ADS1220_Handle 结构的指针
 * @param reg 寄存器地址
 * @param value 存储寄存器值的指针
 * @return 成功返回 ADS1220_OK，失败返回 ADS1220_ERROR
 */
int32_t ADS1220_ReadReg(ADS1220_Handle* handle, uint8_t reg, uint8_t* value)
{
    uint8_t cmd[3];

    if (handle == NULL || handle->spi_read == NULL || handle->spi_write == NULL || value == NULL) {
        return ADS1220_NULL_PTR;
    }

    cmd[0] = ADS1220_RREG | (reg << 2);
    cmd[1] = 0;  /* 读取寄存器个数 - 1 */

    if (handle->spi_write(cmd, 2) != 2) {
        return ADS1220_ERROR;
    }

    if (handle->spi_read(cmd, 1) != 1) {
        return ADS1220_ERROR;
    }

    *value = cmd[0];
    return ADS1220_OK;
}

/**
 * @brief 写入所有配置寄存器
 * @param handle 指向 ADS1220_Handle 结构的指针
 * @return 成功返回 ADS1220_OK，失败返回 ADS1220_ERROR
 */
int32_t ADS1220_WriteConfig(ADS1220_Handle* handle)
{
    uint8_t cmd[6];

    if (handle == NULL || handle->spi_write == NULL) {
        return ADS1220_NULL_PTR;
    }

    cmd[0] = ADS1220_WREG | (ADS1220_REG_CONFIG0 << 2);
    cmd[1] = 3;  /* 写入 4 个寄存器（0-3） */
    cmd[2] = handle->config.config0;
    cmd[3] = handle->config.config1;
    cmd[4] = handle->config.config2;
    cmd[5] = handle->config.config3;

    if (handle->spi_write(cmd, 6) != 6) {
        return ADS1220_ERROR;
    }

    return ADS1220_OK;
}

/**
 * @brief 设置多路选择通道
 * @param handle 指向 ADS1220_Handle 结构的指针
 * @param mux 多路选择值
 * @return 成功返回 ADS1220_OK，失败返回 ADS1220_ERROR
 */
int32_t ADS1220_SetMux(ADS1220_Handle* handle, uint8_t mux)
{
    if (handle == NULL) {
        return ADS1220_NULL_PTR;
    }

    handle->config.config0 = (handle->config.config0 & ~ADS1220_MUX_MASK) | mux;
    return ADS1220_WriteReg(handle, ADS1220_REG_CONFIG0, handle->config.config0);
}

/**
 * @brief 设置 PGA 增益
 * @param handle 指向 ADS1220_Handle 结构的指针
 * @param gain 增益值
 * @return 成功返回 ADS1220_OK，失败返回 ADS1220_ERROR
 */
int32_t ADS1220_SetGain(ADS1220_Handle* handle, uint8_t gain)
{
    if (handle == NULL) {
        return ADS1220_NULL_PTR;
    }

    handle->config.config0 = (handle->config.config0 & ~ADS1220_GAIN_MASK) | gain;
    return ADS1220_WriteReg(handle, ADS1220_REG_CONFIG0, handle->config.config0);
}

/**
 * @brief 设置数据速率
 * @param handle 指向 ADS1220_Handle 结构的指针
 * @param dr 数据速率值
 * @return 成功返回 ADS1220_OK，失败返回 ADS1220_ERROR
 */
int32_t ADS1220_SetDataRate(ADS1220_Handle* handle, uint8_t dr)
{
    if (handle == NULL) {
        return ADS1220_NULL_PTR;
    }

    handle->config.config1 = (handle->config.config1 & ~ADS1220_DR_MASK) | dr;
    return ADS1220_WriteReg(handle, ADS1220_REG_CONFIG1, handle->config.config1);
}

/**
 * @brief 设置数字滤波器
 * @param handle 指向 ADS1220_Handle 结构的指针
 * @param filter 滤波器值
 * @return 成功返回 ADS1220_OK，失败返回 ADS1220_ERROR
 */
int32_t ADS1220_SetFilter(ADS1220_Handle* handle, uint8_t filter)
{
    if (handle == NULL) {
        return ADS1220_NULL_PTR;
    }

    handle->config.config1 = (handle->config.config1 & ~ADS1220_FILTER_MASK) | filter;
    return ADS1220_WriteReg(handle, ADS1220_REG_CONFIG1, handle->config.config1);
}

/**
 * @brief 启动转换
 * @param handle 指向 ADS1220_Handle 结构的指针
 * @return 成功返回 ADS1220_OK，失败返回 ADS1220_ERROR
 */
int32_t ADS1220_Start(ADS1220_Handle* handle)
{
    uint8_t cmd = ADS1220_START;

    if (handle == NULL || handle->spi_write == NULL) {
        return ADS1220_NULL_PTR;
    }

    if (handle->spi_write(&cmd, 1) != 1) {
        return ADS1220_ERROR;
    }

    return ADS1220_OK;
}

/**
 * @brief 停止转换
 * @param handle 指向 ADS1220_Handle 结构的指针
 * @return 成功返回 ADS1220_OK，失败返回 ADS1220_ERROR
 */
int32_t ADS1220_Stop(ADS1220_Handle* handle)
{
    uint8_t cmd = ADS1220_STOP;

    if (handle == NULL || handle->spi_write == NULL) {
        return ADS1220_NULL_PTR;
    }

    if (handle->spi_write(&cmd, 1) != 1) {
        return ADS1220_ERROR;
    }

    return ADS1220_OK;
}

/**
 * @brief 读取 ADC 数据（单次模式）
 * @param handle 指向 ADS1220_Handle 结构的指针
 * @param data 指向 ADS1220_Data 结构的指针
 * @return 成功返回 ADS1220_OK，失败返回 ADS1220_ERROR
 */
int32_t ADS1220_ReadData(ADS1220_Handle* handle, ADS1220_Data* data)
{
    uint8_t cmd[4];
    uint8_t gain_index;

    if (handle == NULL || handle->spi_read == NULL || handle->spi_write == NULL || data == NULL) {
        return ADS1220_NULL_PTR;
    }

    cmd[0] = ADS1220_RDATA;

    if (handle->spi_write(cmd, 1) != 1) {
        return ADS1220_ERROR;
    }

    if (handle->spi_read(cmd, 3) != 3) {
        return ADS1220_ERROR;
    }

    /* 将 3 个字节转换为 24 位有符号整数 */
    data->raw_data = ((int32_t)cmd[0] << 16) | ((uint32_t)cmd[1] << 8) | cmd[2];

    /* 符号扩展：从 24 位到 32 位 */
    if (data->raw_data & 0x800000) {
        data->raw_data |= 0xFF000000;
    }

    /* 获取增益值 */
    gain_index = (handle->config.config0 & ADS1220_GAIN_MASK) >> 1;
    if (gain_index >= 8) {
        gain_index = 0;
    }

    /* 将原始数据转换为电压值 */
    data->voltage = ADS1220_RawToVoltage(data->raw_data, handle->vref, gain_index);

    return ADS1220_OK;
}

/**
 * @brief 读取 ADC 原始数据（连续模式）
 * @param handle 指向 ADS1220_Handle 结构的指针
 * @param raw_data 存储原始 ADC 数据的指针
 * @return 成功返回 ADS1220_OK，失败返回 ADS1220_ERROR
 */
int32_t ADS1220_ReadDataContinuous(ADS1220_Handle* handle, int32_t* raw_data)
{
    uint8_t cmd[3];

    if (handle == NULL || handle->spi_read == NULL || raw_data == NULL) {
        return ADS1220_NULL_PTR;
    }

    if (handle->spi_read(cmd, 3) != 3) {
        return ADS1220_ERROR;
    }

    /* 将 3 个字节转换为 24 位有符号整数 */
    *raw_data = ((int32_t)cmd[0] << 16) | ((uint32_t)cmd[1] << 8) | cmd[2];

    /* 符号扩展：从 24 位到 32 位 */
    if (*raw_data & 0x800000) {
        *raw_data |= 0xFF000000;
    }

    return ADS1220_OK;
}

/**
 * @brief 将原始 ADC 数据转换为电压值
 * @param raw_data 原始 ADC 数据
 * @param vref 参考电压
 * @param gain 增益索引（0-7）
 * @return 电压值（单位：伏）
 */
float ADS1220_RawToVoltage(int32_t raw_data, float vref, uint8_t gain)
{
    float gain_val = gain_values[gain & 0x07];
    /* 电压 = 原始数据 * 参考电压 / (增益 * 2^23) */
    float voltage = (float)raw_data * vref / (gain_val * 8388608.0f);  /* 2^23 = 8388608 */
    return voltage;
}
