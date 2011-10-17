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


#include "contextmanager/DataInterface.h"

//using namespace br::ufscar::lince::ginga::recommender::localagent;
using namespace ::br::pucrio::telemidia::ginga::core::contextmanager;

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace core {
namespace contextmanager {


// constructor
// get data from a file where each line has the format:
// data1,data2,data3,...
// Each line represents a service
// data1 is the offset time (in seconds) for the start of the service
// Ex: 7,globo,corujao,filme,filme
// Globo service "corujao" (genre: filme, subgenre: filme) will start 7
// seconds after the beginning of the simulation
DataInterface::DataInterface() {
	DataItens = new vector<TVData> ;
	string line;
	// load simulation data
	char fname[255];
	cout << "DataInterface::DataInterface(): open file at dir " << getcwd(fname, 255) << endl;
	ifstream rdfile("/usr/local/sbin/sidata.csv");
	if (rdfile.is_open()) {
		while (!rdfile.eof()) {
			getline(rdfile, line);
			if (line != "") {
				vector<string> tks;
				Utilitario::Tokenize(line, tks, ",");
				DataItens->push_back(TVData(Utilitario::convertStringToInt(
						tks[0]), tks[1], tks[2], tks[3], tks[4]));
				 cout << "Loading " << tks[0] << " " << tks[1] << " " << tks[2] << " " << tks[3] << " " << tks[4] << endl;
			}
		}
		rdfile.close();
		cout << "close file..." << endl;
	} else
		cout << "DataInterface::DataInterface(): Unable to open file" << endl;
	// cout << "start vars..." << endl;
	// start time variables
	starttime = time(NULL);
	startpoint = 0;
}
;

// destructor
DataInterface::~DataInterface() {
	delete DataItens;
}
;

// get data simulating the passage of the time
// returns NULL if no data is available when the request is made
TVData *DataInterface::getData() {
	int elapsedtime = time(NULL) - starttime;
	int t = startpoint;

	if (t == 0 && elapsedtime < ((*DataItens)[t]).getTimeStamp())
		return NULL;

	TVData *pd = NULL;
	while (t < DataItens->size() && elapsedtime
			>= ((*DataItens)[t]).getTimeStamp())
		pd = &((*DataItens)[t++]);
	startpoint = t;

	if (pd != NULL)
		return pd;

	return &((*DataItens)[t - 1]);
}
;

}
}
}
}
}
}


