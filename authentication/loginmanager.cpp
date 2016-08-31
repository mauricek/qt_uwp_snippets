#include "loginmanager.h"
#include <qfunctions_winrt.h>
#include <qdebug.h>
#include <QtCore/QCoreApplication>
#include <private/qeventdispatcher_winrt_p.h>
#include <windows.security.credentials.h>
#include <windows.security.credentials.ui.h>
#include <wrl.h>
#include <functional>

using namespace ABI::Windows::Security::Credentials;
using namespace ABI::Windows::Security::Credentials::UI;
using namespace Microsoft::WRL;
using namespace Microsoft::WRL::Wrappers;
using namespace ABI::Windows::Foundation;

LoginManager::LoginManager(QObject *parent) : QObject(parent)
{
    HRESULT hr;

    hr = RoGetActivationFactory(HString::MakeReference(RuntimeClass_Windows_Security_Credentials_KeyCredentialManager).Get(), IID_PPV_ARGS(&statics));

    hr = RoGetActivationFactory(HString::MakeReference(RuntimeClass_Windows_Security_Credentials_UI_UserConsentVerifier).Get(), IID_PPV_ARGS(&ui_statics));
}

bool LoginManager::isValid() const
{
    boolean supported;
    ComPtr<IAsyncOperation<bool>> op;
    HRESULT hr = statics.Get()->IsSupportedAsync(&op);

    hr = QWinRTFunctions::await(op, &supported);
    qDebug() << "Supported:" << supported;
    return supported;
}

void LoginManager::login()
{
    UserConsentVerificationResult request;
    ComPtr<IAsyncOperation<enum ABI::Windows::Security::Credentials::UI::UserConsentVerificationResult>> requestOp;

    HRESULT hr = QEventDispatcherWinRT::runOnXamlThread([this, &request, &requestOp]() {
        UserConsentVerifierAvailability available;
        ComPtr<IAsyncOperation<enum ABI::Windows::Security::Credentials::UI::UserConsentVerifierAvailability>> availableOp;
        ui_statics.Get()->CheckAvailabilityAsync(&availableOp);
        HRESULT hr = QWinRTFunctions::await(availableOp, &available);
        qDebug() << "Available:" << available;

        if (SUCCEEDED(hr) && available == UserConsentVerifierAvailability_Available) {
            hr = ui_statics.Get()->RequestVerificationAsync(HString::MakeReference(L"Checking what is there").Get(), &requestOp);

            //        UserConsentVerificationResult_Verified	= 0,
            //        UserConsentVerificationResult_DeviceNotPresent	= 1,
            //        UserConsentVerificationResult_NotConfiguredForUser	= 2,
            //        UserConsentVerificationResult_DisabledByPolicy	= 3,
            //        UserConsentVerificationResult_DeviceBusy	= 4,
            //        UserConsentVerificationResult_RetriesExhausted	= 5,
            //        UserConsentVerificationResult_Canceled	= 6

        }
        return S_OK;
    });

    hr = QWinRTFunctions::await(requestOp, &request);
    qDebug() << "Result:" << request << hr;
    qDebug() << "HR:" << hr;
}
