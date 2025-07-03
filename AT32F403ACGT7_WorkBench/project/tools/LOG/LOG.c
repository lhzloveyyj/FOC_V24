#include <stdint.h>
#include <string.h>

#include "LOG.h"

// ֡β����
const uint8_t FLOAT_FRAME_TAIL[4] = {0x00, 0x00, 0x80, 0x7F};

/**
 * @brief  �� float ������Ϊ uint8_t �ֽ����飬ĩβ׷��֡β����ʹ�� memcpy��
 * @param  input       float ����ָ��
 * @param  float_count float �������
 * @param  output      �������������Ҫ���� float_count*4 + 4 �ֽڿռ䣩
 * @return ��������ܳ��ȣ���λ���ֽڣ�
 */
uint16_t PackFloatArrayToBytes(const float *input, uint8_t float_count, uint8_t *output)
{
    uint8_t *bytePtr = (uint8_t *)input; // �� float ����ת��Ϊ�ֽ�����

    // ���� float ���ݵ� yx_buffer
    for (uint8_t i = 0; i < float_count * 4; i++) {
        output[i] = bytePtr[i];
    }

    // ׷��֡β {0x00, 0x00, 0x80, 0x7F}
    output[float_count * 4] = 0x00;
    output[float_count * 4 + 1] = 0x00;
    output[float_count * 4 + 2] = 0x80;
    output[float_count * 4 + 3] = 0x7F;

    return float_count * 4 + 4;
}







