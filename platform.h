/**
 * 平台抽象层（HAL）
 * 
 * 此头文件定义了平台特定实现的接口。
 * 应为目标平台提供实现（STM32、AVR 等）
 */

#ifndef PLATFORM_H
#define PLATFORM_H

#include <stdint.h>

/**
 * @brief 初始化平台（SPI、GPIO、时钟等）
 * @return 成功返回 0，失败返回非零值
 */
int32_t HAL_Init(void);

/**
 * @brief 反初始化平台
 * @return 成功返回 0，失败返回非零值
 */
int32_t HAL_Deinit(void);

/**
 * @brief 从 SPI 总线读取数据
 * @param buffer 指向缓冲区的指针，用于存储读取的数据
 * @param length 要读取的字节数
 * @return 读取的字节数，错误时返回负数
 */
int32_t HAL_SPI_Read(uint8_t* buffer, uint32_t length);

/**
 * @brief 写数据到 SPI 总线
 * @param buffer 指向包含要写入数据的缓冲区的指针
 * @param length 要写入的字节数
 * @return 写入的字节数，错误时返回负数
 */
int32_t HAL_SPI_Write(uint8_t* buffer, uint32_t length);

/**
 * @brief 延时指定毫秒数
 * @param ms 延时的毫秒数
 */
void HAL_Delay_Ms(uint32_t ms);

#endif /* PLATFORM_H */
