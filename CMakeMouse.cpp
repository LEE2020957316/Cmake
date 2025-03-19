// CMakeMouse.cpp: 定义应用程序的入口点。
//
#include "CMakeMouse.h"
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>

#include <iostream>
#include <thread>
using namespace std;

//注意因为使用了线程，需要执行 g++ main.cpp -pthread && ./a.out 带参数编译！！！之后运行

// 定义线程数量
static const int nt = 1;
thread t;
bool isStop = false;

// 线程函数实体
int ReadMouse(Mouse* mouse) {
    int fd, bytes;
    unsigned char data[3];
    const char* pDevice = "/dev/input/mice";
    // Open Mouse
    fd = open(pDevice, O_RDWR);
    if (fd == -1)
    {
        printf("ERROR Opening %s\n", pDevice);
        return -1;
    }
    int left, middle, right;
    signed char x, y;
    while (1)
    {
        if(isStop){
            cout << "Mouse Listening thread exit." << endl;
            return 0;
        }
        // Read Mouse     
        bytes = read(fd, data, sizeof(data));
        if (bytes > 0)
        {
            left = data[0] & 0x1;
            right = data[0] & 0x2;
            middle = data[0] & 0x4;
            x = data[1];
            y = data[2];
            mouse->x = x;
            mouse->y = y;
            mouse->left = left > 0 ? true : false;
            mouse->middle = middle > 0 ? true : false;
            mouse->right = right > 0 ? true : false;
            mouse->mc->hasData(x,y,left,middle,right);
        }
    }
}


// a demo derived from Mousecallback which prints out the coordinates.
void MyMouseCallback::hasData(int x, int y, bool left, bool middle, bool right) {
	if (x == 0 && y == 0 && !left && !middle && !right)return;//鼠标不动就不打印
	cout << "x=" << x << "  y=" << y << endl;
	cout << "left=" << left << "  middle="<< middle << "  right=" << right << endl;
}
void Mouse::registerCallback(Mousecallback* mc){
	this->mc = mc;
}
void Mouse::start(){
	t = thread(ReadMouse, this);//启动线程
}
void Mouse::stop(){
    isStop = true;
	//t.detach();
    //cout << "Mouse Listening thread exit." << endl;
}

int main()
{
	Mousecallback* callback = new MyMouseCallback;
	Mouse mouse;
	mouse.registerCallback(callback);
	mouse.start();


	t.detach();//不等待线程t退出，立即返回。


	cout << "Mouse Listening started. 输入input 'q' to stop Listening thread." << endl;
    char s1,s2;
    while (cin >> s1) {
        if (s1 == 'q' || s1 == 'Q') {
            mouse.stop();
            cout << "input any char to exit Main thread..." << endl;
            cin >> s2;
            if (s2 != '\0')break;
        }else{
            //cout<<s2<<endl;
        }
    }


    cout << "Main thread exit." << endl;
	return 0;
}
