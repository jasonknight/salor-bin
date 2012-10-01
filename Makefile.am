# The official version of this program, used by make dist
VERSION = 2.2.5

# The final program is called "myapp"
bin_PROGRAMS = salor-bin

# You have two .cpp files you wrote, myapp.cpp and another.cpp
# Remember to include the name of the resource file with the .cpp extension.
salor_bin_SOURCES = \
	cashdrawer.cpp \
	cutecredit.cpp \
	main.cpp\
	mainwindow.cpp \
	optionsdialog.cpp \
	salor_customer_screen.cpp \
	salor_page.cpp \
	salor_plugin_factory.cpp \
	salor_web_plugin.cpp \
	salorcookiejar.cpp \
	salorjsapi.cpp \
	salorprinter.cpp \
	salorprocess.cpp



# You have one .h file, it's called myapp.h. Therefore, here I list
# its mocced name, moc_myapp.cpp.
nodist_salor_bin_SOURCES = \
	moc_cashdrawer.cpp \
	moc_common_includes.cpp \
	moc_cutecredit.cpp \
	moc_mainwindow.cpp \
	moc_optionsdialog.cpp \
	moc_salor_customer_screen.cpp \
	moc_salor_page.cpp \
	moc_salor_plugin_factory.cpp \
	moc_salor_web_plugin.cpp \
	moc_salorcookiejar.cpp \
	moc_salorjsapi.cpp \
	moc_salorprinter.cpp \
	moc_salorprocess.cpp \
	moc_scales.cpp



# This is to ensure the myapp.h file is distributed in your myapp-0.1.tar.gz
# I also have a resources.qrc I need to include. The png is used in myapp.
EXTRA_DIST = \
	$(nodist_salor_bin_SOURCES:moc_%.cpp=%.h) \
	test.html \
	test.escpos \
	salor-bin.pro \
	*.ui

# This rule lets GNU make create any moc_*.cpp from the equivalent *.h
moc_%.cpp: %.h
	moc $< -o $@

optionsdialog.o: ui_optionsdialog.h

ui_optionsdialog.h:
	uic optionsdialog.ui > ui_optionsdialog.h

# Adjust this line according to your resources.qrc
#resources.cpp: resources.qrc salor-splash.png
#	rcc resources.qrc -o resources.cpp

# This line ensures that generated moc_*.cpp and resources.cpp files are
# deleted when we make clean.
CLEANFILES = $(filter moc_%.cpp,$(nodist_salor_bin_SOURCES)) ui_optionsdialog.h