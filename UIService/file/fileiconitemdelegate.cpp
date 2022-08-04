/*
* Copyright (C) 2019 ~ 2020 Uniontech Software Technology Co., Ltd.
*
* Author:     zhangliangliang <zhangliangliang@uniontech.com>
* Maintainer: zhangliangliang <zhangliangliang@uniontech.com>
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* any later version.
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
* GNU General Public License for more details.
* You should have received a copy of the GNU General Public License
* along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
#include "fileiconitemdelegate.h"
#include "fileitem.h"

#include <QLabel>
#include <QPainter>
#include <QTextEdit>
#include <QLineEdit>
#include <QTextBlock>
#include <QAbstractTextDocumentLayout>
#include <QApplication>
#include <QAbstractItemView>
#include <QVBoxLayout>
#include <dgiosettings.h>
#include <DApplicationHelper>
#include <DStyleOption>
#include <DStyle>
#include <DWidget>
#include <QTextLayout>
#include <private/qtextengine_p.h>
#include <QThreadStorage>
#include <DApplication>
#include <QPainterPath>

DWIDGET_USE_NAMESPACE

#define ICON_SPACING 16
#define ICON_MODE_RECT_RADIUS TEXT_PADDING
#define ICON_MODE_BACK_RADIUS 18

// begin file item global define
#define TEXT_PADDING 4
#define ICON_MODE_ICON_SPACING 5
#define COLUMU_PADDING 10
//#define LEFT_PADDING 10
//#define RIGHT_PADDING 10
// end

// begin file view global define
//#define LIST_MODE_LEFT_MARGIN 20
//#define LIST_MODE_RIGHT_MARGIN 20
// end

#define MAX_FILE_NAME_CHAR_COUNT 255

//#define MAX_PROPERTY_DIALOG_NUMBER 16

//#define DISCBURN_STAGING "discburn"

//QString trimmedEnd(QString str)
//{
//    while (!str.isEmpty()) {
//        switch (str.at(str.count() - 1).toLatin1()) {
//        case '\t':
//        case '\n':
//        case '\r':
//        case '\v':
//        case '\f':
//        case ' ':
//            str.chop(1);
//            break;
//        default:
//            return str;
//        }
//    }

//    return str;
//}

QRectF boundingRect(const QList<QRectF> &rects)
{
    QRectF bounding;

    if (rects.isEmpty())
        return bounding;

    bounding = rects.first();

    for (const QRectF &r : rects) {
        if (r.top() < bounding.top()) {
            bounding.setTop(r.top());
        }

        if (r.left() < bounding.left()) {
            bounding.setLeft(r.left());
        }

        if (r.right() > bounding.right()) {
            bounding.setRight(r.right());
        }

        if (r.bottom() > bounding.bottom()) {
            bounding.setBottom(r.bottom());
        }
    }

    return bounding;
}

QPainterPath boundingPath(QList<QRectF> rects, qreal radius, qreal padding)
{
    QPainterPath path;
    const QMarginsF margins(radius + padding, 0, radius + padding, 0);

    if (rects.count() == 1) {
        path.addRoundedRect(rects.first().marginsAdded(margins).adjusted(0, -padding, 0, padding), radius, radius);

        return path;
    }

    auto joinRightCorner = [&](const QRectF &rect, const QRectF &prevRect, const QRectF &nextRect) {
        if (Q_LIKELY(prevRect.isValid())) {
            qreal new_radius = qMin(radius, qAbs(prevRect.right() - rect.right()) / 2);

            if (rect.right() > prevRect.right()) {
                path.arcTo(rect.right() - new_radius * 2, rect.y() - padding, new_radius * 2, new_radius * 2, 90, -90);
            } else if (rect.right() < prevRect.right()) {
                path.arcTo(rect.right(), rect.y() + padding, new_radius * 2, new_radius * 2, 90, 90);
            }
        } else {
            path.arcTo(rect.right() - radius * 2, rect.y() - padding, radius * 2, radius * 2, 90, -90);
        }

        if (Q_LIKELY(nextRect.isValid())) {
            qreal new_radius = qMin(radius, qAbs(nextRect.right() - rect.right()) / 2);

            if (rect.right() > nextRect.right()) {
                path.arcTo(rect.right() - new_radius * 2, rect.bottom() - new_radius * 2 + padding, new_radius * 2, new_radius * 2, 0, -90);
            } else if (rect.right() < nextRect.right()) {
                path.arcTo(rect.right(), rect.bottom() - new_radius * 2 - padding, new_radius * 2, new_radius * 2, 180, 90);
            }
        } else {
            path.arcTo(rect.right() - radius * 2, rect.bottom() - radius * 2 + padding, radius * 2, radius * 2, 0, -90);
        }
    };

    auto joinLeftCorner = [&](const QRectF &rect, const QRectF &prevRect, const QRectF &nextRect) {
        if (Q_LIKELY(nextRect.isValid())) {
            qreal new_radius = qMin(radius, qAbs(nextRect.x() - rect.x()) / 2);

            if (rect.x() > nextRect.x()) {
                path.arcTo(rect.x() - new_radius * 2, rect.bottom() - new_radius * 2 - padding, new_radius * 2, new_radius * 2, 270, 90);
            } else if (rect.x() < nextRect.x()) {
                path.arcTo(rect.x(), rect.bottom() - new_radius * 2 + padding, new_radius * 2, new_radius * 2, 270, -90);
            }
        } else {
            path.arcTo(rect.x(), rect.bottom() - radius * 2 + padding, radius * 2, radius * 2, 270, -90);
        }

        if (Q_LIKELY(prevRect.isValid())) {
            qreal new_radius = qMin(radius, qAbs(prevRect.x() - rect.x()) / 2);

            if (rect.x() > prevRect.x()) {
                path.arcTo(rect.x() - new_radius * 2, rect.y() + padding, new_radius * 2, new_radius * 2, 0, 90);
            } else if (rect.x() < prevRect.x()) {
                path.arcTo(rect.x(), rect.y() - padding, new_radius * 2, new_radius * 2, 180, -90);
            }
        } else {
            path.arcTo(rect.x(), rect.y() - padding, radius * 2, radius * 2, 180, -90);
        }
    };

    auto preproccess = [&](QRectF &rect, const QRectF &prev) {
        if (qAbs(rect.x() - prev.x()) < radius) {
            rect.setLeft(prev.x());
        }

        if (qAbs(rect.right() - prev.right()) < radius) {
            rect.setRight(prev.right());
        }
    };

    for (int i = 1; i < rects.count(); ++i) {
        preproccess(rects[i], rects.at(i - 1));
    }

    const QRectF &first = rects.first().marginsAdded(margins);

    path.arcMoveTo(first.right() - radius * 2, first.y() - padding, radius * 2, radius * 2, 90);
    joinRightCorner(first, QRectF(), rects.at(1).marginsAdded(margins));

    for (int i = 1; i < rects.count() - 1; ++i) {
        joinRightCorner(rects.at(i) + margins, rects.at(i - 1).marginsAdded(margins), rects.at(i + 1).marginsAdded(margins));
    }

    QRectF last = rects.last();
    const QRectF &prevRect = rects.at(rects.count() - 2);

    joinRightCorner(last.marginsAdded(margins), prevRect.marginsAdded(margins), QRectF());
    joinLeftCorner(last.marginsAdded(margins), prevRect.marginsAdded(margins), QRectF());

    for (int i = rects.count() - 2; i > 0; --i) {
        joinLeftCorner(rects.at(i) + margins, rects.at(i - 1).marginsAdded(margins), rects.at(i + 1).marginsAdded(margins));
    }

    joinLeftCorner(first, QRectF(), rects.at(1).marginsAdded(margins));

    path.closeSubpath();

    return path;
}

class TagTextFormat : public QTextCharFormat
{
public:
    TagTextFormat(int objectType, const QList<QColor> &colors, const QColor &borderColor);

    QList<QColor> colors() const;
    QColor borderColor() const;
    qreal diameter() const;
};

TagTextFormat::TagTextFormat(int objectType, const QList<QColor> &colors, const QColor &borderColor)
{
    setObjectType(objectType);
    setProperty(QTextFormat::UserProperty + 1, QVariant::fromValue(colors));
    setProperty(QTextFormat::UserProperty + 2, borderColor);
}

QList<QColor> TagTextFormat::colors() const
{
    return qvariant_cast<QList<QColor>>(property(QTextFormat::UserProperty + 1));
}

QColor TagTextFormat::borderColor() const
{
    return colorProperty(QTextFormat::UserProperty + 2);
}

qreal TagTextFormat::diameter() const
{
    return 12;
}

FileTagObjectInterface::FileTagObjectInterface()
    : QObject()
{
}

QSizeF FileTagObjectInterface::intrinsicSize(QTextDocument *doc, int posInDocument, const QTextFormat &format)
{
    Q_UNUSED(posInDocument)
    Q_UNUSED(doc)

    const TagTextFormat &f = static_cast<const TagTextFormat &>(format);
    const QList<QColor> &colors = f.colors();
    const int diameter = int(f.diameter());

    if (colors.size() == 1)
        return QSizeF(diameter, diameter);

    return QSizeF(diameter + (colors.size() - 1) * diameter / 2.0, diameter);
}

void FileTagObjectInterface::drawObject(QPainter *painter, const QRectF &rect, QTextDocument *doc, int posInDocument, const QTextFormat &format)
{
    Q_UNUSED(posInDocument)
    Q_UNUSED(doc)

    const TagTextFormat &f = static_cast<const TagTextFormat &>(format);
    const QList<QColor> &colors = f.colors();
    const QColor borderColor = f.borderColor();
    qreal diameter = f.diameter();
    const qreal padding = diameter / 10.0;
    QRectF boundingRect = rect.marginsRemoved(QMarginsF(padding, padding, padding, padding));

    diameter -= padding * 2;

    bool antialiasing = painter->testRenderHint(QPainter::Antialiasing);
    const QPen pen = painter->pen();

    painter->setRenderHint(QPainter::Antialiasing);
    painter->setPen(QPen(borderColor, 1));

    for (const QColor &color : colors) {
        QPainterPath circle;

        circle.addEllipse(QRectF(QPointF(boundingRect.right() - diameter, boundingRect.top()), boundingRect.bottomRight()));
        painter->fillPath(circle, color);
        painter->drawPath(circle);
        boundingRect.setRight(boundingRect.right() - diameter / 2);
    }

    painter->setPen(pen);
    painter->setRenderHint(QPainter::Antialiasing, antialiasing);
}

ExpandedItem::ExpandedItem(FileIconItemDelegate *d, DWidget *parent)
    : DWidget(parent)
    , delegate(d)
{
}

bool ExpandedItem::event(QEvent *ee)
{
    if (ee->type() == QEvent::DeferredDelete) {
        if (!canDeferredDelete) {
            ee->accept();

            return true;
        }
    }

    return DWidget::event(ee);
}

qreal ExpandedItem::opacity() const
{
    return m_opactity;
}

void ExpandedItem::setOpacity(qreal opacity)
{
    if (qFuzzyCompare(opacity, m_opactity))
        return;

    m_opactity = opacity;

    update();
}

void ExpandedItem::paintEvent(QPaintEvent *)
{
    QPainter pa(this);

    pa.setOpacity(m_opactity);
    pa.setPen(option.palette.color(QPalette::BrightText));
    pa.setFont(option.font);

    if (!iconPixmap.isNull()) {
        pa.drawPixmap(iconGeometry().topLeft().toPoint(), iconPixmap);
    }

    if (option.text.isEmpty())
        return;

    const QMargins &margins = contentsMargins();
    QRect label_rect(TEXT_PADDING + margins.left(), margins.top() + iconHeight + TEXT_PADDING + ICON_MODE_ICON_SPACING,
                     width() - TEXT_PADDING * 2 - margins.left() - margins.right(), INT_MAX);
    const QList<QRectF> &lines = delegate->drawText(index, &pa, option.text, label_rect, ICON_MODE_RECT_RADIUS,
                                                    option.palette.brush(QPalette::Normal, QPalette::Highlight),
                                                    QTextOption::WrapAtWordBoundaryOrAnywhere,
                                                    option.textElideMode, Qt::AlignCenter);

    textBounding = boundingRect(lines).toRect();
}

QSize ExpandedItem::sizeHint() const
{
    return QSize(width(), int(textGeometry().bottom() + contentsMargins().bottom()));
}

int ExpandedItem::heightForWidth(int width) const
{
    if (width != this->width()) {
        textBounding = QRect();
    }

    return int(textGeometry(width).bottom() + contentsMargins().bottom());
}

void ExpandedItem::setIconPixmap(const QPixmap &pixmap, int height)
{
    iconPixmap = pixmap;
    iconHeight = height;
    update();
}

QRectF ExpandedItem::iconGeometry() const
{
    const QRect &content_rect = contentsRect();

    if (iconPixmap.isNull()) {
        QRectF rect(content_rect);

        rect.setHeight(iconHeight);

        return rect;
    }

    QRectF icon_rect(QPointF((content_rect.width() - iconPixmap.width() / iconPixmap.devicePixelRatio()) / 2.0,
                             (iconHeight - iconPixmap.height() / iconPixmap.devicePixelRatio()) / 2.0 + content_rect.top()),
                     iconPixmap.size() / iconPixmap.devicePixelRatio());

    return icon_rect;
}

QRectF ExpandedItem::textGeometry(int width) const
{
    if (textBounding.isEmpty() && !option.text.isEmpty()) {
        const QMargins &margins = contentsMargins();

        if (width < 0)
            width = this->width();

        width -= (margins.left() + margins.right());

        QRect label_rect(TEXT_PADDING + margins.left(), iconHeight + TEXT_PADDING + ICON_MODE_ICON_SPACING + margins.top(), width - TEXT_PADDING * 2, INT_MAX);
        const QList<QRectF> &lines = delegate->drawText(index, nullptr, option.text, label_rect, ICON_MODE_RECT_RADIUS, Qt::NoBrush,
                                                        QTextOption::WrapAtWordBoundaryOrAnywhere, option.textElideMode, Qt::AlignCenter);

        textBounding = boundingRect(lines);
    }

    return textBounding;
    ;
}

class FileIconItemDelegatePrivate
{
public:
    FileIconItemDelegatePrivate(FileIconItemDelegate *qq);

    QSize textSize(const QString &text, const QFontMetrics &metrics, int lineHeight = -1) const;
    void drawText(QPainter *painter, const QRect &r, const QString &text,
                  int lineHeight = -1, QRect *br = Q_NULLPTR) const;
    QPixmap getFileIconPixmap(const QModelIndex &index, const QIcon &icon, const QSize &icon_size, QIcon::Mode mode, qreal devicePixelRatio) const;

    QPointer<ExpandedItem> expandedItem;

    //    mutable QHash<QString, QString> elideMap;
    //    mutable QHash<QString, QString> wordWrapMap;
    //    mutable QHash<QString, int> textHeightMap;
    mutable QModelIndex expandedIndex;
    mutable QModelIndex lastAndExpandedInde;

    QList<int> iconSizes;
    /// default icon size is 64px.
    int currentIconSizeIndex = 1;

    QColor focusTextBackgroundBorderColor;
    bool enabledTextShadow = false;
    // 最后一次绘制item时是否画了背景
    mutable bool drawTextBackgroundOnLast = true;

    static int textObjectType;
    static FileTagObjectInterface *textObjectInterface;

    void init();
    void _q_onRowsInserted(const QModelIndex &parent, int first, int last);
    void _q_onRowsRemoved(const QModelIndex &parent, int first, int last);

    char dumy[2];
    FileIconItemDelegate *q_ptr;
    QTextDocument *document = nullptr;

    mutable FileIconItem *fileIconItem = nullptr;

    mutable QModelIndex editingIndex;
    QSize itemSizeHint; // = QSize(190, 190); //mod by zll
    int textLineHeight = 20; //mod by zll

    Q_DECLARE_PUBLIC(FileIconItemDelegate)
};

int FileIconItemDelegatePrivate::textObjectType = QTextFormat::UserObject + 1;
FileTagObjectInterface *FileIconItemDelegatePrivate::textObjectInterface = new FileTagObjectInterface();

FileIconItemDelegatePrivate::FileIconItemDelegatePrivate(FileIconItemDelegate *qq)
    : q_ptr(qq)
{
}

QSize FileIconItemDelegatePrivate::textSize(const QString &text, const QFontMetrics &metrics, int lineHeight) const
{
    QString str = text;

    if (str.endsWith('\n'))
        str.chop(1);

    int max_width = 0;
    int height = 0;

    for (const QString &line : str.split('\n')) {
        max_width = qMax(metrics.width(line), max_width);

        if (lineHeight > 0)
            height += lineHeight;
        else
            height += metrics.height();
    }

    return QSize(max_width, height);
}

void FileIconItemDelegatePrivate::drawText(QPainter *painter, const QRect &r, const QString &text, int lineHeight, QRect *br) const
{
    if (lineHeight <= 0)
        lineHeight = textLineHeight;

    QString str = text;

    if (str.endsWith('\n'))
        str.chop(1);

    QRect textRect = QRect(0, r.top(), 0, 0);

    for (const QString &line : str.split('\n')) {
        QRect br;

        painter->drawText(r.left(), textRect.bottom(), r.width(), lineHeight, Qt::AlignCenter, line, &br);
        textRect.setWidth(qMax(textRect.width(), br.width()));
        textRect.setBottom(textRect.bottom() + lineHeight);
    }

    if (br)
        *br = textRect;
}

QPixmap FileIconItemDelegatePrivate::getFileIconPixmap(const QModelIndex &index, const QIcon &icon, const QSize &icon_size, QIcon::Mode mode, qreal devicePixelRatio) const
{
    Q_UNUSED(index)
    Q_Q(const FileIconItemDelegate);

    QPixmap pixmap = q->getIconPixmap(icon, icon_size, devicePixelRatio, mode);
    QPainter painter(&pixmap);

    /// draw file additional icon

    QList<QRectF> cornerGeometryList = q->getCornerGeometryList(QRect(QPoint(0, 0), icon_size), icon_size / 3);
    //    const QList<QIcon> &cornerIconList = q->parent()->additionalIcon(index);

    //    for (int i = 0; i < cornerIconList.count(); ++i) {
    //        cornerIconList.at(i).paint(&painter, cornerGeometryList.at(i).toRect());
    //    }

    return pixmap;
}

void FileIconItemDelegatePrivate::init()
{
    Q_Q(FileIconItemDelegate);
    //Q_UNUSED(q)

    //q->connect(q, &FileIconItemDelegate::commitData, qobject_cast<FileManageWidget *>(q->parent()->parent()), &FileManageWidget::handleCommitData);
    q->connect(q->parent(), &QAbstractItemView::iconSizeChanged, q, &FileIconItemDelegate::setItemSizeHint);

    QAbstractItemModel *model = q->parent()->model();
    Q_ASSERT(model);

    q->connect(model, SIGNAL(rowsInserted(QModelIndex, int, int)), SLOT(_q_onRowsInserted(QModelIndex, int, int)));
    q->connect(model, SIGNAL(rowsRemoved(QModelIndex, int, int)), SLOT(_q_onRowsRemoved(QModelIndex, int, int)));

    textLineHeight = q->parent()->fontMetrics().height();
}

void FileIconItemDelegatePrivate::_q_onRowsInserted(const QModelIndex &parent, int first, int last)
{
    if (editingIndex.isValid() && first <= editingIndex.row() && !editingIndex.parent().isValid()) {
        editingIndex = parent.child(editingIndex.row() + last - first + 1, editingIndex.column());
    }
}

void FileIconItemDelegatePrivate::_q_onRowsRemoved(const QModelIndex &parent, int first, int last)
{
    if (editingIndex.isValid() && first <= editingIndex.row() && !editingIndex.parent().isValid()) {
        editingIndex = parent.child(editingIndex.row() - last + first - 1, editingIndex.column());
    }
}

FileIconItemDelegate::FileIconItemDelegate(QAbstractItemView *parent)
    : BaseListViewItemDelegate(parent)
    //    , m_checkedIcon(QIcon::fromTheme("emblem-checked"))
    , d_ptr(new FileIconItemDelegatePrivate(this))
{
    Q_D(FileIconItemDelegate);

    d->expandedItem = new ExpandedItem(this, nullptr);
    d->expandedItem->setAttribute(Qt::WA_TransparentForMouseEvents);
    d->expandedItem->canDeferredDelete = false;
    d->expandedItem->setContentsMargins(0, 0, 0, 0);
    /// prevent flash when first call show()
    d->expandedItem->setFixedWidth(0);

    d->iconSizes << 48 << 64 << 96 << 128 << 256;

    //connect(parent, &DFileViewHelper::triggerEdit, this, &DIconItemDelegate::onTriggerEdit);

    parent->setIconSize(iconSizeByIconSizeLevel());
    setItemSizeHint(); //add zll 设置初始大小

    connect(qApp, &DApplication::fontChanged, this, &FileIconItemDelegate::fontChanged);
}

FileIconItemDelegate::~FileIconItemDelegate()
{
    Q_D(FileIconItemDelegate);

    if (d->expandedItem) {
        d->expandedItem->setParent(nullptr);
        d->expandedItem->canDeferredDelete = true;
        delete d->expandedItem; //->deleteLater();
    }

    delete d;
}

QAbstractItemView *FileIconItemDelegate::parent() const
{
    return qobject_cast<QAbstractItemView *>(QObject::parent());
}

QList<QRectF> FileIconItemDelegate::drawText(const QModelIndex &index, QPainter *painter, QTextLayout *layout, const QRectF &boundingRect,
                                             qreal radius, const QBrush &background, QTextOption::WrapMode wordWrap,
                                             Qt::TextElideMode mode, int flags, const QColor &shadowColor) const
{
    initTextLayout(index, layout);

    QList<QRectF> boundingRegion;
    elideText(layout, boundingRect.size(), wordWrap, mode, d_func()->textLineHeight, flags, nullptr,
              painter, boundingRect.topLeft(), shadowColor, QPointF(0, 1),
              background, radius, &boundingRegion);

    return boundingRegion;
}

QList<QRectF> FileIconItemDelegate::drawText(const QModelIndex &index, QPainter *painter, const QString &text, const QRectF &boundingRect,
                                             qreal radius, const QBrush &background, QTextOption::WrapMode wordWrap,
                                             Qt::TextElideMode mode, int flags, const QColor &shadowColor) const
{
    QTextLayout layout;

    layout.setText(text);

    if (painter)
        layout.setFont(painter->font());

    return drawText(index, painter, &layout, boundingRect, radius, background, wordWrap, mode, flags, shadowColor);
}

QPixmap FileIconItemDelegate::getIconPixmap(const QIcon &icon, const QSize &size, qreal pixelRatio = 1.0, QIcon::Mode mode, QIcon::State state)
{
    // ###(zccrs): 开启Qt::AA_UseHighDpiPixmaps后，QIcon::pixmap会自动执行 pixmapSize *= qApp->devicePixelRatio()
    //             而且，在有些QIconEngine的实现中，会去调用另一个QIcon::pixmap，导致 pixmapSize 在这种嵌套调用中越来越大
    //             最终会获取到一个是期望大小几倍的图片，由于图片太大，会很快将 QPixmapCache 塞满，导致后面再调用QIcon::pixmap
    //             读取新的图片时无法缓存，非常影响图片绘制性能。此处在获取图片前禁用 Qt::AA_UseHighDpiPixmaps，自行处理图片大小问题
    bool useHighDpiPixmaps = qApp->testAttribute(Qt::AA_UseHighDpiPixmaps);
    qApp->setAttribute(Qt::AA_UseHighDpiPixmaps, false);

    if (icon.isNull())
        return QPixmap();

    QSize icon_size = icon.actualSize(size, mode, state);
    //取出icon的真实大小
    QList<QSize> iconSizeList = icon.availableSizes();
    QSize iconRealSize;
    if (iconSizeList.count() > 0)
        iconRealSize = iconSizeList.first();
    else
        iconRealSize = icon_size;
    if (iconRealSize.width() <= 0 || iconRealSize.height() <= 0) {
        //        return icon.pixmap(iconRealSize);
        return icon.pixmap(icon_size);
    }

    //确保特殊比例icon的高或宽不为0
    bool isSpecialSize = false;
    QSize tempSize(size.width(), size.height());
    while (icon_size.width() < 1) {
        tempSize.setHeight(tempSize.height() * 2);
        icon_size = icon.actualSize(tempSize, mode, state);
        isSpecialSize = true;
    }
    while (icon_size.height() < 1) {
        tempSize.setWidth(tempSize.width() * 2);
        icon_size = icon.actualSize(tempSize, mode, state);
        isSpecialSize = true;
    }

    if ((icon_size.width() > size.width() || icon_size.height() > size.height()) && !isSpecialSize)
        icon_size.scale(size, Qt::KeepAspectRatio);

    QSize pixmapSize = icon_size * pixelRatio;
    QPixmap px = icon.pixmap(pixmapSize, mode, state);

    // restore the value
    qApp->setAttribute(Qt::AA_UseHighDpiPixmaps, useHighDpiPixmaps);

    //    if (icon_size.width() == 0) {
    //        icon_size.setWidth(1);
    //    }
    //    if (icon_size.height() == 0) {
    //        icon_size.setHeight(1);
    //    }

    //    icon_size = icon_size * pixelRatio;
    //    px = px.scaled(icon_size.width(), icon_size.height(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation);

    //约束特殊比例icon的尺寸
    if (isSpecialSize) {
        if (px.width() > size.width() * pixelRatio) {
            px = px.scaled(int(size.width() * pixelRatio), px.height(), Qt::IgnoreAspectRatio);
        } else if (px.height() > size.height() * pixelRatio) {
            px = px.scaled(px.width(), int(size.height() * pixelRatio), Qt::IgnoreAspectRatio);
        }
    }

    if (px.width() > icon_size.width() * pixelRatio) {
        px.setDevicePixelRatio(px.width() / qreal(icon_size.width()));
    } else if (px.height() > icon_size.height() * pixelRatio) {
        px.setDevicePixelRatio(px.height() / qreal(icon_size.height()));
    } else {
        px.setDevicePixelRatio(pixelRatio);
    }

    return px;
}

QList<QRectF> FileIconItemDelegate::getCornerGeometryList(const QRectF &baseRect, const QSizeF &cornerSize) const
{
    QList<QRectF> list;
    int offset = int(baseRect.width() / 8);
    const QSizeF &offset_size = cornerSize / 2;

    list << QRectF(QPointF(baseRect.right() - offset - offset_size.width(),
                           baseRect.bottom() - offset - offset_size.height()),
                   cornerSize);
    list << QRectF(QPointF(baseRect.left() + offset - offset_size.width(), list.first().top()), cornerSize);
    list << QRectF(QPointF(list.at(1).left(), baseRect.top() + offset - offset_size.height()), cornerSize);
    list << QRectF(QPointF(list.first().left(), list.at(2).top()), cornerSize);

    return list;
}

void FileIconItemDelegate::elideText(QTextLayout *layout, const QSizeF &size, QTextOption::WrapMode wordWrap,
                                     Qt::TextElideMode mode, qreal lineHeight, int flags, QStringList *lines,
                                     QPainter *painter, QPointF offset, const QColor &shadowColor, const QPointF &shadowOffset,
                                     const QBrush &background, qreal backgroundRadius, QList<QRectF> *boundingRegion)
{
    //    qreal height = 0;
    bool drawBackground = background.style() != Qt::NoBrush;
    bool drawShadow = shadowColor.isValid();

    QString text = layout->engine()->hasFormats() ? layout->engine()->block.text() : layout->text();
    QTextOption &text_option = *const_cast<QTextOption *>(&layout->textOption());

    text_option.setWrapMode(wordWrap);

    if (flags & Qt::AlignRight)
        text_option.setAlignment(Qt::AlignRight);
    else if (flags & Qt::AlignHCenter)
        text_option.setAlignment(Qt::AlignHCenter);

    if (painter) {
        text_option.setTextDirection(painter->layoutDirection());
        layout->setFont(painter->font());
    } else {
        // dont paint
        layout->engine()->ignoreBidi = true;
    }

    auto naturalTextRect = [&](const QRectF rect) {
        QRectF new_rect = rect;

        new_rect.setHeight(lineHeight);

        return new_rect;
    };

    auto drawShadowFun = [&](const QTextLine &line) {
        const QPen pen = painter->pen();

        painter->setPen(shadowColor);
        line.draw(painter, shadowOffset);

        // restore
        painter->setPen(pen);
    };

    layout->beginLayout();

    QTextLine line = layout->createLine();
    QRectF lastLineRect;

    int iColumn = 0;

    while (line.isValid()) {
        iColumn++;

        //        height += lineHeight;

        if (iColumn == 2) { // height + lineHeight > size.height()) {
            const QString &end_str = layout->engine()->elidedText(mode, qRound(size.width()), flags, line.textStart());

            layout->endLayout();
            layout->setText(end_str);

            if (layout->engine()->block.docHandle()) {
                const_cast<QTextDocument *>(layout->engine()->block.document())->setPlainText(end_str);
            }

            text_option.setWrapMode(QTextOption::NoWrap);
            layout->beginLayout();
            line = layout->createLine();
            line.setLineWidth(size.width() - 1);
            text = end_str;
        } else {
            line.setLineWidth(size.width());
        }

        line.setPosition(offset);

        const QRectF rect = naturalTextRect(line.naturalTextRect());

        if (painter) {
            if (drawBackground) {
                const QMarginsF margins(backgroundRadius, 0, backgroundRadius, 0);
                QRectF backBounding = rect;
                QPainterPath path;

                if (lastLineRect.isValid()) {
                    if (qAbs(rect.width() - lastLineRect.width()) < backgroundRadius * 2) {
                        backBounding.setWidth(lastLineRect.width());
                        backBounding.moveCenter(rect.center());
                        path.moveTo(lastLineRect.x() - backgroundRadius, lastLineRect.bottom() - backgroundRadius);
                        path.lineTo(lastLineRect.x(), lastLineRect.bottom() - 1);
                        path.lineTo(lastLineRect.right(), lastLineRect.bottom() - 1);
                        path.lineTo(lastLineRect.right() + backgroundRadius, lastLineRect.bottom() - backgroundRadius);
                        path.lineTo(lastLineRect.right() + backgroundRadius, backBounding.bottom() - backgroundRadius);
                        path.arcTo(backBounding.right() - backgroundRadius, backBounding.bottom() - backgroundRadius * 2, backgroundRadius * 2, backgroundRadius * 2, 0, -90);
                        path.lineTo(backBounding.x(), backBounding.bottom());
                        path.arcTo(backBounding.x() - backgroundRadius, backBounding.bottom() - backgroundRadius * 2, backgroundRadius * 2, backgroundRadius * 2, 270, -90);
                        lastLineRect = backBounding;
                    } else if (lastLineRect.width() > rect.width()) {
                        backBounding += margins;
                        path.moveTo(backBounding.x() - backgroundRadius, backBounding.y() - 1);
                        path.arcTo(backBounding.x() - backgroundRadius * 2, backBounding.y() - 1, backgroundRadius * 2, backgroundRadius * 2 + 1, 90, -90);
                        path.lineTo(backBounding.x(), backBounding.bottom() - backgroundRadius);
                        path.arcTo(backBounding.x(), backBounding.bottom() - backgroundRadius * 2, backgroundRadius * 2, backgroundRadius * 2, 180, 90);
                        path.lineTo(backBounding.right() - backgroundRadius, backBounding.bottom());
                        path.arcTo(backBounding.right() - backgroundRadius * 2, backBounding.bottom() - backgroundRadius * 2, backgroundRadius * 2, backgroundRadius * 2, 270, 90);
                        path.lineTo(backBounding.right(), backBounding.top() + backgroundRadius);
                        path.arcTo(backBounding.right(), backBounding.top() - 1, backgroundRadius * 2, backgroundRadius * 2 + 1, 180, -90);
                        path.closeSubpath();
                        lastLineRect = rect;
                    } else {
                        backBounding += margins;
                        path.moveTo(lastLineRect.x() - backgroundRadius * 2, lastLineRect.bottom());
                        path.arcTo(lastLineRect.x() - backgroundRadius * 3, lastLineRect.bottom() - backgroundRadius * 2, backgroundRadius * 2, backgroundRadius * 2, 270, 90);
                        path.lineTo(lastLineRect.x(), lastLineRect.bottom() - 1);
                        path.lineTo(lastLineRect.right(), lastLineRect.bottom() - 1);
                        path.lineTo(lastLineRect.right() + backgroundRadius, lastLineRect.bottom() - backgroundRadius * 2);
                        path.arcTo(lastLineRect.right() + backgroundRadius, lastLineRect.bottom() - backgroundRadius * 2, backgroundRadius * 2, backgroundRadius * 2, 180, 90);

                        //                        path.arcTo(lastLineRect.x() - backgroundReaius, lastLineRect.bottom() - backgroundReaius * 2, backgroundReaius * 2, backgroundReaius * 2, 180, 90);
                        //                        path.lineTo(lastLineRect.x() - backgroundReaius * 3, lastLineRect.bottom());
                        //                        path.moveTo(lastLineRect.right(), lastLineRect.bottom());
                        //                        path.arcTo(lastLineRect.right() - backgroundReaius, lastLineRect.bottom() - backgroundReaius * 2, backgroundReaius * 2, backgroundReaius * 2, 270, 90);
                        //                        path.arcTo(lastLineRect.right() + backgroundReaius, lastLineRect.bottom() - backgroundReaius * 2, backgroundReaius * 2, backgroundReaius * 2, 180, 90);
                        //                        path.lineTo(lastLineRect.right(), lastLineRect.bottom());

                        path.addRoundedRect(backBounding, backgroundRadius, backgroundRadius);
                        lastLineRect = rect;
                    }
                } else {
                    lastLineRect = backBounding;
                    path.addRoundedRect(backBounding + margins, backgroundRadius, backgroundRadius);
                }

                bool a = painter->testRenderHint(QPainter::Antialiasing);
                qreal o = painter->opacity();

                painter->setRenderHint(QPainter::Antialiasing, true);
                painter->setOpacity(1);
                painter->fillPath(path, background);
                painter->setRenderHint(QPainter::Antialiasing, a);
                painter->setOpacity(o);
            }

            if (drawShadow) {
                drawShadowFun(line);
            }

            line.draw(painter, QPointF(0, 0));
        }

        if (boundingRegion) {
            boundingRegion->append(rect);
        }

        offset.setY(offset.y() + lineHeight);

        //        // find '\n'
        //        int text_length_line = line.textLength();
        //        for (int start = line.textStart(); start < line.textStart() + text_length_line; ++start) {
        //            if (text.at(start) == '\n')
        //                height += lineHeight;
        //        }

        if (lines) {
            lines->append(text.mid(line.textStart(), line.textLength()));
        }

        if (iColumn == 2)
            break;
        //        if (height + lineHeight > size.height())
        //            break;

        line = layout->createLine();
    }

    layout->endLayout();
}

QString FileIconItemDelegate::elideText(const QString &text, const QSizeF &size,
                                        QTextOption::WrapMode wordWrap, const QFont &font,
                                        Qt::TextElideMode mode, qreal lineHeight, qreal flags)
{
    QTextLayout textLayout(text);

    textLayout.setFont(font);

    QStringList lines;

    elideText(&textLayout, size, wordWrap, mode, lineHeight, int(flags), &lines);

    return lines.join('\n');
}

void FileIconItemDelegate::wordWrapText(QTextLayout *layout, qreal width, QTextOption::WrapMode wrapMode,
                                        qreal lineHeight, QStringList *lines)
{
    elideText(layout, QSize(int(width), INT_MAX), wrapMode, Qt::ElideNone, lineHeight, 0, lines);
}

QString FileIconItemDelegate::wordWrapText(const QString &text, qreal width, QTextOption::WrapMode wrapMode,
                                           const QFont &font, qreal lineHeight, qreal *height)
{
    QTextLayout layout(text);

    layout.setFont(font);

    QStringList lines;

    wordWrapText(&layout, width, wrapMode, lineHeight, &lines);

    if (height)
        *height = lines.count() * lineHeight;

    return lines.join('\n');
}

static inline Qt::Alignment visualAlignment(Qt::LayoutDirection direction, Qt::Alignment alignment)
{
    if (!(alignment & Qt::AlignHorizontal_Mask))
        alignment |= Qt::AlignLeft;
    if (!(alignment & Qt::AlignAbsolute) && (alignment & (Qt::AlignLeft | Qt::AlignRight))) {
        if (direction == Qt::RightToLeft)
            alignment ^= (Qt::AlignLeft | Qt::AlignRight);
        alignment |= Qt::AlignAbsolute;
    }
    return alignment;
}

namespace DEEPIN_QT_THEME {
static QThreadStorage<QString> colorScheme;
//void (*setFollowColorScheme)(bool) = nullptr;
static bool (*followColorScheme)() = nullptr;
} // namespace DEEPIN_QT_THEME

void FileIconItemDelegate::paintIcon(QPainter *painter, const QIcon &icon, const QRectF &rect, Qt::Alignment alignment, QIcon::Mode mode, QIcon::State state)
{
    if (DEEPIN_QT_THEME::followColorScheme
        && (*DEEPIN_QT_THEME::followColorScheme)()
        && painter->device()->devType() == QInternal::Widget) {
        const DWidget *widget = dynamic_cast<DWidget *>(painter->device());
        const QPalette &pal = widget->palette();
        DEEPIN_QT_THEME::colorScheme.setLocalData(mode == QIcon::Selected ? pal.highlightedText().color().name() : pal.windowText().color().name());
    }

    // Copy of QStyle::alignedRect
    alignment = visualAlignment(painter->layoutDirection(), alignment);
    const qreal pixel_ratio = painter->device()->devicePixelRatioF();
    const QPixmap &px = getIconPixmap(icon, rect.size().toSize(), pixel_ratio, mode, state);
    qreal x = rect.x();
    qreal y = rect.y();
    qreal w = px.width() / px.devicePixelRatio();
    qreal h = px.height() / px.devicePixelRatio();
    if ((alignment & Qt::AlignVCenter) == Qt::AlignVCenter)
        y += (rect.size().height() - h) / 2.0;
    else if ((alignment & Qt::AlignBottom) == Qt::AlignBottom)
        y += rect.size().height() - h;
    if ((alignment & Qt::AlignRight) == Qt::AlignRight)
        x += rect.size().width() - w;
    else if ((alignment & Qt::AlignHCenter) == Qt::AlignHCenter)
        x += (rect.size().width() - w) / 2.0;

    painter->drawPixmap(qRound(x), qRound(y), px);
}

QT_BEGIN_NAMESPACE
Q_WIDGETS_EXPORT void qt_blurImage(QImage &blurImage, qreal radius, bool quality, int transposed = 0);
QT_END_NAMESPACE

void FileIconItemDelegate::drawCheck(QPainter *painter, const QRect &rect) const
{
    QRect rc = rect;
    rc.setSize({20, 20});
    rc.moveTopRight(QPoint(rect.right() - 5, rect.top() + 5));

    int x = rc.x() + 1;
    int y = rc.y() + 1;
    int w = rc.width() - 2;
    int h = rc.height() - 2;

    QPainterPath rectPath;

    rectPath.addEllipse(x, y, w, h);

    painter->fillPath(rectPath, QColor(255, 255, 255));

    DStyleOptionButton check;
    check.state = DStyle::State_On;
    check.rect = rc;

    DApplication::style()->drawPrimitive(DStyle::PE_IndicatorItemViewItemCheck, &check, painter);
}

void FileIconItemDelegate::paint(QPainter *painter,
                                 const QStyleOptionViewItem &option,
                                 const QModelIndex &index) const
{
    Q_D(const FileIconItemDelegate);

    bool isCanvas = false; //parent()->property("isCanvasViewHelper").toBool();
    /// judgment way of the whether drag model(another way is: painter.devType() != 1)
    //bool isDragMode = ((QPaintDevice*)parent()->parent()->viewport() != painter->device());
    bool isDragMode = false;
    bool isEnabled = option.state & QStyle::State_Enabled;
    bool hasFocus = option.state & QStyle::State_HasFocus;

    if (index == d->expandedIndex && !parent()->selectionModel()->isSelected(index))
        const_cast<FileIconItemDelegate *>(this)->hideNotEditingIndexWidget();

    painter->setOpacity(isTransparent(index) ? 0.3 : 1.0);

    QStyleOptionViewItem opt = option;
    initStyleOption(&opt, index);
    painter->setFont(opt.font);

    static QFont old_font = opt.font;

    if (old_font != opt.font) {
        if (d->expandedItem)
            d->expandedItem->setFont(opt.font);

        DWidget *editing_widget = editingIndexWidget();

        if (editing_widget)
            editing_widget->setFont(opt.font);

        const_cast<FileIconItemDelegate *>(this)->updateItemSizeHint();
        //        parent()->parent()->updateEditorGeometries();
    }

    old_font = opt.font;

    bool isSelected = !isDragMode && (opt.state & QStyle::State_Selected) && opt.showDecorationSelected;
    //bool isDropTarget = parent()->isDropTarget(index);
    bool isDropTarget = false;

    DPalette pl(DApplicationHelper::instance()->palette(option.widget));
    QColor c = pl.color(DPalette::ColorGroup::Active, DPalette::ColorType::ItemBackground);
    QColor base_color = c;
    if (option.widget) {
        DPalette pa = DApplicationHelper::instance()->palette(option.widget);
        base_color = option.widget->palette().base().color();
        DGuiApplicationHelper::ColorType ct = DGuiApplicationHelper::toColorType(base_color);
        if (ct == DGuiApplicationHelper::DarkType) {
            base_color = DGuiApplicationHelper::adjustColor(base_color, 0, 0, +5, 0, 0, 0, 0);
        }
    }

    if ((isDropTarget && !isSelected) || option.state & QStyle::StateFlag::State_Selected) {
        if (isCanvas) {
            c = pl.color(DPalette::ColorGroup::Active, QPalette::ColorRole::Highlight);
        } else {
            c.setAlpha(c.alpha() + 30);
        }
    } else if (option.state & QStyle::StateFlag::State_MouseOver) {
        c = c.lighter();
    } else if (!isCanvas) {
        c = base_color;
    }

    QRectF rect = opt.rect;
    int backgroundMargin = isCanvas ? 0 : COLUMU_PADDING;
    if (!isCanvas)
        rect.adjust(backgroundMargin, backgroundMargin, -backgroundMargin, -backgroundMargin); // 为了让对勾右上角， 缩小框框

    QPainterPath path;
    rect.moveTopLeft(QPointF(0.5, 0.5) + rect.topLeft());
    path.addRoundedRect(rect, ICON_MODE_BACK_RADIUS, ICON_MODE_BACK_RADIUS);

    if (!isCanvas && !isDragMode) { // 桌面和拖拽的图标不画背景
        painter->setRenderHint(QPainter::Antialiasing, true);
        painter->fillPath(path, c);
        painter->setRenderHint(QPainter::Antialiasing, false);
    }

    //    if (isDropTarget && !isSelected) {
    //        painter->setPen(c);
    //        painter->setRenderHint(QPainter::Antialiasing, true);
    //        painter->drawPath(path);
    //        painter->setRenderHint(QPainter::Antialiasing, false);
    //    }

    /// init icon geomerty
    QRectF icon_rect = opt.rect;

    icon_rect.setSize(parent()->iconSize());
    int iconTopOffset = isCanvas ? 0 : int((opt.rect.height() - icon_rect.height()) / 3.0);
    icon_rect.moveLeft(opt.rect.left() + (opt.rect.width() - icon_rect.width()) / 2.0);
    icon_rect.moveTop(opt.rect.top() + iconTopOffset); // move icon down

    /// draw icon
    if (opt.icon.isNull() == false) {
        if (isSelected) {
            paintIcon(painter, opt.icon, icon_rect, Qt::AlignCenter, isEnabled ? QIcon::Normal : QIcon::Disabled);
        } else if (isDropTarget) {
            //        QPixmap pixmap = opt.icon.pixmap(icon_rect.size().toSize());
            //        QPainter p(&pixmap);

            //        p.setCompositionMode(QPainter::CompositionMode_SourceAtop);
            //        p.fillRect(QRect(QPoint(0, 0), icon_rect.size().toSize()), QColor(0, 0, 0, int(255 * 0.1)));
            //        p.end();

            //        painter->drawPixmap(icon_rect.toRect(), pixmap);
        } else {
            paintIcon(painter, opt.icon, icon_rect, Qt::AlignCenter, isEnabled ? QIcon::Normal : QIcon::Disabled);
        }
    }

    /// draw file additional icon

    //    const QSizeF &cornerBaseSize = icon_rect.size() / 3;
    //    QList<QRectF> cornerGeometryList = getCornerGeometryList(icon_rect, QSizeF(qMin(24.0, cornerBaseSize.width()), qMin(24.0, cornerBaseSize.height())));
    //    const QList<QIcon> &cornerIconList = parent()->additionalIcon(index);

    //    for (int i = 0; i < cornerIconList.count(); ++i) {
    //        cornerIconList.at(i).paint(painter, cornerGeometryList.at(i).toRect());
    //    }

    if (!isCanvas && isSelected) {
        drawCheck(painter, option.rect);
    }

    if ((index == d->expandedIndex || index == d->editingIndex) && !isDragMode) {
        return;
    }

    QString str = opt.text;

    /// init file name geometry
    QRectF label_rect = opt.rect;

    label_rect.setTop(icon_rect.bottom() + TEXT_PADDING + ICON_MODE_ICON_SPACING);
    label_rect.setWidth(opt.rect.width() - 2 * TEXT_PADDING - 2 * backgroundMargin - ICON_MODE_BACK_RADIUS);
    label_rect.moveLeft(label_rect.left() + TEXT_PADDING + backgroundMargin + ICON_MODE_BACK_RADIUS / 2);

    if (isSelected && isCanvas) {
        painter->setPen(opt.palette.color(QPalette::BrightText));
    } else {
        painter->setPen(opt.palette.color(QPalette::Text));
    }

    /// if has selected show all file name else show elide file name.
    bool singleSelected = selectedIndexsCount() < 2;

    if (isSelected && singleSelected) {
        const_cast<FileIconItemDelegate *>(this)->hideNotEditingIndexWidget();

        int height = 0;

        /// init file name text
        const QList<QRectF> &lines = drawText(index, nullptr, str, label_rect.adjusted(0, 0, 0, 99999), 0, QBrush(Qt::NoBrush));
        height = int(boundingRect(lines).height());

        // we don't expend item in dde-fm but expand item on desktop
        bool shouldExpend = isCanvas;

        if (shouldExpend && height > label_rect.height()) {
            /// use widget(FileIconItem) show file icon and file name label.
            d->expandedIndex = index;

            setEditorData(d->expandedItem, index);
            parent()->setIndexWidget(index, d->expandedItem);

            // 重设item状态
            d->expandedItem->index = index;
            d->expandedItem->option = opt;
            d->expandedItem->textBounding = QRectF();
            d->expandedItem->setFixedWidth(0);
            d->expandedItem->setContentsMargins(backgroundMargin, iconTopOffset, backgroundMargin, 0);

            //            if (parent()->indexOfRow(index) == parent()->rowCount() - 1) {
            //                d->lastAndExpandedInde = index;
            //            }
            if (index.row() == parent()->model()->rowCount() - 1) {
                d->lastAndExpandedInde = index;
            }

            //parent()->updateGeometries();

            return;
        }
    } else {
        if (!singleSelected) {
            const_cast<FileIconItemDelegate *>(this)->hideNotEditingIndexWidget();
        }
    }

    if (isSelected || !d->enabledTextShadow || isDragMode) { // do not draw text background color
        const QList<QRectF> &lines = drawText(index, painter, str, label_rect, ICON_MODE_RECT_RADIUS,
                                              isSelected && isCanvas ? opt.palette.brush(QPalette::Normal, QPalette::Highlight) : QBrush(Qt::NoBrush),
                                              // QTextOption::WrapAtWordBoundaryOrAnywhere, opt.textElideMode, Qt::AlignCenter);
                                              QTextOption::WrapAtWordBoundaryOrAnywhere, Qt::ElideMiddle, Qt::AlignCenter);

        const QColor &border_color = focusTextBackgroundBorderColor();

        if (hasFocus && !singleSelected && border_color.isValid()) {
            QPainterPath line_path = boundingPath(lines, ICON_MODE_RECT_RADIUS, 1);

            painter->setPen(QPen(border_color, 2, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
            painter->drawPath(line_path);
        }
    } else {
        qreal pixel_ratio = painter->device()->devicePixelRatioF();
        QImage text_image((label_rect.size() * pixel_ratio).toSize(), QImage::Format_ARGB32_Premultiplied);
        text_image.fill(Qt::transparent);
        text_image.setDevicePixelRatio(pixel_ratio);

        QPainter p(&text_image);
        p.setPen(painter->pen());
        p.setFont(painter->font());
        drawText(index, &p, str, QRectF(QPoint(0, 0), QSizeF(text_image.size()) / pixel_ratio),
                 ICON_MODE_RECT_RADIUS, QBrush(Qt::NoBrush),
                 QTextOption::WrapAtWordBoundaryOrAnywhere, opt.textElideMode, Qt::AlignCenter);
        p.end();

        QPixmap text_pixmap = QPixmap::fromImage(text_image);
        text_pixmap.setDevicePixelRatio(pixel_ratio);
        qt_blurImage(text_image, 6, false);

        p.begin(&text_image);
        p.setCompositionMode(QPainter::CompositionMode_SourceIn);
        p.fillRect(text_image.rect(), opt.palette.color(QPalette::Shadow));
        p.end();

        painter->drawImage(label_rect.translated(0, 1), text_image);
        painter->drawPixmap(label_rect.topLeft(), text_pixmap);
    }

    painter->setOpacity(1);
}

QSize FileIconItemDelegate::sizeHint(const QStyleOptionViewItem &, const QModelIndex &index) const
{
    Q_D(const FileIconItemDelegate);

    const QSize &size = d->itemSizeHint;

    if (index.isValid() && index == d->lastAndExpandedInde) {
        d->expandedItem->iconHeight = parent()->iconSize().height();

        return QSize(size.width(), d->expandedItem->heightForWidth(size.width()));
    }

    return size;
}

DWidget *FileIconItemDelegate::createEditor(DWidget *parent, const QStyleOptionViewItem &, const QModelIndex &index) const
{
    Q_D(const FileIconItemDelegate);

    d->editingIndex = index;

    FileIconItem *item = new FileIconItem(parent);

    connect(item, &FileIconItem::inputFocusOut, this, &FileIconItemDelegate::onEditWidgetFocusOut);
    connect(item, &FileIconItem::destroyed, this, [this, d] {
        Q_UNUSED(this)
        d->editingIndex = QModelIndex();
    });
    connect(item, &FileIconItem::sigItemRefresh, this, &FileIconItemDelegate::sigItemRefresh);

    d->fileIconItem = item;
    item->newEdit = true;

    return item;
}

void FileIconItemDelegate::updateEditorGeometry(DWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    Q_D(const FileIconItemDelegate);

    const QSize &icon_size = parent()->iconSize();

    editor->move(option.rect.topLeft());
    editor->setMinimumHeight(option.rect.height());

    QStyleOptionViewItem opt = option;
    initStyleOption(&opt, index);

    if (editor == d->expandedItem) {
        if (editor->width() != option.rect.width()) {
            editor->setFixedWidth(option.rect.width());
            d->expandedItem->iconHeight = icon_size.height();
            editor->adjustSize();
        }

        return;
    }

    editor->setFixedWidth(option.rect.width());

    FileIconItem *item = qobject_cast<FileIconItem *>(editor);

    if (!item)
        return;

    QLabel *icon = item->getIconLabel();

    if (icon_size.height() != icon->size().height()) {
        bool isCanvas = parent()->property("isCanvasViewHelper").toBool();
        int topoffset = isCanvas ? 0 : (opt.rect.height() - icon_size.height()) / 3; //update edit pos
        icon->setFixedHeight(icon_size.height() + topoffset);
    }
}

void FileIconItemDelegate::setEditorData(DWidget *editor, const QModelIndex &index) const
{
    Q_D(const FileIconItemDelegate);

    QStyleOptionViewItem opt;

    initStyleOption(&opt, index);

    const QSize &icon_size = parent()->iconSize();

    if (ExpandedItem *item = qobject_cast<ExpandedItem *>(editor)) {
        item->iconHeight = icon_size.height();
        item->setOpacity(isTransparent(index) ? 0.3 : 1);

        return;
    }

    FileIconItem *item = qobject_cast<FileIconItem *>(editor);

    if (!item)
        return;

    if (!item->newEdit)
        return;
    item->newEdit = !item->newEdit; //zll 修改文件名编辑名单击其它文件时的异常问题

    //bool donot_show_suffix{ DFMApplication::instance()->genericAttribute(DFMApplication::GA_ShowedFileSuffixOnRename).toBool() };
    bool donot_show_suffix = false;

    if (item->edit->isReadOnly()) {
        item->edit->setPlainText(index.data().toString());
    } else {
        if (donot_show_suffix) {
            const QString &suffix = index.data().toString();

            editor->setProperty("_d_whether_show_suffix", suffix);

            item->setMaxCharSize(MAX_FILE_NAME_CHAR_COUNT - suffix.toLocal8Bit().size() - (suffix.isEmpty() ? 0 : 1));
            item->edit->setPlainText(index.data().toString());
        } else {
            item->setMaxCharSize(MAX_FILE_NAME_CHAR_COUNT);
            item->edit->setPlainText(index.data().toString());
        }
    }

    item->edit->setAlignment(Qt::AlignHCenter);
    item->edit->document()->setTextWidth(d->itemSizeHint.width());
    item->setOpacity(isTransparent(index) ? 0.3 : 1);

    if (item->edit->isReadOnly())
        return;

    const QString &selectionWhenEditing = baseName(index);
    int endPos = selectionWhenEditing.isEmpty() ? -1 : selectionWhenEditing.length();

    if (endPos == -1 || donot_show_suffix) {
        item->edit->selectAll();
    } else {
        QTextCursor cursor = item->edit->textCursor();

        cursor.setPosition(0);
        cursor.setPosition(endPos, QTextCursor::KeepAnchor);

        item->edit->setTextCursor(cursor);
    }
}

void FileIconItemDelegate::setModelData(DWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
    FileIconItem *item = qobject_cast<FileIconItem *>(editor);
    QString value = item->edit->toPlainText();
    model->setData(index, value);
}

QList<QRect> FileIconItemDelegate::paintGeomertys(const QStyleOptionViewItem &option, const QModelIndex &index, bool sizeHintMode) const
{
    Q_UNUSED(sizeHintMode)
    Q_D(const FileIconItemDelegate);

    QList<QRect> geometries;

    if (index == d->expandedIndex) {
        QRect geometry = d->expandedItem->iconGeometry().toRect();

        geometry.moveTopLeft(geometry.topLeft() + d->expandedItem->pos());

        geometries << geometry;

        geometry = d->expandedItem->textGeometry().toRect();
        geometry.moveTopLeft(geometry.topLeft() + d->expandedItem->pos());
        geometry.setTop(geometries.first().bottom());

        geometries << geometry;

        return geometries;
    }

    /// init icon geomerty

    QRect icon_rect = option.rect;

    icon_rect.setSize(parent()->iconSize());
    icon_rect.moveCenter(option.rect.center());
    icon_rect.moveTop(option.rect.top());

    geometries << icon_rect;

    QString str = index.data(Qt::DisplayRole).toString();

    if (str.isEmpty()) {
        return geometries;
    }

    /// init file name geometry

    QRect label_rect = option.rect;
    bool isCanvas = parent()->property("isCanvasViewHelper").toBool();
    int backgroundMargin = isCanvas ? 0 : COLUMU_PADDING;

    label_rect.setWidth(label_rect.width() - 2 * TEXT_PADDING - 2 * backgroundMargin - ICON_MODE_BACK_RADIUS);
    label_rect.moveLeft(label_rect.left() + TEXT_PADDING + backgroundMargin + ICON_MODE_BACK_RADIUS / 2);
    label_rect.setTop(icon_rect.bottom() + TEXT_PADDING + ICON_MODE_ICON_SPACING);

    QStyleOptionViewItem opt = option;
    //    initStyleOption(&opt, index);

    bool isSelected = !parent()->selectionModel()->isSelected(index) && opt.showDecorationSelected;
    /// if has selected show all file name else show elide file name.
    bool singleSelected = selectedIndexsCount() < 2;

    QTextLayout text_layout;

    text_layout.setFont(option.font);
    text_layout.setText(str);

    bool elide = (!isSelected || !singleSelected);

    auto lines = drawText(index, nullptr, str, QRect(label_rect.topLeft(), QSize(label_rect.width(), INT_MAX)),
                          ICON_MODE_RECT_RADIUS, isSelected ? opt.backgroundBrush : QBrush(Qt::NoBrush),
                          QTextOption::WrapAtWordBoundaryOrAnywhere, elide ? opt.textElideMode : Qt::ElideNone,
                          Qt::AlignCenter);

    label_rect = boundingRect(lines).toRect();
    label_rect.setTop(icon_rect.bottom());

    geometries << label_rect;

    // background rect
    QRect background_rect = option.rect;
    if (!isCanvas) {
        // 为了让对勾右上角， 缩小框框
        background_rect.adjust(backgroundMargin, backgroundMargin, -backgroundMargin, -backgroundMargin);
        geometries << background_rect;
    }

    return geometries;
}

QModelIndexList FileIconItemDelegate::hasWidgetIndexs() const
{
    QModelIndexList hasWidgetIndexs;
    Q_D(const FileIconItemDelegate);

    if (!d->editingIndex.isValid())
        hasWidgetIndexs = QModelIndexList();
    else
        hasWidgetIndexs << d->editingIndex;

    const QModelIndex &index = expandedIndex();

    if (!index.isValid())
        return hasWidgetIndexs;

    return hasWidgetIndexs << index;
}

void FileIconItemDelegate::hideNotEditingIndexWidget()
{
    Q_D(FileIconItemDelegate);

    if (d->expandedIndex.isValid()) {
        parent()->setIndexWidget(d->expandedIndex, nullptr);
        d->expandedItem->hide();
        d->expandedIndex = QModelIndex();
        d->lastAndExpandedInde = QModelIndex();
    }
}

void FileIconItemDelegate::fontChanged(const QFont &font)
{
    Q_D(FileIconItemDelegate);

    qreal fontSize = font.pointSizeF();
    if (fontSize == 8.25) {
        d->itemSizeHint = QSize(148, 148);
    } else if (fontSize == 9) {
        d->itemSizeHint = QSize(151, 151);
    } else if (fontSize == 9.75) {
        d->itemSizeHint = QSize(160, 160);
    } else if (fontSize == 10.5) {
        d->itemSizeHint = QSize(163, 163);
    } else if (fontSize == 11.25) {
        d->itemSizeHint = QSize(166, 166);
    } else if (fontSize == 12) {
        d->itemSizeHint = QSize(172, 172);
    } else if (fontSize == 13.5) {
        d->itemSizeHint = QSize(178, 178);
    } else if (fontSize == 15) {
        d->itemSizeHint = QSize(190, 190);
    }
}

QModelIndex FileIconItemDelegate::expandedIndex() const
{
    Q_D(const FileIconItemDelegate);

    return d->expandedIndex;
}

DWidget *FileIconItemDelegate::expandedIndexWidget() const
{
    Q_D(const FileIconItemDelegate);

    return d->expandedItem;
}

int FileIconItemDelegate::iconSizeLevel() const
{
    Q_D(const FileIconItemDelegate);

    return d->currentIconSizeIndex;
}

int FileIconItemDelegate::minimumIconSizeLevel() const
{
    return 0;
}

int FileIconItemDelegate::maximumIconSizeLevel() const
{
    Q_D(const FileIconItemDelegate);

    return d->iconSizes.count() - 1;
}

/*!
 * \brief Return current icon level if icon can increase; otherwise return -1.
 * \return
 */
