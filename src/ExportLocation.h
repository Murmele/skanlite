/* ============================================================
 * Date        : 2010-07-07
 * Description : Save location settings dialog.
 *
 * Copyright (C) 2008-2012 by Kåre Särs <kare.sars@iki .fi>
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

#ifndef EXPORT_LOCATION_H
#define EXPORT_LOCATION_H

#include "ui_ExportLocation.h"

#include <QDialog>

class ExportLocation : public QDialog, public Ui_ExportLocation
{
    Q_OBJECT
public:
	explicit ExportLocation(QWidget *parent = nullptr);
	~ExportLocation();
	QString getFileUrl();

private Q_SLOTS:
	void accept() override;
    void updateGui();
    void getDir();
	QUrl generateFileUrl();
};

#endif // EXPORT_LOCATION_H

