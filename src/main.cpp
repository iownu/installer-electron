#include <QProcess>
#include <QTemporaryDir>
#include <QFile>

#include <iostream>
#include <cassert>

#ifndef _WIN32
extern char _binary_lunaInstaller_start[];
extern char _binary_lunaInstaller_end[];
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
RUNTIME_ERROR(ExtractingDirecotryError, "Error while extracting luna installer directory to temporary directory");
RUNTIME_ERROR(SettingPermissionError, "Error while setting execute permission on extracted luna installer");


void recursiveCopy(const QDir &copiedDir, const QDir &destination)
{
    assert(copiedDir.exists());

    if (!destination.mkdir(copiedDir.dirName()))
        throw ExtractingDirecotryError();
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


void extractExecutable(const char *fileName)
{
    QFile file(fileName);
#ifdef _WIN32
    static_assert(false, "Michale, wymyśl tutaj coś mądrego");
#else
    if (!file.open(QFile::WriteOnly))
        throw ExtractingFileError();

    file.write(_binary_lunaInstaller_start, _binary_lunaInstaller_end - _binary_lunaInstaller_start);
#endif
}

QString extractLunaInstaller(QTemporaryDir &tmpDir,
                             const char *installerAppDirName,
                             const char *installerExecPath)
{

    const QDir destinationDir(tmpDir.path());
    const QString execPath = tmpDir.filePath(installerExecPath);
    const QString installerAppDirPath = QString(":/") + installerAppDirName;
    const QDir installerAppDir(installerAppDirPath);

    std::cout << "Extracting files to " << destinationDir.path().toStdString() << std::endl;
    recursiveCopy(installerAppDir, destinationDir);
    extractExecutable(execPath.toStdString().c_str());

    return execPath;
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
    auto execPath = extractLunaInstaller(tmpDir, "lunaInstallerApp", "lunaInstallerApp/lunaInstaller");
	setExecPermissionsInFiles(tmpDir, "lunaInstallerApp/lunaInstaller", "lunaInstallerApp/resources/app/consoleInstaller");

    QProcess process;
    process.start(execPath);
    process.waitForFinished(-1); // -1 means no timeout

    return 0;
}
