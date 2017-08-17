#include <QProcess>
#include <QTemporaryDir>
#include <QFile>

#include <iostream>
#include <cassert>

#include "embeddedFiles.h"
#include "files.h"


QString extractLunaInstaller(QTemporaryDir &tmpDir)
{
    const QDir destinationDir(tmpDir.path());

	const QString extractedInstallerExecPath = tmpDir.filePath(InstallerExecPath);
	const QString extractedLibNodeDllPath = tmpDir.filePath(LibNodeDllPath);

	const QString installerAppDirPath = QString(":/") + InstallerAppDirName;
    const QDir installerAppDir(installerAppDirPath);

    std::cout << "Extracting files to " << destinationDir.path().toStdString() << std::endl;
	recursiveCopy(installerAppDir, destinationDir);
	extractFromExecutable(EmbeddedFiles::LunaInstallerExec,
						  extractedInstallerExecPath);
	extractFromExecutable(EmbeddedFiles::LibNodeDll,
						  extractedLibNodeDllPath);

	return extractedInstallerExecPath;
}


int main(int argc, char *argv[])
{
    QTemporaryDir tmpDir;
	tmpDir.setAutoRemove(true);
	auto execPath = extractLunaInstaller(tmpDir);
	setExecPermissionsInFiles(tmpDir.filePath(InstallerExecPath),
							  tmpDir.filePath(ConsoleInstallerExecPath),
							  tmpDir.filePath(LibNodeDllPath));

    QProcess process;
    process.start(execPath);
    process.waitForFinished(-1); // -1 means no timeout

    return 0;
}
