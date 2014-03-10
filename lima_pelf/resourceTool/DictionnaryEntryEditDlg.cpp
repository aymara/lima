/*
 *    Copyright 2002-2013 CEA LIST
 * 
 *    This file is part of LIMA.
 * 
 *    LIMA is free software: you can redistribute it and/or modify
 *    it under the terms of the GNU Affero General Public License as published by
 *    the Free Software Foundation, either version 3 of the License, or
 *    (at your option) any later version.
 * 
 *    LIMA is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU Affero General Public License for more details.
 * 
 *    You should have received a copy of the GNU Affero General Public License
 *    along with LIMA.  If not, see <http://www.gnu.org/licenses/>
 */
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
    connect(lemmaInp, SIGNAL(textChanged(const QString&)), this, SLOT(checkValidity()));
    normalizationInp->setText(dictionnaryEntry->normalization);
    for(int i = 0; i < rem->categoryList.size(); i++)
        categoryCb->addItem(rem->categoryList.at(i));
    categoryCb->setCurrentIndex(categoryCb->findText(dictionnaryEntry->category));
    connect(categoryCb, SIGNAL(currentIndexChanged(const int)), this, SLOT(checkValidity()));
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
    emit updateEntry(dictionnaryEntry);
}

#include "DictionnaryEntryEditDlg.moc"