int FileIconItemDelegate::increaseIcon()
{
    Q_D(const FileIconItemDelegate);

    return setIconSizeByIconSizeLevel(d->currentIconSizeIndex + 1);
}

/*!
 * \brief Return current icon level if icon can decrease; otherwise return -1.
 * \return
 */
int FileIconItemDelegate::decreaseIcon()
{
    Q_D(const FileIconItemDelegate);

    return setIconSizeByIconSizeLevel(d->currentIconSizeIndex - 1);
}

/*!
 * \brief Return current icon level if level is vaild; otherwise return -1.
 * \param level
 * \return
 */
int FileIconItemDelegate::setIconSizeByIconSizeLevel(int level)
{
    Q_D(FileIconItemDelegate);

    if (level == d->currentIconSizeIndex)
        return level;

    if (level >= minimumIconSizeLevel() && level <= maximumIconSizeLevel()) {
        d->currentIconSizeIndex = level;

        parent()->setIconSize(iconSizeByIconSizeLevel());

        return d->currentIconSizeIndex;
    }

    return -1;
}

void FileIconItemDelegate::updateItemSizeHint()
{
    //    Q_D(FileIconItemDelegate);

    //    d->elideMap.clear();
    //    d->wordWrapMap.clear();
    //    d->textHeightMap.clear();
    this->fontChanged(qApp->font());
    //    d->textLineHeight = parent()->fontMetrics().height();

    //    int width = parent()->iconSize().width() + 30;
    //    int height = parent()->iconSize().height() + 2 * COLUMU_PADDING
    //                 + 2 * TEXT_PADDING + ICON_MODE_ICON_SPACING + 3 * d->textLineHeight;
    //    int size = qMax(width, height) + 10; //mod by zll
    //    d->itemSizeHint = QSize(size, size);
    //d->itemSizeHint = QSize(width, parent()->parent()->iconSize().height() + 2 * TEXT_PADDING  + ICON_MODE_ICON_SPACING + 3 * d->textLineHeight);
}

