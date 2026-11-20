#define UNICODE
#define _UNICODE
#include <windows.h>
#include <shlobj.h>
#include <string>
#include <fstream>
#include <vector>
#include <algorithm>
#include <cstdint>

const char MAGIC[8] = {'P', 'K', 'G', 'B', 'N', 'D', 'L', '1'};

std::wstring GetLocalAppDataDir() {
    wchar_t path[MAX_PATH];
    if (SUCCEEDED(SHGetFolderPathW(NULL, CSIDL_LOCAL_APPDATA, NULL, 0, path))) {
        return std::wstring(path);
    }
    wchar_t tempPath[MAX_PATH];
    GetTempPathW(MAX_PATH, tempPath);
    return std::wstring(tempPath);
}

std::wstring GetTempDir() {
    wchar_t tempPath[MAX_PATH];
    GetTempPathW(MAX_PATH, tempPath);
    return std::wstring(tempPath);
}

bool FileExists(const std::wstring& path) {
    DWORD dwAttrib = GetFileAttributesW(path.c_str());
    return (dwAttrib != INVALID_FILE_ATTRIBUTES && !(dwAttrib & FILE_ATTRIBUTE_DIRECTORY));
}

bool DirectoryExists(const std::wstring& path) {
    DWORD dwAttrib = GetFileAttributesW(path.c_str());
    return (dwAttrib != INVALID_FILE_ATTRIBUTES && (dwAttrib & FILE_ATTRIBUTE_DIRECTORY));
}

