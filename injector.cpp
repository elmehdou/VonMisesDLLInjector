#include "injector.h"
#include <QDebug>

bool Injector::Inject(const QString &dllPath, DWORD pid)
{
    if (pid){
        HANDLE handle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid);
        if (handle)
            return Inject(dllPath, handle);
    }

    return false;
}

bool Injector::Inject(const QString &dllPath, HANDLE handle)
{
    if (handle){
        void *allocPtr = VirtualAllocEx(handle, 0, dllPath.length() + 1, MEM_RESERVE | MEM_COMMIT, PAGE_EXECUTE_READWRITE);
        SIZE_T bytesWritten = 0;

        if (WriteProcessMemory(handle, allocPtr, dllPath.toUtf8().data(), dllPath.toUtf8().size(), &bytesWritten)){
            if (!bytesWritten) return false;
            HMODULE kernel32  = GetModuleHandleA("Kernel32.dll");
            if (kernel32){

                FARPROC loadLibraryPtr = GetProcAddress(kernel32, "LoadLibraryA");
                if (loadLibraryPtr){
                    HANDLE threadHandle = 0;
                    if ((threadHandle = CreateRemoteThread(handle, 0, 0, (LPTHREAD_START_ROUTINE)loadLibraryPtr, allocPtr, 0, 0))){
                        WaitForSingleObject(threadHandle, INFINITE);
                        VirtualFreeEx(handle, allocPtr, dllPath.length() + 1, MEM_RELEASE);
                        return true;
                    }
                } else {
                    qDebug() << "Could not find LoadLibraryA.";
                }
            } else {
                qDebug() << "Could not find Kernel32.";
            }
        } else {
            qDebug() << "Could not write to process memory.";
        }
    }

    return false;
}

Injector::Injector(QObject *parent) : QObject(parent)
{

}
