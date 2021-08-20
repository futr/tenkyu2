#include "widget.h"
#include "ui_widget.h"
#include <QDateTime>
#include <QMessageBox>
#include <QTextEdit>
#include <QDialogButtonBox>
#include <QFileInfo>

// dePos : 0 = 0~+90 North

Widget::Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget)
{
    ui->setupUi(this);

    connect( ui->openStarButton,     &QPushButton::clicked, this, &Widget::openStarDir );
    connect( ui->openResourceButton, &QPushButton::clicked, this, &Widget::openStarResourceUI );
    connect( ui->showCreditButton,   &QPushButton::clicked, this, &Widget::showCredit );
    connect( ui->printButton,    &QPushButton::clicked, this, &Widget::startPrinting );
    connect( &cp, &CelestialSpherePrinter::processPage, this, &Widget::pageProcessed );

    ui->gridColorButton->setColor( cp.gridColor );
    ui->frameColorButton->setColor( cp.frameColor );
    ui->bgColorButton->setColor( cp.bgColor );
    ui->starColorButton->setColor( cp.starColor );
    ui->starNameColorButton->setColor( cp.starNameColor );
    ui->underLineColorButton->setColor( cp.underLineColor );
    ui->consteColorButton->setColor( cp.consteColor );
    ui->messierColorButton->setColor( cp.messierColor );
    ui->obsLocationColorButton->setColor( cp.obsPointColor );
    ui->infoTextColorButton->setColor( cp.infoStrColor );

    qDebug() << cp.getLocalDateTime() << cp.getLocalDateTime().toUTC() << cp.getGST( cp.getLocalDateTime() ) << cp.getObsPointZenithRADeg();
}

Widget::~Widget()
{
    delete ui;
}

void Widget::openStarDir()
{
    QString dirName;

    if ( ( dirName = QFileDialog::getExistingDirectory( this, "Select star data file" ) ) == "" ) {
        return;
    }

    if ( !cp.openStarData( dirName ) ) {
        QMessageBox::critical( this, "Error", "Failed to open star data file" );
        return;
    } else {
        QMessageBox::information( this, tr( "Data loaded" ), tr( "%1 stars have been loaded" ).arg( cp.getStarCount() ) );
    }
}

bool Widget::openStarResourceUI()
{
    return openStarResource( true );
}

bool Widget::openStarResource(bool showInfo)
{
    // Read star data from resoruce
    QString dirName;

    dirName = ":/data";

    if ( !cp.openStarData( dirName ) ) {
        if ( showInfo ) QMessageBox::critical( this, "Error", "Failed to open star data file" );
        return false;
    } else {
        if ( showInfo ) QMessageBox::information( this, tr( "Data loaded" ), tr( "%1 stars have been loaded" ).arg( cp.getStarCount() ) );
    }

    return true;
}

