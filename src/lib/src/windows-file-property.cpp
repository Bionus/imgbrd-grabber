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
#include <string>
#include "logger.h"


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
	const std::wstring wideFilename = filename.toStdWString();
	IPropertyStore* pps = NULL;

	// Call the helper to get the property store for the initialized item
	// Note that as long as you have the property store, you are keeping the file open
	// So always release it once you are done.

	HRESULT hr = GetPropertyStore(wideFilename.c_str(), GPS_DEFAULT, &pps);
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

	return SUCCEEDED(hr);
}

bool getWindowsProperty(const QString &filename, const QString &property, QString &out)
{
	const std::wstring wideFilename = filename.toStdWString();
	const std::wstring wideCanonicalName = property.toStdWString();

	// Convert the Canonical name of the property to PROPERTYKEY
	PROPERTYKEY key;
	HRESULT hr = PSGetPropertyKeyFromName(wideCanonicalName.c_str(), &key);
	if (SUCCEEDED(hr)) {
		IPropertyStore* pps = NULL;

		// Call the helper to get the property store for the initialized item
		hr = GetPropertyStore(wideFilename.c_str(), GPS_DEFAULT, &pps);
		if (SUCCEEDED(hr)) {
			GetProperty(pps, key, out);
			pps->Release();
		} else {
			log(QString("Error %1 getting the propertystore for `%2`").arg(hr).arg(filename), Logger::Error);
		}
	} else {
		log(QString("Invalid property specified: %1").arg(property), Logger::Error);
	}

	return SUCCEEDED(hr);
}

bool setWindowsProperty(const QString &filename, const QString &property, const QString &value)
{
	const std::wstring wideFilename = filename.toStdWString();
	const std::wstring wideCanonicalName = property.toStdWString();
	const std::wstring wideValue = value.toStdWString();

	// Convert the Canonical name of the property to PROPERTYKEY
	PROPERTYKEY key;
	HRESULT hr = PSGetPropertyKeyFromName(wideCanonicalName.c_str(), &key);
	if (SUCCEEDED(hr)) {
		IPropertyStore* pps = NULL;

		// Call the helper to get the property store for the
		// initialized item
		hr = GetPropertyStore(wideFilename.c_str(), GPS_READWRITE, &pps);
		if (SUCCEEDED(hr)) {
			PROPVARIANT propvarValue = {0};
			hr = InitPropVariantFromString(wideValue.c_str(), &propvarValue);
			if (SUCCEEDED(hr)) {
				hr = PSCoerceToCanonicalValue(key, &propvarValue);
				if (SUCCEEDED(hr)) {
					// Set the value to the property store of the item.
					hr = pps->SetValue(key, propvarValue);
					if (SUCCEEDED(hr)) {
						// Commit does the actual writing back to the file stream.
						hr = pps->Commit();
						if (FAILED(hr)) {
							log(QString("Error %1 committing to the propertystore \"%2\" for `%2`").arg(hr).arg(property, filename), Logger::Error);
						}
					} else {
						log(QString("Error %1 setting value to the propertystore \"%2\" for `%3`").arg(hr).arg(property, filename), Logger::Error);
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

	return SUCCEEDED(hr);
}

bool clearAllWindowsProperties(const QString &filename)
{
	const std::wstring wideFilename = filename.toStdWString();
	IPropertyStore* pps = NULL;

	HRESULT hr = GetPropertyStore(wideFilename.c_str(), GPS_READWRITE, &pps);
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
					// Build the value
					PROPVARIANT propvarValue = {0};
					PropVariantInit(&propvarValue);

					// Clear the value in the property store of the item.
					hr = pps->SetValue(key, propvarValue);
					if (SUCCEEDED(hr)) {
						i--; // Clearing a value changes the value count and the results of GetCount() and GetAt()
					}

					PropVariantClear(&propvarValue);
				}
			}
		}

		// Commit does the actual writing back to the file stream.
		hr = pps->Commit();
		if (FAILED(hr)) {
			log(QString("Error %1 committing to the propertystore for `%2`").arg(hr).arg(filename), Logger::Error);
		}

		pps->Release();
	} else {
		log(QString("Error %1 getting the propertystore for `%2`").arg(hr).arg(filename), Logger::Error);
	}

	return SUCCEEDED(hr);
}
