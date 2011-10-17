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

#ifndef _PRESENTATIONCONTEXT_H_
#define _PRESENTATIONCONTEXT_H_

#include "util/Observable.h"

#include "util/functions.h"
using namespace ::br::pucrio::telemidia::util;

#include "contextmanager/IContextManager.h"
//#include "contextmanager/system/ISystemInfo.h"
using namespace ::br::pucrio::telemidia::ginga::core::contextmanager;

#include "ContextBase.h"

#include "math.h"

//#if HAVE_WAC 
#include "wac/state/IContextProvider.h"
using namespace ::br::ufscar::lince::ginga::wac::state;
//#end if

/*#if HAVE_RECOMMENDER
#include "recommender/Scheduler.h"
using namespace br::ufscar::lince::ginga::recommender;
#endif*/


#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>
using namespace std;

// marco: support to socket
#include<sys/socket.h>
#include<sys/types.h>
#include<netinet/in.h>
#include<unistd.h>
#include<stdlib.h>
#include<stdio.h>
#include<arpa/inet.h>

#define BUFSIZE 1024

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace ncl {
namespace adaptation {
namespace context {

        void *runAuthenticationServer(void * p ); // marco

	class PresentationContext : public ContextBase, public Observable 
	//#if HAVE_WAC
		, public IContextProvider
	//#endif

	{
		private:
			static PresentationContext* _instance;
			map<string, string>* contextTable;
			IContextManager* contextManager;
			pthread_mutex_t attrMutex;

			PresentationContext();
			~PresentationContext();

		public:

			static bool instantiated(); // Marco
			static PresentationContext* getInstance();
			void setPropertyValue(string propertyName, string value);
			void incPropertyValue(string propertyName);
			void decPropertyValue(string propertyName);
			vector<string>* getPropertyNames();
			string getPropertyValue(string attributeId);
			IContextManager* getContextManager(); //marco
			void lock() {pthread_mutex_lock(&attrMutex);cout<<"++++lock"<<endl;}; //marco
			void unlock() {pthread_mutex_unlock(&attrMutex);cout<<"----unlock"<<endl;}; //marco
			void initializeContext() { // marco
				initializeUserContext();
				initializeSystemValues();
			};

		private:
			void initializeUserContext(); 
			void initializeUserInfo(int currentUserId);
			void initializeSystemValues();
            pthread_t aserver; // marco
			volatile bool stop; // marco

		public:
			bool stopServer(void) {return stop;}; // marco
			void save();
	};
}
}
}
}
}
}
}

#endif //_PRESENTATIONCONTEXT_H_
