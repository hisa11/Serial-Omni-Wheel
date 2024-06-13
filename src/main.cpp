#include "mbed.h"
#include "firstpenguin.hpp"
constexpr uint32_t can_id = 35;
BufferedSerial pc(USBTX, USBRX, 250000); // パソコンとのシリアル通信
CANMessage msg;
CAN can(PA_11, PA_12, (int)1e6);
CAN can2(PB_12, PB_13, (int)1e6);
int16_t pwm0[4] = {0, 0, 0, 0}; // モタドラ1
Timer timer;
FirstPenguin penguin{can_id, can};
int speed = 8000;
int main()//////////////////////////////////////////////////////////正転w, 後転s, 停止x, 加速o, 減速p, 速度表示y
{
printf("prpgram start\n");
    while (1)
    {
        char buf;
        if(pc.readable())
        {
            pc.read(&buf, sizeof(buf));
            if (buf == 'W' || buf == 'w') //
            {
                penguin.pwm[0] = -speed;
                penguin.pwm[1] = speed;
            }
            else if (buf == 'S' || buf == 's') //
            {
                penguin.pwm[0] = speed;
                penguin.pwm[1] = -speed;
            }
            else if (buf == 'X' || buf == 'x') // 停止
            {
                penguin.pwm[0] = 0; // 停止の処理を追加
                penguin.pwm[1] = 0; // 停止の処理を追加
            }
            if (buf == 'o' || buf == 'O') //
            {
                speed += 1000;
            }
            else if (buf == 'P' || buf == 'p') //
            {
                speed -= 1000;
            }else if(buf == 'I' || buf == 'i') //
            {
                speed += 500;
            }
            if (buf == 'y' || buf == 'Y') //
            {
                printf("speed = %d\n", speed);
            }
        }
        penguin.send();
    }
}