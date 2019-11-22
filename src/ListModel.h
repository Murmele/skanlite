#ifndef LISTMODEL_H
#define LISTMODEL_H

#include <QAbstractItemModel>
#include <QStandardItem>
#include <QList>
#include <QIcon>

// https://doc.qt.io/qt-5/qabstractlistmodel.html
// https://stackoverflow.com/questions/42296304/how-add-icon-in-qlistview-using-qstringlistmodel

class ListItem
{
public:
    ListItem(QImage image);
	QImage* previewIcon();
    void setImage(QImage image);
private:
	QImage m_preview;
    QImage m_image;
};

class ListModel: public QAbstractListModel
{
	Q_OBJECT

public:
        ListModel(QObject *parent = nullptr);
        int rowCount(const QModelIndex &parent = QModelIndex()) const override;
        QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
//		bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) override;
//        Qt::ItemFlags flags(const QModelIndex &index) const override;
		bool removeRow(int row, const QModelIndex &parent = QModelIndex());
		bool removeRows(int row, int count, const QModelIndex &parent = QModelIndex()) override;
		bool insertRow(int row, const QModelIndex &parent = QModelIndex());
		bool insertRows(int row, int count, const QModelIndex &parent = QModelIndex()) override;
        bool appendItem(ListItem* item);
private:
        ListItem* getItem(const QModelIndex &index) const;
 private:
    QList<ListItem*> m_scannedDocuments;
};

#endif // LISTMODEL_H
