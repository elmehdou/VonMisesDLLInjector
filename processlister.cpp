#include "processlister.h"

ProcessLister::ProcessLister(QObject *parent) : QObject(parent)
  , timer(this)
{
//    QObject::connect(&timer, &QTimer::timeout, this, &ProcessLister::onUpdateProcessSnashots);
//    timer.start(500);
}

QMap<QString, PROCESSENTRY32W> ProcessLister::getProcessEntries() const
{
    return processEntries;
}

void ProcessLister::onUpdateProcessSnashots()
{
    processEntries.clear();
    HANDLE snapshotHandle = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

    if (snapshotHandle){
        PROCESSENTRY32 processEntry;
        processEntry.dwSize = sizeof (PROCESSENTRY32);
        if (Process32First(snapshotHandle, &processEntry)){
            do {
                QString processName = QString::fromWCharArray(processEntry.szExeFile);
                PROCESSENTRY32 existingEntry = processEntries.value(processName);
                if (existingEntry.dwSize)
                    if (existingEntry.th32ProcessID < processEntry.th32ProcessID)
                        continue;
                processEntries.insert(processName, processEntry);
            } while (Process32Next(snapshotHandle, &processEntry));
        } else {

            qDebug() << "No first process found.";
        }
    } else {
        qDebug() << "Could not take process snapshot.";
    }

    CloseHandle(snapshotHandle);

    emit processEntriesUpdated();
}
