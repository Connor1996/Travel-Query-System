﻿#ifndef TRAVELER_H
#define TRAVELER_H

#include <vector>
#include <QDateTime>
#include <QDate>
#include <QTime>
#include "attribute.h"
#include "schedule.h"

class Traveler
{
public:
    int id, strategy;
    int origin, destination;
    QDateTime startTime, deadlineTime;

    Traveler(int, QDateTime, QDateTime, int, int, int);
    void getTotalTime(int &, int &, int &);
    std::vector<Attribute> getPlan();
    std::vector<Attribute> changePlan(int ,int);


private:
    std::vector<Attribute> plan;
    std::vector<QDateTime> time;
};

#endif // TRAVELER_H
