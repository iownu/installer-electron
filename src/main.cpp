#include <QProcess>
#include <QTemporaryDir>
#include <QFile>

#include <iostream>
#include <cassert>

const auto InstallerAppDirName = "lunaInstallerApp";

// these path are relative to qrc root or temporary directory
const auto InstallerExecPath = "lunaInstallerApp/lunaInstaller";
const auto ConsoleInstallerExecPath = "lunaInstallerApp/resources/app/consoleInstaller";
const auto LibNodeDllPath = "lunaInstallerApp/libnode.so";

enum class EmbeddedFiles {
	LunaInstallerExec, LibNodeDll
};

#ifndef _WIN32
extern char _binary_lunaInstaller_start[];
extern char _binary_lunaInstaller_end[];
extern char _binary_libnode_so_start[];
extern char _binary_libnode_so_end[];

const std::map<EmbeddedFiles, std::pair<char *, char *>> pointersToEmbeddedFile =
{
	{ EmbeddedFiles::LunaInstallerExec, { _binary_lunaInstaller_start, _binary_lunaInstaller_end } },
	{ EmbeddedFiles::LibNodeDll, { _binary_libnode_so_start, _binary_libnode_so_end } }
};
#endif

#define RUNTIME_ERROR(Name,description)                                     \
    struct Name : public std::runtime_error                                 \
    {                                                                       \
        static constexpr const auto Description = description;              \
        Name() : std::runtime_error(Description) {}                         \
        bool operator==(const Name &rhs) const noexcept { return true; }    \
    }


RUNTIME_ERROR(CreateTemporaryDirectoryError, "Could not create temporary directory");
RUNTIME_ERROR(ExtractingFileError, "Error while extracting luna installer files to temporary directory");
RUNTIME_ERROR(ExtractingDirectoryError, "Error while extracting luna installer directory to temporary directory");
RUNTIME_ERROR(ExtractingFromExecError, "Error while extracting embedded file");
RUNTIME_ERROR(SettingPermissionError, "Error while setting execute permission on extracted luna installer");


void recursiveCopy(const QDir &copiedDir, const QDir &destination)
{
    assert(copiedDir.exists());

    if (!destination.mkdir(copiedDir.dirName()))
		throw ExtractingDirectoryError();
    QDir newDir(destination.absoluteFilePath(copiedDir.dirName()));

    for (auto &file : copiedDir.entryInfoList(QDir::Files | QDir::Dirs | QDir::NoDotAndDotDot | QDir::Hidden | QDir::System))
    {
        if (file.isDir())
            recursiveCopy(QDir(file.filePath()), newDir);
        else if (file.isFile())
        {
            if (!QFile::copy(file.absoluteFilePath(), newDir.absoluteFilePath(file.fileName())))
                throw ExtractingFileError();
        }
    }
}


void extractFromExecutable(EmbeddedFiles fileId, const QString &fileName)
{
    QFile file(fileName);
#ifdef _WIN32
    static_assert(false, "Michale, wymyśl tutaj coś mądrego");
#else

    if (!file.open(QFile::WriteOnly))
		throw ExtractingFromExecError();

	auto dataPtrs = pointersToEmbeddedFile.at(fileId);
	file.write(dataPtrs.first, dataPtrs.second - dataPtrs.first);
#endif
}

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

template<class... Paths>
void setExecPermissionsInFiles(QTemporaryDir &tmpDir, Paths... paths)
{
	auto setExecPermission = [&](auto path)
	{
		QFile file(tmpDir.filePath(path));
		if (!file.setPermissions(QFileDevice::ReadUser | QFileDevice::ExeUser))
			throw SettingPermissionError();
	};
	bool dummy[] = { (setExecPermission(paths), true)... };
}


int main(int argc, char *argv[])
{
    QTemporaryDir tmpDir;
    tmpDir.setAutoRemove(false);
	auto execPath = extractLunaInstaller(tmpDir);
	setExecPermissionsInFiles(tmpDir, InstallerExecPath, ConsoleInstallerExecPath, LibNodeDllPath);

    QProcess process;
    process.start(execPath);
    process.waitForFinished(-1); // -1 means no timeout

    return 0;
}
