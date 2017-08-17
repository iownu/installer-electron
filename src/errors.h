#pragma once

#include <string>
#include <sstream>

#define RUNTIME_ERROR(Name,description)                                     \
	struct Name : public std::runtime_error                                 \
	{                                                                       \
		static constexpr const auto Description = description;              \
		Name() : std::runtime_error(Description) {}                         \
		bool operator==(const Name &rhs) const noexcept { return true; }    \
	}

#define RUNTIME_ERROR_PARAM(Name,description)											\
	struct Name : public std::runtime_error												\
	{																					\
		static constexpr const auto Description = description;							\
		template<class T>																\
		explicit Name(T &&param)														\
			: std::runtime_error(makeDescription(std::forward<T>(param))) {}	    	\
																						\
		bool operator==(const Name &rhs) const noexcept { return true; }				\
																						\
	private:																			\
		template<class T>																\
		std::string makeDescription(T &&param)											\
		{																				\
			std::ostringstream os;														\
			os << Description << " (" << std::forward<T>(param) << ")";					\
			return os.str();															\
		}																				\
	}

RUNTIME_ERROR(CreateTemporaryDirectoryError, "Could not create temporary directory");
RUNTIME_ERROR_PARAM(ExtractingFileError, "Error while extracting luna installer files to temporary directory");
RUNTIME_ERROR_PARAM(ExtractingDirectoryError, "Error while extracting luna installer directory to temporary directory");
RUNTIME_ERROR_PARAM(ExtractingFromExecError, "Error while extracting embedded file");
RUNTIME_ERROR_PARAM(SettingPermissionError, "Error while setting execute permission on extracted luna installer");
RUNTIME_ERROR_PARAM(InstallerProcessError, "Luna installer finishes unsuccessfully\n\nError log:\n");
