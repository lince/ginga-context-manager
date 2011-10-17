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

#ifndef _EXECUTIONOBJECT_H_
#define _EXECUTIONOBJECT_H_

#include "util/functions.h"
using namespace ::br::pucrio::telemidia::util;

#include "system/io/interface/input/CodeMap.h"
using namespace ::br::pucrio::telemidia::ginga::core::system::io;

#include "player/IPlayer.h"
using namespace ::br::pucrio::telemidia::ginga::core::player;

#include "ncl/animation/Animation.h"
using namespace ::br::pucrio::telemidia::ncl::animation;

#include "ncl/descriptor/GenericDescriptor.h"
using namespace ::br::pucrio::telemidia::ncl::descriptor;

#include "ncl/components/ReferenceContent.h"
using namespace ::br::pucrio::telemidia::ncl::components;

#include "ncl/interfaces/LabeledAnchor.h"
#include "ncl/interfaces/LambdaAnchor.h"
#include "ncl/interfaces/IntervalAnchor.h"
#include "ncl/interfaces/PropertyAnchor.h"
using namespace ::br::pucrio::telemidia::ncl::interfaces;

#include "ncl/layout/LayoutRegion.h"
using namespace ::br::pucrio::telemidia::ncl::layout;

#include "ncl/connectors/EventUtil.h"
using namespace ::br::pucrio::telemidia::ncl::connectors;

#include "CascadingDescriptor.h"
#include "FormatterEvent.h"
#include "PresentationEvent.h"
#include "SelectionEvent.h"
#include "AttributionEvent.h"
using namespace ::br::pucrio::telemidia::ginga::ncl::model::event;

#include "BeginEventTransition.h"
#include "EndEventTransition.h"
#include "EventTransition.h"
#include "EventTransitionManager.h"
#include "TransitionDispatcher.h"
using namespace ::br::pucrio::telemidia::ginga::ncl::model::event::transition;

#include "FormatterRegion.h"
using namespace ::br::pucrio::telemidia::ginga::ncl::model::presentation;

#include "NodeNesting.h"

#include <pthread.h>
#include <map>
#include <vector>
#include <string>
#include <iostream>
#include <set>
using namespace std;

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace ncl {
namespace model {
namespace components {
  class ExecutionObject {
	protected:
		string id;
		Node* dataObject;
		CascadingDescriptor* descriptor;
		double offsetTime;
		double startTime;
		PresentationEvent* wholeContent;

		set<string> typeSet;
		pthread_mutex_t mutex;
		pthread_mutex_t mutexEvent;
		pthread_mutex_t mutexParentTable;
		bool isLocked;
		bool deleting;
		bool isHandler;
		bool isHandling;

	private:
		map<Node*, Node*>* nodeParentTable;

	protected:
		map<Node*, void*>* parentTable; //CompositionExecutionObject
		bool visible;

	private:
		bool isItCompiled;

	protected:
		map<string, FormatterEvent*>* events;
		vector<PresentationEvent*>* presEvents;
		set<SelectionEvent*>* selectionEvents;
		vector<FormatterEvent*>* otherEvents;
		int pauseCount;
		FormatterEvent* mainEvent;
		EventTransitionManager* transMan;

	public:
		ExecutionObject(string id, Node* node, bool handling);

		ExecutionObject(
				string id,
				Node* node,
				GenericDescriptor* descriptor,
				bool handling);

		ExecutionObject(
				string id,
				Node* node,
				CascadingDescriptor* descriptor,
				bool handling);

		virtual ~ExecutionObject();

	private:
		void initializeExecutionObject(
			    string id,
			    Node* node,
			    CascadingDescriptor* descriptor,
			    bool handling);

	public:
		bool instanceOf(string s);
		int compareToUsingId(ExecutionObject* object);
		Node* getDataObject();
		CascadingDescriptor* getDescriptor();
		string getId();
		void* getParentObject(); //CompositeExecutionObject
		void* getParentObject(Node* node); //CompositeExecutionObject
		void addParentObject(void* parentObject, Node* parentNode);
		void addParentObject(Node* node, void* parentObject, Node* parentNode);
		void setDescriptor(CascadingDescriptor* cascadingDescriptor);
		void setDescriptor(GenericDescriptor* descriptor);
		string toString();
		virtual bool addEvent(FormatterEvent* event);
		void addPresentationEvent(PresentationEvent* event);
		int compareTo(ExecutionObject* object);
		int compareToUsingStartTime(ExecutionObject* object);
		bool containsEvent(FormatterEvent* event);
		FormatterEvent* getEventFromAnchorId(string anchorId);
		FormatterEvent* getEvent(string id);
		vector<FormatterEvent*>* getEvents();
		set<AnchorEvent*>* getSampleEvents();
		double getExpectedStartTime();
		PresentationEvent* getWholeContentPresentationEvent();
		void setStartTime(double t);
		void updateEventDurations();
		void updateEventDuration(PresentationEvent* event);
		bool removeEvent(FormatterEvent* event);
		bool isCompiled();
		void setCompiled(bool status);
		void removeNode(Node* node);
		vector<Node*>* getNodes();
		NodeNesting* getNodePerspective();
		NodeNesting* getNodePerspective(Node* node);
		vector<ExecutionObject*>* getObjectPerspective();
		vector<ExecutionObject*>* getObjectPerspective(Node* node);
		vector<Node*>* getParentNodes();
		FormatterEvent* getMainEvent();
		virtual bool prepare(FormatterEvent* event, double offsetTime);
		virtual bool start();
		void updateTransitionTable(
				double value, IPlayer* player, short int transType);

		void resetTransitionEvents(short int transType);
		void prepareTransitionEvents(short int transType, double startTime);
		set<double>* getTransitionsValues(short int transType);
		EventTransition* getNextTransition();
		virtual bool stop();
		virtual bool abort();
		virtual bool pause();
		virtual bool resume();
		virtual bool setPropertyValue(
				AttributionEvent* event, string value, Animation* anim);

	public:
		virtual bool setPropertyValue(string propName, vector <string> *params);
		virtual bool setPropertyValue(string propName, vector <double> *params);

		double getPropertyValueAsDouble(string param);
		string getPropertyValueAsString(string param);

		virtual bool unprepare();

		void setHandling(bool isHandling);
		void setHandler(bool isHandler);
		void selectionEvent(int accessCode, double currentTime);
		set<int>* getInputEvents();

	protected:
		bool lock();
		bool unlock();

	private:
		void lockEvents();
		void unlockEvents();

		void lockParentTable();
		void unlockParentTable();
  };
}
}
}
}
}
}
}

#endif //_EXECUTIONOBJECT_H_
