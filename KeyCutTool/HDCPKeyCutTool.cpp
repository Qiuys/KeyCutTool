#include "HDCPKeyCutTool.h"
using namespace std;

HDCPKeyCutTool::~HDCPKeyCutTool() {
	closeFiles();
}

/*
DO NOT USE THIS FUNCTION. PLEASE USE readKeyCountFormat1 INSTEAD!
get HDCP key count from file which type is 1
*/
int HDCPKeyCutTool::getHDCPKeyCount1() {
	int m;//current byte that getted from file
	int sum = 0;//count of key
	int weights = 256 * 256 * 256;//weight of current byte

	for (int i = 0;i < 4;i++) {
		m = fgetc(InFile);
		sum += m*weights;
		weights /= 256;
	}

	return sum;
}

/*
DO NOT USE THIS FUNCTION. PLEASE USE readKeyCountFormat2 INSTEAD!
get HDCP key count from file which type is 2
*/
int HDCPKeyCutTool::getHDCPKeyCount2() {
	int m;//current byte that getted from file
	int sum = 0;//count of key
	int weights = 1000000;//weight of current byte

	for (int i = 0;i < 4;i++) {
		m = fgetc(InFile);
		sum += m*weights;
		weights /= 100;
	}

	return sum;
}

/*
DO NOT USE THIS FUNCTION. PLEASE USE readKeyCountFormat3 INSTEAD!
get HDCP key count from file which type is 3
*/
int HDCPKeyCutTool::getHDCPKeyCount3() {
	int m;//current byte that getted from file
	int sum = 0;//count of key
	int digit = 0;//a digit of current HEX byte
	int weights = 10000000;//weight of current digit

	for (int i = 0;i < 4;i++) {
		m = fgetc(InFile);
		digit = m / 16;
		sum += (digit*weights);
		weights /= 10;

		digit = m % 16;
		sum += (digit*weights);
		weights /= 10;
	}

	return sum;
}

/*
change number of key int  to int array
prepare to write into file which head type is 1
1000个key写为 00 00 03 E8
*/
void HDCPKeyCutTool::setHDCPKeyCountHelp1(int sum, int* mm) {
	int total = 0;
	for (int i = (headLength-1);i >= 0;i--) {
		mm[i] = sum % 256;
		sum = (sum - mm[i]) / 256;
	}
}

/*
change number of key int  to int array
prepare to write into file which head type is 2
1000个key写为 00 00 0A 00
*/
void HDCPKeyCutTool::setHDCPKeyCountHelp2(int sum, int* mm) {
	//int weight = 1000000;
	int weight = 1;
	for (int i = 1;i < headLength;i++) {
		weight *= 100;
	}
	for (int i = 0;i < headLength;i++) {
		mm[i] = sum / weight;
		sum -= (weight*mm[i]);
		weight /= 100;
	}
}

/*
change number of key int  to int array
prepare to write into file which head type is 3
1000个key写为 00 00 10 00
*/
void HDCPKeyCutTool::setHDCPKeyCountHelp3(int sum, int* mm) {
	int m;
	int digit;

	for (int i = (headLength-1);i >= 0;i--) {
		digit = sum % 10;
		m = digit;
		sum = (sum - digit) / 10;

		digit = sum % 10;
		m += digit * 16;
		sum = (sum - digit) / 10;

		mm[i] = m;
	}
}

/*
put 4 byte into file.represent the number of key in the file which type is 1
这个是将1000个key显示为 00 00 03 E8
*/
int HDCPKeyCutTool::setHDCPKeyCount1(int newKeyCount) {
	if (newKeyCount < 0)
		return -1;
	//int mm[4];
	int* mm=new int[headLength];
	setHDCPKeyCountHelp1(newKeyCount, mm);
	for (int i = 0;i < headLength;i++) {
		fputc(mm[i], OutFile);
	}
	return 0;
}

/*
put 4 byte into file.represent the number of key in the file which type is 2
这个是将1000个key显示为 00 00 0A 00
*/
int HDCPKeyCutTool::setHDCPKeyCount2(int newKeyCount) {
	if (newKeyCount < 0)
		return -1;
	//int mm[4];
	int* mm = new int[headLength];
	setHDCPKeyCountHelp2(newKeyCount, mm);
	for (int i = 0;i < headLength;i++) {
		fputc(mm[i], OutFile);
	}
	return 0;
}

/*
put 4 byte into file.represent the number of key in the file which type is 3
这个是将1000个key写为 00 00 10 00
*/
int HDCPKeyCutTool::setHDCPKeyCount3(int newKeyCount) {
	if (newKeyCount < 0)
		return -1;
	//int mm[4];
	int* mm = new int[headLength];
	setHDCPKeyCountHelp3(newKeyCount, mm);
	for (int i = 0;i < headLength;i++) {
		fputc(mm[i], OutFile);
	}
	return 0;
}

