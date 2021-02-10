#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>
#include <QList>
#include <QSerialPortInfo>
#include <QDateTime>
#include <QTimer>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    playerID= -1;
    ui->setupUi(this);
   // ZeroMemory(&state, sizeof(XINPUT_STATE));
    for (DWORD i = 0; i < XUSER_MAX_COUNT && playerID == -1; i++)
    {
        ZeroMemory(&state, sizeof(XINPUT_STATE));

        if ( XInputGetState(i , &state) == 0)
        {
            playerID = i;
            LastPadState = state.dwPacketNumber;
            XINPUT_VIBRATION vibration;
            ZeroMemory( &vibration, sizeof(XINPUT_VIBRATION) );
           // vibration.wLeftMotorSpeed = 32000; // use any value between 0-65535 here
           // vibration.wRightMotorSpeed = 16000; // use any value between 0-65535 here
            //XInputSetState( playerID, &vibration );
        }
        if( playerID != -1)
        {

        }
    }
    
    this->Timer1 = new QTimer(this);
    this->Timer1->setInterval(250);


  //this->discoveryAgent = new QBluetoothDeviceDiscoveryAgent(this);
   //this->socket = new QBluetoothSocket(QBluetoothServiceInfo::RfcommProtocol, this);

    QObject::connect(this->Timer1, SIGNAL(timeout()), this, SLOT(DisplayTrigger()));


    /*connect(this->discoveryAgent, SIGNAL(finished()),this, SLOT(searchingFinished()));
    connect(this->socket, SIGNAL(connected()),this, SLOT(connectionEstablished()));
    connect(this->socket, SIGNAL(disconnected()),this, SLOT(connectionInterrupted()));
    connect(this->socket, SIGNAL(readyRead()),this, SLOT(socketReadyToRead()));*/

    //TriggerSignalState = 0;
    qDebug()<<playerID;
    this->device = new QSerialPort(this);
    Timer1->start();
    ui->PowerDisplay->setRange(0,100);
    ui->PowerDisplay->setValue(0);

    ui->PitchDisplay->setPalette(Qt::red);
    ui->RollDisplay->setPalette(Qt::red);
    ui->YawDisplay->setPalette(Qt::red);
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::on_pushButtonSerach_clicked()
{
    ui->comboBoxDevices->clear();
    this->addToLogs("Szukam urządzeń...");
    //this->discoveryAgent->start();
    //connect(this->discoveryAgent, SIGNAL(deviceDiscovered(QBluetoothDeviceInfo)), this, SLOT(captureDeviceProperties(QBluetoothDeviceInfo)));
    QList<QSerialPortInfo> devices;
    devices = QSerialPortInfo::availablePorts();
    for(int i = 0; i < devices.count(); i++)
    {
      addToLogs("Znaleziono urządzenie: " + devices.at(i).portName() +" " +  devices.at(i).description()) ;
      ui->comboBoxDevices->addItem(devices.at(i).portName() + " " + devices.at(i).description());
    }
   // ui->pushButtonSerach->setEnabled(false);
}

void MainWindow::addToLogs(QString message)
{
    QString currentDateTime = QDateTime::currentDateTime().toString("yyyy.MM.dd hh:mm:ss");
    ui->textEditConsole->append(currentDateTime + "\t" + message);
}


void MainWindow::on_pushButtonConnect_clicked()
{
    if(ui->comboBoxDevices->count() == 0)
    {
      this->addToLogs("Nie wykryto żadnych urządzeń!");
      return;
    }
    QString portName = ui->comboBoxDevices->currentText().split(" ").first();
    //QString portAdress = ui->comboBoxDevices->currentText().split(" ").last();
    this->device->setPortName(portName);
   // static const QString serviceUuid(QStringLiteral("4C4C4544-0038-5610-8034-CAC04F534D32"));
   //this->socket->connectToService(QBluetoothAddress(portAdress),QBluetoothUuid(serviceUuid),QIODevice::ReadWrite);
    if(!device->isOpen())
    {
        if(device->open(QSerialPort::ReadWrite))
        {
          this->device->setBaudRate(QSerialPort::Baud9600);
          this->device->setDataBits(QSerialPort::Data8);
          this->device->setParity(QSerialPort::NoParity);
          this->device->setStopBits(QSerialPort::OneStop);
          this->device->setFlowControl(QSerialPort::NoFlowControl);
          connect(this->device, SIGNAL(readyRead()), this, SLOT(readFromPort()));
          this->addToLogs("Otwarto port szeregowy.");
        }
        else if(!device->open(QSerialPort::ReadWrite))
        {
            this->addToLogs("Nie powiodło się otwarcie portu!");
        }
    else
      {
            this->addToLogs("Port już jest otwarty!  " + portName);
            return;
      }

    }

}

