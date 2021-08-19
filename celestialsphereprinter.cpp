#include "celestialsphereprinter.h"
#include <QDateTime>
#include <QtMath>
#include <QTextDocument>
#include <QPdfWriter>
#include <memory>

CelestialSpherePrinter::CelestialSpherePrinter(QObject *parent) : QObject(parent)
{
}

QString CelestialSpherePrinter::generateFileName(bool useDate)
{
    // generate file name
    QString timeStr = QDateTime::currentDateTimeUtc().toString( "yyMMddhhmmss" );
    QString mirStr = inv ? tr( "mir" ) : tr( "upr" );
    QString docName;

    if ( useDate ) {
        docName = QString( "sg-r%1-spl%2-%3-%4.pdf" ).arg( static_cast<int>( radius ) ).arg( static_cast<int>( raSplit ) ).arg( mirStr ).arg( timeStr );
    } else {
        docName = QString( "sg-r%1-spl%2-%3.pdf" ).arg( static_cast<int>( radius ) ).arg( static_cast<int>( raSplit ) ).arg( mirStr );
    }

    return docName;
}

bool CelestialSpherePrinter::openStarData(QString dirName)
{
    if ( !starMap.openDir( dirName ) ) {
        return false;
    }

    return true;
}

bool CelestialSpherePrinter::startPrinting(QString fileName)
{
    if ( fileName.isEmpty() ) {
        return false;
    }

    return startWritePDF( fileName );
}

QString CelestialSpherePrinter::getCreditText()
{
    auto creditText = R"(# 鏡像天球儀
----------
* 設計 : 高橋徹
* プログラム作成 : 高橋昌土 (ilce.ma@gmail.com)

# 使用データについて
鏡像天球儀は「Astro Commons」さんから取得させていただいたデータを使用して作図しています。

## 「Astro Commons」さんのウェブサイト
http://astronomy.webcrow.jp/

## 星座データ
### 使用ファイル
* line.csv
* position.csv
* constellation_name_utf8.csv
### ライセンス
CC0 Public Domain

## ヒッパルコス星表
### 使用ファイル
* hip_lite_a.csv
* hip_lite_b.csv
* hip_100.csv
### ライセンス
CC BY 4.0
### 元データのクレジット
HIPPARCOS - Hipparcos Main Catalog
http://heasarc.gsfc.nasa.gov/W3Browse/star-catalog/hipparcos.html

## メシエ天体
### 使用ファイル
* messier.csv
### ライセンス
CC BY 4.0
### 元データのクレジット
SEDS Messier Database
http://messier.seds.org/

2MASS Atlas Image Gallery: The Messier Catalog
http://www.ipac.caltech.edu/2mass/gallery/messiercat.html

Atlas Image [or Atlas Image mosaic] obtained as part of the Two Micron All Sky Survey (2MASS), a joint project of the University of Massachusetts and the Infrared Processing and Analysis Center/California Institute of Technology, funded by the National Aeronautics and Space Administration and the National Science Foundation.)";
    return creditText;
}

int CelestialSpherePrinter::getStarCount() const
{
    return starMap.starHash.count();
}

bool CelestialSpherePrinter::startWritePDF(QString fileName, QIODevice *dev)
{
    std::unique_ptr<QPdfWriter> printer;

    if ( !fileName.isEmpty() ) {
        printer = std::unique_ptr<QPdfWriter>( new QPdfWriter( fileName ) );
    } else if ( dev != nullptr ) {
        printer = std::unique_ptr<QPdfWriter>( new QPdfWriter( dev ) );
    } else {
        return false;
    }

    // PDF
    printer->setPageSize( pageSize );
    printer->setResolution( printDPI );
    int dpi = printer->resolution();

    QPainter painter( printer.get() );

    if ( !painter.isActive() ) {
        return false;
    }

    // Printing loop
    for ( int i = 0; i < raSplit; i += raRepeatCount ) {
        for ( int dec = 0; dec < 2; dec++ ) {
            for ( int l = 0; l < raRepeatCount; l++ ) {
                int ra = i + l;
                QPointF segOffsetMm;
                double kawaWidth  = ( radius * 2 * M_PI ) / raSplit;
                double kawaHeight = ( radius * 2 * M_PI ) / 4;
                double ofx, ofy;

                if ( inv ) {
                    ofx = ( segmentOffsetMm.x() + kawaWidth ) * ( raRepeatCount - 1 - l );
                } else {
                    ofx = ( segmentOffsetMm.x() + kawaWidth ) * l;
                }

                if ( isNorth( dec ) ) {
                    ofy = ( segmentOffsetMm.y() + kawaHeight ) * dec;
                    segOffsetMm = QPointF( ofx, ofy );
                } else {
                    ofx = ofx + southOffsetMm.x();
                    ofy = southOffsetMm.y();
                    segOffsetMm = QPointF( ofx, ofy );
                }

                paintStarFune( &painter, ra, dec, segOffsetMm, dpi );
            }

            // New page
            if ( deRepeatCount == 1 ) {
                printer->newPage();
            }
        }

        // New page
        if ( deRepeatCount == 2 ) {
            printer->newPage();
        }

        // Report
        emit processPage( i, raSplit - 1 );
    }

    // Draw caps
    double capRadius      = radius * qSin( qDegreesToRadians( 90 - qAbs( capDecNorth ) ) );
    double capRadiusSouth = radius * qSin( qDegreesToRadians( 90 - qAbs( capDecSouth ) ) );
    paintStarCap( &painter, capDecNorth, QPointF( capRadius, capRadius ), dpi );
    paintStarCap( &painter, capDecSouth, QPointF( capRadiusSouth, capRadiusSouth + capRadius * 2 + 30 ), dpi );

    if ( printCreditText ) {
        printer->newPage();
        drawCreditText( &painter, QPointF(), dpi );
    }

    // Explicit end printing
    painter.end();

    // Report
    emit processPage( raSplit - 1, raSplit - 1 );

    return true;
}

