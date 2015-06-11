#include <tk_network_access_manager.h>
#include <elysian_debug.hpp>
#include <QJsonDocument>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <functional>


QueryUrl::QueryUrl(const QString &host, const QUrlQuery &query): QUrl(host) {
    setQuery(query);
}


TkNetworkAccessManager::TkNetworkAccessManager(QObject* parent):
    QNetworkAccessManager(parent)
{
    connect(this, &QNetworkAccessManager::sslErrors, this, &TkNetworkAccessManager::_onSslErrors);
}

QNetworkReply* TkNetworkAccessManager::get(const QUrl& url, std::function<void(QNetworkReply*, const QJsonDocument&)> callback) {
    QNetworkReply* resp = QNetworkAccessManager::get(QNetworkRequest(url));

    QObject::connect(resp, &QNetworkReply::finished, [=](void) {
        checkNetworkReplyError(resp);
        QJsonDocument&& json = jsonNetworkReply(resp);
        callback(resp, json);
        resp->deleteLater();
    });

    return resp;
}

QNetworkReply* TkNetworkAccessManager::post(const QUrl& url, const QByteArray& data, std::function<void(QNetworkReply*, const QJsonDocument&)> callback) {
    QNetworkReply* resp = QNetworkAccessManager::post(QNetworkRequest(url), data);

    QObject::connect(resp, &QNetworkReply::finished, [=](void) {
        checkNetworkReplyError(resp);
        QJsonDocument&& json = jsonNetworkReply(resp);
        callback(resp, json);
        resp->deleteLater();
    });

    return resp;
}

QNetworkReply* TkNetworkAccessManager::post(const QUrl& url, std::function<void (QNetworkReply*, const QJsonDocument&)> callback) {
    QByteArray dipshitData;
    return post(url, dipshitData, callback);
}



void TkNetworkAccessManager::_onSslErrors(QNetworkReply* reply, QList<QSslError> errors) {
    reply->ignoreSslErrors(errors);
}

bool TkNetworkAccessManager::checkNetworkReplyError(QNetworkReply *reply) {
    if(reply->error() != QNetworkReply::NoError) {
       QString error = reply->errorString();
       Debug::printf(Debug::DBG_WARNING, "QNetworkReply::Error: %s", Q_CSTR(error));
        return true;
    }
    return false;
}

QJsonDocument TkNetworkAccessManager::jsonNetworkReply(QNetworkReply *reply) {
    QJsonParseError parseError;

    QString string(reply->readAll().data());
    QJsonDocument&& json = QJsonDocument::fromJson(string.toUtf8(), &parseError);

    if(json.isNull()) {
        Debug::printf(Debug::DBG_WARNING, "Error parsing JSON network response. [Error: %s] [Raw: %s]", Q_CSTR(parseError.errorString()), Q_CSTR(string));
    }

    return json;
}

