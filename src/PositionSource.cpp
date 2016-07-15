#include "PositionSource.h"

#include <QDebug>

PositionSource::PositionSource(QObject *parent) :
    QObject(parent)
{
    source = QGeoPositionInfoSource::createDefaultSource(this);

    if (source)
    {    
        source->setUpdateInterval(1000);
        connect(source, SIGNAL(positionUpdated(QGeoPositionInfo)), this, SLOT(positionUpdated(QGeoPositionInfo)));
    
    
        DBThread *dbThread=DBThread::GetInstance();
        if (dbThread!=NULL)
        {
            connect(this,SIGNAL(newPositionReady(QGeoPositionInfo)),
                    dbThread,SLOT(TriggerPositionRendering(QGeoPositionInfo)));
        
        }
        source->startUpdates();        
    }
    
}

//PositionSource::~PositionSource()
//{
//    //nocode
//}

void PositionSource::positionUpdated(const QGeoPositionInfo &info)
{
    if(info.isValid())
    {
        qDebug() << "Valid";
        lastPosition = info;
        emit newPositionReady(info);
    }
    else
    {
        if(lastPosition.isValid())
            emit newPositionReady(lastPosition);
        qDebug() << "Position error";
    }
    
}

void PositionSource::updatePosition()
{
    if (source)
        source->startUpdates();
}

void PositionSource::stopUpdates()
{
    if (source)
        source->stopUpdates();
}


QGeoPositionInfo PositionSource::lastKnownPosition(bool /*fromSatellitePositioningMethodsOnly*/) const
{
    return lastPosition;
}