void CelestialSpherePrinter::paintStarFune(QPainter *painter, int raPos, int dePos, QPointF segOffsetMm, int dpi)
{
    // 舟形
    // 印刷開始
    auto offPx = mmToPx( pageOffsetMm, dpi );
    // double kawaWidth = ( radius * 2 * M_PI ) / raSplit;
    double kawaHeight = ( radius * 2 * M_PI ) / 4;
    double alpha = 360.0 / raSplit / 2.0;

    double minRA    = raPos * ( 360 / raSplit );
    double maxRA    = ( raPos + 1 ) * ( 360 / raSplit );
    double centerRA = raPos * ( 360 / raSplit ) + alpha;
    double minDE, maxDE;

    if ( isNorth( dePos ) ) {
        minDE = 0;
        maxDE = 90;
    } else {
        minDE = -90;
        maxDE = 0;
    }

    QPen pen;
    QFont font;
    painter->save();

    // Create width list
    QList<QPointF> wakuList;
    QPolygonF waku;

    // Create Fune Frame
    for ( int d = 0; d < 2; d++ ) {
        for ( int i = 0; i <= 90; i += deRes ) {
            int phi = i;

            QPointF p;

            if ( d == 0 ) {
                p = mmPosOnFune( qDegreesToRadians( 360.0 / raSplit / 2.0 ), qDegreesToRadians( static_cast<double>( phi ) ) );
            } else {
                p = mmPosOnFune( qDegreesToRadians( - 360.0 / raSplit / 2.0 ), qDegreesToRadians( static_cast<double>( 90 - phi ) ) );
            }

            if ( !isNorth( dePos ) ) {
                p = QPointF( -p.x(), p.y() );
            }

            wakuList << p;
        }
    }

    for ( auto &p : wakuList ) {
        waku << mmPosOnFuneToPrintPos( p, dpi, dePos, segOffsetMm );
    }

    // Draw waku
    pen = painter->pen();
    pen.setColor( frameColor );
    pen.setStyle( Qt::SolidLine );
    pen.setWidthF( 1.5 );
    painter->setPen( pen );
    painter->setBrush( bgColor );
    painter->drawPolygon( waku );

    // Draw grids
    pen.setWidthF( gridLineWidth );
    pen.setColor( gridColor );
    pen.setStyle( Qt::DotLine );
    painter->setBrush( Qt::NoBrush );
    painter->setPen( pen );

    for ( int i = 0; i <= 90; i += 10 ) {
        int phi;
        QPolygonF deGrid;

        if ( isNorth( dePos ) ) {
            phi = i;
        } else {
            phi = -i;
        }

        for ( int dtheta = -alpha; dtheta <= alpha; dtheta++ ) {
            auto p  = mmPosOnFune( qDegreesToRadians( static_cast<double>( dtheta ) ), qDegreesToRadians( static_cast<double>( phi ) ) );
            auto px = mmPosOnFuneToPrintPos( p, dpi, dePos, segOffsetMm );

            deGrid << px;
        }

        painter->drawPolyline( deGrid );
    }

    // Draw RA grid
    int RAGridWidth = 15;
    pen.setWidthF( gridLineWidth );
    pen.setColor( gridColor );
    pen.setStyle( Qt::DotLine );
    painter->setBrush( Qt::NoBrush );
    painter->setPen( pen );

    for ( int t = RAGridWidth; t < 360 / raSplit; t += RAGridWidth ) {
        QPolygonF raGrid;

        for ( int i = 0; i <= 90; i++ ) {
            int phi;

            if ( isNorth( dePos ) ) {
                phi = i;
            } else {
                phi = -i;
            }

            auto p  = mmPosOnFune( qDegreesToRadians( static_cast<double>( -alpha + t ) ), qDegreesToRadians( static_cast<double>( phi ) ) );
            auto px = mmPosOnFuneToPrintPos( p, dpi, dePos, segOffsetMm );

            raGrid << px;
        }

        painter->drawPolyline( raGrid );
    }

    // Draw Observation point latitude line
    if ( printObsPointLine ) {
        if ( ( isNorth( dePos ) && obsLatitude >= 0 ) || ( !isNorth( dePos ) && obsLatitude < 0 ) ) {
            // Draw zenith line
            QPolygonF latLine;

            for ( int dtheta = -alpha; dtheta <= alpha; dtheta++ ) {
                auto p  = mmPosOnFune( qDegreesToRadians( static_cast<double>( dtheta ) ), qDegreesToRadians( static_cast<double>( obsLatitude ) ) );
                auto px = mmPosOnFuneToPrintPos( p, dpi, dePos, segOffsetMm );

                latLine << px;
            }
            pen.setWidthF( obsPointLineWidth );
            pen.setColor( obsPointColor );
            pen.setStyle( Qt::DotLine );
            painter->setPen( pen );
            painter->drawPolyline( latLine );

            // Draw latitude
            if ( raPos == 0 ) {
                QString latStr;

                if ( useAlphabetText ) {
                    latStr = tr( "Zenith at %1°" ).arg( static_cast<int>( obsLatitude ) );
                } else {
                    latStr = tr( "緯度 %1°の天頂" ).arg( static_cast<int>( obsLatitude ) );
                }

                auto px = mmPosOnFuneToPrintPos( mmPosOnFune( 0, qDegreesToRadians( obsLatitude ) ), dpi, dePos, segOffsetMm );

                pen = painter->pen();
                font = painter->font();
                pen.setColor( obsPointColor );
                pen.setStyle( Qt::SolidLine );
                font.setPointSizeF( infoStrPoint );
                drawStringRawPxAling( painter, latStr, pen, font, Qt::AlignHCenter | Qt::AlignBottom, false, px, 0, QPointF( 0, -1 ) );
            }
        }

        // Draw horizon line
        QPolygonF horizonLine;
        double horizonDec;

        if ( isNorth( dePos ) ) {
            horizonDec = 90 - qAbs( obsLatitude );
        } else {
            horizonDec = -( 90 - qAbs( obsLatitude ) );
        }

        for ( int dtheta = -alpha; dtheta <= alpha; dtheta++ ) {
            auto p  = mmPosOnFune( qDegreesToRadians( static_cast<double>( dtheta ) ), qDegreesToRadians( static_cast<double>( horizonDec ) ) );
            auto px = mmPosOnFuneToPrintPos( p, dpi, dePos, segOffsetMm );

            horizonLine << px;
        }

        pen.setWidthF( obsPointLineWidth );
        pen.setColor( obsPointColor );
        pen.setStyle( Qt::DashLine );
        painter->setPen( pen );
        painter->drawPolyline( horizonLine );
    }

    // Draw constellation lines
    if ( printConstellations ) {
        QPen linePen;
        linePen.setColor( consteColor );
        linePen.setStyle( Qt::SolidLine );
        linePen.setWidthF( consteLineWidth );

        for ( auto &line : starMap.consteLineList ) {
            drawLineOnFune( painter, line, segOffsetMm, raPos, dePos, linePen, dpi );
        }
    }

    // Draw stars
    for ( auto &star : starMap.starHash ) {
        // 範囲チェック
        if ( star.pos.ra < minRA || star.pos.ra >= maxRA || star.pos.de < minDE - 1 || star.pos.de >= maxDE + 1 ) {
            continue;
        }

        if ( star.mag >= maxMagnitude ) {
            continue;
        }

        auto rp = radianPosOnFune( centerRA, star.pos.ra, star.pos.de, inv );
        double dtheta = rp.x();
        double phi = rp.y();
        auto px = mmPosOnFuneToPrintPos( mmPosOnFune( dtheta, phi ), dpi, dePos, segOffsetMm );

        drawStarToRawPx( painter, px.x(), px.y(), star.mag );
    }

    // Draw messiers
    if ( printMessiers ) {
        for ( auto &m : starMap.messierList ) {
            // 範囲チェック
            if ( m.pos.ra < minRA || m.pos.ra >= maxRA || m.pos.de < minDE - 1 || m.pos.de >= maxDE + 1 ) {
                continue;
            }

            auto rp = radianPosOnFune( centerRA, m.pos.ra, m.pos.de, inv );
            double dtheta = rp.x();
            double phi = rp.y();
            auto px = mmPosOnFuneToPrintPos( mmPosOnFune( dtheta, phi ), dpi, dePos, segOffsetMm );

            pen.setWidthF( messierLineWidth );
            pen.setStyle( Qt::DotLine );
            pen.setColor( messierColor );
            drawEllipseToRawPx( painter, px, messierRadius, messierRadius, pen );
        }
    }

    // Star name
    if ( printStarNames ) {
        QPen spen = painter->pen();
        QFont font = painter->font();
        spen.setColor( starNameColor );
        spen.setStyle( Qt::SolidLine );
        font.setPointSizeF( starNamePoint );

        for ( auto &star : starMap.famousStarHash ) {
            // 範囲チェック
            if ( star.pos.ra < minRA || star.pos.ra >= maxRA || star.pos.de < minDE || star.pos.de >= maxDE ) {
                continue;
            }

            if ( star.mag >= maxMagnitude ) {
                continue;
            }

            auto rp = radianPosOnFune( centerRA, star.pos.ra, star.pos.de, inv );
            double dtheta = rp.x();
            double phi = rp.y();

            drawStringOnFune( painter, useAlphabetText ? star.nameEng : star.name, spen, font, false, printNameUnderLine, false, 10, QPointF( 1.5, 0.1 ), dePos, dtheta, phi, dpi, segOffsetMm );
        }
    }

    // Draw contellation names
    if ( printStarNames && printConstellations ) {
        QPen spen = painter->pen();
        QFont font = painter->font();
        spen.setColor( consteColor );
        spen.setStyle( Qt::SolidLine );
        font.setPointSizeF( consteNamePoint );

        for ( auto &conste : starMap.consteHash ) {
            // 範囲チェック
            if ( conste.pos.ra < minRA || conste.pos.ra >= maxRA || conste.pos.de < minDE || conste.pos.de >= maxDE ) {
                continue;
            }

            auto rp = radianPosOnFune( centerRA, conste.pos.ra, conste.pos.de, inv );
            double dtheta = rp.x();
            double phi = rp.y();

            drawStringOnFune( painter, useAlphabetText ? conste.nameEng : conste.name, spen, font, false, false, false, 10, QPointF( 0, 0 ), dePos, dtheta, phi, dpi, segOffsetMm );
        }
    }

    // Draw messier names
    if ( printStarNames && printMessiers ) {
        QPen spen = painter->pen();
        QFont font = painter->font();
        // spen.setBrush( Qt::SolidPattern );
        spen.setColor( messierColor );
        spen.setStyle( Qt::SolidLine );
        font.setPointSizeF( messierNamePoint );

        for ( auto &m : starMap.messierList ) {
            // 範囲チェック
            if ( m.pos.ra < minRA || m.pos.ra >= maxRA || m.pos.de < minDE || m.pos.de >= maxDE ) {
                continue;
            }

            auto rp = radianPosOnFune( centerRA, m.pos.ra, m.pos.de, inv );
            double dtheta = rp.x();
            double phi = rp.y();

            drawStringOnFune( painter, useAlphabetText ? m.nameEng : m.name, spen, font, false, false, false, 10, QPointF( 1, 0 ), dePos, dtheta, phi, dpi, segOffsetMm );
        }
    }

    // Draw RA info text
    pen = painter->pen();
    font = painter->font();
    pen.setColor( infoStrColor );
    pen.setStyle( Qt::SolidLine );
    font.setPointSizeF( infoStrPoint );

    for ( int i = 0; i < 360 / raSplit / RAGridWidth; i++ ) {
        int raH  = i + minRA / 15;
        int raBase8H = raH - 8;

        if ( raBase8H < 0 ) {
            raBase8H = 24 + raBase8H;
        }

        int raMonth  = raBase8H / 2 + 3;
        int timeBase = raBase8H % 2;

        if ( raMonth > 12 ) {
            raMonth = raMonth - 12;
        }

        auto raHStr = tr( "%1h" ).arg( raH );
        QString raDateStr;

        if ( useAlphabetText ) {
            raDateStr = tr( "%1/20 %2h" ).arg( raMonth ).arg( timeBase + 20 );
        } else {
            raDateStr = tr( "%1/20 %2時" ).arg( raMonth ).arg( timeBase + 20 );
        }

        auto rp = radianPosOnFune( centerRA, raH * 15, 0, inv );
        double dtheta = rp.x();
        double phi = rp.y();
        auto px = mmPosOnFuneToPrintPos( mmPosOnFune( dtheta, phi ), dpi, dePos, segOffsetMm );

        int alignH;
        QPointF offset;

        if ( inv ) {
            alignH = Qt::AlignRight;
            offset = QPointF( -2, 0 );
        } else {
            alignH = Qt::AlignLeft;
            offset = QPointF( 2, 0 );
        }

        if ( isNorth( dePos ) ) {
            drawStringRawPxAling( painter, raHStr, pen, font, Qt::AlignBottom | alignH, false, px, 0, offset );
        } else {
            drawStringRawPxAling( painter, raDateStr, pen, font, Qt::AlignTop | alignH, false, px, 0, offset );
        }
    }

    // Draw Dec info txt
    if ( raPos == 0 ) {
        for ( int i = 0; i < 90; i += 10 ) {
            double dec;

            if ( isNorth( dePos ) ) {
                dec = i;
            } else {
                dec = -i;
            }

            QString latStr = QString( "%1°" ).arg( static_cast<int>( dec ) );

            auto px = mmPosOnFuneToPrintPos( mmPosOnFune( 0, qDegreesToRadians( dec ) ), dpi, dePos, segOffsetMm );

            int alignH;
            QPointF offset;

            if ( !inv ) {
                alignH = Qt::AlignRight;
                offset = QPointF( -2, 0 );
            } else {
                alignH = Qt::AlignLeft;
                offset = QPointF( 2, 0 );
            }

            pen = painter->pen();
            font = painter->font();
            pen.setColor( infoStrColor );
            pen.setStyle( Qt::SolidLine );
            font.setPointSizeF( infoStrPoint );
            drawStringRawPxAling( painter, latStr, pen, font, alignH | Qt::AlignBottom, false, px, 0, offset );
        }
    }

    // Draw info
    QString infoText;

    if ( useAlphabetText ) {
        infoText = tr( "RA %1° to %2°, Dec %3° to %4°" ).arg( minRA ).arg( maxRA ).arg( minDE ).arg( maxDE );
    } else {
        infoText = tr( "赤経 %1° から %2°, 赤緯 %3° から %4°" ).arg( minRA ).arg( maxRA ).arg( minDE ).arg( maxDE );
    }

    pen.setColor( infoStrColor );
    pen.setStyle( Qt::SolidLine );
    painter->setPen( pen );
    painter->drawText( offPx.x() + mmToPx( segOffsetMm.rx(), dpi ), mmToPx( kawaHeight, dpi ) + offPx.y() + mmToPx( segOffsetMm.ry(), dpi ) + painter->fontMetrics().height() + 5, infoText );

    painter->restore();
}

