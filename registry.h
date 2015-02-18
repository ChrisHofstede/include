// Registry.h
// This function will register a component.
HRESULT RegisterServer(const TCHAR* szModuleName, REFCLSID clsid, const TCHAR* szFriendlyName, const TCHAR* szVerIndProgID, const TCHAR* szProgID, const TCHAR* szThreadingModel);

// This function will unregister a component.
HRESULT UnregisterServer(REFCLSID clsid, const TCHAR* szVerIndProgID, const TCHAR* szProgID);
