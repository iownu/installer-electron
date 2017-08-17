#pragma once

#include <QString>

enum class EmbeddedFiles {
	LunaInstallerExec, LibNodeDll
};

void extractFromExecutable(EmbeddedFiles fileId, const QString &fileName);
