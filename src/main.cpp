#include "hooks/hooks.hpp"
#include "interfaces.hpp"

#include <iostream>
#include <thread>

#include <Windows.h>

unsigned long __stdcall on_attach(void* thread_parameter) {
#ifdef _DEBUG
    AllocConsole();

    FILE* file;

    freopen_s(&file, "CONOUT$", "w", stdout);
#endif

    do {
        if (!interfaces::initialize())
            break;

        if (!hooks::setup())
            break;

#ifdef _DEBUG
        std::cout << "Initialized!\n";
        std::cout << "Press [DELETE] to unload.\n";
#endif

        while (!(GetAsyncKeyState(VK_DELETE) & 0x1))
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
    } while (false);

    hooks::restore();

    std::this_thread::sleep_for(std::chrono::seconds(2));

#ifdef _DEBUG
    fclose(file);

    FreeConsole();
#endif

    FreeLibraryAndExitThread(static_cast<HMODULE>(thread_parameter), 0);
}

BOOL APIENTRY DllMain(const HMODULE module, const DWORD reason_for_call, const LPVOID reserved) {
    if (reason_for_call != DLL_PROCESS_ATTACH)
        return FALSE;

    if (const HANDLE thread = CreateThread(nullptr, 0, on_attach, module, 0, nullptr); thread != nullptr)
        CloseHandle(thread);

    return TRUE;
}