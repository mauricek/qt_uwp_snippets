#include "livetile.h"
#include <qfunctions_winrt.h>
#include <QDebug>

#include <windows.ui.notifications.h>
#include <windows.data.xml.dom.h>
#include <wrl.h>
#include <functional>

using namespace Microsoft::WRL;
using namespace Microsoft::WRL::Wrappers;

using namespace ABI::Windows::UI;
using namespace ABI::Windows::UI::Notifications;
using namespace ABI::Windows::Data::Xml::Dom;

class LiveTilePrivate
{
public:
    ComPtr<ITileUpdateManagerStatics> updateManager;
    ComPtr<ITileNotificationFactory> notificationFactory;
    ComPtr<IXmlDocument> currentTemplateDocument;
};


LiveTile::LiveTile(QObject *parent) : QObject(parent)
{
    m_priv = new LiveTilePrivate;

    HRESULT hr;
    hr = RoGetActivationFactory(HString::MakeReference(RuntimeClass_Windows_UI_Notifications_TileUpdateManager).Get(),
                                IID_PPV_ARGS(&m_priv->updateManager));
    Q_ASSERT_SUCCEEDED(hr);

    hr = RoGetActivationFactory(HString::MakeReference(RuntimeClass_Windows_UI_Notifications_TileNotification).Get(),
                                IID_PPV_ARGS(&m_priv->notificationFactory));
    Q_ASSERT_SUCCEEDED(hr);
}

QString LiveTile::text() const
{
    return m_text;
}

QString LiveTile::image() const
{
    return m_image;
}

QString LiveTile::title() const
{
    return m_title;
}

void LiveTile::setText(QString text)
{
    if (m_text == text)
        return;

    m_text = text;
    emit textChanged(text);
}

void LiveTile::setImage(QString image)
{
    if (m_image == image)
        return;

    m_image = image;
    emit imageChanged(image);
}

void LiveTile::update()
{
    HRESULT hr;

    // TemplateContent is a complete xml description of the visual
    // representation of a tile
    hr = m_priv->updateManager->GetTemplateContent(TileTemplateType_TileSquareText01,
                                                 &m_priv->currentTemplateDocument);
    if (FAILED(hr)) {
        qDebug("Could not access template document");
        return;
    }

    // Now we need to exchange/add content to the xml, ie the string
    // we want to have represented
    ComPtr<IXmlNodeList> nodes;
    hr = m_priv->currentTemplateDocument->GetElementsByTagName(HString::MakeReference(L"text").Get(), &nodes);

    {
        ComPtr<IXmlNode> node;
        hr = nodes->Item(0, &node);
        ComPtr<IXmlText> textNode;
        HString hTitle;
        hTitle.Set((PCWSTR)m_title.utf16(), m_title.size());
        hr = m_priv->currentTemplateDocument->CreateTextNode(hTitle.Get(), &textNode);
        ComPtr<IXmlNode> textNodeNode;
        hr = textNode.As(&textNodeNode);
        ComPtr<IXmlNode> appendedChild;
        hr = node->AppendChild(textNodeNode.Get(), &appendedChild);
    }

    {
        ComPtr<IXmlNode> node;
        hr = nodes->Item(1, &node);
        ComPtr<IXmlText> textNode;
        HString hString;
        hString.Set((PCWSTR)m_text.utf16(), m_text.size());
        hr = m_priv->currentTemplateDocument->CreateTextNode(hString.Get(), &textNode);
        ComPtr<IXmlNode> textNodeNode;
        hr = textNode.As(&textNodeNode);
        ComPtr<IXmlNode> appendedChild;
        hr = node->AppendChild(textNodeNode.Get(), &appendedChild);
    }
    ComPtr<ITileNotification> notification;
    hr = m_priv->notificationFactory->CreateTileNotification(m_priv->currentTemplateDocument.Get(),
                                                      &notification);

    ComPtr<ITileUpdater> tileUpdater;
    hr = m_priv->updateManager->CreateTileUpdaterForApplication(&tileUpdater);

    hr = tileUpdater->Update(notification.Get());
}

void LiveTile::setTitle(QString title)
{
    if (m_title == title)
        return;

    m_title = title;
    emit titleChanged(title);
}
