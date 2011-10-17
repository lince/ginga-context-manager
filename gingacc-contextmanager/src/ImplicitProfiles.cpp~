/* Copyright (C) Marco Cristo & Angelo Filipe 2010
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#ifdef HAVE_RECOMMENDER
#include "recommender/ImplicitProfiles.h"
#else
#include "ImplicitProfiles.h"
#endif

//using namespace br::ufscar::lince::ginga::recommender::localagent;
using namespace br::ufscar::lince::ginga::recommender::icm;

namespace br {
namespace ufscar {
namespace lince {
namespace ginga {
namespace recommender {
namespace icm {

// constructor for ImplicitContext
ImplicitProfiles::ImplicitProfiles(PresentationContext *pctx) {
	// cout << "+++ calling ImplicitProfiles()" << endl;
	map<string, string> *up;
	map<string, string>::iterator it;
	int i, j, pid;
	profileSize = 11;
	time_t utime;

	iprofiles = new tIProfiles;
	curProfile = new map<int, int> ;
	nextpid = new map<int, int> ;

	// get instance of (explicit) presentation context
	// and corresponding context manager
	this->pctx = pctx;
	IContextManager *ctxm = pctx->getContextManager();

	// load users and profiles
	// cout << "+++ ImplicitProfiles() -- loading profiles" << endl;
	vector<int>* vusers = ctxm->getUserIds(); // get user ids
	for (i = 0; i < vusers->size(); i++) {

		// cout << "+++ ImplicitProfiles() -- loading profile for user " << (*vusers)[i] << endl;
		// for each user, get its profiles
		up = ctxm->getUserProfile((*vusers)[i]);
		tIProfileMap* mprofiles = new tIProfileMap;

		// set current profile for user
		// cout << "+++ ImplicitProfiles() -- set curProfile " << (*vusers)[i] << endl;
		(*curProfile)[(*vusers)[i]] = 0;

		// cout << "+++ ImplicitProfiles() -- it begin" << endl;
		it = up->begin();
		int maxpid = 0;
		while (it != up->end()) {
			//cout << "+++ ImplicitProfiles() -- " << it->first << endl;
			if (it->first.substr(0, CURIPROFVAR_SIZE) == CURIPROFVAR) {
				vector<string> tks;
				Utilitario::Tokenize(it->second, tks, ":");
				//cout << "+++ ImplicitProfiles() -- tokenized! " << tks[1] << endl;
				(*curProfile)[(*vusers)[i]] = Utilitario::convertStringToInt(tks[1]);

			} else if (it->first.substr(0, IPROFVAR_SIZE) == IPROFVAR) {
				// get prof id
				pid = Utilitario::convertStringToInt(it->first.substr(IPROFVAR_SIZE));
				//cout << "+++ ImplicitProfiles() -- loading profile " << pid << " for user " << (*vusers)[i] << endl;
				// get prof-value and convert it into float vector
				vector<int> *pfs = new vector<int> ;
				vector<string> tks;
				Utilitario::Tokenize(it->second, tks, ",");
				utime = Utilitario::convertStringToInt(tks[0]);
				for (j = 1; j < tks.size(); j++) {
					pfs->push_back(Utilitario::convertStringToInt(tks[j])); // maybe floats instead ints
				}
				profileSize = pfs->size();
				(*mprofiles)[pid] = (tIProfileObject) make_pair(utime, pfs);
				if (pid > maxpid)
					maxpid = pid;
			}
			it++;
		}

		if (mprofiles->size() == 0) {
			// cout << "+++ ImplicitProfiles() -- create new profile for user " << (*vusers)[i] << endl;
			// create a new implicit profile if user does not have one
			vector<int> *pfs = new vector<int> ;
			for (j = 0; j < profileSize; j++)
				pfs->push_back(0);
			utime = time(NULL); // last update time = creation time = now :)
			(*mprofiles)[0] = (tIProfileObject) make_pair(utime, pfs);
			(*iprofiles)[(*vusers)[i]] = mprofiles;
			// update Ginga context
			modify((*vusers)[i], 0);

		} else
			(*iprofiles)[(*vusers)[i]] = mprofiles;

		// cout << "+++ ImplicitProfiles() -- set nexpid " << (*vusers)[i] << endl;
		(*nextpid)[(*vusers)[i]] = maxpid + 1;
		setCurrentId((*vusers)[i], (*curProfile)[(*vusers)[i]]);
	}
	delete vusers;


	// clean up profile base by eliminating old profiles
	deleteOld();

	// clean up profile base by merging very similar profiles
	cluster();

	// starts auxiliar map
	/*
	s2i["Outros"] = 0;
	s2i["Turismo"] = 1;
	s2i["Auditorio"] = 2;
	s2i["Entrevista"] = 3;
	s2i["Politico"] = 4;
	s2i["Musical"] = 5;
	s2i["Telejornais"] = 6;
	s2i["Documentario"] = 7;
	s2i["indefinido"] = 8;
	s2i["Culinaria"] = 9;
	s2i["Moda"] = 10;
	s2i["Reportagem"] = 11;
	s2i["Debate"] = 12;
	s2i["Religioso"] = 13;
	s2i["Infantil"] = 14;
	s2i["Erotico"] = 15;
	s2i["Feminino"] = 16;
	s2i["Reality show"] = 17;
	s2i["Making of"] = 18;
	s2i["Humoristico"] = 19;
	s2i["Music"] = 20;
	s2i["Novela"] = 21;
	s2i["Serie"] = 22;
	s2i["Rural"] = 23;
	s2i["Esporte"] = 24;
	s2i["Educativo"] = 25;
	s2i["Televendas"] = 26;
	s2i["Show"] = 27;
	s2i["Game show"] = 28;
	s2i["Premiacao"] = 29;
	s2i["Saude"] = 30;
	s2i["Filme"] = 31;
	s2i["Sorteio"] = 32; */

	s2i["Outros"] = 0;
	s2i["Musical"] = 1;
	s2i["Telejornais"] = 2;
	s2i["Romance"] = 3;
	s2i["Infantil"] = 4;
	s2i["Acao"] = 5;
	s2i["Novela"] = 6;
	s2i["Serie"] = 7;
	s2i["Esporte"] = 8;
	s2i["Comedia"] = 9;
	s2i["Filme"] = 10;

}

