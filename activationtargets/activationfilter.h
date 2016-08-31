#ifndef ACTIVATIONFILTER_H
#define ACTIVATIONFILTER_H

#include <QObject>
#include <QSharedDataPointer>
#include <wrl.h>

class ActivationFilterPrivate;

class ActivationFilter : public QObject
{
    Q_OBJECT
public:
    explicit ActivationFilter(QObject *parent = 0);

    bool eventFilter(QObject *, QEvent *e);

signals:
    void protocolActivation(const QString &);
    void shareActivationStarted();
    void fileActivation(const QStringList &);

public:
    QString activationText() const;

public slots:
    void finishShareActivation(bool error = false,
                               const QString &errorString = QString());

private:
    bool handleProtocol(Microsoft::WRL::ComPtr<IInspectable> &inspectable);
    bool handleShareEvent(Microsoft::WRL::ComPtr<IInspectable> &inspectable);
    bool handleFile(Microsoft::WRL::ComPtr<IInspectable> &inspectable);

    QSharedDataPointer<ActivationFilterPrivate> d;
};

#endif // ACTIVATIONFILTER_H
