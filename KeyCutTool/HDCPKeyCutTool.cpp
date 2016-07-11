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
FUNCTION NOT BE USED
Get current key index in src kye file.
return: index of the key.
*/
int HDCPKeyCutTool::getLocationOfKey() {
	long loc = ftell(InFile);
	if (loc < (headLength - 1))
		return -1;
	return (loc - headLength) / keyLength;
}

/*
Change quantity of key from int to int array
prepare to write into file which head type is 1
Type 1 : 00 00 03 E8 represent 1000
param sum: interget to write to head.
param mm: a temp int array to store changed-format sum.
return void.
*/
void HDCPKeyCutTool::setHDCPKeyCountHelp1(int sum, int* mm) {
	int total = 0;
	for (int i = (headLength-1);i >= 0;i--) {
		mm[i] = sum % 256;
		sum = (sum - mm[i]) / 256;
	}
}

/*
Change quantity of key from int to int array
prepare to write into file which head type is 2
Type 2 : 00 00 0A 00 represent 1000
param sum: interget to write to head.
param mm: a temp int array to store changed-format sum.
return void.
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
Change quantity of key from int to int array
prepare to write into file which head type is 3
Type 3 : 00 00 10 00 represent 1000
param sum: interget to write to head.
param mm: a temp int array to store changed-format sum.
return void.
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
Head of key file is serval bytes representing key quantity.
This function writes head of the dest key file which head type is Type 1.
Type 1 : 00 00 03 E8 represent 1000
param newKeyCount: The quantity of key in the new key file
return: WORK_FINE: work fine. others: 1.param less then 0
*/
int HDCPKeyCutTool::setHDCPKeyCount1(int newKeyCount) {
	if (newKeyCount < 0)
		return HDCP_INNER_PARAM_KEY_QUANTITY_ERROR;
	//int mm[4];
	int* mm = new int[headLength];
	setHDCPKeyCountHelp1(newKeyCount, mm);
	for (int i = 0;i < headLength;i++) {
		fputc(mm[i], OutFile);
	}
	delete[] mm;
	mm = NULL;
	return WORK_FINE;
}

/*
Head of key file is serval bytes representing key quantity.
This function writes head of the dest key file which head type is Type 2.
Type 2 : 00 00 0A 00 represent 1000
param newKeyCount: The quantity of key in the new key file
return: WORK_FINE: work fine. others: 1.param less then 0
*/
int HDCPKeyCutTool::setHDCPKeyCount2(int newKeyCount) {
	if (newKeyCount < 0)
		return HDCP_INNER_PARAM_KEY_QUANTITY_ERROR;
	//int mm[4];
	int* mm = new int[headLength];
	setHDCPKeyCountHelp2(newKeyCount, mm);
	for (int i = 0;i < headLength;i++) {
		fputc(mm[i], OutFile);
	}
	delete[] mm;
	mm = NULL;
	return WORK_FINE;
}

/*
Head of key file is serval bytes representing key quantity.
This function writes head of the dest key file which head type is Type 3.
Type 3 : 00 00 10 00 represent 1000
param newKeyCount: The quantity of key in the new key file
return: WORK_FINE: work fine. others: 1.param less then 0
*/
int HDCPKeyCutTool::setHDCPKeyCount3(int newKeyCount) {
	if (newKeyCount < 0)
		return HDCP_INNER_PARAM_KEY_QUANTITY_ERROR;
	//int mm[4];
	int* mm = new int[headLength];
	setHDCPKeyCountHelp3(newKeyCount, mm);
	for (int i = 0;i < headLength;i++) {
		fputc(mm[i], OutFile);
	}
	delete[] mm;
	mm = NULL;
	return WORK_FINE;
}

/*
Read key quantity from src key file head which is Type 1.
Type 1 : 00 00 03 E8 represent 1000
param InFile: The src file to be cutted.
param headLength: Length of head.
return: The quantity represented by head.
*/
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

/*
Read key quantity from src key file head which is Type 2.
Type 2 : 00 00 0A 00 represent 1000
param InFile: The src file to be cutted.
param headLength: Length of head.
return: The quantity represented by head.
*/
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

/* 
Read key quantity from src key file head which is Type 3.
Type 3 : 00 00 10 00 represent 1000
param InFile: The src file to be cutted.
param headLength: Length of head.
return: The quantity represented by head.
*/
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