QColor FileIconItemDelegate::focusTextBackgroundBorderColor() const
{
    Q_D(const FileIconItemDelegate);

    return d->focusTextBackgroundBorderColor;
}

bool FileIconItemDelegate::enabledTextShadow() const
{
    Q_D(const FileIconItemDelegate);

    return d->enabledTextShadow;
}

void FileIconItemDelegate::setFocusTextBackgroundBorderColor(QColor focusTextBackgroundBorderColor)
{
    Q_D(FileIconItemDelegate);

    d->focusTextBackgroundBorderColor = focusTextBackgroundBorderColor;

    //    if (d->expandedItem)
    //        d->expandedItem->setBorderColor(focusTextBackgroundBorderColor);
}

void FileIconItemDelegate::setEnabledTextShadow(bool enabledTextShadow)
{
    Q_D(FileIconItemDelegate);

    d->enabledTextShadow = enabledTextShadow;
}

void FileIconItemDelegate::initTextLayout(const QModelIndex &index, QTextLayout *layout) const
{
    Q_D(const FileIconItemDelegate);

    const QVariantHash &ep = index.data().toHash();
    const QList<QColor> &colors = qvariant_cast<QList<QColor>>(ep.value("colored"));

    if (!colors.isEmpty()) {
        if (!layout->engine()->block.docHandle()) {
            if (!d->document)
                const_cast<FileIconItemDelegatePrivate *>(d)->document = new QTextDocument(const_cast<FileIconItemDelegate *>(this));

            d->document->setPlainText(layout->text());
            layout->engine()->block = d->document->firstBlock();
        }

        layout->engine()->docLayout()->registerHandler(d->textObjectType, d->textObjectInterface);
        QTextCursor cursor(layout->engine()->docLayout()->document());
        TagTextFormat format(d->textObjectType, colors, (d->drawTextBackgroundOnLast || colors.size() > 1) ? Qt::white : QColor(0, 0, 0, 25));

        cursor.setPosition(0);
        cursor.insertText(QString(QChar::ObjectReplacementCharacter), format);
    }
}