void CelestialSpherePrinter::paintStarCap(QPainter *painter, int edgeDE, QPointF segOffsetMm, int dpi)
{
    // Paint a North or South cap
    painter->save();
    QPen pen   = painter->pen();
    QFont font = painter->font();

    auto offPx = mmToPx( pageOffsetMm, dpi );
    double capRadMm;
    int absEdgeDE = qAbs( edgeDE );
    capRadMm = qSin( qDegreesToRadians( static_cast<double>( 90 - absEdgeDE ) ) ) * radius;
    int invFlag = 1;

    if ( !inv ) {
        invFlag = -invFlag;
    }
    if ( edgeDE < 0 ) {
        invFlag = -invFlag;
    }

    // Create Frame
    QPolygonF waku;

    for ( int ra = 0; ra <= 360; ++ra ) {
        double raRad = qDegreesToRadians( static_cast<double>( ra ) );
        QPointF p;

        p = QPointF( qSin( raRad ) * capRadMm, qCos( raRad ) * capRadMm );
        p = p + segOffsetMm + pageOffsetMm;

        waku << mmToPx( p, dpi );
    }

    // Draw waku
    pen.setColor( frameColor );
    pen.setStyle( Qt::SolidLine );
    pen.setWidthF( gridLineWidth );
    painter->setPen( pen );
    painter->setBrush( bgColor );
    painter->drawPolygon( waku );

    // Draw grids
    pen.setWidthF( gridLineWidth );
    pen.setColor( gridColor );
    pen.setStyle( Qt::DotLine );
    painter->setBrush( Qt::NoBrush );
    painter->setPen( pen );

    for ( int i = 0; i <= 90 - absEdgeDE; i += 10 ) {
        double currentRadMm = qSin( qDegreesToRadians( static_cast<double>( i ) ) ) * radius;
        QPolygonF deGrid;

        for ( int ra = 0; ra <= 360; ++ra ) {
            double raRad = qDegreesToRadians( static_cast<double>( ra ) );
            QPointF p;

            p = QPointF( qSin( raRad ) * currentRadMm, qCos( raRad ) * currentRadMm );
            p = p + segOffsetMm + pageOffsetMm;

            deGrid << mmToPx( p, dpi );
        }

        painter->drawPolygon( deGrid );
    }

    // Draw RA grid
    int RAGridWidth = 15;
    pen.setWidthF( gridLineWidth );
    pen.setColor( gridColor );
    pen.setStyle( Qt::DotLine );
    painter->setBrush( Qt::NoBrush );
    painter->setPen( pen );

    for ( int t = 0; t <= 360; t += RAGridWidth ) {
        QPolygonF raGrid;
        double raRad = qDegreesToRadians( static_cast<double>( t ) );
        QPointF raGridCenter = segOffsetMm + pageOffsetMm;
        QPointF raGridEdge = QPointF( qSin( raRad ) * capRadMm, qCos( raRad ) * capRadMm ) + raGridCenter;

        raGrid << mmToPx( raGridCenter, dpi ) << mmToPx( raGridEdge, dpi );

        painter->drawPolyline( raGrid );
    }

    // Draw constellation lines
    if ( printConstellations ) {
        pen.setColor( consteColor );
        pen.setStyle( Qt::SolidLine );
        pen.setWidthF( consteLineWidth );

        for ( auto &line : starMap.consteLineList ) {
            drawLineOnCap( painter, line, segOffsetMm, edgeDE, pen, dpi, inv );
        }
    }

    // Draw stars
    double minDE, maxDE;
    if ( edgeDE >= 0 ) {
        minDE = edgeDE;
        maxDE = 90;
    } else {
        minDE = -90;
        maxDE = edgeDE;
    }

    for ( auto &star : starMap.starList ) {
        // Haitteiruka
        if ( star.pos.de < minDE || star.pos.de >= maxDE ) {
            continue;
        }

        if ( star.mag >= maxMagnitude ) {
            continue;
        }

        auto px = pxPosOnCap( star.pos, edgeDE, inv, segOffsetMm, dpi );
        drawStarToRawPx( painter, px.x(), px.y(), star.mag );
    }

    // Draw messiers
    if ( printMessiers ) {
        for ( auto &m : starMap.messierList ) {
            // 範囲チェック
            if ( m.pos.de < minDE - 1 || m.pos.de >= maxDE + 1 ) {
                continue;
            }

            auto px = pxPosOnCap( m.pos, edgeDE, inv, segOffsetMm, dpi );

            pen.setWidthF( messierLineWidth );
            pen.setStyle( Qt::DotLine );
            pen.setColor( messierColor );
            drawEllipseToRawPx( painter, px, messierRadius, messierRadius, pen );
        }
    }

    // Star name
    if ( printStarNames ) {
        pen = painter->pen();
        font = painter->font();
        pen.setColor( starNameColor );
        pen.setStyle( Qt::SolidLine );
        font.setPointSizeF( starNamePoint );

        for ( auto &star : starMap.famousStarHash ) {
            // 範囲チェック
            if ( star.mag >= maxMagnitude ) {
                continue;
            }

            if ( star.pos.de < minDE || star.pos.de >= maxDE ) {
                continue;
            }

            auto px = pxPosOnCap( star.pos, edgeDE, inv, segOffsetMm, dpi );
            double rotateStr = star.pos.ra * invFlag;

            if ( edgeDE < 0 ) {
                rotateStr = rotateStr + 90;
            } else {
                rotateStr = rotateStr - 90;
            }

            drawStringRawPxAling( painter, useAlphabetText ? star.nameEng : star.name, pen, font, Qt::AlignTop | Qt::AlignHCenter, false, px, rotateStr, QPointF( 0, 4 ) );
        }
    }

    // Constellation name
    if ( printStarNames && printConstellations ) {
        pen = painter->pen();
        font = painter->font();
        pen.setColor( consteColor );
        pen.setStyle( Qt::SolidLine );
        font.setPointSizeF( consteNamePoint );

        for ( auto &e : starMap.consteHash ) {
            // 範囲チェック
            if ( e.pos.de < minDE || e.pos.de >= maxDE ) {
                continue;
            }

            auto px = pxPosOnCap( e.pos, edgeDE, inv, segOffsetMm, dpi );
            double rotateStr = e.pos.ra * invFlag;

            if ( edgeDE < 0 ) {
                rotateStr = rotateStr + 90;
            } else {
                rotateStr = rotateStr - 90;
            }

            drawStringRawPxAling( painter, useAlphabetText ? e.nameEng : e.name, pen, font, Qt::AlignTop | Qt::AlignHCenter, false, px, rotateStr, QPointF( 0, -4 ) );
        }
    }

    // Draw messier names
    if ( printStarNames && printMessiers ) {
        pen = painter->pen();
        font = painter->font();
        pen.setColor( messierColor );
        pen.setStyle( Qt::SolidLine );
        font.setPointSizeF( messierNamePoint );

        for ( auto &e : starMap.messierList ) {
            // 範囲チェック
            if ( e.pos.de < minDE || e.pos.de >= maxDE ) {
                continue;
            }

            auto px = pxPosOnCap( e.pos, edgeDE, inv, segOffsetMm, dpi );
            double rotateStr = e.pos.ra * invFlag;

            if ( edgeDE < 0 ) {
                rotateStr = rotateStr + 90;
            } else {
                rotateStr = rotateStr - 90;
            }

            drawStringRawPxAling( painter, useAlphabetText ? e.nameEng : e.name, pen, font, Qt::AlignTop | Qt::AlignHCenter, false, px, rotateStr, QPointF( 0, 4 ) );
        }
    }

    // Draw RA info text
    pen = painter->pen();
    font = painter->font();
    pen.setColor( infoStrColor );
    pen.setStyle( Qt::SolidLine );
    font.setPointSizeF( infoStrPoint );
    auto px = pxPosOnCap( CelestialPos( 0, edgeDE ), edgeDE, inv, segOffsetMm, dpi );
    if ( edgeDE >= 0 ) {
        drawStringRawPxAling( painter, tr( "0h" ), pen, font, Qt::AlignBottom | Qt::AlignHCenter, false, px, -90, QPointF( 0, -5 ) );
    } else {
        drawStringRawPxAling( painter, tr( "0h" ), pen, font, Qt::AlignTop | Qt::AlignHCenter, false, px, +90, QPointF( 0, 5 ) );
    }

    // Draw info
    QString infoText;

    if ( useAlphabetText ) {
        infoText = tr( "Dec %1° to %2°" ).arg( minDE ).arg( maxDE );
    } else {
        infoText = tr( "赤緯 %1° から %2°" ).arg( minDE ).arg( maxDE );
    }

    pen.setColor( infoStrColor );
    pen.setStyle( Qt::SolidLine );
    painter->setPen( pen );
    painter->drawText( offPx.x() + mmToPx( segOffsetMm.rx(), dpi ), mmToPx( capRadMm, dpi ) + offPx.y() + mmToPx( segOffsetMm.ry(), dpi ) + painter->fontMetrics().height() + 10, infoText );

    painter->restore();
}