bool CreateDirRecursive(const std::wstring& path) {
    if (DirectoryExists(path)) return true;
    size_t pos = path.find_last_of(L"\\/");
    if (pos != std::wstring::npos) {
        CreateDirRecursive(path.substr(0, pos));
    }
    return CreateDirectoryW(path.c_str(), NULL) || GetLastError() == ERROR_ALREADY_EXISTS;
}

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow) {
    wchar_t selfPath[MAX_PATH];
    GetModuleFileNameW(NULL, selfPath, MAX_PATH);

    HANDLE hSelf = CreateFileW(selfPath, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
    if (hSelf == INVALID_HANDLE_VALUE) {
        MessageBoxW(NULL, L"Ошибка открытия исполняемого файла.", L"HeterojunctionLab Launcher", MB_ICONERROR);
        return 1;
    }

    LARGE_INTEGER fileSize;
    GetFileSizeEx(hSelf, &fileSize);

    if (fileSize.QuadPart < 16) {
        CloseHandle(hSelf);
        MessageBoxW(NULL, L"Некорректный размер пакета.", L"HeterojunctionLab Launcher", MB_ICONERROR);
        return 1;
    }

    // Читаем последние 16 байт
    LARGE_INTEGER offset;
    offset.QuadPart = fileSize.QuadPart - 16;
    SetFilePointerEx(hSelf, offset, NULL, FILE_BEGIN);

    char footer[16];
    DWORD bytesRead = 0;
    ReadFile(hSelf, footer, 16, &bytesRead, NULL);

    if (memcmp(footer, MAGIC, 8) != 0) {
        CloseHandle(hSelf);
        MessageBoxW(NULL, L"Пакет поврежден или отсутствует встроенный архив.", L"HeterojunctionLab Launcher", MB_ICONERROR);
        return 1;
    }

    uint64_t zipSize = *reinterpret_cast<uint64_t*>(footer + 8);
    if (zipSize == 0 || (int64_t)zipSize > fileSize.QuadPart - 16) {
        CloseHandle(hSelf);
        MessageBoxW(NULL, L"Неверный размер встроенного архива.", L"HeterojunctionLab Launcher", MB_ICONERROR);
        return 1;
    }

    uint64_t zipOffset = fileSize.QuadPart - 16 - zipSize;

    std::wstring localAppData = GetLocalAppDataDir();
    std::wstring appDir = localAppData + L"\\HeterojunctionLab\\app";
    std::wstring targetExe = appDir + L"\\HeterojunctionLab.exe";
    std::wstring markerFile = appDir + L"\\.version";

    // Проверяем, распакована ли уже эта версия
    bool needExtract = true;
    if (FileExists(targetExe) && FileExists(markerFile)) {
        HANDLE hMarker = CreateFileW(markerFile.c_str(), GENERIC_READ, 0, NULL, OPEN_EXISTING, 0, NULL);
        if (hMarker != INVALID_HANDLE_VALUE) {
            uint64_t savedZipSize = 0;
            DWORD r = 0;
            ReadFile(hMarker, &savedZipSize, sizeof(savedZipSize), &r, NULL);
            CloseHandle(hMarker);
            if (r == sizeof(savedZipSize) && savedZipSize == zipSize) {
                needExtract = false;
            }
        }
    }

    if (needExtract) {
        CreateDirRecursive(appDir);

        // Извлекаем ZIP во временный файл
        std::wstring tempZip = GetTempDir() + L"\\hjl_bundle_" + std::to_wstring(GetCurrentProcessId()) + L".zip";
        HANDLE hTempZip = CreateFileW(tempZip.c_str(), GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
        if (hTempZip == INVALID_HANDLE_VALUE) {
            CloseHandle(hSelf);
            MessageBoxW(NULL, L"Не удалось создать временный файл архива.", L"HeterojunctionLab Launcher", MB_ICONERROR);
            return 1;
        }

        offset.QuadPart = zipOffset;
        SetFilePointerEx(hSelf, offset, NULL, FILE_BEGIN);

        const DWORD BUFFER_SIZE = 1024 * 1024; // 1 MB
        std::vector<char> buffer(BUFFER_SIZE);
        uint64_t remaining = zipSize;

        while (remaining > 0) {
            DWORD toRead = (DWORD)std::min((uint64_t)BUFFER_SIZE, remaining);
            DWORD actualRead = 0;
            if (!ReadFile(hSelf, buffer.data(), toRead, &actualRead, NULL) || actualRead == 0) {
                break;
            }
            DWORD actualWritten = 0;
            WriteFile(hTempZip, buffer.data(), actualRead, &actualWritten, NULL);
            remaining -= actualRead;
        }

        CloseHandle(hTempZip);
        CloseHandle(hSelf);

        // Распаковка через встроенный tar.exe или powershell
        std::wstring extractCmd = L"tar.exe -xf \"" + tempZip + L"\" -C \"" + appDir + L"\"";
        
        STARTUPINFOW si = { sizeof(si) };
        PROCESS_INFORMATION pi;
        si.dwFlags = STARTF_USESHOWWINDOW;
        si.wShowWindow = SW_HIDE;

        std::vector<wchar_t> cmdBuf(extractCmd.begin(), extractCmd.end());
        cmdBuf.push_back(0);

        BOOL ok = CreateProcessW(NULL, cmdBuf.data(), NULL, NULL, FALSE, CREATE_NO_WINDOW, NULL, NULL, &si, &pi);
        if (ok) {
            WaitForSingleObject(pi.hProcess, 30000); // До 30 секунд
            CloseHandle(pi.hProcess);
            CloseHandle(pi.hThread);
        } else {
            // Fallback: powershell Expand-Archive
            std::wstring psCmd = L"powershell -WindowStyle Hidden -Command \"Expand-Archive -Path '" + tempZip + L"' -DestinationPath '" + appDir + L"' -Force\"";
            std::vector<wchar_t> psBuf(psCmd.begin(), psCmd.end());
            psBuf.push_back(0);
            if (CreateProcessW(NULL, psBuf.data(), NULL, NULL, FALSE, CREATE_NO_WINDOW, NULL, NULL, &si, &pi)) {
                WaitForSingleObject(pi.hProcess, 60000);
                CloseHandle(pi.hProcess);
                CloseHandle(pi.hThread);
            }
        }

        // Удаляем временный zip
        DeleteFileW(tempZip.c_str());

        // Записываем маркер версии
        HANDLE hMarker = CreateFileW(markerFile.c_str(), GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
        if (hMarker != INVALID_HANDLE_VALUE) {
            DWORD w = 0;
            WriteFile(hMarker, &zipSize, sizeof(zipSize), &w, NULL);
            CloseHandle(hMarker);
        }
    } else {
        CloseHandle(hSelf);
    }

    if (!FileExists(targetExe)) {
        MessageBoxW(NULL, L"Не удалось найти исполняемый файл приложения после распаковки.", L"HeterojunctionLab Launcher", MB_ICONERROR);
        return 1;
    }

    // Запуск основного приложения HeterojunctionLab.exe
    STARTUPINFOW appSi = { sizeof(appSi) };
    PROCESS_INFORMATION appPi;

    std::wstring runCmd = L"\"" + targetExe + L"\" " + pCmdLine;
    std::vector<wchar_t> runBuf(runCmd.begin(), runCmd.end());
    runBuf.push_back(0);

    if (!CreateProcessW(targetExe.c_str(), runBuf.data(), NULL, NULL, FALSE, 0, NULL, appDir.c_str(), &appSi, &appPi)) {
        MessageBoxW(NULL, L"Ошибка запуска HeterojunctionLab.exe", L"HeterojunctionLab Launcher", MB_ICONERROR);
        return 1;
    }

    WaitForSingleObject(appPi.hProcess, INFINITE);

    DWORD exitCode = 0;
    GetExitCodeProcess(appPi.hProcess, &exitCode);

    CloseHandle(appPi.hProcess);
    CloseHandle(appPi.hThread);

    return (int)exitCode;
}
