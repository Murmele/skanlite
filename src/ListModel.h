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
	ListItem();
	ListItem(QString name, QImage image, int id, int previewHeight);
	const QImage *previewIcon() const;
	void setPreviewIcon(QImage icon);
	const QImage *image() const;
	int id() const;
	void setId(int id);
	void setImage(const QImage *image, int previewHeight);
	void setImage(QImage image, int previewHeigth);
	void setName(QString name);
	void setChecked(bool checked);
	QString name() const;
	bool checked() const;
private:
	int m_id;
	QImage m_preview;
    QImage m_image;
	QString m_name;
	bool m_checked{true};
};

class ListModel: public QAbstractListModel
{
	Q_OBJECT

public:
        ListModel(QObject *parent = nullptr);
		bool appendImage(QImage& image);
        int rowCount(const QModelIndex &parent = QModelIndex()) const override;
        QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
		bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) override;
		bool setDataFromItem(const QModelIndex &index, const ListItem* item);
		Qt::ItemFlags flags(const QModelIndex &index) const override;
		QModelIndex index(int row, int column = 0, const QModelIndex &parent = QModelIndex()) const override;
		bool removeRow(int row, const QModelIndex &parent = QModelIndex());
		bool removeRows(int row, int count, const QModelIndex &parent = QModelIndex()) override;
		bool insertRow(int row, const QModelIndex &parent = QModelIndex());
		bool insertRows(int row, int count, const QModelIndex &parent = QModelIndex()) override;
        bool appendItem(ListItem* item);
		bool moveRows(const QModelIndex &sourceParent, int sourceRow, int count, const QModelIndex &destinationParent, int destinationChild) override;
		bool moveRow(const QModelIndex &sourceParent, int sourceRow, const QModelIndex &destinationParent, int destinationChild);
		Qt::DropActions supportedDropActions() const override;
		bool canDropMimeData(const QMimeData *data, Qt::DropAction action, int row, int column, const QModelIndex &parent);
		bool dropMimeData(const QMimeData *data, Qt::DropAction action, int row, int column, const QModelIndex &parent) override;
		QMimeData *mimeData(const QModelIndexList &indexes) const override;
		QStringList mimeTypes() const;
		bool removeItem(const QModelIndex& index);
		ListItem* getItem(const int row) const;
		int previewHeight();
		void deleteSelectedScans();
		bool changeSelectionOfScans(int checkstate);

Q_SIGNALS:
	void selectionChanged(Qt::CheckState checkstate);
	void currentItemChanged(const ListItem* item);
	void itemAboutToBeRemoved(const ListItem* item);

public Q_SLOTS:
	void changePreviewSize(int heigth);
	void currentItemChangedSlot(const QModelIndex &current, const QModelIndex &previous);

private:
        ListItem* getItem(const QModelIndex &index) const;
		ListItem* getItemFromId(int id) const ;
		void checkSelectionOfAllItems();
		Qt::CheckState m_checkSate{Qt::CheckState::Unchecked};
		int m_maxId{-1}; // -1 means no images are available
		bool m_suppressCheckStateChanges{false};
 private:
	QList<ListItem*> m_scannedDocuments;
	int m_previewHeight{100};
};

#endif // LISTMODEL_H
