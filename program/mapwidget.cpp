﻿#include "mapwidget.h"

#include <QApplication>
#include <QStateMachine>
#include <QPushButton>
#include <QSignalTransition>
#include <QPropertyAnimation>
#include <QPainter>
#include <QState>
#include <QLabel>

MapWidget::MapWidget(QWidget *parent) :
    QWidget(parent), state(-1)
{
    this->setAutoFillBackground(true);
    QPalette palette = this->palette();
    palette.setBrush(QPalette::Background, QBrush(QPixmap(":/map.jpg")));
    this->setPalette(palette);
}

void MapWidget::paintEvent(QPaintEvent *)
{
    QPainter painter(this);

    Widget *fatherPtr = (Widget *)parentWidget();
    if (fatherPtr->currentTraveler != -1)
    {
        std::vector<Attribute> path = fatherPtr->travelers[fatherPtr->currentTraveler].getPlan();
        painter.drawPixmap((setPointPos(path)), setPointGraph());
    }
    update();
}

QPixmap MapWidget::setPointGraph()
{
    QPixmap pointGraph;
    switch(state)
    {
    case -2://arrived destination
        pointGraph = QPixmap(":/arrive.ico");
        break;
    case -1://pause waiting
        pointGraph = QPixmap(":/pause.ico");
        break;
    case 0:
        pointGraph = QPixmap(":/car.ico");
        break;
    case 1:
        pointGraph = QPixmap(":/train.ico");
        break;
    case 2:
        pointGraph = QPixmap(":/plane.ico");
        break;
    }

    return pointGraph;
}

QPointF MapWidget::setPointPos(const std::vector<Attribute> &path)
{
    Widget *fatherPtr = (Widget *)parentWidget();
    static QPointF pointPos;

    if(fatherPtr->getSpentTime() >= fatherPtr->travelers[fatherPtr->currentTraveler].totalTime)
    {
         pointPos = getCityCor(path[path.size()-1].to);
         state = -2;
         qDebug() << "State: Arrival";
    }
    else
        for (std::vector<Attribute>::size_type index = 0;
            index != path.size(); index++)
        {
            if (fatherPtr->getSpentTime() <=
                    getSplitTime(fatherPtr->travelers[fatherPtr->currentTraveler].startTime,
                                 fatherPtr->travelers[fatherPtr->currentTraveler].getCityDepartureDateTime(path[index].from)))
            {
                pointPos = getCityCor(path[index].from);
                state = -1;
                qDebug() << "State: Stop" << index << fatherPtr->travelers[fatherPtr->currentTraveler].getCityDepartureDateTime(path[index].from).time().hour() << fatherPtr->travelers[fatherPtr->currentTraveler].getCityDepartureDateTime(path[index].from).time().minute();
                break;
            }
            else if (fatherPtr->getSpentTime() <=
                     getSplitTime(fatherPtr->travelers[fatherPtr->currentTraveler].startTime,
                                  fatherPtr->travelers[fatherPtr->currentTraveler].getCityArrivalDateTime(path[index].to)))
            {
                pointPos = getCityCor(path[index].from);
                QDateTime spentTime = fatherPtr->getSpentTime();
                QDateTime start2Begin = getSplitTime(fatherPtr->travelers[fatherPtr->currentTraveler].startTime,
                        fatherPtr->travelers[fatherPtr->currentTraveler].getCityDepartureDateTime(path[index].from));
                QDateTime start2End = getSplitTime(fatherPtr->travelers[fatherPtr->currentTraveler].startTime,
                        fatherPtr->travelers[fatherPtr->currentTraveler].getCityArrivalDateTime(path[index].to));
                pointPos += getMoveDistance(spentTime, start2Begin, start2End, path[index].from, path[index].to);
                state = path[index].vehicle;
                qDebug() << "State: Run" << index;
                break;
            }
        }
    qDebug() << pointPos.x() << pointPos.y();
    qDebug() << "----------------------------";
    return pointPos;
}

