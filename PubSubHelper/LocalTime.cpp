#include "pch.h"
#include "LocalTime.h"

std::string GetLocalTime()
{
	char str[32]{ 0 };

	time_t a = time(nullptr);

	struct tm time_info;

	// localtime_s, Microsoft version (returns zero on success, an error code on failure)

	if (localtime_s(&time_info, &a) == 0)
		strftime(str, sizeof(str), "%H:%M:%S", &time_info);

	return str;
}