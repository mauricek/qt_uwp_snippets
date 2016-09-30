#ifndef LIVETILE_H
#define LIVETILE_H

#include <QObject>

// References:
// https://msdn.microsoft.com/en-us/library/windows/apps/windows.ui.notifications.tileupdatemanager.aspx
// https://msdn.microsoft.com/en-us/library/windows/apps/windows.ui.notifications.tiletemplatetype.aspx
// Available tiles:
// https://msdn.microsoft.com/en-us/library/windows/apps/windows.ui.notifications.tiletemplatetype.aspx

class LiveTilePrivate;

class LiveTile : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString text READ text WRITE setText NOTIFY textChanged)
    Q_PROPERTY(QString title READ title WRITE setTitle NOTIFY titleChanged)
    Q_PROPERTY(QString image READ image WRITE setImage NOTIFY imageChanged)

public:
    explicit LiveTile(QObject *parent = 0);

    QString text() const;
    QString image() const;
    QString title() const;

signals:
    void textChanged(QString text);
    void imageChanged(QString image);

    void titleChanged(QString title);

public slots:
    void setText(QString text);
    void setImage(QString image);
    void update();
    void setTitle(QString title);

private:
    LiveTilePrivate *m_priv;
    QString m_text;
    QString m_image;
    QString m_title;
};

#endif // LIVETILE_H
