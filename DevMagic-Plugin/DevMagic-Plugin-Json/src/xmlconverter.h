#ifndef XMLCONVERTER_H
#define XMLCONVERTER_H

#include <QJsonValue>
#include <QJsonArray>
#include <QHash>
#include <QJsonObject>
#include <QString>
#include <QXmlStreamWriter>

class XmlConverter{
public:
    // ========== 核心转换功能 ==========
    QString convert(const QJsonValue &value, const QString &rootTag = "root") {
        QString xml;
        QXmlStreamWriter writer(&xml);
        writer.setAutoFormatting(true);
        writer.setAutoFormattingIndent(2);
        writer.writeStartDocument();
        convertToXml(writer, value, rootTag,0);
        writer.writeEndDocument();
        return xml;
    }
private:
    // ========== XML转换核心 ==========
    void convertToXml(QXmlStreamWriter &writer, const QJsonValue &value, const QString &tag, int depth) {
        if (depth > 100) { // 限制递归深度
            qWarning() << "Maximum recursion depth exceeded!";
            return;
        }

        switch (value.type()) {
        case QJsonValue::Object: {
            writer.writeStartElement(sanitizeTagName(tag));
            for (const auto &key : value.toObject().keys()) {
                convertToXml(writer, value.toObject().value(key), sanitizeTagName(key), depth + 1);
            }
            writer.writeEndElement();
            break;
        }
        case QJsonValue::Array: {
            QJsonArray array = value.toArray();
            if (!array.isEmpty()) {
                writer.writeStartElement(sanitizeTagName(tag));
                for (int i = 0; i < array.size(); ++i) {
                    QString itemTag = tag + "Item"; // 动态生成子标签名
                    convertToXml(writer, array[i], itemTag, depth + 1);
                }
                writer.writeEndElement();
            }
            break;
        }
        case QJsonValue::String:
            writer.writeTextElement(sanitizeTagName(tag), escapeXml(value.toString()));
            break;
        case QJsonValue::Bool:
            writer.writeTextElement(sanitizeTagName(tag), value.toBool() ? "true" : "false");
            break;
        case QJsonValue::Double: {
            double num = value.toDouble();
            if (num == static_cast<int>(num)) {
                writer.writeTextElement(sanitizeTagName(tag), QString::number(static_cast<int>(num)));
            } else {
                writer.writeTextElement(sanitizeTagName(tag), QString::number(num));
            }
            break;
        }
        case QJsonValue::Null:
            writer.writeEmptyElement(sanitizeTagName(tag));
            break;
        default:
            writer.writeTextElement(sanitizeTagName(tag), value.toString());
        }
    }


    QString sanitizeTagName(const QString &tag) const {
        QString tmp = tag.trimmed();
        if (tmp.isEmpty()) {
            return "unnamed"; // 默认标签名
        }
        tmp.replace(QRegularExpression("[^a-zA-Z0-9_]"), "_");
        tmp.replace(QRegularExpression("_+"), "_");
        if (!tmp[0].isLetter() && tmp[0] != '_') {
            tmp.prepend('_');
        }
        return tmp;
    }


    QString escapeXml(const QString &text) {
        QString result = text;
        result.replace("&", "&amp;");
        result.replace("<", "&lt;");
        result.replace(">", "&gt;");
        result.replace("\"", "&quot;");
        return result;
    }

};

#endif // XMLCONVERTER_H
