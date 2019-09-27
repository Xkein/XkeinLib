#pragma once

#include <Windows.h>
#include "tools.h"
#include "String.h"

XKEINNAMESPACE_START

	class File
	{
	protected:
		STATEMENTS(the function code is mainly come from the below
			https://www.cnblogs.com/02xiaoma/archive/2012/11/11/2764904.html
			http://www.m5home.com/bbs/thread-9291-1-1.html
			https://blog.csdn.net/baggiowangyu/article/details/6424874
		)
		virtual DWORD GetFullName() _NOEXCEPT
		{
			typedef long NTSTATUS;

			// Define the base asynchronous I/O argument types
			typedef struct _IO_STATUS_BLOCK
			{
				union
				{
					NTSTATUS Status;
					PVOID    Pointer;
				};

				ULONG_PTR Information;
			} IO_STATUS_BLOCK, *PIO_STATUS_BLOCK;

			typedef enum _FILE_INFORMATION_CLASS
			{
				// end_wdm
				FileDirectoryInformation = 1,
				FileFullDirectoryInformation,   // 2
				FileBothDirectoryInformation,   // 3
				FileBasicInformation,           // 4  wdm
				FileStandardInformation,        // 5  wdm
				FileInternalInformation,        // 6
				FileEaInformation,              // 7
				FileAccessInformation,          // 8
				FileNameInformation,            // 9
				FileRenameInformation,          // 10
				FileLinkInformation,            // 11
				FileNamesInformation,           // 12
				FileDispositionInformation,     // 13
				FilePositionInformation,        // 14 wdm
				FileFullEaInformation,          // 15
				FileModeInformation,            // 16
				FileAlignmentInformation,       // 17
				FileAllInformation,             // 18
				FileAllocationInformation,      // 19
				FileEndOfFileInformation,       // 20 wdm
				FileAlternateNameInformation,   // 21
				FileStreamInformation,          // 22
				FilePipeInformation,            // 23
				FilePipeLocalInformation,       // 24
				FilePipeRemoteInformation,      // 25
				FileMailslotQueryInformation,   // 26
				FileMailslotSetInformation,     // 27
				FileCompressionInformation,     // 28
				FileObjectIdInformation,        // 29
				FileCompletionInformation,      // 30
				FileMoveClusterInformation,     // 31
				FileQuotaInformation,           // 32
				FileReparsePointInformation,    // 33
				FileNetworkOpenInformation,     // 34
				FileAttributeTagInformation,    // 35
				FileTrackingInformation,        // 36
				FileIdBothDirectoryInformation, // 37
				FileIdFullDirectoryInformation, // 38
				FileValidDataLengthInformation, // 39
				FileShortNameInformation,       // 40
				FileMaximumInformation
				// begin_wdm
			} FILE_INFORMATION_CLASS, *PFILE_INFORMATION_CLASS;

			typedef struct _UNICODE_STRING
			{
				USHORT Length;
				USHORT MaximumLength;
#ifdef MIDL_PASS
				[size_is(MaximumLength / 2), length_is((Length) / 2)] USHORT * Buffer;
#else // MIDL_PASS
				PWSTR  Buffer;
#endif // MIDL_PASS
			} UNICODE_STRING, *PUNICODE_STRING;

			typedef struct _OBJECT_NAME_INFORMATION
			{
				UNICODE_STRING Name;
			} OBJECT_NAME_INFORMATION, *POBJECT_NAME_INFORMATION;

			typedef NTSTATUS(NTAPI *PNTQUERYINFORMATIONFILE)(
				IN  HANDLE                 FileHandle,
				OUT PIO_STATUS_BLOCK       IoStatusBlock,
				OUT PVOID                  FileInformation,
				IN  DWORD                  Length,
				IN  FILE_INFORMATION_CLASS FileInformationClass
				);

			typedef long(__stdcall*PNtQueryObject)(HANDLE ObjectHandle, ULONG ObjectInformationClass, PVOID ObjectInformation, ULONG ObjectInformationLength, PULONG ReturnLength);

			PNtQueryObject NtQueryObject(reinterpret_cast<PNtQueryObject>(GetProcAddress(GetModuleHandleW(L"ntdll.dll"), "NtQueryObject")));
			OBJECT_NAME_INFORMATION name, *pname; ULONG len;
			NtQueryObject(hFile, 1, &name, sizeof name, &len);
			pname = reinterpret_cast<POBJECT_NAME_INFORMATION>(malloc(len));
			NtQueryObject(hFile, 1, pname, len, &len);

			PNTQUERYINFORMATIONFILE NtQueryInformationFile(reinterpret_cast<PNTQUERYINFORMATIONFILE>(GetProcAddress(GetModuleHandleW(L"ntdll.dll"), "NtQueryInformationFile")));
			NTSTATUS status = -1;
			IO_STATUS_BLOCK IoStatus = { 0 };
			POBJECT_NAME_INFORMATION pfni = NULL;

			if (hFile != INVALID_HANDLE_VALUE)
			{
				constexpr size_t allocSize = sizeof(OBJECT_NAME_INFORMATION) + MAX_PATH * sizeof(WCHAR);
				pfni = (POBJECT_NAME_INFORMATION)malloc(allocSize);
				if (pfni != NULL)
				{
					RtlZeroMemory(pfni, allocSize);
					status = NtQueryInformationFile(hFile, &IoStatus, pfni, allocSize, FileNameInformation);
					if (!((NTSTATUS)(status) >= 0)) {
						free(pfni);
						return GetLastError();
					}
				}
			}

			auto VolumeDeviceNameToDosName = [](IN LPCWSTR DeviceName, OUT LPWSTR DosName) -> DWORD
			{
				typedef struct _MOUNTDEV_NAME {
					USHORT  NameLength;
					WCHAR   Name[1];
				} MOUNTDEV_NAME, *PMOUNTDEV_NAME;

				typedef struct _MOUNTMGR_VOLUME_PATHS {
					ULONG MultiSzLength;
					WCHAR MultiSz[1];
				} MOUNTMGR_VOLUME_PATHS, *PMOUNTMGR_VOLUME_PATHS;

				PMOUNTDEV_NAME MountName = NULL;
				CHAR buffer1[512] = { 0 };
				CHAR buffer2[512] = { 0 };
				HANDLE MountPointMgr = NULL;
				DWORD BytesReturned = 0;

				MountPointMgr = CreateFileW(
					L"\\\\.\\MountPointManager",
					0,
					FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
					NULL,
					OPEN_EXISTING,
					0,
					NULL
				);
				if (MountPointMgr == INVALID_HANDLE_VALUE)
					return GetLastError();

				MountName = (PMOUNTDEV_NAME)buffer1;
				MountName->NameLength = (USHORT)wcslen(DeviceName) * 2;
				wcscpy_s(MountName->Name, MountName->NameLength, DeviceName);

				DeviceIoControl(
					MountPointMgr,
					CTL_CODE(((ULONG) 'm'), 12, METHOD_BUFFERED, FILE_ANY_ACCESS),
					MountName,
					512,
					buffer2,
					512,
					&BytesReturned,
					NULL
				);
				if (!GetLastError())
					wcscpy_s(DosName, 3, ((PMOUNTMGR_VOLUME_PATHS)buffer2)->MultiSz);

				CloseHandle(MountPointMgr);
				return GetLastError();
			};

			std::wstring pathNT(pname->Name.Buffer);
			std::wstring path((PWSTR)&pfni->Name.Buffer);
			pathNT.erase(pathNT.find(path));

			WCHAR pDosName[3]{ 0 };
			DWORD ret = VolumeDeviceNameToDosName(pathNT.c_str(), pDosName);
			path.insert(0, pDosName);

			fileFullName = path.c_str();

			free(pfni);
			free(pname);


			return ret;
		}

	public:
		File() _NOEXCEPT : hFile(INVALID_HANDLE_VALUE), pBuffer(NULL)
		{ }

		~File() _NOEXCEPT
		{
			Close();
		}

		virtual BOOL Open(LPCSTR name, DWORD dwDesiredAccess = GENERIC_READ | GENERIC_WRITE,
			DWORD dwShareMode = FILE_SHARE_READ,
			DWORD dwFlagsAndAttributes = FILE_ATTRIBUTE_NORMAL) _NOEXCEPT
		{
			if ((hFile = CreateFileA(fileName = name, dwDesiredAccess, dwShareMode,
				NULL, OPEN_ALWAYS, dwFlagsAndAttributes, NULL)) == NULL)
			{
				return FALSE;
			}
			GetFullName();

			if (dwDesiredAccess & GENERIC_WRITE) {
				Backup();
			}

			return GetFileInformationByHandle(hFile, &fileInformation);
		}

		virtual BOOL Open(LPCWSTR name, DWORD dwDesiredAccess = GENERIC_READ | GENERIC_WRITE,
			DWORD dwShareMode = FILE_SHARE_READ,
			DWORD dwFlagsAndAttributes = FILE_ATTRIBUTE_NORMAL) _NOEXCEPT
		{
			if ((hFile = CreateFileW(fileName = name, dwDesiredAccess, dwShareMode,
				NULL, OPEN_ALWAYS, dwFlagsAndAttributes, NULL)) == NULL)
			{
				return FALSE;
			}
			GetFullName();

			if (dwDesiredAccess & GENERIC_WRITE) {
				Backup();
			}

			return GetFileInformationByHandle(hFile, &fileInformation);
		}

		virtual void Close() _NOEXCEPT
		{
			CloseHandle(hFile);
			hFile = INVALID_HANDLE_VALUE;
			GetFileInformation();
			fileName.CleanUp();
			fileFullName.CleanUp();
		}

		virtual const BY_HANDLE_FILE_INFORMATION* GetFileInformation() const _NOEXCEPT
		{
			return &fileInformation;
		}

		virtual BOOL Read(DWORD luNumberOfBytesToRead, LPVOID buffer = NULL) _NOEXCEPT
		{
			if (buffer == NULL) {
				buffer = malloc(luNumberOfBytesToRead + 1);
			}
			return ReadFile(hFile, pBuffer = buffer, luNumberOfBytesToRead, &luNumberOfBytesToRead, NULL);
		}

		virtual DWORD SetPointer(LONG lDistanceToMove, PLONG lpDistanceToMoveHigh = NULL, DWORD dwMoveMethod = FILE_CURRENT) _NOEXCEPT
		{
			return SetFilePointer(hFile, lDistanceToMove, lpDistanceToMoveHigh, dwMoveMethod);
		}

		virtual LONGLONG GetPointer() _NOEXCEPT
		{
			union {
				LONG long_part[2];
				LONGLONG llong_part;
			} ptr;
			ptr.long_part[0] = SetPointer(0, &ptr.long_part[1]);
			return ptr.llong_part;
		}

		virtual DWORD GetType() _NOEXCEPT
		{
			return GetFileType(hFile);
		}

		virtual DWORD GetSize(LPDWORD lpFileSizeHigh = NULL) _NOEXCEPT
		{
			return GetFileSize(hFile, lpFileSizeHigh);
		}

		virtual BOOL Write(LPCVOID lpBuffer, DWORD dwNumberOfBytesToWrite) _NOEXCEPT
		{
			return WriteFile(hFile, pBuffer = const_cast<LPVOID>(lpBuffer), dwNumberOfBytesToWrite, &dwNumberOfBytesToWrite, NULL);
		}

		virtual LPCVOID GetBuffer() _NOEXCEPT
		{
			return pBuffer;
		}

		virtual BOOL Backup() const _NOEXCEPT
		{
			size_t fileNameLength = wcslen(fileName);
			constexpr WCHAR backup[] = L"_b";
			PWCHAR buf = (PWCHAR)malloc(fileNameLength * sizeof(WCHAR) + sizeof(backup));
			wcscpy_s(buf, fileNameLength + sizeof(WCHAR), fileName);
			wcscat_s(buf, fileNameLength + sizeof(backup) / sizeof(WCHAR), backup);
			BOOL ret = CopyFileW(fileName, buf, FALSE);
			free(buf);
			return ret;
		}

	protected:
		HANDLE hFile;
		PVOID pBuffer;
		BY_HANDLE_FILE_INFORMATION fileInformation;
		String fileName;
		String fileFullName;
	};

XKEINNAMESPACE_END