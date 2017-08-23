#pragma once

#include <QString>

enum class EmbeddedFiles {
    LunaInstallerExec, LibNodeDll, ConsoleInstallerExec
};

void extractFromExecutable(EmbeddedFiles fileId, const QString &fileName);