void Widget::startPrinting()
{
    // Print stars
    // Copy ui settings
    cp.radius = ui->radiusSpinBox->value();
    cp.pageOffsetMm    = QPointF( ui->printOffXSpinBpx->value(), ui->printOffYSpinBox->value() );
    cp.southOffsetMm   = QPointF( ui->southOffsetXSpinBox->value(), ui->southOffsetYSpinBox->value() );
    cp.segmentOffsetMm = QPointF( ui->segOffsetXSpinBox->value(), ui->segOffsetYSpinBox->value() );
    cp.deRes = ui->deResSpinBox->value();
    cp.raSplit = ui->raNumSpinBox->value();
    cp.inv = ui->invCheckBox->isChecked();
    cp.raRepeatCount = ui->raCountSpinBox->value();
    cp.deRepeatCount = ui->deCountSpinBox->value();
    cp.printStarNames = ui->printNamesCheckBox->isChecked();
    cp.printConsteName = ui->printConsteNameCheckBox->isChecked();
    cp.printMessierName = ui->printMessierNameCheckBox->isChecked();
    cp.maxMagnitude = ui->minMagSpinBox->value();
    cp.obsLatitude = ui->latSpinBox->value();
    cp.printConstellations = ui->printConsteCheckBox->isChecked();
    cp.printMessiers = ui->printMessierCheckBox->isChecked();
    cp.printObsPointLine = ui->printObsLocCheckBox->isChecked();
    cp.printNameUnderLine = ui->printNameUnderLineCheckBox->isChecked();
    cp.printCreditText = ui->printCreditCheckBox->isChecked();
    cp.printCoordinateText = ui->printCoordInfoCheckBox->isChecked();
    cp.printSegmentInfoText = ui->printSegInfoCheckBox->isChecked();
    cp.useAlphabetText = ui->useAlphabetTextCheckBox->isChecked();
    cp.starNamePoint = ui->starNameSizeSpinBox->value();
    cp.messierNamePoint = ui->messierNameSizeSpinBox->value();
    cp.consteNamePoint  = ui->consteNameSizeSpinBox->value();
    cp.infoStrPoint = ui->infoTextSizeSpinBox->value();
    cp.gridLineWidth = ui->gridLineWidthSpinBox->value();
    cp.consteLineWidth = ui->consteLineWidthSpinBox->value();
    cp.messierLineWidth = ui->messierLineWidthSpinBox->value();
    cp.obsPointLineWidth = ui->obsLocLineWidthSpinBox->value();
    cp.messierRadius = ui->messierRadiusSpinBox->value();
    cp.capDecNorth = ui->capNSpinBox->value();
    cp.capDecSouth = ui->capSSpinBox->value();
    cp.starSize = ui->starSizeSpinBox->value();
    cp.starSizeFactor = ui->starSizeFactorSpinBox->value();

    cp.gridColor = ui->gridColorButton->color();
    cp.frameColor = ui->frameColorButton->color();
    cp.bgColor = ui->bgColorButton->color();
    cp.starColor = ui->starColorButton->color();
    cp.starNameColor = ui->starNameColorButton->color();
    cp.underLineColor = ui->underLineColorButton->color();
    cp.consteColor = ui->consteColorButton->color();
    cp.messierColor = ui->messierColorButton->color();
    cp.obsPointColor = ui->obsLocationColorButton->color();
    cp.infoStrColor = ui->infoTextColorButton->color();

    QString docName = QDir( QFileInfo( beforeFileName ).absolutePath() ).absoluteFilePath( cp.generateFileName( ui->useDateNameCheckBox->isChecked() ) );
    QString fileName = QFileDialog::getSaveFileName( this, tr( "Print PDF" ), docName, tr("PDF (*.pdf)") );

    if ( fileName.isEmpty() ) {
        return;
    }

    setDisabled( true );
    pageProcessed( 0, 0 );

    // Read data from resource if no stars have been loaded
    if ( cp.getStarCount() == 0 ) {
        if ( !openStarResource( false ) ) {
            setEnabled( true );
            return;
        }
    }

    beforeFileName = fileName;

    cp.startPrinting( fileName );

    setEnabled( true );
}

void Widget::pageProcessed(int i, int max)
{
    QApplication::processEvents();
    ui->progressBar->setMaximum( max );
    ui->progressBar->setValue( i );
}

void Widget::showCredit()
{
    std::unique_ptr<QDialog> d( new QDialog( this ) );
    auto bb = new QDialogButtonBox( QDialogButtonBox::Ok, d.get() );
    auto te = new QTextEdit( d.get() );
    connect( bb, &QDialogButtonBox::accepted, d.get(), &QDialog::accept );
    d->setLayout( new QVBoxLayout( d.get() ) );
    d->layout()->addWidget( te );
    d->layout()->addWidget( bb );
    te->setMarkdown( cp.getCreditText() );
    d->setWindowTitle( tr( "About" ) );
    d->resize( this->size() * 0.8 );
    d->exec();
}
