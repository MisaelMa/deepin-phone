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

#include <QBoxLayout>
#include <DTextEdit>
#include <QTextBlock>
#include <QGraphicsOpacityEffect>
#include <QApplication>
#include <QMenu>
#include <QPainter>

#include <danchors.h>
#include <DTextEdit>
#include <DListView>

#include "fileitem.h"
#include <private/qtextedit_p.h>

DWIDGET_USE_NAMESPACE

#define TEXT_PADDING 4
#define ICON_MODE_ICON_SPACING 5

FileIconItem::FileIconItem(DWidget *parent)
    : QFrame(parent)
{
    icon = new QLabel(this);
    edit = new CanSetDragTextEdit(this);
    connect(qobject_cast<CanSetDragTextEdit *>(edit), &CanSetDragTextEdit::inputFocusOut, this, &FileIconItem::inputFocusOut);

    icon->setAlignment(Qt::AlignCenter);
    icon->setFrameShape(QFrame::NoFrame);
    icon->installEventFilter(this);

    edit->setWordWrapMode(QTextOption::WrapAtWordBoundaryOrAnywhere);
    edit->setAlignment(Qt::AlignHCenter);
    edit->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    edit->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    edit->setFrameShape(QFrame::NoFrame);
    edit->installEventFilter(this);
    edit->setAcceptRichText(false);
    edit->setContextMenuPolicy(Qt::CustomContextMenu);
    edit->setAcceptDrops(false);
    static_cast<CanSetDragTextEdit *>(edit)->setDragEnabled(false);

    auto vlayout = new QVBoxLayout;
    vlayout->setContentsMargins(0, 0, 0, 0);
    vlayout->setSpacing(0);
    setLayout(vlayout);

    vlayout->addWidget(icon, 0, Qt::AlignTop | Qt::AlignHCenter);
    vlayout->addSpacing(ICON_MODE_ICON_SPACING);
    vlayout->addWidget(edit, 0, Qt::AlignTop | Qt::AlignHCenter);

    setFrameShape(QFrame::NoFrame);
    setFocusProxy(edit);

    connect(edit, &QTextEdit::textChanged, this, [=] {
        QSignalBlocker blocker(edit);
        Q_UNUSED(blocker)

        QString text = edit->toPlainText();
        const QString old_text = text;

        int text_length = text.length();
        int text_line_height = fontMetrics().height();

        //text = DFMGlobal::preprocessingFileName(text);
        //text = qobject_cast<DListView *>(parent)->currentIndex().data().toString();

        QVector<uint> list = text.toUcs4();
        int cursor_pos = edit->textCursor().position() - text_length + text.length();

        while (text.toLocal8Bit().size() > maxCharSize) {
            list.removeAt(--cursor_pos);

            text = QString::fromUcs4(list.data(), list.size());
        }

        if (text.count() != old_text.count()) {
            edit->setPlainText(text);
        }

        if (editTextStackCurrentItem() != text) {
            pushItemToEditTextStack(text);
        }

        QTextCursor cursor = edit->textCursor();

        cursor.movePosition(QTextCursor::Start);

        do {
            QTextBlockFormat format = cursor.blockFormat();

            format.setLineHeight(text_line_height, QTextBlockFormat::FixedHeight);
            cursor.setBlockFormat(format);
        } while (cursor.movePosition(QTextCursor::NextBlock));

        cursor.setPosition(cursor_pos);

        edit->setTextCursor(cursor);
        edit->setAlignment(Qt::AlignHCenter);

        if (edit->isVisible()) {
            updateEditorGeometry();
        }
    });
    connect(edit, &QTextEdit::customContextMenuRequested, this, &FileIconItem::popupEditContentMenu);
}

qreal FileIconItem::opacity() const
{
    if (opacityEffect)
        return opacityEffect->opacity();

    return 1;
}

void FileIconItem::setOpacity(qreal opacity)
{
    if (opacity - 1.0 >= 0) {
        if (opacityEffect) {
            opacityEffect->deleteLater();
            opacityEffect = Q_NULLPTR;
        }

        return;
    } else if (!opacityEffect) {
        opacityEffect = new QGraphicsOpacityEffect(this);
        setGraphicsEffect(opacityEffect);
    }

    opacityEffect->setOpacity(opacity);
}

void FileIconItem::setMaxCharSize(int maxSize)
{
    maxCharSize = maxSize;
}

QSize FileIconItem::sizeHint() const
{
    return QSize(width(), icon->height() + edit->height());
}

void FileIconItem::popupEditContentMenu()
{
    QMenu *menu = edit->createStandardContextMenu();

    if (!menu || edit->isReadOnly()) {
        return;
    }

    QAction *undo_action = menu->findChild<QAction *>(QStringLiteral("edit-undo"));
    QAction *redo_action = menu->findChild<QAction *>(QStringLiteral("edit-redo"));

    if (undo_action) {
        undo_action->setEnabled(editTextStackCurrentIndex > 0);
        disconnect(undo_action, SIGNAL(triggered(bool)));
        connect(undo_action, &QAction::triggered, this, &FileIconItem::editUndo);
    }
    if (redo_action) {
        redo_action->setEnabled(editTextStackCurrentIndex < editTextStack.count() - 1);
        disconnect(redo_action, SIGNAL(triggered(bool)));
        connect(redo_action, &QAction::triggered, this, &FileIconItem::editRedo);
    }

    menu->exec(QCursor::pos());
    menu->deleteLater();
}

void FileIconItem::editUndo()
{
    disableEditTextStack = true;
    QTextCursor cursor = edit->textCursor();
    edit->setPlainText(editTextStackBack());
    edit->setTextCursor(cursor);
}

