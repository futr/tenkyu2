#include "colorselectbutton.h"
#include <QPainter>
#include <QColorDialog>

ColorSelectButton::ColorSelectButton(QWidget *parent) : QPushButton( parent )
{
    connect( this, &QPushButton::clicked, this, &ColorSelectButton::selectColor );
}

const QColor &ColorSelectButton::color() const
{
    return m_color;
}

void ColorSelectButton::setColor(const QColor &newColor)
{
    if (m_color == newColor)
        return;
    m_color = newColor;

    update();

    emit colorChanged();
}

void ColorSelectButton::resetColor()
{
    setColor({}); // TODO: Adapt to use your actual default value
}

void ColorSelectButton::selectColor()
{
    auto sc = QColorDialog::getColor( m_color, this, tr( "Select color" ), QColorDialog::ShowAlphaChannel );

    if ( !sc.isValid() ) {
        return;
    }

    setColor( sc );
}

void ColorSelectButton::paintEvent(QPaintEvent *event)
{
    QPushButton::paintEvent( event );

    QPainter p( this );
    QRect r = contentsRect();
    double w = qMin( width(), height() );
    QRect colorRect( 0, 0, w * colorBoxFactor, w * colorBoxFactor );
    colorRect.moveCenter( r.center() );

    QColor textColor = this->palette().color( QPalette::ButtonText );

    if ( m_color.isValid() ) {
        p.setPen( m_color.darker() );
        p.setBrush( m_color );
        p.drawRect( colorRect );
    } else {
        p.setPen( textColor );
        p.setBrush( Qt::Dense6Pattern );
        p.drawRect( colorRect );
    }
}
