#include <QProcess>
#include <QTemporaryDir>
#include <QFile>

#include <iostream>
#include <cassert>

#define RUNTIME_ERROR(Name,description)                                     \
    struct Name : public std::runtime_error                                 \
    {                                                                       \
        static constexpr const auto Description = description;              \
        Name() : std::runtime_error(description) {}                         \
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
            recursiveCopy(file.dir(), newDir);
        else if (file.isFile())
        {
            if (!QFile::copy(file.absoluteFilePath(), newDir.absoluteFilePath(file.fileName())))
                throw ExtractingFileError();
        }
    }
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

    QFile execFile(execPath);
    if (!execFile.setPermissions(QFileDevice::ReadUser | QFileDevice::ExeUser))
        throw SettingPermissionError();
    return execPath;
}



int main(int argc, char *argv[])
{
    QTemporaryDir tmpDir;
    tmpDir.setAutoRemove(true);
    auto path = extractLunaInstaller(tmpDir, "lunaInstallerApp", "lunaInstallerApp/lunaInstaller");


    return 0;
}
