#==============================================================================================================
#
# @file     view3d.pro
# @author   Lorenz Esch <lesch@mgh.harvard.edu>
# @since    0.1.6
# @date     August, 2020
#
# @section  LICENSE
#
# Copyright (C) 2020, Lorenz Esch. All rights reserved.
#
# Redistribution and use in source and binary forms, with or without modification, are permitted provided that
# the following conditions are met:
#     * Redistributions of source code must retain the above copyright notice, this list of conditions and the
#       following disclaimer.
#     * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and
#       the following disclaimer in the documentation and/or other materials provided with the distribution.
#     * Neither the name of MNE-CPP authors nor the names of its contributors may be used
#       to endorse or promote products derived from this software without specific prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED
# WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
# PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
# INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
# PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
# HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
# NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
# POSSIBILITY OF SUCH DAMAGE.
#
#
# @brief    This project file generates the makefile for the view3d plugin.
#
#==============================================================================================================

include(../../../../mne-cpp.pri)

TEMPLATE = lib

CONFIG += skip_target_version_ext

CONFIG += plugin

DEFINES += VIEW3D_PLUGIN

QT += gui widgets 3dextras

contains(MNECPP_CONFIG, wasm) {
    DEFINES += WASMBUILD
}

TARGET = view3d
CONFIG(debug, debug|release) {
    TARGET = $$join(TARGET,,,d)
}

DESTDIR = $${MNE_BINARY_DIR}/mne_analyze_plugins

contains(MNECPP_CONFIG, static) {
    CONFIG += staticlib
    DEFINES += STATICBUILD
} else {
    CONFIG += shared
}

LIBS += -L$${MNE_LIBRARY_DIR}
CONFIG(debug, debug|release) {
    LIBS += -lanSharedd \
            -lmnecppDisp3Dd \
            -lmnecppDispd \
            -lmnecppConnectivityd \
            -lmnecppRtProcessingd \
            -lmnecppInversed \
            -lmnecppFwdd \
            -lmnecppMned \
            -lmnecppFiffd \
            -lmnecppFsd \
            -lmnecppUtilsd \
} else {
    LIBS += -lanShared \
            -lmnecppDisp3D \
            -lmnecppDisp \
            -lmnecppConnectivity \
            -lmnecppRtProcessing \
            -lmnecppInverse \
            -lmnecppFwd \
            -lmnecppMne \
            -lmnecppFiff \
            -lmnecppFs \
            -lmnecppUtils \
}

SOURCES += \
    view3d.cpp \

HEADERS += \
    view3d_global.h \
    view3d.h \

FORMS += \

OTHER_FILES += view3d.json

RESOURCES += \

RESOURCE_FILES +=\

# Copy resource files from repository to bin resource folder
COPY_CMD = $$copyResources($${RESOURCE_FILES})
QMAKE_POST_LINK += $${COPY_CMD}

# Put generated form headers into the origin --> cause other src is pointing at them
UI_DIR = $${PWD}

INCLUDEPATH += $${EIGEN_INCLUDE_DIR}
INCLUDEPATH += $${MNE_INCLUDE_DIR}
INCLUDEPATH += $${MNE_ANALYZE_INCLUDE_DIR}

# Install headers to include directory
header_files.files = $${HEADERS}
header_files.path = $${MNE_INSTALL_INCLUDE_DIR}/mne_analyze_plugins

# suppress visibility warnings
unix: QMAKE_CXXFLAGS += -Wno-attributes

unix:!macx {
    # === Unix ===
    QMAKE_RPATHDIR += $ORIGIN/../../lib
}

# Activate FFTW backend in Eigen for non-static builds only
contains(MNECPP_CONFIG, useFFTW):!contains(MNECPP_CONFIG, static) {
    DEFINES += EIGEN_FFTW_DEFAULT
    INCLUDEPATH += $$shell_path($${FFTW_DIR_INCLUDE})
    LIBS += -L$$shell_path($${FFTW_DIR_LIBS})

    win32 {
        # On Windows
        LIBS += -llibfftw3-3 \
                -llibfftw3f-3 \
                -llibfftw3l-3 \
    }

    unix:!macx {
        # On Linux
        LIBS += -lfftw3 \
                -lfftw3_threads \
    }
}