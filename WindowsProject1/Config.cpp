#include "Config.h"
Config::~Config() {};


std::string Config::getFileName() {
	return FileName;
}

int Config::getWidth() {
	return WIDTH;
}
int Config::getHeight() {
	return HEIGHT;
}

int Config::getN() {
	return N;
}

COLORREF Config::getGridColor() {
	return gridColor;
}

COLORREF Config::getBackgroundColor() {
	return backgroundColor;
}

void Config::setBackgroundColor(COLORREF color) {
	backgroundColor = color;
}
void Config::setGridColor(COLORREF color) {
	gridColor = color;
}
void Config::setN(int n) {
	N = n;
}
void Config::setWidth(int w) {
	WIDTH = w;
}
void Config::setHeight(int h) {
	HEIGHT = h;
}

void Stream::init() {
	int WIDTH;
	int HEIGHT;
	int N;
	COLORREF gridColor;
	COLORREF backgroundColor;
	std::ifstream inputFile(config->getFileName());
	if (inputFile.is_open()) {
		inputFile >> WIDTH >> HEIGHT >> N >> gridColor >> backgroundColor;
		config->setHeight(HEIGHT);
		config->setWidth(WIDTH);
		config->setN(N);
		config->setGridColor(gridColor);
		config->setBackgroundColor(backgroundColor);
		inputFile.close();
	}
	else {
		config->setDefaultValues();
	}
}

void Stream::save() {
	std::ofstream outputFile(config->getFileName());
	outputFile << config->getWidth() << " " << config->getHeight() << " " << config->getN() << " " << config->getGridColor() << " " << config->getBackgroundColor() << std::endl;
	outputFile.close();
}

std::shared_ptr<Config> Stream::getConfig() {
	return config;
}