// create a new implicit profile if user does not have one
void ImplicitProfiles::createProfile(int uid) {
	//cout << "ImplicitProfiles::createProfile " << uid << endl;
	vector<int> *pfs = new vector<int> ;
	for (int j = 0; j < profileSize; j++)
		pfs->push_back(0);
	time_t utime = time(NULL); // last update time = creation time = now :)
	tIProfileMap* mprofiles = new tIProfileMap;
	//cout << "ImplicitProfiles::createProfile mprofiles[0] " << uid << endl;
	(*mprofiles)[0] = (tIProfileObject) make_pair(utime, pfs);
	//cout << "ImplicitProfiles::createProfile iprofiles[uid] " << uid << endl;
	(*iprofiles)[uid] = mprofiles;

	// update Ginga context
	//cout << "ImplicitProfiles::createProfile modify " << uid << endl;
	modify(uid, 0);

	// set current profile for user
	//cout << "ImplicitProfiles::createProfile set cur and nextpid " << uid << endl;
	(*curProfile)[uid] = 0;
	setValue(uid, CURIPROFVAR, Utilitario::convertIntToString(uid) + ":0");

	// cout << "+++ ImplicitProfiles() -- set nexpid " << (*vusers)[i] << endl;
	(*nextpid)[uid] = 1;
};

// destructor for implicit context manager
ImplicitProfiles::~ImplicitProfiles() {
	// free memory
	tIProfiles::iterator it = iprofiles->begin();
	while (it != iprofiles->end()) {
		tIProfileMap* uprofs = it->second;
		tIProfileMap::iterator itu = uprofs->begin();
		while (itu != uprofs->end()) {
			delete itu->second.second;
			itu++;
		}
		delete uprofs;
		it++;
	}
	delete iprofiles;
	delete curProfile;
	delete nextpid;
}
;

// show implicit profiles -- debug assistance ;)))
void ImplicitProfiles::show() {
	tIProfiles::iterator it = iprofiles->begin();
	while (it != iprofiles->end()) {
		tIProfileMap* uprofs = it->second;
		tIProfileMap::iterator itu = uprofs->begin();
		while (itu != uprofs->end()) {
			cout << it->first << "\t" << itu->first << "\t"
					<< itu->second.first << " (" << getLastUpdateInDays(
					itu->second.first) << " days)\t";
			vector<int> *v = itu->second.second;
			cout << "<";
			for (int j = 0; j < v->size(); j++)
				cout << (*v)[j] << ", ";
			cout << ">" << endl;
			itu++;
		}
		it++;
	}
}
;

