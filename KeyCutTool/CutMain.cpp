#include <iostream>
#include "KeyToolManager.h"
using namespace std;

void cleanArray(char * inputFile, char * outputFolder, char * newFileName) {
	if (inputFile != NULL) {
		delete[] inputFile;
		inputFile = NULL;
	}
	if (outputFolder != NULL) {
		delete[] outputFolder;
		outputFolder = NULL;
	}
	if (newFileName != NULL) {
		delete[] newFileName;
		newFileName = NULL;
	}
}
int main() {
	while (1) {
		char * inputFile = new char[1024];//"C:/Users/sorou/Desktop/testfolder/HDCP_Key_1000K.bin";
		char * outputFolder = new char[1024];//"C:/Users/sorou/Desktop/testfolder/output";
		char * newFileName = new char[1024];//"HDCP_LIENCE";
		int keyType = 1; //HDCP
		int headLength = 4;
		int keyLength = 308;
		int keyCountFormat = 3;
		int aimkeyCountFormat = 1;
		int KeyBeginNum = 1;//3211;
		int KeyEachFile = 10;//500;
		int KeyFileCount = 1;//247;

		int retval = WORK_FINE;
		KeyToolManager ktm;
		
		cout << "=================HDCP Key 分割工具=================" << endl;
		cout << "================v0.1 20160711 qiuys================" << endl;
		cout << "！个人程序，不保证结果正确性！请用其他工具校验结果！" << endl;
		cout << "===================================================\n" << endl;
		cout << "开始请输入 1     |     退出请输入其他数字 " << endl;
		int inputcommand = 0;
		cin >> inputcommand;
		if( inputcommand != 1 ){
			break;
		}
		cout << "请设置 要切割的Key文件的绝对路径：" << endl;
		cin >> inputFile;
		cout << "请设置 保存切割后新Key文件的空文件夹的绝对路径：" << endl;
		cin >> outputFolder;
		cout << "请输入 切割后新Key的文件名：" << endl;
		cin >> newFileName;
		cout << "请选择 要切割的Key类型（当前只支持HDCP。只能输入 1 ）：" << endl;
		cin >> keyType;
		cout << "请设置 Key的头部长度（最前面有多少个Byte用于记录key数量）(HDCP是 4)：" << endl;
		cin >> headLength;
		cout << "请设置 单个Key的长度(Byte) (HDCP是 308)：" << endl;
		cin >> keyLength;
		cout << "请选择 要切割的Key文件头部计数的类型：" << endl;
		cout << "	1 ： 1000 记录为： 00 00 03 E8" << endl;
		cout << "	2 ： 1000 记录为： 00 00 0A 00" << endl;
		cout << "	3 ： 1000 记录为： 00 00 10 00" << endl;
		cin >> keyCountFormat;
		cout << "请设置 切割后新Key的文件头部计数类型，同上1/2/3：" << endl;
		cin >> aimkeyCountFormat;
		cout << "请选择 从要切割的Key文件 第几个Key开始切割：" << endl;
		cin >> KeyBeginNum;
		cout << "请设置 切割后的新Key文件，每个文件包含多少个Key：" << endl;
		cin >> KeyEachFile;
		cout << "请设置 总共要切割出多少个新Key文件：" << endl;
		cin >> KeyFileCount;

		cout << "参数设置完毕。开始切割请输入 1 ，重新设置参数请输入其他数字 " << endl;
		cin >> inputcommand;
		if (inputcommand != 1) {
			cleanArray(inputFile, outputFolder, newFileName);
			continue;
		}
		
		retval = ktm.setInFile(inputFile);
		if (retval != WORK_FINE) {
			cout << "Error: Main setInFile!" << endl;
			cleanArray(inputFile, outputFolder, newFileName);
			continue;
		}
		retval = ktm.setOutFile(outputFolder, newFileName);
		if (retval != WORK_FINE) {
			cout << "Error: Main setOutFile!" << endl;
			cleanArray(inputFile, outputFolder, newFileName);
			continue;
		}
		retval = ktm.setKeyType(keyType);
		if (retval != WORK_FINE) {
			cout << "Error: Main setKeyType!" << endl;
			cleanArray(inputFile, outputFolder, newFileName);
			continue;
		}
		retval = ktm.setHDCPKeyFormat(headLength, keyLength, keyCountFormat, aimkeyCountFormat);
		if (retval != WORK_FINE) {
			cout << "Error: Main setHDCPKeyFormat!" << endl;
			cleanArray(inputFile, outputFolder, newFileName);
			continue;
		}
		retval = ktm.setHDCPKeyCutParam(KeyBeginNum, KeyEachFile, KeyFileCount);
		if (retval != WORK_FINE) {
			cout << "Error: Main setHDCPKeyCutParam!" << endl;
			cleanArray(inputFile, outputFolder, newFileName);
			continue;
		}
		retval = ktm.startCut();
		if (retval != WORK_FINE) {
			cout << "Error: Main startCut!" << endl;
			cleanArray(inputFile, outputFolder, newFileName);
			continue;
		}

		cout << "=====================完毕=====================\n\n\n" << endl;
		cleanArray(inputFile, outputFolder, newFileName);
	}
	return 0;
}