/*
Open src key file and dest key file.Prepare to read and write.
param inFile: Absolute path of src key file. Format: E:/work/VS/tempTestKey/HDCP_testkey.bin
param outFile: Absolute path of dest key file. Format: E:/work/VS/tempTestKey/out/HDCP.bin_1_2_2
return: WORK_FINE: work fine. others: 1.file open failed.
*/
int HDCPKeyCutTool::openFiles(char * inFile, char* outFile) {
	//InFile = fopen(inFile, "rb");
	//OutFile = fopen(outFile, "ab");
	//if (InFile == NULL || OutFile == NULL) {
	//	return -1;
	//}
	if ((fopen_s(&InFile, inFile, "rb") != 0) || (fopen_s(&OutFile, outFile, "ab") != 0)) {
		return HDCP_INNER_PARAM_FILE_OPEN_ERROR;
	}
	return WORK_FINE;
}

/*
Close src and dest key file handler.
return void.
*/
void HDCPKeyCutTool::closeFiles() {
	if (InFile != NULL)
		fclose(InFile);
	if (OutFile != NULL)
		fclose(OutFile);
}

/*
Get a key from src key file and store it in char* aKey.
param aKey: A char array to store a single key which read from src key file. 
return: WORK_FINE: read sucessfully. others: 1.read failed.
*/
int HDCPKeyCutTool::getOneKey(char* aKey) {
	int ret = fread(aKey, sizeof(char), keyLength, InFile);
	if (ret != keyLength) {
		return HDCP_INNER_KEY_OP_ERROR;
	}
	return WORK_FINE;
}

/*
Write a key to dest key file.
param aKey: A char array storing a single key which will be writed into dest key file.
return: How many bytes have be written into dest key file.
*/
int HDCPKeyCutTool::setOneKey(char* aKey) {
	int i = 0;
	for (;i < keyLength;i++) {
		fputc(aKey[i], OutFile);
	}
	return i;
}

/*
Map next key's index to its first byte location in src key file.
In order to read next key from src key file.
param loc: The index of next key. First key is 0.Second key is 1.
			(After mapped, first HDCP key location is 4, second is 4+308)
return: WORK_FINE: work fine. others: 1.param wrong.
*/
int HDCPKeyCutTool::setLocationOfKey(int loc) {
	if (loc >= keyCount || loc < 0)
		return HDCP_INNER_PARAM_KEY_RANGE_ERROR;
	fseek(InFile, loc*keyLength + headLength, SEEK_SET);
	return WORK_FINE;
}

/*
Step 1.Check the validity of given parameters.
param inFile : The file to be checked.
param headLength : The head is a field record the count of keys in this file.
headLength is the length(bytes) of this field.
param keyLength : The length of an individual key.
param keyCountFormat : The format of input file head.
param aimkeyCountFormat: The format of  output files head.
return : WORK_FINE: Checked ok. others: 1: The file can't be open. 2:headLength not mach the file.
3:keyLength not mach the file. 4:keyLength or keyCountFormat not mach the file.
*/
int HDCPKeyCutTool::checkKeyFormat(char * inFile, int headLength, int keyLength, int keyCountFormat, int aimkeyCountFormat) {
	//1. check inFile
	FILE * tempInFile;
	if (fopen_s(&tempInFile, inFile, "rb") != 0) {
		cout << "Open Key File Error!" << endl;
		return HDCP_PARAM_FILE_OPEN_ERROR;// return 1,means the key file can not be opened.Maybe it is not exist.
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
		return HDCP_PARAM_KEY_HEADLEN_ERROR;
	}
	fseek(tempInFile, 0, SEEK_SET);

	//3. check keyLength
	if (keyLength <= 0) {
		cout << "keyLength is wrong!" << endl;
		fclose(tempInFile);
		return HDCP_PARAM_KEY_KEYLEN_ERROR;
	}

	//4. check keyLength and keyCountFormat.
	int readKeyCount = 0;
	if (keyCountFormat == 1) {
		readKeyCount = readKeyCountFormat1(tempInFile, headLength);
		cout << "read key quantitiy (Format 1) = " << readKeyCount << endl;
		if (headLength + readKeyCount*keyLength != fileSize) {
			cout << "keyLength or keyCountFormat not mach the file!" << endl;
			fclose(tempInFile);
			return HDCP_PARAM_KEY_KEYLENHEADF_ERROR;
		}
	}else if (keyCountFormat == 2) {
		readKeyCount = readKeyCountFormat2(tempInFile, headLength);
		cout << "read key quantitiy (Format 2) = " << readKeyCount << endl;
		if (headLength + readKeyCount*keyLength != fileSize) {
			cout << "keyLength or keyCountFormat not mach the file!" << endl;
			fclose(tempInFile);
			return HDCP_PARAM_KEY_KEYLENHEADF_ERROR;
		}
	}else if (keyCountFormat == 3) {
		readKeyCount = readKeyCountFormat3(tempInFile, headLength);
		cout << "read key quantitiy (Format 3) = " << readKeyCount << endl;
		if (headLength + readKeyCount*keyLength != fileSize) {
			cout << "keyLength or keyCountFormat not mach the file!" << endl;
			fclose(tempInFile);
			return HDCP_PARAM_KEY_KEYLENHEADF_ERROR;
		}
	}else{
		fclose(tempInFile);
		cout << "keyCountFormat is wrong!" << endl;
		return HDCP_PARAM_KEY_HEADF_ERROR;
	}

	if (aimkeyCountFormat != 1 && aimkeyCountFormat !=2 && aimkeyCountFormat != 3) {
		cout << "aimkeyCountFormat is wrong!" << endl;
		fclose(tempInFile);
		return HDCP_PARAM_KEY_HEADF_ERROR;
	}

	//Do some recording.
	HDCPKeyCutTool::keyCount = readKeyCount;
	HDCPKeyCutTool::headLength = headLength;
	HDCPKeyCutTool::keyLength = keyLength;
	HDCPKeyCutTool::inHeadType = keyCountFormat;
	HDCPKeyCutTool::outHeadType = aimkeyCountFormat;

	fclose(tempInFile);
	cout << "Check Successful!" << endl;
	return WORK_FINE;
}

