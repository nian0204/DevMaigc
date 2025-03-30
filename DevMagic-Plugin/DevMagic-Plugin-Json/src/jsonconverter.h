#ifndef JSONCONVERTER_H
#define JSONCONVERTER_H

#include <QJsonValue>
#include <QJsonObject>
#include <QJsonArray>  // ✅ 完整QJsonArray定义
#include <QString>
#include <QXmlStreamWriter>  // ✅ 完整QXmlStreamWriter定义
#include <QRegularExpression>

class JsonConverter {
public:
    // ========== 核心转换功能 ==========
    QString toXml(const QJsonValue &value, const QString &rootTag = "root") {
        QString xml;
        QXmlStreamWriter writer(&xml);
        writer.setAutoFormatting(true);
        writer.setAutoFormattingIndent(2);
        writer.writeStartDocument();
        convertToXml(writer, value, rootTag);
        writer.writeEndDocument();
        return xml;
    }

    QString toJavaClass(const QJsonObject &object, const QString &className = "JsonObject") {
        QString code;
        code += QString("import java.util.List;\n\npublic class %1 {\n").arg(className);
        addJavaFields(code, object, className);
        code += "}\n";
        return code;
    }

    QString toGoStruct(const QJsonObject &object, const QString &structName = "JsonObject") {
        QString code;
        code += QString("package main\n\nimport \"encoding/json\"\n\n");
        code += QString("type %1 struct {\n").arg(structName);
        addGoFields(code, object);
        code += "}\n";
        return code;
    }

    // ========== 辅助功能 ==========
    bool isJsonValid(const QJsonValue &value) {
        return value.isObject() || value.isArray();
    }

    QString escapeXml(const QString &text) {
        QString result = text;
        result.replace("&", "&amp;");
        result.replace("<", "&lt;");
        result.replace(">", "&gt;");
        result.replace("\"", "&quot;");
        return result;
    }

private:
    // ========== XML转换核心 ==========
    void convertToXml(QXmlStreamWriter &writer, const QJsonValue &value, const QString &tag) {
        switch (value.type()) {
        case QJsonValue::Object: {
            writer.writeStartElement(tag);
            for (auto it = value.toObject().begin(); it != value.toObject().end(); ++it) {
                convertToXml(writer, it.value(), sanitizeTagName(it.key()));
            }
            writer.writeEndElement();
            break;
        }
        case QJsonValue::Array: {
            QJsonArray array = value.toArray();
            writer.writeStartElement(tag);
            for (int i = 0; i < array.size(); ++i) {  // ✅ 索引遍历避免前向声明问题
                convertToXml(writer, array[i], "item");
            }
            writer.writeEndElement();
            break;
        }
        case QJsonValue::String:
            writer.writeTextElement(tag, escapeXml(value.toString()));
            break;
        case QJsonValue::Bool:
            writer.writeTextElement(tag, value.toBool() ? "true" : "false");
            break;
        case QJsonValue::Null:
            writer.writeEmptyElement(tag);
            break;
        default:  // 处理数字、嵌套值等
            writer.writeTextElement(tag, value.toString());
        }
    }

    // ========== Java代码生成 ==========
    void addJavaFields(QString &code, const QJsonObject &object, const QString &className) {
        QHash<QString, QString> fieldTypes;
        for (auto it = object.begin(); it != object.end(); ++it) {
            QString fieldName = toCamelCase(it.key());
            QString type = getJavaType(it.value());
            fieldTypes[fieldName] = type;
        }

        // 生成字段
        for (auto it = fieldTypes.begin(); it != fieldTypes.end(); ++it) {
            code += QString("    private %1 %2;\n").arg(it.value(), it.key());
        }

        // 生成构造方法
        code += QString("\n    public %1(").arg(className);
        QString params;
        for (auto it = fieldTypes.begin(); it != fieldTypes.end(); ++it) {
            params += QString("%1 %2, ").arg(it.value(), it.key());
        }
        if (!params.isEmpty()) params.chop(2);
        code += params + ") {\n";
        for (auto it = fieldTypes.begin(); it != fieldTypes.end(); ++it) {
            code += QString("        this.%1 = %1;\n").arg(it.key());
        }
        code += "    }\n";

        // 生成Getter/Setter
        for (auto it = fieldTypes.begin(); it != fieldTypes.end(); ++it) {
            code += QString("\n    public %1 get%2() {\n        return %3;\n    }\n")
            .arg(it.value(), it.key().left(1).toUpper() + it.key().mid(1), it.key());
            code += QString("    public void set%2(%1 %3) {\n        this.%3 = %3;\n    }\n")
                        .arg(it.value(), it.key().left(1).toUpper() + it.key().mid(1), it.key());
        }
    }

    // ========== Go代码生成 ==========
    void addGoFields(QString &code, const QJsonObject &object) {
        for (auto it = object.begin(); it != object.end(); ++it) {
            QString fieldName = toGoFieldName(it.key());
            QString jsonTag = QString("`json:\"%1\"`").arg(it.key());
            QString type = getGoType(it.value());
            code += QString("    %1 %2 %3\n").arg(fieldName, type, jsonTag);
        }
    }

    // ========== 类型映射 ==========
    QString getJavaType(const QJsonValue &value) {
        switch (value.type()) {
        case QJsonValue::String: return "String";
        case QJsonValue::Double:
            return value.toDouble() == value.toInt() ? "int" : "double";
        case QJsonValue::Bool: return "boolean";
        case QJsonValue::Array: return "List<Object>";
        case QJsonValue::Object: return "JsonObject";  // 假设嵌套对象
        default: return "Object";
        }
    }

    QString getGoType(const QJsonValue &value) {
        switch (value.type()) {
        case QJsonValue::String: return "string";
        case QJsonValue::Double:
            return value.toDouble() == value.toInt() ? "int" : "float64";
        case QJsonValue::Bool: return "bool";
        case QJsonValue::Array: return "[]interface{}";
        case QJsonValue::Object: return "map[string]interface{}";
        default: return "interface{}";
        }
    }

    // ========== 命名转换 ==========
    QString toCamelCase(const QString &str) {
        if (str.isEmpty()) return str;
        QStringList parts = str.split(QRegularExpression("[^a-zA-Z0-9]"));
        QString result = parts[0].toLower();
        for (int i = 1; i < parts.size(); ++i) {
            result += parts[i].left(1).toUpper() + parts[i].mid(1).toLower();
        }
        return result;
    }

    QString toGoFieldName(const QString &str) {
        return str.left(1).toUpper() + str.mid(1).toLower();
    }

    QString sanitizeTagName(const QString &tag) const {
        QString tmp = tag;
        return tmp.replace(QRegularExpression("[^a-zA-Z0-9_]"), "_"); // ✅ const版本
    }
};

#endif // JSONCONVERTER_H
