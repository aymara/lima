// Copyright 2002-2013 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

#include "DictionnaryEntryEditDlg.h"
using namespace Lima::Pelf;

DictionnaryEntryEditDlg::DictionnaryEntryEditDlg (QWidget* p) :
    QDialog(p),
    Ui::DictionnaryEntryEditDialog(),
    isNew(false)
{
    setupUi(this);
}

void DictionnaryEntryEditDlg::init (ResourceEditorTableModel* rem, AbstractResourceEntry* are)
{
    if(are == 0)
    {
///@TODO Possible memory leak:  ensure that this object will be deleted
        dictionnaryEntry = new DictionnaryEntry();
        connect(
            this,
            SIGNAL(updateEntry(AbstractResourceEntry*)),
            rem,
            SLOT(addEntry(AbstractResourceEntry*))
        );
    }
    else
    {
        dictionnaryEntry = (DictionnaryEntry*)are;
    }
    lemmaInp->setText(dictionnaryEntry->lemma);
    connect(lemmaInp, SIGNAL(textChanged(QString&)), this, SLOT(checkValidity()));
    normalizationInp->setText(dictionnaryEntry->normalization);
    for(int i = 0; i < rem->categoryList.size(); i++)
        categoryCb->addItem(rem->categoryList.at(i));
    categoryCb->setCurrentIndex(categoryCb->findText(dictionnaryEntry->category));
    connect(categoryCb, SIGNAL(currentIndexChanged(int)), this, SLOT(checkValidity()));
    connect(buttonBox, SIGNAL(accepted()), this, SLOT(submit()));
    checkValidity();
    show();
/*    if (are == 0)
    {
      delete dictionnaryEntry;
    }*/
}

void DictionnaryEntryEditDlg::checkValidity ()
{
    bool checked = true;
    checked &= !lemmaInp->text().isEmpty();
    checked &= !categoryCb->itemText(categoryCb->currentIndex()).isEmpty();
    buttonBox->button(QDialogButtonBox::Ok)->setEnabled(checked);
}

void DictionnaryEntryEditDlg::submit ()
{
    dictionnaryEntry->lemma = lemmaInp->text();
    dictionnaryEntry->normalization = normalizationInp->text();
    dictionnaryEntry->category = categoryCb->itemText(categoryCb->currentIndex());
    dictionnaryEntry->displayable = true;
    Q_EMIT updateEntry(dictionnaryEntry);
}

#include "DictionnaryEntryEditDlg.moc"
