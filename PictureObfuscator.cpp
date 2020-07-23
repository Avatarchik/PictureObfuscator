#pragma comment(lib, "gdiplus.lib")
#include <iostream>
#include <fstream>
#include <string>
#include <windows.h>
#include <gdiplus.h>
#include <vector>
using namespace std;
using namespace Gdiplus;

int GetEncoderClsid(const WCHAR* format, CLSID* pClsid)
{
	UINT num = 0;
	UINT size = 0;

	ImageCodecInfo* pImageCodecInfo = NULL;

	GetImageEncodersSize(&num, &size);
	if (size == 0)
	{
		return -1;
	}
	pImageCodecInfo = (ImageCodecInfo*)(malloc(size));
	if (pImageCodecInfo == NULL)
	{
		return -1;
	}

	GetImageEncoders(num, size, pImageCodecInfo);

	for (UINT j = 0; j < num; ++j)
	{
		if (wcscmp(pImageCodecInfo[j].MimeType, format) == 0)
		{
			*pClsid = pImageCodecInfo[j].Clsid;
			free(pImageCodecInfo);
			return j;
		}
	}

	free(pImageCodecInfo);
	return -1;
}

struct RgbRECT {
	Color col;
	bool Exchanged;
};

struct ChangeSolu {
	int seed1, seed2;
};

int main()
{
	cout << "WELCOME TO <PicObfer> By Steesha. [Release:2020.7.22]" << endl;
	cout << ">> Input your input-picture File. FileName:[e.g: C:\\MyFile.jpg]" << endl;
	string inputFileName, outputFileName;//输入文件,输出文件
	cin >> inputFileName;
	cout << ">> Input your output-picture File. FileName:[e.g: C:\\MyFile1.jpg]" << endl;
	cin >> outputFileName;
	//READ
	Color color;
	GdiplusStartupInput gdi;
	ULONG_PTR t0ken;
	GdiplusStartup(&t0ken, &gdi, NULL);//START UP
	wstring inputFileName_(inputFileName.begin(), inputFileName.end());//string->wstring
	wstring outputFileName_(outputFileName.begin(), outputFileName.end());//string->wstring
	Bitmap* picture = new Bitmap(inputFileName_.c_str(), FALSE);//B I T M A P
	UINT height = picture->GetHeight();//高度
	UINT width = picture->GetWidth();//宽度
	cout << "width " << width << ", height " << height << endl;
	ofstream fout(outputFileName.c_str());
	vector<RgbRECT> Colors;
	vector<ChangeSolu> ExChangeMap;
	RgbRECT locrgb; ChangeSolu locslu;
	for (UINT y = 0; y < height; y++)//遍历色块 x - y
		for (UINT x = 0; x < width; x++) {
			Status st = picture->GetPixel(x, y, &color);//获取图片像素信息
			locrgb.col = color;
			locrgb.Exchanged = false;
			Colors.push_back(locrgb);
			int unit1 = rand() % Colors.size();
			int unit2 = rand() % Colors.size();
			locslu.seed1 = unit1;
			locslu.seed2 = unit2;
			ExChangeMap.push_back(locslu);
		}
	cout << ExChangeMap.size() << endl;
	srand((int)123456);

	int count = 0;
	for (UINT y = 0; y < height; y++)//交换色块 x - y
		for (UINT x = 0; x < width; x++) {
			//确保每一个元素被交换了
			bool exchangedcomplete = false;
			while (!exchangedcomplete) {
				int changeSol1 = ExChangeMap[count].seed1;
				int changeSol2 = ExChangeMap[count].seed2;
				if (!Colors[changeSol1].Exchanged) {//确保seed1的每个像素被移动
					picture->SetPixel(x, y, Colors[changeSol1].col);
					picture->SetPixel(x, y, Colors[changeSol2].col);
					exchangedcomplete = true;
				}
				count++;
				if (count > (ExChangeMap.size() - 200)) {
					cout << changeSol1 << "  " << ExChangeMap.size() << endl;
				}

			}
		}


	CLSID pngClsid;
	GetEncoderClsid(L"image/png", &pngClsid);
	picture->Save(L"d:\\Mosaic2.bmp", &pngClsid, NULL);
	//(outputFileName_.c_str(), &pngClsid, NULL);
	cout << GetLastError() << endl;
	fout.close();

	delete picture;
	GdiplusShutdown(t0ken);
	system("pause");
	return 0;
}