// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

// These #defines prevent the idl-generated headers from trying to include
// Windows.h from the SDK rather than this one.
#define RPC_NO_WINDOWS_H
#define COM_NO_WINDOWS_H

// Allcaps type definitions
#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include <limits.h>

#ifndef _BASETSD_H_
// Note: using fixed-width here to match Windows widths
// Specifically this is different for 'long' vs 'LONG'
typedef uint8_t UINT8;
typedef int8_t INT8;
typedef uint16_t UINT16;
typedef int16_t INT16;
typedef uint32_t UINT32, UINT, ULONG, DWORD, BOOL, WINBOOL;
typedef int32_t INT32, INT, LONG;
typedef unsigned long long UINT64;
typedef long long INT64, LONG_PTR;
typedef void VOID, *HANDLE, *RPC_IF_HANDLE, *LPVOID;
typedef const void *LPCVOID;
typedef size_t SIZE_T;
typedef float FLOAT;
typedef double DOUBLE;
typedef unsigned char BYTE, *PBYTE;
typedef int HWND;
typedef int PALETTEENTRY;
typedef int HDC;
typedef uint16_t WORD;
typedef void* PVOID;
typedef char BOOLEAN;
typedef UINT64 ULONGLONG;
typedef uint16_t USHORT, *PUSHORT;
typedef INT64 LONGLONG, *PLONGLONG;
typedef INT64 LONG_PTR, *PLONG_PTR;
typedef INT64 LONG64, *PLONG64;
typedef UINT64 ULONG64, *PULONG64;
typedef wchar_t WCHAR, *PWSTR;
typedef uint8_t UCHAR, *PUCHAR;
typedef uintptr_t ULONG_PTR, *PULONG_PTR;
typedef uintptr_t UINT_PTR, *PUINT_PTR;
typedef INT64 INT_PTR, *PINT_PTR;
typedef INT8* PINT8;
typedef INT16* PINT16;
typedef INT32* PINT32;
typedef INT64* PINT64;
typedef UINT8* PUINT8;
typedef UINT16* PUINT16;
typedef UINT32* PUINT32;
typedef UINT64* PUINT64;
typedef SIZE_T* PSIZE_T;
typedef FLOAT* PFLOAT;
typedef DOUBLE* PDOUBLE;
typedef BOOLEAN* PBOOLEAN;
#endif

// Note: WCHAR is not the same between Windows and Linux, to enable
// string manipulation APIs to work with resulting strings.
// APIs to D3D/DXCore will work on Linux wchars, but beware with
// interactions directly with the Windows kernel.
typedef char CHAR, *PSTR, *LPSTR, TCHAR, *PTSTR, *PCHAR;
typedef const char *LPCSTR, *PCSTR, *LPCTSTR, *PCTSTR, *PCSTR;
typedef wchar_t WCHAR, *PWSTR, *LPWSTR, *PWCHAR;
typedef const wchar_t *LPCWSTR, *PCWSTR;

#undef LONG_MAX
#define LONG_MAX INT_MAX
#undef ULONG_MAX
#define ULONG_MAX UINT_MAX

// Misc defines
#define MIDL_INTERFACE(x) interface
#define __analysis_assume(x)
#define TRUE 1u
#define FALSE 0u
#define DECLSPEC_UUID(x)
#define DECLSPEC_NOVTABLE
#define DECLSPEC_SELECTANY
#ifdef __cplusplus
#define EXTERN_C extern "C"
#else
#define EXTERN_C extern
#endif
#define APIENTRY
#define OUT
#define IN
#define CONST const
#define MAX_PATH 260
#define GENERIC_ALL 0x10000000L
#define C_ASSERT(expr) static_assert((expr))
#define _countof(a) (sizeof(a) / sizeof(*(a)))
#define EXTERN extern

typedef struct tagRECTL
{
    LONG left;
    LONG top;
    LONG right;
    LONG bottom;
} RECTL;

typedef struct tagPOINT
{
    int x;
    int y;
} POINT;

typedef struct GUID {
    uint32_t Data1;
    uint16_t Data2;
    uint16_t Data3;
    uint8_t  Data4[ 8 ];
} GUID;

