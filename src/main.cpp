#include "mbed.h"
BufferedSerial pc(USBTX, USBRX, 250000); // Serial communication object
DigitalOut led3(D8);                     // LED object
DigitalOut led(D9);                      // LED object
DigitalOut led2(D10);                    // LED object

int value_x = 0;
int value_y = 0;
int value_z = 0;

CANMessage msg;                  // CAN message object
CAN can{PA_11, PA_12, (int)1e6}; // CAN communication object
int16_t pwm0[4] = {0, 0, 0, 0};  // モタドラ1

// '|'が受け取られるまでデータをバッファに追加する関数

void readUntilPipe(char *output_buf, int output_buf_size)
{
    char buf[20];
    int output_buf_index = 0;
    while (1)
    {
        if (pc.readable())
        {
            ssize_t num = pc.read(buf, sizeof(buf) - 1); // -1 to leave space for null terminator
            buf[num] = '\0';
            for (int i = 0; i < num; i++)
            {
                if (buf[i] == '|')
                {
                    output_buf[output_buf_index] = '\0';
                    return;
                }
                else if (buf[i] != '\n' && output_buf_index < output_buf_size - 1)
                { // 改行文字を無視します
                    output_buf[output_buf_index++] = buf[i];
                }
            }
        }
        if (output_buf_index >= output_buf_size - 1) // Prevent buffer overflow
        {
            output_buf[output_buf_index] = '\0';
            return;
        }
    }
}

int main()
{
    char output_buf[20]; // 出力用のバッファを作成します
    while (1)
    {
        readUntilPipe(output_buf, sizeof(output_buf)); // '|'が受け取られるまでデータを読み込みます
        if (strncmp(output_buf, "L3_x:", 5) == 0) // "R3_x:"という文字列で始まるかどうかを確認します
        {
            char *ptr = output_buf + 5; // "R3_x:"の後の文字列の先頭ポインタを取得
            // 数字部分を読み取る
            value_x = atoi(ptr);
            // 数字を使って何かをする（ここでは単にPCに出力）
            printf("Rx: %d\n", value_x);
        }
        else if (strncmp(output_buf, "L3_y:", 5) == 0) // "R3_x:"という文字列で始まるかどうかを確認します
        {
            char *ptr = output_buf + 5; // "R3_x:"の後の文字列の先頭ポインタを取得
            // 数字部分を読み取る
            value_y = atoi(ptr);
            // 数字を使って何かをする（ここでは単にPCに出力）
            printf("Ry: %d\n", value_y);
        }
        else if (strncmp(output_buf, "R3_x:", 5) == 0) // "R3_x:"という文字列で始まるかどうかを確認します
        {
            char *ptr = output_buf + 5; // "R3_x:"の後の文字列の先頭ポインタを取得
            // 数字部分を読み取る
            value_z = atoi(ptr);
            // 数字を使って何かをする（ここでは単にPCに出力）
            printf("Rz: %d\n", value_z);
        }

        pwm0[0] = -(value_y - value_x) + value_z;
        pwm0[1] = -(value_y + value_x) + value_z;
        pwm0[2] = -value_y + value_x + value_z;
        pwm0[3] = -(-value_y - value_x) + value_z;

        // pwm0[0] = -value_y + value_x　+ value_z;
        // pwm0[1] = -value_y - value_x　+ value_z;
        // pwm0[2] = -value_y + value_x　+ value_z;
        // pwm0[3] = value_y + value_x　+ value_z;

        CANMessage msg0(1, (const uint8_t *)pwm0, 8);
        can.write(msg0);
        // ループの最後でバッファをクリアします
        memset(output_buf, 0, sizeof(output_buf));
    }
}
