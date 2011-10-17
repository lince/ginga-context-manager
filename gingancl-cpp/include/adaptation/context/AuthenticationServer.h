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

#ifndef _AUTHENTICATION_SERVER_H_
#define _AUTHENTICATION_SERVER_H_

#include "PresentationContext.h"
#include "contextmanager/ImplicitContextManager.h"
#include "recommender/localagent/LocalAgent.h"

#include <iostream>
#include <string>
#include <vector>
#include <set>
#include <map>
using namespace std;

#include<sys/socket.h>
#include<sys/types.h>
#include<netinet/in.h>
#include<unistd.h>
#include<stdlib.h>
#include<stdio.h>
#include<arpa/inet.h>

using namespace ::br::pucrio::telemidia::ginga::core::contextmanager;
//using namespace ::br::ufscar::lince::ginga::recommender::icm;

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace ncl {
namespace adaptation {
namespace context {

#define P_OK         "NOERR\n"
#define P_NOPWU      "NOPWU\n"
#define P_NOCMD      "NOCMD\n"
#define P_ERROR      "ERROR\n"

class AuthenticationServer {
	private:
		set<string> userfields;
		static const int BufferSize = 1024;
		static const int ResultSize = 6;
		static const int ASPort = 8183;
		PresentationContext *pctx;
		IContextManager* ctxm;
		ImplicitContextManager* ictxm;
		LocalAgent* lagt;

		void tokenize(const std::string& str, 
		              std::vector<std::string>& tokens,
		              const std::string& delimiters = " ")
		{
			std::string::size_type lastPos = str.find_first_not_of(delimiters, 0);
			std::string::size_type pos     = str.find_first_of(delimiters, lastPos);
			while (std::string::npos != pos || std::string::npos != lastPos) {
		        	tokens.push_back(str.substr(lastPos, pos - lastPos));
		                lastPos = str.find_first_not_of(delimiters, pos);
		                pos = str.find_first_of(delimiters, lastPos);
			}
		};
		IGingaUser *getGingaUser(string login, string passw);
		bool setUserProperty(IGingaUser *u, string passwd, string p, string v);
		bool getUserProperty(IGingaUser *u, string passwd, string p, string &v);
		void processClient(int new_socket);
		const char *addUser(int csocket, string passwd, string name, string age, string location, string gender);
		const char *removeUser(int csocket, string userid, string passwd);
		const char *updateUser(int csocket, string userid, string passwd, string property, string value);
		const char *queryUser(int csocket, string userid, string passwd, string property);
		const char *login(int csocket, string userid, string passwd);
		const char *logout(int csocket, string userid, string passwd);
		const char *getCurrentUser(int csocket);
		const char *listUsers(int csocket);
#ifdef HAVE_PROFILEMONITOR
		const char *debug(int csocket);
#endif
		const char *showVar(int csocket, string userid, string passwd, string property);
		const char *updateVar(int csocket, string userid, string passwd, string property, string value);
		void saveUser (int uid);

	public:
		AuthenticationServer(PresentationContext *pctx);
		
		void start();
		~AuthenticationServer();

};
		
		 


}
}
}
}
}
}
}

#endif //_AUTHENTICATION_SERVER_H_
