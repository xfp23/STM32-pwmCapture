

# PWM输入捕获

本示例演示如何使用定时器实现PWM输入捕获。**Timer3** 的 **Channel1** 用于输出PWM波形（PA6），而 **Timer1** 的 **Channel1** 和 **Channel2** 用于捕获该PWM波形（PA8）。

## API使用

### 1. 初始化捕获句柄

首先，声明一个捕获句柄并配置捕获参数。

```c
typedef enum {
    PWM_CAPTURE_OK = 0x00,               // 操作成功
    PWM_CAPTURE_ERROR = 0xFF,            // 操作失败
    PWM_CAPTURE_INITIALIZED = 0x01,      // 已初始化
    PWM_CAPTURE_CHANNEL_MISMATCH = 0x02, // 通道不匹配
} PwmCaptureState_t;  // 操作状态
```

定义捕获句柄和配置结构体：

```c
pwm_Capture_Handle_t pwmCapture_Handle = NULL;
pwm_Capture_conf_t pwmCapture_Config;
```

接着，初始化 `pwm_Capture_conf_t` 配置结构体：

```c
pwm_Capture_conf_t pwmCapture_Config = {
    .htim = &htim1,                // 指定定时器
    .RiseChannel = TIM_CHANNEL_1,   // 上升沿通道
    .FallChannel = TIM_CHANNEL_2,   // 下降沿通道
}; 
pwmCapture_Init(&pwmCapture_Handle, &pwmCapture_Config);
```

调用 `pwmCapture_Init` 来初始化PWM捕获模块：

```c
PwmCaptureState_t state = pwmCapture_Init(&pwmCapture_Handle, &pwmCapture_Config);
```

### 2. 启动PWM捕获

初始化时会自动启动捕获，如果需要重新启动捕获，请使用 `pwmCapture_Start` 函数：

```c
state = pwmCapture_Start(&pwmCapture_Handle);
```

### 3. 捕获回调

定时器的中断回调函数会在每次捕获到PWM信号时被触发，处理捕获数据：

```c
void pwmCapture_Callback(pwm_Capture_Handle_t *pwm_Cap_handle, TIM_HandleTypeDef *htim) {
    // 捕获事件处理逻辑
    uint32_t pulseWidth = pwmCapture_getPulseWidth(*pwm_Cap_handle);
    uint32_t frequency = pwmCapture_getFreq(*pwm_Cap_handle);
    float dutyCycle = pwmCapture_getDuty(*pwm_Cap_handle);

    // 进一步处理捕获的数据
}
```

### 4. 获取捕获数据

你可以通过以下函数获取捕获到的PWM信号的不同参数：

- **脉冲宽度**：

    ```c
    uint32_t pulseWidth = pwmCapture_getPulseWidth(pwmCapture_Handle);
    ```

- **频率**：

    ```c
    uint32_t frequency = pwmCapture_getFreq(pwmCapture_Handle);
    ```

- **占空比**：

    ```c
    float dutyCycle = pwmCapture_getDuty(pwmCapture_Handle);
    ```

- **周期**：

    ```c
    uint32_t period = pwmCapture_getPeriod(pwmCapture_Handle);
    ```

- **捕获是否完成**：

    ```c
    bool captureComplete = pwmCapture_getComplete(&pwmCapture_Handle);
    ```

### 5. 停止捕获

当你需要停止捕获时，可以调用 `pwmCapture_Stop` 函数：

```c
state = pwmCapture_Stop(&pwmCapture_Handle);
```

### 6. 重置捕获

若需要重置捕获状态，可以使用 `pwmCapture_Reset`：

```c
state = pwmCapture_Reset(&pwmCapture_Handle);
```

### 7. 删除捕获句柄

捕获结束后，可以删除捕获句柄并释放资源：

```c
state = pwmCapture_Delete(&pwmCapture_Handle);
```

## API函数概述

### `pwmCapture_Init(pwm_Capture_Handle_t *handle, pwm_Capture_conf_t *conf)`
- **功能**：初始化PWM捕获模块。
- **参数**：
  - `handle`：捕获句柄的指针。
  - `conf`：包含定时器句柄和上升沿、下降沿通道配置的结构体。
- **返回值**：`PwmCaptureState_t` 状态。

### `pwmCapture_Callback(pwm_Capture_Handle_t *pwm_Cap_handle, TIM_HandleTypeDef *htim)`
- **功能**：定时器回调函数，在捕获事件发生时被调用。
- **参数**：
  - `pwm_Cap_handle`：捕获句柄。
  - `htim`：定时器句柄。
  
### `pwmCapture_Start(pwm_Capture_Handle_t *handle)`
- **功能**：启动PWM捕获。
- **参数**：
  - `handle`：捕获句柄。
- **返回值**：`PwmCaptureState_t` 状态。

### `pwmCapture_Stop(pwm_Capture_Handle_t *handle)`
- **功能**：停止PWM捕获。
- **参数**：
  - `handle`：捕获句柄。
- **返回值**：`PwmCaptureState_t` 状态。

### `pwmCapture_Reset(pwm_Capture_Handle_t *handle)`
- **功能**：重置捕获模块。
- **参数**：
  - `handle`：捕获句柄。
- **返回值**：`PwmCaptureState_t` 状态。

### `pwmCapture_Delete(pwm_Capture_Handle_t *handle)`
- **功能**：删除捕获句柄并释放资源。
- **参数**：
  - `handle`：捕获句柄。
- **返回值**：`PwmCaptureState_t` 状态。

### `pwmCapture_getPulseWidth(pwm_Capture_Handle_t handle)`
- **功能**：获取PWM信号的脉冲宽度。
- **参数**：
  - `handle`：捕获句柄。
- **返回值**：PWM信号的脉冲宽度（单位：微秒）。

### `pwmCapture_getFreq(pwm_Capture_Handle_t handle)`
- **功能**：获取PWM信号的频率。
- **参数**：
  - `handle`：捕获句柄。
- **返回值**：PWM信号的频率（单位：Hz）。

### `pwmCapture_getDuty(pwm_Capture_Handle_t handle)`
- **功能**：获取PWM信号的占空比。
- **参数**：
  - `handle`：捕获句柄。
- **返回值**：PWM信号的占空比（单位：%）。

### `pwmCapture_getPeriod(pwm_Capture_Handle_t handle)`
- **功能**：获取PWM信号的周期。
- **参数**：
  - `handle`：捕获句柄。
- **返回值**：PWM信号的周期（单位：微秒）。

### `pwmCapture_getComplete(pwm_Capture_Handle_t *handle)`
- **功能**：检查PWM捕获是否完成。
- **参数**：
  - `handle`：捕获句柄。
- **返回值**：`true` 表示捕获完成，`false` 表示尚未完成。
