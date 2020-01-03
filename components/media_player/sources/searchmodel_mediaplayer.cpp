/******************************************************************************
 *
 * Copyright (C) 2018-2019 Marton Borzak <hello@martonborzak.com>
 *
 * This file is part of the YIO-Remote software project.
 *
 * YIO-Remote software is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * YIO-Remote software is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with YIO-Remote software. If not, see <https://www.gnu.org/licenses/>.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *****************************************************************************/

#include "searchmodel_mediaplayer.h"

SearchModel::SearchModel(QObject *parent) : QAbstractListModel(parent), m_count(0) {}

int SearchModel::count() const { return m_count; }

int SearchModel::rowCount(const QModelIndex &p) const {
    Q_UNUSED(p)
    return m_data.size();
}

QVariant SearchModel::data(const QModelIndex &index, int role) const {
    if (index.row() < 0 || index.row() >= m_data.count()) return QVariant();
    const SearchModelItem *item = m_data[index.row()];
    switch (role) {
        case TypeRole:
            return item->item_type();
        case ModelRole:
            return QVariant::fromValue(item->item_model());
    }
    return QVariant();
}

QHash<int, QByteArray> SearchModel::roleNames() const {
    QHash<int, QByteArray> roles;
    roles[TypeRole] = "title";
    roles[ModelRole] = "item_data";
    return roles;
}

void SearchModel::append(SearchModelItem *o) {
    int i = m_data.size();
    beginInsertRows(QModelIndex(), i, i);
    m_data.append(o);

    // Emit changed signals
    emit countChanged(count());

    endInsertRows();
}

void SearchModel::insert(SearchModelItem *o, int i) {
    beginInsertRows(QModelIndex(), i, i);
    m_data.insert(i, o);

    // Emit changed signals
    emit countChanged(count());

    endInsertRows();
}

SearchModelItem *SearchModel::get(int i) { return m_data.value(i); }

void SearchModel::clear() {
    beginResetModel();
    m_data.clear();
    endResetModel();
}

void SearchModel::setCount(int count) {
    if (m_count == count) return;

    m_count = count;
    emit countChanged(m_count);
}

SearchModelList::SearchModelList(QObject *parent) : QAbstractListModel(parent), m_count(0) {}

int SearchModelList::count() const { return m_count; }

int SearchModelList::rowCount(const QModelIndex &p) const {
    Q_UNUSED(p)
    return m_data.size();
}

QVariant SearchModelList::data(const QModelIndex &index, int role) const {
    //    Q_UNUSED(role)
    //    return QVariant::fromValue(m_data[index.row()]);
    if (index.row() < 0 || index.row() >= m_data.count()) return QVariant();
    const SearchModelListItem &item = m_data[index.row()];
    switch (role) {
        case KeyRole:
            return item.item_key();
        case TypeRole:
            return item.item_type();
        case TitleRole:
            return item.item_title();
        case SubTitleRole:
            return item.item_subtitle();
        case ImageUrlRole:
            return item.item_imageUrl();
        case CommandsRole:
            return item.item_commands();
    }
    return QVariant();
}

QHash<int, QByteArray> SearchModelList::roleNames() const {
    QHash<int, QByteArray> roles;
    roles[KeyRole] = "item_key";
    roles[TypeRole] = "item_type";
    roles[TitleRole] = "item_title";
    roles[SubTitleRole] = "item_subtitle";
    roles[ImageUrlRole] = "item_image";
    roles[CommandsRole] = "item_commands";
    return roles;
}

void SearchModelList::append(SearchModelListItem &o) {
    int i = m_data.size();
    beginInsertRows(QModelIndex(), i, i);
    m_data.append(o);

    // Emit changed signals
    emit countChanged(count());

    endInsertRows();
}

void SearchModelList::setCount(int count) {
    if (m_count == count) return;

    m_count = count;
    emit countChanged(m_count);
}
