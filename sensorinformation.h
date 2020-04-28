#ifndef SENSORINFORMATION_H
#define SENSORINFORMATION_H

#include <QtCore/QObject>
#include <QtCore/QString>

class QTimer;
class QMqttClient;

class Ic_MQTT_client : public QObject
{
    Q_OBJECT
    Q_PROPERTY(double diameter READ diameter WRITE setDiameter NOTIFY diameterChanged)
    Q_PROPERTY(double distance READ distance WRITE setDistance NOTIFY distanceChanged)

public:
    explicit Ic_MQTT_client(QObject *parent = nullptr);
    void start();
    //bool connect();
    QString publish_topic(QString topicDiameter, QString topicDistance);
    double diameter() const;
    double distance() const;

signals:

    void diameterChanged(double diameter);
    void distanceChanged(double distance);

public slots:

    void setDiameter(double diameter);
    void setDistance(double distance);

private:
    QMqttClient *client_;
    QString id_;
    QTimer *fiveSecondTimer_;
    QTimer *secondTimer_;
    QTimer *tenMsTimer_;

    double diameter_{10.};
    double distance_{50.};

};

#endif // SENSORINFORMATION_H
