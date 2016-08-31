#ifndef LOGINMANAGER_H
#define LOGINMANAGER_H

#include <QObject>
#include <wrl.h>
namespace ABI {
namespace Windows {
namespace Security {
namespace Credentials {
namespace UI {
struct IUserConsentVerifierStatics;
}
struct IKeyCredentialManagerStatics;
}
}
}
}

using namespace ABI::Windows::Security::Credentials;

class LoginManager : public QObject
{
    Q_OBJECT
public:
    enum {
        Idle = 0,
        Verifying,
        Done
    } State;

    enum {
        NoError = 0,
        Error
    };
    explicit LoginManager(QObject *parent = 0);

    bool isValid() const;
signals:

public slots:
    void login();
private:
    Microsoft::WRL::ComPtr<ABI::Windows::Security::Credentials::IKeyCredentialManagerStatics> statics;
    Microsoft::WRL::ComPtr<ABI::Windows::Security::Credentials::UI::IUserConsentVerifierStatics> ui_statics;
};

#endif // LOGINMANAGER_H
