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

#ifndef WQT4GUI_H
#define WQT4GUI_H

#include <string>
#include <vector>

#include <boost/program_options.hpp>

#include "../WGUI.h"

class WMainWindow;
class WGraphicsEngine;
class WKernel;

/**
 * The QT4 Based GUI implementation. Implements WGUI.
 * \ingroup gui
 */
class WQt4Gui : public WGUI
{
public:

    /**
     * Constructor.
     *
     * \param argc number of arguments given on command line.
     * \param argv arguments given on command line.
     */
    WQt4Gui( int argc, char** argv );

    /**
     * Default destructor.
     */
    virtual ~WQt4Gui();

    /**
     * Runs the GUI. All initialization should be done here.
     *
     * \return the return code.
     */
    virtual int run();

    /**
     * adds a dataset to the dataset browser
     *
     * \param module
     * \param subjectId
     */
    void addDatasetToBrowser( boost::shared_ptr< WModule > module, int subjectId );

    /**
     * adds a module to the dataset browser
     *
     * \param module
     */
    void addModuleToBrowser( boost::shared_ptr< WModule > module );

    /**
     * returns a vector of pointers to the loaded datasets for a given subject.
     *
     * \param subjectId the ID of the subject to get the list for.
     * \param onlyTextures true if only textures should be returned.
     *
     * \return list of datasets.
     */
    virtual std::vector< boost::shared_ptr< WDataSet > > getDataSetList( int subjectId, bool onlyTextures = false );

    /**
     * getter functions for the signales proved by the gui
     */
    boost::signal1< void, std::vector< std::string > >* getLoadButtonSignal();

    /**
     * this function allows modules to register their property object with the gui
     */
    virtual void connectProperties( boost::shared_ptr<WProperties> properties );

protected:

private:

    /**
     * Main window containing all needed widgets.
     */
    WMainWindow* m_gui;

    /**
     * Graphics Engine instance.
     */
    boost::shared_ptr< WGraphicsEngine > m_ge;

    /**
     * Kernel instance.
     */
    boost::shared_ptr< WKernel > m_kernel;

    boost::program_options::variables_map m_optionsMap; //!< Map storing the program options.
    boost::program_options::variables_map m_guiConfiguration; //!< Map storing the configuration of the GUI

    /**
     * This function defines and parses the valid command line options.
     * This might once be put in a separate class like WOptionHandler.
     * At the moment it seems reasonable that different GUIs might have
     * different command line options, thus we implement their parsing
     * in the GUI implemntation itself, i.e. here.
     */
    bool parseOptions( int argc, char** argv );
};

#endif  // WQT4GUI_H

