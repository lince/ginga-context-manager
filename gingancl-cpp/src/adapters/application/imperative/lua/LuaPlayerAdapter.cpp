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

#include "../../../../../include/adapters/application/imperative/lua/LuaPlayerAdapter.h"

#include "../../../../../include/adapters/AdaptersComponentSupport.h"

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace ncl {
namespace adapters {
namespace application {
namespace lua {
	LuaPlayerAdapter::LuaPlayerAdapter(IPlayerAdapterManager* manager) :
		ApplicationPlayerAdapter(manager)  {

	}

	void LuaPlayerAdapter::createPlayer() {
		if (fileExists(mrl)) {
#if HAVE_COMPSUPPORT
			playerCompName = "LuaPlayer";
			player = ((PlayerCreator*)(cm->getObject(playerCompName)))(
					mrl.c_str(), true);
#else
			player = new LuaPlayer(mrl.c_str());
#endif

		} else {
			player = NULL;
			cout << "LuaPlayerAdapter::createPlayer Warning! ";
			cout << "file not found: '" << mrl.c_str() << "'" << endl;
		}
		FormatterPlayerAdapter::createPlayer();
	}

	void LuaPlayerAdapter::setCurrentEvent(FormatterEvent* event) {
		string interfaceId;

		lockEvent();
		if (event == NULL) {
			unlockEvent();
			return;
		}

		if (preparedEvents->count(event->getId()) != 0 &&
				!event->instanceOf("SelectionEvent") &&
				event->instanceOf("AnchorEvent")) {

			interfaceId = ((AnchorEvent*)event)->getAnchor()->getId();

			if ((((AnchorEvent*)event)->getAnchor())->instanceOf(
					"LabeledAnchor")) {

				interfaceId = ((LabeledAnchor*)((AnchorEvent*)event)->
						getAnchor())->getLabel();

			} else if ((((AnchorEvent*)event)->getAnchor())->instanceOf(
					"LambdaAnchor")) {

				interfaceId = "";
			}

			currentEvent = event;
			((ApplicationExecutionObject*)object)->setCurrentEvent(
					currentEvent);

			player->setCurrentScope(interfaceId);

		} else if (event->instanceOf("AttributionEvent")) {
			interfaceId = ((AttributionEvent*)
					event)->getAnchor()->getPropertyName();

			player->setScope(interfaceId, IPlayer::TYPE_ATTRIBUTION);

			currentEvent = event;
			((ApplicationExecutionObject*)object)->setCurrentEvent(
					currentEvent);

			player->setCurrentScope(interfaceId);
		}
		unlockEvent();
	}

	void LuaPlayerAdapter::pushEPGEvent(map<string, string> t) {
		//((LuaPlayer*)player)->pushEPGEvent(t);
	}
}
}
}
}
}
}
}
}

extern "C" ::br::pucrio::telemidia::ginga::ncl::adapters::IPlayerAdapter*
		createLuaAdapter(IPlayerAdapterManager* manager, void* param) {

	return new ::br::pucrio::telemidia::ginga::ncl::adapters::application::lua::LuaPlayerAdapter(manager);
}

extern "C" void destroyLuaAdapter(
		::br::pucrio::telemidia::ginga::ncl::adapters::IPlayerAdapter* player) {

	return delete player;
}
