#include "client.h"
#include <QVBoxLayout>
#include <QMessageBox>
#include <QLabel>
#include <QTime>

Client::Client(QString serverName, QWidget* parent)
    : QWidget(parent), nextBlockSize(0), revision(0)
    // Устанавливаем nextBlockSize и revision равными нулю
{
    server_name = serverName;
    // Инициализируем сокет
    localSocket = new QLocalSocket(this);

    // Устанавливаем соединение между сигналом ошибки сокета с обработчиком ошибок
    connect(localSocket, QOverload<QLocalSocket::LocalSocketError>::of(&QLocalSocket::error),
        this, &Client::slotError);

    // Устанавливаем имя сервера, к которому сокет должен подключаться
    localSocket->setServerName(serverName);


    // Соединяем сигнал сокета о готовности приёма данных данных с обработчиком данных
    connect(localSocket, SIGNAL(readyRead()), SLOT(slotReadyRead()));
    connect(localSocket, &QLocalSocket::stateChanged, this, &Client::socketStateChanged);

    // Инициализируем элементы интерфейса
    textEdit = new QTextEdit;
    send_file = new QPushButton("Send file");

    // Соединяем нажатие кнопки с обработчиком, передающим информацию о ревизии на сервер
    connect(send_file, SIGNAL(clicked()), this, SLOT(sendFileToServer()));

    // Настраиваем элементы интерфейса и формируем вид окна клиента
    textEdit->setReadOnly(true);
    layout = new QVBoxLayout;
    layout->addWidget(new QLabel("Sender revisions"));
    layout->addWidget(textEdit);
    layout->addWidget(send_file);
    setLayout(layout);

    // Подключение к сокету по таймеру
    tmrToConnect = new QTimer(this);
    tmrToConnect->setSingleShot(true); // !! по умолчанию false
    // по таймеру делаем попытки подключения
    connect(tmrToConnect, SIGNAL(timeout()), this, SLOT(slotToConnect()));
    localSocket->connectToServer(serverName);
}

Client::~Client()
{

}

void Client::slotToConnect()
{
    qDebug() << "connect to server ....";
    localSocket->connectToServer(server_name);
}

void Client::socketStateChanged(QLocalSocket::LocalSocketState state)
{
    switch(state)
    {
    case QLocalSocket::UnconnectedState:
        tmrToConnect->start(TIME_TO_CONNECT);
        break;
    case QLocalSocket::ConnectingState:
    case QLocalSocket::ConnectedState:
    case QLocalSocket::ClosingState:
        if(tmrToConnect->isActive())
            tmrToConnect->stop();
        break;
    }
}


// Слот чтения информации, получаемой от сервера
void Client::slotReadyRead()
{
    // Всё аналогично приёму информации на стороне сервера
    QDataStream in(localSocket);
    in.setVersion(QDataStream::Qt_5_3);
    QTime time;
    QString string;
    for(;;)
    {
        if(!nextBlockSize)
        {
            if(localSocket->bytesAvailable() < (int)sizeof(quint16))
                break;
        }
        in >> nextBlockSize;

        if(localSocket->bytesAvailable() < nextBlockSize)
            break;

        in >> time >> string;

        textEdit->append(time.toString() + " " + string);
        nextBlockSize = 0;
    }
}

// Слот обработки ошибок сокета
void Client::slotError(QLocalSocket::LocalSocketError error)
{
    QString strError =
        "Error: " + (error == QLocalSocket::ServerNotFoundError ?
                     "The server was not found." :
                     error == QLocalSocket::PeerClosedError ?
                     "The server is closed." :
                     error == QLocalSocket::ConnectionRefusedError ?
                     "The connection was refused." :
                     QString(localSocket->errorString()));
    textEdit->append(strError);
}

void Client::sendFileToServer()
{
    // Блок для передачи
    QByteArray arrayBlock = 0;
    QDataStream out(&arrayBlock, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_5_3);

    QFile file(QString(SHARED_APP_DATA_PATH) + "/input.txt");
    QTextStream stream(&file);
    QString input_str = "";

    if(file.open(QIODevice::ReadOnly))
           input_str = stream.readAll();
    file.close();

    out << quint32(0) << QTime::currentTime() << input_str;
    out.device()->seek(0);
    out << quint32(arrayBlock.size() - sizeof(quint32));
    out.device()->seek(0);
    out << quint32(arrayBlock.size() - sizeof(quint32));

    localSocket->write(arrayBlock);
    localSocket->waitForBytesWritten();
}


// Слот обработки сигнала соединения с сервером
void Client::slotConnected()
{
    textEdit->append("Received the connected() signal");
}
