#ifndef COLORSELECTBUTTON_H
#define COLORSELECTBUTTON_H

#include <QPushButton>
#include <QColor>

class ColorSelectButton : public QPushButton
{
    Q_OBJECT
    Q_PROPERTY(QColor color READ color WRITE setColor RESET resetColor NOTIFY colorChanged)
public:
    explicit ColorSelectButton(QWidget *parent = 0);

    const QColor &color() const;
    void setColor(const QColor &newColor);
    void resetColor();

public slots:
private slots:
    void selectColor();

signals:
    void colorChanged();

protected:
    virtual void paintEvent(QPaintEvent *event) override;

private:
    QColor m_color;
    double colorBoxFactor = 0.7;

};

#endif // COLORSELECTBUTTON_H
