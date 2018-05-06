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

//��������
typedef unsigned long CHD_WMP_T; // �豸�����������0
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

//typedef enum{}��ö�������ͣ���������ֻ�ܷ��ذ�������ֵ
//�����豸״̬
typedef enum {
	CHD_RET_SUCCESS = SUCCESSFUL, // �ɹ�
	CHD_RET_FAILED = (SUCCESSFUL - 1), // ʧ��
	CHD_RET_TIMEOUT = (SUCCESSFUL - 2), // ��ʱ
	CHD_RET_CONNECT_ERROR = (SUCCESSFUL - 3), // ���Ӵ���
	CHD_RET_SEND_ERROR = (SUCCESSFUL - 4), // �������
	CHD_RET_RECV_ERROR = (SUCCESSFUL - 5), // �������
	CHD_RET_DISCONNECT = (SUCCESSFUL - 6), // �豸�ѵ���
	CHD_RET_HANDLE_ERROR = (SUCCESSFUL - 7), // �������
	CHD_RET_SESSIONID_ERROR = (SUCCESSFUL - 8), // �ỰID����
	CHD_RET_PROTOCOL_ERROR = (SUCCESSFUL - 9), // Э�����
}CHD_RET_E;

//������Ƶ��ʽ
typedef enum {
	CHD_FMT_YUYV = 0x1, //YUYV ����
	CHD_FMT_MJPEG, //MJPEG ����
	CHD_FMT_H264, //H264 ����
}CHD_VIDEO_FMT_E;

//���������������
typedef enum{
	CHD_STREAM_VIDEO = 0x00, //��Ƶ����
	CHD_STREAM_PICTURE = 0x01, //ͼƬ����
	CHD_STREAM_AUDIO = 0x02, //��Ƶ����
	CHD_STREAM_SERIAL = 0x03, //��������
	CHD_PARAM_CHANGE_VABILITY = 0X04, //��ͷ���ܲ��������ı�
	CHD_PARAM_CHANGE_VPARAM = 0X05, //����ͷ���ò��������ı�
	CHD_PARAM_CHANGE_VCTRL = 0X06, //����ͷ���Ʋ��������ı�
	CHD_PARAM_CHANGE_AUDEO = 0X07, //��Ƶ���������ı�
	CHD_PARAM_CHANGE_SERIAL = 0X08, //���ڲ��������ı�
	CHD_PARAM_CHANGE_GPIO = 0X09, //GPIO״̬�����ı�
}CHD_STREAM_TYPE_E;

//����ͼ�����
typedef struct {
	CHD_U32 bexist; //��ǰ����ͷ�Ƿ����
	CHD_U32 format; //��Ƶ��ʽ
	CHD_U32 resolu; //��Ƶ�ֱ��ʣ���ͼ���͸���϶���
	CHD_U32 fps; //��Ƶ֡��
	CHD_U32 BPS; //��������
	CHD_U32 timestamp; //ʱ���
	CHD_U32 datalen; //��ǰ֡��Ƶ���ݳ���
	CHD_S8 pData[0]; //��Ƶ������ʼ��ַ
}CHD_VIDEO_FRAME_S;

//������Ƶ����
typedef struct{
    CHD_U32 format;
    CHD_U32 resolu;
    CHD_U32 fps;
    CHD_U32 maxfps;
} CHD_VIDEO_PARAM_S;

//����ʱ�����
typedef struct{
    CHD_U16 year;
    CHD_U8 month;
    CHD_U8 mday;
    CHD_U8 wday;
    CHD_U8 hour;
    CHD_U8 minute;
    CHD_U8 second;
} CHD_TIME_S;

//��������ָ��typedef char(*fun)(int),������fun��intΪ����char����
//��������
typedef CHD_RET_E(*CHD_ConnectDevice)(CHD_WMP_T * phandle, CHD_S8 * IPAddress);
typedef CHD_RET_E(*CHD_Disconnect)(CHD_WMP_T handle);
typedef CHD_RET_E(*CHD_Poll)(CHD_WMP_T handle, CHD_STREAM_TYPE_E * type, CHD_U32 timeout_sec, CHD_U32 timeout_msec);
//��Ƶ����
typedef CHD_RET_E(*CHD_Video_Begin)(CHD_WMP_T handle);
typedef CHD_RET_E(*CHD_Video_End)(CHD_WMP_T handle);
typedef CHD_RET_E(*CHD_Video_RequestVideoData)(CHD_WMP_T handle, CHD_VIDEO_FRAME_S ** pvdata);
typedef CHD_RET_E(*CHD_Video_ReleaseVideoData)(CHD_WMP_T handle, CHD_VIDEO_FRAME_S * pvdata);
typedef CHD_RET_E(*CHD_Video_RequestPicData)(CHD_WMP_T handle,CHD_VIDEO_FRAME_S ** pvdata);
typedef CHD_RET_E(*CHD_Video_ReleasePicData)(CHD_WMP_T handle, CHD_VIDEO_FRAME_S * pvdata);
typedef CHD_RET_E(*CHD_Video_GetParam)(CHD_WMP_T handle, CONST CHD_VIDEO_PARAM_S * pstParam);
typedef CHD_RET_E(*CHD_Video_GetFPS)(CHD_WMP_T handle, CHD_U32 * pFPS);
typedef void(*CHD_Video_AnalysisResolu)(int resolu, unsigned int * w, unsigned int * h);
//���ڲ���
typedef CHD_RET_E(*CHD_Serial_Begin)(CHD_WMP_T handle);
typedef CHD_RET_E(*CHD_Serial_End)(CHD_WMP_T handle);
typedef CHD_RET_E(*CHD_Serial_SendData)(CHD_WMP_T handle, CONST CHD_VOID * pdata, CHD_U32 datalen);
//��ȡʱ��
typedef CHD_RET_E(*CHD_GetSystemTime)(CHD_WMP_T handle, CHD_TIME_S * stime);


//���庯��ָ�����
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
    //LoadLibraryA��GetProcAddress��FreeLibrary�Ƕ�̬�����ʽ����
    HMODULE CHD_dll = LoadLibraryA("CHD_WMP.dll");
    //�ж϶�̬�����״̬
    if(CHD_dll == NULL){
        cout << "��̬��CHD_WMP.dll����ʧ��" << endl;
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
        case CHD_STREAM_VIDEO: // ��Ƶdata);
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







