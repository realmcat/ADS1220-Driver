# ADS1220 ADC 驱动程序（C 语言）

用 C 语言实现的德州仪器（TI）ADS1220 24 位模数转换器（ADC）完整驱动程序。本驱动提供了清晰、模块化的接口来控制 ADS1220 的 SPI 通信。

## 特性

### ✨ 完整的 ADS1220 寄存器控制
- 多路选择（MUX）通道选择
- 可编程增益放大器（PGA）
- 数据速率配置
- 数字滤波器选择
- 参考电��管理

### 📊 多种工作模式
- 单次转换模式
- 连续转换模式
- 温度传感器读取

### 🔌 平台无关设计
- 基于回调函数的 SPI 接口
- 易于集成不同的微控制器平台
- 驱动代码中没有硬件特定依赖

### 🔄 自动数据转换
- 自动 24 位至 32 位符号扩展
- 原始 ADC 值到电压的自动转换
- 考虑增益的电压计算

### ⚠️ 完整的错误处理
- 返回错误代码机制
- 参数合法性检查

## 文件结构

```
├── ads1220.h              # 驱动头文件（API 定义）
├── ads1220.c              # 驱动实现
├── main.c                 # 使用示例和演示
├── platform.h             # 平台抽象层接口
├── platform_stm32.c       # STM32 HAL 实现示例
└── README.md              # 本文档
```

## 快速开始

### 1. 基本初始化

```c
#include "ads1220.h"

// 创建驱动句柄
ADS1220_Handle ads1220;

// 设置 SPI 通信回调函数
ads1220.spi_read = your_spi_read_function;
ads1220.spi_write = your_spi_write_function;
ads1220.delay_ms = your_delay_function;

// 使用 2.048V 参考电压初始化
ADS1220_Init(&ads1220, 2.048f);
```

### 2. 配置测量

```c
// 选择测量通道（AIN0 vs AIN1）
ADS1220_SetMux(&ads1220, ADS1220_MUX_AIN0_AIN1);

// 设置增益
ADS1220_SetGain(&ads1220, ADS1220_GAIN_1);

// 设置数据速率（330 SPS）
ADS1220_SetDataRate(&ads1220, ADS1220_DR_330);

// 设置滤波器类型
ADS1220_SetFilter(&ads1220, ADS1220_FILTER_SINC3);
```

### 3. 读取 ADC 数据

```c
ADS1220_Data data;

// 启动转换
ADS1220_Start(&ads1220);

// 等待转换完成（根据数据速率调整）
delay_ms(10);

// 读取转换后的数据
ADS1220_ReadData(&ads1220, &data);

printf("原始值: %d\n", data.raw_data);
printf("电压: %.6f V\n", data.voltage);
```

## API 参考

### 初始化函数

```c
int32_t ADS1220_Init(ADS1220_Handle* handle, float vref);
int32_t ADS1220_Reset(ADS1220_Handle* handle);
```

### 配置函数

```c
int32_t ADS1220_SetMux(ADS1220_Handle* handle, uint8_t mux);
int32_t ADS1220_SetGain(ADS1220_Handle* handle, uint8_t gain);
int32_t ADS1220_SetDataRate(ADS1220_Handle* handle, uint8_t dr);
int32_t ADS1220_SetFilter(ADS1220_Handle* handle, uint8_t filter);
int32_t ADS1220_WriteReg(ADS1220_Handle* handle, uint8_t reg, uint8_t value);
int32_t ADS1220_ReadReg(ADS1220_Handle* handle, uint8_t reg, uint8_t* value);
```

### 数据采集函数

```c
int32_t ADS1220_Start(ADS1220_Handle* handle);
int32_t ADS1220_Stop(ADS1220_Handle* handle);
int32_t ADS1220_ReadData(ADS1220_Handle* handle, ADS1220_Data* data);
int32_t ADS1220_ReadDataContinuous(ADS1220_Handle* handle, int32_t* raw_data);
```

### 工具函数

```c
float ADS1220_RawToVoltage(int32_t raw_data, float vref, uint8_t gain);
```

## 多路选择通道

```c
ADS1220_MUX_AIN0_AIN1   // 差分 AIN0 vs AIN1
ADS1220_MUX_AIN0_AIN2   // 差分 AIN0 vs AIN2
ADS1220_MUX_AIN0_AIN3   // 差分 AIN0 vs AIN3
ADS1220_MUX_AIN1_AIN2   // 差分 AIN1 vs AIN2
ADS1220_MUX_AIN1_AIN3   // 差分 AIN1 vs AIN3
ADS1220_MUX_AIN2_AIN3   // 差分 AIN2 vs AIN3
ADS1220_MUX_AIN1_GND    // 单端 AIN1 vs GND
ADS1220_MUX_AIN2_GND    // 单端 AIN2 vs GND
ADS1220_MUX_AIN3_GND    // 单端 AIN3 vs GND
```

## 可编程增益放大器（PGA）

```c
ADS1220_GAIN_1          // 增益 = 1
ADS1220_GAIN_2          // 增益 = 2
ADS1220_GAIN_4          // 增益 = 4
ADS1220_GAIN_8          // 增益 = 8
ADS1220_GAIN_16         // 增益 = 16
ADS1220_GAIN_32         // 增益 = 32
ADS1220_GAIN_64         // 增益 = 64
ADS1220_GAIN_128        // 增益 = 128
```

## 数据速率

```c
ADS1220_DR_20           // 20 SPS（每秒采样数）
ADS1220_DR_45           // 45 SPS
ADS1220_DR_90           // 90 SPS
ADS1220_DR_175          // 175 SPS
ADS1220_DR_330          // 330 SPS（默认）
ADS1220_DR_600          // 600 SPS
ADS1220_DR_1000         // 1000 SPS
```

