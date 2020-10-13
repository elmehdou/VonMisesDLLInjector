#include "mainwindow.h"
#include "processlister.h"
#include "ui_mainwindow.h"
#include "injector.h"

#include <QFileDialog>
#include <QMessageBox>
#include <QListWidgetItem>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    initializeConnections();

    emit ui->updateProcessListBtn->clicked();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::initializeConnections()
{
    QObject::connect(ui->updateProcessListBtn, &QPushButton::clicked, ProcessLister::getInstance(), &ProcessLister::onUpdateProcessSnashots);
    QObject::connect(ProcessLister::getInstance(), &ProcessLister::processEntriesUpdated, this, &MainWindow::onProcessEntriesUpdated);
    QObject::connect(ui->injectDllBtn, &QPushButton::clicked, this, &MainWindow::onInjectDLL);
}

void MainWindow::onProcessEntriesUpdated()
{
    QString processName;
    QList<QListWidgetItem *> selecteditems = ui->processListWidget->selectedItems();
    if (selecteditems.count())
        processName = selecteditems.at(0)->text();

    ui->processListWidget->clear();
    QMap<QString, PROCESSENTRY32> processEntriesMap = ProcessLister::getInstance()->getProcessEntries();
    ui->processListWidget->insertItems(0, processEntriesMap.keys());

    if (!processName.isEmpty()){
        QList<QListWidgetItem *> foundItems = ui->processListWidget->findItems(processName, Qt::MatchFixedString);
        if (foundItems.count())
            ui->processListWidget->setCurrentItem(foundItems.at(0));
    }
}

void MainWindow::onInjectDLL()
{
    QList<QListWidgetItem *> selecteditems = ui->processListWidget->selectedItems();
    if (!selecteditems.count()) {
        QMessageBox::warning(this, "No selected process", "A process has to be selected to inject a DLL.");
        return;
    }
    QString processName = selecteditems.at(0)->text();

    QString dllPath = QFileDialog::getOpenFileName(this, "Select a Dynamic Library", QString(), "Dynamic Library (*.dll)");
    if (dllPath.isEmpty()){
        return;
    } else {
        QMessageBox::StandardButton response = QMessageBox::question(this, "Are you sure ?", QString("The dll file '%1' is about to be injected in the process '%2'. Are you sure you want to proceed ?").arg(dllPath.split('/').last(), processName));
        if (response != QMessageBox::Yes)
            return;
    }

    QString dllBaseName = dllPath.split('/').last();
    PROCESSENTRY32 processEntry = ProcessLister::getInstance()->getProcessEntries().value(processName);
    if (!processEntry.dwSize){
        QMessageBox::warning(this, "Injection failure !", QString("Could not find process '%1'").arg(processName));
        return;
    }

    DWORD pid = processEntry.th32ProcessID;

    if (Injector::Inject(dllPath, pid))
        QMessageBox::information(this, "Injection success !", QString("'%1' was successfully injected with '%2'.").arg(processName, dllBaseName));
    else
        QMessageBox::warning(this, "Injection failure !", QString("Failed to inject '%1' into '%2'.\nError code %3.").arg(dllBaseName, processName, QString::number(GetLastError())));
}

