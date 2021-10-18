#pragma once

constexpr int LOG_ALL = 1;
constexpr int LOG_NOTICE = 2;
constexpr int LOG_CRITICAL = 3;

class CLogger {

public:

	CLogger(int logLevel = LOG_ALL);
	~CLogger();

	void Logging(const wchar_t* log, int logLevel);

private:

	int _logLevel;

	wchar_t* _fileName;

};