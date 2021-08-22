#-------------------------------------------------
#
# Project created by QtCreator 2018-05-18T15:00:30
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

win32-msvc*: {
    message( "MSVC!" )
    QMAKE_CXXFLAGS += /source-charset:utf-8 /execution-charset:utf-8
}

RC_ICONS = icon.ico
RC_CODEPAGE = 65001
TRANSLATIONS = tenkyu_ja.ts

TARGET = tenkyu
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
        celestialsphereprinter.cpp \
        colorselectbutton.cpp \
        main.cpp \
        sofa/20210125_a/c/src/a2af.c \
        sofa/20210125_a/c/src/a2tf.c \
        sofa/20210125_a/c/src/ab.c \
        sofa/20210125_a/c/src/ae2hd.c \
        sofa/20210125_a/c/src/af2a.c \
        sofa/20210125_a/c/src/anp.c \
        sofa/20210125_a/c/src/anpm.c \
        sofa/20210125_a/c/src/apcg.c \
        sofa/20210125_a/c/src/apcg13.c \
        sofa/20210125_a/c/src/apci.c \
        sofa/20210125_a/c/src/apci13.c \
        sofa/20210125_a/c/src/apco.c \
        sofa/20210125_a/c/src/apco13.c \
        sofa/20210125_a/c/src/apcs.c \
        sofa/20210125_a/c/src/apcs13.c \
        sofa/20210125_a/c/src/aper.c \
        sofa/20210125_a/c/src/aper13.c \
        sofa/20210125_a/c/src/apio.c \
        sofa/20210125_a/c/src/apio13.c \
        sofa/20210125_a/c/src/atci13.c \
        sofa/20210125_a/c/src/atciq.c \
        sofa/20210125_a/c/src/atciqn.c \
        sofa/20210125_a/c/src/atciqz.c \
        sofa/20210125_a/c/src/atco13.c \
        sofa/20210125_a/c/src/atic13.c \
        sofa/20210125_a/c/src/aticq.c \
        sofa/20210125_a/c/src/aticqn.c \
        sofa/20210125_a/c/src/atio13.c \
        sofa/20210125_a/c/src/atioq.c \
        sofa/20210125_a/c/src/atoc13.c \
        sofa/20210125_a/c/src/atoi13.c \
        sofa/20210125_a/c/src/atoiq.c \
        sofa/20210125_a/c/src/bi00.c \
        sofa/20210125_a/c/src/bp00.c \
        sofa/20210125_a/c/src/bp06.c \
        sofa/20210125_a/c/src/bpn2xy.c \
        sofa/20210125_a/c/src/c2i00a.c \
        sofa/20210125_a/c/src/c2i00b.c \
        sofa/20210125_a/c/src/c2i06a.c \
        sofa/20210125_a/c/src/c2ibpn.c \
        sofa/20210125_a/c/src/c2ixy.c \
        sofa/20210125_a/c/src/c2ixys.c \
        sofa/20210125_a/c/src/c2s.c \
        sofa/20210125_a/c/src/c2t00a.c \
        sofa/20210125_a/c/src/c2t00b.c \
        sofa/20210125_a/c/src/c2t06a.c \
        sofa/20210125_a/c/src/c2tcio.c \
        sofa/20210125_a/c/src/c2teqx.c \
        sofa/20210125_a/c/src/c2tpe.c \
        sofa/20210125_a/c/src/c2txy.c \
        sofa/20210125_a/c/src/cal2jd.c \
        sofa/20210125_a/c/src/cp.c \
        sofa/20210125_a/c/src/cpv.c \
        sofa/20210125_a/c/src/cr.c \
        sofa/20210125_a/c/src/d2dtf.c \
        sofa/20210125_a/c/src/d2tf.c \
        sofa/20210125_a/c/src/dat.c \
        sofa/20210125_a/c/src/dtdb.c \
        sofa/20210125_a/c/src/dtf2d.c \
        sofa/20210125_a/c/src/eceq06.c \
        sofa/20210125_a/c/src/ecm06.c \
        sofa/20210125_a/c/src/ee00.c \
        sofa/20210125_a/c/src/ee00a.c \
        sofa/20210125_a/c/src/ee00b.c \
        sofa/20210125_a/c/src/ee06a.c \
        sofa/20210125_a/c/src/eect00.c \
        sofa/20210125_a/c/src/eform.c \
        sofa/20210125_a/c/src/eo06a.c \
        sofa/20210125_a/c/src/eors.c \
        sofa/20210125_a/c/src/epb.c \
        sofa/20210125_a/c/src/epb2jd.c \
        sofa/20210125_a/c/src/epj.c \
        sofa/20210125_a/c/src/epj2jd.c \
        sofa/20210125_a/c/src/epv00.c \
        sofa/20210125_a/c/src/eqec06.c \
        sofa/20210125_a/c/src/eqeq94.c \
        sofa/20210125_a/c/src/era00.c \
        sofa/20210125_a/c/src/fad03.c \
        sofa/20210125_a/c/src/fae03.c \
        sofa/20210125_a/c/src/faf03.c \
        sofa/20210125_a/c/src/faju03.c \
        sofa/20210125_a/c/src/fal03.c \
        sofa/20210125_a/c/src/falp03.c \
        sofa/20210125_a/c/src/fama03.c \
        sofa/20210125_a/c/src/fame03.c \
        sofa/20210125_a/c/src/fane03.c \
        sofa/20210125_a/c/src/faom03.c \
        sofa/20210125_a/c/src/fapa03.c \
        sofa/20210125_a/c/src/fasa03.c \
        sofa/20210125_a/c/src/faur03.c \
        sofa/20210125_a/c/src/fave03.c \
        sofa/20210125_a/c/src/fk425.c \
        sofa/20210125_a/c/src/fk45z.c \
        sofa/20210125_a/c/src/fk524.c \
        sofa/20210125_a/c/src/fk52h.c \
        sofa/20210125_a/c/src/fk54z.c \
        sofa/20210125_a/c/src/fk5hip.c \
        sofa/20210125_a/c/src/fk5hz.c \
        sofa/20210125_a/c/src/fw2m.c \
        sofa/20210125_a/c/src/fw2xy.c \
        sofa/20210125_a/c/src/g2icrs.c \
        sofa/20210125_a/c/src/gc2gd.c \
        sofa/20210125_a/c/src/gc2gde.c \
        sofa/20210125_a/c/src/gd2gc.c \
        sofa/20210125_a/c/src/gd2gce.c \
        sofa/20210125_a/c/src/gmst00.c \
        sofa/20210125_a/c/src/gmst06.c \
        sofa/20210125_a/c/src/gmst82.c \
        sofa/20210125_a/c/src/gst00a.c \
        sofa/20210125_a/c/src/gst00b.c \
        sofa/20210125_a/c/src/gst06.c \
        sofa/20210125_a/c/src/gst06a.c \
        sofa/20210125_a/c/src/gst94.c \
        sofa/20210125_a/c/src/h2fk5.c \
        sofa/20210125_a/c/src/hd2ae.c \
        sofa/20210125_a/c/src/hd2pa.c \
        sofa/20210125_a/c/src/hfk5z.c \
        sofa/20210125_a/c/src/icrs2g.c \
        sofa/20210125_a/c/src/ir.c \
        sofa/20210125_a/c/src/jd2cal.c \
        sofa/20210125_a/c/src/jdcalf.c \
        sofa/20210125_a/c/src/ld.c \
        sofa/20210125_a/c/src/ldn.c \
        sofa/20210125_a/c/src/ldsun.c \
        sofa/20210125_a/c/src/lteceq.c \
        sofa/20210125_a/c/src/ltecm.c \
        sofa/20210125_a/c/src/lteqec.c \
        sofa/20210125_a/c/src/ltp.c \
        sofa/20210125_a/c/src/ltpb.c \
        sofa/20210125_a/c/src/ltpecl.c \
        sofa/20210125_a/c/src/ltpequ.c \
        sofa/20210125_a/c/src/num00a.c \
        sofa/20210125_a/c/src/num00b.c \
        sofa/20210125_a/c/src/num06a.c \
        sofa/20210125_a/c/src/numat.c \
        sofa/20210125_a/c/src/nut00a.c \
        sofa/20210125_a/c/src/nut00b.c \
        sofa/20210125_a/c/src/nut06a.c \
        sofa/20210125_a/c/src/nut80.c \
        sofa/20210125_a/c/src/nutm80.c \
        sofa/20210125_a/c/src/obl06.c \
        sofa/20210125_a/c/src/obl80.c \
        sofa/20210125_a/c/src/p06e.c \
        sofa/20210125_a/c/src/p2pv.c \
        sofa/20210125_a/c/src/p2s.c \
        sofa/20210125_a/c/src/pap.c \
        sofa/20210125_a/c/src/pas.c \
        sofa/20210125_a/c/src/pb06.c \
        sofa/20210125_a/c/src/pdp.c \
        sofa/20210125_a/c/src/pfw06.c \
        sofa/20210125_a/c/src/plan94.c \
        sofa/20210125_a/c/src/pm.c \
        sofa/20210125_a/c/src/pmat00.c \
        sofa/20210125_a/c/src/pmat06.c \
        sofa/20210125_a/c/src/pmat76.c \
        sofa/20210125_a/c/src/pmp.c \
        sofa/20210125_a/c/src/pmpx.c \
        sofa/20210125_a/c/src/pmsafe.c \
        sofa/20210125_a/c/src/pn.c \
        sofa/20210125_a/c/src/pn00.c \
        sofa/20210125_a/c/src/pn00a.c \
        sofa/20210125_a/c/src/pn00b.c \
        sofa/20210125_a/c/src/pn06.c \
        sofa/20210125_a/c/src/pn06a.c \
        sofa/20210125_a/c/src/pnm00a.c \
        sofa/20210125_a/c/src/pnm00b.c \
        sofa/20210125_a/c/src/pnm06a.c \
        sofa/20210125_a/c/src/pnm80.c \
        sofa/20210125_a/c/src/pom00.c \
        sofa/20210125_a/c/src/ppp.c \
        sofa/20210125_a/c/src/ppsp.c \
        sofa/20210125_a/c/src/pr00.c \
        sofa/20210125_a/c/src/prec76.c \
        sofa/20210125_a/c/src/pv2p.c \
        sofa/20210125_a/c/src/pv2s.c \
        sofa/20210125_a/c/src/pvdpv.c \
        sofa/20210125_a/c/src/pvm.c \
        sofa/20210125_a/c/src/pvmpv.c \
        sofa/20210125_a/c/src/pvppv.c \
        sofa/20210125_a/c/src/pvstar.c \
        sofa/20210125_a/c/src/pvtob.c \
        sofa/20210125_a/c/src/pvu.c \
        sofa/20210125_a/c/src/pvup.c \
        sofa/20210125_a/c/src/pvxpv.c \
        sofa/20210125_a/c/src/pxp.c \
        sofa/20210125_a/c/src/refco.c \
        sofa/20210125_a/c/src/rm2v.c \
        sofa/20210125_a/c/src/rv2m.c \
        sofa/20210125_a/c/src/rx.c \
        sofa/20210125_a/c/src/rxp.c \
        sofa/20210125_a/c/src/rxpv.c \
        sofa/20210125_a/c/src/rxr.c \
        sofa/20210125_a/c/src/ry.c \
        sofa/20210125_a/c/src/rz.c \
        sofa/20210125_a/c/src/s00.c \
        sofa/20210125_a/c/src/s00a.c \
        sofa/20210125_a/c/src/s00b.c \
        sofa/20210125_a/c/src/s06.c \
        sofa/20210125_a/c/src/s06a.c \
        sofa/20210125_a/c/src/s2c.c \
        sofa/20210125_a/c/src/s2p.c \
        sofa/20210125_a/c/src/s2pv.c \
        sofa/20210125_a/c/src/s2xpv.c \
        sofa/20210125_a/c/src/sepp.c \
        sofa/20210125_a/c/src/seps.c \
        sofa/20210125_a/c/src/sp00.c \
        sofa/20210125_a/c/src/starpm.c \
        sofa/20210125_a/c/src/starpv.c \
        sofa/20210125_a/c/src/sxp.c \
        sofa/20210125_a/c/src/sxpv.c \
        sofa/20210125_a/c/src/taitt.c \
        sofa/20210125_a/c/src/taiut1.c \
        sofa/20210125_a/c/src/taiutc.c \
        sofa/20210125_a/c/src/tcbtdb.c \
        sofa/20210125_a/c/src/tcgtt.c \
        sofa/20210125_a/c/src/tdbtcb.c \
        sofa/20210125_a/c/src/tdbtt.c \
        sofa/20210125_a/c/src/tf2a.c \
        sofa/20210125_a/c/src/tf2d.c \
        sofa/20210125_a/c/src/tpors.c \
        sofa/20210125_a/c/src/tporv.c \
        sofa/20210125_a/c/src/tpsts.c \
        sofa/20210125_a/c/src/tpstv.c \
        sofa/20210125_a/c/src/tpxes.c \
        sofa/20210125_a/c/src/tpxev.c \
        sofa/20210125_a/c/src/tr.c \
        sofa/20210125_a/c/src/trxp.c \
        sofa/20210125_a/c/src/trxpv.c \
        sofa/20210125_a/c/src/tttai.c \
        sofa/20210125_a/c/src/tttcg.c \
        sofa/20210125_a/c/src/tttdb.c \
        sofa/20210125_a/c/src/ttut1.c \
        sofa/20210125_a/c/src/ut1tai.c \
        sofa/20210125_a/c/src/ut1tt.c \
        sofa/20210125_a/c/src/ut1utc.c \
        sofa/20210125_a/c/src/utctai.c \
        sofa/20210125_a/c/src/utcut1.c \
        sofa/20210125_a/c/src/xy06.c \
        sofa/20210125_a/c/src/xys00a.c \
        sofa/20210125_a/c/src/xys00b.c \
        sofa/20210125_a/c/src/xys06a.c \
        sofa/20210125_a/c/src/zp.c \
        sofa/20210125_a/c/src/zpv.c \
        sofa/20210125_a/c/src/zr.c \
        starmap.cpp \
        widget.cpp

HEADERS += \
        celestialsphereprinter.h \
        colorselectbutton.h \
        sofa/20210125_a/c/src/sofa.h \
        sofa/20210125_a/c/src/sofam.h \
        starmap.h \
        widget.h

FORMS += \
        widget.ui

RESOURCES += \
    resource.qrc

DISTFILES += \
    sofa/20210125_a/c/src/makefile
