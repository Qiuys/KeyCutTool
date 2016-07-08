#include "KeyToolManager.h"

int main() {
	char * inputFile = "C:/Users/sorou/Desktop/testfolder/HDCP_KEY_10_1-10.bin";
	char * outputFolder = "C:/Users/sorou/Desktop/testfolder/output";
	char * newFileName = "HDCP_LIENCE";
	int keyType = 1; //HDCP
	int headLength = 4;
	int keyLength = 308;
	int keyCountFormat = 1;
	int aimkeyCountFormat = 1;
	int KeyBeginNum = 1;
	int KeyEachFile = 3;
	int KeyFileCount = 3;

	KeyToolManager ktm;
	ktm.setInFile(inputFile);
	ktm.setOutFile(outputFolder, newFileName);
	ktm.setKeyType(keyType);
	ktm.setHDCPKeyFormat(headLength, keyLength, keyCountFormat, aimkeyCountFormat);
	ktm.setHDCPKeyCutParam(KeyBeginNum, KeyEachFile, KeyFileCount);
	ktm.startCut();

	return 0;
}