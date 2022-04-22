
// 版权声明区域

// ----------------------------------------------------------------------------------------------------

// ----------------------------------------------------------------------------------------------------
// include区域

#include <stdio.h>	 // 标准输入输出
#include <stdlib.h>	 // 标准库
#include <windows.h> //
#include <pthread.h> // 多线程库
#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>
#define STB_IMAGE_RESIZE_IMPLEMENTATION
#include <stb/stb_image_resize.h>
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb/stb_image_write.h>

#define bool char
#define true 1
#define false 0

HINSTANCE hInst;

int screen_w;
int screen_h;

int window_list_w;
int window_list_h;
HWND *window_list;
bool *window_is_show;
int window_list_len;

unsigned char *data_dst;

void *func1(void *arg)
{
	// 加载图片并显示
	int frame_idx = 1; // 指定编号从1开始
	char filename[32];
	int img_src_w, img_src_h;
	unsigned char *data_src;
	while (1)
	{
		// 加载图片
		sprintf(filename, "./img/%d.png", frame_idx); // 先找png
		data_src = stbi_load(filename, &img_src_w, &img_src_h, NULL, 3);
		if (data_src == NULL)
		{
			sprintf(filename, "./img/%d.jpg", frame_idx); // 再找jpg
			data_src = stbi_load(filename, &img_src_w, &img_src_h, NULL, 3);
			if (data_src == NULL)
			{
				sprintf(filename, "./img/%d.bmp", frame_idx); // 再找bmp
				data_src = stbi_load(filename, &img_src_w, &img_src_h, NULL, 3);
				if (data_src == NULL)
					break; // 都找不到，返回
			}
		}
		// 缩放图片
		stbir_resize_uint8(data_src, img_src_w, img_src_h, 0, data_dst, window_list_w, window_list_h, 0, 3);
		// 释放图片
		stbi_image_free(data_src);
		// 显示
		for (int i = 0; i < window_list_len; i++)
		{
			if ((int)data_dst[i * 3] > 128 || (int)data_dst[i * 3 + 1] > 128 || (int)data_dst[i * 3 + 2] > 128)
			{
				if (!window_is_show)
				{
					ShowWindow(window_list[i], SW_SHOW);
					window_is_show = true;
				}
			}
			else
			{
				if (window_is_show)
				{
					ShowWindow(window_list[i], SW_HIDE);
					window_is_show = false;
				}
			}
		}
		frame_idx++;
	}
	// 函数结束，同时把main函数退了
	exit(0);
}

/**
 * @brief 主函数
 *
 */
int main(int argc, char **argv)
{
	// 分离控制台
	FreeConsole();
	// 获取实例句柄
	hInst = GetModuleHandle(NULL);
	// 获取屏幕大小
	screen_w = GetSystemMetrics(SM_CXSCREEN);
	screen_h = GetSystemMetrics(SM_CYSCREEN);
	screen_w = 1280;
	screen_h = 960;

	// 设置窗口数量
	window_list_w = 20; // 横向数量
	window_list_h = 15; // 纵向数量

	// 注册窗口类
	WNDCLASS wndclass;
	wndclass.style = CS_HREDRAW | CS_VREDRAW;					  // 样式
	wndclass.lpszClassName = "myclass";							  // 类名
	wndclass.lpszMenuName = NULL;								  // 菜单:无
	wndclass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH); // 背景颜色
	wndclass.lpfnWndProc = DefWindowProc;						  // 处理函数
	wndclass.cbWndExtra = 0;									  // 实例扩展:无
	wndclass.cbClsExtra = 0;									  // 类扩展:无
	wndclass.hInstance = hInst;									  // 实例句柄
	wndclass.hIcon = LoadIcon(NULL, IDI_APPLICATION);			  // 最小化图标:使用缺省图标
	wndclass.hCursor = LoadCursor(NULL, IDC_ARROW);				  // 光标
	if (RegisterClass(&wndclass) == NULL)
		return 0;

	// 分配空间储存窗口句柄
	window_list_len = window_list_w * window_list_h;
	while ((window_list = malloc(window_list_len * sizeof(HWND))) == NULL)
		;
	// 分配空间储存窗口显示、隐藏状态
	while ((window_is_show = malloc(window_list_len * sizeof(bool))) == NULL)
		;
	// 分配空间储存图像
	while ((data_dst = malloc(window_list_len * 3)) == NULL)
		;

	// 创建窗口
	for (int y = 0; y < window_list_h; y++)
		for (int x = 0; x < window_list_w; x++)
		{
			int idx = y * window_list_w + x;
			int window_x = screen_w * x / window_list_w;
			int window_y = screen_h * y / window_list_h;
			int window_w = 50;
			int window_h = 50;
			window_list[idx] = CreateWindowEx(
				WS_EX_TOPMOST, // 加上 WS_EX_TOOLWINDOW 如果不想在任务栏显示
				"myclass",	   //类名
				"",			   //标题
				WS_POPUP,	   //风格 WS_POPUP
				window_x,	   //位置x
				window_y,	   //位置y
				window_w,	   //宽度
				window_h,	   //高度
				NULL,		   //父窗口
				NULL,		   //子菜单
				hInst,		   //该窗口应用程序的实例句柄
				NULL);
			// 创建失败直接退出，不管了
			if (window_list[idx] == NULL)
				return 0;
			window_is_show[idx] = true;
			// 显示窗口
			ShowWindow(window_list[idx], SW_SHOW);
		}

	// 创建窗口显示、隐藏控制线程
	pthread_t t;
	pthread_create(&t, NULL, func1, NULL);
	pthread_detach(t);

	// 开始处理窗口消息(直接调用默认处理函数)
	MSG msg;
	while (GetMessage(&msg, NULL, 0, 0)) //从消息队列中获取消息
	{
		TranslateMessage(&msg); //将虚拟键消息转换为字符消息
		DispatchMessage(&msg);	//分发到回调函数(过程函数)
	}

	// 程序结束，退出
	return 0;
}