QDateTime MapWidget::getSplitTime(QDateTime former, QDateTime later)
{
    int durationSec = (later.time().second() - former.time().second());
    int durationMin = (later.time().minute() - former.time().minute() - (int)((durationSec >= 0)?0:1));
    int durationHour = (later.time().hour() - former.time().hour() - (int)((durationMin >= 0)?0:1));
    int durationDay = (later.date().day() - former.date().day() - (int)((durationHour >= 0)?0:1) + former.date().daysInMonth())
            % former.date().daysInMonth();
    durationSec = (durationSec + 60) % 60;
    durationMin = (durationMin + 60) % 60;
    durationHour = (durationHour + 24) % 24;

    return QDateTime(QDate(1, 1, durationDay+1), QTime(durationHour, durationMin, durationSec, 999));

    /*
    int formerYear, formerMonth, formerDay,
            formerHour, formerMin;
    int laterYear, laterMonth, laterDay,
            laterHour, laterMin;
    QDate formerDate = former.date();
    QTime formerTime = former.time();
    QDate laterDate = later.date();
    QTime laterTime = later.time();
    formerDate.getDate(&formerYear, &formerMonth, &formerDay);
    formerHour = formerTime.hour();
    formerMin = formerTime.minute();
    laterDate.getDate(&laterYear, &laterMonth, &laterDay);
    laterHour = laterTime.hour();
    laterMin = laterTime.minute();

    QDateTime splitDateTime;
    int splitYear, splitMonth, splitDay,
            splitHour, splitMin;
    splitYear = laterYear - formerYear;
    splitMonth = laterMonth - formerMonth;
    splitDay = laterDay - formerDay;
    splitHour = laterHour - formerHour;
    splitMin = laterMin - formerMin;

    if (splitMin < 0)
    {
        splitMin += 60;
        splitHour--;
    }
    if (splitHour < 0)
    {
        splitHour += 24;
        splitDay--;
    }
    if (splitDay < 0)
    {
        splitMonth--;
        switch (formerMonth)
        {
        case 1:
        case 3:
        case 5:
        case 7:
        case 8:
        case 10:
        case 12:
            splitDay += 31;
            break;
        case 4:
        case 6:
        case 9:
        case 11:
            splitDay += 30;
            break;
        case 2:
            if (formerYear % 4 == 0 || formerYear % 400 == 0)
                splitDay += 29;
            else
                splitDay += 28;
            break;
        }
    }
    if (splitMonth < 0)
    {
        splitMonth += 12;
        splitYear--;
    }
    QDate splitDate(splitYear, splitMonth, splitDay);
    QTime splitTime(splitHour, splitMin, 0, 0);
    splitDateTime.setDate(splitDate);
    splitDateTime.setTime(splitTime);
    return splitDateTime;
    */
}

QPointF MapWidget::getCityCor(int city)
{
    //QPointF coordinate;
    int x, y;
    switch (city)
    {
    case 0:
        x = 20;
        y = 20;
        break;
    case 1:
        x = 95;
        y = 20;
        break;
    case 2:
        x = 170;
        y = 20;
        break;
    case 3:
        x = 245;
        y = 20;
        break;
    case 4:
        x = 20;
        y = 95;
        break;
    case 5:
        x = 95;
        y = 95;
        break;
    case 6:
        x = 170;
        y = 95;
        break;
    case 7:
        x = 245;
        y = 95;
        break;
    case 8:
        x = 20;
        y = 170;
        break;
    case 9:
        x = 95;
        y = 170;
        break;
    case 10:
        x = 170;
        y = 170;
        break;
    case 11:
        x = 245;
        y = 170;
    }
   // coordinate.setX(x);
   // coordinate.setY(y);
    return QPointF(x, y);
}

double MapWidget::getTimeDifference(QDateTime shorterDateTime, QDateTime longerDateTime)
{
    int shorterYear, shorterMonth, shorterDay,
            shorterHour, shorterMin, shorterSec;
    int longerYear, longerMonth, longerDay,
            longerHour, longerMin, longerSec;
    QDate shorterDate = shorterDateTime.date();
    QTime shorterTime = shorterDateTime.time();
    QDate longerDate = longerDateTime.date();
    QTime longerTime = longerDateTime.time();
    shorterDate.getDate(&shorterYear, &shorterMonth, &shorterDay);
    shorterHour = shorterTime.hour();
    shorterMin = shorterTime.minute();
    shorterSec = shorterTime.second();
    longerDate.getDate(&longerYear, &longerMonth, &longerDay);
    longerHour = longerTime.hour();
    longerMin = longerTime.minute();
    longerSec = longerTime.second();

    int diffYear, diffMonth, diffDay,
            diffHour, diffMin, diffSec;
    diffYear = longerYear - shorterYear;
    diffMonth = longerMonth - shorterMonth;
    diffDay = longerDay - shorterDay;
    diffHour = longerHour - shorterHour;
    diffMin = longerMin - shorterMin;
    diffSec = longerTime.second() - shorterTime.second();

    diffMonth += 12 * diffYear;
    diffDay += 30 * diffMonth;
    diffHour += 24 * diffDay;
    diffMin += 60 * diffHour;
    diffSec += 60 * diffMin;

    return (double)diffSec;
}

QPointF MapWidget::getMoveDistance(QDateTime spentTime, QDateTime start2Begin, QDateTime start2End,
                                   int from, int to)
{
    //QPointF moveDistance;
    double increaseRatio = getTimeDifference(start2Begin, spentTime)/getTimeDifference(start2Begin, start2End);
    double xIncrease = (getCityCor(to) - getCityCor(from)).x() * increaseRatio;
    double yIncrease = (getCityCor(to) - getCityCor(from)).y() * increaseRatio;
    //moveDistance.setX(xIncrease);
    //moveDistance.setY(yIncrease);
    return QPointF(xIncrease, yIncrease);
}
