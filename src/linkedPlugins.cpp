#include <QtPlugin>

#ifdef __linux__
Q_IMPORT_PLUGIN(QXcbIntegrationPlugin)
//Q_IMPORT_PLUGIN(QXcbEglIntegrationPlugin)
#elif _WIN32 || __MINGW32__
Q_IMPORT_PLUGIN(QWindowsIntegrationPlugin)
#endif
