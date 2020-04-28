#include "sensorinformation.h"
#include <QCoreApplication>

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    Ic_MQTT_client s;
    s.start();

    return a.exec();
}