bool FileIconItemDelegate::eventFilter(QObject *object, QEvent *event)
{
    if (event->type() == QEvent::KeyPress) {
        QKeyEvent *e = static_cast<QKeyEvent *>(event);

        if (e->key() == Qt::Key_Enter || e->key() == Qt::Key_Return) {
            e->accept();

            return true;
        }
    } /*else if (event->type() == QEvent::FocusOut) {
        onEditWidgetFocusOut();

        return true;
    }*/

    return QStyledItemDelegate::eventFilter(object, event);
}

void FileIconItemDelegate::onEditWidgetFocusOut()
{
    Q_D(const FileIconItemDelegate);

    QString fileName = d->fileIconItem->edit->toPlainText().trimmed();
    fileName = fileName.remove('\n');
    fileName = fileName.remove('\r');

    parent()->model()->setData(d->editingIndex, fileName);

    emit sigFileNewFolder(fileName);
    qDebug() << __LINE__ << __FUNCTION__;

    //    qobject_cast<FileManageWidget *>(parent()->parent())->slotFileItemEditFinish(fileName);
    d->fileIconItem->newEdit = false;

    if (qApp->focusWidget() && qApp->focusWidget()->window() == parent()->window()
        && qApp->focusWidget() != parent()->parent()) {
        hideAllIIndexWidget();
    }

    if (d->fileIconItem->itemRefresh) {
        d->fileIconItem->itemRefresh = false;
        emit sigItemRefresh();
    }
}

