#define STRICT
#ifdef WITH_MAIN_FUNCTION
#include <stdio.h>
#else
#define WIN32_LEAN_AND_MEAN
#endif
#define UNICODE
#define _UNICODE
#include <windows.h>
#include <stdlib.h>

#ifdef _DEBUG
static void print_error(LPCWSTR prefix, DWORD error_number)
{
	LPWSTR buffer = NULL;
	DWORD count = 0;

	count = FormatMessageW(FORMAT_MESSAGE_ALLOCATE_BUFFER
			| FORMAT_MESSAGE_FROM_SYSTEM
			| FORMAT_MESSAGE_IGNORE_INSERTS,
			NULL, error_number, LANG_NEUTRAL,
			(LPTSTR)&buffer, 0, NULL);
	if (count < 1)
		count = FormatMessageW(FORMAT_MESSAGE_ALLOCATE_BUFFER
				| FORMAT_MESSAGE_FROM_STRING
				| FORMAT_MESSAGE_ARGUMENT_ARRAY,
				L"Code 0x%1!08x!",
				0, LANG_NEUTRAL, (LPTSTR)&buffer, 0,
				(va_list*)&error_number);
	MessageBox(NULL, buffer, prefix, MB_OK);
	LocalFree((HLOCAL)buffer);
}
#endif

WINAPI __declspec(dllexport)
int edit_git_bash(LPWSTR git_bash_path, LPWSTR new_command_line)
{
	HANDLE handle;
	int len, alloc, result = 0;
	WCHAR *buffer;

	len = wcslen(new_command_line);
	alloc = 2 * (len + 16);
	buffer = calloc(alloc, 1);
	if (!buffer)
		return 1;
	buffer[0] = (WCHAR) len;
	memcpy(buffer + 1, new_command_line, 2 * len);

	if (!(handle = BeginUpdateResource(git_bash_path,TRUE)))
		return 2;

        if (!UpdateResource(handle, RT_STRING, MAKEINTRESOURCE(1),
			MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
			buffer, alloc))
		result = 3;
        if (!EndUpdateResource(handle, FALSE))
		return 4;

	return result;
}

#ifdef WITH_MAIN_FUNCTION
int main()
{
	LPWSTR command_line = GetCommandLineW();
	int argc, result;
	LPWSTR *argv;

	argv = CommandLineToArgvW(command_line, &argc);
	if (argc != 3) {
		fwprintf(stderr, L"Usage: %s <exe> <command-line>\n", argv[0]);
		exit(1);
	}

	result = edit_git_bash(argv[1], argv[2]);

	if (result)
		fwprintf(stderr, L"Error code %d\n", result);

	return !!result;
}
#endif
