﻿#include <iostream>
#include <ShlObj_core.h>
#include <tchar.h>
#include <elzip.hpp>

#pragma comment(lib, "Urlmon.lib")

bool DirectoryContainsFile(const TCHAR* directoryPath)
{
    WIN32_FIND_DATA findData;
    std::string narrowPath(directoryPath, directoryPath + _tcslen(directoryPath));
    HANDLE hFind = FindFirstFileA((narrowPath + "\\3Dash.exe").c_str(), &findData);

    if (hFind != INVALID_HANDLE_VALUE)
    {
        FindClose(hFind);
        return true;
    }

    return false;
}

void RemoveMelonLoader(std::filesystem::path path) {
    try {
        std::filesystem::remove_all(path / std::string("MelonLoader"));
        std::filesystem::remove_all(path / std::string("Mods"));
        std::filesystem::remove_all(path / std::string("Plugins"));
        std::filesystem::remove_all(path / std::string("UserData"));
        std::filesystem::remove_all(path / std::string("UserLibs"));
        std::filesystem::remove(path / std::string("dobby.dll"));
        std::filesystem::remove(path / std::string("NOTICE.txt"));
        std::filesystem::remove(path / std::string("version.dll"));

        std::cout << ", Done\n";
    }
    catch (const std::filesystem::filesystem_error& ex) {
        std::cerr << "Error removing MelonLoader: " << ex.what() << std::endl;
    }
}

void RemoveBepInEx(std::filesystem::path path) {
    try {
        std::filesystem::remove_all(path / std::string("BepInEx"));
        std::filesystem::remove(path / std::string("changelog.txt"));
        std::filesystem::remove(path / std::string("doorstop_config.ini"));
        std::filesystem::remove(path / std::string("winhttp.dll"));
    }
    catch (const std::filesystem::filesystem_error& ex) {
        std::cerr << "Error removing BepInEx: " << ex.what() << std::endl;
    }
}

int main()
{
    HWND console = GetConsoleWindow();

    std::wstring version = L"1.0.2";
    std::wstring title = L"3MM Installer v" + version;

    SetWindowTextW(console, title.c_str());

    int input;
    do {
        std::cout << "1) Install/Update 3DashModMenu\n";
        std::cout << "2) Uninstall 3DashModMenu and BepInEx\n\n";

        std::cout << "Select an option: ";
        std::cin >> input;

        if (input == 1) {
            bool folder_found = false;
            TCHAR path[MAX_PATH];
            do
            {
                BROWSEINFO bi = { 0 };
                bi.lpszTitle = "Select 3Dash folder";
                LPITEMIDLIST pidl = SHBrowseForFolder(&bi);

                if (pidl != NULL)
                {
                    if (SHGetPathFromIDList(pidl, path))
                    {
                        if (DirectoryContainsFile(path))
                        {
                            CoTaskMemFree(pidl);
                            folder_found = true;
                            break;
                        }
                    }
                    CoTaskMemFree(pidl);
                }
                else
                {
                    break;
                }
                MessageBox(NULL, "This directory does not contain 3Dash.exe", "3Dash not found", MB_OK);
            } while (true);

            system("cls");
            if (folder_found)
            {
                system("cls");

                std::filesystem::path BepInEx_path = std::filesystem::path(path) / std::string("BepInEx");
                std::filesystem::path MelonLoader_path = std::filesystem::path(path) / std::string("MelonLoader");
                std::filesystem::path BepInEx_plugins = BepInEx_path / std::string("plugins");

                // Delete MelonLoader if found
                if (std::filesystem::is_directory(MelonLoader_path)) {
                    std::cout << "Removing MelonLoader";

                    RemoveMelonLoader(std::filesystem::path(path));
                }

                // Install BepInEx
                if (!std::filesystem::is_directory(BepInEx_path)) {
                    std::cout << "Downloading BepInEx";

                    const char* DownloadUrl = "https://github.com/BepInEx/BepInEx/releases/download/v5.4.22/BepInEx_x64_5.4.22.0.zip";
                    const char* BepinexFile = "BepInEx_x64_5.4.22.0.zip";

                    HRESULT result = URLDownloadToFileA(NULL, DownloadUrl, BepinexFile, 0, NULL);
                    if (result != S_OK) {
                        std::cout << "Error downloading BepInEx!\n";
                    }
                    std::cout << ", Done\n";

                    std::cout << "Extracting BepInEx";

                    elz::extractZip(BepinexFile, path, "");
                    std::remove(BepinexFile);

                    std::cout << ", Done\n";

                    std::cout << "Creating plugins directory";
                    std::filesystem::create_directory(BepInEx_plugins);

                    std::cout << ", Done\n";
                }

                // Install 3DashModMenu
                if (std::filesystem::is_directory(BepInEx_plugins)) {
                    std::cout << "Downloading 3DashModMenu";

                    const char* url = "https://github.com/Wolf11221/3DashModMenu/releases/latest/download/3DashModMenu.dll";
                    const char* name = "3DashModMenu.dll";
                    std::filesystem::path sourceFilePath(name);
                    std::filesystem::path destinationFilePath = BepInEx_plugins / sourceFilePath.filename();

                    HRESULT result = URLDownloadToFileA(NULL, url, destinationFilePath.string().c_str(), 0, NULL);
                    if (result != S_OK) {
                        std::cout << "Error downloading 3MM!\n";
                    }

                    std::cout << ", Done!\n\n";
                }
                //std::cout << "Done!\n\n";
            }
        }
        else if (input == 2) {
            TCHAR path[MAX_PATH];
            do
            {
                BROWSEINFO bi = { 0 };
                bi.lpszTitle = "Select 3Dash folder";
                LPITEMIDLIST pidl = SHBrowseForFolder(&bi);

                if (pidl != NULL)
                {
                    if (SHGetPathFromIDList(pidl, path))
                    {
                        if (DirectoryContainsFile(path))
                        {
                            CoTaskMemFree(pidl);
                            break;
                        }
                    }
                    CoTaskMemFree(pidl);
                }
                else
                {
                    break;
                }
                MessageBox(NULL, "The directory does not contain 3Dash.exe", "3Dash not found", MB_OK);
            } while (true);

            system("cls");

            std::cout << "Removing files";

            RemoveBepInEx(path);

            std::cout << ", Done!\n\n";
        }
    } while (input != 0);

    return 0;
}
