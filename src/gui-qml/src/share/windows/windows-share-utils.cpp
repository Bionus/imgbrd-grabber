#include "windows-share-utils.h"
#include <QOperatingSystemVersion>
#include <roapi.h>
#include <windows.h>
#include <windows.foundation.h>
#include <windows.applicationmodel.datatransfer.h>
#include <winstring.h>
#include <wrl.h>
#include <ShObjIdl_core.h>
#include "logger.h"

WindowsShareUtils::WindowsShareUtils(QQuickItem* parent)
	: BaseShareUtils(parent)
{}


using namespace ABI::Windows::Foundation;
using namespace ABI::Windows::ApplicationModel::DataTransfer;
using namespace Microsoft::WRL;
using namespace Microsoft::WRL::Wrappers;

wchar_t *PromiseFlatString(const QString &str)
{
	auto *ret = new wchar_t[str.length() + 1];
	str.toWCharArray(ret);
	ret[str.length()] = 0;
	return ret;
}

bool createComUri(const QUrl &url, IUriRuntimeClass **instance)
{
	if (!url.isValid()) {
		return false;
	}

	HRESULT hr;

	// Convert QUrl to HSTRING
	HSTRING hUrl;
	const QString &strUrl = url.toString();
	hr = WindowsCreateString(PromiseFlatString(strUrl), strUrl.length(), &hUrl);
	if (FAILED(hr)) {
		return false;
	}
	//HStringUniquePtr url(rawUrl);

	// Get the IUri constructor
	ComPtr<IUriRuntimeClassFactory> uriFactory;
	hr = GetActivationFactory(HStringReference(RuntimeClass_Windows_Foundation_Uri).Get(), &uriFactory);
	if (FAILED(hr)) {
		return false;
	}

	// Build the IUri from the HSTRING
	hr = uriFactory->CreateUri(hUrl, instance);
	if (FAILED(hr)) {
		return false;
	}

	return true;
}

bool windowsShare(const QString &text, const QUrl &url, const QString &title = "")
{
	// Share APIs have been introduced in Windows 10
	if (QOperatingSystemVersion::current() < QOperatingSystemVersion(QOperatingSystemVersion::Windows, 10)) {
		return false;
	}

	HRESULT hr;

	// Convert the text to share to HSTRING
	HSTRING hText;
	if (!text.isEmpty()) {
		hr = WindowsCreateString(PromiseFlatString(text), text.length(), &hText);
		if (FAILED(hr)) {
			return false;
		}
	}
	//HStringUniquePtr text(hText);

	// Convert the url to share to an IUri COM instance
	ComPtr<IUriRuntimeClass> uri;
	if (!url.isEmpty()) {
		bool ok = createComUri(url, &uri);
		if (!ok) {
			return false;
		}
	}

	// Convert the title to share to HSTRING
	HSTRING hTitle;
	hr = WindowsCreateString(PromiseFlatString(title), title.length(), &hTitle);
	if (FAILED(hr)) {
		return false;
	}
	//HStringUniquePtr shareTitle(hTitle);

	HWND hwnd = GetForegroundWindow();
	if (!hwnd) {
		return false;
	}

	ComPtr<IDataTransferManagerInterop> dtmInterop;
	hr = RoGetActivationFactory(HStringReference(RuntimeClass_Windows_ApplicationModel_DataTransfer_DataTransferManager).Get(), IID_PPV_ARGS(&dtmInterop));
	if (FAILED(hr)) {
		return false;
	}

	ComPtr<IDataTransferManager> dtm;
	hr = dtmInterop->GetForWindow(hwnd, IID_PPV_ARGS(&dtm));
	if (FAILED(hr)) {
		return false;
	}

	auto callback = Callback<ITypedEventHandler<DataTransferManager*, DataRequestedEventArgs*>>([uri = std::move(uri), hText, hTitle, url, text](IDataTransferManager*, IDataRequestedEventArgs* pArgs) -> HRESULT {
		ComPtr<IDataRequest> spDataRequest;
		HRESULT hr = pArgs->get_Request(&spDataRequest);
		if (FAILED(hr)) {
			return hr;
		}

		ComPtr<IDataPackage> spDataPackage;
		hr = spDataRequest->get_Data(&spDataPackage);
		if (FAILED(hr)) {
			return hr;
		}

		ComPtr<IDataPackage2> spDataPackage2;
		hr = spDataPackage->QueryInterface(IID_PPV_ARGS(&spDataPackage2));
		if (FAILED(hr)) {
			return hr;
		}

		ComPtr<IDataPackagePropertySet> spDataPackageProperties;
		hr = spDataPackage->get_Properties(&spDataPackageProperties);
		if (FAILED(hr)) {
			return hr;
		}

		hr = spDataPackageProperties->put_Title(hTitle);
		if (FAILED(hr)) {
			return hr;
		}

		if (!url.isEmpty()) {
			hr = spDataPackage2->SetWebLink(uri.Get());
			if (FAILED(hr)) {
				return hr;
			}
		}

		if (!text.isEmpty()) {
			hr = spDataPackage->SetText(hText);
			if (FAILED(hr)) {
				return hr;
			}
		}

		return S_OK;
	});

	EventRegistrationToken dataRequestedToken;
	hr = dtm->add_DataRequested(callback.Get(), &dataRequestedToken);
	if (FAILED(hr)) {
		return false;
	}

	hr = dtmInterop->ShowShareUIForWindow(hwnd);
	if (FAILED(hr)) {
		return false;
	}

	return true;
}


bool WindowsShareUtils::share(const QString &text)
{
	return windowsShare(text, {});
}

bool WindowsShareUtils::shareUrl(const QUrl &url)
{
	return windowsShare({}, url);
}

bool WindowsShareUtils::sendFile(const QString &path, const QString &mimeType, const QString &title)
{
	return false;
}