#ifdef INITGUID
#ifdef __cplusplus
#define DEFINE_GUID(name,l,w1,w2,b1,b2,b3,b4,b5,b6,b7,b8) EXTERN_C const GUID DECLSPEC_SELECTANY name = { l, w1, w2, { b1, b2, b3, b4, b5, b6, b7, b8 } }
#else
#define DEFINE_GUID(name,l,w1,w2,b1,b2,b3,b4,b5,b6,b7,b8) const GUID DECLSPEC_SELECTANY name = { l, w1, w2, { b1, b2, b3, b4, b5, b6, b7, b8 } }
#endif
#else
#define DEFINE_GUID(name,l,w1,w2,b1,b2,b3,b4,b5,b6,b7,b8) EXTERN_C const GUID name
#endif

typedef GUID IID;
typedef GUID UUID;
typedef GUID CLSID;
#ifdef __cplusplus
#define REFGUID const GUID &
#define REFIID const IID &
#define REFCLSID const IID &

//__inline int InlineIsEqualGUID(REFGUID rguid1, REFGUID rguid2)
//{
//    return (
//        ((uint32_t *)&rguid1)[0] == ((uint32_t *)&rguid2)[0] &&
//        ((uint32_t *)&rguid1)[1] == ((uint32_t *)&rguid2)[1] &&
//        ((uint32_t *)&rguid1)[2] == ((uint32_t *)&rguid2)[2] &&
//        ((uint32_t *)&rguid1)[3] == ((uint32_t *)&rguid2)[3]);
//}

//inline bool operator==(REFGUID guidOne, REFGUID guidOther)
//{
//    return !!InlineIsEqualGUID(guidOne, guidOther);
//}

//inline bool operator!=(REFGUID guidOne, REFGUID guidOther)
//{
//    return !(guidOne == guidOther);
//}

#else
#define REFGUID const GUID *
#define REFIID const IID *
#define REFCLSID const IID *
#endif

// SAL annotations
#define _In_
#define _In_z_
#define _In_opt_
#define _In_opt_z_
#define _In_reads_(x)
#define _In_reads_opt_(x)
#define _In_reads_bytes_(x)
#define _In_reads_bytes_opt_(x)
#define _In_range_(x, y)
#define _In_bytecount_(x)
#define _Out_
#define _Out_opt_
#define _Outptr_
#define _Outptr_opt_result_z_
#define _Outptr_opt_result_bytebuffer_(x)
#define _COM_Outptr_
#define _COM_Outptr_result_maybenull_
#define _COM_Outptr_opt_
#define _COM_Outptr_opt_result_maybenull_
#define _Out_writes_(x)
#define _Out_writes_z_(x)
#define _Out_writes_opt_(x)
#define _Out_writes_all_(x)
#define _Out_writes_all_opt_(x)
#define _Out_writes_to_opt_(x, y)
#define _Out_writes_bytes_(x)
#define _Out_writes_bytes_all_(x)
#define _Out_writes_bytes_all_opt_(x)
#define _Out_writes_bytes_opt_(x)
#define _Inout_
#define _Inout_opt_
#define _Inout_updates_(x)
#define _Inout_updates_bytes_(x)
#define _Field_size_(x)
#define _Field_size_opt_(x)
#define _Field_size_bytes_(x)
#define _Field_size_full_(x)
#define _Field_size_full_opt_(x)
#define _Field_size_bytes_full_(x)
#define _Field_size_bytes_full_opt_(x)
#define _Field_size_bytes_part_(x, y)
#define _Field_range_(x, y)
#define _Field_z_
#define _Check_return_
#define _IRQL_requires_(x)
#define _IRQL_requires_min_(x)
#define _IRQL_requires_max_(x)
#define _At_(x, y)
#define _Always_(x)
#define _Return_type_success_(x)
#define _Translates_Win32_to_HRESULT_(x)
#define _Maybenull_
#define _Outptr_result_maybenull_
#define _Outptr_result_nullonfailure_
#define _Analysis_assume_(x)
#define _Success_(x)
#define _In_count_(x)
#define _In_opt_count_(x)
#define _Use_decl_annotations_
#define _Null_terminated_
#define _Printf_format_string_

// Calling conventions
#define __cdecl
#define __stdcall
#define STDMETHODCALLTYPE
#define STDAPICALLTYPE
#define STDAPI EXTERN_C HRESULT STDAPICALLTYPE
#define WINAPI

#if defined(lint)
// Note: lint -e530 says don't complain about uninitialized variables for
// this variable.  Error 527 has to do with unreachable code.
// -restore restores checking to the -save state
#define UNREFERENCED_PARAMETER(P) \
    /*lint -save -e527 -e530 */ \
    { \
        (P) = (P); \
    } \
    /*lint -restore */
#else
#define UNREFERENCED_PARAMETER(P) (P)
#endif
