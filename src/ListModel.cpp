#include "ListModel.h"

#include "klocalizedstring.h"

//#########################################################################
// Listitem
//#########################################################################
ListItem::ListItem(QImage image): m_image(image) {

}

QImage *ListItem::previewIcon() {
	return &m_preview;
}

void ListItem::setImage(QImage image) {
	m_image = image;
	m_preview = m_image.scaledToHeight(100);
}

//#########################################################################
// Listmodel
//#########################################################################

ListModel::ListModel(QObject *parent) : QAbstractListModel(parent)
{

}

int ListModel::rowCount(const QModelIndex &parent) const {
	int length =  parent.isValid() ? 0 : m_scannedDocuments.length(); // ? why this order?
	return length;
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

	if (role == Qt::DisplayRole || role == Qt::EditRole)
		return QVariant(i18n("Teststring"));

	return QVariant();
}

//Qt::ItemFlags ListModel::flags(const QModelIndex &index) const {
//	if (!index.isValid())
//			return 0;

//	return Qt::ItemIsSelectable | Qt::ItemIsEditable | Qt::ItemIsUserCheckable;
//}

bool ListModel::appendItem(ListItem* item) {
	int count = m_scannedDocuments.count();
	insertRows(count, 1);

	QModelIndex itemIndex = createIndex(count, 0, item); // create index for the new item
	m_scannedDocuments[count] = item;
	return true;
}

//bool ListModel::setData(const QModelIndex &index, const QVariant &value, int role) {
//	return true;
//}

bool ListModel::removeRows(int row, int count, const QModelIndex &parent)  {
	beginRemoveRows(parent,row, row + count -1);
	for (int i = 0; i < count; i++)
		m_scannedDocuments.removeAt(row);
	endRemoveRows();
	return true;
}

bool ListModel::removeRow(int row, const QModelIndex &parent) {
	return removeRows(row, 1, parent);
}

bool ListModel::insertRows(int row, int count, const QModelIndex &parent) {
	beginInsertRows(parent, row, row + count -1);
	ListItem* item = nullptr;
	m_scannedDocuments.insert(row, item);
	endInsertRows();
	return true;
}

bool ListModel::insertRow(int row, const QModelIndex &parent) {
	return insertRows(row, 1, parent);
}

ListItem* ListModel::getItem(const QModelIndex &index) const {
	if (index.isValid()) {
		ListItem *item = static_cast<ListItem*>(index.internalPointer());
		if (item)
			return item;
	}
	return nullptr;
}
