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
		
		cout << "=================HDCP Key �ָ��=================" << endl;
		cout << "================v0.1 20160711 qiuys================" << endl;
		cout << "�����˳��򣬲���֤�����ȷ�ԣ�������������У������" << endl;
		cout << "===================================================\n" << endl;
		cout << "��ʼ������ 1     |     �˳��������������� " << endl;
		int inputcommand = 0;
		cin >> inputcommand;
		if( inputcommand != 1 ){
			break;
		}
		cout << "������ Ҫ�и��Key�ļ��ľ���·����" << endl;
		cin >> inputFile;
		cout << "������ �����и����Key�ļ��Ŀ��ļ��еľ���·����" << endl;
		cin >> outputFolder;
		cout << "������ �и����Key���ļ�����" << endl;
		cin >> newFileName;
		cout << "��ѡ�� Ҫ�и��Key���ͣ���ǰֻ֧��HDCP��ֻ������ 1 ����" << endl;
		cin >> keyType;
		cout << "������ Key��ͷ�����ȣ���ǰ���ж��ٸ�Byte���ڼ�¼key������(HDCP�� 4)��" << endl;
		cin >> headLength;
		cout << "������ ����Key�ĳ���(Byte) (HDCP�� 308)��" << endl;
		cin >> keyLength;
		cout << "��ѡ�� Ҫ�и��Key�ļ�ͷ�����������ͣ�" << endl;
		cout << "	1 �� 1000 ��¼Ϊ�� 00 00 03 E8" << endl;
		cout << "	2 �� 1000 ��¼Ϊ�� 00 00 0A 00" << endl;
		cout << "	3 �� 1000 ��¼Ϊ�� 00 00 10 00" << endl;
		cin >> keyCountFormat;
		cout << "������ �и����Key���ļ�ͷ���������ͣ�ͬ��1/2/3��" << endl;
		cin >> aimkeyCountFormat;
		cout << "��ѡ�� ��Ҫ�и��Key�ļ� �ڼ���Key��ʼ�и" << endl;
		cin >> KeyBeginNum;
		cout << "������ �и�����Key�ļ���ÿ���ļ��������ٸ�Key��" << endl;
		cin >> KeyEachFile;
		cout << "������ �ܹ�Ҫ�и�����ٸ���Key�ļ���" << endl;
		cin >> KeyFileCount;

		cout << "����������ϡ���ʼ�и������� 1 ���������ò����������������� " << endl;
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

		cout << "=====================���=====================\n\n\n" << endl;
		cleanArray(inputFile, outputFolder, newFileName);
	}
	return 0;
}