double CelestialSpherePrinter::mmToPx(double mm, double dpi)
{
    return mm / 25.4 * dpi;
}

QPointF CelestialSpherePrinter::mmToPx(QPointF p, double dpi)
{
    return QPointF( mmToPx( p.x(), dpi ), mmToPx( p.y(), dpi ) );
}

void CelestialSpherePrinter::drawStarToRawPx(QPainter *p, double x, double y, double magnitude)
{
    double w = starSize / qPow( starSizeFactor, magnitude );

    p->save();
    p->setPen( Qt::NoPen );
    p->setBrush( starColor );
    p->drawEllipse( QPointF( x, y ), w, w );
    p->restore();
}

void CelestialSpherePrinter::drawEllipseToRawPx(QPainter *p, QPointF px, double w, double h, QPen pen)
{
    // Draw a ellipse to the raw px on the painter
    p->save();
    p->setPen( pen );
    p->setBrush( Qt::NoBrush );
    p->drawEllipse( px, w, h );
    p->restore();
}

void CelestialSpherePrinter::drawCrossToRawPx(QPainter *p, QPointF px, double w, double h, QPen pen)
{
    // Draw a cross to the raw px on the painter
    p->save();
    p->setPen( pen );
    p->setBrush( Qt::NoBrush );
    p->drawEllipse( px, w, h );
    p->drawLine( px + QPointF( -w / 2, -h / 2 ), px + QPointF( w / 2,  h / 2 ) );
    p->drawLine( px + QPointF( -w / 2,  h / 2 ), px + QPointF( w / 2, -h / 2 ) );
    p->restore();
}

