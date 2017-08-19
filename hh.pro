QT += widgets

FORMS += \
    widget.ui \
    end_widget.ui \
    log_widget.ui \
    setdialog.ui

HEADERS += \
    widget.h \
    white.h \
    black.h \
    chess.h \
    stack.h \
    end_widget.h \
    log_widget.h \
    setdialog.h

SOURCES += \
    widget.cpp \
    main.cpp \
    white.cpp \
    black.cpp \
    chess.cpp \
    end_widget.cpp \
    log_widget.cpp \
    setdialog.cpp

RESOURCES += \
    myimages.qrc

RC_ICONS = myico.ico

