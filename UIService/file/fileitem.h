/*
 * Copyright (C) 2016 ~ 2018 Uniontech Software Technology Co., Ltd.
 *               2016 ~ 2018 dragondjf
 *
 * Author:     dragondjf<dingjiangfeng@deepin.com>
 *
 * Maintainer: dragondjf<dingjiangfeng@deepin.com>
 *             zccrs<zhangjide@deepin.com>
 *             Tangtong<tangtong@deepin.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef FILEITEM_H
#define FILEITEM_H

#include <QLabel>
#include <QStack>
#include <DTextEdit>
#include <DWidget>

QT_BEGIN_NAMESPACE
class QTextEdit;
class QGraphicsOpacityEffect;
QT_END_NAMESPACE

DWIDGET_USE_NAMESPACE
class CanSetDragTextEdit : public DTextEdit
{
    Q_OBJECT

public:
    explicit CanSetDragTextEdit(DWidget *parent = nullptr);
    explicit CanSetDragTextEdit(const QString &text, DWidget *parent = nullptr);
    //set QTextEdit can drag
    void setDragEnabled(const bool &bdrag);

signals:
    void inputFocusOut();
};

class FileIconItemEdit;
class FileIconItem : public QFrame
{
    Q_OBJECT

    Q_PROPERTY(qreal opacity READ opacity WRITE setOpacity)

public:
    explicit FileIconItem(DWidget *parent = nullptr);

    qreal opacity() const;
    void setOpacity(qreal opacity);
    void setMaxCharSize(int maxSize);

    QSize sizeHint() const Q_DECL_OVERRIDE;

    inline QLabel *getIconLabel() const
    {
        return icon;
    }
    inline QTextEdit *getTextEdit() const
    {
        return edit;
    }

public slots:

signals:
    void inputFocusOut();
    void sigItemRefresh();

private slots:
    void popupEditContentMenu();
    void editUndo();
    void editRedo();

protected:
    void updateEditorGeometry();
    bool event(QEvent *ee) Q_DECL_OVERRIDE;
    bool eventFilter(QObject *obj, QEvent *ee) Q_DECL_OVERRIDE;

private:
    //    void updateStyleSheet();

    QString editTextStackCurrentItem() const;
    QString editTextStackBack();
    QString editTextStackAdvance();
    void pushItemToEditTextStack(const QString &item);

    bool canDeferredDelete = true;
    QLabel *icon;
    QTextEdit *edit;
    int editTextStackCurrentIndex = -1;
    bool disableEditTextStack = false;
    QStack<QString> editTextStack;
    QGraphicsOpacityEffect *opacityEffect = Q_NULLPTR;
    int maxCharSize = INT_MAX;

    bool newEdit = false;
    bool inputOut = false;
    bool itemRefresh = false;

    friend class FileIconItemDelegate;
};

#endif // FILEITEM_H
