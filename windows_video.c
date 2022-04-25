
/**
 * <one line to give the program's name and a brief idea of what it does.>
 * Copyright (C) 2022  xystd
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#include <stdio.h>						// 标准输入输出
#include <stdlib.h>						// 标准库
#include <unistd.h>						//
#include <pthread.h>					// 多线程库
#define STB_IMAGE_IMPLEMENTATION		//
#include <stb/stb_image.h>				//
#define STB_IMAGE_RESIZE_IMPLEMENTATION //
#include <stb/stb_image_resize.h>		//
#define STB_IMAGE_WRITE_IMPLEMENTATION	//
#include <stb/stb_image_write.h>		//
#include <windows.h>					//

#define bool char
#define true 1
#define false 0

HINSTANCE hInst;

int screen_w;
int screen_h;

int viewport_offset_x;
int viewport_offset_y;
int viewport_w;
int viewport_h;

int window_spacing;

int window_list_w;
int window_list_h;
HWND *window_list;
bool *window_is_show;
int window_list_len;

unsigned char *data_dst;

HDC dc;
HDC cdc;
HBITMAP bmp;

unsigned char *screenshot;
bool auto_screenshot;

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
				if (!window_is_show[i])
				{
					ShowWindow(window_list[i], SW_SHOW);
					window_is_show[i] = true;
				}
			}
			else
			{
				if (window_is_show[i])
				{
					ShowWindow(window_list[i], SW_HIDE);
					window_is_show[i] = false;
				}
			}
		}
		if (auto_screenshot)
		{
			// 稍微等待一会儿
			// Sleep(10);
			// 算了，不等了
			// 将显示器内容复制到内存
			BitBlt(cdc, 0, 0, screen_w, screen_h, dc, 0, 0, SRCCOPY);
			GetBitmapBits(bmp, screen_w * screen_h * 4, screenshot);
			// 交换红、蓝通道以便保存
			for (int i = 0; i < screen_w * screen_h * 4; i += 4)
			{
				unsigned char a = screenshot[i];
				screenshot[i] = screenshot[i + 2];
				screenshot[i + 2] = a;
			}
			// 保存图片
			sprintf(filename, "./img_out/%d.png", frame_idx);
			stbi_write_png(filename, screen_w, screen_h, 4, screenshot, 0);
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
	// 获取主显示器大小
	screen_w = GetSystemMetrics(SM_CXSCREEN);
	screen_h = GetSystemMetrics(SM_CYSCREEN);

	// 设置viewport位置，窗口会显示在viewport指定的区域内
	// 你可以使用
	// viewport_w = screen_w; viewport_h = screen_h;
	// 来使整个主显示器成为窗口的放置区域
	viewport_offset_x = 0;
	viewport_offset_y = 0;
	viewport_w = 1280;
	viewport_h = 960;
	// 设置窗口数量
	window_list_w = 40; // 横向数量
	window_list_h = 30; // 纵向数量
	// 设置窗口间距
	window_spacing = 5;
	// 自动截图，会在每帧刷新完时保存主显示器(这里注意!)为图片
	auto_screenshot = true;

	// gpl3协议的声明，请尊重开源协议，不要随意修改此处的内容
	if (MessageBox(NULL, "\
xystd windows_video V1.0  Copyright (C) 2022  xystd\n\
This program comes with ABSOLUTELY NO WARRANTY; for details click `yes\'.\n\
This is free software, and you are welcome to redistribute it\n\
under certain conditions; click `yes\' for details.\n\
",
				   "关于window_video", MB_YESNO) == IDYES)

	{
		MessageBox(NULL, "\
  4. Conveying Verbatim Copies.\n\
\n\
  You may convey verbatim copies of the Program\'s source code as you\n\
receive it, in any medium, provided that you conspicuously and\n\
appropriately publish on each copy an appropriate copyright notice;\n\
keep intact all notices stating that this License and any\n\
non-permissive terms added in accord with section 7 apply to the code;\n\
keep intact all notices of the absence of any warranty; and give all\n\
recipients a copy of this License along with the Program.\n\
\n\
  You may charge any price or no price for each copy that you convey,\n\
and you may offer support or warranty protection for a fee.\n\
\n\
  5. Conveying Modified Source Versions.\n\
\n\
  You may convey a work based on the Program, or the modifications to\n\
produce it from the Program, in the form of source code under the\n\
terms of section 4, provided that you also meet all of these conditions:\n\
\n\
    a) The work must carry prominent notices stating that you modified\n\
    it, and giving a relevant date.\n\
\n\
    b) The work must carry prominent notices stating that it is\n\
    released under this License and any conditions added under section\n\
    7.  This requirement modifies the requirement in section 4 to\n\
    \"keep intact all notices\".\n\
\n\
    c) You must license the entire work, as a whole, under this\n\
    License to anyone who comes into possession of a copy.  This\n\
    License will therefore apply, along with any applicable section 7\n\
    additional terms, to the whole of the work, and all its parts,\n\
    regardless of how they are packaged.  This License gives no\n\
    permission to license the work in any other way, but it does not\n\
    invalidate such permission if you have separately received it.\n\
\n\
    d) If the work has interactive user interfaces, each must display\n\
    Appropriate Legal Notices; however, if the Program has interactive\n\
    interfaces that do not display Appropriate Legal Notices, your\n\
    work need not make them do so.\n\
\n\
  A compilation of a covered work with other separate and independent\n\
works, which are not by their nature extensions of the covered work,\n\
and which are not combined with it such as to form a larger program,\n\
in or on a volume of a storage or distribution medium, is called an\n\
\"aggregate\" if the compilation and its resulting copyright are not\n\
used to limit the access or legal rights of the compilation\'s users\n\
beyond what the individual works permit.  Inclusion of a covered work\n\
in an aggregate does not cause this License to apply to the other\n\
parts of the aggregate.\n\
",
				   "关于window_video",
				   MB_OK);
		MessageBox(NULL, "\
  15. Disclaimer of Warranty.\n\
\n\
  THERE IS NO WARRANTY FOR THE PROGRAM, TO THE EXTENT PERMITTED BY\n\
APPLICABLE LAW.  EXCEPT WHEN OTHERWISE STATED IN WRITING THE COPYRIGHT\n\
HOLDERS AND/OR OTHER PARTIES PROVIDE THE PROGRAM \"AS IS\" WITHOUT WARRANTY\n\
OF ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING, BUT NOT LIMITED TO,\n\
THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR\n\
PURPOSE.  THE ENTIRE RISK AS TO THE QUALITY AND PERFORMANCE OF THE PROGRAM\n\
IS WITH YOU.  SHOULD THE PROGRAM PROVE DEFECTIVE, YOU ASSUME THE COST OF\n\
ALL NECESSARY SERVICING, REPAIR OR CORRECTION.\n\
\n\
  16. Limitation of Liability.\n\
\n\
  IN NO EVENT UNLESS REQUIRED BY APPLICABLE LAW OR AGREED TO IN WRITING\n\
WILL ANY COPYRIGHT HOLDER, OR ANY OTHER PARTY WHO MODIFIES AND/OR CONVEYS\n\
THE PROGRAM AS PERMITTED ABOVE, BE LIABLE TO YOU FOR DAMAGES, INCLUDING ANY\n\
GENERAL, SPECIAL, INCIDENTAL OR CONSEQUENTIAL DAMAGES ARISING OUT OF THE\n\
USE OR INABILITY TO USE THE PROGRAM (INCLUDING BUT NOT LIMITED TO LOSS OF\n\
DATA OR DATA BEING RENDERED INACCURATE OR LOSSES SUSTAINED BY YOU OR THIRD\n\
PARTIES OR A FAILURE OF THE PROGRAM TO OPERATE WITH ANY OTHER PROGRAMS),\n\
EVEN IF SUCH HOLDER OR OTHER PARTY HAS BEEN ADVISED OF THE POSSIBILITY OF\n\
SUCH DAMAGES.\n\
\n\
  17. Interpretation of Sections 15 and 16.\n\
\n\
  If the disclaimer of warranty and limitation of liability provided\n\
above cannot be given local legal effect according to their terms,\n\
reviewing courts shall apply local law that most closely approximates\n\
an absolute waiver of all civil liability in connection with the\n\
Program, unless a warranty or assumption of liability accompanies a\n\
copy of the Program in return for a fee.\n\
",
				   "关于window_video",
				   MB_OK);
	}

	// 如果开启自动截图
	if (auto_screenshot)
	{
		// 获取屏幕dc
		dc = GetDC(NULL);
		if (dc == NULL)
			return 0; // 出错直接退出
		// 创建兼容dc与兼容位图
		cdc = CreateCompatibleDC(dc);
		bmp = CreateCompatibleBitmap(dc, screen_w, screen_h);
		if (cdc == NULL || bmp == NULL)
			return 0; // 出错直接退出
		SelectObject(cdc, bmp);
		// 分配内存用于储存位图
		while ((screenshot = malloc(screen_w * screen_h * 4)) == NULL)
			;
		// 创建文件夹
		mkdir("./img_out");
	}

	// 注册窗口类
	WNDCLASS wndclass = {0};
	wndclass.style = CS_HREDRAW | CS_VREDRAW;					  // 样式
	wndclass.lpszClassName = "myclass";							  // 类名
	wndclass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH); // 背景颜色，这里使用白色
	wndclass.lpfnWndProc = DefWindowProc;						  // 处理函数
	wndclass.hInstance = hInst;									  // 实例句柄
	wndclass.hIcon = LoadIcon(NULL, IDI_APPLICATION);			  // 最小化图标:使用缺省图标
	wndclass.hCursor = LoadCursor(NULL, IDC_ARROW);				  // 光标
	if (RegisterClass(&wndclass) == NULL)
		return 0; // 窗口类注册失败就直接退出
				  // 如果想要加错误处理就在此处添加

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
			window_list[idx] = CreateWindowEx(
				WS_EX_TOPMOST,										// 加上 WS_EX_TOOLWINDOW 如果不想在任务栏显示
				"myclass",											// 类名
				"",													// 标题
				WS_POPUP,											// 风格 弹出窗口(就是去掉标题栏)
				viewport_offset_x + viewport_w * x / window_list_w, // 位置x
				viewport_offset_y + viewport_h * y / window_list_h, // 位置y
				viewport_w / window_list_w - window_spacing,		// 宽度
				viewport_h / window_list_h - window_spacing,		// 高度
				NULL,												// 父窗口
				NULL,												// 子菜单
				hInst,												// 该窗口应用程序的实例句柄
				NULL);
			// 创建失败直接退出，不管了
			if (window_list[idx] == NULL)
				return 0;
			// 显示窗口
			ShowWindow(window_list[idx], SW_SHOW);
			window_is_show[idx] = true;
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

	// 程序结束，退出(一般不会从这里退出)
	return 0;
}
