#include <stdio.h>
#include <Windows.h>
#include <time.h>
#include <string.h>

#include "log.h"

CLogger::CLogger(int logLevel) {

	_logLevel = logLevel;

	constexpr int fileNameLen = 30;

	_fileName = new wchar_t[fileNameLen];
	
	ZeroMemory(_fileName, sizeof(wchar_t) * fileNameLen);

	__time64_t nowTime;
	time(&nowTime);

	tm nowTm;
	_localtime64_s(&nowTm, &nowTime);

	swprintf_s(_fileName, fileNameLen, L"log_%d%02d%02d_%02d%02d%02d.txt", 1900 + nowTm.tm_year, nowTm.tm_mon, nowTm.tm_mday, nowTm.tm_hour, nowTm.tm_min, nowTm.tm_sec);

	FILE* file;
	_wfopen_s(&file, _fileName, L"w");

	fclose(file);

}

CLogger::~CLogger() {
	delete[] _fileName;
}

void CLogger::Logging(const wchar_t* log, int logLevel) {

	if (logLevel < _logLevel) {
		return;
	}

	FILE* file;
	_wfopen_s(&file, _fileName, L"a");
	int logLen = wcslen(log);
	fwrite(log, sizeof(wchar_t), logLen, file);
	fwrite(L"\n", 1, 1, file);
	fclose(file);
}