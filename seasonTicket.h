#ifndef SEASONTICKET_H
#define SEASONTICKET_H
#include "global.h"
#include <QString>

struct snTckt{
    explicit snTckt(const QString num_= QString(),
            const QString &barcode_=QString(),
            const QString &encodedBarcode_=QString(),
            const int &isPrn_=0)
        : num(num_), barcode(barcode_), isPrn(isPrn_) {}

    bool operator<(const snTckt &other) const
        { return num != other.num ? num < other.num : false; }

    QString num;
    QString barcode;
    QString encodedBarcode;
    int isPrn;

};

#endif // SEASONTICKET_H
