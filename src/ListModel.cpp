#include "ListModel.h"

#include "klocalizedstring.h"

#include <QMimeData>

//#########################################################################
// Listitem
//#########################################################################
ListItem::ListItem() {

}

ListItem::ListItem(QString name, QImage image, int id): m_name(name), m_id(id) {
	setImage(image);
}

const QImage *ListItem::previewIcon() const{
	return &m_preview;
}

void ListItem::setPreviewIcon(QImage icon) {
	m_preview = icon;
}

const QImage* ListItem::image() const {
	return &m_image;
}

void ListItem::setId(int id) {
	m_id = id;
}

void ListItem::setImage(const QImage* image) {
	m_image = *image;
	m_preview = m_image.scaledToHeight(100);
}

void ListItem::setImage(QImage image) {
	m_image = image;
	m_preview = m_image.scaledToHeight(100);
}

QString ListItem::name() const{
	return m_name;
}

bool ListItem::checked() const {
	return m_checked;
}

int ListItem::id() const {
	return m_id;
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
	return Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsUserCheckable | Qt::ItemIsDropEnabled;
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

bool ListModel::canDropMimeData(const QMimeData *data,
	Qt::DropAction action, int row, int column, const QModelIndex &parent)
{
	Q_UNUSED(action);
	Q_UNUSED(row);
	Q_UNUSED(parent);

	if (!data->hasFormat(i18n("skanLite/ScannedDocuments/Id")))
		return false;

	if (column > 0)
		return false;

	return true;
}

QMimeData* ListModel::mimeData(const QModelIndexList &indexes) const {
	QMimeData* mimeData = new QMimeData();

	// do I have to check this?
	if (!indexes.count())
		return mimeData;

	QByteArray encodedData;
	QDataStream stream(&encodedData, QIODevice::WriteOnly);

	for (auto index : indexes) {
		ListItem* item = getItem(index);
		stream << item->id();
	}

	mimeData->setData(i18n("skanLite/ScannedDocuments/Id"), encodedData);

	return mimeData;
}

/*!
 * \brief ListModel::mimeTypes
 * Needed, because otherwise dropMimeData is never called, because the mimeType is not valid
 * \return
 */
QStringList ListModel::mimeTypes() const
{
	QStringList types;
	types << i18n("skanLite/ScannedDocuments/Id");
	return types;
}

bool ListModel::dropMimeData(const QMimeData *data, Qt::DropAction action, int row, int column, const QModelIndex &parent) {

	if (action == Qt::IgnoreAction)
		return true;

	if (!data->hasFormat(i18n("skanLite/ScannedDocuments/Id")))
		return false;

	if (column > 0)
		return false;

	if (parent.isValid())
		return false; // drop on another item makes no sense, so ignore these cases

	// https://doc.qt.io/archives/4.6/model-view-dnd.html
	if (row == -1)
		row = rowCount(QModelIndex());

	QByteArray encodedData = data->data(i18n("skanLite/ScannedDocuments/Id"));
	QDataStream stream(&encodedData, QIODevice::ReadOnly);

	QVector<int> ids;
	while (!stream.atEnd()) {
		int id;
		stream >> id;
		ids.append(id);
	}


	for (int i = 0; i < ids.count(); i++) {
		ListItem* item = getItemFromId(ids[i]); // get item before inserting rows, because otherwise the order is wrong

		if (!item)
			return false;

		insertRows(row + i, 1);
		QModelIndex idx = index(row + i, 0);

		setDataFromItem(idx, item);
	}

	// true to call deleteRow() after inserting
	// https://stackoverflow.com/questions/59130393/qlistview-moverow-from-model-not-called
	return true;
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

bool ListModel::appendImage(QImage& image) {
	m_maxId++;

	ListItem* item = new ListItem(QString::number(m_maxId + 1), image, m_maxId);
	return appendItem(item);
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

	if (role == Qt::CheckStateRole) {
		if (m_suppressCheckStateChanges)
			return false;
		item->setChecked(Qt::Checked == value.toInt());
		checkSelectionOfAllItems();
	}

	if (role == Qt::DecorationRole)
		item->setPreviewIcon(value.value<QImage>());

	dataChanged(index, index, QVector<int>(role));
	return true;
}

bool ListModel::setDataFromItem(const QModelIndex &index, const ListItem* item) {
	if (!index.isValid())
		return false;

	ListItem* newItem = getItem(index);

	newItem->setImage(item->image());
	newItem->setName(item->name());
	newItem->setId(item->id());
	newItem->setChecked(item->checked());

	dataChanged(index, index, QVector<int>(Qt::EditRole));
	dataChanged(index, index, QVector<int>(Qt::CheckStateRole));
	dataChanged(index, index, QVector<int>(Qt::DecorationRole));

	return true;
}

/*!
 * \brief ListModel::removeItem
 * Removes item from the model
 * \param index index of the item which should be removed
 * \return true if success, else false
 */
bool ListModel::removeItem(const QModelIndex& index) {
	return removeRows(index.row(), 1, index.parent());
}

bool ListModel::removeRows(int row, int count, const QModelIndex &parent)  {
	beginRemoveRows(parent,row, row + count -1);
	for (int i = 0; i < count; i++) {
		emit itemAboutToBeRemoved(m_scannedDocuments[row]);
		m_scannedDocuments.removeAt(row);
	}
	endRemoveRows();

	checkSelectionOfAllItems();
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

	checkSelectionOfAllItems();
	return true;
}

bool ListModel::insertRow(int row, const QModelIndex &parent) {
	return insertRows(row, 1, parent);
}

ListItem* ListModel::getItem(const int row) const {
	QModelIndex idx = index(row, 0);
	return getItem(idx);
}

ListItem* ListModel::getItem(const QModelIndex &index) const {
	if (!index.isValid())
		return nullptr;

	ListItem *item = static_cast<ListItem*>(index.internalPointer());
	if (item)
		return item;

	return nullptr;
}

ListItem* ListModel::getItemFromId(int id) const {
	if (id < 0)
		return nullptr;

	for (auto item : m_scannedDocuments) {
		if (item->id() == id)
			return item;
	}
	return nullptr;
}

/*!
 * \brief ListModel::deleteSelectedScans
 * Delets all scans which are selected in the listview
 */
void ListModel::deleteSelectedScans() {

	int count = rowCount();
	for (int i = count - 1; i >= 0; i--) {
		QModelIndex idx = index(i, 0, QModelIndex());
		if (data(idx, Qt::CheckStateRole).toInt() == Qt::CheckState::Checked)
			removeItem(idx);
	}
}

bool ListModel::changeSelectionOfScans(int checkstate) {
	if (rowCount() == 0)
		return false;

	for (int i = 0; i < rowCount(); i++) {
		QModelIndex idx = index(i, 0, QModelIndex());
		setData(idx, checkstate, Qt::CheckStateRole);
	}
	return true;
}

void ListModel::checkSelectionOfAllItems() {

	bool unchecked = false; // set to true when one item is unchecked
	bool checked = false; // set to true when on item is checked

	Qt::CheckState checkState = Qt::CheckState::Unchecked;

	if (rowCount() == 0)
		return;

	for (int i = 0; i < rowCount(); i++) {
		QModelIndex idx = index(i, 0, QModelIndex());
		if (data(idx, Qt::CheckStateRole).toInt() == Qt::CheckState::Checked) {
			checked = true;
			if (unchecked)
				break;
			continue;
		}

		if (data(idx, Qt::CheckStateRole).toInt() == Qt::CheckState::Unchecked) {
			unchecked = true;
			if (checked)
				break;
			continue;
		}
	}

	if (unchecked && checked)
		checkState = Qt::CheckState::PartiallyChecked;
	else if (checked)
		checkState = Qt::CheckState::Checked;
	else
		checked = Qt::CheckState::Unchecked;

	m_suppressCheckStateChanges = true;
	if (checkState != m_checkSate) {
		m_checkSate = checkState;
		emit selectionChanged(m_checkSate);
	}
	m_suppressCheckStateChanges = false;
}

void ListModel::currentItemChangedSlot(const QModelIndex &current, const QModelIndex &previous) {
	emit currentItemChanged(getItem(current));
}