QPointF CelestialSpherePrinter::radianPosOnFune(double centerRA, double ra, double de, bool invRA)
{
    double localRA = ra - centerRA;
    double dtheta;
    double phi;

    if ( !invRA ) {
        dtheta = qDegreesToRadians( static_cast<double>( localRA ) );
        phi    = qDegreesToRadians( static_cast<double>( de ) );
    } else {
        dtheta = qDegreesToRadians( static_cast<double>( -localRA ) );
        phi    = qDegreesToRadians( static_cast<double>( de ) );
    }

    return QPointF( dtheta, phi );
}

QPointF CelestialSpherePrinter::mmPosOnFune(double dtheta, double phi)
{
    // QPair<緯度増加方向の位置, 経度増加方向の位置>
    // 船の中心からの位置
    // 引数はrad
    double x, y;

    if ( phi == 0 ) {
        x = 0;
        y = radius * dtheta;
    } else {
        x = radius * ( phi + ( 1 / qTan( phi ) ) * ( 1 - cos( dtheta * qSin( phi ) ) ) );
        y = radius / qTan( phi ) * qSin( dtheta * qSin( phi ) );
    }

    return QPointF( x, y );
}

QPointF CelestialSpherePrinter::mmPosOnFuneToPrintPos(QPointF fposMm, int dpi, int dePos, QPointF segmOffsetMm)
{
    // Convert mm pos on fune to printing px pos
    double xpx;
    double ypx;
    auto offPx = mmToPx( pageOffsetMm, dpi );
    double kawaWidthPx = mmToPx( ( radius * 2 * M_PI ) / raSplit, dpi );
    double kawaHeightPx = mmToPx( ( radius * 2 * M_PI ) / 4, dpi );

    double fypx = mmToPx( fposMm.x(), dpi );
    double fxpx = mmToPx( fposMm.y(), dpi );

    double centerXpx = offPx.x() + kawaWidthPx / 2.0;
    double centerYpx;

    if ( isNorth( dePos ) ) {
        // Dex >= 0 North
        centerYpx = offPx.y() + kawaHeightPx;
    } else {
        // Dec < 0 South
        centerYpx = offPx.y();
    }

    xpx = centerXpx + fxpx;
    ypx = centerYpx - fypx;

    return QPointF( xpx, ypx ) + mmToPx( segmOffsetMm, dpi );
}

