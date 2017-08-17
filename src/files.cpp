#include "files.h"

#include <cassert>

#include "errors.h"

void recursiveCopy(const QDir &copiedDir, const QDir &destination)
{
	assert(copiedDir.exists());

	if (!destination.mkdir(copiedDir.dirName()))
		throw ExtractingDirectoryError(copiedDir.absolutePath().toStdString());
	QDir newDir(destination.absoluteFilePath(copiedDir.dirName()));

	for (auto &file : copiedDir.entryInfoList(QDir::Files | QDir::Dirs | QDir::NoDotAndDotDot | QDir::Hidden | QDir::System))
	{
		if (file.isDir())
			recursiveCopy(QDir(file.filePath()), newDir);
		else if (file.isFile())
		{
			if (!QFile::copy(file.absoluteFilePath(), newDir.absoluteFilePath(file.fileName())))
				throw ExtractingFileError(file.absoluteFilePath().toStdString());
		}
	}
}