/*
Step 2.Check wether keys are enough or not.
param keyBeginNum : The number of first key to be seperated from file.
param keyCount : The count of keys to be seperated from file.
return : WORK_FINE: Can be seperated successfully. others: 1.param is unavailable
2.The keys in file are not enough
*/
int HDCPKeyCutTool::checkCommand(int keyBeginNum, int keyCount) {
	if (keyBeginNum <= 0 || keyCount <= 0) {
		cout << "keyBeginNum or keyCount is unavailable!" << endl;
		return HDCP_PARAM_KEY_RANGESET_ERROR;
	}
	int keyLastNum = keyBeginNum + keyCount - 1;
	if (keyLastNum > HDCPKeyCutTool::keyCount) {
		cout << "keys are not enough for you!" << endl;
		return HDCP_PARAM_KEY_RANGESET_ERROR;
	}
	return WORK_FINE;
}

/*
After Everything are checked and prepared.
Call this function to select the legal file path which will be operated soon.
param inFile: A legal path of src key file.
param outFile: A legal path of dest key file.
return: WORK_FINE: files opened successfully. others: 1.file opened failed.
*/
int HDCPKeyCutTool::setOperatedFiles(char * inFile, char * outFile) {
	int retval = WORK_FINE;
	retval = openFiles(inFile, outFile);
	if (retval != WORK_FINE) {
		cout << "Opening Files Error!" << endl;
		return HDCP_PARAM_FILE_OPEN_ERROR;
	}
	return WORK_FINE;
}

/*
After keys operating is finish. Call this function to close the files.
return void.
*/
void HDCPKeyCutTool::cleanOperatedFiles() {
	closeFiles();
}

/*
Call this function to cut keys.
param newKeyCount: The quantity of key that will cutted into target HDCP key file.
param newKeyLoc: The first index of key in src key file that begin to cut into dest key file.
return: WORK_FINE: work fine. others: error occur
*/
int HDCPKeyCutTool::cutKeys(int newKeyCount, int newKeyLoc) {
	int retval = WORK_FINE;
	if (newKeyLoc < 0 || newKeyLoc >= keyCount || newKeyCount <= 0 || (newKeyLoc + newKeyCount)>keyCount) {
		cout << "Target Key Range Error!" << endl;
		return HDCP_PARAM_KEY_RANGESET_ERROR;
	}

	if ( (retval = setLocationOfKey(newKeyLoc)) != WORK_FINE ) {
		cout << "Inner Error. setLocationOfKey" << endl;
		return HDCP_PARAM_KEY_KEYLOC_ERROR;
	}

	cout << "Cutting From Key Index " << newKeyLoc << endl;
	cout << "Cutting Key Count " << newKeyCount << endl;
	switch (outHeadType) {
	case 1:
		retval = setHDCPKeyCount1(newKeyCount);
		break;
	case 2:
		retval = setHDCPKeyCount2(newKeyCount);
		break;
	case 3:
		retval = setHDCPKeyCount3(newKeyCount);
		break;
	default:
		retval = setHDCPKeyCount1(newKeyCount);
		break;
	}
	if (retval != WORK_FINE) {
		return retval;
	}
	char * tempKey = new char[keyLength];
	for (int i = 0;i<newKeyCount;i++) {
		if (getOneKey(tempKey) != WORK_FINE) {
			cout << "Get Key From Source File Error!" << endl;
			delete[] tempKey;
			tempKey = NULL;
			return HDCP_INNER_KEY_OP_ERROR;
		}
		setOneKey(tempKey);
	}
	delete[] tempKey;
	tempKey = NULL;
	cout << "Cut Successfully!" << endl;
	return WORK_FINE;
}