/*Get key quantity from head in which type is 1.
1000个key显示为 00 00 03 E8*/
int HDCPKeyCutTool::readKeyCountFormat1(FILE * InFile, int headLength) {
	int m;//current byte that getted from file
	int sum = 0;//count of key
	int weights = 1;//weight of current byte
	for (int i = 1;i < headLength;i++) {
		weights *= 256;
	}

	for (int i = 0;i < headLength;i++) {
		m = fgetc(InFile);
		sum += m*weights;
		weights /= 256;
	}
	return sum;
}

/*Get key quantity from head in which type is 2.
1000个key显示为 00 00 0A 00*/
int HDCPKeyCutTool::readKeyCountFormat2(FILE * InFile, int headLength) {
	int m;//current byte that getted from file
	int sum = 0;//count of key
	int weights = 1;//weight of current byte
	for (int i = 1;i < headLength;i++) {
		weights *= 100;
	}
	for (int i = 0;i < headLength;i++) {
		m = fgetc(InFile);
		sum += m*weights;
		weights /= 100;
	}

	return sum;
}

/*Get key quantity from head in which type is 3.
1000个key显示为 00 00 10 00*/
int HDCPKeyCutTool::readKeyCountFormat3(FILE * InFile, int headLength) {
	int m;//current byte that getted from file
	int sum = 0;//count of key
	int digit = 0;//a digit of current HEX byte
	int weights = 10;//weight of current digit
	for (int i = 1;i < headLength;i++) {
		weights *= 100;
	}
	for (int i = 0;i < headLength;i++) {
		m = fgetc(InFile);
		digit = m / 16;
		sum += (digit*weights);
		weights /= 10;

		digit = m % 16;
		sum += (digit*weights);
		weights /= 10;
	}

	return sum;
}

int HDCPKeyCutTool::openFiles(char * inFile, char* outFile) {
	//InFile = fopen(inFile, "rb");
	//OutFile = fopen(outFile, "ab");
	//if (InFile == NULL || OutFile == NULL) {
	//	return -1;
	//}
	if ((fopen_s(&InFile, inFile, "rb") != 0) || (fopen_s(&OutFile, outFile, "ab") != 0)) {
		return -1;
	}
	return 0;
}

void HDCPKeyCutTool::closeFiles() {
	if (InFile != NULL)
		fclose(InFile);
	if (OutFile != NULL)
		fclose(OutFile);
}

/*
get a key from file then store in char* aKey.
return 0 when success
return -1 when fail
*/
int HDCPKeyCutTool::getOneKey(char* aKey) {
	int ret = fread(aKey, sizeof(char), keyLength, InFile);
	if (ret != keyLength) {
		return -1;
	}
	return 0;
}

/*
put a key to file
return the size of key
*/
int HDCPKeyCutTool::setOneKey(char* aKey) {
	int i = 0;
	for (;i < keyLength;i++) {
		fputc(aKey[i], OutFile);
	}
	return i;
}

/*
get current key location in inFile
*/
int HDCPKeyCutTool::getLocationOfKey() {
	long loc = ftell(InFile);
	if (loc < 3)
		return 0;
	return (loc - 4) / keyLength;
}

/*
set a new key location in inFile
*/
int HDCPKeyCutTool::setLocationOfKey(int loc) {
	if (loc >= keyCount || loc < 0)
		return -1;
	fseek(InFile, loc*keyLength + 4, SEEK_SET);
	return getLocationOfKey();
}

