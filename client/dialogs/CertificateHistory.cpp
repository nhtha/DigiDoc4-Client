/*
 * QDigiDocCrypto
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 *
 */

#include "CertificateHistory.h"
#include "ui_CertificateHistory.h"

#include "Styles.h"
#include "common/SslCertificate.h"
#include "dialogs/CertificateDetails.h"
#include "effects/Overlay.h"

#include <QDebug>
#include <QFile>
#include <QMessageBox>
#include <QStandardPaths>
#include <QtCore/QXmlStreamReader>
#include <QtCore/QXmlStreamWriter>


bool HistoryCertData::operator==(const HistoryCertData& other)
{
	return	this->CN == other.CN &&
			this->type == other.type &&
			this->issuer == other.issuer &&
			this->expireDate == other.expireDate;
}


CertificateHistory::CertificateHistory(QList<HistoryCertData>& historyCertData, QWidget *parent)
:	QDialog( parent )
,	ui(new Ui::CertificateHistory)
,	historyCertData(historyCertData)
{
	ui->setupUi(this);
	setWindowFlags( Qt::Dialog | Qt::FramelessWindowHint );
	setWindowModality( Qt::ApplicationModal );

	QFont condensed = Styles::font(Styles::Condensed, 12);
	ui->close->setFont(condensed);
	ui->select->setFont(condensed);
	ui->remove->setFont(condensed);

	QStringList horzHeaders;
	horzHeaders << tr("Owner") << tr("Type") << tr("Issuer") << tr("Expiry date");
	ui->view->setHeaderLabels(horzHeaders);

	connect(ui->close, &QPushButton::clicked, this, &CertificateHistory::reject);
	connect(ui->select, &QPushButton::clicked, this, &CertificateHistory::select);
	connect(ui->remove, &QPushButton::clicked, this, &CertificateHistory::remove);

	this->historyCertData = historyCertData;
	fillView();
	for(int i = 0; i < 4; i++)
		ui->view->resizeColumnToContents(i);

#ifdef Q_OS_WIN32
	setStyleSheet("QWidget#CertificateHistory{ border-radius: 2px; background-color: #FFFFFF; border: solid #D9D9D8; border-width: 1px 1px 1px 1px;}");
#endif
}

CertificateHistory::~CertificateHistory()
{
	delete ui;
}

void CertificateHistory::fillView()
{
	ui->view->clear();
	for(const HistoryCertData& certData : historyCertData)
	{
		QTreeWidgetItem *i = new QTreeWidgetItem( ui->view );
		i->setText( 0, certData.CN );
		i->setText( 1, certData.type );
		i->setText( 2, certData.issuer );
		i->setText( 3, certData.expireDate );
		ui->view->addTopLevelItem( i );
	}
}

void CertificateHistory::getSelectedItems(QList<HistoryCertData>& selectedCertData)
{
	auto selectedRows = ui->view->selectedItems();

	for(QTreeWidgetItem* selectedRow : selectedRows)
	{
		selectedCertData.append(
			{
				selectedRow->data(0, Qt::DisplayRole).toString(),
				selectedRow->data(1, Qt::DisplayRole).toString(),
				selectedRow->data(2, Qt::DisplayRole).toString(),
				selectedRow->data(3, Qt::DisplayRole).toString(),
					});
	}
}


void CertificateHistory::select()
{
	QList<HistoryCertData> selectedCertData;

	getSelectedItems(selectedCertData);
	emit addSelectedCerts(selectedCertData);
}

void CertificateHistory::remove()
{
	QList<HistoryCertData> selectedCertData;

	getSelectedItems(selectedCertData);
	emit removeSelectedCerts(selectedCertData);
	fillView();
}

int CertificateHistory::exec()
{
	Overlay overlay(parentWidget());
	overlay.show();
	auto rc = QDialog::exec();
	overlay.close();

	return rc;
}