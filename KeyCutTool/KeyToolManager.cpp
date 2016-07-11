#include "KeyToolManager.h"
#include <afx.h>
using namespace std;

KeyToolManager::~KeyToolManager() {
	if (HDCP_Tool != NULL) {
		delete HDCP_Tool;
		HDCP_Tool = NULL;
	}
	if (inFile != NULL) {
		delete[] inFile;
		inFile = NULL;
	}
	if (outFolder != NULL) {
		delete[] outFolder;
		outFolder = NULL;
	}
	if (filePrefix != NULL) {
		delete[] filePrefix;
		filePrefix = NULL;
	}
}

/*
Check the input file which to be seperated is available or not.
param in : Url of the file.Type: char* , Format: E:/work/VS/tempTestKey/HDCP_KEY_testkey.bin
return : WORK_FINE: File opened successfully. others: File opened failed.
*/
int KeyToolManager::setInFile(char * in) {
	//FILE * InFile = fopen(in, "rb");
	//if (InFile == NULL) {
	//	cout << "Key file is not exist!" << endl;
	//	return -1;
	//}
	FILE * InFile = NULL;
	if (fopen_s(&InFile,in, "rb") != 0) {
		cout << "Key file is not exist!" << endl;
		return MANAGER_PARAM_FILE_OPEN_ERROR;
	}
	fclose(InFile);
	inFile = new char[1024];
	strcpy_s(inFile,1024, in);
	return WORK_FINE;
}

/*
Check the output folder and output file name prefix.
param out: Url of the output folder. Type: char* , Format: E:/work/VS/tempTestKey/out or E:/work/VS/tempTestKey/out/
param fprefix: Name of the new files which is seperated from the src key file. Type: char* , Format: HDCP_KEY
return :WORK_FINE: set sucessfully. others: 1 out url or fprefix is empty. 2 TODO: space of the output disk is not enough.
3 TODO: output folder is denid to be written. 4 fprefix contains "\ / : * ? " < > |  " is illegal.
5 output folder is not exist. 6 output folder is not empty.
*/
int KeyToolManager::setOutFile(char * iout, char * ifprefix) {
	if (iout == NULL || iout == "" || ifprefix == NULL) {
		cout << "Wrong out put folder or file prefix!" << endl;
		return MANAGER_PARAM_FILE_OPEN_ERROR;
	}
	//TODO: Check space of the folder is enough or not.
	//TODO: Check the folder is writable or not.

	char *out=new char[1024];
	char *fprefix=new char[1024];
	strcpy_s(out,1024,iout);
	strcpy_s(fprefix, 1024, ifprefix);

	//Check wether the out put folder url is end with "/"
	int outLen = strlen(out);
	if (out[outLen - 1] != '/') {
		//char aimOut[1024];
		//strcpy(aimOut, out);
		//strcat(aimOut, "/");
		//strcpy_s(aimOut, 1024,out);
		//strcat_s(aimOut, 1024,"/");
		//out = aimOut;
		strcat_s(out, 1024, "/");
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
				return MANAGER_PARAM_FILE_OPEN_ERROR;
			}
		}
	}

	//Check wether the out put folder is exist.
	struct _stat fileStat;
	if (!((_stat(out, &fileStat) == 0) && (fileStat.st_mode & _S_IFDIR)))
	{
		cout << "Out put folder not exsit!" << endl;
		return MANAGER_PARAM_FILE_OPEN_ERROR;
	}

	//Check wether the out put folder is empty.
	char * dest = NULL;
	dest = new char[1024];
	char * postfix = "*.*";
	//strcpy(dest, out);
	//strcat(dest, postfix);
	strcpy_s(dest, 1024,out);
	strcat_s(dest, 1024,postfix);

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
			delete[] dest;
			return MANAGER_PARAM_FILE_OPEN_ERROR;
		}
	}
	cff.Close();
	delete[] dest;

	outFolder = out;
	filePrefix = fprefix;
	return WORK_FINE;
}

/*
Define which kind of key to be operated. Only support HDCP by far.
param keyType: 1 is for HDCP
return : WORK_FINE: set sucessfully. others: key type is not defined
*/
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
		return MANAGER_PARAM_KEY_TYPE_ERROR;
	}
	return WORK_FINE;
}

