#pragma once

#include <QAbstractListModel>
#include <QObject>

class SearchModelItem : public QObject {
    Q_OBJECT
 public:
    SearchModelItem() {}

    SearchModelItem(const QString& type, QObject* model) : m_type(type), m_model(model) {}

    QString  item_type() const { return m_type; }
    QObject* item_model() const { return m_model; }

 private:
    QString  m_type;
    QObject* m_model;
};

class SearchModel : public QAbstractListModel {
    Q_OBJECT
    Q_PROPERTY(int count READ count WRITE setCount NOTIFY countChanged)

 public:
    enum SearchRoles { TypeRole = Qt::UserRole + 1, ModelRole };

    explicit SearchModel(QObject* parent = nullptr);
    ~SearchModel() {}

    int count() const;

    int                    rowCount(const QModelIndex& p = QModelIndex()) const;
    QVariant               data(const QModelIndex& index, int role = Qt::DisplayRole) const;
    QHash<int, QByteArray> roleNames() const;

    void append(SearchModelItem* o);
    void insert(SearchModelItem* o, int i);

    Q_INVOKABLE QObject* returnSubModel(int index) { return m_data.at(index)->item_model(); }
    Q_INVOKABLE void     clear();

 public slots:
    void setCount(int count);

 signals:
    void countChanged(int count);

 private:
    int                     m_count;
    QList<SearchModelItem*> m_data;
};

class SearchModelListItem {
 public:
    SearchModelListItem(const QString& key, const QString& type, const QString& title, const QString& subtitle,
                        const QString& imageUrl, const QVariant& commands)
        : m_key(key), m_type(type), m_title(title), m_subtitle(subtitle), m_imageUrl(imageUrl), m_commands(commands) {}

    QString  item_key() const { return m_key; }
    QString  item_type() const { return m_type; }
    QString  item_title() const { return m_title; }
    QString  item_subtitle() const { return m_subtitle; }
    QString  item_imageUrl() const { return m_imageUrl; }
    QVariant item_commands() const { return m_commands; }

 private:
    QString  m_key;
    QString  m_type;
    QString  m_title;
    QString  m_subtitle;
    QString  m_imageUrl;
    QVariant m_commands;
};

class SearchModelList : public QAbstractListModel {
    Q_OBJECT
    Q_PROPERTY(int count READ count WRITE setCount NOTIFY countChanged)

 public:
    enum SearchRoles { KeyRole = Qt::UserRole + 1, TypeRole, TitleRole, SubTitleRole, ImageUrlRole, CommandsRole };

    explicit SearchModelList(QObject* parent = nullptr);
    ~SearchModelList() {}

    int                    count() const;
    int                    rowCount(const QModelIndex& p = QModelIndex()) const;
    QVariant               data(const QModelIndex& index, int role = Qt::DisplayRole) const;
    QHash<int, QByteArray> roleNames() const;

    void append(SearchModelListItem& o);

 public slots:
    void setCount(int count);

 signals:
    void countChanged(int count);

 private:
    int                        m_count;
    QList<SearchModelListItem> m_data;
};