void FileIconItem::editRedo()
{
    disableEditTextStack = true;
    QTextCursor cursor = edit->textCursor();
    edit->setPlainText(editTextStackAdvance());
    edit->setTextCursor(cursor);
}

bool FileIconItem::event(QEvent *ee)
{
    if (ee->type() == QEvent::DeferredDelete) {
        if (!canDeferredDelete) {
            ee->accept();

            return true;
        }
    } else if (ee->type() == QEvent::Resize) {
        updateEditorGeometry();
        int marginsHeight = contentsMargins().top();
        resize(width(), icon->height() + edit->height() + ICON_MODE_ICON_SPACING + marginsHeight);
    } else if (ee->type() == QEvent::FontChange) {
        edit->setFont(font());
    } else if (ee->type() == QEvent::MouseButtonPress) {
        if (!inputOut) {
            inputOut = true;
            emit inputFocusOut();
            qDebug() << "FileIconItem::event():MouseButtonPress(inputFocusOut())";
            return true;
        }
    }

    return QFrame::event(ee);
}

bool FileIconItem::eventFilter(QObject *obj, QEvent *ee)
{
    switch (ee->type()) {
    case QEvent::Resize:
        if (obj == icon || obj == edit) {
            resize(width(), icon->height() + edit->height() + ICON_MODE_ICON_SPACING);
        }

        break;
    case QEvent::KeyPress: {
        if (obj != edit) {
            return QFrame::eventFilter(obj, ee);
        }

        QKeyEvent *event = static_cast<QKeyEvent *>(ee);

        if (event->key() != Qt::Key_Enter && event->key() != Qt::Key_Return) {
            if (event == QKeySequence::Undo) {
                editUndo();
            } else if (event == QKeySequence::Redo) {
                editRedo();
            } else {
                return QFrame::eventFilter(obj, ee);
            }

            disableEditTextStack = false;
            ee->accept();

            return true;
        }

        if (!(event->modifiers() & Qt::ShiftModifier)) {
            ee->accept();
            parentWidget()->setFocus();

            return true;
        } else {
            event->accept();
            return false;
        }

        //break;
    }
    case QEvent::FocusOut:
        if (obj == edit && qApp->focusWidget() != edit) {
            if (!inputOut && qobject_cast<DWidget *>(parent()->parent())->window()->isActiveWindow()) {
                inputOut = true;
                emit inputFocusOut();
                qDebug() << "FileIconItem::eventFilter():inputFocusOut";
            } else if (!inputOut && !qobject_cast<DWidget *>(parent()->parent())->window()->isActiveWindow()) {
                itemRefresh = true;
                qDebug() << __LINE__ << "FileIconItem::eventFilter(): item create edit refresh.";
                //                QPoint pos(edit->pos());
                //                QMouseEvent pressEvent(QEvent::MouseButtonPress, pos, Qt::LeftButton, Qt::LeftButton, Qt::NoModifier);
                //                QApplication::sendEvent(edit, &pressEvent);

                emit inputFocusOut();
                inputOut = true;
            }
            return true;
        }

        break;
    case QEvent::Show:
        updateEditorGeometry();

        break;
    default:
        break;
    }

    return QFrame::eventFilter(obj, ee);
}

void FileIconItem::updateEditorGeometry()
{
    edit->setFixedWidth(width());
    int text_height = int(edit->document()->size().height());

    if (edit->isReadOnly()) {
        if (edit->isVisible()) {
            edit->setFixedHeight(text_height);
        }
    } else {
        edit->setFixedHeight(qMin(fontMetrics().height() * 3 + TEXT_PADDING * 2, text_height));
    }
}

//void FileIconItem::updateStyleSheet()
//{
//    QString base = "FileIconItem[showBackground=true] QTextEdit {background: %1; color: %2;}";

//    base.append("FileIconItem QTextEdit {color: %3}");
//    base = base.arg(palette().color(QPalette::Background).name(QColor::HexArgb))
//               .arg(palette().color(QPalette::BrightText).name(QColor::HexArgb))
//               .arg(palette().color(QPalette::Text).name(QColor::HexArgb));

//    // WARNING: setStyleSheet will clean margins!!!!!!
//    auto saveContent = contentsMargins();
//    setStyleSheet(base);
//    setContentsMargins(saveContent);
//}

QString FileIconItem::editTextStackCurrentItem() const
{
    return editTextStack.value(editTextStackCurrentIndex);
}

QString FileIconItem::editTextStackBack()
{
    editTextStackCurrentIndex = qMax(0, editTextStackCurrentIndex - 1);
    const QString &text = editTextStackCurrentItem();

    return text;
}

QString FileIconItem::editTextStackAdvance()
{
    editTextStackCurrentIndex = qMin(editTextStack.count() - 1, editTextStackCurrentIndex + 1);
    const QString &text = editTextStackCurrentItem();

    return text;
}

void FileIconItem::pushItemToEditTextStack(const QString &item)
{
    if (disableEditTextStack) {
        return;
    }

    editTextStack.remove(editTextStackCurrentIndex + 1, editTextStack.count() - editTextStackCurrentIndex - 1);
    editTextStack.push(item);
    ++editTextStackCurrentIndex;
}

CanSetDragTextEdit::CanSetDragTextEdit(DWidget *parent)
    : DTextEdit(parent)
{
}

CanSetDragTextEdit::CanSetDragTextEdit(const QString &text, DWidget *parent)
    : DTextEdit(text, parent)
{
}

void CanSetDragTextEdit::setDragEnabled(const bool &bdrag)
{
    QTextEditPrivate *dd = reinterpret_cast<QTextEditPrivate *>(qGetPtrHelper(d_ptr));
    dd->control->setDragEnabled(bdrag);
}
