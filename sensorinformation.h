#ifndef SENSORINFORMATION_H
#define SENSORINFORMATION_H

#include <QtCore/QObject>
#include <QtCore/QString>

class QTimer;
class QMqttClient;

class SensorInformation : public QObject
{
    Q_OBJECT
    Q_PROPERTY(double diameter READ diameter WRITE setDiameter NOTIFY diameterChanged)
    Q_PROPERTY(double distance READ distance WRITE setDistance NOTIFY distanceChanged)

public:
    explicit SensorInformation(QObject *parent = nullptr);

    void start();
    double diameter() const;
    double distance() const;

signals:

    void diameterChanged(double diameter);
    void distanceChanged(double distance);

public slots:

    void setDiameter(double diameter);
    void setDistance(double distance);

private:
    QMqttClient *m_client;
    QString m_id;
    QTimer *m_fiveSecondTimer;
    QTimer *m_secondTimer;
    QTimer *m_tenMsTimer;

    double m_diameter{10.};
    double m_distance{50.};
};

#endif // SENSORINFORMATION_H
