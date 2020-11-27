#pragma once

#include "Globals.h"

#define _WIN32_DCOM
#include <iostream>
#include <comdef.h>
#include <Wbemidl.h>
#pragma comment(lib, "wbemuuid.lib")

struct gpu_information {
	std::string name;
};

static bool GPUInformation(gpu_information &info)
{
	CoUninitialize();
	HRESULT hres;
	
	hres = CoInitializeEx(0, COINIT_MULTITHREADED);
	if (FAILED(hres)) {

		_com_error err(hres);
		LPCTSTR errMsg = err.ErrorMessage();
		LOG(_ERROR, "%s", errMsg);
		return false;
	}
	
	hres = CoInitializeSecurity(NULL, -1, NULL, NULL, RPC_C_AUTHN_LEVEL_DEFAULT, RPC_C_IMP_LEVEL_IMPERSONATE, NULL, EOAC_NONE, NULL);
	if (FAILED(hres)) { 
		CoUninitialize(); 
		return false; 
	}
	
	IWbemLocator* pLoc = NULL;
	hres = CoCreateInstance(CLSID_WbemLocator, 0, CLSCTX_INPROC_SERVER, IID_IWbemLocator, (LPVOID*)&pLoc);
	if (FAILED(hres)) { CoUninitialize(); return false; }
	
	IWbemServices* pSvc = NULL;
	hres = pLoc->ConnectServer(_bstr_t(L"root\\CIMV2"), NULL, NULL, 0, NULL, 0, 0, &pSvc);
	if (FAILED(hres)) { pLoc->Release(); CoUninitialize(); return false; }
	
	hres = CoSetProxyBlanket(pSvc, RPC_C_AUTHN_WINNT, RPC_C_AUTHZ_NONE, NULL, RPC_C_AUTHN_LEVEL_CALL, RPC_C_IMP_LEVEL_IMPERSONATE, NULL, EOAC_NONE);
	if (FAILED(hres)) { 
		pSvc->Release(); 
		pLoc->Release(); 
		CoUninitialize(); 
		return false; 
	}
	
	IEnumWbemClassObject* pEnumerator = NULL;

	hres = pSvc->ExecQuery(bstr_t("WQL"), bstr_t("SELECT * FROM Win32_VideoController"),
								WBEM_FLAG_FORWARD_ONLY | WBEM_FLAG_RETURN_IMMEDIATELY, NULL, &pEnumerator);
	
	if (FAILED(hres)) { 
		pSvc->Release(); 
		pLoc->Release(); 
		CoUninitialize(); 
		return false; 
	}
	
	IWbemClassObject* pclsObj;
	
	ULONG uReturn = 0;
	while (pEnumerator) {

		HRESULT hr = pEnumerator->Next(WBEM_INFINITE, 1, &pclsObj, &uReturn);
		
		if (0 == uReturn)break;

		VARIANT vtProp;
		hr = pclsObj->Get(L"Caption", 0, &vtProp, 0, 0);
		info.name = (char *) vtProp.bstrVal;
		VariantClear(&vtProp);



	}

	pSvc->Release();
	pLoc->Release();
	pEnumerator->Release();
	pclsObj->Release();
	CoUninitialize();

	return true;
}