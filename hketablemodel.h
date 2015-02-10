#ifndef HKETABLEMODEL_H
#define HKETABLEMODEL_H

#include <QAbstractTableModel>
#include "seasonTicket.h"

class HkeTableModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    explicit HkeTableModel(QObject *parent = 0);

    Qt::ItemFlags flags(const QModelIndex &index) const;

    QVariant data(const QModelIndex &index, int role=Qt::DisplayRole) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role=Qt::DisplayRole) const;

    int rowCount(const QModelIndex &index=QModelIndex()) const;
    int columnCount(const QModelIndex &index=QModelIndex()) const;

    void clearMdl();
    bool setData(const QModelIndex &index, const QVariant &value, int role=Qt::EditRole);
    bool setHeaderData(int, Qt::Orientation, const QVariant&, int=Qt::EditRole) { return false; }

//    bool insertRows(int row, int count,
//                    const QModelIndex &parent=QModelIndex());
//    bool removeRows(int row, int count,
//                    const QModelIndex &parent=QModelIndex());

//    QString filename() const { return m_filename; }
    void setFileName(const QString &filename=QString());
    void save(const QString &filename=QString());
    void createLst(QList<snTckt> lst);
    void load(const QString &filename=QString());

    bool IsDirty() {return isDirty;}
    void setIsDirty(bool b) {isDirty = b;}
    QString getFileName() {return m_filename;}
signals:

public slots:

private:
    QString m_filename;
    QList<snTckt> lstSnTckt;
    void createConnect();
    bool isDirty;
private slots:
    void sltUpdPrn(QModelIndex bIdx, QModelIndex eIdx);
};

#endif // HKETABLEMODEL_H
