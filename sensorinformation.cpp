#include "sensorinformation.h"

#include <QtCore/QRandomGenerator>
#include <QtCore/QTimer>
#include <QtCore/QUuid>

#include <QtMqtt/QMqttClient>

//generates sensor data for testing
inline double moveValue(double currentValue, double min, double max, double maxStep)
{
    static QRandomGenerator generator;
    double result = currentValue + generator.bounded(maxStep * 2.) - maxStep;
    if (result < min)
        return min;
    else if (result > max)
        return max;
    return result;
}


Ic_MQTT_client::Ic_MQTT_client(QObject *parent) : QObject(parent)
{
        fiveSecondTimer_ = new QTimer(this);
        fiveSecondTimer_->setInterval(5000);
        fiveSecondTimer_->setSingleShot(false);
        connect(fiveSecondTimer_, &QTimer::timeout, [this]()
        {
            // Every five seconds report device still online
            if (client_->state() == QMqttClient::Connected)
            {
                const QString content = QLatin1String(">Online");
                client_->publish(QLatin1String("inlevelSensors/active"), content.toUtf8());
            }
        });

        secondTimer_ = new QTimer(this);
        secondTimer_->setInterval(1000);
        secondTimer_->setSingleShot(false);
        connect(secondTimer_, &QTimer::timeout, [this]() {
            setDistance(moveValue(distance(), 0., 100., 1));
            setDiameter(moveValue(diameter(), 0., 150., 1));
        });

        client_ = new QMqttClient(this);
        client_->setWillTopic(QLatin1String("inlevelSensors/active"));
        const QString will = QLatin1String(">Offline");
        client_->setWillMessage(will.toUtf8());
        client_->setHostname("localhost");
        client_->setPort(1883);
        connect(client_, &QMqttClient::stateChanged, [](QMqttClient::ClientState s)
        {
            qDebug() << "Client state changed:" << s;
        });

}

void Ic_MQTT_client::start()
{
    client_->connectToHost();
    fiveSecondTimer_->start();
    secondTimer_->start();
}


double Ic_MQTT_client::distance() const
{
    return distance_;
}

double Ic_MQTT_client::diameter() const
{
    return diameter_;
}


void Ic_MQTT_client::setDistance(double distance)
{
    client_->publish(QString::fromLatin1("inlevelSensors/%1/distance"),
                      QByteArray::number(distance));
    distance_ = distance;

    //just for testing
    qDebug() << "Test data distance: " << Ic_MQTT_client::distance_;
}

void Ic_MQTT_client::setDiameter(double diameter)
{
    client_->publish(QString::fromLatin1("inlevelSensors/%1/distance"),
                      QByteArray::number(diameter));
    diameter_ = diameter;

    //just for testing
    qDebug() << "Test data diameter: " << Ic_MQTT_client::diameter_;
}