void FileIconItemDelegate::onTriggerEdit(const QModelIndex &index)
{
    Q_D(FileIconItemDelegate);

    if (index == d->expandedIndex) {
        parent()->setIndexWidget(index, nullptr);
        d->expandedItem->hide();
        d->expandedIndex = QModelIndex();
        d->lastAndExpandedInde = QModelIndex();
        parent()->edit(index);
    }
}

QSize FileIconItemDelegate::iconSizeByIconSizeLevel() const
{
    Q_D(const FileIconItemDelegate);

    int size = d->iconSizes.at(d->currentIconSizeIndex);

    return QSize(size, size);
}

bool FileIconItemDelegate::isTransparent(const QModelIndex &index) const
{
    Q_UNUSED(index)
    return false;
}

DWidget *FileIconItemDelegate::editingIndexWidget() const
{
    Q_D(const FileIconItemDelegate);

    return parent()->indexWidget(d->editingIndex);
}

int FileIconItemDelegate::selectedIndexsCount() const
{
    return parent()->selectionModel()->selectedIndexes().count();
}

QString FileIconItemDelegate::baseName(const QModelIndex &index) const
{
    QString name = parent()->model()->data(index).toString();
    return name;
}

void FileIconItemDelegate::hideAllIIndexWidget()
{
    Q_D(const FileIconItemDelegate);

    hideNotEditingIndexWidget();

    if (d->editingIndex.isValid()) {
        parent()->setIndexWidget(d->editingIndex, nullptr);

        d->editingIndex = QModelIndex();
    }
}

void FileIconItemDelegate::setItemSizeHint()
{
    //    Q_D(FileIconItemDelegate);

    this->fontChanged(qApp->font());
    //    d->textLineHeight = parent()->fontMetrics().height();

    //    int width = parent()->iconSize().width() + 30;
    //    int height = parent()->iconSize().height() + 2 * COLUMU_PADDING
    //                 + 2 * TEXT_PADDING + ICON_MODE_ICON_SPACING + 3 * 22;
    //    int size = qMax(width, height);

    //    //    qDebug() << "textLineHeight=================" << d->textLineHeight;
    //    d->itemSizeHint = QSize(size, size);
}