// delete very old profiles
void ImplicitProfiles::deleteOld() {
	tIProfiles::iterator it = iprofiles->begin();
	while (it != iprofiles->end()) {
		tIProfileMap* uprofs = it->second;
		tIProfileMap::iterator itu = uprofs->begin();
		if (uprofs->size() > 1) {
			int userid = it->first;
			itu++; // profile 0 not is deleted!
			while (itu != uprofs->end()) {
				int profid = itu->first;
				if (getLastUpdateInDays(itu->second.first)
						> MAX_PROFILE_UNUSED_TIME) {
					delVar(userid, IPROFVAR
							+ Utilitario::convertIntToString(profid));
					tIProfileMap::iterator aux = itu++;
					uprofs->erase(aux);
				} else
					itu++;
			}
		}
		it++;
	}
}
;

// merge very similar pofiles
void ImplicitProfiles::cluster() {
	// check if number of profiles < MAX_PROFILES_PER_TVSET
	int profile_count = (*iprofiles)[0]->size();
	if (profile_count < MAX_PROFILES_PER_TVSET) 
		return;

	// cluster similar profiles if many profiles were found
  	int Minclus = 2;
  	int Maxclus = MAX_PROFILES_PER_TVSET - 1;
	int i = 0, j = 0;
	//angelo
	tIProfiles::iterator it = iprofiles->begin();
	vector<vector<int> >  points;
	vector <int> pids; 
	while (it != iprofiles->end()) {
		if(it->first == 0){
		tIProfileMap* uprofs = it->second;
		tIProfileMap::iterator itu = uprofs->begin();
			while (itu != uprofs->end()) {
				vector<int>* auxvec;
				auxvec = itu->second.second ;	
				points.push_back(*auxvec);
				//points.push_back(itu->second.second);				
				pids.push_back(itu->first);
				itu++;
			}
		}
		it++;
	}
	IterativeKMeans ikm(points);
	ikm.cluster(Minclus, Maxclus);
	vector<int> clusterid = ikm.getClusterIds();
	vector<vector<int> > centroids = ikm.getCentroids();	

	if (centroids.size() > 0) {
		for (i = 0; i < (int)clusterid.size(); i++)
			cout << i << " in cluster " << clusterid[i] << endl;	

		int pid;
		for (i = 0; i < (int)centroids.size(); i++) {
			cout << i << ": ";
			vector<int> *vaux = new vector<int>;
			for (j = 0; j < (int)centroids[0].size(); j++){
				cout << (centroids[i])[j] << " ";
				vaux->push_back((centroids[i])[j]);
			}
			insert(0, vaux, pid);
			cout << "inseriu usuario " << pid << " no profile 0" << endl;
			cout << endl;
		}

		//remove os iprofs que foram usados no cluster do usuario DEFAULT
		for(i = 0; i < (int)pids.size(); i++){
			remove(0, pids[i]);
			cout << "removeu profile: " << pids[i] << " do usuario 0" << endl;
		}

		setCurrentId(0, pid);
	}
}; 


// get profile vector
vector<int>* ImplicitProfiles::get(int userid, int profid) {
	tIProfiles::iterator it = iprofiles->find(userid);
	if (it != iprofiles->end()) {
		tIProfileMap::iterator itu = it->second->find(profid);
 		if (itu != it->second->end())
			return itu->second.second;
	}
	return NULL;
}
;

// find profile age in days (note: if profile has 4.7 days, function will return 4 days)
int ImplicitProfiles::findLastUpdateInDays(int userid, int profid) {
	tIProfiles::iterator it = iprofiles->find(userid);
	if (it != iprofiles->end()) {
		tIProfileMap::iterator itu = it->second->find(profid);
		if (itu != it->second->end())
			return getLastUpdateInDays(itu->second.first);
	}
	return -1;
}

// get profile age in days (note: if profile has 4.7 days, function will return 4 days)
// -- no map existance check is performed!!!
int ImplicitProfiles::getLastUpdateInDays(int t) {
	time_t now = time(NULL);
	return (now - t) / 86400; // number of seconds in one day = 86400
}

