#ifndef HDCP_KEY_CUT_TOOL_H
#define HDCP_KEY_CUT_TOOL_H
#include <cstdio>
#include <iostream>
class HDCPKeyCutTool {
public:
	~HDCPKeyCutTool();
	int checkKeyFormat(char * inFile, int headLength, int keyLength, int keyCountFormat, int aimkeyCountFormat);
	int checkCommand(int keyBeginNum, int keyCount);
	int setOperatedFiles(char * inFile, char* outFile);
	int cutKeys(int newKeyCount, int newKeyLoc);
	void cleanOperatedFiles();
private:
	FILE * InFile, *OutFile;
	int keyCount = 0 ;//key count of input file
	int inHeadType = 1;//head type of the input file
	int outHeadType = 1;//head type of the ouput files
	int headLength = 4;//head length of key file
	int keyLength = 308;// length of a single key

	int getHDCPKeyCount1();
	int getHDCPKeyCount2();
	int getHDCPKeyCount3();

	void setHDCPKeyCountHelp1(int sum, int* mm);
	void setHDCPKeyCountHelp2(int sum, int* mm);
	void setHDCPKeyCountHelp3(int sum, int* mm);

	int setHDCPKeyCount1(int newKeyCount);
	int setHDCPKeyCount2(int newKeyCount);
	int setHDCPKeyCount3(int newKeyCount);

	int readKeyCountFormat1(FILE * InFile, int headLength);

	int openFiles(char * inFile, char* outFile);
	void closeFiles();

	int getOneKey(char* aKey);
	int setOneKey(char* aKey);

	int getLocationOfKey();
	int setLocationOfKey(int loc);
};
#endif