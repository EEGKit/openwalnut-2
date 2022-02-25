//---------------------------------------------------------------------------
//
// Project: OpenWalnut ( http://www.openwalnut.org )
//
// Copyright 2009 OpenWalnut Community, BSV@Uni-Leipzig and CNCF@MPI-CBS
// For more information see http://www.openwalnut.org/copying
//
// This file is part of OpenWalnut.
//
// OpenWalnut is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// OpenWalnut is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with OpenWalnut. If not, see <http://www.gnu.org/licenses/>.
//
//---------------------------------------------------------------------------

#ifndef WPROPERTYTRANSFERFUNCTION2DWIDGET_H
#define WPROPERTYTRANSFERFUNCTION2DWIDGET_H

#include <string>

#include <QLineEdit>
#include <QSlider>
#include <QHBoxLayout>

#include "WPropertyWidget.h"

#include "transferFunction2D/WTransferFunction2DWidget.h"
#include "core/common/WTransferFunction2D.h"

/**
 * Implements a property widget for WPropDouble.
 */
class WPropertyTransferFunction2DWidget: public WPropertyWidget, public WTransferFunction2DGuiNotificationClass
{
    Q_OBJECT
public:
    /**
     * Constructor. Creates a new widget appropriate for the specified property.
     *
     * \param property the property to handle
     * \param parent the parent widget.
     * \param propertyGrid the grid used to layout the labels and property widgets
     */
    WPropertyTransferFunction2DWidget( WPropTransferFunction2D property, QGridLayout* propertyGrid, QWidget* parent = 0 );

    /**
     * Destructor.
     */
    virtual ~WPropertyTransferFunction2DWidget();

protected:
    /**
    * Called when the gui has new data
    * \param tf New transfer function
    */
    virtual void guiUpdate( const WTransferFunction2D& tf );

    /**
     * Called whenever the widget should update.
     */
    virtual void update();

    /**
     * The integer property represented by this widget.
     */
    //WPropDouble m_doubleProperty;
    WPropTransferFunction2D m_transferFunctionProperty;

    /**
     * The slider allowing modification of the integer value
     */
    //QSlider m_slider;

    /**
     * The edit field showing the value of the slider
     */
    //QLineEdit m_edit;

    /**
     * Layout used to position the label and the checkbox
     */
    QHBoxLayout m_layout;

    /**
     * The current minimum value.
     */
    double m_min;

    /**
     * The current maximum value.
     */
    double m_max;

    /**
     * Used to show the property as text.
     */
    //QLabel m_asText;

    /**
     * The layout used for the pure output (information properties)
     */
    QHBoxLayout m_infoLayout;

    /** the current transfer function */
    WTransferFunction2DWidget m_transferFunction;

    /** the previously used transfer function */
    WTransferFunction2D lastTransferFunction;

    /**
     * internal synchronization flag
     */
    bool modifying;
};

#endif  // WPROPERTYTRANSFERFUNCTION2DWIDGET_H
