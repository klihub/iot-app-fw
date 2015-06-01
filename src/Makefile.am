SUBDIRS         = . # common/tests
AM_CFLAGS       = $(WARNING_CFLAGS) $(AM_CPPFLAGS) \
		  -DSYSCONFDIR=\"@SYSCONFDIR@\" -DLIBDIR=\"@LIBDIR@\"
IOT_CFLAGS   =
pkgconfigdir    = ${libdir}/pkgconfig

bin_PROGRAMS    =
lib_LTLIBRARIES =
pkgconfig_DATA  =
EXTRA_DIST      =

QUIET_GEN      = $(Q:@=@echo '  GEN   '$@;)

AM_CPPFLAGS     = -I$(top_builddir) -I$(top_builddir)/src
BUILT_SOURCES   =

###################################
# IoT common library
#

lib_LTLIBRARIES += libiot-common.la
EXTRA_DIST      += common/iot-common.pc
pkgconfig_DATA  += common/iot-common.pc


libiot_commonh_ladir      =		\
		$(includedir)/iot

libiot_commonh_la_HEADERS =		\
		common.h		\
		config.h

libiot_common_ladir      =		\
		$(includedir)/iot/common

libiot_common_la_HEADERS =		\
		common/macros.h		\
		common/list.h		\
		common/log.h		\
		common/debug.h 		\
		common/env.h		\
		common/mm.h		\
		common/hash-table.h	\
		common/hashtbl.h	\
		common/mainloop.h	\
		common/utils.h		\
		common/socket-utils.h	\
		common/refcnt.h		\
		common/fragbuf.h	\
		common/json.h		\
		common/transport.h	\
		common/mask.h

libiot_common_la_REGULAR_SOURCES =		\
		common/log.c			\
		common/debug.c			\
		common/env.c			\
		common/mm.c			\
		common/hash-table.c		\
		common/mainloop.c		\
		common/utils.c			\
		common/socket-utils.c		\
		common/fragbuf.c		\
		common/json.c			\
		common/transport.c		\
		common/stream-transport.c	\
		common/dgram-transport.c

libiot_common_la_SOURCES =				\
		$(libiot_common_la_REGULAR_SOURCES)

libiot_common_la_CFLAGS  =		\
		$(AM_CFLAGS)		\
		$(JSON_CFLAGS)

libiot_common_la_LDFLAGS =					\
		-Wl,-version-script=linker-script.common	\
		-version-info @IOT_VERSION_INFO@

libiot_common_la_LIBADD  = 		\
		$(JSON_LIBS)		\
		-lrt

libiot_common_la_DEPENDENCIES =	\
		linker-script.common	\
		$(filter %.la, $(libiot_common_la_LIBADD))

# linker script generation
linker-script.common: $(libiot_common_la_HEADERS)
	$(QUIET_GEN)$(top_builddir)/build-aux/gen-linker-script -q \
		-P "$(CC)" -c "$(libiot_common_la_CFLAGS)" \
		-p '^iot_|^_iot_|^mrp_|^_mrp_' -o $@ $^

clean-linker-script::
	-rm -f linker-script.common


###################################
# IoT pulse glue library
#

iot_pulse_headers =			\
		common/pulse-glue.h

if PULSE_ENABLED
lib_LTLIBRARIES += libiot-pulse.la
EXTRA_DIST      += common/iot-pulse.pc
pkgconfig_DATA  += common/iot-pulse.pc

libiot_pulse_ladir      =		\
		$(includedir)/iot/common

libiot_pulse_la_HEADERS = $(iot_pulse_headers)

libiot_pulse_la_SOURCES =		\
		common/pulse-glue.c

libiot_pulse_la_CFLAGS  =		\
		$(AM_CFLAGS)		\
		$(PULSE_CFLAGS)

libiot_pulse_la_LDFLAGS =		\
		-Wl,-version-script=linker-script.pulse \
		-version-info @IOT_VERSION_INFO@

libiot_pulse_la_LIBADD  =		\
		libiot-common.la	\
		$(PULSE_LIBS)

libiot_pulse_la_DEPENDENCIES =	\
		linker-script.pulse	\
		$(filter %.la, $(libiot_pulse_la_LIBADD))

libpulseincludedir      = $(includedir)/iot/pulse
libpulseinclude_HEADERS = $(libiot_pulse_la_HEADERS)
endif

# linker script generation
linker-script.pulse: $(iot_pulse_headers)
	$(QUIET_GEN)$(top_builddir)/build-aux/gen-linker-script -q \
		-P "$(CC)" -c "$(libiot_pulse_la_CFLAGS)" \
                -p '^iot_|^_iot|^mrp_|^_mrp_' -o $@ $^

clean-linker-script::
	-rm -f linker-script.pulse

###################################
# IoT ecore glue library
#

iot_ecore_headers =			\
		common/ecore-glue.h

