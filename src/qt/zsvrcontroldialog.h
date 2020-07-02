// Copyright (c) 2017-2020 The PIVX developers
// Distributed under the MIT/X11 software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef ZSVRCONTROLDIALOG_H
#define ZSVRCONTROLDIALOG_H

#include <QDialog>
#include <QTreeWidgetItem>
#include "zsvr/zerocoin.h"

class CZerocoinMint;
class WalletModel;

namespace Ui {
class ZSvrControlDialog;
}

class CZSvrControlWidgetItem : public QTreeWidgetItem
{
public:
    explicit CZSvrControlWidgetItem(QTreeWidget *parent, int type = Type) : QTreeWidgetItem(parent, type) {}
    explicit CZSvrControlWidgetItem(int type = Type) : QTreeWidgetItem(type) {}
    explicit CZSvrControlWidgetItem(QTreeWidgetItem *parent, int type = Type) : QTreeWidgetItem(parent, type) {}

    bool operator<(const QTreeWidgetItem &other) const;
};

class ZSvrControlDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ZSvrControlDialog(QWidget *parent);
    ~ZSvrControlDialog();

    void setModel(WalletModel* model);

    static std::set<std::string> setSelectedMints;
    static std::set<CMintMeta> setMints;
    static std::vector<CMintMeta> GetSelectedMints();

private:
    Ui::ZSvrControlDialog *ui;
    WalletModel* model;

    void updateList();
    void updateLabels();

    enum {
        COLUMN_CHECKBOX,
        COLUMN_DENOMINATION,
        COLUMN_PUBCOIN,
        COLUMN_VERSION,
        COLUMN_CONFIRMATIONS,
        COLUMN_ISSPENDABLE
    };
    friend class CZSvrControlWidgetItem;

private Q_SLOTS:
    void updateSelection(QTreeWidgetItem* item, int column);
    void ButtonAllClicked();
};

#endif // ZPIVCONTROLDIALOG_H