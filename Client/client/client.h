#ifndef CLIENT_H
#define CLIENT_H

#include <QMainWindow>
#include <QLocalSocket>
#include <QTextEdit>
#include <QPushButton>
#include <QTimer>
#include <QFile>
#include <QString>
#include <QStringList>
#include <QFileInfo>
#include <QVBoxLayout>

#define TIME_TO_CONNECT 10000 //msec

class Client : public QWidget
{
    Q_OBJECT

public:
    Client(QString serverName, QWidget* parent = 0);
    ~Client();

    friend QDataStream& operator <<(QDataStream& stream, const QFile& file);
    friend QDataStream& operator >>(QDataStream& stream, QFile& file);


private:
    // Указатель на QLocalSocket
    QLocalSocket* localSocket;

    // Указатели на элементы интерфейса
    QTextEdit* textEdit;
    QPushButton* send_file;

    // Размер принимаемого от сервера блока
    quint16 nextBlockSize;

    // Номер ревизии, отправляемый серверу
    // Увеличивается при каждом нажатии QPushButton
    int revision;

    QTimer *tmrToConnect;
    QString server_name;
    QVBoxLayout* layout;

private slots:
    // Слот чтения информации, получаемой от сервера
    void slotReadyRead();

    // Слот обработки ошибок сокета
    void slotError(QLocalSocket::LocalSocketError error);

    // Слот передачи информации на сервер
    void sendFileToServer();

    // Слот обработки сигнала соединения с сервером
    void slotConnected();

    void slotToConnect();

    void socketStateChanged(QLocalSocket::LocalSocketState state);
};
#endif // CLIENT_H
