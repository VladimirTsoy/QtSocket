#include "server.h"

#include <QMessageBox>
#include <QLabel>
#include <QTime>
#include "sharedlib.h"

Server::Server(QString serverName, QWidget* parent)
    : QWidget(parent), nextBlockSize(0) // Устанавливаем nextBlockSize равным нулю
{
    // Создаём и запускаем сервер командой listen.
    // Если сервер не может быть запущен, выдать сообщение об ошибке и завершить работу программы
    localServer = new QLocalServer(this);
    if(!localServer->listen(serverName))
    {
        QMessageBox::critical(0, "Server error",
                              "Unable to start server:" + localServer->errorString());
        localServer->close();
        return;
    }

    // Соединяем сигнал сервера о наличии нового подключения с обработчиком нового клиентского подключения
    connect(localServer, SIGNAL(newConnection()), this, SLOT(slotNewConnection()));

    // Формируем окно для просмотра текстовых сообщений от клиента
    textEdit = new QTextEdit;
    textEdit->setReadOnly(true);
    layout = new QVBoxLayout;
    layout->addWidget(new QLabel(serverName));
    layout->addWidget(textEdit);
    setLayout(layout);
}

Server::~Server()
{
    delete localServer;
    delete textEdit;
    delete layout;
}

// Слот обработки нового клиентского подключения
void Server::slotNewConnection()
{
    // Получаем сокет, подключённый к серверу
    QLocalSocket* localSocket = localServer->nextPendingConnection();

    // Соединяем сигнал отключения сокета с обработчиком удаления сокета
    connect(localSocket, SIGNAL(disconnected()), localSocket, SLOT(deleteLater()));
//    connect(localSocket, SIGNAL())
    // Соединяем сигнал сокета о готовности передачи данных с обработчиком данных
    connect(localSocket, SIGNAL(readyRead()), this, SLOT(slotReadClient()));
    // Отправляем информацию клиенту о соединении с сервером
    sendToClient(localSocket, "Server response: Connected!");
}

// Слот чтения информации от клиента
void Server::slotReadClient()
{
    // Получаем QLocalSocket после срабатывания сигнала о готовности передачи данных
    QLocalSocket* localSocket = (QLocalSocket*)sender();
    // Создаём входной поток получения данных на основе сокета
    QDataStream in(localSocket);
    // Устанавливаем версию сериализации данных потока. У клиента и сервера они должны быть одинаковыми
    in.setVersion(QDataStream::Qt_5_3);
    QTime time;
    QString str;

    // Бесконечный цикл нужен для приёма блоков данных разных размеров, от двух байт и выше
    while(true)
    {
        qDebug() << "nextBlockSize = " << nextBlockSize;
        // Если размер блока равен нулю
        if(!nextBlockSize)
        {
            // Если размер передаваемого блока меньше двух байт, выйти из цикла
            if(localSocket->bytesAvailable() < (int)sizeof(quint32))
                break;
            // Извлекаем из потока размер блока данных
            in >> nextBlockSize;
        }

        if(localSocket->bytesAvailable() < nextBlockSize)
            break;

        // Извлекаем из потока время и строку
        in >> time >> str;

        // Преобразуем полученные данные и показываем их в виджете
        QString message = time.toString() + " " + "Client has sent: \n" + str;
        textEdit->append(message);

        nextBlockSize = 0;

        // Отправляем ответ клиенту
        sendToClient(localSocket, "Server response: received ");
    }

    SharedLib sh_lib;
    sh_lib.volwes_to_upper(str);
    QFile file(QString(SHARED_APP_DATA_PATH) + "/output.txt");
    if(file.open(QIODevice::WriteOnly))
    {
        QTextStream stream(&file);
        stream << str;
    }
    file.close();
}

// Метод для отправки клиенту подтверждения о приёме информации
void Server::sendToClient(QLocalSocket* localSocket, const QString& string)
{
    // Поскольку заранее размер блока неизвестен (параметр string может быть любой длины),
    // вначале создаём объект array класса QByteArray
    QByteArray array;
    // На его основе создаём выходной поток
    QDataStream out(&array, QIODevice::WriteOnly);
    // Устанавливаем версию сериализации данных потока
    out.setVersion(QDataStream::Qt_5_3);
    // Записываем в поток данные для отправки. На первом месте идёт нулевой размер блока
    out << quint16(0) << QTime::currentTime() << string;

    // Перемещаем указатель на начало блока
    out.device()->seek(0);
    // Записываем двухбайтное значение действительного размера блока без учёта пересылаемого размера блока
    out << quint16(array.size() - sizeof(quint16));

    // Отправляем получившийся блок клиенту
    localSocket->write(array);
}
