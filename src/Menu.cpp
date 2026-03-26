#include "Screen.h"

std::string Menu::openFileDialog() {
    std::string filePath = "";

    // 1. Initialize COM (Required for Windows Dialogs)
    HRESULT hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);
    if (FAILED(hr)) return "";

    IFileOpenDialog* pFileOpen;

    // 2. Create the FileOpenDialog object
    hr = CoCreateInstance(CLSID_FileOpenDialog, NULL, CLSCTX_ALL,
        IID_IFileOpenDialog, reinterpret_cast<void**>(&pFileOpen));

    if (SUCCEEDED(hr)) {
        // 3. Set file filters (only show .ch8 or .bin files)
        COMDLG_FILTERSPEC fileTypes[] = {
            { L"Chip-8 ROMs", L"*.ch8;*.bin;*.rom" },
            { L"All Files",    L"*.*" }
        };
        pFileOpen->SetFileTypes(2, fileTypes);

        // 4. Show the Dialog
        hr = pFileOpen->Show(NULL);

        if (SUCCEEDED(hr)) {
            IShellItem* pItem;
            hr = pFileOpen->GetResult(&pItem);
            if (SUCCEEDED(hr)) {
                PWSTR pszFilePath;
                hr = pItem->GetDisplayName(SIGDN_FILESYSPATH, &pszFilePath);

                if (SUCCEEDED(hr)) {
                    // Convert Wide string (Windows) to standard string
                    std::wstring ws(pszFilePath);
                    filePath = std::string(ws.begin(), ws.end());
                    CoTaskMemFree(pszFilePath);
                }
                pItem->Release();
            }
        }
        pFileOpen->Release();
    }
    CoUninitialize();
    return filePath;
};