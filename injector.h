#ifndef INJECTOR_H
#define INJECTOR_H
#include <windows.h>

#include <QObject>


class Injector : public QObject
{
    Q_OBJECT
public:
    inline static Injector *getInstance(){
        if (!instance) instance = new Injector();
        return instance;
    }

    static bool Inject(const QString &dllPath, DWORD pid);
    static bool Inject(const QString &dllPath, HANDLE handle);

private:
    explicit Injector(QObject *parent = nullptr);

private:
    inline static Injector *instance = nullptr;

signals:

};

#endif // INJECTOR_H
