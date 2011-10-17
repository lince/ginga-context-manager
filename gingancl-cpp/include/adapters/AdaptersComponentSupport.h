/******************************************************************************
Este arquivo eh parte da implementacao do ambiente declarativo do middleware
Ginga (Ginga-NCL).

Direitos Autorais Reservados (c) 1989-2007 PUC-Rio/Laboratorio TeleMidia

Este programa eh software livre; voce pode redistribui-lo e/ou modificah-lo sob
os termos da Licenca Publica Geral GNU versao 2 conforme publicada pela Free
Software Foundation.

Este programa eh distribuido na expectativa de que seja util, porem, SEM
NENHUMA GARANTIA; nem mesmo a garantia implicita de COMERCIABILIDADE OU
ADEQUACAO A UMA FINALIDADE ESPECIFICA. Consulte a Licenca Publica Geral do
GNU versao 2 para mais detalhes.

Voce deve ter recebido uma copia da Licenca Publica Geral do GNU versao 2 junto
com este programa; se nao, escreva para a Free Software Foundation, Inc., no
endereco 59 Temple Street, Suite 330, Boston, MA 02111-1307 USA.

Para maiores informacoes:
ncl @ telemidia.puc-rio.br
http://www.ncl.org.br
http://www.ginga.org.br
http://www.telemidia.puc-rio.br
******************************************************************************
This file is part of the declarative environment of middleware Ginga (Ginga-NCL)

Copyright: 1989-2007 PUC-RIO/LABORATORIO TELEMIDIA, All Rights Reserved.

This program is free software; you can redistribute it and/or modify it under
the terms of the GNU General Public License version 2 as published by
the Free Software Foundation.

This program is distributed in the hope that it will be useful, but WITHOUT ANY
WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
PARTICULAR PURPOSE.  See the GNU General Public License version 2 for more
details.

You should have received a copy of the GNU General Public License version 2
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA

For further information contact:
ncl @ telemidia.puc-rio.br
http://www.ncl.org.br
http://www.ginga.org.br
http://www.telemidia.puc-rio.br
*******************************************************************************/

#ifndef ADAPTERSCOMPONENTSUPPORT_H_
#define ADAPTERSCOMPONENTSUPPORT_H_

#include "../../config.h"

#if HAVE_COMPSUPPORT
#include "ginga/cm/IComponentManager.h"
using namespace ::br::pucrio::telemidia::ginga::core::cm;
#else
#include "../../../gingacc-system/include/io/InputManager.h"
#include "../../../gingacc-system/include/io/interface/output/dfb/DFBSurface.h"

#include "../../../gingacc-player/include/ProgramAV.h"
#include "../../../gingacc-player/include/ChannelPlayer.h"
#include "../../../gingacc-player/include/AVPlayer.h"
#include "../../../gingacc-player/include/LinksPlayer.h"
#include "../../../gingacc-player/include/LuaPlayer.h"
#include "../../../gingacc-player/include/ImagePlayer.h"
#include "../../../gingacc-player/include/PlainTxtPlayer.h"
#include "../../../gingacc-player/include/SrtPlayer.h"

#include "av/AVPlayerAdapter.h"
#include "av/ChannelPlayerAdapter.h"
using namespace ::br::pucrio::telemidia::ginga::ncl::adapters::av;

#include "av/tv/ProgramAVPlayerAdapter.h"
using namespace ::br::pucrio::telemidia::ginga::ncl::adapters::av::tv;

#include "image/ImagePlayerAdapter.h"
using namespace ::br::pucrio::telemidia::ginga::ncl::adapters::image;

#include "text/PlainTxtPlayerAdapter.h"
#include "text/SubtitlePlayerAdapter.h"
using namespace ::br::pucrio::telemidia::ginga::ncl::adapters::text;

#include "INclEditListener.h"
using namespace ::br::pucrio::telemidia::ginga::ncl::adapters;

#include "application/imperative/lua/LuaPlayerAdapter.h"
using namespace ::br::pucrio::telemidia::ginga::ncl::adapters::application::lua;

#include "application/declarative/xhtml/LinksPlayerAdapter.h"
using namespace br::pucrio::telemidia::ginga::ncl::adapters::application::xhtml;

#include "application/declarative/ncl/NCLPlayerAdapter.h"
using namespace ::br::pucrio::telemidia::ginga::ncl::adapters::application::ncl;

#include "FormatterPlayerAdapter.h"

#include "../Formatter.h"
using namespace ::br::pucrio::telemidia::ginga::ncl;
#endif

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace ncl {
namespace adapters {
#if HAVE_COMPSUPPORT
	static IComponentManager* cm = IComponentManager::getCMInstance();
#endif
}
}
}
}
}
}

#endif /* ADAPTERSCOMPONENTSUPPORT_H_ */
