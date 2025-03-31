#ifndef GOCONVERTER_H
#define GOCONVERTER_H
#include <QJsonValue>
#include <QJsonArray>
#include <QHash>
#include <QJsonObject>

class GoConverter{
private:
    QSet<QString> generatedClasses;
    QString capitalize(const QString &str) {
        if (str.isEmpty()) return str;
        QString result = str;
        result[0] = result[0].toUpper(); // 将首字母转换为大写
        return result;
    }
    void generateGoStruct(QString &code, const QJsonObject &object, const QString &structName) {
        if (generatedClasses.contains(structName)) {
            return; // 避免重复生成
        }
        generatedClasses.insert(structName);

        // 开始结构体定义
        code += QString("type %1 struct {\n").arg(structName);

        // 遍历字段并生成结构体字段
        for (auto it = object.begin(); it != object.end(); ++it) {
            QString fieldName = capitalize(it.key()); // Go 字段名首字母大写
            QString fieldType = getGoType(it.value(), it.key());
            QString jsonTag = QString("`json:\"%1\"`").arg(it.key()); // JSON 标签

            code += QString("    %1 %2 %3\n").arg(fieldName, fieldType, jsonTag);
        }

        // 结束结构体定义
        code += "}\n\n";

        // 如果有嵌套结构体，递归生成
        for (auto it = object.begin(); it != object.end(); ++it) {
            if (it.value().isObject()) {
                QString nestedStructName = capitalize(it.key());
                generateGoStruct(code, it.value().toObject(), nestedStructName);
            } else if (it.value().isArray()) {
                QJsonArray array = it.value().toArray();
                if (!array.empty() && array[0].isObject()) {
                    QString nestedStructName = capitalize(it.key())+"Item";
                    generateGoStruct(code, array[0].toObject(), nestedStructName);
                }
            }
        }
    }

    QString getGoType(const QJsonValue &value, const QString &fieldName) {
        switch (value.type()) {
        case QJsonValue::String:
            return "string";
        case QJsonValue::Double:
            return value.toDouble() == value.toInt() ? "int" : "float64";
        case QJsonValue::Bool:
            return "bool";
        case QJsonValue::Array: {
            QJsonArray array = value.toArray();
            if (!array.empty()) {
                QJsonValue firstElement = array[0];
                if (firstElement.isObject()) {
                    return QString("[]*%1").arg(capitalize(fieldName)+"Item");
                } else {
                    return QString("[]%1").arg(getGoType(firstElement, fieldName));
                }
            } else {
                return "[]interface{}";
            }
        }
        case QJsonValue::Object:
            return QString("*%1").arg(capitalize(fieldName));
        default:
            return "interface{}";
        }
    }

public:
    QString convert(const QJsonValue &value, const QString &structName){
        QString code;

        // 添加包声明和必要的导入
        code += "package main\n\n";
        code += "import \"encoding/json\"\n\n";

        // 生成主结构体
        if (value.isObject()) {
            generateGoStruct(code, value.toObject(), structName);
        }
        generatedClasses.clear();
        return code;
    }
};

#endif // GOCONVERTER_H
