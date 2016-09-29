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

Q_DECLARE_METATYPE(LoginManager::LoginError);
Q_DECLARE_METATYPE(LoginManager::State);

LoginManager::LoginManager(QObject *parent) : QObject(parent)
  , m_state(LoginManager::Idle)
  , m_error(LoginManager::NoError)
{
    HRESULT hr;

    hr = RoGetActivationFactory(HString::MakeReference(RuntimeClass_Windows_Security_Credentials_UI_UserConsentVerifier).Get(), IID_PPV_ARGS(&ui_statics));

    qRegisterMetaType<LoginError>("LoginError");
    qRegisterMetaType<State>("State");
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

HRESULT LoginManager::authenticationDone(IAsyncOperation<enum ABI::Windows::Security::Credentials::UI::UserConsentVerificationResult> *requestOp, AsyncStatus)
{
    HRESULT hr;
    UserConsentVerificationResult verificationResult;
    hr = requestOp->GetResults(&verificationResult);
    if (FAILED(hr) || verificationResult != UserConsentVerificationResult_Verified) {
        QMetaObject::invokeMethod(this, "setError", Q_ARG(LoginError, LoginManager::Error));
        QMetaObject::invokeMethod(this, "setState", Q_ARG(State, LoginManager::Done));
    } else {
        QMetaObject::invokeMethod(this, "setError", Q_ARG(LoginError, LoginManager::NoError));
        QMetaObject::invokeMethod(this, "setState", Q_ARG(State, LoginManager::Done));
    }
    return S_OK;
}

void LoginManager::login()
{
    setError(LoginManager::NoError);
    setState(LoginManager::Verifying);

    ComPtr<IAsyncOperation<UserConsentVerificationResult>> requestOp;
    HRESULT hr;
    hr = QEventDispatcherWinRT::runOnXamlThread([this, &requestOp]() {
        HRESULT hr;
        hr = ui_statics.Get()->RequestVerificationAsync(HString::MakeReference(L"Title for authentication").Get(), &requestOp);
        if (SUCCEEDED(hr)) {
            auto opCallback = Callback<IAsyncOperationCompletedHandler<UserConsentVerificationResult>>(this, &LoginManager::authenticationDone);
            hr = requestOp.Get()->put_Completed(opCallback.Get());
        }
        return S_OK;
    });

    if (!requestOp) {
        setError(LoginManager::Error);
        setState(LoginManager::Done);
        return;
    }
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
