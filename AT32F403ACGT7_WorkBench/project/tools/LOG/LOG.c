#include <stdint.h>
#include <string.h>

#include "LOG.h"

// 帧尾定义
const uint8_t FLOAT_FRAME_TAIL[4] = {0x00, 0x00, 0x80, 0x7F};

/**
 * @brief  将 float 数组打包为 uint8_t 字节数组，末尾追加帧尾（不使用 memcpy）
 * @param  input       float 数据指针
 * @param  float_count float 数组个数
 * @param  output      输出缓冲区（需要至少 float_count*4 + 4 字节空间）
 * @return 输出数据总长度（单位：字节）
 */
uint16_t PackFloatArrayToBytes(const float *input, uint8_t float_count, uint8_t *output)
{
    uint8_t *bytePtr = (uint8_t *)input; // 将 float 数据转换为字节数组

    // 复制 float 数据到 yx_buffer
    for (uint8_t i = 0; i < float_count * 4; i++) {
        output[i] = bytePtr[i];
    }

    // 追加帧尾 {0x00, 0x00, 0x80, 0x7F}
    output[float_count * 4] = 0x00;
    output[float_count * 4 + 1] = 0x00;
    output[float_count * 4 + 2] = 0x80;
    output[float_count * 4 + 3] = 0x7F;

    return float_count * 4 + 4;
}







