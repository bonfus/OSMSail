#ifndef POSITIONSOURCE_H
#define POSITIONSOURCE_H

#include <memory>

#include <QGeoPositionInfo> 
#include <QGeoCoordinate> 
#include <QGeoPositionInfoSource>

#include "DBThread.h"




class PositionSource : public QObject
{
    Q_OBJECT
    
signals:
    void newPositionReady(const QGeoPositionInfo &info);

public slots:
    void positionUpdated(const QGeoPositionInfo &info);

public:
    PositionSource(QObject *parent = 0);
    virtual ~PositionSource() {};

    QGeoPositionInfo lastKnownPosition(bool fromSatellitePositioningMethodsOnly = false) const;
    
    void updatePosition();
    void stopUpdates();
    
private:
    QGeoPositionInfoSource *source = NULL;
    QGeoPositionInfo lastPosition;
};

typedef std::shared_ptr<PositionSource> PositionSourceRef;


#endif