if ECORE_ENABLED
lib_LTLIBRARIES += libiot-ecore.la
EXTRA_DIST      += common/iot-ecore.pc
pkgconfig_DATA  += common/iot-ecore.pc

libiot_ecore_ladir      =		\
		$(includedir)/iot/common

libiot_ecore_la_HEADERS = $(iot_ecore_headers)

libiot_ecore_la_SOURCES =		\
		common/ecore-glue.c

libiot_ecore_la_CFLAGS  =		\
		$(AM_CFLAGS)		\
		$(ECORE_CFLAGS)

libiot_ecore_la_LDFLAGS =		\
		-Wl,-version-script=linker-script.ecore \
		-version-info @IOT_VERSION_INFO@

libiot_ecore_la_LIBADD  =		\
		libiot-common.la	\
		$(ECORE_LIBS)

libiot_ecore_la_DEPENDENCIES =	\
		linker-script.ecore	\
		$(filter %.la, $(libiot_ecore_la_LIBADD))

libecoreincludedir      = $(includedir)/iot/ecore
libecoreinclude_HEADERS = $(libiot_ecore_la_HEADERS)
endif

# linker script generation
linker-script.ecore: $(iot_ecore_headers)
	$(QUIET_GEN)$(top_builddir)/build-aux/gen-linker-script -q \
		-P "$(CC)" -c "$(libiot_ecore_la_CFLAGS)" \
		-p '^iot_|^_iot|^mrp_|^_mrp_' -o $@ $^

clean-linker-script::
	-rm -f linker-script.ecore

###################################
# IoT glib glue library
#

iot_glib_headers =			\
		common/glib-glue.h

if GLIB_ENABLED
lib_LTLIBRARIES += libiot-glib.la
EXTRA_DIST      += common/iot-glib.pc
pkgconfig_DATA  += common/iot-glib.pc

libiot_glib_ladir      =		\
		$(includedir)/iot/common

libiot_glib_la_HEADERS = $(iot_glib_headers)

libiot_glib_la_SOURCES =		\
		common/glib-glue.c

libiot_glib_la_CFLAGS  =		\
		$(AM_CFLAGS)		\
		$(GLIB_CFLAGS)

libiot_glib_la_LDFLAGS =		\
		-Wl,-version-script=linker-script.glib \
		-version-info @IOT_VERSION_INFO@

libiot_glib_la_LIBADD  =		\
		libiot-common.la	\
		$(GLIB_LIBS)d

libiot_glib_la_DEPENDENCIES =	\
		linker-script.glib	\
		$(filter %.la, $(libiot_glib_la_LIBADD))

libglibincludedir      = $(includedir)/iot/glib
libglibinclude_HEADERS = $(libiot_glib_la_HEADERS)
endif

# linker script generation
linker-script.glib: $(iot_glib_headers)
	$(QUIET_GEN)$(top_builddir)/build-aux/gen-linker-script -q \
		-P "$(CC)" -c "$(libiot_glib_la_CFLAGS)" \
		-p '^iot_|^_iot|^mrp_|^_mrp_' -o $@ $^

clean-linker-script::
	-rm -f linker-script.glib

###################################
# IoT Qt glue library
#

iot_qt_headers	=		\
		common/qt-glue.h

if QT_ENABLED
lib_LTLIBRARIES += libiot-qt.la
EXTRA_DIST      += common/iot-qt.pc
pkgconfig_DATA  += common/iot-qt.pc

BUILT_SOURCES   += common/qt-glue-priv.moc.h

libiot_qt_ladir      =		\
		$(includedir)/iot/common

libiot_qt_la_HEADERS = $(iot_qt_headers)

libiot_qt_la_SOURCES =		\
		common/qt-glue.cpp

libiot_qt_la_CPPFLAGS  =		\
		$(AM_CFLAGS)		\
		$(QTCORE_CFLAGS)

libiot_qt_la_LDFLAGS =		\
		-Wl,-version-script=linker-script.qt \
		-version-info @IOT_VERSION_INFO@

libiot_qt_la_LIBADD  = $(QTCORE_LIBS)

libiot_qt_la_DEPENDENCIES =		\
		linker-script.qt	\
		$(filter %.la, $(libiot_qt_la_LIBADD))

libqtincludedir      = $(includedir)/iot/qt
libqtinclude_HEADERS = $(libiot_qt_la_HEADERS)

# run moc to generate Qt meta objects
common/qt-glue-priv.moc.h: common/qt-glue-priv.h
	$(QUIET_GEN)$(QT_MOC) -DQT_ENABLED=1 $< -o$@
endif

# linker script generation
linker-script.qt: $(iot_qt_headers)
	$(QUIET_GEN)$(top_builddir)/build-aux/gen-linker-script -q \
		-P "$(CC)" -c "$(libiot_qt_la_CPPFLAGS) -DQT_ENABLED=1" \
		-p '^iot_|^_iot|^mrp_|^_mrp_' -o $@ $^

clean-linker-script::
	-rm -f linker-script.qt