void WinApi::init() {
	DWORD bytesRead;
	std::string FileName = config->getFileName();
	int WIDTH;
	int HEIGHT;
	int N;
	COLORREF gridColor;
	COLORREF backgroundColor;
	std::wstring wFileName(FileName.begin(), FileName.end());
	HANDLE fileHandle = CreateFile(wFileName.c_str(), GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

	DWORD size = GetFileSize(fileHandle, NULL);
	char* data = new char[size];
	ReadFile(fileHandle, data, size, &bytesRead, NULL);

	if (fileHandle != INVALID_HANDLE_VALUE) {
		std::istringstream iss(data);

		iss >> WIDTH >> HEIGHT >> N >> gridColor >> backgroundColor;
		config->setHeight(HEIGHT);
		config->setWidth(WIDTH);
		config->setN(N);
		config->setGridColor(gridColor);
		config->setBackgroundColor(backgroundColor);
		delete[] data;
		CloseHandle(fileHandle);
	}
	else {
		config->setDefaultValues();
	}
}

void WinApi::save() {
	std::string FileName = config->getFileName();
	std::wstring wFileName(FileName.begin(), FileName.end());
	DWORD bytesWritten;
	HANDLE fileHandle = CreateFile(wFileName.c_str(), GENERIC_WRITE, 0, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
	if (fileHandle != INVALID_HANDLE_VALUE) {

		int bufferSize = snprintf(nullptr, 0, "%d %d %d %d %d", config->getWidth(), config->getHeight(), config->getN(), config->getGridColor(), config->getBackgroundColor()) + 1; // +1 для нулевого символа завершения строки
	//// Выделяем динамический массив для хранения строки
		char* buffer = new char[bufferSize];
		snprintf(buffer, bufferSize, "%d %d %d %d %d", config->getWidth(), config->getHeight(), config->getN(), config->getGridColor(), config->getBackgroundColor());

		WriteFile(fileHandle, buffer, bufferSize, &bytesWritten, NULL);
		delete[] buffer;
		CloseHandle(fileHandle);
	}
}

std::shared_ptr<Config> WinApi::getConfig() {
	return config;
}

void FileVariable::init()
{
	FILE* fp_read;
	unsigned long gridColorLU;
	unsigned long backgroundColorLU;
	std::string FileName = config->getFileName();
	int WIDTH;
	int HEIGHT;
	int N;
	errno_t err = fopen_s(&fp_read, FileName.c_str(), "r");
	if (err == 0) {
		fscanf_s(fp_read, "%d %d %d %lu %lu\n", &WIDTH, &HEIGHT, &N, &gridColorLU, &backgroundColorLU);
		config->setHeight(HEIGHT);
		config->setWidth(WIDTH);
		config->setN(N);
		config->setGridColor((COLORREF)gridColorLU);
		config->setBackgroundColor((COLORREF)backgroundColorLU);

		fclose(fp_read);
	}
	else {
		config->setDefaultValues();
	}
}

void FileVariable::save()
{
	FILE* fp_write;
	std::string FileName = config->getFileName();
	unsigned long gridColorLU = static_cast<unsigned long>(config->getGridColor());
	unsigned long backgroundColorLU = static_cast<unsigned long>(config->getBackgroundColor());
	errno_t err = fopen_s(&fp_write, FileName.c_str(), "w");

	fprintf(fp_write, "%d %d %d %lu %lu\n", config->getWidth(), config->getHeight(), config->getN(), gridColorLU, backgroundColorLU);

	fclose(fp_write);
}

std::shared_ptr<Config> FileVariable::getConfig() {
	return config;
}

void Map::init() {
	int WIDTH;
	int HEIGHT;
	int N;
	COLORREF gridColor;
	COLORREF backgroundColor;
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
		config->setHeight(HEIGHT);
		config->setWidth(WIDTH);
		config->setN(N);
		config->setGridColor(gridColor);
		config->setBackgroundColor(backgroundColor);
		UnmapViewOfFile(buffer);
	}
	else {
		config->setDefaultValues();
	}
	CloseHandle(hFileMap);
	CloseHandle(hFile);
}

void Map::save() {

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
		int bufferSize = snprintf(nullptr, 0, "%d %d %d %d %d", config->getWidth(), config->getHeight(), config->getN(), config->getGridColor(), config->getBackgroundColor()) + 1;
		char* buffer = new char[bufferSize];
		snprintf(buffer, bufferSize, "%d %d %d %d %d", config->getWidth(), config->getHeight(), config->getN(), config->getGridColor(), config->getBackgroundColor());

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

std::shared_ptr<Config> Map::getConfig() {
	return config;
}

void Config::setDefaultValues() {
	WIDTH = 320;
	HEIGHT = 240;
	N = 3;
	gridColor = RGB(255, 0, 0);
	backgroundColor = RGB(0, 0, 255);
}

template<typename T, typename... Args>
void ConfigManager::configure(Args&&... args) {
	config = std::make_unique<T>(std::forward<Args>(args)...);
}

void ConfigManager::init() {
	config->init();
}

void ConfigManager::save() {
	config->save();
}

std::shared_ptr<Config> ConfigManager::getConfig() {
	return config->getConfig();
}

void Factory::setConfigInit(HWND hWnd, const char* arg, ConfigManager& configManager) {
	std::shared_ptr<Config> config = std::make_shared<Config>();
	if (__argc >= 2)
	{
		switch (arg[0]) {
		case '1':
			configManager.configure<Map>(config);
			MessageBox(hWnd, _T("Map"), _T("Map"), MB_OK | MB_ICONQUESTION);
			break;
		case '2':
			configManager.configure<FileVariable>(config);
			//MessageBox(hWnd, _T("FileVariable"), _T("FileVariable"), MB_OK | MB_ICONQUESTION);
			break;
		case '3':
			configManager.configure<Stream>(config);
			//MessageBox(hWnd, _T("Stream"), _T("Stream"), MB_OK | MB_ICONQUESTION);
			break;
		case '4':
			configManager.configure<WinApi>(config);
			//MessageBox(hWnd, _T("WinAPI"), _T("WinAPI"), MB_OK | MB_ICONQUESTION);
			break;
		default:
			configManager.configure<Stream>(config);
			//MessageBox(hWnd, _T("Stream"), _T("Stream"), MB_OK | MB_ICONQUESTION);
			break;
		}
	}
	else {
		configManager.configure<Stream>(config);
		MessageBox(hWnd, _T("Stream"), _T("Stream"), MB_OK | MB_ICONQUESTION);
	}
}