/*
Define the format of HDCP key file.
param headLength: The begining of key file is several byte that indicate how many keys in the file (head).
For example, there are 4 byte in a HDCP key file. This param define the how many bytes being used here.
param keyLength: The length of a single key.(fixed length, bytes)
param keyCountFormat: The format of head in src key file.
param aimkeyCountFormat: The format of head in output key files
return: WORK_FINE: set sucessfully. others: set failed,please check the param.
*/
int KeyToolManager::setHDCPKeyFormat(int headLength, int keyLength, int keyCountFormat, int aimkeyCountFormat) {
	int retval = WORK_FINE;
	if ((retval = HDCP_Tool->checkKeyFormat(inFile, headLength, keyLength, keyCountFormat, aimkeyCountFormat)) != WORK_FINE) {
		return retval;
	}
	return WORK_FINE;
}

/*
Determine the quantity of HDCP keys to be cutted from the src HDCP key file.
param keyBeginNum: Cut the keys in src file beginning with this key.
param keyEachFile: How many keys in a single output key file.
param keyFileCount: How many key files will be created.
return: WORK_FINE: parameters are fine. others: 1 param illegal. 2 Key quantity is not enough for requirement.
*/
int KeyToolManager::setHDCPKeyCutParam(int keyBeginNum, int keyEachFile, int keyFileCount) {
	if (keyBeginNum <= 0 || keyEachFile <= 0 || keyFileCount <= 0) {
		cout << "setHDCPKeyCutParam is wrong!" << endl;
		return MANAGER_PARAM_KEY_RANGESET_ERROR;
	}
	int retval = WORK_FINE;

	if ((retval = HDCP_Tool->checkCommand(keyBeginNum, keyEachFile*keyFileCount)) != WORK_FINE) {
		return retval;
	}
	KeyToolManager::keyBeginNum = keyBeginNum;
	KeyToolManager::keyEachFile = keyEachFile;
	KeyToolManager::keyFileCount = keyFileCount;
	cout << "Set HDCP key cut param finish." << endl;
	return WORK_FINE;
}

/*
Call this function to cut keys from src key file and generate dest key files.
return: WORK_FINE: work fine. others: something wrong.
*/
int KeyToolManager::startCut() {
	int retval = WORK_FINE;
	int newKeyLoc = keyBeginNum - 1;//Index of a key,like 0/1/2 ...
	int newKeyCount = keyEachFile;//How many keys will be cutted into one key file.
	int filesCount = keyFileCount;//How many dest key files will be created.

	for (int i = 1;i <= filesCount;i++) {
		char newfileName[3000] = "";
		//strcpy(newfileName, outFolder);
		//strcat(newfileName, filePrefix);
		strcpy_s(newfileName, 1024,outFolder);//absolute path of dest output folder
		strcat_s(newfileName, 1024,filePrefix);//prefix of dest key file name

		strcat_s(newfileName, 1024, "_");

		char newKeyFirstLocString[1024] = "";
		//_itoa((newKeyLoc + 1), newKeyFirstLocString, 10);
		//strcat(newfileName, newKeyFirstLocString);
		_itoa_s((newKeyLoc + 1), newKeyFirstLocString, 10);
		strcat_s(newfileName, 1024,newKeyFirstLocString);//first key's number is

		//strcat(newfileName, "_");
		strcat_s(newfileName, 1024,"_");

		char newKeyLastLocString[1024] = "";
		//_itoa((newKeyLoc + newKeyCount), newKeyLastLocString, 10);
		//strcat(newfileName, newKeyLastLocString);
		_itoa_s((newKeyLoc + newKeyCount), newKeyLastLocString, 10);
		strcat_s(newfileName, 1024,newKeyLastLocString);//last key's number 

		//strcat(newfileName, "_");
		strcat_s(newfileName, 1024,"_");

		char newKeyCountString[1024];
		//_itoa(newKeyCount, newKeyCountString, 10);
		//strcat(newfileName, newKeyCountString);
		_itoa_s(newKeyCount, newKeyCountString, 10);
		strcat_s(newfileName, 1024,newKeyCountString);//key quantity is a part of dest key file name

		retval = HDCP_Tool->setOperatedFiles(inFile, newfileName);
		if (retval != WORK_FINE) {
			HDCP_Tool->cleanOperatedFiles();
			return retval;
		}
		retval = HDCP_Tool->cutKeys(newKeyCount, newKeyLoc);
		if (retval != WORK_FINE) {
			HDCP_Tool->cleanOperatedFiles();
			return retval;
		}
		HDCP_Tool->cleanOperatedFiles();

		newKeyLoc += newKeyCount;//update index for next loop
	}
	return WORK_FINE;
}