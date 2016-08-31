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

ComPtr<ITileUpdateManagerStatics> updateManager;
ComPtr<ITileNotificationFactory> notificationFactory;
ComPtr<IXmlDocument> currentTemplateDocument;

LiveTile::LiveTile(QObject *parent) : QObject(parent)
{
//    Tile Erzeugen:
//    ITileNotificationFactory
//    -> CreateTileNotification (woher XML FIle?)

//    ITileUpdateManagerStatics
//    -> GetTemplateContent(enum), ergibt XML
//    -> CreateTileUpdaterForApplication

//    ergibt ITileUpdater
//    -> Update (Notification)

    HRESULT hr;
    hr = RoGetActivationFactory(HString::MakeReference(RuntimeClass_Windows_UI_Notifications_TileUpdateManager).Get(),
                                IID_PPV_ARGS(&updateManager));
    Q_ASSERT_SUCCEEDED(hr);

    hr = RoGetActivationFactory(HString::MakeReference(RuntimeClass_Windows_UI_Notifications_TileNotification).Get(),
                                IID_PPV_ARGS(&notificationFactory));
    Q_ASSERT_SUCCEEDED(hr);
}

QString LiveTile::text() const
{
    return m_text;
}

QString LiveTile::description() const
{
    return m_description;
}

QString LiveTile::image() const
{
    return m_image;
}

void LiveTile::setText(QString text)
{
    if (m_text == text)
        return;

    m_text = text;
    emit textChanged(text);
}

void LiveTile::setDescription(QString description)
{
    if (m_description == description)
        return;

    m_description = description;
    emit descriptionChanged(description);
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
//    hr = updateManager.Get()->GetTemplateContent(TileTemplateType_TileSquareText01,
//                                                 &currentTemplateDocument);
    hr = updateManager.Get()->GetTemplateContent(TileTemplateType_TileSquarePeekImageAndText01,
                                                 &currentTemplateDocument);
    if (FAILED(hr)) {
        qDebug("Could not access template document");
        return;
    }

    // Get content into XML File
    ComPtr<IXmlNodeList> nodes;
//    var tileAttributes = tileXml.getElementsByTagName("text");
//        tileAttributes[0].appendChild(tileXml.createTextNode("Hello World!"));
    hr = currentTemplateDocument.Get()->GetElementsByTagName(HString::MakeReference(L"text").Get(), &nodes);

    ComPtr<IXmlNode> node;
    hr = nodes.Get()->Item(0, &node);
    ComPtr<IXmlText> textNode;
    hr = currentTemplateDocument.Get()->CreateTextNode(HString::MakeReference(L"Wird References...").Get(), &textNode);
    ComPtr<IXmlNode> textNodeNode;
    hr = textNode.As(&textNodeNode);
    ComPtr<IXmlNode> appendedChild;
    hr = node.Get()->AppendChild(textNodeNode.Get(), &appendedChild);

#if 1 // Testing additional texts
    {
    ComPtr<IXmlNode> node;
    hr = nodes.Get()->Item(1, &node);
    ComPtr<IXmlText> textNode;
    hr = currentTemplateDocument.Get()->CreateTextNode(HString::MakeReference(L"Subtitle magic").Get(), &textNode);
    ComPtr<IXmlNode> textNodeNode;
    hr = textNode.As(&textNodeNode);
    ComPtr<IXmlNode> appendedChild;
    hr = node.Get()->AppendChild(textNodeNode.Get(), &appendedChild);
    }
#endif
#if 1 // For testing remote images
    {
        hr = currentTemplateDocument.Get()->GetElementsByTagName(HString::MakeReference(L"image").Get(), &nodes);
    ComPtr<IXmlNode> node;

    // get_Attributes IXmlNamedNodeMap
    // Check for values: https://msdn.microsoft.com/en-us/library/windows/apps/br212855.aspx
    // we need "src", maybe "id" as well
    // GetNamedItem src IXmlNode
    // Set text for src node
    // SetNamedItem (modified node)
    hr = nodes.Get()->Item(0, &node);
    ComPtr<IXmlText> textNode;
    hr = currentTemplateDocument.Get()->CreateTextNode(HString::MakeReference(L"http://colorlib.com/dazzling/wp-content/uploads/sites/6/2013/03/image-alignment-150x150.jpg").Get(), &textNode);
    ComPtr<IXmlNode> textNodeNode;
    hr = textNode.As(&textNodeNode);
    ComPtr<IXmlNode> appendedChild;
    hr = node.Get()->AppendChild(textNodeNode.Get(), &appendedChild);
    }
#endif
// https://colorlib.com/dazzling/wp-content/uploads/sites/6/2013/03/image-alignment-150x150.jpg
    ComPtr<ITileNotification> notification;
    hr = notificationFactory.Get()->CreateTileNotification(currentTemplateDocument.Get(),
                                                      &notification);

    ComPtr<ITileUpdater> tileUpdater;
    hr = updateManager.Get()->CreateTileUpdaterForApplication(&tileUpdater);

    hr = tileUpdater.Get()->Update(notification.Get());

    qDebug() << "Am ende:" << hr;
}
