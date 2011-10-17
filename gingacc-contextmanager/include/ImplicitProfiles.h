/******************************************************************************
 Este arquivo eh parte da implementacao do ambiente declarativo do middleware
 Ginga (Ginga-NCL).

 Copyright (C) 2009 UFSCar/Lince, Todos os Direitos Reservados.

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
 lince@dc.ufscar.br
 http://www.ncl.org.br
 http://www.ginga.org.br
 http://lince.dc.ufscar.br
 ******************************************************************************
 This file is part of the declarative environment of middleware Ginga (Ginga-NCL)

 Copyright (C) 2009 UFSCar/Lince, Todos os Direitos Reservados.

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
 lince@dc.ufscar.br
 http://www.ncl.org.br
 http://www.ginga.org.br
 http://lince.dc.ufscar.br
 *******************************************************************************/

#ifndef _IMPLICIT_PROFILES_H_
#define _IMPLICIT_PROFILES_H_

#include <string>
#include "config.h"


#include "recommender/utils/Utils.h"
#include <../../../gingancl-cpp/include/adaptation/context/RuleAdapter.h>
#include "recommender/miningalgorithm/ikmeans.h"

/*#ifdef HAVE_RECOMMENDER
#include "recommender/utils/Utils.h"
#include <../../../gingancl-cpp/include/adaptation/context/RuleAdapter.h>
#include "recommender/miningalgorithm/ikmeans.h"
#else
#include "../../Recommender/Utils/include/Utils.h"
#include <../../../gingancl-cpp/include/adaptation/context/RuleAdapter.h>
#include "../../Recommender/MiningAlgorithm/include/ikmeans/ikmeans.h"
//#include "utils.h"  // <=--------- DEBUG!!!
//#include <RuleAdapter.h>  // <=--------- DEBUG!!!
#endif*/
using namespace br::pucrio::telemidia::ginga::ncl::adaptation::context;
using namespace ::br::ufscar::lince::ginga::recommender::util;

#ifdef HAVE_LOG4CXX
#include <log4cxx/logger.h>
using namespace log4cxx;
#endif

#include <stdio.h>
#include <sstream>

