if (APPLE)
    set(_name macdeployqt)
elseif (WIN32)
    set(_name windeployqt)
else()
    set(_name linuxdeployqt)
endif()

find_program(
    QT_DEPLOY
    NAMES ${_name}
    PATH_SUFFIXES bin
    PATHS
        ENV QTDIR
        ENV QT_DIR
        ENV QT5_DIR
)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(QtDeploy DEFAULT_MSG QT_DEPLOY)
mark_as_advanced(QT_DEPLOY)
