#include "embeddedFiles.h"

#include <map>
#include <QFile>

#include "errors.h"

#ifdef __MINGW32__
extern char _binary_electronLunaInstaller_exe_start[];
extern char _binary_electronLunaInstaller_exe_end[];
extern char _binary_node_dll_start[];
extern char _binary_node_dll_end[];
extern char _binary_consoleInstaller_exe_start[];
extern char _binary_consoleInstaller_exe_end[];

const std::map<EmbeddedFiles, std::pair<char *, char *>> pointersToEmbeddedFile =
{
	{ EmbeddedFiles::LunaInstallerExec, { _binary_electronLunaInstaller_exe_start, _binary_electronLunaInstaller_exe_end } },
    { EmbeddedFiles::LibNodeDll, { _binary_node_dll_start, _binary_node_dll_end } },
    { EmbeddedFiles::ConsoleInstallerExec, { _binary_consoleInstaller_exe_start, _binary_consoleInstaller_exe_end } }
};

#else
extern char _binary_electronLunaInstaller_start[];
extern char _binary_electronLunaInstaller_end[];
extern char _binary_libnode_so_start[];
extern char _binary_libnode_so_end[];
extern char _binary_consoleInstaller_start[];
extern char _binary_consoleInstaller_end[];

const std::map<EmbeddedFiles, std::pair<char *, char *>> pointersToEmbeddedFile =
{
	{ EmbeddedFiles::LunaInstallerExec, { _binary_electronLunaInstaller_start, _binary_electronLunaInstaller_end } },
    { EmbeddedFiles::LibNodeDll, { _binary_libnode_so_start, _binary_libnode_so_end } },
    { EmbeddedFiles::ConsoleInstallerExec, { _binary_consoleInstaller_start, _binary_consoleInstaller_end } }
};

#endif




void extractFromExecutable(EmbeddedFiles fileId, const QString &fileName)
{
	QFile file(fileName);

	if (!file.open(QFile::WriteOnly))
		throw ExtractingFromExecError(fileName.toStdString());

	auto dataPtrs = pointersToEmbeddedFile.at(fileId);
	file.write(dataPtrs.first, dataPtrs.second - dataPtrs.first);
}
