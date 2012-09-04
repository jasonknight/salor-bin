#ifndef COMMON_INCLUDES_H
#define COMMON_INCLUDES_H

// Linux based includes

#ifdef Q_OS_LINUX
#include <termios.h> /* POSIX terminal control definitions */
#include <linux/stat.h>
#endif

#ifdef Q_OS_WIN32

#endif

#include <string.h>  /* String function definitions */
#include <unistd.h>  /* UNIX standard function definitions */
#include <fcntl.h>   /* File control definitions */
#include <errno.h>   /* Error number definitions */
#include <stdio.h>   /* Standard input/output definitions */
#endif // COMMON_INCLUDES_H
