#include "widget.h"
#include <QApplication>
#include <QTranslator>
#include <QLibraryInfo>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QTranslator qtTranslator;
    if ( !qtTranslator.load( QLocale(), QLatin1String( "qt" ), QLatin1String( "_" ), QLibraryInfo::location( QLibraryInfo::TranslationsPath ), QLatin1String( ".qm" ) ) ) {
        qtTranslator.load( QLocale(), QLatin1String( "qt" ), QLatin1String( "_" ), QLatin1String( "./translations" ), QLatin1String( ".qm" ) );
    }
    a.installTranslator( &qtTranslator );

    QTranslator myappTranslator;
    if ( !myappTranslator.load( QLocale(), QLatin1String( "tenkyu" ), QLatin1String( "_" ), QLibraryInfo::location( QLibraryInfo::TranslationsPath ), QLatin1String( ".qm" ) ) ) {
        myappTranslator.load( QLocale(), QLatin1String( "tenkyu" ), QLatin1String( "_" ), QLatin1String( "./translations" ), QLatin1String( ".qm" ) );
    }
    a.installTranslator( &myappTranslator );

    QCoreApplication::setApplicationName( QObject::tr( "MirrorImageCelestialGlobeGenerator" ) );
    QGuiApplication::setApplicationDisplayName( QObject::tr( "Mirror-image Celestial Globe Generator" ) );

    Widget w;
    w.show();

    return a.exec();
}
