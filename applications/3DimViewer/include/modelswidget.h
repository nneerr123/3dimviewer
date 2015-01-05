///////////////////////////////////////////////////////////////////////////////
// $Id$
//
// 3DimViewer
// Lightweight 3D DICOM viewer.
//
// Copyright 2008-2012 3Dim Laboratory s.r.o.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//   http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef modelswidget_H_included
#define modelswidget_H_included

#include <QWidget>

#include <data/CModel.h>
#include <data/CObjectObserver.h>
#include <QTableWidgetItem>

namespace Ui
{
    class ModelsWidget;
}

class CModelsWidget : public QWidget, public data::CObjectObserver<data::CModel>
{
    Q_OBJECT

    enum EColumns
    {
        COL_NAME = 0,
        COL_VISIBLE,
        COL_COLOR,
        COL_DELETE,
        COL_COUNT
    };

public:
    //! Constructor
    explicit CModelsWidget(QWidget *parent = 0);
    //! Destructor
    ~CModelsWidget();

    //! Called on volume data change.
    void onNewDensityData(data::CStorageEntry *pEntry);

    //! Return storage id of the selected model, -1 if no model is selected
    int getSelectedModelStorageId();

private:
    //! Method called on model change
    void objectChanged(data::CModel *pData);

    //! Update table
    void updateTable();

private slots:
    //! On model color button clicked
    void onModelColorButton();

    //! On model remove button clicked
    void onModelRemoveButton();

    //! On model item changed
    void onModelItemChanged(QTableWidgetItem *item);

private:
    // GUI
    Ui::ModelsWidget *ui;
    //! Signal connection for volume data change
    vpl::mod::tSignalConnection m_Connection;
    //! Was this my change?
    bool m_bMyChange;
};

// modelswidget_H_included
#endif