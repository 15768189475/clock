#include "main.h"

#define ROW 100
#define COL 30
uint64 draw_data[128];

void DrawThread(void const *argument)
{
    uint64 bit = 1;
    while (1)
    {
        osSignalWait(0, osWaitForever);

        for (uint16 i = 0; i < 128; i++)
        {
            for (uint16 j = 0; j < 64; j++) // draw_data
            {
                if (draw_data[i] & (bit << j))
                {
                    draw_data[i] &= ~(bit << j);
                }
                // if (draw_data[i][j])
                // {
                //     Write(true);
                //     draw_data[i][j] = false;
                //     Move(i,j);

                //     for (uint16 m = 0; m < ROW; m++)
                //     {
                //         for (uint16 n = 0; n < COL; n++)
                //         {
                //             if (draw_data[i + m][j + n])
                //             {
                //             }
                //         }
                //     }
                // }
            }
        }
    }
}

#define l1 50
#define l2 50
#define l3 50
#define l4 50
#define l5 50

void Move(uint16 x, uint16 y)
{
    //计算左右角度
    uint8 left_role, right_role;

    float ll = (x - COL / 2 + l3 / 2) * (x - COL / 2 + l3 / 2) + (y - ROW) * (y - ROW);
    float a = acos((l2 * l2 + ll - l1 * l1) / (2 * l2 * sqrt(ll)));
    float ll2 = (x - COL / 2) * (x - COL / 2) + (y - ROW) * (y - ROW);
    float b = acos((ll + l3 * l3 / 4 - ll2) / (2 * l3 / 2 * sqrt(ll)));

    left_role = a + b;

    ll = (x - COL / 2 - l3 / 2) * (x - COL / 2 - l3 / 2) + (y - ROW) * (y - ROW);
    a = acos((l4 * l4 + ll - l5 * l5) / (2 * l4 * sqrt(ll)));
    b = acos((ll + l3 * l3 / 4 - ll2) / (2 * l3 / 2 * sqrt(ll)));
    right_role = a + b;

    SetLeftServo(left_role);
    SetRightServo(right_role);

    //移动延时
    static uint16 last_x, last_y;
    uint16 distance = sqrt((x - last_x) * (x - last_x) + (y - last_y) * (y - last_y)); // distance 用于延时
    last_x = x;
    last_y = y;
    osDelay(distance);
}