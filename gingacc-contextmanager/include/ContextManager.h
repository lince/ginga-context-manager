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

#ifndef _ContextManager_H_
#define _ContextManager_H_

#include "IContextManager.h"

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace core {
namespace contextmanager {

  class ContextManager : public IContextManager {
	private:
		map<int, IGingaUser*>* users;
		map<int, map<string, string>*>* contexts;
		string usersUri, contextsUri;
		int curUserId;
		ISystemInfo* systemInfo;

		static IContextManager* _instance;
		ContextManager();

	public:
		~ContextManager();
		static IContextManager* getInstance();

	private:
		void initializeUsers();
		void initializeContexts();

	public:
		void addContextVar(int userId, string varName, string varValue);
		void addUser(IGingaUser* newUser);
		void saveUsersAccounts();
		void saveUsersProfiles();

	private:
		void saveProfile(int fd, int userId, map<string, string>* profile);

	public:
		void setCurrentUserId(int userId);
		int getCurrentUserId();
		IGingaUser* getUser(int userId);
		map<string,string>* getUserProfile(int userId);
		map<string, string>* getUsersNames();
		ISystemInfo* getSystemInfo();

		// Marco
		/** 
		 *  Give a list of users ids
		 *  @return vector<int>* pointer for a feature vector which corresponds to the user ids
		 */
		vector<int>* getUserIds();
		
		/** 
		 *  Add user to context.
		 *  @param name string user name
		 *  @param passwd string user passwd
		 *  @param age int user age
		 *  @param location string user location
		 *  @param gender char user gender
		 *  @return int  the next ID of user
		 */
		int addUser(string name, string passwd, int age, string location, char gender);
		/** 
		 *  Remove user from context
		 *  @param userid int user id
		 *  @return bool the user is successfully removed
		 */
		bool removeUser(int userid);

	private:
		/** 
		 *  List users names
		 */
		void listUsersNicks();
  };
}
}
}
}
}
}

#endif /*_ContextManager_H_*/
