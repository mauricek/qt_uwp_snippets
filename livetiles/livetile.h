#ifndef LIVETILE_H
#define LIVETILE_H

#include <QObject>

// References:
// https://msdn.microsoft.com/en-us/library/windows/apps/windows.ui.notifications.tileupdatemanager.aspx
// https://msdn.microsoft.com/en-us/library/windows/apps/windows.ui.notifications.tiletemplatetype.aspx
// Available tiles:
// https://msdn.microsoft.com/en-us/library/windows/apps/windows.ui.notifications.tiletemplatetype.aspx

class LiveTile : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString text READ text WRITE setText NOTIFY textChanged)
    Q_PROPERTY(QString description READ description WRITE setDescription NOTIFY descriptionChanged)
    Q_PROPERTY(QString image READ image WRITE setImage NOTIFY imageChanged)
    QString m_text;

    QString m_description;

    QString m_image;

public:
    explicit LiveTile(QObject *parent = 0);

    QString text() const;
    QString description() const;
    QString image() const;

signals:
    void textChanged(QString text);
    void descriptionChanged(QString description);
    void imageChanged(QString image);

public slots:
    void setText(QString text);
    void setDescription(QString description);
    void setImage(QString image);
    void update();
};

#endif // LIVETILE_H