## 数字滤波器

```c
ADS1220_FILTER_SINC1    // SINC1 滤波器
ADS1220_FILTER_SINC2    // SINC2 滤波器
ADS1220_FILTER_SINC3    // SINC3 滤波器（更好的噪声抑制）
ADS1220_FILTER_SINC4    // SINC4 滤波器
```

## 平台集成

驱动程序使用回调函数进行 SPI 通信。要与您的平台集成：

1. 实现以下回调函数：
   - `spi_read()` - 从 SPI 总线读取
   - `spi_write()` - 写入 SPI 总线
   - `delay_ms()` - 延时函数

2. 在初始化前设置回调：
```c
ads1220_handle.spi_read = your_spi_read;
ads1220_handle.spi_write = your_spi_write;
ads1220_handle.delay_ms = your_delay_ms;
```

3. 参考 `platform_stm32.c` 查看 STM32 HAL 实现示例

## 使用示例

### 示例 1：单通道测量

```c
// 配置 AIN0 vs AIN1
ADS1220_SetMux(&ads1220, ADS1220_MUX_AIN0_AIN1);
ADS1220_SetGain(&ads1220, ADS1220_GAIN_1);
ADS1220_SetDataRate(&ads1220, ADS1220_DR_330);

// 读取数据
ADS1220_Start(&ads1220);
delay_ms(10);

ADS1220_Data data;
ADS1220_ReadData(&ads1220, &data);
printf("电压: %.6f V\n", data.voltage);
```

### 示例 2：多通道扫描

```c
uint8_t channels[] = {
    ADS1220_MUX_AIN0_GND,
    ADS1220_MUX_AIN1_GND,
    ADS1220_MUX_AIN2_GND
};

for (int i = 0; i < 3; i++) {
    ADS1220_SetMux(&ads1220, channels[i]);
    delay_ms(5);  // 等待稳定
    
    ADS1220_Start(&ads1220);
    delay_ms(10);
    
    ADS1220_Data data;
    ADS1220_ReadData(&ads1220, &data);
    printf("通道 %d: %.6f V\n", i, data.voltage);
}
```

### 示例 3：连续数据采集

```c
// 启动连续转换
ADS1220_Start(&ads1220);

int32_t raw_data;
for (int i = 0; i < 100; i++) {
    delay_ms(5);
    ADS1220_ReadDataContinuous(&ads1220, &raw_data);
    float voltage = ADS1220_RawToVoltage(raw_data, 2.048f, 0);
    printf("样本 %d: %d 原始, %.6f V\n", i, raw_data, voltage);
}

// 停止连续转换
ADS1220_Stop(&ads1220);
```

### 示例 4：温度测量

```c
// 使能温度传感器
ads1220.config.config2 |= 0x10;
ADS1220_WriteReg(&ads1220, ADS1220_REG_CONFIG2, ads1220.config.config2);

// 选择温度输入
ADS1220_SetMux(&ads1220, ADS1220_MUX_AIN3_GND);

// 读取温度
ADS1220_Start(&ads1220);
delay_ms(10);

ADS1220_Data data;
ADS1220_ReadData(&ads1220, &data);

// 温度 = 原始值 / 128（简化）
float temp = (float)data.raw_data / 128.0f;
printf("温度: %.2f °C\n", temp);
```

## 错误处理

驱动程序函数返回错误代码：

```c
#define ADS1220_OK          0   // 成功
#define ADS1220_ERROR      -1   // 一般错误
#define ADS1220_NULL_PTR   -2   // 空指针错误
```

始终检查返回值：

```c
if (ADS1220_Init(&ads1220, 2.048f) != ADS1220_OK) {
    // 处理初始化错误
    printf("ADS1220 初始化失败\n");
}
```

## SPI 通信说明

- **SPI 模式**：MODE 1（CPOL=0，CPHA=1）
- **时钟频率**：4 MHz（典型值）
- **数据格式**：MSB 优先
- **CS 控制**：手动（软件控制）

ADS1220 SPI 命令格式：
- 命令：1 字节
- 寄存器读写：2-3 字节
- 数据读取：返回 3 字节

## 电压计算

驱动程序自动将原始 ADC 数据转换为电压：

```
电压（V）= 原始数据 * VREF / (增益 * 2^23)
```

其中：
- `原始数据`：24 位有符号整数
- `VREF`：参考电压（2.048V 或 4.096V）
- `增益`：PGA 增益（1-128）
- `2^23`：8,388,608（分辨率位数）

## 功耗

- **活跃模式**：典型 0.5 mA @ 2.7V
- **睡眠模式**：典型 1 µA
- **供电范围**：2.7V 至 5.5V

## 典型性能

- **分辨率**：24 位
- **精度**：±0.05%（典型）
- **线性度**：±0.05% FSR（典型）
- **噪声**：使用数字滤波器时低噪声

## 故障排除

**问题**：读取全 0
- 检查 SPI 通信和 CS 时序
- 验证参考电压稳定
- 检查 ADC 电源供应

**问题**：读数嘈杂
- 使用 SINC3 或 SINC4 滤波器
- 降低数据速率
- 检查信号布线和接地

**问题**：响应缓慢
- 提高数据速率
- 使用 SINC1 滤波器（更快，滤波少）
- 减少通道切换间稳定时间

## 参考资源

- TI ADS1220 数据手册：https://www.ti.com/product/ADS1220
- SPI 通信协议
- 24 位 ADC 设计考虑

## 许可证

本驱动程序按原样提供，可用于教育和商业用途。

## 支持

关于此驱动程序实现的问题或建议，请参考 TI ADS1220 数据手册和应用笔记。
