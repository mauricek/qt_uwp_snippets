#include "loginmanager.h"
#include <qfunctions_winrt.h>
#include <qdebug.h>
#include <QtCore/QCoreApplication>
#include <private/qeventdispatcher_winrt_p.h>
#include <windows.security.credentials.h>
#include <windows.security.credentials.ui.h>
#include <functional>
#include <ppltasks.h>
#include <wrl.h>

using namespace Windows::Security::Credentials;
using namespace Windows::Security::Credentials::UI;
using namespace Windows::Foundation;
using namespace Microsoft::WRL;
using namespace Microsoft::WRL::Wrappers;
using namespace concurrency;

// Calm down VS bug about conversion issues
HRESULT __stdcall GetActivationFactoryByPCWSTR(void*,Platform::Guid&, void**);
namespace __winRT
{
    HRESULT __stdcall __getActivationFactoryByPCWSTR(const void* str, ::Platform::Guid& pGuid, void** ppActivationFactory)
    {
        return GetActivationFactoryByPCWSTR(const_cast<void*>(str), pGuid, ppActivationFactory);
    }
}
// End VSBug

Q_DECLARE_METATYPE(LoginManager::LoginError);
Q_DECLARE_METATYPE(LoginManager::State);

LoginManager::LoginManager(QObject *parent) : QObject(parent)
  , m_state(LoginManager::Idle)
  , m_error(LoginManager::NoError)
{
    qRegisterMetaType<LoginError>("LoginError");
    qRegisterMetaType<State>("State");
}

bool LoginManager::isValid() const
{
    IAsyncOperation<UserConsentVerifierAvailability>^ availOp = UserConsentVerifier::CheckAvailabilityAsync();
    while (availOp->Status == Windows::Foundation::AsyncStatus::Started)
        QThread::yieldCurrentThread();
    UserConsentVerifierAvailability available = availOp->GetResults();
    return available == UserConsentVerifierAvailability::Available;
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

    HRESULT hr;
    hr = QEventDispatcherWinRT::runOnXamlThread([this]() {
        Platform::String^ title(L"Some title");
        create_task(UserConsentVerifier::RequestVerificationAsync(title)).then([this](UserConsentVerificationResult verificationResult)
        {
            if (verificationResult != UserConsentVerificationResult::Verified) {
                QMetaObject::invokeMethod(this, "setError", Q_ARG(LoginManager::LoginError, LoginManager::Error));
                QMetaObject::invokeMethod(this, "setState", Q_ARG(LoginManager::State, LoginManager::Done));
            } else {
                QMetaObject::invokeMethod(this, "setError", Q_ARG(LoginManager::LoginError, LoginManager::NoError));
                QMetaObject::invokeMethod(this, "setState", Q_ARG(LoginManager::State, LoginManager::Done));
            }
        });
        return S_OK;
    });
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
