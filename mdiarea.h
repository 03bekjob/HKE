#ifndef MDIAREA_H
#define MDIAREA_H

#include <QMdiArea>

class MdiArea : public QMdiArea
{
    Q_OBJECT
public:
    explicit MdiArea(QWidget *parent = 0);

signals:
    void sgnCloseSubWin();
public slots:
   void sltCloseAllSubWin();
protected:
    void paintEvent(QPaintEvent *);
private slots:
   void sltCloseSubWin();
   //void sltCloseAllSubWin();
   void sltCascadeSubWin();
   void sltTileSubWin();
};

#endif // MDIAREA_H
