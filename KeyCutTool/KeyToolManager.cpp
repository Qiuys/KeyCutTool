#include "KeyToolManager.h"
#include <afx.h>
using namespace std;

/*Check the input file which to be seperated is available or not.
param in : Url of the file.Type: char* , Format: E:/work/VS/tempTestKey/HDCP_KEY_testkey.bin
return : 0 : File opened successfully. -1 File opened failed.*/
int KeyToolManager::setInFile(char * in) {
	FILE * InFile = fopen(in, "rb");
	if (InFile == NULL) {
		cout << "Key file is not exist!" << endl;
		return -1;
	}
	fclose(InFile);
	inFile = in;
	return 0;
}

/*Check the output folder and output file name prefix.
param out: Url of the output folder. Type: char* , Format: E:/work/VS/tempTestKey/out or E:/work/VS/tempTestKey/out/
param fprefix: Name of the new files which is seperated from the src key file. Type: char* , Format: HDCP_KEY
return :0 set sucessfully. 1 out url or fprefix is empty. 2 TODO: space of the output disk is not enough.
3 TODO: output folder is denid to be written. 4 fprefix contains "\ / : * ? " < > |  " is illegal.
5 output folder is not exist. 6 output folder is not empty.
*/
int KeyToolManager::setOutFile(char * out, char * fprefix) {
	if (out == NULL || out == "" || fprefix == NULL) {
		cout << "Wrong out put folder or file prefix!" << endl;
		return 1;
	}
	//TODO: Check space of the folder is enough or not.
	//TODO: Check the folder is writable or not.

	//Check wether the out put folder url is end with "/"
	int outLen = strlen(out);
	if (out[outLen - 1] != '/') {
		char * aimOut = (char *)malloc(1024);
		strcpy(aimOut, out);
		strcat(aimOut, "/");
		out = aimOut;
	}

	//Check wether the out put file name contains  \ / : * ? " < > |  
	if (fprefix != "") {
		char * temp = NULL;
		char * donotuse[10] = { "/","\\",":","*","?","\"","<",">","|"," " };
		int donotuseLen = sizeof(donotuse) / sizeof(char*);
		for (int i = 0;i < donotuseLen;i++) {
			temp = strstr(fprefix, donotuse[i]);
			if (temp != NULL) {
				cout << "Error file prefix! Contains \'" << donotuse[i] << "\'" << endl;
				return 4;
			}
		}
	}

	//Check wether the out put folder is exist.
	struct _stat fileStat;
	if (!((_stat(out, &fileStat) == 0) && (fileStat.st_mode & _S_IFDIR)))
	{
		cout << "Out put folder not exsit!" << endl;
		return 5;
	}

	//Check wether the out put folder is empty.
	char * dest = NULL;
	dest = (char *)malloc(1024);
	char * postfix = "*.*";
	strcpy(dest, out);
	strcat(dest, postfix);

	CString csout = dest;
	CFileFind cff;
	BOOL finding = cff.FindFile(csout);
	while (finding)
	{
		finding = cff.FindNextFile();
		if (cff.IsDots()) {
			continue;//skip . and ..
		}
		else {
			cout << "Out put folder is not empty!" << endl;
			cff.Close();
			return 6;
		}
	}
	cff.Close();
	outFolder = out;
	filePrefix = fprefix;
	return 0;
}

/*选择要分割的密钥类型keyType 当前只支持 keyType==1（HDCP key)*/
/*Define which kind of key to be operated. Only support HDCP by far.
param keyType: 1 is for HDCP
return : 0 set sucessfully. 10 key type is not defined*/
int KeyToolManager::setKeyType(int keyType) {
	cout << "Key Type is " << keyType << endl;
	switch (keyType) {
	case 1:
		KeyToolManager::keyType = 1;
		HDCP_Tool = new HDCPKeyCutTool();
		break;
	default:
		cout << "Unavailable Key Type: " << keyType << endl;
		KeyToolManager::keyType = 0;
		return 10;
	}
	return 0;
}

/*Define the format of HDCP key file.
param headLength: The begining of key file is several byte that indicate how many keys in the file (head).
For example, there are 4 byte in a HDCP key file. This param define the how many bytes being used here.
param keyLength: The length of a single key.(fixed length, bytes)
param keyCountFormat: The format of head in src key file.
param aimkeyCountFormat: The format of head in output key files
return: 0 set sucessfully. 1 set failed,please check the param.*/
int KeyToolManager::setHDCPKeyFormat(int headLength, int keyLength, int keyCountFormat, int aimkeyCountFormat) {
	if (HDCP_Tool->checkKeyFormat(inFile, headLength, keyLength, keyCountFormat, aimkeyCountFormat) != 0) {
		return 1;
	}
	return 0;
}

/*Determine the quantity of HDCP keys to be cutted from the src HDCP key file.
param keyBeginNum: Cut the keys in src file beginning with this key.
param keyEachFile: How many keys in a single output key file.
param keyFileCount: How many key files will be created.
return: 0 parameters are fine. 1 param illegal. 2 Key quantity is not enough for requirement.
*/
int KeyToolManager::setHDCPKeyCutParam(int keyBeginNum, int keyEachFile, int keyFileCount) {
	if (keyBeginNum <= 0 || keyEachFile <= 0 || keyFileCount <= 0) {
		cout << "setHDCPKeySeperateParam is wrong!" << endl;
		return 1;
	}

	if (HDCP_Tool->checkCommand(keyBeginNum, keyEachFile*keyFileCount) != 0) {
		return 2;
	}
	KeyToolManager::keyBeginNum = keyBeginNum;
	KeyToolManager::keyEachFile = keyEachFile;
	KeyToolManager::keyFileCount = keyFileCount;
	cout << "Set HDCP key cut param finish." << endl;
	return 0;
}

int KeyToolManager::startCut() {
	int newKeyLoc = keyBeginNum - 1;//起始下标
	int newKeyCount = keyEachFile;//每个文件要包含多少个KEY
	int filesCount = keyFileCount;//247;//总共要多少个文件

	for (int i = 1;i <= filesCount;i++) {
		char newfileName[1024] = "";
		strcpy(newfileName, outFolder);//构造文件名前缀
		strcat(newfileName, filePrefix);

		char newKeyFirstLocString[1024] = "";
		_itoa((newKeyLoc + 1), newKeyFirstLocString, 10);
		strcat(newfileName, newKeyFirstLocString);//将起始key编号写到文件名中

		strcat(newfileName, "_");

		char newKeyLastLocString[1024] = "";
		_itoa((newKeyLoc + newKeyCount), newKeyLastLocString, 10);
		strcat(newfileName, newKeyLastLocString);//将末尾key编号写到文件名中

		strcat(newfileName, "_");

		char newKeyCountString[100];
		_itoa(newKeyCount, newKeyCountString, 10);
		strcat(newfileName, newKeyCountString);//将key数量写到文件名中

		HDCP_Tool->setOperatedFiles(inFile, newfileName);
		HDCP_Tool->cutKeys(newKeyCount, newKeyLoc);
		HDCP_Tool->cleanOperatedFiles();

		newKeyLoc += newKeyCount;//更新下标
	}
	return 0;
}