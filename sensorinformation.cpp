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

SensorInformation::SensorInformation(QObject *parent) : QObject(parent)
{
    m_id = QUuid::createUuid().toString();

    m_fiveSecondTimer = new QTimer(this);
    m_fiveSecondTimer->setInterval(5000);
    m_fiveSecondTimer->setSingleShot(false);
    connect(m_fiveSecondTimer, &QTimer::timeout, [this]() {
        // Every five seconds report device still online
        if (m_client->state() == QMqttClient::Connected) {
            const QString content = m_id + QLatin1String(">Online");
            m_client->publish(QLatin1String("qtdemosensors/active"), content.toUtf8());
        }
    });

    m_secondTimer = new QTimer(this);
    m_secondTimer->setInterval(1000);
    m_secondTimer->setSingleShot(false);
    connect(m_secondTimer, &QTimer::timeout, [this]() {
        setDistance(moveValue(distance(), 0., 100., 1));
        setDiameter(moveValue(diameter(), 0., 150., 1));
    });

    m_tenMsTimer = new QTimer(this);
    connect(m_tenMsTimer, &QTimer::timeout, [this]() {
    });
    m_tenMsTimer->setInterval(20);
    m_tenMsTimer->setSingleShot(false);

    m_client = new QMqttClient(this);
    m_client->setWillTopic(QLatin1String("qtdemosensors/active"));
    const QString will = m_id + QLatin1String(">Offline");
    m_client->setWillMessage(will.toUtf8());
    m_client->setHostname("localhost");
    m_client->setPort(1883);
    connect(m_client, &QMqttClient::stateChanged, [](QMqttClient::ClientState s) {
        qDebug() << "Client state changed:" << s;
    });

}

void SensorInformation::start()
{
    m_client->connectToHost();
    m_fiveSecondTimer->start();
    m_secondTimer->start();
    m_tenMsTimer->start();
}


double SensorInformation::distance() const
{
    return m_distance;
}

double SensorInformation::diameter() const
{
    return m_diameter;
}

void SensorInformation::setDistance(double distance)
{
    if (qFuzzyCompare(m_distance, distance))
        return;

    m_client->publish(QString::fromLatin1("qtdemosensors/%1/distance").arg(m_id),
                      QByteArray::number(distance));
    m_distance = distance;
    emit distanceChanged(m_distance);

    //just for testing
    qDebug() << "Test data distance: " << SensorInformation::m_distance;
}

void SensorInformation::setDiameter(double diameter)
{
    if (qFuzzyCompare(m_diameter, diameter))
        return;

    m_client->publish(QString::fromLatin1("qtdemosensors/%1/diameter").arg(m_id),
                      QByteArray::number(diameter));
    m_diameter = diameter;
    emit diameterChanged(m_diameter);

    //just for testing
    qDebug() << "Test data diameter: " << SensorInformation::m_diameter;
}
