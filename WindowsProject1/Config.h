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
	std::shared_ptr<Config> getConfig() override {
		return config;
	}
};

class Stream : public IConfigurable {
private:
	std::shared_ptr<Config> config;
public:
	Stream(std::shared_ptr<Config> config) : config(config) {}

	void init() override;
	void save() override;
	std::shared_ptr<Config> getConfig() override {
		return config;
	}
};

class FileVariable : public IConfigurable {
private:
	std::shared_ptr<Config> config;
public:
	FileVariable(std::shared_ptr<Config> config) : config(config) {}

	void init() override;
	void save() override;
	std::shared_ptr<Config> getConfig() override {
		return config;
	}
};

class Map : public IConfigurable {
private:
	std::shared_ptr<Config> config;
public:
	Map(std::shared_ptr<Config> config) : config(config) {}

	void init() override;
	void save() override;
	std::shared_ptr<Config> getConfig() override {
		return config;
	}
};

class ConfigManager {
private:
	std::unique_ptr<IConfigurable> config;

public:
	template<typename T, typename... Args>
	void configure(Args&&... args) {
		config = std::make_unique<T>(std::forward<Args>(args)...);
	}

	void init() {
		config->init();
	}

	void save() {
		config->save();
	}

	std::shared_ptr<Config> getConfig() {
		return config->getConfig();
	}
};

class Factory {
public:
	static void setConfigInit(HWND hWnd, const char* arg, ConfigManager& configManager) {
		std::shared_ptr<Config> config = std::make_shared<Config>();
		if (__argc >= 3)
		{
			switch (arg[0]) {
			case '1':
				configManager.configure<Map>(config);
				//MessageBox(hWnd, _T("Map"), _T("Map"), MB_OK | MB_ICONQUESTION);
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
		else configManager.configure<Stream>(config);
	}
};