QPointF CelestialSpherePrinter::pxPosOnCap(CelestialPos pos, int edgeDE, bool invRA, QPointF segOffsetMm, int dpi)
{
    double phi = qDegreesToRadians( 90 - qFabs( pos.de ) );
    double starRA = qDegreesToRadians( static_cast<double>( pos.ra ) );
    double starRadius = qSin( phi ) * radius;

    if ( !invRA ) {
        starRA = -starRA;
    }
    if ( edgeDE < 0 ) {
        starRA = -starRA;
    }

    QPointF raGridCenter = segOffsetMm + pageOffsetMm;
    QPointF starPos = QPointF( qCos( starRA ) * starRadius, qSin( starRA ) * starRadius ) + raGridCenter;
    return mmToPx( starPos, dpi );
}

void CelestialSpherePrinter::drawStringOnFune(QPainter *p, QString str, QPen pen, QFont font, bool center, bool drawUnderLine, bool flipY, double autoRotateDec, QPointF strOffsetMm, int dePos, double dtheta, double phi, int dpi, QPointF segOffsetMm)
{
    // Draw string on fune
    // いまのところ、適当にかいている。ほんとうは境界をよけるとかすべき

    if ( str == "null" || str == "" ) {
        return;
    }

    p->save();
    p->setFont( font );
    p->setPen( pen );

    auto pmm = mmPosOnFune( dtheta, phi );
    auto px  = mmPosOnFuneToPrintPos( pmm, dpi, dePos, segOffsetMm );
    auto strOffPx = mmToPx( strOffsetMm, dpi );
    // double alpha = 360.0 / raSplit / 2.0;
    auto centerPx = mmPosOnFuneToPrintPos( mmPosOnFune( 0, 0 ), dpi, dePos, segOffsetMm );

    // 指定された点を基準とする
    QTransform t;
    t.translate( px.x(), px.y() );

    // Get bounding rect
    QRectF boudingRect;
    boudingRect = p->boundingRect( QRectF(), Qt::AlignCenter, str );

    double txtStartX;
    double txtEndX;
    double txtY;
    double lineStartX;
    double lineEndX;

    // 基本位置決定
    if ( center ) {
        txtStartX =  - boudingRect.width()  / 2.0 + strOffPx.x();
        txtEndX   =  + boudingRect.width()  / 2.0 + strOffPx.x();
        txtY      =  - boudingRect.height() / 2.0 + strOffPx.y();
        lineStartX = txtStartX;
        lineEndX   = txtEndX;
    } else {
        if ( dtheta < 0 ) {
            txtStartX =  + strOffPx.x();
            txtEndX   =  + strOffPx.x() + boudingRect.width();
            lineStartX = 0;
            lineEndX   = txtEndX;
        } else {
            txtStartX = - strOffPx.x() - boudingRect.width();
            // txtEndX   = px.x() - strOffPx.x();
            lineStartX = txtStartX;
            lineEndX   = 0;
        }

        // 上下にはみ出す場合 flip
        bool forceFlipY = false;

        if ( isNorth( dePos ) ) {
            int txtBottomY = px.y() - strOffPx.y() + boudingRect.height();
            if ( txtBottomY > centerPx.y() ) {
                forceFlipY = true;
            }
        } else {
            int txtTopY = px.y() + strOffPx.y() - boudingRect.height();
            if ( txtTopY < centerPx.y() ) {
                forceFlipY = true;
            }
        }

        if ( isNorth( dePos ) != ( flipY | forceFlipY ) ) {
            txtY = - strOffPx.y();
        } else {
            txtY = + strOffPx.y() - boudingRect.height();
        }
    }



    QRectF textRect = boudingRect;
    textRect.moveTopLeft( QPointF( txtStartX, txtY ) );

    // 必要なら回転
    if ( qAbs( qRadiansToDegrees( phi ) ) > ( 90 - autoRotateDec ) ) {
        if ( isNorth( dePos ) ) {
            if ( dtheta < 0 ) {
                t.rotate( 90 );
            } else {
                t.rotate( -90 );
            }
        } else {
            if ( dtheta < 0 ) {
                t.rotate( -90 );
            } else {
                t.rotate( 90 );
            }
        }
    }

    // 変換
    p->setTransform( t );

    // 描画
    QRectF br;
    p->drawText( textRect, Qt::AlignCenter, str, &br );

    // Debug
    /*
    QPen lpen = p->pen();
    lpen.setWidthF( 0.1 );
    p->setPen( lpen );
    p->drawRect( textRect );
    */

    // 必要なら下線
    if ( drawUnderLine ) {
        QPen lpen = p->pen();
        lpen.setColor( underLineColor );
        lpen.setStyle( Qt::DashLine );
        lpen.setWidthF( 0.5 );
        p->setPen( lpen );
        p->drawLine( QLineF( lineStartX, 0, lineEndX, 0 ) );
    }

    p->restore();
}

