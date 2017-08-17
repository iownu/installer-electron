#include "embeddedFiles.h"

#include <map>
#include <QFile>

#include "errors.h"

extern char _binary_lunaInstaller_start[];
extern char _binary_lunaInstaller_end[];
extern char _binary_libnode_so_start[];
extern char _binary_libnode_so_end[];

const std::map<EmbeddedFiles, std::pair<char *, char *>> pointersToEmbeddedFile =
{
	{ EmbeddedFiles::LunaInstallerExec, { _binary_lunaInstaller_start, _binary_lunaInstaller_end } },
	{ EmbeddedFiles::LibNodeDll, { _binary_libnode_so_start, _binary_libnode_so_end } }
};

void extractFromExecutable(EmbeddedFiles fileId, const QString &fileName)
{
	QFile file(fileName);

	if (!file.open(QFile::WriteOnly))
		throw ExtractingFromExecError(fileName.toStdString());

	auto dataPtrs = pointersToEmbeddedFile.at(fileId);
	file.write(dataPtrs.first, dataPtrs.second - dataPtrs.first);
}
