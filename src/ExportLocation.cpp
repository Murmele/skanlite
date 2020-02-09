/* ============================================================
* Date        : 2010-07-07
* Description : Save location settings dialog.
*
* Copyright (C) 2010-2012 by Kare Sars <kare.sars@iki.fi>
* Copyright (C) 2014 by Gregor Mitsch: port to KDE5 frameworks
*
* This program is free software; you can redistribute it and/or
* modify it under the terms of the GNU General Public License as
* published by the Free Software Foundation; either version 2 of
* the License or (at your option) version 3 or any later version
* accepted by the membership of KDE e.V. (or its successor approved
*  by the membership of KDE e.V.), which shall act as a proxy
* defined in Section 14 of version 3 of the license.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License.
* along with this program.  If not, see <http://www.gnu.org/licenses/>
*
* ============================================================ */

#include "ExportLocation.h"

#include <QDebug>
#include <QFileDialog>
#include <QPushButton>
#include <QComboBox>

#include <KMessageBox>
#include <KStandardGuiItem>
#include <KIO/StatJob>
#include <KJobWidgets>

ExportLocation::ExportLocation(QWidget *parent)
    : QDialog(parent)
{
    setupUi(this);

    u_urlRequester->setMode(KFile::Directory);
	connect(u_urlRequester, &KUrlRequester::textChanged, this, &ExportLocation::updateGui);
	connect(u_filename, &QLineEdit::textChanged, this, &ExportLocation::updateGui);
}

ExportLocation::~ExportLocation()
{
}

void ExportLocation::accept()
{
	QUrl fileUrl(generateFileUrl());

	bool exists;
	if (fileUrl.isLocalFile()) {
		exists = QFileInfo(fileUrl.toLocalFile()).exists();
	}
	else {
		KIO::StatJob *statJob = KIO::stat(fileUrl, KIO::StatJob::DestinationSide, 0);
		KJobWidgets::setWindow(statJob, QApplication::activeWindow());
		exists = statJob->exec();
	}
	if (exists) {
		if (KMessageBox::warningContinueCancel(this,
			i18n("Do you want to overwrite \"%1\"?", fileUrl.fileName()),
			QString(),
			KStandardGuiItem::overwrite(),
			KStandardGuiItem::cancel(),
			QLatin1String("editorWindowSaveOverwrite")
		) ==  KMessageBox::Continue) {
			QDialog::accept();
		}
	} else
		QDialog::accept();
}

QUrl ExportLocation::generateFileUrl()
{
	const QString name = QString::fromLatin1("%2.pdf").arg(u_filename->text());
	QString dir = QDir::cleanPath(u_urlRequester->url().toString()).append(QLatin1Char('/')); //make sure whole value is processed as path to directory
	return QUrl(dir).resolved(QUrl(name));
}

void ExportLocation::updateGui()
{
	u_resultValue->setText(generateFileUrl().toString(QUrl::PreferLocalFile | QUrl::NormalizePathSegments));
}

void ExportLocation::getDir(void)
{
    const QString newDir = QFileDialog::getExistingDirectory(this, QString(), u_urlRequester->url().toLocalFile());
    if (!newDir.isEmpty()) {
        u_urlRequester->setUrl(QUrl::fromLocalFile(newDir));
    }
}

/*!
 * \brief ExportLocation::getFilePath
 * Return filepath + filename
 */
QString ExportLocation::getFileUrl()
{
	return u_resultValue->text();
}
