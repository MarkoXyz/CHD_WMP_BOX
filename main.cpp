#include<stdio.h>
#include<stdlib.h>
#include <iostream>
#include <windows.h>
#include <process.h>
#include <WinSock2.h>
#include <exception>
#include<opencv2/opencv.hpp>
//#include "my_typedef.h"
using namespace std;
using namespace cv;

//变量类型
typedef unsigned long CHD_WMP_T; // 设备句柄，这里是0
typedef unsigned char CHD_U8;
typedef char CHD_S8;
typedef unsigned short CHD_U16;
typedef short CHD_S16;
typedef unsigned int CHD_U32;
typedef int CHD_S32;
typedef unsigned long CHD_UL32;
typedef long CHD_SL32;
typedef void CHD_VOID;
#define CONST const
#define SUCCESSFUL (0x0000)

//typedef enum{}是枚举数据型，定义后变量只能返回包含的数值
//定义设备状态
typedef enum {
	CHD_RET_SUCCESS = SUCCESSFUL, // 成功
	CHD_RET_FAILED = (SUCCESSFUL - 1), // 失败
	CHD_RET_TIMEOUT = (SUCCESSFUL - 2), // 超时
	CHD_RET_CONNECT_ERROR = (SUCCESSFUL - 3), // 连接错误
	CHD_RET_SEND_ERROR = (SUCCESSFUL - 4), // 网络错误
	CHD_RET_RECV_ERROR = (SUCCESSFUL - 5), // 网络错误
	CHD_RET_DISCONNECT = (SUCCESSFUL - 6), // 设备已掉线
	CHD_RET_HANDLE_ERROR = (SUCCESSFUL - 7), // 句柄错误
	CHD_RET_SESSIONID_ERROR = (SUCCESSFUL - 8), // 会话ID错误
	CHD_RET_PROTOCOL_ERROR = (SUCCESSFUL - 9), // 协议错误
}CHD_RET_E;

//定义视频格式
typedef enum {
	CHD_FMT_YUYV = 0x1, //YUYV 数据
	CHD_FMT_MJPEG, //MJPEG 数据
	CHD_FMT_H264, //H264 数据
}CHD_VIDEO_FMT_E;

//定义监听数据类型
typedef enum{
	CHD_STREAM_VIDEO = 0x00, //视频数据
	CHD_STREAM_PICTURE = 0x01, //图片数据
	CHD_STREAM_AUDIO = 0x02, //音频数据
	CHD_STREAM_SERIAL = 0x03, //串口数据
	CHD_PARAM_CHANGE_VABILITY = 0X04, //像头性能参数发生改变
	CHD_PARAM_CHANGE_VPARAM = 0X05, //摄像头设置参数发生改变
	CHD_PARAM_CHANGE_VCTRL = 0X06, //摄像头控制参数发生改变
	CHD_PARAM_CHANGE_AUDEO = 0X07, //音频参数发生改变
	CHD_PARAM_CHANGE_SERIAL = 0X08, //串口参数发生改变
	CHD_PARAM_CHANGE_GPIO = 0X09, //GPIO状态发生改变
}CHD_STREAM_TYPE_E;

//定义图像参数
typedef struct {
	CHD_U32 bexist; //当前摄像头是否存在
	CHD_U32 format; //视频格式
	CHD_U32 resolu; //视频分辨率，由图像宽和高组合而成
	CHD_U32 fps; //视频帧率
	CHD_U32 BPS; //传输速率
	CHD_U32 timestamp; //时间戳
	CHD_U32 datalen; //当前帧视频数据长度
	CHD_S8 pData[0]; //视频数据起始地址
}CHD_VIDEO_FRAME_S;

//定义视频参数
typedef struct{
    CHD_U32 format;
    CHD_U32 resolu;
    CHD_U32 fps;
    CHD_U32 maxfps;
} CHD_VIDEO_PARAM_S;

//定义时间参数
typedef struct{
    CHD_U16 year;
    CHD_U8 month;
    CHD_U8 mday;
    CHD_U8 wday;
    CHD_U8 hour;
    CHD_U8 minute;
    CHD_U8 second;
} CHD_TIME_S;

//声明函数指针typedef char(*fun)(int),即函数fun是int为参数char返回
//基本操作
typedef CHD_RET_E(*CHD_ConnectDevice)(CHD_WMP_T * phandle, CHD_S8 * IPAddress);
typedef CHD_RET_E(*CHD_Disconnect)(CHD_WMP_T handle);
typedef CHD_RET_E(*CHD_Poll)(CHD_WMP_T handle, CHD_STREAM_TYPE_E * type, CHD_U32 timeout_sec, CHD_U32 timeout_msec);
//视频操作
typedef CHD_RET_E(*CHD_Video_Begin)(CHD_WMP_T handle);
typedef CHD_RET_E(*CHD_Video_End)(CHD_WMP_T handle);
typedef CHD_RET_E(*CHD_Video_RequestVideoData)(CHD_WMP_T handle, CHD_VIDEO_FRAME_S ** pvdata);
typedef CHD_RET_E(*CHD_Video_ReleaseVideoData)(CHD_WMP_T handle, CHD_VIDEO_FRAME_S * pvdata);
typedef CHD_RET_E(*CHD_Video_RequestPicData)(CHD_WMP_T handle,CHD_VIDEO_FRAME_S ** pvdata);
typedef CHD_RET_E(*CHD_Video_ReleasePicData)(CHD_WMP_T handle, CHD_VIDEO_FRAME_S * pvdata);
typedef CHD_RET_E(*CHD_Video_GetParam)(CHD_WMP_T handle, CONST CHD_VIDEO_PARAM_S * pstParam);
typedef CHD_RET_E(*CHD_Video_GetFPS)(CHD_WMP_T handle, CHD_U32 * pFPS);
typedef void(*CHD_Video_AnalysisResolu)(int resolu, unsigned int * w, unsigned int * h);
//串口操作
typedef CHD_RET_E(*CHD_Serial_Begin)(CHD_WMP_T handle);
typedef CHD_RET_E(*CHD_Serial_End)(CHD_WMP_T handle);
typedef CHD_RET_E(*CHD_Serial_SendData)(CHD_WMP_T handle, CONST CHD_VOID * pdata, CHD_U32 datalen);
//获取时间
typedef CHD_RET_E(*CHD_GetSystemTime)(CHD_WMP_T handle, CHD_TIME_S * stime);


