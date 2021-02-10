#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSerialPort>
#include <xinput.h>
#include <windows.h>
#include <QBluetoothDeviceDiscoveryAgent>
#include <QBluetoothDeviceDiscoveryAgent>
#include <QBluetoothSocket>


//#pragma comment(lib, "XInput.lib")

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_pushButtonSerach_clicked();


    void on_pushButtonConnect_clicked();

    void on_pushButtonClose_clicked();
    void readFromPort();
    void sendMessageToDevice(QString message);

    void on_pushButton_clicked();
    void addToLogs(QString message);
    void on_pushButton_2_clicked();
    void DisplayTrigger();

    /*void captureDeviceProperties(const QBluetoothDeviceInfo &device);
    void searchingFinished();
    void connectionEstablished();
    void connectionInterrupted();
    void socketReadyToRead();*/

signals:

     void valueChanged( int value );

private:
    Ui::MainWindow *ui;
    QSerialPort *device;
    XINPUT_STATE state;  //mój gamepad
    DWORD LastPadState;
    int playerID;
    //BYTE TriggerSignalState;
    QTimer *Timer1;
   /* QBluetoothDeviceDiscoveryAgent *discoveryAgent;
    QBluetoothSocket *socket;*/
    int MSGCount;
};
#endif // MAINWINDOW_H
