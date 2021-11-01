#include "main.h"
#include <stdio.h>
#include <windows.h>
#include <iostream>
#include "Comm/DtComm.h"
#include <fstream>
#include "thread"
#include <string>
#include "BufferManager/BufferManger.h"
#include "IniFileHelper/inifile.h"
using namespace inifile;

#define ImageWidthPixel 640
#define ImageHeightPixel 480

using namespace std;
void WriteImageToDat(char* path, char* image, int imageLen)
{
	ofstream outFile(path, ios::out | ios::binary);
	if (!outFile)
	{
		printf("Open File Error:%s", path);
	}

	outFile.write(image, imageLen);
	outFile.close();
}

void Save(BufferManager<byte>* manager, int num)
{
	for (size_t i = 0; i < num; i++)
	{
		int size;
		const byte* image = (*manager).PopBuffer(&size);
		if (image == 0)
		{
			i--;
			continue;
		}
		string path = "Data\\Bg_Gray" + to_string(i) + ".dat";
		WriteImageToDat(&path[0], (char*)image, size);
	}
}

void GetResolution(uint16_t* pixelWidth, uint16_t* pixelHeight)
{
	IniFile iniFile;
	char path[128] = "Config.ini";
	if (0 != iniFile.Load(path)) {
		cout << "can't load " << path << endl;
		return;
	}

	iniFile.GetIntValueOrDefault("Resolution", "PixelWidth", (int*)pixelWidth, ImageWidthPixel);
	iniFile.GetIntValueOrDefault("Resolution", "PixelHeight", (int*)pixelHeight, ImageHeightPixel);
}

int main(int argc, char* argv[])
{
	DtComm* comm = DtComm::Instance();
	int ret = comm->OpenDevice();

	char path[128] = "config.ini";
	ret = comm->InitDevice(path);

	comm->LoadSif2610Regs(path);

	comm->InitSPI();
	comm->LoadVcselDriverRegs(path);

	uint16_t width, height;
	GetResolution(&width, &height);

	comm->StartStream(width, height);

	BufferManager<byte> rawBuffer(30);
	thread ty = thread(Save, &rawBuffer, UINT32_MAX);

	while (true)
	{
		byte* image = new byte[width * height * 2];
		comm->GetSensorImage(image);
		rawBuffer.PushBuffer(image, width * height * 2);
	}
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