// get current profile id
int ImplicitProfiles::getCurrentId(int userid) {
	map<int, int>::iterator i = curProfile->find(userid);
	if (i == curProfile->end())
		return -1;
	else
		return i->second;
}
;

// get profiles
tIProfiles* ImplicitProfiles::get() {
	if (iprofiles->size() > 0)
		return iprofiles;
	else
		return NULL;
}
;

// insert profile
bool ImplicitProfiles::insert(int userid, vector<int> *v, int &profid) {
	tIProfiles::iterator i = iprofiles->find(userid);
	if (i == iprofiles->end())
		return false;
	tIProfileMap* mprofiles = i->second;
	profid = (*nextpid)[userid];
	(*nextpid)[userid]++;
	time_t utime = time(NULL);
	(*mprofiles)[profid] = make_pair(utime, v);
	return modify(userid, profid);
}

// delete implicit profile property
void ImplicitProfiles::delVar(int userid, string property) {
	map<string, string> *contextTable =
			pctx->getContextManager()->getUserProfile(userid);
	map<string, string>::iterator i = contextTable->find(property);
	if (i != contextTable->end()) {
		pctx->lock();
		contextTable->erase(i);
		pctx->unlock();
	}
}
;

// delete profile
bool ImplicitProfiles::remove(int userid, int profid) {
	// current user/profid cannot be removed
	if (userid == 0 && profid == 0)
		return false;
	tIProfiles::iterator i = iprofiles->find(userid);
	if (i == iprofiles->end())
		return false;
	tIProfileMap* mprofiles = i->second;
	tIProfileMap::iterator ip = mprofiles->find(profid);
	if (ip == mprofiles->end())
		return false;
	delVar(userid, IPROFVAR + Utilitario::convertIntToString(profid));
	mprofiles->erase(ip);
	return true;
}
;

// set implicit profile property
void ImplicitProfiles::setValue(int userid, string property, string value) {
	map<string, string> *contextTable =
			pctx->getContextManager()->getUserProfile(userid);
	//cout << "ImplicitProfiles:setValue: " << property << "[" << userid << "] = " << value << " contextTable: " << hex << contextTable << endl;
	pctx->lock();
	(*contextTable)[property] = value;
	pctx->unlock();
}
;

// modify profile
bool ImplicitProfiles::modify(int userid, int profid) {
	tIProfiles::iterator i = iprofiles->find(userid);
	if (i == iprofiles->end())
		return false;
	tIProfileMap* mprofiles = i->second;
	tIProfileMap::iterator ip = mprofiles->find(profid);
	if (ip == mprofiles->end())
		return false;
	vector<int> *v = ip->second.second;
	ip->second.first = time(NULL);
	string values = Utilitario::convertIntToString(ip->second.first);
	for (int j = 0; j < v->size(); j++)
		values = values + "," + Utilitario::convertIntToString((*v)[j]);
	setValue(userid, IPROFVAR + Utilitario::convertIntToString(profid),
			values);
	//debug
	//cout << "¬¬¬¬¬¬¬¬¬¬¬¬¬¬¬¬¬¬¬¬¬¬¬¬¬¬¬¬UserId=" << userid << " " << IPROFVAR + Utilitario::convertIntToString(profid) << "=" << values << endl; 
	return true;
}
;

// set current profile of a given user
bool ImplicitProfiles::setCurrentId(int userid, int profid) {
	// profid is not checked! be wise!
	if (curProfile->find(userid) == curProfile->end())
		return false;
	(*curProfile)[userid] = profid; // current profile for each user
	setValue(userid, CURIPROFVAR, Utilitario::convertIntToString(userid) + ":" +  
                 Utilitario::convertIntToString(profid));
	//debug
	//cout << "%%%%%%%%%%%%%%%%%%%%%%%%%%%% Userid=" << Utilitario::convertIntToString(userid) << " ProfId=" << Utilitario::convertIntToString(profid) << endl;
	return true;
}
;

// Search the entire value of the subgenres
int ImplicitProfiles::getAxis(string s) {
	map<string, int>::iterator i = s2i.find(s);
	if (i != s2i.end())
		return i->second;
	else
		return 0;
}
;

}
}
}
}
}
}