using namespace std;

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace core {
namespace contextmanager {

#define IPROFVAR    		"default.iprof"
#define CURIPROFVAR 		"default.curiprof"

#define IPROFVAR_SIZE    	13
#define CURIPROFVAR_SIZE 	16

// A typical family has a couple plus three children. 
// If we consider shared watching, we might get some 
// additional shared profiles. Thus, 15 profiles seems 
// to be a reasonable guess.
#define MAX_PROFILES_PER_TVSET	15  

/**! Profile type: timestamp and feature vector. */
typedef pair<time_t, vector<int>*> tIProfileObject;

/**! Profile map type: map for profiles of a user. */
typedef map<int, tIProfileObject> tIProfileMap;

/**! Profiles type: all the users and their profiles.. */
typedef map<int, tIProfileMap*> tIProfiles;

/** Class ImplicitProfiles
 * Implicit user profiles 
 * @author Marco Cristo & Angelo Bittar
 */
class ImplicitProfiles {

private:

	/**! Pointer to Ginga presentation context. Allows for access to live context variables. */
	PresentationContext *pctx;

	/**! Implicit profiles. */
	tIProfiles* iprofiles; // implicit profiles

	/**! Next pid to be used for each user. */
	map<int, int> *nextpid; // next pid to be used for each user

	/**! Current profile for each user */
	map<int, int> *curProfile; // current profile for each user

	/**! Size (number of features) of the profile vector. */
	int profileSize;

	/**! Maximum time the profile stays in database without being used. */
	// static const int MAX_PROFILE_UNUSED_TIME = 45; // 45 days
	static const int MAX_PROFILE_UNUSED_TIME = 365; // 45 days

	/**! Map data to axis in internal vector representation */
	map<string, int> s2i;

	/** 
	 *  Delete profiles that have not been updated for a long time
	 */
	void deleteOld();

	/** 
	 *  Merge very similar pofiles, if number of profiles is greater than a certain threshold
	 */
	void cluster();

	/** 
	 *  Set the value of an implicit profile property for a given user id, that is, property[userid] = value.
	 *  @param userid integer user id
	 *  @param property the string property to be set
	 *  @param value the string value to be instantiated
	 */
	void setValue(int userid, string property, string value);

	/** 
	 *  Delete an implicit profile property for a given user id.
	 *  @param userid the user id
	 *  @param property the string property to be deleted
	 */
	void delVar(int userid, string property);

public:
#ifdef HAVE_PROFILEMONITOR
	//Codigo de depuração - Ângelo Bitar
	void setValues (int userid, string property, string value){
		setValue(userid, property, value);
	};
	void delVars (int userid, string property){
		delVar(userid, property);
	};
#endif
	
	/** 
	 * Get instance of (explicit) presentation context and corresponding context manager.
	 * Load users and profiles and create new implicit profiles for users that do not have one.
	 * Set current profile for each user and current implicit user profile.
	 * Clean up profile base by eliminating old profiles and clustering similar profiles.
	 *  @param pcxt pointer to Presentation Context
	 */
	ImplicitProfiles(PresentationContext *pctx);

	/** 
	 *  Destructor for implicit profiles
	 */
	~ImplicitProfiles();

	/** 
	 * Create a new implicit profile 
	 * @param uid user id
	 */
	void createProfile(int uid);

	/** 
	 * Returns current profile id for giver user id
	 * @param userid user id
	 * @return int current profile id
	 */
	int getCurrentId(int userid);

	/** 
	 * Returns true if current implicit user id was set
	 * @param userid user id
	 * @return bool value indicating if opeation was performed
	 */
	bool setCurrentImplicitUserId(int userid);

	/** 
	 * Returns true if current profile id was set
	 * @param userid user id
	 * @param profid profile id
	 * @return bool value indicating if opeation was performed
	 */
	bool setCurrentId(int userid, int profid);

	/** 
	 * Returns map profiles which stores the profiles of all users
	 * @return map<int, map<int, pair<time_t, vector<int>*> >*>* pointer for all profiles
	 */
	tIProfiles* get();

	/** 
	 * Returns a profile vector of a user
	 * @param userid user id
	 * @param profid profile id
	 * @return vector<int>* pointer for a feature vector which corresponds to the user profile
	 */
	vector<int>* get(int userid, int profid);

	/** 
	 * Find profile age in days
	 * @param userid user id
	 * @param profid profile id
	 * @return int profile age (note: if profile has 4.7 days, function will return 4 days)
	 */
	int findLastUpdateInDays(int userid, int profid);

	/** 
	 * Calculate profile age in days given a certain unix timestamp t
	 * @param t timestamp
	 * @return int profile age (note: if profile has 4.7 days, function will return 4 days)
	 */
	int getLastUpdateInDays(int t);

	/** 
	 * Returns true if profile profid of user userid given by vector v was inserted
	 * @param userid user id
	 * @param *v pointer to profile vector
	 * @param profid profile id given for inserted profile
	 * @return bool value indicating if opeation was performed
	 */
	bool insert(int userid, vector<int> *v, int &profid);
	
	/** 
	 * Returns true if profile profid of user userid was deleted
	 * @param userid user id
	 * @param profid profile id of deleted profile
	 * @return bool value indicating if opeation was performed
	 */
	bool remove(int userid, int profid);

	/** 
	 * Returns true if profile profid of user userid was modified
	 * @param userid user id
	 * @param profid profile id of modified profile
	 * @return bool value indicating if opeation was performed
	 */
	bool modify(int userid, int profid);

	/** 
	 *  Show implicit profiles -- debug assistance.
	 */
	void show();

	/** 
	 * Indicating the position of subgenre in the vector
	 *  Get axis value associated with input data.
	 *  @param s name of the subGenre.
	 *  @return int value indicating the position of subgenre in the vector.
	 */
	int getAxis(string s);

	/**
	 * Return the size that profiles have.
	 *  Get profile size.
	 *  @return int the size of profiles.
	 */
	int getSize(void) {
		return profileSize;
	}
	;
};

}
}
}
}
}
}

#endif /*_IMPLICIT_PROFILES_H_*/
