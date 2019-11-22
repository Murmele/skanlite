#include "ListModel.h"

//#########################################################################
// Listitem
//#########################################################################
ListItem::ListItem(QImage image): m_image(image) {

}

QIcon* ListItem::previewIcon() {
	return &m_preview;
}

void ListItem::setImage(QImage image) {
	m_image = image;
}

//#########################################################################
// Listmodel
//#########################################################################

ListModel::ListModel(QObject *parent) : QAbstractListModel(parent)
{

}

int ListModel::rowCount(const QModelIndex &parent) const {
	return parent.isValid() ? m_scannedDocuments.length() : 0;
}

QVariant ListModel::data(const QModelIndex &index, int role) const {
	if (!index.isValid())
			return QVariant();

	if (role == Qt::DecorationRole) {
		ListItem* item = getItem(index);
		if (item)
			return *item->previewIcon();
	}

	if (role == Qt::BackgroundRole)
		return QColor(Qt::red);

	return QVariant();
}

bool ListModel::setData(const QModelIndex &index, const QVariant &value, int role) {
	return true;
}

Qt::ItemFlags ListModel::flags(const QModelIndex &index) const {
	if (!index.isValid())
			return 0;

	return Qt::ItemIsSelectable | Qt::ItemIsEditable | Qt::ItemIsUserCheckable;
}

bool ListModel::appendItem(ListItem* item) {
	beginInsertRows(QModelIndex(), m_scannedDocuments.count(), m_scannedDocuments.count());
	m_scannedDocuments.append(item);
	endInsertRows();
	return true;
}

//bool ListModel::removeRows(int row, int count, const QModelIndex &parent)  {
//	beginRemoveRows(parent,row, row + count -1);
//	endRemoveRows();
//}

//bool ListModel::removeRow(int row, const QModelIndex &parent) {

//}

//bool ListModel::insertRows(int row, int count, const QModelIndex &parent) {
//	beginInsertRows(parent, row, row + count -1);
//	endInsertRows();
//}

//bool ListModel::insertRow(int row, const QModelIndex &parent) {

//}

ListItem* ListModel::getItem(const QModelIndex &index) const {
	if (index.isValid()) {
		ListItem *item = static_cast<ListItem*>(index.internalPointer());
		if (item)
			return item;
	}
	return nullptr;
}
