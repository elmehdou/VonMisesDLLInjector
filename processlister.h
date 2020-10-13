#ifndef PROCESSLISTER_H
#define PROCESSLISTER_H

#include <windows.h>
#include <tlhelp32.h>
#include <psapi.h>

#include <QObject>
#include <QDebug>
#include <QTimer>

class ProcessLister : public QObject
{
    Q_OBJECT
public:
    inline static ProcessLister *getInstance(){
        if (!instance) instance = new ProcessLister();
        return instance;
    }

    QMap<QString, PROCESSENTRY32W> getProcessEntries() const;

private:
    explicit ProcessLister(QObject *parent = nullptr);

private:
    inline static ProcessLister *instance = nullptr;
    QMap<QString,PROCESSENTRY32> processEntries;
    QTimer timer;

public slots:
    void onUpdateProcessSnashots();

signals:
    void processEntriesUpdated();
};

#endif // PROCESSLISTER_H
