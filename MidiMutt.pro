QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS
DEFINES += __WINDOWS_MM__

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    DarkStyle/DarkStyle.cpp \
    include/RtMidi/RtMidi.cpp \
    source/action.cpp \
    source/actionarea.cpp \
    source/keyaction.cpp \
    source/keyhandler.cpp \
    source/main.cpp \
    source/mainwindow.cpp \
    source/midihandler.cpp \
    source/midiwindow.cpp \
    source/mouseaction.cpp \
    source/mousehandler.cpp \
    source/outputwindow.cpp \
    source/profile.cpp \
    source/profilearea.cpp \
    source/scriptaction.cpp \
    source/settingswindow.cpp

HEADERS += \
    DarkStyle/DarkStyle.h \
    header/action.h \
    header/actionarea.h \
    header/app.h \
    header/keyaction.h \
    header/keyhandler.h \
    header/mainwindow.h \
    header/midihandler.h \
    header/midiwindow.h \
    header/mouseaction.h \
    header/mousehandler.h \
    header/outputwindow.h \
    header/profile.h \
    header/profilearea.h \
    header/scriptaction.h \
    header/settingswindow.h \
    include/RtMidi/RtMidi.h \
    include/lua/lauxlib.h \
    include/lua/lua.h \
    include/lua/lua.hpp \
    include/lua/luaconf.h \
    include/lua/luajit.h \
    include/lua/lualib.h \
    include/sol/config.hpp \
    include/sol/forward.hpp \
    include/sol/sol.hpp \

 win32:RC_ICONS += icons/icon.ico

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    DarkStyle/darkstyle.qrc \
    img.qrc

INCLUDEPATH += $$PWD/ \
            += $$PWD/include \
            += $$PWD/include/RtMidi \
            += $$PWD/include/sol \
            += $$PWD/include/lua
DEPENDPATH += $$PWD/.

win32: LIBS += -L$$PWD/lib -L$$(luajit) -lwinmm -llua51

INCLUDEPATH += $$PWD/.
DEPENDPATH += $$PWD/.

DISTFILES += \
    DarkStyle/darkstyle/darkstyle.qss \
    DarkStyle/darkstyle/icon_branch_closed.png \
    DarkStyle/darkstyle/icon_branch_end.png \
    DarkStyle/darkstyle/icon_branch_more.png \
    DarkStyle/darkstyle/icon_branch_open.png \
    DarkStyle/darkstyle/icon_checkbox_checked.png \
    DarkStyle/darkstyle/icon_checkbox_checked_disabled.png \
    DarkStyle/darkstyle/icon_checkbox_checked_pressed.png \
    DarkStyle/darkstyle/icon_checkbox_indeterminate.png \
    DarkStyle/darkstyle/icon_checkbox_indeterminate_disabled.png \
    DarkStyle/darkstyle/icon_checkbox_indeterminate_pressed.png \
    DarkStyle/darkstyle/icon_checkbox_unchecked.png \
    DarkStyle/darkstyle/icon_checkbox_unchecked_disabled.png \
    DarkStyle/darkstyle/icon_checkbox_unchecked_pressed.png \
    DarkStyle/darkstyle/icon_close.png \
    DarkStyle/darkstyle/icon_radiobutton_checked.png \
    DarkStyle/darkstyle/icon_radiobutton_checked_disabled.png \
    DarkStyle/darkstyle/icon_radiobutton_checked_pressed.png \
    DarkStyle/darkstyle/icon_radiobutton_unchecked.png \
    DarkStyle/darkstyle/icon_radiobutton_unchecked_disabled.png \
    DarkStyle/darkstyle/icon_radiobutton_unchecked_pressed.png \
    DarkStyle/darkstyle/icon_restore.png \
    DarkStyle/darkstyle/icon_undock.png \
    DarkStyle/darkstyle/icon_vline.png