//1.第一步，检查输入的参数与key文件是否匹配。
/*
Check the validity of given parameters.
param inFile : The file to be checked.
param headLength : The head is a field record the count of keys in this file.
headLength is the length(bytes) of this field.
param keyLength : The length of an individual key.
param keyCountFormat : The format of input file head.
param aimkeyCountFormat: The format of  output files head.
return : 0: Checked ok. 1: The file can't be open. 2:headLength not mach the file.
3:keyLength not mach the file. 4:keyLength or keyCountFormat not mach the file.
*/
int HDCPKeyCutTool::checkKeyFormat(char * inFile, int headLength, int keyLength, int keyCountFormat, int aimkeyCountFormat) {
	//1. check inFile
	FILE * tempInFile;
	if (fopen_s(&tempInFile, inFile, "rb") != 0) {
		cout << "Open Key File Error!" << endl;
		return 1;// return 1,means the key file can not be opened.Maybe it is not exist.
	}
	//tempInFile = fopen(inFile, "rb");
	//if (tempInFile == NULL) {
	//	//open file failed
	//	cout << "Open Key File Error!" << endl;
	//	return 1;// return 1,means the key file can not be opened.Maybe it is not exist.
	//}

	//2. check headLength
	fseek(tempInFile, 0, SEEK_END);
	int fileSize = ftell(tempInFile);
	cout << "Input file size is : " << fileSize << endl;
	if (headLength >= fileSize) {
		cout << "headLength is longer then the file!" << endl;
		fclose(tempInFile);
		return 2;
	}
	fseek(tempInFile, 0, SEEK_SET);

	//3. check keyLength
	if (keyLength <= 0) {
		cout << "keyLength is wrong!" << endl;
		fclose(tempInFile);
		return 3;
	}

	//4. check keyLength and keyCountFormat.
	int readKeyCount = 0;
	if (keyCountFormat == 1) {
		readKeyCount = readKeyCountFormat1(tempInFile, headLength);
		cout << "read key quantitiy (Format 1) = " << readKeyCount << endl;
		if (headLength + readKeyCount*keyLength != fileSize) {
			cout << "keyLength or keyCountFormat not mach the file!" << endl;
			fclose(tempInFile);
			return 4;
		}
	}else if (keyCountFormat == 2) {
		readKeyCount = readKeyCountFormat2(tempInFile, headLength);
		cout << "read key quantitiy (Format 2) = " << readKeyCount << endl;
		if (headLength + readKeyCount*keyLength != fileSize) {
			cout << "keyLength or keyCountFormat not mach the file!" << endl;
			fclose(tempInFile);
			return 4;
		}
	}else if (keyCountFormat == 3) {
		readKeyCount = readKeyCountFormat3(tempInFile, headLength);
		cout << "read key quantitiy (Format 3) = " << readKeyCount << endl;
		if (headLength + readKeyCount*keyLength != fileSize) {
			cout << "keyLength or keyCountFormat not mach the file!" << endl;
			fclose(tempInFile);
			return 4;
		}
	}else{
		fclose(tempInFile);
		cout << "keyCountFormat is wrong!" << endl;
		return 4;
	}

	if (aimkeyCountFormat != 1 && aimkeyCountFormat !=2 && aimkeyCountFormat != 3) {
		cout << "aimkeyCountFormat is wrong!" << endl;
		fclose(tempInFile);
		return 4;
	}

	//Do some recording.
	HDCPKeyCutTool::keyCount = readKeyCount;
	HDCPKeyCutTool::headLength = headLength;
	HDCPKeyCutTool::keyLength = keyLength;
	HDCPKeyCutTool::inHeadType = keyCountFormat;
	HDCPKeyCutTool::outHeadType = aimkeyCountFormat;

	fclose(tempInFile);
	cout << "Check Successful!" << endl;
	return 0;
}

//2.第二步，检查需要提取的key数量是否会超出key文件的范围
/*
Check wether keys are enough or not.
param keyBeginNum : The number of first key to be seperated from file.
param keyCount : The count of keys to be seperated from file.
return : 0:Can be seperated successfully 1:param is unavailable
2:The keys in file are not enough
*/
int HDCPKeyCutTool::checkCommand(int keyBeginNum, int keyCount) {
	if (keyBeginNum <= 0 || keyCount <= 0) {
		cout << "keyBeginNum or keyCount is unavailable!" << endl;
		return 1;
	}
	int keyLastNum = keyBeginNum + keyCount - 1;
	if (keyLastNum > HDCPKeyCutTool::keyCount) {
		cout << "keys are not enough for you!" << endl;
		return 2;
	}
	return 0;
}

int HDCPKeyCutTool::setOperatedFiles(char * inFile, char * outFile) {
	if (openFiles(inFile, outFile) != 0) {
		cout << "Opening Files Error!" << endl;
		return -1;
	}
	return 0;
}

void HDCPKeyCutTool::cleanOperatedFiles() {
	closeFiles();
}

/*
parameter:
newKeyCount:the count of key that will seperated into target HDCP key file
newKeyLoc:the first index of key in input HDCP key file that begin to seperate into target HDCP key file
*/
int HDCPKeyCutTool::cutKeys(int newKeyCount, int newKeyLoc) {
	if (newKeyLoc < 0 || newKeyLoc >= keyCount || newKeyCount <= 0 || (newKeyLoc + newKeyCount)>keyCount) {
		cout << "Target Key Range Error!" << endl;
		return -1;
	}

	if (setLocationOfKey(newKeyLoc) == -1) {
		cout << "Inner Error. setLocationOfKey" << endl;
		return -1;
	}

	cout << "Cutting From Key Index " << newKeyLoc << endl;
	cout << "Cutting Key Count " << newKeyCount << endl;
	switch (outHeadType) {//不同的key计数格式需要选择不同的函数
	case 1:
		setHDCPKeyCount1(newKeyCount);
		break;
	case 2:
		setHDCPKeyCount2(newKeyCount);
		break;
	case 3:
		setHDCPKeyCount3(newKeyCount);
		break;
	default:
		setHDCPKeyCount1(newKeyCount);
		break;
	}
	char * tempKey = new char[keyLength];
	for (int i = 0;i<newKeyCount;i++) {
		if (getOneKey(tempKey) == -1) {
			cout << "Get Key From Source File Error!" << endl;
			return -1;
		}
		setOneKey(tempKey);
	}

	cout << "Cut Successfully!" << endl;
	return 0;
}