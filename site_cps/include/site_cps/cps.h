/*****************************************************************************/
/*! \file    cps.h
*   \author  (c) 2018 by zhusizhi Development
*   \date    Date: 2018/09/12
*   \version PRODUCTVERSION: 1.0.0
*
*   \brief   definition of exported API functions and constants
*//***************************************************************************/

#ifndef __CPS_HEADER__
#define __CPS_HEADER__

#include <QString>
#include <QDateTime>

#pragma pack(push, 8)

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */

/* make sure __LINUX__ is defined */
#if defined __linux__
#   if !defined __LINUX__
#       define __LINUX__
#   endif /* !defined __LINUX__ */
#endif /* defined __linux__ */




// ----------------------------------------------------------------------------
// Driver name
// ----------------------------------------------------------------------------
#if defined(__linux__) || defined(__LINUX__)
#   define DRIVER_DLL_NAME                  ".so"
#elif defined (_WIN64)
#   define DRIVER_DLL_NAME                  ".dll"
#else
#   define DRIVER_DLL_NAME                  ".dll"
#endif


// ----------------------------------------------------------------------------
// Typedefs
// ----------------------------------------------------------------------------
#ifdef __LINUX__
        #define FORCEINLINE         inline
        #define USHORT              IS_U16

        #include <unistd.h>
        #include <wchar.h>
        #include <stdint.h>

        // aliases for common Win32 types
        typedef int32_t           BOOLEAN;
        typedef int32_t           BOOL;
        typedef int32_t           INT;
        typedef uint32_t          UINT;
        typedef int32_t           LONG;
        typedef void              VOID;
        typedef void*             LPVOID;
        typedef uint32_t          ULONG;

        typedef uint64_t          UINT64;
        typedef int64_t           __int64;
        typedef int64_t           LONGLONG;
        typedef uint32_t          DWORD;
        typedef uint16_t          WORD;

        typedef unsigned char     BYTE;
        typedef char              CHAR;
        typedef char              TCHAR;
        typedef unsigned char     UCHAR;

        typedef int8_t*           LPTSTR;
        typedef const int8_t*     LPCTSTR;
        typedef const int8_t*     LPCSTR;
        typedef uint32_t          WPARAM;
        typedef uint32_t          LPARAM;
        typedef uint32_t          LRESULT;
        typedef uint32_t          HRESULT;

        typedef void*             HWND;
        typedef void*             HGLOBAL;
        typedef void*             HINSTANCE;
        typedef void*             HDC;
        typedef void*             HMODULE;
        typedef void*             HKEY;
        typedef void*             HANDLE;

        typedef BYTE*             LPBYTE;
        typedef DWORD*            PDWORD;
        typedef VOID*             PVOID;
        typedef CHAR*             PCHAR;



    #ifndef FALSE
        #define FALSE 0
    #endif
    #ifndef TRUE
        #define TRUE 1
    #endif

    #ifndef HIBYTE
        #define HIBYTE(_x_)    ( (_x_)>>8 )
    #endif

    #ifndef LOBYTE
        #define LOBYTE(_x_)    ( (_x_)&0x00ff )
    #endif

    #ifndef HIWORD
        #define HIWORD(_x_)    ( (_x_)>>16 )
    #endif

    #ifndef LOWORD
        #define LOWORD(_x_)    ( (_x_)&0x0000ffff )
    #endif

    #ifndef _min_
        #define _min_( _x_, _y_ ) ( (_x_)<(_y_) ? (_x_) : (_y_) )
    #endif

    #ifndef _max_
        #define _max_( _x_, _y_ ) ( (_x_)>(_y_) ? (_x_) : (_y_) )
    #endif

    #ifndef WM_USER
        #define WM_USER        0x400
    #endif

        // unknown stuff for Linux
        #define WINAPI
        #define CALLBACK
        #undef  UNICODE
        #define __stdcall
        #define __cdecl

#if defined (_IDS_EXPORT)
        #define IDSEXP    __attribute__((visibility("default"))) INT
        #define IDSEXPUL  __attribute__((visibility("default"))) ULONG
#else
        #define IDSEXP    INT
        #define IDSEXPUL  ULONG
#endif

        typedef long (*WNDPROC) (HWND, UINT, WPARAM, LPARAM);

    #define ZeroMemory(a,b)      memset((a), 0, (b))
    #define OutputDebugString(s) fprintf(stderr, s)


    #define INFINITE    -1
#else

#include <windows.h>

#if defined (_MSC_VER) || defined (__BORLANDC__) || defined (_WIN32_WCE)
  #if defined (_PURE_C) && !defined (_IDS_EXPORT) && !defined (_FALC_EXPORT)
    #define IDSEXP     extern  __declspec(dllimport) INT __cdecl
    #define IDSEXPUL   extern  __declspec(dllimport) ULONG __cdecl
  #elif defined (__STDC__) && !defined (_IDS_EXPORT) && !defined (_FALC_EXPORT)
    #define IDSEXP   extern  __declspec(dllimport) INT __cdecl
    #define IDSEXPUL extern  __declspec(dllimport) ULONG __cdecl
  #elif !defined (_IDS_EXPORT) && !defined (_FALC_EXPORT)   // using the DLL, not creating one
    #define IDSEXP   extern "C" __declspec(dllimport) INT __cdecl
    #define IDSEXPUL extern "C" __declspec(dllimport) ULONG __cdecl
  #elif defined (_IDS_VBSTD) || defined (_FALC_VBSTD)     // for creating stdcall dll
    #define IDSEXP    extern __declspec(dllexport) INT __stdcall
    #define IDSEXPUL  extern __declspec(dllexport) ULONG __stdcall
  #else            // for creating cdecl dll
    #define IDSEXP    extern  __declspec(dllexport) INT __cdecl
    #define IDSEXPUL  extern  __declspec(dllexport) ULONG __cdecl
  #endif