void CelestialSpherePrinter::drawStringRawPxAling(QPainter *p, QString str, QPen pen, QFont font, int flags, bool drawBoundingRect, QPointF pos, double rotateDeg, QPointF offsetPx)
{
    if ( str == "null" || str == "" ) {
        return;
    }

    p->save();
    p->setFont( font );
    p->setPen( pen );

    // 指定された点を基準とする
    QTransform t;
    t.translate( pos.x(), pos.y() );

    // Get bounding rect
    QRectF boundingRect;
    boundingRect = p->boundingRect( QRectF(), flags, str );

    t.rotate( rotateDeg );
    t.translate( offsetPx.x(), offsetPx.y() );

    // 変換
    p->setTransform( t );

    // 描画
    p->drawText( boundingRect, flags, str );

    if ( drawBoundingRect ) {
        QPen pen = p->pen();
        pen.setWidthF( 0.1 );
        pen.setStyle( Qt::SolidLine );
        p->setPen( pen );
        p->drawRect( boundingRect );
    }

    p->restore();
}

void CelestialSpherePrinter::drawLineOnFune(QPainter *p, CelestialLine line, QPointF segOffsetMm, int raPos, int dePos, QPen pen, int dpi)
{
    auto p1 = line.pos[0];
    auto p2 = line.pos[1];
    double minRA = raPos * ( 360 / raSplit );
    // double maxRA = ( raPos + 1 ) * ( 360 / raSplit );
    double alpha = 360.0 / raSplit / 2.0;
    double centerRA = raPos * ( 360 / raSplit ) + alpha;
    CelestialPos minRAPos, maxRAPos;
    CelestialPos startPos, endPos;
    CelestialPos startPosOnFune, endPosOnFune;
    double grad;

    if ( p1.ra > p2.ra ) {
        minRAPos = p2;
        maxRAPos = p1;
    } else {
        minRAPos = p1;
        maxRAPos = p2;
    }

    // 180度を超えていたら最短距離が360/0度をまたぐと判断
    if ( ( maxRAPos.ra - minRAPos.ra ) > 180 ) {
        // またぐ
        startPos = maxRAPos;
        endPos   = minRAPos;

        // 船中心座標に変換
        if ( minRA <= endPos.ra ) {
            // 線の終点が領域最小の右
            startPosOnFune.ra = -centerRA - ( 360 - startPos.ra );
            endPosOnFune.ra   = endPos.ra - centerRA;
        } else {
            // 線の始点が領域最大より左, その他
            startPosOnFune.ra = startPos.ra - centerRA;
            endPosOnFune.ra   = ( 360 - centerRA ) + endPos.ra;
        }
    } else {
        // またがない
        startPos = minRAPos;
        endPos   = maxRAPos;

        // 船の中心座標に変換
        startPosOnFune.ra = startPos.ra - centerRA;
        endPosOnFune.ra   = endPos.ra   - centerRA;
    }

    startPosOnFune.de = startPos.de;
    endPosOnFune.de   = endPos.de;

    // 範囲チェック
    if ( startPosOnFune.ra > alpha || endPosOnFune.ra < -alpha ) {
        return;
    }

    // 傾き
    grad = ( endPosOnFune.de - startPosOnFune.de ) / ( endPosOnFune.ra - startPosOnFune.ra );

    // 開始地点が描画範囲の外場合丸める
    if ( startPosOnFune.ra < -alpha ) {
        startPosOnFune.de = startPosOnFune.de + ( -alpha - startPosOnFune.ra ) * grad;
        startPosOnFune.ra = -alpha;
    }

    // 終了地点が描画範囲の外の場合丸める
    if ( endPosOnFune.ra > alpha ) {
        endPosOnFune.de = startPosOnFune.de + ( alpha - startPosOnFune.ra ) * grad;
        endPosOnFune.ra = alpha;
    }

    // 範囲チェック
    if ( isNorth( dePos ) ) {
        if ( startPosOnFune.de < 0 && endPosOnFune.de < 0 ) {
            return;
        }
    } else {
        if ( startPosOnFune.de > 0 && endPosOnFune.de > 0 ) {
            return;
        }
    }

    // 赤緯が範囲外なら丸める
    if ( isNorth( dePos ) ) {
        if ( startPosOnFune.de < 0 ) {
            startPosOnFune.ra = startPosOnFune.ra + ( 1 / grad ) * qAbs( startPosOnFune.de );
            startPosOnFune.de = 0;
        }

        if ( endPosOnFune.de < 0 ) {
            endPosOnFune.ra = startPosOnFune.ra + ( 1 / grad ) * -qAbs( startPosOnFune.de );
            endPosOnFune.de = 0;
        }
    } else {
        if ( startPosOnFune.de > 0 ) {
            startPosOnFune.ra = startPosOnFune.ra + ( 1 / grad ) * -qAbs( startPosOnFune.de );
            startPosOnFune.de = 0;
        }

        if ( endPosOnFune.de > 0 ) {
            endPosOnFune.ra = startPosOnFune.ra + ( 1 / grad ) * qAbs( startPosOnFune.de );
            endPosOnFune.de = 0;
        }
    }

    if ( inv ) {
        startPosOnFune.ra = -startPosOnFune.ra;
        endPosOnFune.ra = -endPosOnFune.ra;
    }

    QPointF dtphi1( qDegreesToRadians( startPosOnFune.ra ), qDegreesToRadians( startPosOnFune.de ) );
    QPointF dtphi2( qDegreesToRadians( endPosOnFune.ra ),   qDegreesToRadians( endPosOnFune.de ) );

    drawLineOnFune( p, dtphi1, dtphi2, segOffsetMm, dePos, pen, dpi );
}

