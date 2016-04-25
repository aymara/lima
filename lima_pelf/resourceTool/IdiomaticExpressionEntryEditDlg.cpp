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
#include "IdiomaticExpressionEntryEditDlg.h"
using namespace Lima::Pelf;

IdiomaticExpressionEntryEditDlg::IdiomaticExpressionEntryEditDlg (QWidget* p) :
    QDialog(p),
    Ui::IdiomaticExpressionEntryEditDialog(),
    idiomaticExpressionEntry(0),
    isNew(false)
{
    setupUi(this);
}

void IdiomaticExpressionEntryEditDlg::init (ResourceEditorTableModel* rem, AbstractResourceEntry* are)
{
    qDebug() << "IdiomaticExpressionEntryEditDlg::init" << (void*)rem << (void*)are;
    if(are == 0)
    {
        idiomaticExpressionEntry = new IdiomaticExpressionEntry();
        connect(
            this,
            SIGNAL(updateEntry(AbstractResourceEntry*)),
            rem,
            SLOT(addEntry(AbstractResourceEntry*))
        );
    }
    else
    {
        idiomaticExpressionEntry = (IdiomaticExpressionEntry*)are;
    }
    triggerInp->setText(idiomaticExpressionEntry->trigger);
    connect(triggerInp, SIGNAL(textChanged(const QString&)), this, SLOT(checkValidity()));
    for(int i = 0; i < rem->categoryList.size(); i++)
        categoryCb->addItem(rem->categoryList.at(i));
    categoryCb->setCurrentIndex(categoryCb->findText(idiomaticExpressionEntry->category));
    connect(categoryCb, SIGNAL(currentIndexChanged(const int)), this, SLOT(checkValidity()));
    automatonInp->setText(idiomaticExpressionEntry->automaton);
    connect(automatonInp, SIGNAL(textChanged(const QString&)), this, SLOT(checkValidity()));
    lemmaInp->setText(idiomaticExpressionEntry->lemma);
    contextualCb->addItem("");
    contextualCb->addItem("A");
    contextualCb->addItem("D");
    contextualCb->setCurrentIndex(contextualCb->findText(idiomaticExpressionEntry->contextual));
    connect(contextualCb, SIGNAL(currentIndexChanged(const int)), this, SLOT(checkValidity()));
    connect(buttonBox, SIGNAL(accepted()), this, SLOT(submit()));
    checkValidity();
    show();
/*    if (are == 0)
    {
      delete idiomaticExpressionEntry;
      idiomaticExpressionEntry = 0;
    }*/
}

void IdiomaticExpressionEntryEditDlg::checkValidity ()
{
    bool checked = true;
    checked &= !triggerInp->text().isEmpty();
    checked &= !categoryCb->itemText(categoryCb->currentIndex()).isEmpty();
    checked &= !automatonInp->text().isEmpty();
    buttonBox->button(QDialogButtonBox::Ok)->setEnabled(checked);
}

void IdiomaticExpressionEntryEditDlg::submit ()
{
    qDebug() << "IdiomaticExpressionEntryEditDlg::submit" << (void*)idiomaticExpressionEntry;
    idiomaticExpressionEntry->trigger = triggerInp->text();
    idiomaticExpressionEntry->category = categoryCb->itemText(categoryCb->currentIndex());
    idiomaticExpressionEntry->automaton = automatonInp->text();
    idiomaticExpressionEntry->lemma = lemmaInp->text();
    idiomaticExpressionEntry->contextual = contextualCb->itemText(contextualCb->currentIndex());
    idiomaticExpressionEntry->displayable = true;
    Q_EMIT updateEntry(idiomaticExpressionEntry);
}

#include "IdiomaticExpressionEntryEditDlg.moc"
