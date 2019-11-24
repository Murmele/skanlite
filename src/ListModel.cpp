#include "ListModel.h"

#include "klocalizedstring.h"

//#########################################################################
// Listitem
//#########################################################################
ListItem::ListItem(QString name, QImage image): m_name(name), m_checked(false) {
	setImage(image);
}

QImage *ListItem::previewIcon() {
	return &m_preview;
}

void ListItem::setImage(QImage image) {
	m_image = image;
	m_preview = m_image.scaledToHeight(100);
}

QString ListItem::name() {
	return m_name;
}

bool ListItem::checked() {
	return m_checked;
}

void ListItem::setName(QString name) {
	m_name = name;
}

void ListItem::setChecked(bool checked) {
	m_checked = checked;
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

// https://stackoverflow.com/questions/17786086/how-set-qcheckbox-in-qabstractitemmodel
QVariant ListModel::data(const QModelIndex &index, int role) const {
	if (!index.isValid())
			return QVariant();

	ListItem* item = getItem(index);
	if (!item)
		return QVariant();

	if (role == Qt::DecorationRole)
			return *item->previewIcon();

// Just for debugging
//	if (role == Qt::BackgroundRole)
//		return QColor(Qt::red);

	if (role == Qt::DisplayRole || role == Qt::EditRole)
		return item->name();

	if (role == Qt::CheckStateRole)
		return item->checked();

	return QVariant();
}
// https://www.qtcentre.org/threads/23258-How-to-reorder-items-in-QListView
Qt::ItemFlags ListModel::flags(const QModelIndex &index) const {
	if (index.isValid())
		return Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsUserCheckable |  Qt::ItemIsDragEnabled;
	return Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsUserCheckable |  Qt::ItemIsDragEnabled | Qt::ItemIsDropEnabled;
}

QModelIndex ListModel::index(int row, int column, const QModelIndex &parent) const {
	if (column != 0 || row < 0 || row >= m_scannedDocuments.count())
		return QModelIndex();

	ListItem* item = m_scannedDocuments[row];
	QModelIndex index = createIndex(row, column, item);
	return index;
}

bool ListModel::appendItem(ListItem* item) {
	int count = m_scannedDocuments.count();
	insertRows(count, 1);

	//QModelIndex itemIndex = createIndex(count, 0, item); // create index for the new item
	m_scannedDocuments[count] = item;
	return true;
}

/*!
 * This function is called from the view when something was changed
 * \param index
 * \param value
 * \param role
 * \return
 */
bool ListModel::setData(const QModelIndex &index, const QVariant &value, int role) {
	if (!index.isValid())
		return false;

	ListItem* item = getItem(index);
	if (!item)
		return false;

	if (role == Qt::EditRole)
		item->setName(value.toString());

	if (role == Qt::CheckStateRole)
		item->setChecked(value.toBool());

	return true;
}

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
	if (!index.isValid())
		return nullptr;

	ListItem *item = static_cast<ListItem*>(index.internalPointer());
	if (item)
		return item;

	return nullptr;
}
