#include "ListModel.h"

#include "klocalizedstring.h"

//#########################################################################
// Listitem
//#########################################################################
ListItem::ListItem() {

}

ListItem::ListItem(QString name, QImage image): m_name(name), m_checked(false) {
	setImage(image);
}

QImage *ListItem::previewIcon() {
	return &m_preview;
}

void ListItem::setPreviewIcon(QImage icon) {
	m_preview = icon;
}

QImage* ListItem::image() {
	return &m_image;
}

void ListItem::setImage(QImage* image) {
	m_image = *image;
	m_preview = m_image.scaledToHeight(100);
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

	if (role == Qt::CheckStateRole) {
		if (item->checked())
			return Qt::CheckState::Checked;
		return Qt::CheckState::Unchecked;
	}

	return QVariant();
}
// https://www.qtcentre.org/threads/23258-How-to-reorder-items-in-QListView
Qt::ItemFlags ListModel::flags(const QModelIndex &index) const {
	if (index.isValid())
		return Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsUserCheckable |  Qt::ItemIsDragEnabled | Qt::ItemIsDropEnabled | Qt::ItemIsEditable;
	return Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsUserCheckable |  Qt::ItemIsDragEnabled | Qt::ItemIsDropEnabled;
}

/*!
 * Must be reimplemented, because moving rows should be possible
 * \brief ListModel::moveRows
 * \param sourceParent
 * \param sourceRow
 * \param count
 * \param destinationParent
 * \param destinationChild
 * \return
 */
bool ListModel::moveRows(const QModelIndex &sourceParent, int sourceRow, int count, const QModelIndex &destinationParent, int destinationChild) {
	if (sourceRow < 0
			//|| sourceRow + count - 1 >= rowCount(sourceParent)
			|| destinationChild <= 0
			//|| destinationChild > rowCount(destinationParent)
			|| sourceRow == destinationChild - 1
			|| count <= 0) {
			return false;
		}

	return true;
}

bool ListModel::moveRow(const QModelIndex &sourceParent, int sourceRow, const QModelIndex &destinationParent, int destinationChild) {

	return true;
}

/*!
 * Support only move actions, because otherwise the image is copied
 * \brief ListModel::supportedDropActions
 * \return
 */
Qt::DropActions ListModel::supportedDropActions() const {
	return Qt::MoveAction;
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
		item->setChecked(Qt::Checked == value.toInt());

	if (role == Qt::DecorationRole)
		item->setPreviewIcon(value.value<QImage>());

	dataChanged(index, index, QVector<int>(role));
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
	ListItem* item = new ListItem();
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
