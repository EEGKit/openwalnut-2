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

#ifndef WEEGCHANNELINFO_H
#define WEEGCHANNELINFO_H

#include <cstddef>

#include <string>

#include <boost/shared_ptr.hpp>

#include "io/WPagerEEG.h"


/**
 * Class which contains information about one channel of an EEG recording, read
 * from a WPagerEEG.
 * \ingroup dataHandler
 */
class WEEGChannelInfo
{
public:
    /**
     * Constructor
     *
     * \param pager pager class which contains the data, read from a file on
     *              demand
     * \param channelID number of this channel
     */
    WEEGChannelInfo( boost::shared_ptr< WPagerEEG > pager, std::size_t channelID );

    /**
     * Get the label of the channel.
     *
     * \return label as string
     */
    std::string getLabel() const;

protected:
private:
    std::string m_label; //!< label of the channel
};

#endif  // WEEGCHANNELINFO_H
