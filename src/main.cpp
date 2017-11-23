#include <QProcess>
#include <QTemporaryDir>
#include <QFile>
#include <QMessageBox>
#include <QApplication>

#include <cassert>
#include <iostream>

#include "embeddedFiles.h"
#include "files.h"

QString extractLunaInstaller(QTemporaryDir &tmpDir)
{
    const QDir destinationDir(tmpDir.path());

	const QString extractedInstallerExecPath = tmpDir.path() + "/" + InstallerExecPath;
	const QString extractedLibNodeDllPath = tmpDir.path() + "/" + LibNodeDllPath;
    const QString extractedConsoleInstallerExecPath = tmpDir.path() + "/" + ConsoleInstallerExecPath;

	const QString installerAppDirPath = QString(":/") + InstallerAppDirName;
    const QDir installerAppDir(installerAppDirPath);

	recursiveCopy(installerAppDir, destinationDir);
	extractFromExecutable(EmbeddedFiles::LunaInstallerExec,
						  extractedInstallerExecPath);
	extractFromExecutable(EmbeddedFiles::LibNodeDll,
						  extractedLibNodeDllPath);
    extractFromExecutable(EmbeddedFiles::ConsoleInstallerExec,
                          extractedConsoleInstallerExecPath);

	return extractedInstallerExecPath;
}

bool extractLibraries(QTemporaryDir &tmpDir)
{
	const QDir destinationDir(tmpDir.path());

	const QString librariesPath = QString(":/") + LibrariesDirName;
	const QDir librariesDir(librariesPath);
	if (librariesDir.exists())
	{
		recursiveCopy(librariesDir, destinationDir);
		return true;
	}
	else
		return false;
}


void setupLibraries(QTemporaryDir &tmpDir, QProcess &process)
{
	const bool thereAreLibraries = extractLibraries(tmpDir);
	const QString librariesPath = tmpDir.path() + "/" + LibrariesDirName;
#ifdef __linux__
	if (thereAreLibraries) {
		auto env = QProcessEnvironment::systemEnvironment();
//		env.insert("LD_DEBUG", "libs");
		env.insert("LD_LIBRARY_PATH", librariesPath + ":" + env.value("LD_LIBRARY_PATH"));
		process.setProcessEnvironment(env);
	}
#else
	assert(!thereAreLibraries);
#endif
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
		setExecPermissionsInFiles(tmpDir.path() + "/" + InstallerExecPath,
								  tmpDir.path() + "/" + ConsoleInstallerExecPath,
								  tmpDir.path() + "/" + LibNodeDllPath);

		QProcess process;
		setupLibraries(tmpDir, process);
		process.start(execPath);
		process.waitForFinished(-1); // -1 means no timeout
		int exitCode = process.exitCode();
		if (exitCode)
		{
			throw InstallerProcessError(process.readAllStandardError().toStdString());
		}
		std::cout << process.readAllStandardError().toStdString() << std::endl;
		return 0;
	}
	catch(std::runtime_error &e)
	{
        std::cerr << "Error during loading electron application: " << std::endl;
        std::cerr << e.what() << std::endl;
		QMessageBox::critical(nullptr, "Luna installer", QString("Error when loading installer: ") + e.what());
	}

	return -1;
}
