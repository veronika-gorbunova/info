#include <stdio.h>
#include <conio.h>
#include <windows.h>
#include <winuser.h>
#include <iostream>
#include <accctrl.h>
#include <aclapi.h>

void main()
{
	wchar_t mus[] = { L"" };
	DWORD dwRes;
	PSID pEveryoneSID = NULL, pAdminSID = NULL;
	PACL pACL = NULL;
	PSECURITY_DESCRIPTOR pSD = NULL;

	HKEY hkSub = NULL;

	SID_IDENTIFIER_AUTHORITY SIDAuthWorld = SECURITY_WORLD_SID_AUTHORITY;
	SID_IDENTIFIER_AUTHORITY SIDAuthNT = SECURITY_NT_AUTHORITY;
	if (!AllocateAndInitializeSid(&SIDAuthWorld, 1,
		SECURITY_WORLD_RID, 0, 0, 0, 0, 0, 0, 0,
		&pEveryoneSID))
	{
		wprintf(L"AllocateAndInitializeSid Error %u\n", GetLastError());
		goto Cleanup;
	}

	if (!AllocateAndInitializeSid(&SIDAuthNT, 2,
		SECURITY_BUILTIN_DOMAIN_RID,
		DOMAIN_ALIAS_RID_ADMINS,
		0, 0, 0, 0, 0, 0,
		&pAdminSID))
	{
		wprintf(L"AllocateAndInitializeSid Error %u\n", GetLastError());
		goto Cleanup;
	}

	EXPLICIT_ACCESS ea[2];
	ZeroMemory(&ea, 2 * sizeof(EXPLICIT_ACCESS));
	ea[0].grfAccessPermissions = GENERIC_READ;
	ea[0].grfAccessMode = SET_ACCESS;
	ea[0].grfInheritance = CONTAINER_INHERIT_ACE;
	ea[0].Trustee.TrusteeForm = TRUSTEE_IS_SID;
	ea[0].Trustee.TrusteeType = TRUSTEE_IS_WELL_KNOWN_GROUP;
	ea[0].Trustee.ptstrName = (LPTSTR)pEveryoneSID;

	ea[1].grfAccessPermissions = GENERIC_ALL;
	ea[1].grfAccessMode = SET_ACCESS;
	ea[1].grfInheritance = CONTAINER_INHERIT_ACE;
	ea[1].Trustee.TrusteeForm = TRUSTEE_IS_SID;
	ea[1].Trustee.TrusteeType = TRUSTEE_IS_GROUP;
	ea[1].Trustee.ptstrName = (LPTSTR)pAdminSID;

	dwRes = SetEntriesInAcl(2, ea, NULL, &pACL);
	if (ERROR_SUCCESS != dwRes)
	{
		wprintf(L"SetEntriesInAcl Error %u\n", GetLastError());
		goto Cleanup;
	}

	pSD = (PSECURITY_DESCRIPTOR)LocalAlloc(LPTR, SECURITY_DESCRIPTOR_MIN_LENGTH);
	if (NULL == pSD)
	{
		wprintf(L"LocalAlloc Error %u\n", GetLastError());
		goto Cleanup;
	}

	if (!InitializeSecurityDescriptor(pSD, SECURITY_DESCRIPTOR_REVISION))
	{
		wprintf(L"InitializeSecurityDescriptor Error %u\n", GetLastError());
		goto Cleanup;
	}

	if (!SetSecurityDescriptorDacl(pSD, TRUE, pACL, FALSE))
	{
		wprintf(L"SetSecurityDescriptorDacl Error %u\n", GetLastError());
		goto Cleanup;
	}

	SECURITY_ATTRIBUTES sa;
	sa.nLength = sizeof(SECURITY_ATTRIBUTES);
	sa.lpSecurityDescriptor = pSD;
	sa.bInheritHandle = FALSE;

	if (RegOpenKey(HKEY_CURRENT_USER, L"SOFTWARE\\", &hkSub) == 0) {
		RegDeleteKey(hkSub, L"test29");
		RegCloseKey(hkSub);
	}

	wprintf(L"RegCreateKeyEx result %u\n",
		RegCreateKeyEx(HKEY_CURRENT_USER, L"SOFTWARE\\pi", 0, mus,
			REG_OPTION_NON_VOLATILE, KEY_SET_VALUE, &sa, &hkSub, &dwRes));

	if (hkSub) {
		char szPath[] = "123";
		if (RegSetValueExA(hkSub, "Password", 0, REG_SZ, (LPBYTE)szPath, strlen(szPath)) == ERROR_SUCCESS)
		{
			MessageBoxW(NULL, L"Key was created!", L"Success", MB_OK);
		}
		else
			MessageBoxW(NULL, L"Key not was created!", L"Error", MB_OK);
	}
Cleanup:

	if (pEveryoneSID)
		FreeSid(pEveryoneSID);
	if (pAdminSID)
		FreeSid(pAdminSID);
	if (pACL)
		LocalFree(pACL);
	if (pSD)
		LocalFree(pSD);
	if (hkSub)
		RegCloseKey(hkSub);

	system("regedit");
	return;
}
