#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QFileDialog>
#include <QDebug>

#include "starmap.h"
#include "celestialsphereprinter.h"

namespace Ui {
class Widget;
}

class Widget : public QWidget
{
    Q_OBJECT
public:
    explicit Widget(QWidget *parent = 0);
    ~Widget();

public slots:
    void openStarDir();
    bool openStarResource(bool showInfo);
    bool openStarResourceUI();
    void startPrinting();

    void pageProcessed( int i, int max );

    void showCredit();

private:
    Ui::Widget *ui;
    CelestialSpherePrinter cp;
    QString beforeFileName;
};

#endif // WIDGET_H
