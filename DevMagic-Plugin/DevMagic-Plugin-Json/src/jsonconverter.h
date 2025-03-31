#ifndef JSONCONVERTER_H
#define JSONCONVERTER_H

#include <QJsonValue>
#include <QJsonObject>
#include <QJsonArray>  // ✅ 完整QJsonArray定义
#include <QString>
#include <QXmlStreamWriter>  // ✅ 完整QXmlStreamWriter定义
#include <QRegularExpression>
#include "javaconverter.h"
#include "goconverter.h"
#include "xmlconverter.h"

class JsonConverter {
private:
    JavaConverter *javaConverter;
    GoConverter *goConverter;
    XmlConverter *xmlConverter;
public:
    JsonConverter(){
        javaConverter = new JavaConverter();
        goConverter = new GoConverter();
        xmlConverter = new XmlConverter();
    }
    QString convert(const QString lau,const QJsonValue &value, const QString rootName){
        if (lau == "Java"){
            return javaConverter->convert(value,rootName);
        }
        if (lau == "Go"){
            return goConverter->convert(value,rootName);
        }
        if (lau == "XML"){
            return xmlConverter->convert(value,rootName);
        }
        return "暂不支持的类型";
    }

    // ========== 辅助功能 ==========
    bool isJsonValid(const QJsonValue &value) {
        return value.isObject() || value.isArray();
    }

};

#endif // JSONCONVERTER_H
