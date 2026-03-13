// 最小化包含头文件
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <tlhelp32.h>

// 目标进程名称
#define TARGET_PROCESS_NAME "StudentMain.exe"

/**
 * @brief 查找目标进程ID
 * @param szProcessName 进程名称
 * @return 成功返回进程ID，失败返回0
 */
DWORD FindProcessId(const char* szProcessName) {
    DWORD dwPID = 0;
    HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (hSnapshot != INVALID_HANDLE_VALUE) {
        PROCESSENTRY32 pe32;
        pe32.dwSize = sizeof(PROCESSENTRY32);
        if (Process32First(hSnapshot, &pe32)) {
            do {
                if (strcmp(pe32.szExeFile, szProcessName) == 0) {
                    dwPID = pe32.th32ProcessID;
                    break;
                }
            } while (Process32Next(hSnapshot, &pe32));
        }
        CloseHandle(hSnapshot);
    }
    return dwPID;
}

/**
 * @brief 暴力写内存（进程虚拟地址空间擦除）
 * @param dwPID 目标进程ID
 * @return 成功返回TRUE，失败返回FALSE
 */
BOOL KillByMemoryWrite(DWORD dwPID) {
    // 打开进程（需PROCESS_VM_WRITE | PROCESS_VM_OPERATION权限）
    HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, dwPID);
    if (!hProcess) return FALSE;

    // 核心逻辑：遍历常见用户态内存区域，按页写入0，擦除地址空间
    const SIZE_T dwPageSize = 4096;
    BYTE lpZeroBuffer[dwPageSize] = {0};
    BOOL bEraseSuccess = FALSE;

    // 遍历内存地址范围（0x10000 ~ 0x7FFFFFFF，避开系统核心区）
    for (ULONG_PTR lpAddr = 0x10000; lpAddr < 0x7FFFFFFF; lpAddr += dwPageSize) {
        SIZE_T dwBytesWritten = 0;
        // 尝试写入目标进程内存（部分地址无权限，忽略即可）
        if (WriteProcessMemory(hProcess, (LPVOID)lpAddr, lpZeroBuffer, dwPageSize, &dwBytesWritten)) {
            bEraseSuccess = TRUE;
        }
    }

    // 释放资源
    CloseHandle(hProcess);
    return bEraseSuccess;
}

// 主函数
int main() {
    // 查找StudentMain.exe进程
    DWORD dwPID = FindProcessId(TARGET_PROCESS_NAME);
    if (dwPID == 0) {
        return 1; // 进程未找到，直接退出
    }

    // 执行暴力写内存
    KillByMemoryWrite(dwPID);
    return 0;
}