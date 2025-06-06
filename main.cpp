#include <QCoreApplication>
#include <QTimer>
#include <QDir>
#include <QStandardPaths>
#include "qhookermain.h"

void PrintHelp()
{
    printf(
        "usage: QMamehook"
        #ifdef Q_OS_WIN
        ".exe"
        #endif
        " [-v] [-c] [-p <path>] [-s <sort type>]\n\n"
        "QMamehook: A force-feedback distributor program for lightgun devices\n\n"
        "options:\n"
        "  -v               Enable verbose output (effectively repeating whatever the connected server sends)\n"
        "\n"
        "  -c               Set QMH to exit after the server closes once,\n"
        "                   rather than wait for another connection.\n"
        "\n"
        "  -p <path>        Load config files from the provided <path> instead of the default location\n"
        "                   When not set, QMH will look for .ini files in the following path:\n"
        "                   %s\n"
        "\n"
        "  -s <sort type>   Sort assigned ports based on selected criteria, where <sort type> can be one of:\n"
        "                       pid-ascending   - Product ID in Ascending Order (lowest to highest) [default]\n"
        "                       pid-descending  - Product ID in Descending Order (highest to lowest)\n"
        "                       port-ascending  - System-assigned Port Number, in Ascending Order (lowest to highest)\n"
        "                       port-descending - System-assigned Port Number, in Descending Order (highest to lowest)\n"
        "\n"
        "  -h, --help       Show this help. Helpful, huh? c:\n"
        "\n"
        "If this software has brought any value to you, please consider supporting me (That One Seong) on Ko-fi:\n"
        "https://ko-fi.com/Z8Z5NNXWL\n"
        ,
        QString(QStandardPaths::writableLocation(QStandardPaths::ConfigLocation) +
        #ifndef Q_OS_WIN
        "/QMamehook"
        #endif
        "/ini/").toLocal8Bit().constData()
        );
}

int main(int argc, char *argv[])
{
    QCoreApplication *app = new QCoreApplication(argc, argv);

    qhookerMain mainApp;

    // argc is a count of arguments starting from 1 (executable inclusive),
    // argv is an array containing each argument starting from 0 (executable inclusive).
    if(argc > 1) {
        QStringList arguments;
        for(int i = 1; i < argc; ++i)
            arguments.append(argv[i]);

        if(argc < 3 && (arguments.last() == "-h" || arguments.last() == "--help")) {
            PrintHelp();
            return 0;
        }

        if(arguments.contains("-v")) {
            mainApp.verbosity = true;
            printf("Enabling verbose output.\n");
            arguments.removeAt(arguments.indexOf("-v"));
        }

        if(arguments.contains("-c")) {
            mainApp.closeOnDisconnect = true;
            printf("This session will close upon disconnect.\n");
            arguments.removeAt(arguments.indexOf("-c"));
        }

        if(arguments.contains("-p")) {
            if(arguments.indexOf("-p") < arguments.count()-1) {
                // QDir::fromNativeSeparators uses forwardslashes on both OSes, thank Parace
                mainApp.customPath = QDir::fromNativeSeparators(arguments[arguments.indexOf("-p")+1]);
                mainApp.customPathSet = true;

                if(QDir::isRelativePath(mainApp.customPath))
                    mainApp.customPath.prepend(QDir::currentPath() + '/');

                if(!mainApp.customPath.endsWith('/'))
                    mainApp.customPath.append('/');

                if(QFile::exists(mainApp.customPath)) {
                    printf("Setting search path to \"%s\"\n\n", mainApp.customPath.toLocal8Bit().constData());
                    arguments.removeAt(arguments.indexOf("-p")+1);
                    arguments.removeAt(arguments.indexOf("-p"));
                } else {
                    printf("ERROR: Specified custom path \"%s\" does not seem to exist!\n", arguments.at(arguments.indexOf("-p")+1).toLocal8Bit().constData());
                    return 1;
                }
            } else {
                printf("ERROR: Custom path flag called without any path specified!\n");
                return 1;
            }
        }

        if(arguments.contains("-s")) {
            if(arguments.indexOf("-s") < arguments.size()) {
                if(arguments.at(arguments.indexOf("-s")+1).toLower() == "pid-ascending")
                    printf("Sorting devices by Product ID in ascending order.\n");
                else if(arguments.at(arguments.indexOf("-s")+1).toLower() == "pid-descending") {
                    printf("Sorting devices by Product ID in descending order.\n");
                    mainApp.sortType = qhookerMain::sortPIDdescend;
                } else if(arguments.at(arguments.indexOf("-s")+1).toLower() == "port-ascending") {
                    printf("Sorting devices by port number in ascending order.\n");
                    mainApp.sortType = qhookerMain::sortPortAscend;
                } else if(arguments.at(arguments.indexOf("-s")+1).toLower() == "port-descending") {
                    printf("Sorting devices by port number in descending order.\n");
                    mainApp.sortType = qhookerMain::sortPortDescend;
                } else {
                    printf("ERROR: Sorting type \"%s\" is invalid! Sort type must be one of:\n"
                           "    pid-ascending\n"
                           "    pid-descending\n"
                           "    port-ascending\n"
                           "    port-descending\n",
                           arguments.at(arguments.indexOf("-s")+1).toLocal8Bit().constData());
                    return 1;
                }

                arguments.removeAt(arguments.indexOf("-s")+1);
                arguments.removeAt(arguments.indexOf("-s"));
            } else {
                printf("ERROR: No sorting type specified!\n");
                return 1;
            }
        }
    }

    // connect up the signals
    QObject::connect(&mainApp, SIGNAL(finished()),
                     app, SLOT(quit()), Qt::QueuedConnection);
    QObject::connect(app, SIGNAL(aboutToQuit()),
                     &mainApp, SLOT(aboutToQuitApp()), Qt::QueuedConnection);

    // This code will start the messaging engine in QT and in
    // 10ms it will start the execution in the MainClass.run routine;
    QTimer::singleShot(10, &mainApp, SLOT(run()));

    return app->exec();
}