void CelestialSpherePrinter::drawLineOnFune(QPainter *p, QPointF dtphi1, QPointF dtphi2, QPointF segOffsetMm, int dePos, QPen pen, int dpi)
{
    // Draw a line on fune
    p->save();

    QPointF p1 = mmPosOnFuneToPrintPos( mmPosOnFune( dtphi1.x(), dtphi1.y() ), dpi, dePos, segOffsetMm );
    QPointF p2 = mmPosOnFuneToPrintPos( mmPosOnFune( dtphi2.x(), dtphi2.y() ), dpi, dePos, segOffsetMm );

    p->setPen( pen );
    p->drawLine( p1, p2 );

    p->restore();
}

void CelestialSpherePrinter::drawLineOnCap(QPainter *p, CelestialLine line, QPointF segOffsetMm, int edgeDE, QPen pen, int dpi, bool invRA)
{
    // Draw a line on the cap
    auto p1 = line.pos[0];
    auto p2 = line.pos[1];
    bool north;
    double absEdgeDE = qAbs( edgeDE );

    if ( edgeDE >= 0 ) {
        north = true;
    } else {
        north = false;
    }

    if ( north ) {
        if ( p1.de < edgeDE && p2.de < edgeDE ) {
            return;
        }
    } else {
        if ( p1.de > edgeDE && p2.de > edgeDE ) {
            return;
        }
    }

    QPointF raGridCenter = segOffsetMm + pageOffsetMm;
    QPointF centerPx = mmToPx( raGridCenter, dpi );

    CelestialPos minRAPos, maxRAPos;
    CelestialPos startPos, endPos;
    CelestialPos startPosOnCap, endPosOnCap;
    double grad;

    if ( p1.ra > p2.ra ) {
        minRAPos = p2;
        maxRAPos = p1;
    } else {
        minRAPos = p1;
        maxRAPos = p2;
    }

    // 180度を超えていたら最短距離が360/0度をまたぐと判断
    if ( ( maxRAPos.ra - minRAPos.ra ) > 180 ) {
        // またぐ
        startPos = maxRAPos;
        endPos   = minRAPos;

        startPosOnCap = startPos;
        endPosOnCap = CelestialPos( endPos.ra + 360, endPos.de );
    } else {
        // またがない
        startPos = minRAPos;
        endPos   = maxRAPos;

        startPosOnCap = startPos;
        endPosOnCap = endPos;
    }

    // 傾き
    grad = ( endPosOnCap.ra - startPosOnCap.ra ) / ( endPosOnCap.de - startPosOnCap.de );

    // round up dec
    if ( qAbs( startPosOnCap.de ) < absEdgeDE ) {
        double addDec = edgeDE - startPosOnCap.de;
        double newRa = startPosOnCap.ra + addDec * grad;
        startPosOnCap = CelestialPos( newRa, edgeDE );
    }

    if ( qAbs( endPosOnCap.de ) < absEdgeDE ) {
        double addDec = edgeDE - endPosOnCap.de;
        double newRa = endPosOnCap.ra + addDec * grad;
        endPosOnCap = CelestialPos( newRa, edgeDE );
    }

    // Inv
    if ( !invRA ) {
        startPosOnCap.ra = -startPosOnCap.ra;
        endPosOnCap.ra = -endPosOnCap.ra;
    }

    if ( edgeDE < 0 ) {
        startPosOnCap.ra = -startPosOnCap.ra;
        endPosOnCap.ra = -endPosOnCap.ra;
    }

    double startPosRadiusPx = mmToPx( qSin( qDegreesToRadians( 90 - qAbs( startPosOnCap.de ) ) ) * radius, dpi );
    double endPosRadiusPx   = mmToPx( qSin( qDegreesToRadians( 90 - qAbs( endPosOnCap.de ) ) ) * radius, dpi );
    auto startPosPx = QPointF( startPosRadiusPx * qCos( qDegreesToRadians( startPosOnCap.ra ) ), startPosRadiusPx * qSin( qDegreesToRadians( startPosOnCap.ra ) ) );
    auto endPosPx   = QPointF( endPosRadiusPx * qCos( qDegreesToRadians( endPosOnCap.ra ) ), endPosRadiusPx * qSin( qDegreesToRadians( endPosOnCap.ra ) ) );

    // Draw a line
    QTransform t;
    t.translate( centerPx.x(), centerPx.y() );

    p->save();
    p->setTransform( t );
    p->setPen( pen );
    p->drawLine( startPosPx, endPosPx );
    p->restore();
}

void CelestialSpherePrinter::drawCreditText(QPainter *p, QPointF offsetMm, int dpi)
{
    auto creditText = getCreditText();
    QTextDocument doc;
#if (QT_VERSION >= QT_VERSION_CHECK(5, 14, 0))
    doc.setMarkdown( creditText );
#else
    doc.setPlainText( creditText );
#endif

    p->save();
    QRectF r = p->window();
    r.setTopLeft( r.topLeft() + mmToPx( offsetMm + pageOffsetMm, dpi ) );
    r.setBottomRight( r.bottomRight() - mmToPx( pageOffsetMm, dpi ) );
    // p->drawText( r, creditText );
    p->translate( r.topLeft() );
    doc.setTextWidth( r.width() );
    doc.drawContents( p, QRectF( QPointF(), QPointF( r.width(), r.height() ) ) );
    p->restore();
}

bool CelestialSpherePrinter::isNorth(int dePos)
{
    if ( dePos == 0 ) {
        return true;
    } else {
        return false;
    }
}