void MainWindow::on_pushButtonClose_clicked()
{
    if(this->device->isOpen())
    {
        this->device->close();
        this->addToLogs("Zamknięto port.");
    }
    else
    {
        this->addToLogs("Port nie został otwarty!");
        return;
    }
}

void MainWindow::readFromPort()
{
    QStringList EulerAngles; //0pich, 1roll, 2yaw
    while(this->device->canReadLine())
    {
        QString line = this->device->readLine();
        EulerAngles = line.split("\t");
        //qDebug() << line;
        ui->PitchDisplay->display(EulerAngles[0]);
        ui->RollDisplay->display(EulerAngles[1]);
        ui->YawDisplay->display(EulerAngles[2]);


/*        QString terminator = "\t";
        QString terminator2 = "\n";
        int LinePos = line.lastIndexOf(terminator2);
        QString angles = line.left(LinePos);
        int SeparatorPos = angles.lastIndexOf(terminator);
        //qDebug() << line.left(pos);
        //this->addToLogs(angles.left(SeparatorPos));
        //this->addToLogs(angles.right(SeparatorPos));
        pitch = angles.left(SeparatorPos);
        roll = angles.right(SeparatorPos);
        ui->PitchDisplay->display(angles.left(SeparatorPos));
        ui->RollDisplay->display(angles.right(SeparatorPos));*/
    }
}

void MainWindow::sendMessageToDevice(QString message)
{
    if(this->device->isOpen() && this->device->isWritable())
    {
        this->addToLogs("Wysyłam informacje do urządzenia " + message);
        this->device->write(message.toStdString().c_str(),3);
    }
    else
    {
        this->addToLogs("Nie mogę wysłać wiadomości. Port nie jest otwarty!");
    }
}

void MainWindow::on_pushButton_clicked()
{
    this->sendMessageToDevice("95");
    ui->PowerDisplay->setValue(100);
}

void MainWindow::on_pushButton_2_clicked()
{
    this->sendMessageToDevice("45");
    ui->PowerDisplay->setValue(0);
}

void MainWindow::DisplayTrigger()
{
    XInputGetState(playerID , &state);

    if(LastPadState != state.dwPacketNumber)
    {
        LastPadState = state.dwPacketNumber;
        float speed = state.Gamepad.bRightTrigger*55/255 + 45;
        qDebug()<<speed;
        QString message = QString::number(speed);
        if(speed >=0 && speed<= 100)
        {
            ui->PowerDisplay->setValue(speed);
        }
        else
        {
            addToLogs("Błędna prędkość:" + message);
        }
        if(this->device->isOpen() && this->device->isWritable())
        {
            this-> sendMessageToDevice(message);
        }
    }

}
/*void MainWindow::captureDeviceProperties(const QBluetoothDeviceInfo &device)
{
  ui->comboBoxDevices->addItem(device.name() + " " + device.address().toString());
}

void MainWindow::searchingFinished()
{
  ui->pushButtonSerach->setEnabled(true);
  ui->textEditConsole->append(QDateTime::currentDateTime().toString("yyyy.MM.dd hh:mm:ss")  + "\tWyszukiwanie zakończone");
}*/
/*void MainWindow::connectionEstablished()
{
  this->addToLogs("Połączenie ustanowione.");
}

void MainWindow::connectionInterrupted()
{
  this->addToLogs("Połączenie przerwane.");
}

void MainWindow::socketReadyToRead()
{
  this->addToLogs("Odczytuje dane.");
}*/
