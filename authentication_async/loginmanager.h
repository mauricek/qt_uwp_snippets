#ifndef LOGINMANAGER_H
#define LOGINMANAGER_H

#include <QObject>
#include <windows.security.credentials.ui.h>
#include <wrl.h>

class LoginManager : public QObject
{
    Q_OBJECT
    Q_PROPERTY(State state READ state WRITE setState NOTIFY stateChanged)
    Q_PROPERTY(LoginError error READ error WRITE setError NOTIFY errorChanged)
public:
    enum State {
        Idle = 0,
        Verifying,
        Done
    };

    enum LoginError {
        NoError = 0,
        Error
    };
    explicit LoginManager(QObject *parent = 0);

    bool isValid() const;
    State state() const;
    LoginError error() const;

    HRESULT authenticationDone(ABI::Windows::Foundation::IAsyncOperation<enum ABI::Windows::Security::Credentials::UI::UserConsentVerificationResult> *,
                                  ABI::Windows::Foundation::AsyncStatus);
signals:
    void stateChanged(State state);
    void errorChanged(LoginError error);

public slots:
    void login();
    void setState(State state);
    void setError(LoginError error);

private:
    Microsoft::WRL::ComPtr<ABI::Windows::Security::Credentials::UI::IUserConsentVerifierStatics> ui_statics;

    State m_state;
    LoginError m_error;
};

#endif // LOGINMANAGER_H
