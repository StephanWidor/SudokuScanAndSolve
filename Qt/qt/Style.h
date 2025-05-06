#pragma once
#include "base/colors.h"
#include <QColor>
#include <QObject>

namespace qt {

class Style : public QObject
{
    Q_OBJECT
public:
    Q_PROPERTY(int globalMargin READ globalMargin CONSTANT)

    Q_PROPERTY(QColor white READ white CONSTANT)
    Q_PROPERTY(QColor transparent READ transparent CONSTANT)
    Q_PROPERTY(QColor lightGray READ lightGray CONSTANT)
    Q_PROPERTY(QColor darkGray READ darkGray CONSTANT)
    Q_PROPERTY(QColor orange READ orange CONSTANT)
    Q_PROPERTY(QColor red READ red CONSTANT)
    Q_PROPERTY(QColor green READ green CONSTANT)

    explicit Style(QObject *parent = nullptr): QObject(parent) {}

    static constexpr int globalMargin() { return 30; }

    static QColor white() { return convert(colors::white()); }
    static QColor transparent() { return convert(colors::transparent()); }
    static QColor lightGray() { return convert(colors::lightGray()); }
    static QColor darkGray() { return convert(colors::darkGray()); }
    static QColor orange() { return convert(colors::orange()); }
    static QColor red() { return convert(colors::red()); }
    static QColor green() { return convert(colors::green()); }

    static QColor convert(const colors::Color &color) { return {color[0], color[1], color[2], color[3]}; }
};

}    // namespace qt
