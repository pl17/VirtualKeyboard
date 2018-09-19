#pragma once

//Fix string.h
extern "C"
{
	void* memset(void *, int, size_t);
	int memcpy(void*, const void*, size_t);
	int memcmp(const void*, const void*, size_t);
	int strlen(const char*);
	wchar_t* wcscpy(wchar_t*, const wchar_t*);
	int wcslen(const wchar_t*);
	int _stricmp(const char*, const char*);
}
