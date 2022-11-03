// Copyright 2002-2013 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

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
    connect(triggerInp, SIGNAL(textChanged(QString&)), this, SLOT(checkValidity()));
    for(int i = 0; i < rem->categoryList.size(); i++)
        categoryCb->addItem(rem->categoryList.at(i));
    categoryCb->setCurrentIndex(categoryCb->findText(idiomaticExpressionEntry->category));
    connect(categoryCb, SIGNAL(currentIndexChanged(int)), this, SLOT(checkValidity()));
    automatonInp->setText(idiomaticExpressionEntry->automaton);
    connect(automatonInp, SIGNAL(textChanged(QString&)), this, SLOT(checkValidity()));
    lemmaInp->setText(idiomaticExpressionEntry->lemma);
    contextualCb->addItem("");
    contextualCb->addItem("A");
    contextualCb->addItem("D");
    contextualCb->setCurrentIndex(contextualCb->findText(idiomaticExpressionEntry->contextual));
    connect(contextualCb, SIGNAL(currentIndexChanged(int)), this, SLOT(checkValidity()));
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
