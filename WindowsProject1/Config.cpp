#include "Config.h"


void Config::init() {
	std::ifstream inputFile(FileName);
	if (inputFile.is_open()) {
		inputFile >> WIDTH >> HEIGHT >> N >> gridColor >> backgroundColor;
		inputFile.close();
	}
	else {
		setDefaultValues();
	}
}

void Config::save(){
	std::ofstream outputFile(FileName);
	outputFile << WIDTH << " " << HEIGHT << " " << N << " " << gridColor << " " << backgroundColor << std::endl;
	outputFile.close();
}

void Config::initWinAPI() {
	DWORD bytesRead;
	std::wstring wFileName(FileName.begin(), FileName.end());
	HANDLE fileHandle = CreateFile(wFileName.c_str(), GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

	DWORD size = GetFileSize(fileHandle, NULL);
	char* data = new char[size];
	ReadFile(fileHandle, data, size, &bytesRead, NULL);

	if (fileHandle != INVALID_HANDLE_VALUE) {
		std::istringstream iss(data);

		iss >> WIDTH >> HEIGHT >> N >> gridColor >> backgroundColor;
		delete[] data;
		CloseHandle(fileHandle);
	}

}

void Config::saveWinAPI() {
	std::wstring wFileName(FileName.begin(), FileName.end());
	DWORD bytesWritten;
	HANDLE fileHandle = CreateFile(wFileName.c_str(), GENERIC_WRITE, 0, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
	if (fileHandle != INVALID_HANDLE_VALUE) {

		int bufferSize = snprintf(nullptr, 0, "%d %d %d %d %d", WIDTH, HEIGHT, N, gridColor, backgroundColor) + 1; // +1 для нулевого символа завершения строки
	// Выделяем динамический массив для хранения строки
		char* buffer = new char[bufferSize];
		snprintf(buffer, bufferSize, "%d %d %d %d %d", WIDTH, HEIGHT, N, gridColor, backgroundColor);

		WriteFile(fileHandle, buffer, bufferSize, &bytesWritten, NULL);
		delete[] buffer;
		CloseHandle(fileHandle);
	}
}

void Config::initFileVariable()
{
	FILE* fp_read;
	unsigned long gridColorLU;
	unsigned long backgroundColorLU;
	errno_t err = fopen_s(&fp_read, FileName.c_str(), "r");

	fscanf_s(fp_read, "%d %d %d %lu %lu\n", &WIDTH, &HEIGHT, &N, &gridColorLU, &backgroundColorLU);
	gridColor = (COLORREF)gridColorLU;
	backgroundColor = (COLORREF)backgroundColorLU;

	fclose(fp_read);
}

void Config::saveFileVariable()
{
	FILE* fp_write;
	unsigned long gridColorLU = static_cast<unsigned long>(gridColor);
	unsigned long backgroundColorLU = static_cast<unsigned long>(backgroundColor);
	errno_t err = fopen_s(&fp_write, FileName.c_str(), "w");

	fprintf(fp_write, "%d %d %d %lu %lu\n", WIDTH, HEIGHT, N, gridColorLU, backgroundColorLU);

	fclose(fp_write);
}

void Config::initMap() {

	HANDLE hFile = CreateFile(
		L"config.txt", GENERIC_READ, 0, NULL,
		OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

	DWORD fSize = GetFileSize(hFile, NULL);
	HANDLE hFileMap = CreateFileMapping(hFile, NULL, PAGE_READONLY, 0, fSize, NULL);

	if (hFile != INVALID_HANDLE_VALUE && hFileMap != NULL)
	{
		char* buffer = (char*)MapViewOfFile(hFileMap, FILE_MAP_READ, 0, 0, 0);
		std::istringstream iss(buffer);
		iss >> WIDTH >> HEIGHT >> N >> gridColor >> backgroundColor;
		UnmapViewOfFile(buffer);
	}

	CloseHandle(hFileMap);
	CloseHandle(hFile);
}

void Config::saveMap() {

	HANDLE hFile = CreateFile(
		L"config.txt", GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ, NULL,
		OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL
	);

	DWORD fSize = GetFileSize(hFile, NULL);
	HANDLE hFileMap = CreateFileMapping(
		hFile, NULL, PAGE_READWRITE, 0, fSize, NULL
	);

	LPVOID lpMapView = MapViewOfFile(
		hFileMap, FILE_MAP_READ | FILE_MAP_WRITE, 0, 0, 0
	);
	if (hFile != INVALID_HANDLE_VALUE && hFileMap != NULL)
	{
		int bufferSize = snprintf(nullptr, 0, "%d %d %d %d %d", WIDTH, HEIGHT, N, gridColor, backgroundColor) + 1;
		char* buffer = new char[bufferSize];
		snprintf(buffer, bufferSize, "%d %d %d %d %d", WIDTH, HEIGHT, N, gridColor, backgroundColor);

		char* destination = reinterpret_cast<char*>(lpMapView);
		memcpy(destination, buffer, bufferSize);

		DWORD bytesWritten;
		WriteFile(hFile, lpMapView, bufferSize, &bytesWritten, NULL);

		UnmapViewOfFile(lpMapView);
		delete[] buffer;
	}
	CloseHandle(hFileMap);
	CloseHandle(hFile);

}


void Config::setDefaultValues() {
	WIDTH = 320;
	HEIGHT = 240;
	N = 3;
	gridColor = RGB(255, 0, 0);
	backgroundColor = RGB(0, 0, 255);
}
