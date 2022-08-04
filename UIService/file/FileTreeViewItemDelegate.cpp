/*
* Copyright (C) 2019 ~ 2020 Uniontech Software Technology Co.,Ltd.
*
* Author:     wangzhixuan <wangzhixuan@uniontech.com>
*
* Maintainer: wangzhixuan <wangzhixuan@uniontech.com>
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
#include "FileTreeViewItemDelegate.h"

#include <QLineEdit>

FileTreeViewItemDelegate::FileTreeViewItemDelegate(QAbstractItemView *parent)
    : BaseTreeViewItemDelegate(parent)
{
    m_treeType = E_Widget_File;
}

QWidget *FileTreeViewItemDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    Q_UNUSED(option);
    Q_UNUSED(index);

    auto item = new QLineEdit(parent);
    connect(item, &QLineEdit::editingFinished, this, &FileTreeViewItemDelegate::onEditWidgetFocusOut);
    return item;
}

void FileTreeViewItemDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
    auto item = qobject_cast<QLineEdit *>(editor);
    if (!item)
        return;

    item->setText(index.data().toString());
}

void FileTreeViewItemDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
    auto item = qobject_cast<QLineEdit *>(editor);
    if (!item)
        return;

    QString text = item->text().trimmed();
    text = text.remove('\n');
    text = text.remove('\r');
    model->setData(index, text);

    //  pms 38936
    m_pItemView->closePersistentEditor(index);
}

void FileTreeViewItemDelegate::updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    Q_UNUSED(index);

    editor->setGeometry(option.rect.x() + 46, option.rect.y(), option.rect.width() - 46, option.rect.height());
}

void FileTreeViewItemDelegate::onEditWidgetFocusOut()
{
    auto textEdit = static_cast<QLineEdit *>(sender());
    if (textEdit) {
        if (!textEdit->hasFocus()) { //解决按回车执行两次问题
            QString fileName = textEdit->text().trimmed();
            fileName = fileName.remove('\n');
            fileName = fileName.remove('\r');

            emit sigFileNewFolder(fileName);
        }
    }
}
