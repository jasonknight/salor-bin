bin_PROGRAMS = salor-bin

salor_bin_SOURCES = \
	drawerobserver.cpp \
	scale.cpp \
	salorcapture.cpp \
	cutecredit.cpp \
	downloader.cpp \
	main.cpp\
	mainwindow.cpp \
	network.cpp \
	optionsdialog.cpp \
	customerscreen.cpp \
	salorpage.cpp \
	salorcookiejar.cpp \
	salorjsapi.cpp \
	salorprinter.cpp \
	salorprocess.cpp


nodist_salor_bin_SOURCES = \
	moc_drawerobserver.cpp \
	moc_scale.cpp \
	moc_salorcapture.cpp \
	moc_common_includes.cpp \
	moc_cutecredit.cpp \
	moc_downloader.cpp \
	moc_mainwindow.cpp \
	moc_network.cpp \
	moc_optionsdialog.cpp \
	moc_customerscreen.cpp \
	moc_salorpage.cpp \
	moc_salorcookiejar.cpp \
	moc_salorjsapi.cpp \
	moc_salorprinter.cpp \
	moc_salorprocess.cpp


EXTRA_DIST = \
	$(nodist_salor_bin_SOURCES:moc_%.cpp=%.h) \
	test.html \
	test.escpos \
	salor-bin.pro \
	*.ui

# This rule lets GNU make create any moc_*.cpp from the equivalent *.h
moc_%.cpp: %.h
	moc-qt4 $< -o $@

optionsdialog.o: ui_optionsdialog.h
mainwindow.o: ui_mainwindow.h

ui_optionsdialog.h:
	uic optionsdialog.ui > ui_optionsdialog.h

ui_mainwindow.h:
	uic mainwindow.ui > ui_mainwindow.h

# Adjust this line according to your resources.qrc
#resources.cpp: resources.qrc salor-splash.png
#	rcc resources.qrc -o resources.cpp

CLEANFILES = $(filter moc_%.cpp,$(nodist_salor_bin_SOURCES)) ui_optionsdialog.h