//定义函数指针变量
CHD_ConnectDevice CHD_WMP_ConnectDevice;
CHD_Disconnect CHD_WMP_Disconnect;
CHD_Poll CHD_WMP_Poll;
CHD_Video_Begin CHD_WMP_Video_Begin;
CHD_Video_End CHD_WMP_Video_End;
CHD_Video_RequestVideoData CHD_WMP_Video_RequestVideoData;
CHD_Video_ReleaseVideoData CHD_WMP_Video_ReleaseVideoData;
CHD_Video_AnalysisResolu CHD_WMP_Video_AnalysisResolu;
CHD_Serial_Begin CHD_WMP_Serial_Begin;
CHD_Serial_End CHD_WMP_Serial_End;
CHD_Serial_SendData CHD_WMP_Serial_SendData;






int thread_run = 0;
CHD_WMP_T handle = 0;
/*unsigned __stdcall chird_thread(void *arg){return 0;}*/
int main(int argc, char* argv[])
{
    //LoadLibraryA，GetProcAddress，FreeLibrary是动态库的显式调用
    HMODULE CHD_dll = LoadLibraryA("CHD_WMP.dll");
    //判断动态库加载状态
    if(CHD_dll == NULL){
        cout << "动态库CHD_WMP.dll加载失败" << endl;
        return 0;
    }
	else{
	   CHD_WMP_ConnectDevice = (CHD_ConnectDevice)GetProcAddress(CHD_dll, "CHD_WMP_ConnectDevice");
	   CHD_WMP_Disconnect = (CHD_Disconnect)GetProcAddress(CHD_dll, "CHD_WMP_Disconnect");
	   CHD_WMP_Poll = (CHD_Poll)GetProcAddress(CHD_dll, "CHD_WMP_Poll");
	   CHD_WMP_Video_Begin = (CHD_Video_Begin)GetProcAddress(CHD_dll, "CHD_WMP_Video_Begin");
	   CHD_WMP_Video_End = (CHD_Video_End)GetProcAddress(CHD_dll, "CHD_WMP_Video_End");
	   CHD_WMP_Video_RequestVideoData = (CHD_Video_RequestVideoData)GetProcAddress(CHD_dll, "CHD_WMP_Video_RequestVideoData");
	   CHD_WMP_Video_ReleaseVideoData = (CHD_Video_ReleaseVideoData)GetProcAddress(CHD_dll, "CHD_WMP_Video_ReleaseVideoData");
	   CHD_WMP_Serial_Begin = (CHD_Serial_Begin)GetProcAddress(CHD_dll, "CHD_WMP_Serial_Begin");
	   CHD_WMP_Serial_End = (CHD_Serial_End)GetProcAddress(CHD_dll, "CHD_WMP_Serial_End");
	   CHD_WMP_Serial_SendData = (CHD_Serial_SendData)GetProcAddress(CHD_dll, "CHD_WMP_Serial_SendData");
	   CHD_WMP_Video_AnalysisResolu = (CHD_Video_AnalysisResolu)GetProcAddress(CHD_dll, "CHD_WMP_Video_AnalysisResolu");
	}

	CHD_RET_E ret;//ret = CHD_WMP_ConnectDevice(&handle, "192.168.100.254");
	ret = CHD_WMP_ConnectDevice(&handle, "192.168.31.114");

	if (ret != CHD_RET_SUCCESS) {
			printf("connect fail, error code:%d\n", ret);
		return -1;
	}

	ret = CHD_WMP_Video_Begin(handle);
	if (ret != CHD_RET_SUCCESS) {
		printf("open video stream fail, error code:%d\n", ret);
		return -1;
	}

	ret = CHD_WMP_Serial_Begin(handle);
	if (ret != CHD_RET_SUCCESS) {
		printf("open serial fail, error code:%d\n", ret);
		return -1;
	}
/*
    thread_run = 1;
    int image_num = 0;
    char path[100] = {0};
	CHD_STREAM_TYPE_E type;
	CHD_VIDEO_FRAME_S * pvdata;
	FILE *fp;
	while(thread_run){
        ret = CHD_WMP_Poll(handle, &type, 2, 0);
        switch (type) {
        case CHD_STREAM_VIDEO: // 视频data);
            ret = CHD_WMP_Video_RequestVideoData(handle, &pvdata);
            //printf("%d\n", &pvdata -> fps);
            sprintf(path, "snap\\image%d.jpg", image_num);
            fp = fopen(path, "wb");
            try {
                for (int i = 0; i < pvdata -> datalen; i++)
                    fwrite(&pvdata -> pData[i], 1, 1, fp);
            }
            catch (const char* e) {}
            fclose(fp);
            image_num ++;
        CHD_WMP_Video_ReleaseVideoData(handle, pvdata);
        Sleep(500);
        }
	}
*/
    printf("hello world");
    return 0;
}







