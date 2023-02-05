#include "networkhelper.h"

#include <QJsonDocument>
#include <QTextStream>

#include <QtGlobal>
#if QT_VERSION < QT_VERSION_CHECK(5, 14, 0)
namespace Qt
{
    static auto endl = ::endl;
}
#endif

NetworkHelper::NetworkHelper()
{

}

const QString NetworkHelper::debugRequest(QNetworkRequest request)
{
	QString ret(request.url().toString()); //output the url
	const QList<QByteArray>& rawHeaderList(request.rawHeaderList());
	foreach (QByteArray rawHeader, rawHeaderList) { //traverse and output the header
		ret += rawHeader + ":" + request.rawHeader(rawHeader);
	}
	return ret;
}

QJsonObject NetworkHelper::string_to_hash(QString str)
{
	QJsonObject obj;
	QJsonDocument doc = QJsonDocument::fromJson(str.toUtf8());
	// check validity of the document
	if(!doc.isNull())
	{
		if(doc.isObject())
		{
			obj = doc.object();
		}
		else
		{
			qDebug() << "Document is not an object" << Qt::endl;
		}
	}
	else
	{
		qDebug() << "Invalid JSON...\n" << str << Qt::endl;
	}

	return obj;
}




