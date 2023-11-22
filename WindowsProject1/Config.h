#pragma once

#include <windows.h>
#include <fstream>
#include <iostream>
#include <sstream>
#include <tchar.h>

class Config {

private:
	std::string FileName;
	int WIDTH;
	int HEIGHT;
	int N;
	COLORREF gridColor;
	COLORREF backgroundColor;

public:
	Config(std::string FileName = "config.txt", int width = 320, int height = 240, int n = 3,
		COLORREF gridColor = RGB(255, 0, 0), COLORREF backgroundColor = RGB(0, 0, 255))
		:FileName(FileName), WIDTH(width), HEIGHT(height), N(n), gridColor(gridColor), backgroundColor(backgroundColor) {
	}
	~Config();
	std::string getFileName();
	int getWidth();
	int getHeight();
	int getN();
	COLORREF getGridColor();
	COLORREF getBackgroundColor();

	void setWidth(int w);
	void setHeight(int h);
	void setBackgroundColor(COLORREF color);
	void setGridColor(COLORREF color);
	void setN(int n);

	void setDefaultValues();
};

class IConfigurable {
public:
	virtual void init() = 0;
	virtual void save() = 0;
	virtual std::shared_ptr<Config> getConfig() = 0;
};

class WinApi : public IConfigurable {
private:
	std::shared_ptr<Config> config;
public:
	WinApi(std::shared_ptr<Config> config) : config(config) {}

	void init() override;
	void save() override;
	std::shared_ptr<Config> getConfig() override;
};

class Stream : public IConfigurable {
private:
	std::shared_ptr<Config> config;
public:
	Stream(std::shared_ptr<Config> config) : config(config) {}

	void init() override;
	void save() override;
	std::shared_ptr<Config> getConfig() override;
};

class FileVariable : public IConfigurable {
private:
	std::shared_ptr<Config> config;
public:
	FileVariable(std::shared_ptr<Config> config) : config(config) {}

	void init() override;
	void save() override;
	std::shared_ptr<Config> getConfig() override;
};

class Map : public IConfigurable {
private:
	std::shared_ptr<Config> config;
public:
	Map(std::shared_ptr<Config> config) : config(config) {}

	void init() override;
	void save() override;
	std::shared_ptr<Config> getConfig() override;
};

class ConfigManager {
private:
	std::unique_ptr<IConfigurable> config;

public:
	template<typename T, typename... Args>
	void configure(Args&&... args);

	void init();
	void save();
	std::shared_ptr<Config> getConfig();
};

class Factory {
public:
	static void setConfigInit(HWND hWnd, const char* arg, ConfigManager& configManager);
};