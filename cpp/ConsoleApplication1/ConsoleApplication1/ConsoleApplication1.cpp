// ConsoleApplication1.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>

using namespace std;
class point
{
public:
    int x;
    int y;
};
int bottom = 50;
int left1 = 50;
int left2 = 50;
int right1 = 50;
int right2 = 50;
double role_left = 0;
double role_right = 0;

#define pi 3.1415926

void GetPoint2(point& point2)
{
    point2.x = bottom + right1 * cos(pi - role_right);
    point2.y = right1 * sin(pi - role_right);
}
void GetPoint3(point& point3)
{
    point3.x = -left1 * cos(pi - role_left);
    point3.y = sin(pi - role_left);
}
int main()
{


    point point1;
    point1.x = 0;
    point1.y = bottom;

    point point2;
    point point3;
    point point4;

    for (int i = 0; i < 180; i++)
    {
        for (int j = 0; j < 180; j++)
        {
            role_left = i*pi / 180;
            role_right = j*pi / 180;
            GetPoint2(point2);
            GetPoint3(point3);
            if (point2.x > point3.x)
            {
                if (point2.y <= point3.y)
                {
                    double l = sqrt((point2.x - point3.x) * (point2.x - point3.x) + (point2.y - point3.y) * (point2.y - point3.y));
                    double a = asin((point3.y - point2.y) / l);
                    double b = acos((l * l + right2 * right2 - left2 * left2) / (2 * l * right2));
                    point4.x = (point2.x - right2 * cos(a + b));
                    point4.y = (point2.y + right2 * sin(a + b));
                }
                else
                {
                    double l = sqrt((point2.x - point3.x) * (point2.x - point3.x) + (point2.y - point3.y) * (point2.y - point3.y));
                    double a = asin((point2.y - point3.y) / l);
                    double b = acos((l * l + left2 * left2 - right2 * right2) / (2 * l * left2));
                    point4.x = (point3.x + left2 * cos(a + b));
                    point4.y = (point3.y + left2 * sin(a + b));
                }
                cout << point4.x << "    " << point4.y << endl;
            }
            


        }
    }






    std::cout << "Hello World!\n";
}

// 运行程序: Ctrl + F5 或调试 >“开始执行(不调试)”菜单
// 调试程序: F5 或调试 >“开始调试”菜单

// 入门使用技巧: 
//   1. 使用解决方案资源管理器窗口添加/管理文件
//   2. 使用团队资源管理器窗口连接到源代码管理
//   3. 使用输出窗口查看生成输出和其他消息
//   4. 使用错误列表窗口查看错误
//   5. 转到“项目”>“添加新项”以创建新的代码文件，或转到“项目”>“添加现有项”以将现有代码文件添加到项目
//   6. 将来，若要再次打开此项目，请转到“文件”>“打开”>“项目”并选择 .sln 文件
