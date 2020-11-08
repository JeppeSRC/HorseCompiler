#include "util.h"
#include <core/error/error.h>
#include <math.h>

uint64 GetValue(const char c) {
	return c - (48 * (c >= '0' && c <= '9')) - (55 * (c >= 'A' && c <= 'F')) - (87 * (c >= 'a' && c <= 'f'));
}

uint64 StringUtils::ToUint64(const String& string, uint8 base, uint64 start, uint64 end) {
	HC_ASSERT(base != 0);
	HC_ASSERT(start <= end);

	if (end == -1) end = string.length - 1;

	HC_ASSERT(start >= 0 && end < string.length);

	uint64 value = 0;

	for (uint64 i = start; i <= end; i++) {
		value += GetValue(string[end + start - i]) * (uint64)pow((double)base, (double)(i - start));
	}

	return value;
}

uint64 StringUtils::ToUint64(const char* const string, uint8 base, uint64 start, uint64 end) {
	return ToUint64(TmpString(string), base, start, end);
}

void StringUtils::ReplaceChar(String& string, char oldChar, char newChar) {
	uint64 index = 0;

	while ((index = string.Find(oldChar, index)) != String::npos) {
		string[index] = newChar;
	}
}

String StringUtils::GetPathFromFilename(String filename) {
	ReplaceChar(filename, '\\', '/');

	uint64 slash = filename.FindR('/', 0);

	if (slash != String::npos) {
		return filename.Remove(slash + 1, filename.length - 1);
	}

	return "";
}