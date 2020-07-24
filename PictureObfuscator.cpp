//==============================================================
//  Copyright (C) 2020 Steeliest Org.. All rights reserved.
//  The information contained herein is confidential, proprietary
//  to Jeteam Inc. Use of this information by anyone other than 
//  authorized employees of Jeteam Inc is granted only under a 
//  written non-disclosure agreement, expressly prescribing the 
//  scope and manner of such use.
//==============================================================
//  Create by Steesha at 2020.7.24.
//  Version 1.0
//  Steesha [steesha@qq.com]
//  注：不建议使用管理员模式启动，否则将无法向控制台里面拖入文件
//  用法：详见README.md
//==============================================================
#pragma comment(lib, "gdiplus.lib")
#include <iostream>
#include <fstream>
#include <string>
#include <windows.h>
#include <gdiplus.h>
#include <vector>
#include <time.h>
using namespace std;
using namespace Gdiplus;

struct GDIStartupInfomation { GdiplusStartupInput input; ULONG_PTR token; };//GDI初始化信息(用于关闭)
enum FileFormat { PNG, BMP, JPEG };//图片文件格式

GDIStartupInfomation StartupGDI();//初始化GDI
void ShutdownGDI(GDIStartupInfomation info);//关闭GDI
Bitmap* ReadInPicture(string inputFile);//读入图片文件
int GetEncoderClsid(const WCHAR* format, CLSID* pClsid);
int SavePicture(Bitmap* picture, string outputFile, FileFormat format);//保存图片，默认PNG，返回GetLastError的内容
Bitmap* Obfuscate(Bitmap* picture, int Rseed);//混淆图片
Bitmap* Deobfuscate(Bitmap* picture, int Rseed);//反混淆图片
void string_replace(std::string& strBig, const std::string& strsrc, const std::string& strdst);//文本替换
std::string GetPathOrURLShortName(std::string strFullName, bool noformat);//文件路径取文件名
int getRand(int min, int max);
string getOutputName(string inputName, bool IsDeobfuscate);// e.g:  C:\MyFile.png -> C:\MyFile_Obfuscated.png / C:\MyFile_Deobfuscated.png
int main()
{
	system("title PictureObfuscator");
	//->Start
	cout << "---------------------------------------------------------------------------------------------------" << endl;
	cout << "WELCOME TO <PictureObfuscator> By Steesha. [Release on 2020 Jul. 23rd]" << endl;
	cout << ">> Input your Picture File. FileName:[e.g: C:\\MyFile.jpg]" << endl;
	cout << ">>If your inputfile is:[e.g: C:\\MyFile.jpg], outputfile will be:[e.g: C:\\MyFile_(de)obfuscated.jpg]" << endl;
	//<-Start

	//->Input
	string inputFileName;
	cin >> inputFileName;
	//<-Input

	//->init
	GDIStartupInfomation GdiInfo = StartupGDI();//初始化GDI
	//<-init

	//->Read
	Bitmap* picture = ReadInPicture(inputFileName);//读入图片文件
	UINT pic_height = picture->GetHeight();//获取图片高度
	UINT pic_width = picture->GetWidth();//获取图片宽度
	cout << "---------------------------------------------------------------------------------------------------" << endl;
	cout << "Picture Info:" << endl;
	cout << "---->Picture Height:" << pic_height << endl << "---->Picture Width:" << pic_width << endl;
	cout << "---------------------------------------------------------------------------------------------------" << endl;
	//<-Read

	//->Choose Method
	bool IsDeobfuscate;
	cout << "METHOD: " << endl << "[0]Obfuscate Picture" << endl << "[1]Deobfuscate Picture" << endl;
	cin >> IsDeobfuscate;
	cout << "YOU chose <" << (IsDeobfuscate ? "Deobfuscate" : "Obfuscate") << "Picture" << ">" << endl;
	//<-Choose Method
	cout << "---------------------------------------------------------------------------------------------------" << endl;
	//->Enter Seed
	cout << "Enter Your  obf/deobf seed[int]:";
	int pseed;
	cin >> pseed;
	//<-Enter Seed
	cout << "---------------------------------------------------------------------------------------------------" << endl;
	cout << "Processing......." << endl;
	system(IsDeobfuscate ? "title Deobfuscating" : "title Obfuscating");
	system("cls");
	//->Process
	picture = IsDeobfuscate ? Deobfuscate(picture, pseed) : Obfuscate(picture, pseed);
	//<-Process

	//->Save
	string outPath = inputFileName;//获取文件输出名
	outPath = getOutputName(outPath, IsDeobfuscate);
	cout << outPath << endl;
	int save_ret = SavePicture(picture, outPath, PNG);
	if (save_ret != 0) {
		//Err
		cout << "SAVE PICTURE ERROR, ERRORCODE <" << save_ret << ">" << endl;
		cout << "Process Exit....." << endl;
		ShutdownGDI(GdiInfo);//关闭GDI
		delete picture;//DELETE
		system("pause");
		return -1;
	}
	else {
		cout << "SAVE PICTURE SUCCESSFUL AT  <" << outPath << ">" << endl;
		cout << "picture seed is<" << pseed << ">" << endl;
		cout << "Process Exit....." << endl;
	}
	//<-Save

	//->Shutdown
	delete picture;//DELETE
	ShutdownGDI(GdiInfo);//关闭GDI
	system("pause");
	return 0;
	//<-Shutdown
}

GDIStartupInfomation StartupGDI()//初始化GDI
{
	GDIStartupInfomation info;
	GdiplusStartup(&info.token, &info.input, NULL);//START UP
	return info;
}

void ShutdownGDI(GDIStartupInfomation info)//关闭GDI
{
	GdiplusShutdown(info.token);
}

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

