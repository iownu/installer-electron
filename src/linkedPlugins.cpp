#include <QtPlugin>

#ifdef __linux__
Q_IMPORT_PLUGIN(QXcbIntegrationPlugin)
Q_IMPORT_PLUGIN(QXcbEglIntegrationPlugin)
#endif
