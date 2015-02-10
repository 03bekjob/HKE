#include "hketablemodel.h"
#include <QAbstractItemModel>
#include <QStyleOptionComboBox>
#include <QApplication>
#include <QFile>
#include <QMessageBox>
#include <QDataStream>

//#include <QDebug>

namespace {
const int MAXCOLUMNS  = 4;
}

QDataStream &operator<<(QDataStream &out, const snTckt &item)
{
    out << item.num
        << item.barcode
        << item.encodedBarcode
        << static_cast<quint16>(item.isPrn);
    return out;
}


QDataStream &operator>>(QDataStream &in, snTckt &item)
{
    quint16 isPrn;
    in >> item.num >> item.barcode >> item.encodedBarcode >> isPrn;
    item.isPrn = static_cast<int>(isPrn);
    return in;
}

HkeTableModel::HkeTableModel(QObject *parent) :
    QAbstractTableModel(parent)
{
    isDirty = false;
    createConnect();
}


Qt::ItemFlags HkeTableModel::flags(const QModelIndex &index) const
{
    Qt::ItemFlags theFlags = QAbstractTableModel::flags(index);
    if (index.isValid())
        theFlags |= Qt::ItemIsSelectable /*| Qt::ItemIsEditable*/ |
                    Qt::ItemIsEnabled;
    return theFlags;

}

QVariant HkeTableModel::data(const QModelIndex &index, int role) const
{
    if(!index.isValid() ||
       index.row() < 0  || index.row() >= lstSnTckt.count() ||
       index.column()<0 || index.column()>=MAXCOLUMNS )
    {
        return QVariant();
    }

    const snTckt &item = lstSnTckt.at(index.row());
    if (role == Qt::SizeHintRole) {
        QStyleOptionComboBox option;
        switch (index.column()) {
        case num: option.currentText = item.num;
        {
            option.currentText = QString::number(MAXCOUNT);
            const QString header = headerData(num,
                                              Qt::Horizontal, Qt::DisplayRole).toString();
            if (header.length() > option.currentText.length())
                option.currentText = header;
            break;
        }

            break;
        case bc: option.currentText = item.barcode;
            break;
        case ebc: option.currentText = item.encodedBarcode;
            break;
        case isprn: option.currentText = QString::number(item.isPrn);
            break;
        default: Q_ASSERT(false);
        }
        QFontMetrics fontMetrics(data(index, Qt::FontRole).value<QFont>());
        option.fontMetrics = fontMetrics;
        QSize size(fontMetrics.width(option.currentText),
                   fontMetrics.height());
        return qApp->style()->sizeFromContents(QStyle::CT_ComboBox,
                                               &option, size);
    }
    if (role == Qt::DisplayRole /*|| role == Qt::EditRole*/) {
        switch (index.column()) {
        case num: return item.num;
        case bc: return item.barcode;
        case ebc: return item.encodedBarcode;
        case isprn: return item.isPrn;
        default: Q_ASSERT(false);

        }
   }
    return QVariant();

}

QVariant HkeTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if(role != Qt::DisplayRole)
        return QVariant();
    if(orientation == Qt::Horizontal){
        switch (section) {
        case num: return tr("Номер");
        case bc: return tr("Штрих-код");
        case ebc: return tr("закодирваный\nштрих-код");
        case isprn: return tr("Печать\n(да/нет)");
        default: Q_ASSERT(false);
        }
    }

    return section+1;
}

int HkeTableModel::rowCount(const QModelIndex &index) const
{
    return index.isValid() ? 0 : lstSnTckt.count();
}

int HkeTableModel::columnCount(const QModelIndex &index) const
{
    return index.isValid() ? 0 : MAXCOLUMNS;
}

void HkeTableModel::clearMdl()
{
    beginResetModel();
    lstSnTckt.clear();
    endResetModel();

}

bool HkeTableModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if(!index.isValid() || role != Qt::EditRole ||
       index.row() < 0  || index.row() >= lstSnTckt.count() ||
       index.column()<0 || index.column()>=MAXCOLUMNS )
    {
        return false;
    }
    snTckt &item = lstSnTckt[index.row()];//.at(index.row());
    switch (index.column()) {
    case isprn:
        {
            bool ok;
            int iPrn = value.toInt(&ok);
            if(!ok){
                item.isPrn = iPrn;
            }
        }
        break;
    default:
        break;
    }
    emit dataChanged(index,index);
    return true;
}

void HkeTableModel::setFileName(const QString &filename)
{
    m_filename = filename;
}

void HkeTableModel::save(const QString &filename)
{
    if (!filename.isEmpty())
        m_filename = filename;
    if (m_filename.isEmpty()){
        QMessageBox::information(NULL,tr("ВНИМАНИЕ"),tr("Отсутвует имя файла"));
        return;
    }
    QFile file(m_filename);
    if (file.open(QIODevice::WriteOnly)){

        QDataStream out(&file);
//        out << MagicNumber << FormatNumber;
        out.setVersion(QDataStream::Qt_5_3);
        QListIterator<snTckt> i(lstSnTckt);
        while (i.hasNext())
            out << i.next();
    }
    else{
        QMessageBox::information(NULL,tr("ВНИМАНИЕ"),tr("Не возможно открыть файл"));
        return;
    }

    file.close();
}

void HkeTableModel::createLst(QList<snTckt> lst)
{
    lstSnTckt = lst;
//    for(int i=0; i<lstSnTckt.count(); i++){
//        qDebug()<<lstSnTckt[i].num<<lstSnTckt[i].barcode;
//    }
    save(m_filename);
}

void HkeTableModel::load(const QString &filename)
{
    if (!filename.isEmpty())
        m_filename = filename;

    if (m_filename.isEmpty()){
            QMessageBox::critical(NULL,"ОШИБКА","Отсутсвует имя файла");
            return;
    }
    QFile file(m_filename);
    if (!file.open(QIODevice::ReadOnly)){
        QMessageBox::critical(NULL,"ОШИБКА","Не возможно открыть файл");
        return;
    }
    QDataStream in(&file);
    in.setVersion(QDataStream::Qt_5_3);

    beginResetModel();
    lstSnTckt.clear();

    snTckt item;
    while (!in.atEnd()) {
        in >> item;
//        qDebug()<<"item "<<item.num<<item.barcode<<item.encodedBarcode<<item.isPrn;
        lstSnTckt << item;

    }
    //qSort(lstSnTckt);
    //reset();
    endResetModel();
//for(int i = 0; i<lstSnTckt.count(); i++)    {
//    qDebug()<<"lstSnTckt"<<lstSnTckt[i].num<<lstSnTckt.at(i).barcode<<lstSnTckt.at(i).encodedBarcode<<lstSnTckt.at(i).isPrn;
//}

}


void HkeTableModel::createConnect()
{
    connect(this,SIGNAL(dataChanged(QModelIndex,QModelIndex)),this,SLOT(sltUpdPrn(QModelIndex,QModelIndex)));
}

void HkeTableModel::sltUpdPrn(QModelIndex bIdx, QModelIndex eIdx)
{
    if(bIdx.isValid()){
        int row = bIdx.row();
        QModelIndex idx;
        idx = this->index(row,1);
        QString barcode = idx.data().toString();
        for(int i=0; i<lstSnTckt.count(); i++){
            if(barcode == lstSnTckt[i].barcode){
                lstSnTckt[i].isPrn = 1;
            }
        }
    }
}
