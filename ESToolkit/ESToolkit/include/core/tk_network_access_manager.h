#ifndef TK_NETWORK_ACCESS_MANAGER_H
#define TK_NETWORK_ACCESS_MANAGER_H

#include <QNetworkAccessManager>
#include <QUrl>
#include <functional>

class QNetworkReply;
class QByteArray;
class QJsonDocument;


struct QueryUrl: public QUrl {
    QueryUrl(const QString& host, const QUrlQuery& query);
};

class TkNetworkAccessManager: public QNetworkAccessManager {
    Q_OBJECT
public:
                            TkNetworkAccessManager(QObject* parent=nullptr);

    QNetworkReply*          get(const QUrl& url, std::function<void(QNetworkReply*, const QJsonDocument&)> callback);
    QNetworkReply*          post(const QUrl& url, const QByteArray& data, std::function<void(QNetworkReply*, const QJsonDocument&)> callback);
    QNetworkReply*          post(const QUrl& url, std::function<void (QNetworkReply*, const QJsonDocument&)> callback);

     static bool            checkNetworkReplyError(QNetworkReply* reply);
     static QJsonDocument   jsonNetworkReply(QNetworkReply* reply);

private Q_SLOTS:

    void                    _onSslErrors(QNetworkReply* reply, QList<QSslError> errors);

};


#endif // TK_NETWORK_ACCESS_MANAGER_H

