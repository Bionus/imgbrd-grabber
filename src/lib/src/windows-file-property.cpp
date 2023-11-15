// https://github.com/microsoft/Windows-classic-samples/tree/master/Samples/Win7Samples/winui/shell/appplatform/PropertyEdit

#include "windows-file-property.h"
#include <shobjidl.h>
#include <propsys.h>
#include <propvarutil.h>
#include <propkey.h>
#include <strsafe.h>
#include <QDebug>
#include <QMap>
#include <QString>
#include "logger.h"


wchar_t *toWCharT2(const QString &str)
{
	auto *out = new wchar_t[str.length() + 1];
	str.toWCharArray(out);
	out[str.length()] = 0;
	return out;
}

HRESULT GetPropertyStore(PCWSTR pszFilename, GETPROPERTYSTOREFLAGS gpsFlags, IPropertyStore** ppps)
{
	WCHAR szExpanded[MAX_PATH];
	HRESULT hr = ExpandEnvironmentStringsW(pszFilename, szExpanded, ARRAYSIZE(szExpanded)) ? S_OK : HRESULT_FROM_WIN32(GetLastError());
	if (SUCCEEDED(hr)) {
		WCHAR szAbsPath[MAX_PATH];
		hr = _wfullpath(szAbsPath, szExpanded, ARRAYSIZE(szAbsPath)) ? S_OK : E_FAIL;
		if (SUCCEEDED(hr)) {
			hr = SHGetPropertyStoreFromParsingName(szAbsPath, NULL, gpsFlags, IID_PPV_ARGS(ppps));
		}
	}
	return hr;
}

bool GetProperty(IPropertyStore *pps, REFPROPERTYKEY key, QString &out)
{
	PROPVARIANT propvarValue = {0};
	HRESULT hr = pps->GetValue(key, &propvarValue);
	if (SUCCEEDED(hr)) {
		PWSTR pszDisplayValue = NULL;
		hr = PSFormatForDisplayAlloc(key, propvarValue, PDFF_DEFAULT, &pszDisplayValue);
		if (SUCCEEDED(hr)) {
			out = QString::fromWCharArray(pszDisplayValue);
			CoTaskMemFree(pszDisplayValue);
		}
		PropVariantClear(&propvarValue);
	}
	return SUCCEEDED(hr);
}


void initializeWindowsProperties()
{
	CoInitializeEx(NULL, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);
}

void uninitializeWindowsProperties()
{
	CoUninitialize();
}

bool getAllWindowsProperties(const QString &filename, QMap<QString, QString> &out)
{
	PCWSTR pszFilename = toWCharT2(filename);
	IPropertyStore* pps = NULL;

	// Call the helper to get the property store for the initialized item
	// Note that as long as you have the property store, you are keeping the file open
	// So always release it once you are done.

	HRESULT hr = GetPropertyStore(pszFilename, GPS_DEFAULT, &pps);
	if (SUCCEEDED(hr)) {
		// Retrieve the number of properties stored in the item.
		DWORD cProperties = 0;
		hr = pps->GetCount(&cProperties);
		if (SUCCEEDED(hr)) {
			for (DWORD i = 0; i < cProperties; i++) {
				// Get the property key at a given index.
				PROPERTYKEY key;
				hr = pps->GetAt(i, &key);
				if (SUCCEEDED(hr)) {
					// Get the canonical name of the property
					PWSTR pszCanonicalName = NULL;
					hr = PSGetNameFromPropertyKey(key, &pszCanonicalName);
					if (SUCCEEDED(hr)) {
						QString name = QString::fromWCharArray(pszCanonicalName);
						CoTaskMemFree(pszCanonicalName);

						QString val;
						if (GetProperty(pps, key, val)) {
							out.insert(name, val);
						}
					}
				}
			}
		}
		pps->Release();
	} else {
		log(QString("Error %1 getting the propertystore for `%2`").arg(hr).arg(filename), Logger::Error);
	}

	delete pszFilename;

	return SUCCEEDED(hr);
}

bool getWindowsProperty(const QString &filename, const QString &property, QString &out)
{
	PCWSTR pszFilename = toWCharT2(filename);
	PCWSTR pszCanonicalName = toWCharT2(property);

	// Convert the Canonical name of the property to PROPERTYKEY
	PROPERTYKEY key;
	HRESULT hr = PSGetPropertyKeyFromName(pszCanonicalName, &key);
	if (SUCCEEDED(hr)) {
		IPropertyStore* pps = NULL;

		// Call the helper to get the property store for the initialized item
		hr = GetPropertyStore(pszFilename, GPS_DEFAULT, &pps);
		if (SUCCEEDED(hr)) {
			GetProperty(pps, key, out);
			pps->Release();
		} else {
			log(QString("Error %1 getting the propertystore for `%2`").arg(hr).arg(filename), Logger::Error);
		}
	} else {
		log(QString("Invalid property specified: %1").arg(property), Logger::Error);
	}

	delete pszFilename;
	delete pszCanonicalName;

	return SUCCEEDED(hr);
}

bool setWindowsProperty(const QString &filename, const QString &property, const QString &value)
{
	PCWSTR pszFilename = toWCharT2(filename);
	PCWSTR pszCanonicalName = toWCharT2(property);
	PCWSTR pszValue = toWCharT2(value);

	// Convert the Canonical name of the property to PROPERTYKEY
	PROPERTYKEY key;
	HRESULT hr = PSGetPropertyKeyFromName(pszCanonicalName, &key);
	if (SUCCEEDED(hr)) {
		IPropertyStore* pps = NULL;

		// Call the helper to get the property store for the
		// initialized item
		hr = GetPropertyStore(pszFilename, GPS_READWRITE, &pps);
		if (SUCCEEDED(hr)) {
			PROPVARIANT propvarValue = {0};
			hr = InitPropVariantFromString(pszValue, &propvarValue);
			if (SUCCEEDED(hr)) {
				hr = PSCoerceToCanonicalValue(key, &propvarValue);
				if (SUCCEEDED(hr)) {
					// Set the value to the property store of the item.
					hr = pps->SetValue(key, propvarValue);
					if (SUCCEEDED(hr)) {
						// Commit does the actual writing back to the file stream.
						hr = pps->Commit();
						if (FAILED(hr)) {
							log(QString("Error %1 committing to the propertystore for `%2`").arg(hr).arg(filename), Logger::Error);
						}
					} else {
						log(QString("Error %1 setting value to the propertystore for `%2`").arg(hr).arg(filename), Logger::Error);
					}
				}
				PropVariantClear(&propvarValue);
			}
			pps->Release();
		} else {
			log(QString("Error %1 getting the propertystore for `%2`").arg(hr).arg(filename), Logger::Error);
		}
	} else {
		log(QString("Invalid property specified: %1").arg(property), Logger::Error);
	}

	delete pszFilename;
	delete pszCanonicalName;
	delete pszValue;

	return SUCCEEDED(hr);
}
