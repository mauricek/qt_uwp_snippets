#include "activationfilter.h"
#include <QApplication>
#include <QPushButton>
#include <qabstracteventdispatcher.h>
#include <windows.h>
#include <qfunctions_winrt.h>
#include <qdebug.h>

#include <functional>
#include <wrl.h>
#include <windows.foundation.h>
#include <windows.storage.pickers.h>
#include <Windows.ApplicationModel.activation.h>
#include <Windows.ApplicationModel.datatransfer.sharetarget.h>
#include <windows.foundation.h>
#include <windows.system.threading.h>
#include <windows.ui.core.h>
#include <windows.applicationmodel.core.h>
#include <windows.ui.xaml.controls.h>

using namespace ABI::Windows::UI;
using namespace ABI::Windows::UI::Xaml;
using namespace ABI::Windows::UI::Xaml::Controls;

using namespace Microsoft::WRL;
using namespace Microsoft::WRL::Wrappers;
using namespace ABI::Windows::System::Threading;
using namespace ABI::Windows::Foundation;
using namespace ABI::Windows::UI::Core;
using namespace ABI::Windows::ApplicationModel::Core;

using namespace Microsoft::WRL;
using namespace Microsoft::WRL::Wrappers;
using namespace ABI::Windows::ApplicationModel::Activation;
using namespace ABI::Windows::Foundation;
using namespace ABI::Windows::Foundation::Collections;
using namespace ABI::Windows::Storage;
using namespace ABI::Windows::Storage::Pickers;
using namespace ABI::Windows::ApplicationModel::DataTransfer::ShareTarget;
using namespace ABI::Windows::ApplicationModel::DataTransfer;

class QActivationEvent : public QEvent
{
public:
    IInspectable *args() const {
        return reinterpret_cast<IInspectable *>(d);
    }
};

class ActivationFilterPrivate : public QSharedData
{
public:
    QString foo;
    ComPtr<IShareTargetActivatedEventArgs> shareTarget;
    ComPtr<IShareOperation> shareOp;

};

ActivationFilter::ActivationFilter(QObject *parent) : QObject(parent)
{
    d = new ActivationFilterPrivate();
}

bool ActivationFilter::eventFilter(QObject *, QEvent *e)
{
    if (e->type() != QEvent::WinEventAct)
        return false;

    qDebug() << "Inside Event Filter";
    HRESULT hr;
    QActivationEvent *event = static_cast<QActivationEvent *>(e);
    ComPtr<IInspectable> inspectable = event->args();
    ComPtr<IActivatedEventArgs> arguments;
    hr = inspectable.As(&arguments);
    Q_ASSERT_SUCCEEDED(hr);

    ActivationKind activationKind;
    hr = arguments->get_Kind(&activationKind);
    Q_ASSERT_SUCCEEDED(hr);

    switch(activationKind) {
    case (ActivationKind_ShareTarget):
        return handleShareEvent(inspectable);
    case (ActivationKind_Protocol):
        return handleProtocol(inspectable);
    case (ActivationKind_File):
        return handleFile(inspectable);
    case (ActivationKind_PickFileContinuation):
    case (ActivationKind_PickFolderContinuation):
    case (ActivationKind_PickSaveFileContinuation):
        // ### Add File handling magic
    default:
        return false;
    }

    return false;
}

#define RETURN_EMPTY_STRING_IF_FAILED(msg) \
    if (FAILED(hr)) { \
        qErrnoWarning(hr, msg); \
        return QString(); \
    }

QString ActivationFilter::activationText() const
{
    if (!d->shareTarget || !d->shareOp)
        return QString();

    HRESULT hr;
    ComPtr<IDataPackageView> dataView;
    hr = d->shareOp.Get()->get_Data(&dataView);
    RETURN_EMPTY_STRING_IF_FAILED("Could not access share data view.");

    QString result;
    ComPtr<IAsyncOperation<HSTRING>> op;
    HString res;
    hr = dataView->GetTextAsync(&op);
    RETURN_EMPTY_STRING_IF_FAILED("Could not access share text.");
    hr = QWinRTFunctions::await(op, res.GetAddressOf());
    RETURN_EMPTY_STRING_IF_FAILED("Could not query share text operation");
    if (FAILED(hr) || !res.IsValid())
        return QString();

    quint32 l;
    const wchar_t *raw = res.GetRawBuffer(&l);
    return QString::fromWCharArray(raw, l);
}

void ActivationFilter::finishShareActivation(bool error, const QString &errorString)
{
    qDebug() << __FUNCTION__ << error << errorString;

    if (!d->shareTarget || !d->shareOp)
        return;

    if (error) {
        HString msg;
        msg.Set(reinterpret_cast<const wchar_t*>(errorString.utf16()), errorString.length());
        d->shareOp.Get()->ReportError(msg.Get());
    } else
        d->shareOp.Get()->ReportCompleted();
}

bool ActivationFilter::handleProtocol(ComPtr<IInspectable> &inspectable)
{
    qDebug() << __FUNCTION__;
    ComPtr<IProtocolActivatedEventArgs> protocol;
    HRESULT hr;
    hr = inspectable.As(&protocol);
    RETURN_FALSE_IF_FAILED("Could not access activation protocol");
    ComPtr<IUriRuntimeClass> uri;
    hr = protocol.Get()->get_Uri(&uri);
    RETURN_FALSE_IF_FAILED("Could not access protocol uri");
    HString res;
    hr = uri.Get()->get_RawUri(res.GetAddressOf());
    if (FAILED(hr) || !res.IsValid())
        return false;

    quint32 l;
    const wchar_t *raw = res.GetRawBuffer(&l);
    const QString result = QString::fromWCharArray(raw, l);
    emit protocolActivation(result);
    return true;
}

bool ActivationFilter::handleShareEvent(ComPtr<IInspectable> &inspectable)
{
    qDebug() << __FUNCTION__;
    HRESULT hr;
    hr = inspectable.As(&d->shareTarget);
    RETURN_FALSE_IF_FAILED("Could not access share target arguments.");
    hr = d->shareTarget.Get()->get_ShareOperation(&d->shareOp);
    RETURN_FALSE_IF_FAILED("Could not access share operation.");

    return true;
}

bool ActivationFilter::handleFile(Microsoft::WRL::ComPtr<IInspectable> &inspectable)
{
    QStringList fileList;
    ComPtr<IFileActivatedEventArgs> fileArgs;
    HRESULT hr;
    hr = inspectable.As(&fileArgs);
    RETURN_FALSE_IF_FAILED("Could not access file activation args.");
    ComPtr<IVectorView<ABI::Windows::Storage::IStorageItem*>> items;
    fileArgs.Get()->get_Files(&items);
    RETURN_FALSE_IF_FAILED("Could not access file activation items.");
    quint32 count;
    hr = items->get_Size(&count);
    for (quint32 i = 0; i < count; ++i) {
        ComPtr<IStorageItem> item;
        hr = items->GetAt(i, &item);
        if (FAILED(hr))
            continue;
        HString path;
        hr = item.Get()->get_Path(path.GetAddressOf());
        if (FAILED(hr))
            continue;
        quint32 l;
        const wchar_t *raw = path.GetRawBuffer(&l);
        fileList.append(QString::fromWCharArray(raw, l));
    }

    emit fileActivation(fileList);
    return true;
}
