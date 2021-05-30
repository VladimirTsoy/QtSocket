#ifndef SERVER_H
#define SERVER_H

#include <QWidget>
#include <QLocalServer>
#include <QLocalSocket>
#include <QTextEdit>
#include <QFile>
#include <QTextStream>
#include <QVBoxLayout>

class Server : public QWidget
{
    Q_OBJECT

public:
    Server(QString serverName, QWidget* parent = 0);
    ~Server();

private:

    // Указатель на QLocalServer
    QLocalServer* localServer;

    QVBoxLayout* layout;

    // Указатель на QTextEdit, в котором будет показываться поступающая
    // от клиента информация
    QTextEdit* textEdit;

    // Переменная для хранения размера получаемого от клиента блока
    quint32 nextBlockSize;

    // Метод для отправки клиенту подтверждения о приёме информации
    void sendToClient(QLocalSocket* localSocket, const QString& string);

public slots:
    // Слот обработки нового клиентского подключения
    void slotNewConnection();

    // Слот чтения информации от клиента
    void slotReadClient();
};
#endif // SERVER_H