Bitmap* ReadInPicture(string inputFile) {//读入图片文件
	wstring inputFileName_(inputFile.begin(), inputFile.end());//string->wstring
	Bitmap* picture = new Bitmap(inputFileName_.c_str(), FALSE);
	return picture;
}

int SavePicture(Bitmap* picture, string outputFile, FileFormat format)//保存图片，默认PNG，返回GetLastError的内容
{
	CLSID pngClsid;
	switch (format) {
	case PNG:
		GetEncoderClsid(L"image/png", &pngClsid);
		break;
	case BMP:
		GetEncoderClsid(L"image/bmp", &pngClsid);
		break;
	case JPEG:
		GetEncoderClsid(L"image/jpeg", &pngClsid);
		break;
	}
	wstring outputFileName_(outputFile.begin(), outputFile.end());//string->wstring
	picture->Save(outputFileName_.c_str(), &pngClsid, NULL);
	return GetLastError();
}

clock_t start_t = clock();//时钟

int GetSeeds(int Rseed, vector<int>& TxSeed, vector<int>& TySeed, Bitmap* picture) {//返回lcounter
	srand((int)Rseed);
	int full = picture->GetHeight() * picture->GetWidth();
	int lcounter = 0;
	cout << "[Creating Seeds]" << "0.00" << "%" << endl;
	for (UINT i = 0; i < picture->GetHeight(); i++) {
		for (UINT j = 0; j < picture->GetWidth(); j++) {
			TxSeed.push_back(getRand(0, picture->GetWidth()));
			start_t = clock();
			if (start_t % 100 == 0) {
				cout << "[Creating Seeds]" << ((float)lcounter / full * 100) / 2 << "%" << endl;
			}
			lcounter++;
		}
		TySeed.push_back(getRand(0, picture->GetHeight()));
	}
	cout << "[Creating Seeds]" << "50.00" << "%" << endl;
	return full / 2;
}

Bitmap* Obfuscate(Bitmap* picture, int Rseed) {
	int full = picture->GetHeight() * picture->GetWidth();
	vector<int> TxSeed, TySeed;//Transfer
	GetSeeds(Rseed, TxSeed, TySeed, picture);
	int lcounter = 0;
	//ExChange
	Color colorf; Color colors;
	UINT height = picture->GetHeight(); UINT width = picture->GetWidth();
	for (UINT y = 0; y < height; y++) {
		for (UINT x = 0; x < width; x++) {
			picture->GetPixel(x, y, &colorf);
			picture->GetPixel(TxSeed[x], TySeed[y], &colors);
			picture->SetPixel(x, y, colors);
			picture->SetPixel(TxSeed[x], TySeed[y], colorf);
			start_t = clock();
			if (start_t % 100 == 0) {
				cout << "[Obfuscate]" << 50.0 + ((float)lcounter / full * 100) / 2 << "%" << endl;
			}
			lcounter++;
		}
	}
	cout << "[Obfuscate]" << "100.00" << "%" << endl;
	cout << endl;
	return picture;
}

Bitmap* Deobfuscate(Bitmap* picture, int Rseed) {
	int full = picture->GetHeight() * picture->GetWidth();
	vector<int> TxSeed, TySeed;//Transfer
	GetSeeds(Rseed, TxSeed, TySeed, picture);
	int lcounter = 0;
	//ExChange
	Color colorf, colors;
	UINT height = picture->GetHeight(); UINT width = picture->GetWidth();
	for (UINT i = 0; i < height; i++) {
		for (UINT j = 0; j < width; j++) {
			int x = width - j - 1;
			int y = height - i - 1;
			picture->GetPixel(x, y, &colorf);
			picture->GetPixel(TxSeed[x], TySeed[y], &colors);
			picture->SetPixel(x, y, colors);
			picture->SetPixel(TxSeed[x], TySeed[y], colorf);
			start_t = clock();
			if (start_t % 100 == 0) {
				cout << "[Deobfuscate]" << 50.0 + ((float)lcounter / full * 100) / 2 << "%" << endl;
			}
			lcounter++;
		}
	}
	cout << "[Deobfuscate]" << "100.00" << "%" << endl;
	cout << endl;
	return picture;
}

void string_replace(std::string& strBig, const std::string& strsrc, const std::string& strdst)//文本替换
{
	std::string::size_type pos = 0;
	std::string::size_type srclen = strsrc.size();
	std::string::size_type dstlen = strdst.size();

	while ((pos = strBig.find(strsrc, pos)) != std::string::npos)
	{
		strBig.replace(pos, srclen, strdst);
		pos += dstlen;
	}
}

std::string GetPathOrURLShortName(std::string strFullName, bool noformat)//文件路径取文件名
{
	if (strFullName.empty())
	{
		return "";
	}

	string_replace(strFullName, "/", "\\");

	std::string::size_type iPos = strFullName.find_last_of('\\') + 1;
	strFullName = strFullName.substr(iPos, strFullName.length() - iPos);
	std::string::size_type iPos_ = 0;
	if (noformat) {
		iPos_ = strFullName.find_last_of('.') + 1;
		string strFullName_ = strFullName.substr(iPos_, strFullName.length() - iPos_);
		string_replace(strFullName, "." + strFullName_, "");
	}

	return strFullName;
}

int getRand(int min, int max) {
	return min + (int)max * rand() / (RAND_MAX + 1);
}

string getOutputName(string inputName, bool IsDeobfuscate) {// e.g:  C:\MyFile.png -> C:\MyFile_Obfuscated.png / C:\MyFile_Deobfuscated.png
	string outPath = inputName;
	string fileshort = GetPathOrURLShortName(outPath, true);
	string_replace(outPath, fileshort, fileshort + "_" + (IsDeobfuscate ? "Deobfuscated" : "Obfuscated"));
	return outPath;
}