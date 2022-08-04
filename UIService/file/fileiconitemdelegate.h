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
#ifndef FILEICONITEMDELEGATE_H
#define FILEICONITEMDELEGATE_H

#include "base/BaseListViewItemDelegate.h"
#include <QPointer>
#include <QTextLayout>
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
#include "dstyleoption.h"
#include <DStyle>
#include <DWidget>

DWIDGET_USE_NAMESPACE

class FileIconItemDelegatePrivate;
class FileIconItemDelegate : public BaseListViewItemDelegate
{
    Q_OBJECT

    Q_PROPERTY(QColor focusTextBackgroundBorderColor READ focusTextBackgroundBorderColor WRITE setFocusTextBackgroundBorderColor)
    Q_PROPERTY(bool enabledTextShadow READ enabledTextShadow WRITE setEnabledTextShadow)

public:
    FileIconItemDelegate(QAbstractItemView *parent);
    ~FileIconItemDelegate() override;

signals:
    void sigFileNewFolder(const QString &);
    void sigItemRefresh();

public:
    QAbstractItemView *parent() const;

    void paint(QPainter *painter,
               const QStyleOptionViewItem &option,
               const QModelIndex &index) const Q_DECL_OVERRIDE;
    QSize sizeHint(const QStyleOptionViewItem &, const QModelIndex &) const Q_DECL_OVERRIDE;
    DWidget *createEditor(DWidget *parent, const QStyleOptionViewItem &, const QModelIndex &index) const Q_DECL_OVERRIDE;

