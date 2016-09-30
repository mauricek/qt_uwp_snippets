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
  , m_state(LoginManager::Idle)
  , m_error(LoginManager::NoError)
{
    HRESULT hr;

    hr = RoGetActivationFactory(HString::MakeReference(RuntimeClass_Windows_Security_Credentials_UI_UserConsentVerifier).Get(), IID_PPV_ARGS(&ui_statics));
}

bool LoginManager::isValid() const
{
    UserConsentVerifierAvailability available;
    ComPtr<IAsyncOperation<enum ABI::Windows::Security::Credentials::UI::UserConsentVerifierAvailability>> availableOp;
    ui_statics.Get()->CheckAvailabilityAsync(&availableOp);
    HRESULT hr = QWinRTFunctions::await(availableOp, &available);

    if (FAILED(hr) || available != UserConsentVerifierAvailability_Available)
        return false;

    return true;
}

LoginManager::State LoginManager::state() const
{
    return m_state;
}

LoginManager::LoginError LoginManager::error() const
{
    return m_error;
}

void LoginManager::login()
{
    setError(LoginManager::NoError);
    setState(LoginManager::Verifying);

    UserConsentVerificationResult request;
    ComPtr<IAsyncOperation<enum ABI::Windows::Security::Credentials::UI::UserConsentVerificationResult>> requestOp;

    HRESULT hr = QEventDispatcherWinRT::runOnXamlThread([this, &requestOp]() {
        HRESULT hr;
        hr = ui_statics.Get()->RequestVerificationAsync(HString::MakeReference(L"Title for authentication").Get(), &requestOp);
        return S_OK;
    });

    if (!requestOp || FAILED(QWinRTFunctions::await(requestOp, &request))) {
        setError(LoginManager::Error);
        setState(LoginManager::Done);
        return;
    }

    UserConsentVerificationResult verificationResult;
    hr = requestOp.Get()->GetResults(&verificationResult);
    //        UserConsentVerificationResult_Verified    = 0,
    //        UserConsentVerificationResult_DeviceNotPresent    = 1,
    //        UserConsentVerificationResult_NotConfiguredForUser    = 2,
    //        UserConsentVerificationResult_DisabledByPolicy    = 3,
    //        UserConsentVerificationResult_DeviceBusy  = 4,
    //        UserConsentVerificationResult_RetriesExhausted    = 5,
    //        UserConsentVerificationResult_Canceled    = 6
    if (FAILED(hr) || verificationResult != UserConsentVerificationResult_Verified) {
        setError(LoginManager::Error);
        setState(LoginManager::Done);
        return;
    }

    setState(LoginManager::Done);
    setError(LoginManager::NoError);
}

void LoginManager::setState(LoginManager::State state)
{
    if (m_state == state)
        return;

    m_state = state;
    emit stateChanged(state);
}

void LoginManager::setError(LoginManager::LoginError error)
{
    if (m_error == error)
        return;

    m_error = error;
    emit errorChanged(error);
}
