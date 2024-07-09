#include "Constants.h"

namespace constants
{
const PCA9685::DeviceAddress device_address = 0x40;
const PCA9685::Pin output_enable_pin = 2;

const size_t loop_delay = 500;  // 20ms的周期

const PCA9685::Channel channel = 0;

// 调整脉冲宽度范围为0.5ms到2.5ms
const PCA9685::DurationMicroseconds servo_pulse_duration_min = 500;  // 0.5ms
const PCA9685::DurationMicroseconds servo_pulse_duration_max = 2500; // 2.5ms
const PCA9685::DurationMicroseconds servo_pulse_duration_increment = 100;
}