    void updateEditorGeometry(DWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &) const Q_DECL_OVERRIDE;
    void setEditorData(DWidget *editor, const QModelIndex &index) const Q_DECL_OVERRIDE;
    void setModelData(DWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const override;

    QList<QRect> paintGeomertys(const QStyleOptionViewItem &option, const QModelIndex &index, bool sizeHintMode = false) const;

    QModelIndexList hasWidgetIndexs() const;
    void hideNotEditingIndexWidget();
    void fontChanged(const QFont &);

    QModelIndex expandedIndex() const;
    DWidget *expandedIndexWidget() const;

    int iconSizeLevel() const;
    int minimumIconSizeLevel() const;
    int maximumIconSizeLevel() const;

    int increaseIcon();
    int decreaseIcon();
    int setIconSizeByIconSizeLevel(int level);

    void updateItemSizeHint();

    QColor focusTextBackgroundBorderColor() const;
    bool enabledTextShadow() const;

public slots:
    void setFocusTextBackgroundBorderColor(QColor focusTextBackgroundBorderColor);
    void setEnabledTextShadow(bool enabledTextShadow);

protected:
    void initTextLayout(const QModelIndex &index, QTextLayout *layout) const;

    bool eventFilter(QObject *object, QEvent *event) Q_DECL_OVERRIDE;

    QList<QRectF> drawText(const QModelIndex &index, QPainter *painter, QTextLayout *layout,
                           const QRectF &boundingRect, qreal radius, const QBrush &background,
                           QTextOption::WrapMode wordWrap = QTextOption::WrapAtWordBoundaryOrAnywhere,
                           Qt::TextElideMode mode = Qt::ElideMiddle, int flags = Qt::AlignCenter,
                           const QColor &shadowColor = QColor()) const;

    QList<QRectF> drawText(const QModelIndex &index, QPainter *painter, const QString &text,
                           const QRectF &boundingRect, qreal radius, const QBrush &background,
                           QTextOption::WrapMode wordWrap = QTextOption::WrapAtWordBoundaryOrAnywhere,
                           Qt::TextElideMode mode = Qt::ElideMiddle, int flags = Qt::AlignCenter,
                           const QColor &shadowColor = QColor()) const;

    static void paintCircleList(QPainter *painter, QRectF boundingRect, qreal diameter, const QList<QColor> &colors, const QColor &borderColor);
    static QPixmap getIconPixmap(const QIcon &icon, const QSize &size, qreal pixelRatio, QIcon::Mode mode = QIcon::Normal, QIcon::State state = QIcon::Off);

    QList<QRectF> getCornerGeometryList(const QRectF &baseRect, const QSizeF &cornerSize) const;

    static void paintIcon(QPainter *painter, const QIcon &icon, const QRectF &rect, Qt::Alignment alignment = Qt::AlignCenter,
                          QIcon::Mode mode = QIcon::Normal, QIcon::State state = QIcon::Off);

    static QString wordWrapText(const QString &text, qreal width,
                                QTextOption::WrapMode wrapMode,
                                const QFont &font,
                                qreal lineHeight,
                                qreal *height = nullptr);

    static QString elideText(const QString &text, const QSizeF &size,
                             QTextOption::WrapMode wordWrap,
                             const QFont &font,
                             Qt::TextElideMode mode,
                             qreal lineHeight,
                             qreal flags = 0);

    static void wordWrapText(QTextLayout *layout, qreal width,
                             QTextOption::WrapMode wrapMode,
                             qreal lineHeight,
                             QStringList *lines = nullptr);

    static void elideText(QTextLayout *layout, const QSizeF &size,
                          QTextOption::WrapMode wordWrap,
                          Qt::TextElideMode mode, qreal lineHeight,
                          int flags = 0, QStringList *lines = nullptr,
                          QPainter *painter = nullptr, QPointF offset = QPoint(0, 0),
                          const QColor &shadowColor = QColor(),
                          const QPointF &shadowOffset = QPointF(0, 1),
                          const QBrush &background = QBrush(Qt::NoBrush),
                          qreal backgroundReaius = 4,
                          QList<QRectF> *boundingRegion = nullptr);

private:
    /**
     * @brief 绘制 item 选中的图标效果
     * @param painter
     * @param rect
     */
    void drawCheck(QPainter *painter, const QRect &rect) const;

    void onEditWidgetFocusOut();
    void onTriggerEdit(const QModelIndex &index);
    QSize iconSizeByIconSizeLevel() const;
    //    QIcon m_checkedIcon;

    Q_DECLARE_PRIVATE(FileIconItemDelegate)

    friend class ExpandedItem;

    FileIconItemDelegatePrivate *d_ptr;

    bool isTransparent(const QModelIndex &index) const;
    DWidget *editingIndexWidget() const;
    int selectedIndexsCount() const;
    QString baseName(const QModelIndex &index) const;
    void hideAllIIndexWidget();
    void onItemEditAgagin();
    void setItemSizeHint();
};

class ExpandedItem : public DWidget
{
    Q_OBJECT

    Q_PROPERTY(qreal opacity READ opacity WRITE setOpacity)

public:
    explicit ExpandedItem(FileIconItemDelegate *d, DWidget *parent = nullptr);

    bool event(QEvent *ee) override;

    qreal opacity() const;

    void setOpacity(qreal opacity);
    void paintEvent(QPaintEvent *) override;

    QSize sizeHint() const override;

    int heightForWidth(int width) const override;

    void setIconPixmap(const QPixmap &pixmap, int height);

    QRectF iconGeometry() const;

    QRectF textGeometry(int width = -1) const;

    QPixmap iconPixmap;
    int iconHeight = 0;
    mutable QRectF textBounding;
    QModelIndex index;
    QStyleOptionViewItem option;
    qreal m_opactity = 1;
    bool canDeferredDelete = true;
    FileIconItemDelegate *delegate;
};

class FileTagObjectInterface : public QObject
    , public QTextObjectInterface
{
    Q_OBJECT
    Q_INTERFACES(QTextObjectInterface)

public:
    explicit FileTagObjectInterface();

    QSizeF intrinsicSize(QTextDocument *doc, int posInDocument,
                         const QTextFormat &format) override;

    void drawObject(QPainter *painter, const QRectF &rect, QTextDocument *doc,
                    int posInDocument, const QTextFormat &format) override;
};
#endif // FILEICONITEMDELEGATE_H
