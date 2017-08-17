#include <QProcess>
#include <QTemporaryDir>
#include <QFile>
#include <QMessageBox>
#include <QApplication>

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

	recursiveCopy(installerAppDir, destinationDir);
	extractFromExecutable(EmbeddedFiles::LunaInstallerExec,
						  extractedInstallerExecPath);
	extractFromExecutable(EmbeddedFiles::LibNodeDll,
						  extractedLibNodeDllPath);

	return extractedInstallerExecPath;
}


int main(int argc, char *argv[])
{
	QApplication app(argc, argv);
	try
	{
		QTemporaryDir tmpDir;
		tmpDir.setAutoRemove(true);
		if (!tmpDir.isValid())
			throw CreateTemporaryDirectoryError();

		auto execPath = extractLunaInstaller(tmpDir);
		setExecPermissionsInFiles(tmpDir.filePath(InstallerExecPath),
								  tmpDir.filePath(ConsoleInstallerExecPath),
								  tmpDir.filePath(LibNodeDllPath));

		QProcess process;
		process.start(execPath);
		process.waitForFinished(-1); // -1 means no timeout
		int exitCode = process.exitCode();
		if (exitCode)
		{
			throw InstallerProcessError(process.readAllStandardError().toStdString());
		}
		return 0;
	}
	catch(std::runtime_error &e)
	{
		QMessageBox::critical(nullptr, "Luna installer", QString("Error when loading installer: ") + e.what());
	}

	return -1;
}
