#include <windows.h>
#include <iostream>

using namespace std;

HANDLE port;

bool writeData(unsigned long numBytes, char* buffer)
{
	unsigned long  numberOfBytesWritten;
	WriteFile(port, buffer, numBytes,&numberOfBytesWritten, NULL);

	return 1;
}

bool readData(unsigned long* numberOfBytesRead, unsigned long bufferSize, char* buffer)
{
	unsigned long errors, numberOfBytesToRead;
	COMSTAT comstat;

	ClearCommError(port, &errors, &comstat);
	
	if (comstat.cbInQue > 0) {
		if (ReadFile(port, buffer, bufferSize,numberOfBytesRead, NULL) == false)
			cout << "read error" << endl;
	}
	else
		*numberOfBytesRead = 0;

	return 1;
}

void printData(char* buffer, unsigned long size) {
	for (unsigned long i = 0; i < size; i++)
	{
		cout << buffer[i];
	}
}

char* readFile(char* filename, unsigned long& size) {
	HANDLE file = CreateFileA(filename, GENERIC_READ, FILE_SHARE_READ, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
	if (port == INVALID_HANDLE_VALUE) {
		cout << "Error open file" << endl;
		exit(1);
	}
	size = (unsigned long)GetFileSize(file, 0);
	HANDLE file_mapping = CreateFileMappingA(file, 0, PAGE_WRITECOPY, 0, 0, 0);
	char* buffer = (char*)MapViewOfFileEx(file_mapping, FILE_MAP_COPY, 0, 0, 0, 0);

	return buffer;
}

int main() {
	port = CreateFile(L"COM5", GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, NULL);
	if (port == INVALID_HANDLE_VALUE) {
		cout << "Error create file" << endl;
		exit(1);
	}

	DCB dcb;
	dcb.DCBlength = sizeof(dcb);
	GetCommState(port, &dcb);

	dcb.DCBlength = sizeof(dcb);
	dcb.BaudRate = CBR_1200;
	dcb.Parity = ODDPARITY;
	dcb.StopBits = ONESTOPBIT;
	dcb.ByteSize = 7;
	dcb.fParity = TRUE;

	SetCommState(port, &dcb);

	unsigned long bufSize;
	char* bufferOut = readFile((char*)"art.txt", bufSize);
	char* bufferIn = new char[strlen(bufferOut)];

	if(writeData(strlen(bufferOut), bufferOut) == false) {
		cout << "write error" << endl;
	}

	unsigned long bytesRead;
	if (readData(&bytesRead, strlen(bufferOut), bufferIn)) {

		printData(bufferIn, bytesRead);
		cout << endl;
	}

	return 0;
}