#elif !defined (_IDS_EXPORT) && !defined (_FALC_EXPORT)  // using the DLL, not creating one
  #define IDSEXP    extern  __declspec(dllimport) INT __cdecl
  #define IDSEXPUL  extern  __declspec(dllimport) ULONG __cdecl
#endif

typedef int     INT;

#endif    // Linux

#ifdef _WIN32_WCE
  typedef TCHAR IS_CHAR;
#else
  typedef char IS_CHAR;
#endif

//枚举按钮的几种状态
typedef enum{
    NORMALSTU,
    ENTER,
    PRESS,
    NOSTATUS
}ButtonStatus;

typedef enum{
    INIT_STATE = 0,
    DONING = 1,
    SUCCESS = 2,
    FAILED
}TASK_STATUS;

typedef struct _CPSTIME
{
    WORD      wYear;
    WORD      wMonth;
    WORD      wDay;
    WORD      wHour;
    WORD      wMinute;
    WORD      wSecond;
    WORD      wMilliseconds;
    BYTE      byReserved[10];
} CPSTIME;

typedef struct _CPS_Vector3
{
    double x;
    double y;
    double z;
}CPS_Vector3;

typedef struct _CPS_PRODUCTION
{
    QString strTaskId;
    QString strProductionId;
    QString strProductionName;
    int iProductionState;
    QString strCreateTime;
    QString strFinishTime;
    QString strStartVector;
    QString strFinishVector;

    _CPS_PRODUCTION()
    {
        strTaskId = "";
        strProductionId = "";
        strProductionName = "";
        iProductionState = 0;
        strCreateTime = "";
        strFinishTime = "";
        strStartVector = "";
        strFinishVector = "";
    }
} CPS_PRODUCTION, *PCPS_PRODUCTION;

typedef struct _CPS_TASK
{
    QString strTaskID;
    QString strTaskName;
    QString strTaskCode;
    QString strTaskDescribe;
    QString strAddress;
    int iTaskStatus;
    QString taskStartDate;
    QString taskPlanFinishDate;
    QString taskActualFinishDate;
    int iTaskDetail;

    QList<CPS_PRODUCTION> ppList;

} CPS_TASK, *PCPS_TASK;


typedef struct _CPS_XMPP_INFO
{
  QString strFlag;
  QString strTaskID;

} CPS_XMPP_INFO, *PCPS_XMPP_INFO;
// anti flicker modes
typedef enum E_ANTI_FLICKER_MODE
{
    ANTIFLCK_MODE_OFF             = 0,
    ANTIFLCK_MODE_SENS_AUTO       = 0x00000001,
    ANTIFLCK_MODE_SENS_50_FIXED   = 0x00000002,
    ANTIFLCK_MODE_SENS_60_FIXED   = 0x00000004
}ANTI_FLICKER_MODE;

typedef struct _KNEEPOINT
{
    double x;
    double y;
} KNEEPOINT, *PKNEEPOINT;

typedef struct _OPENGL_DISPLAY
{
    int     nWindowID;
    void     *pDisplay;
} OPENGL_DISPLAY;


typedef struct
{
    INT s32X;
    INT s32Y;
} IS_POINT_2D;

typedef struct
{
    INT s32Width;
    INT s32Height;
} IS_SIZE_2D;

typedef struct
{
    INT s32X;
    INT s32Y;
    INT s32Width;
    INT s32Height;
} IS_RECT;


// ----------------------------------------------------------------------------
// functions and datatypes only valid for CPS ETH
// ----------------------------------------------------------------------------

#pragma pack(push, 1)

  // IP V4 address
  typedef union _CPS_ETH_ADDR_IPV4
  {
      struct
      {
          BYTE  by1;
          BYTE  by2;
          BYTE  by3;
          BYTE  by4;
      } by;

      DWORD dwAddr;

  } CPS_ETH_ADDR_IPV4, *PCPS_ETH_ADDR_IPV4;

  // Ethernet address
  typedef struct _CPS_ETH_ADDR_MAC
  {
      BYTE abyOctet[6];

  } CPS_ETH_ADDR_MAC, *PCPS_ETH_ADDR_MAC;

  // IP configuration
  typedef struct _CPS_ETH_IP_CONFIGURATION
  {
      CPS_ETH_ADDR_IPV4    ipAddress;      /*!< IP address */
      CPS_ETH_ADDR_IPV4    ipSubnetmask;   /*!< IP subnetmask */

      BYTE                  reserved[4];    /*!< reserved */

  } CPS_ETH_IP_CONFIGURATION, *PCPS_ETH_IP_CONFIGURATION;


#pragma pack(pop)



#ifdef __cplusplus
};
#endif  /* __cplusplus */

#pragma pack(pop)

#endif  // #ifndef __IDS_HEADER__
