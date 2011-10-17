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

#include "../include/ContextManager.h"

#include "../include/user/GingaUser.h"
#include "../include/system/SystemInfo.h"

#include "util/functions.h"
using namespace ::br::pucrio::telemidia::util;

#include <sys/types.h>
#include <fcntl.h>

#include <iostream>
#include <fstream>
using namespace std;

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace core {
namespace contextmanager {
	ContextManager::ContextManager() {
		usersUri    = "/usr/local/etc/ginga/files/contextmanager/users.ini";
		contextsUri = "/usr/local/etc/ginga/files/contextmanager/contexts.ini";
		users       = new map<int, IGingaUser*>;
		contexts    = new map<int, map<string, string>*>;
		curUserId   = -1;
		systemInfo  = new SystemInfo();

		initializeUsers();
		initializeContexts();

		systemInfo->setSystemTable(getUserProfile(getCurrentUserId()));
	}

	ContextManager::~ContextManager() {
		map<int, IGingaUser*>::iterator i;

		i = users->begin();
		while (i != users->end()) {
			delete i->second;
			++i;
		}
	}

	IContextManager* ContextManager::_instance = NULL;

	IContextManager* ContextManager::getInstance() {
		if (_instance == NULL) {
			_instance = new ContextManager();
		}

		return _instance;
	}

	void ContextManager::initializeUsers() {
		IGingaUser* newUser;
		ifstream fis;
		string line = "", name = "", location = "", passwd = "";
		int id = -1, age = -1;
		char genre = 'a';
		bool validUser = false;
		bool invalidUser = false;

		fis.open(usersUri.c_str(), ifstream::in);

		if (!fis.is_open()) {
			cout << "ContextManager::initializeUsers() Warning: can't open ";
			cout << "config file '" << usersUri << "'" << endl;
			return;
		}

		while (fis.good()) {
			if (line == "::") {
				fis >> line;
				if (line == "=") {
					fis >> line;
					curUserId = stof(line);
				} else {
					invalidUser = true;
					cout << "ContextManager::initializeUsers ";
					cout << "warning! currentUser token must to be ':: ='";
					cout << endl;
				}
			}

			if (line != "||") {
				fis >> line;

			} else {
				fis >> line;
				if (line == "=") {
					fis >> line;
					id = (int)stof(line);
					if (id >= 0) {
						fis >> line;
						name = line;
						if (name != "") {
							fis >> line;
							passwd = line;
							if (passwd != "") {
								fis >> line;
								age = (int)stof(line);
								if (age >= 0) {
									fis >> line;
									location = line;
									if (location != "") {
										fis >> line;
										if (line == "m" ||
												line == "f") {

											genre = line[0];
											validUser = true;
											invalidUser = false;

										} else {
											invalidUser = true;
											cout << "ContextManager::";
											cout << "initializeUsers ";
											cout << "warning! genre != m and f";
											cout << endl;
										}

									} else {
										invalidUser = true;
										cout << "ContextManager::";
										cout << "initializeUsers ";
										cout << "warning! NULL location";
										cout << endl;
									}

								} else {
									invalidUser = true;
									cout << "ContextManager::initializeUsers ";
									cout << "warning! age < 0" << endl;
								}

							} else {
								invalidUser = true;
								cout << "ContextManager::initializeUsers ";
								cout << "warning! NULL passwd." << endl;
							}

						} else {
							invalidUser = true;
							cout << "ContextManager::initializeUsers warning! ";
							cout << "name == ''" << endl;
						}

					} else {
						invalidUser = true;
						cout << "ContextManager::initializeUsers warning! ";
						cout << "token < 0" << endl;
					}

				} else {
					invalidUser = true;
					cout << "ContextManager::initializeUsers warning! ";
					cout << "token != '='" << endl;
				}
			}

			if (validUser) {
				validUser = false;
				newUser = new GingaUser(id, name, passwd);
				newUser->setUserAge(passwd, age);
				newUser->setUserLocation(passwd, location);
				newUser->setUserGenre(passwd, genre);

				addUser(newUser);

			} else if (invalidUser) {
				cout << "ContextManager::initializeUsers warning! Invalid ";
				cout << "user '" << curUserId << "'" << endl;
			}
		}

		fis.close();
	}

	int ContextManager::addUser(string name, string passwd, int age, string location, char gender) {
		int nextuid = 1;  // note userid 0 is reserved for default user
		vector<int>* uids = getUserIds();
		if (uids->size() > 0)
			nextuid = (*uids)[uids->size() - 1] + 1;
		delete uids;

		IGingaUser* newUser;
		newUser = new GingaUser(nextuid, name, passwd);
		newUser->setUserAge(passwd, age);
		newUser->setUserLocation(passwd, location);
		newUser->setUserGenre(passwd, gender);

		addUser(newUser);

		return nextuid;
	};

	void ContextManager::initializeContexts() {
		ifstream fis;
		string line = "", key = "", value = "";
		int id = -1;

		fis.open(contextsUri.c_str(), ifstream::in);

		if (!fis.is_open()) {
			cout << "ContextManager::initializeContexts() Warning: can't open ";
			cout << "cfg file '" << contextsUri << "'" << endl;
			return;
		}

		while (fis.good()) {
			if (line == "::") {
				fis >> line;
				if (line == "=") {
					fis >> line;
					curUserId = stof(line);
				}
			}

			if (line != "||") {
				fis >> line;

			} else {
				fis >> line;
				if (line == "=") {
					fis >> line;
					id = (int)stof(line);
					if (id >= 0) {
						while (fis.good()) {
							fis >> line;
							if (line == "||") {
								break;
							}

							key = line;
							fis >> line;
							if (line == "=") {
								fis >> line;
								value = line;
								addContextVar(id, key, value);
								// debug
								//cout << "ContextManager::initializeContexts: " << key << "=" << value << endl;
							}
						}
					}
				}
			}
		}

		fis.close();
	}

	void ContextManager::addContextVar(
			int userId, string varName, string varValue) {

		map<int, map<string, string>*>::iterator i;
		map<string, string>* vars;

		i = contexts->find(userId);
		if (i != contexts->end()) {
			vars = i->second;

		} else {
			vars = new map<string, string>;
			(*contexts)[userId] = vars;
		}

		(*vars)[varName] = varValue;
	}

	void ContextManager::addUser(IGingaUser* newUser) {
		int id;

		id = newUser->getUserId();
		if (users->count(id) != 0) {
			cout << "ContextManager::addUser Warning! ";
			cout << "Trying to add the same user twice (id = '";
			cout << id << "'" << endl;
			return;
		}

		(*users)[id] = newUser;
	}

	void ContextManager::saveUsersAccounts() {
		int fd;
		map<int, IGingaUser*>::iterator i;
		remove(usersUri.c_str());
		fd = open(usersUri.c_str(), O_CREAT | O_LARGEFILE | O_WRONLY);

		if (fd <= 0) {
			return;
		}

		GingaUser::saveString(fd, ":: =");
		GingaUser::saveString(fd, itos(curUserId));
		GingaUser::saveString(fd, "\n");

		i = users->begin();
		while (i != users->end()) {
			cout << "saving... " << i->first << "=" << i->second << endl;
			i->second->saveTo(fd);
			GingaUser::saveString(fd, "\n");
			++i;
		}

		close(fd);
	}

	void ContextManager::saveUsersProfiles() {
		int fd;
		map<int, map<string, string>*>::iterator i;

		remove(contextsUri.c_str());
		fd = open(contextsUri.c_str(), O_CREAT | O_LARGEFILE | O_WRONLY);

		if (fd <= 0) {
			return;
		}

		GingaUser::saveString(fd, ":: =");
		GingaUser::saveString(fd, itos(curUserId));
		GingaUser::saveString(fd, "\n");

		i = contexts->begin();
		while (i != contexts->end()) {
			saveProfile(fd, i->first, i->second);
			++i;
		}

		close(fd);
	}

	void ContextManager::saveProfile(
			int fd, int userId, map<string, string>* profile) {

		map<string, string>::iterator i;

		GingaUser::saveString(fd, "|| =");
		GingaUser::saveString(fd, itos(userId));
		GingaUser::saveString(fd, "\n");

		i = profile->begin();
		while (i != profile->end()) {
			// marco -- debug
			cout << "saving... " << i->first << "=" << i->second << endl;
			GingaUser::saveString(fd, i->first);
			GingaUser::saveString(fd, "=");
			GingaUser::saveString(fd, i->second);
			GingaUser::saveString(fd, "\n");
			++i;
		}
	}

	void ContextManager::setCurrentUserId(int userId) {
		cout << "+++ curUserid = " << userId << " " << users->count(userId) << endl;
		if (users->count(userId) != 0) {
			curUserId = userId;
		}
	}

	int ContextManager::getCurrentUserId() {
		return curUserId;
	}

	bool ContextManager::removeUser(int userId) {
		// assume that user 0 is "default" and 
		// cannot be deleted 
		if (userId == 0)
			return false;
		map<int, IGingaUser*>::iterator i;
		i = users->find(userId);
		if (i != users->end()) 
			users->erase(i);
		else {
			cout << "ContextManager::removeUser Warning! can't find user '";
			cout << userId << "'" << endl;
			return false;
		}

		// note that user may have no context variables
		map<int, map<string, string>*>::iterator i2;
		i2 = contexts->find(userId);
		if (i2 != contexts->end()) 
			contexts->erase(i2);

		if (userId == curUserId) {
			if (users->size() == 0)
				curUserId = -1;  // this should not be possible since user 0 is never deleted!
			else {
				//i = users->begin();
				curUserId = 0;   // default user is the current user
			}
		}
		return true;
	}

	IGingaUser* ContextManager::getUser(int userId) {
		map<int, IGingaUser*>::iterator i;

		i = users->find(userId);
		if (i != users->end()) {
			return i->second;
		}

		cout << "ContextManager::getUser Warning! can't find user '";
		cout << userId << "'" << endl;
		return NULL;
	}

	map<string,string>* ContextManager::getUserProfile(int userId) {
		map<int, map<string, string>*>::iterator i;

		i = contexts->find(userId);
		if (i != contexts->end()) {
			return i->second;
		}

		cout << "ContextManager::getUserProfile Warning! can't find profile ";
		cout << "of user '" << userId << "'" << endl;
		return NULL;
	}

	map<string, string>* ContextManager::getUsersNames() {
		map<string, string>* names;
		map<int, IGingaUser*>::iterator i;

		names = new map<string, string>;
		i = users->begin();
		while (i != users->end()) {
			(*names)[i->second->getUserName()] = "";
			++i;
		}

		if (names->empty()) {
			delete names;
			names = NULL;
		}

		return names;
	}

	ISystemInfo* ContextManager::getSystemInfo() {
		return systemInfo;
	}

	void ContextManager::listUsersNicks() {
		map<int, IGingaUser*>::iterator i;

		cout << "ContextManager::listUsersNicks '";
		i = users->begin();
		while (i != users->end()) {
			cout << i->second->getUserName() << "' ";
			++i;
		}
		cout << endl;
	}

	// Marco
	vector<int>* ContextManager::getUserIds() {
		vector<int>* vusers = new vector<int>;
		map<int, IGingaUser*>::iterator i;

		i = users->begin();
		while (i != users->end()) {
			vusers->push_back(i->first);
			++i;
		}

		if (vusers->empty()) {
			delete vusers;
			vusers = NULL;
		}

		return vusers;
	}

}
}
}
}
}
}

extern "C" ::br::pucrio::telemidia::ginga::core::contextmanager::
		IContextManager* createContextManager() {

	return ::br::pucrio::telemidia::ginga::core::contextmanager::
			ContextManager::getInstance();
}

extern "C" void destroyContextManager(
		::br::pucrio::telemidia::ginga::core::contextmanager::IContextManager*
		cm) {

	delete cm;
}
