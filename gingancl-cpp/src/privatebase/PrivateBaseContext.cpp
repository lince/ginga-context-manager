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

#include "../include/privatebase/PrivateBaseContext.h"

#include "converter/IDocumentConverter.h"
using namespace ::br::pucrio::telemidia::converter;

#include "../config.h"
#if HAVE_COMPSUPPORT
#include "cm/IComponentManager.h"
using namespace ::br::pucrio::telemidia::ginga::core::cm;
#else
#include "../../ncl30-converter/include/ncl/NclDocumentConverter.h"
using namespace ::br::pucrio::telemidia::converter::ncl;
#endif

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace ncl {
#if HAVE_COMPSUPPORT
	static IComponentManager* cm = IComponentManager::getCMInstance();
#endif

	PrivateBaseContext::PrivateBaseContext() {
		this->baseDocuments       = new map<string, NclDocument*>;
		this->visibleDocuments    = new map<string, NclDocument*>;
		this->embeddedDocuments   = new map<string, EmbeddedNclData*>;
		this->idToLocation        = new map<string, string>;
		this->layouts             = new map<NclDocument*, IDeviceLayout*>;
		this->layoutsGB           = new set<IDeviceLayout*>;
		this->privateBase         = NULL;
	}

	PrivateBaseContext::~PrivateBaseContext() {
		map<string, NclDocument*>::iterator i;
		map<string, EmbeddedNclData*>::iterator j;
		set<IDeviceLayout*>::iterator k;

		if (baseDocuments != NULL) {
			i = baseDocuments->begin();
			while (i != baseDocuments->end()) {
				delete i->second;
				++i;
			}
			delete baseDocuments;
			baseDocuments = NULL;
		}

		if (visibleDocuments != NULL) {
			i = visibleDocuments->begin();
			while (i != visibleDocuments->end()) {
				delete i->second;
				++i;
			}
			delete visibleDocuments;
			visibleDocuments = NULL;
		}

		if (embeddedDocuments != NULL) {
			j = embeddedDocuments->begin();
			while (j != embeddedDocuments->end()) {
				delete j->second->embeddedDocument;
				delete j->second;
				++j;
			}
			delete embeddedDocuments;
			embeddedDocuments = NULL;
		}

		if (idToLocation != NULL) {
			delete idToLocation;
			idToLocation = NULL;
		}

		if (layoutsGB != NULL) {
			k = layoutsGB->begin();
			while (k != layoutsGB->end()) {
				delete *k;
				++k;
			}
			delete layoutsGB;
			layoutsGB = NULL;
		}

		if (layouts != NULL) {
			delete layouts;
			layouts = NULL;
		}

		if (privateBase != NULL) {
			delete privateBase;
			privateBase = NULL;
		}
	}

	void PrivateBaseContext::createPrivateBase(string id) {
		if (privateBase != NULL) {
			cout << "PrivateBaseContext::createPrivateBase Warning!";
			cout << " trying to overwrite private base context with a ";
			cout << " new private base '" << id << "'" << endl;
			return;
		}

		privateBase = new PrivateBase(id);
	}

	NclDocument* PrivateBaseContext::compileDocument(
			string location, IDeviceLayout* deviceLayout) {

		IDocumentConverter* compiler = NULL;
		NclDocument* document = NULL;
		string docUrl;

		if (isAbsolutePath(location)) {
			docUrl = location;

		} else {
			docUrl = getCurrentPath() + location;
		}

#if HAVE_COMPSUPPORT
		compiler = ((ConverterCreator*)(cm->getObject(
				"NclDocumentConverter")))(this, deviceLayout);
#else
		compiler = new NclDocumentConverter(this, deviceLayout);
#endif

		compiler->parse(docUrl);
		document = (NclDocument*)(compiler->getObject("return", "document"));

		delete compiler;
		compiler = NULL;

		layoutsGB->insert(deviceLayout);
		(*layouts)[document] = deviceLayout;
    	return document;
	}

	NclDocument* PrivateBaseContext::addDocument(
			string location, IDeviceLayout* deviceLayout) {

		NclDocument* newDocument;
		NclDocument* document;
		string docUrl, id = "";

		if (isAbsolutePath(location)) {
			docUrl = location;

		} else {
			docUrl = getCurrentPath() + location;
		}

		docUrl = updatePath(docUrl);

		if (baseDocuments->count(docUrl) != 0) {
			cout << "PrivateBaseContext::addDocument Warning! ";
			cout << "Trying to add the same doc twice '" << docUrl;
			cout << "'";
			cout << endl;
			return (*baseDocuments)[docUrl];

		} else if (visibleDocuments->count(docUrl) != 0) {
			document = (*visibleDocuments)[docUrl];
			visibleDocuments->erase(visibleDocuments->find(docUrl));

			if (document->getBody() != NULL) {
				privateBase->addNode(document->getBody());
			}

			(*baseDocuments)[docUrl] = document;
			return document;

		} else {
			newDocument = compileDocument(docUrl, deviceLayout);
			if (newDocument != NULL) {
				id = newDocument->getId();
				if (idToLocation->count(id) != 0) {
#if HAVE_COMPSUPPORT
					printTimeStamp();
					cm->releaseComponentFromObject("NclDocumentConverter");
#endif
					cout << "PrivateBaseContext::addDocument Warning! ";
					cout << "Can't add newDoc: replicated id '";
					cout << id << "'";
					cout << endl;
					return NULL;
				}

				if (newDocument->getBody() != NULL) {
					privateBase->addNode(newDocument->getBody());
				}

				(*baseDocuments)[docUrl] = newDocument;
				(*idToLocation)[id]      = docUrl;

			} else {
				cout << "PrivateBaseContext::addDocument Warning! ";
				cout << "Can't compile document '" << docUrl << "'";
				cout << endl;
			}

#if HAVE_COMPSUPPORT
			printTimeStamp();
			cm->releaseComponentFromObject("NclDocumentConverter");
#endif

			cout << "PrivateBaseContext::addDocument ";
			cout << "id='" << id << "' url='" << docUrl << "' all done";
			cout << endl;
			return newDocument;
		}
	}

	NclDocument* PrivateBaseContext::embedDocument(
			string docId,
			string nodeId,
			string location,
			IDeviceLayout* deviceLayout) {

		map<string, EmbeddedNclData*>::iterator i;
		EmbeddedNclData* data;
		string docUrl;
		NclDocument* newDocument;

		i = embeddedDocuments->find(docId + "::" + nodeId);
		if (i != embeddedDocuments->end()) {
			data = i->second;
			if (data->embeddedNclNodeId == nodeId) {
				cout << "PrivateBaseContext::embedDocument Warning! ";
				cout << "Trying to embed the same nodeId twice '";
				cout << nodeId << "' for parent '" << docId << "'";
				cout << endl;
				return data->embeddedDocument;
			}
		}

		if (isAbsolutePath(location)) {
			docUrl = location;

		} else {
			docUrl = getCurrentPath() + location;
		}

		docUrl = updatePath(docUrl);

		//TODO: insert new embedded document body inside its node
		newDocument = compileDocument(docUrl, deviceLayout);
		if (newDocument == NULL) {
			return NULL;
		}

		data                          = new EmbeddedNclData;
		data->embeddedDocument        = newDocument;
		data->embeddedNclNodeId       = nodeId;
		data->embeddedNclNodeLocation = docUrl;

		(*embeddedDocuments)[docId + "::" + nodeId] = data;

		return newDocument;
	}

	void* PrivateBaseContext::addVisibleDocument(
			string location, IDeviceLayout* deviceLayout) {

		NclDocument* newDocument;
		string id;

		location = updatePath(location);
		if (baseDocuments->count(location) != 0) {
			return (*baseDocuments)[location];

		} else if (visibleDocuments->count(location) != 0) {
			return (*visibleDocuments)[location];

		} else {
			newDocument = compileDocument(location, deviceLayout);
			if (newDocument != NULL) {
				id = newDocument->getId();
				if (idToLocation->count(id) != 0) {
					return NULL;
				}

				(*visibleDocuments)[location] = newDocument;
				(*idToLocation)[id] = location;
		    	return newDocument;
			}
		}

		return NULL;
	}

	string PrivateBaseContext::getDocumentLocation(string docId) {
		if (idToLocation->count(docId) == 0) {
			return "";
		}

		return (*idToLocation)[docId];
	}

	string PrivateBaseContext::getEmbeddedDocumentLocation(
			string parentDocId, string nodeId) {

		map<string, EmbeddedNclData*>::iterator i;

		i = embeddedDocuments->find(parentDocId + "::" + nodeId);
		if (i != embeddedDocuments->end()) {
			return i->second->embeddedNclNodeLocation;
		}

		return "";
	}

	NclDocument* PrivateBaseContext::getDocument(string id) {
		string location;

		if (idToLocation->count(id) == 0) {
			return NULL;
		}

		location = (*idToLocation)[id];

		if (baseDocuments->count(location) != 0) {
			return (*baseDocuments)[location];

		} else {
			return (*visibleDocuments)[location];
		}
	}

	NclDocument* PrivateBaseContext::getEmbeddedDocument(
			string parentDocId, string nodeId) {

		map<string, EmbeddedNclData*>::iterator i;
		NclDocument* document = NULL;

		i = embeddedDocuments->find(parentDocId + "::" + nodeId);
		if (i != embeddedDocuments->end()) {
			document = i->second->embeddedDocument;
		}

		return document;
	}

	vector<NclDocument*>* PrivateBaseContext::getDocuments() {
		map<string, NclDocument*>::iterator i;
		vector<NclDocument*>* documents;
		documents = new vector<NclDocument*>;

		for (i = baseDocuments->begin(); i != baseDocuments->end(); ++i) {
			documents->push_back(i->second);
		}

		return documents;
	}

	NclDocument* PrivateBaseContext::removeDocument(string id) {
		NclDocument* document;

		document = getBaseDocument(id);
		if (document != NULL && idToLocation->count(id) != 0) {
			if (baseDocuments->count((*idToLocation)[id]) != 0) {
				baseDocuments->erase(baseDocuments->find((*idToLocation)[id]));
			}

			if (document->getBody() != NULL) {
				privateBase->removeNode(document->getBody());
			}

			return document;

		} else if (idToLocation->count(id) != 0) {
			if (visibleDocuments->count((*idToLocation)[id]) != 0){
				visibleDocuments->erase(
					    visibleDocuments->find((*idToLocation)[id]));

				return document;
			}

		}

		return NULL;
	}

	NclDocument* PrivateBaseContext::removeEmbeddedDocument(
			string parentDocId, string nodeId) {

		map<string, EmbeddedNclData*>::iterator i;
		NclDocument* document = NULL;

		i = embeddedDocuments->find(parentDocId + "::" + nodeId);
		if (i != embeddedDocuments->end()) {
			document = i->second->embeddedDocument;
			delete i->second;
			embeddedDocuments->erase(i);
		}
		return document;
	}

	void PrivateBaseContext::clear() {
		NclDocument* document;
		map<string, NclDocument*>::iterator i;

		i = baseDocuments->begin();
		while (i != baseDocuments->end()) {
			document = i->second;
			document->clear();
			++i;
		}

		i = visibleDocuments->begin();
		while (i != visibleDocuments->end()) {
			document = i->second;
			document->clear();
			++i;
		}

		baseDocuments->clear();
		visibleDocuments->clear();
		idToLocation->clear();

		privateBase->clearAnchors();
		privateBase->clearPorts();
		privateBase->clearNodes();
	}

	NclDocument* PrivateBaseContext::getBaseDocument(string documentId) {
		string location;
		NclDocument* document;

		if (idToLocation->count(documentId) == 0) {
			return NULL;
		}

		location = (*idToLocation)[documentId];
		if (baseDocuments->count(location) == 0) {
			return NULL;
		}

		document = (*baseDocuments)[location];
		return document;
	}

	void* PrivateBaseContext::compileEntity(
		    string location,
		    NclDocument* document,
			void* parentObject) {

		IDocumentConverter* compiler;
		string entityUrl;
		void* entity;
		IDeviceLayout* deviceLayout;

		if (isAbsolutePath(location)) {
			entityUrl = location;

		} else {
			entityUrl = getCurrentPath() + location;
		}

		if (layouts->count(document) == 0) {
			cout << "PrivateBaseContext::compileEntity Warning! Can't ";
			cout << "find layout for '" << location << "'" << endl;
			return NULL;
		}

		deviceLayout = (*layouts)[document];

#if HAVE_COMPSUPPORT
		compiler = ((ConverterCreator*)(cm->getObject(
				"NclDocumentConverter")))(this, deviceLayout);

#else
		compiler = new NclDocumentConverter(this, deviceLayout);
#endif

		entity = compiler->parseEntity(entityUrl, document, parentObject);

		delete compiler;

#if HAVE_COMPSUPPORT
		printTimeStamp();
		cm->releaseComponentFromObject("NclDocumentConverter");
#endif

		return entity;
	}

	LayoutRegion* PrivateBaseContext::addRegion(
		    string documentId,
		    string regionBaseId,
		    string regionId,
		    string xmlRegion) {

		NclDocument* document;
		RegionBase* base;
		LayoutRegion* region;
		LayoutRegion* parentRegion;

		document = getBaseDocument(documentId);
		if (document == NULL) {
			return NULL;
		}

		if (regionBaseId == "") {
			base = document->getRegionBase("systemScreen(0)");

		} else {
			base = document->getRegionBase(regionBaseId);
		}

		if (base == NULL) {
			return NULL;
		}

		if (trim(regionId) == "") {
			parentRegion = NULL;
			region = (LayoutRegion*)compileEntity(xmlRegion, document, base);

		} else {
			parentRegion = document->getRegion(regionId);
			if (parentRegion == NULL) {
				return NULL;
			}
			region = (LayoutRegion*)compileEntity(xmlRegion, document, base);
		}

		if (region == NULL) {
			return NULL;
		}

		if (parentRegion == NULL) {
			base->addRegion(region);

		} else {
			parentRegion->addRegion(region);
		}

		return region;
	}

	LayoutRegion* PrivateBaseContext::removeRegion(
		    string documentId, string regionBaseId, string regionId) {

		NclDocument* document;
		RegionBase* base;
		LayoutRegion* region;

		document = getBaseDocument(documentId);
		if (document == NULL) {
			return NULL;
		}

		if (regionBaseId == "") {
			base = document->getRegionBase("systemScreen(0)");

		} else {
			base = document->getRegionBase(regionBaseId);
		}

		if (base == NULL) {
			return NULL;
		}

		region = base->getRegion(regionId);
		if (region == NULL) {
			return NULL;
		}

		if (base->removeRegion(region)) {
			return region;
		}

		return NULL;
	}

	RegionBase* PrivateBaseContext::addRegionBase(
		     string documentId, string xmlRegionBase) {

		NclDocument* document;
		RegionBase* base;

		document = getBaseDocument(documentId);
		if (document == NULL) {
			return NULL;
		}

		base = (RegionBase*)compileEntity(xmlRegionBase, document, NULL);
		if (base == NULL) {
			return NULL;
		}

		document->addRegionBase(base);
		return base;
	}

	RegionBase* PrivateBaseContext::removeRegionBase(
		    string documentId, string regionBaseId) {

		NclDocument* document;
		RegionBase* base;

		document = getBaseDocument(documentId);
		if (document == NULL) {
			return NULL;
		}

		base = document->getRegionBase(regionBaseId);
		if (base == NULL) {
			return NULL;
		}

		document->removeRegionBase(regionBaseId);
		return base;
	}

	Rule* PrivateBaseContext::addRule(string documentId, string xmlRule) {

		NclDocument* document;
		RuleBase* base;
		Rule* rule;

		document = getBaseDocument(documentId);
		if (document == NULL) {
			return NULL;
		}

		base = document->getRuleBase();
		if (base == NULL) {
			return NULL;
		}

		rule = (Rule*)compileEntity(xmlRule, document, base);
		if (rule == NULL) {
			return NULL;
		}

		base->addRule(rule);
		return rule;
	}

	Rule* PrivateBaseContext::removeRule(string documentId, string ruleId) {
		NclDocument* document;
		RuleBase* base;
		Rule* rule;

		document = getBaseDocument(documentId);
		if (document == NULL) {
			return NULL;
		}

		base = document->getRuleBase();
		if (base == NULL) {
			return NULL;
		}

		rule = base->getRule(ruleId);
		if (rule == NULL) {
			return NULL;
		}

		if (base->removeRule(rule)) {
			return rule;

		} else {
			return NULL;
		}
	}

	RuleBase* PrivateBaseContext::addRuleBase(
		    string documentId, string xmlRuleBase) {

		NclDocument* document;
		RuleBase* base;

		document = getBaseDocument(documentId);
		if (document == NULL) {
			return NULL;
		}

		base = (RuleBase*)compileEntity(xmlRuleBase, document, NULL);
		if (base == NULL) {
			return NULL;
		}

		document->setRuleBase(base);
		return base;
	}

	RuleBase* PrivateBaseContext::removeRuleBase(
		    string documentId, string ruleBaseId) {

		NclDocument* document;
		RuleBase* base;

		document = getBaseDocument(documentId);
		if (document == NULL) {
			return NULL;
		}

		base = document->getRuleBase();
		if (base == NULL) {
			return NULL;
		}

		document->setRuleBase(NULL);
		return base;
	}

	Transition* PrivateBaseContext::addTransition(
		    string documentId, string xmlTransition) {

		NclDocument* document;
		TransitionBase* base;
		Transition* transition;

		document = getBaseDocument(documentId);
		if (document == NULL) {
			return NULL;
		}

		base = document->getTransitionBase();
		if (base == NULL) {
			return NULL;
		}

		transition = (Transition*)compileEntity(xmlTransition, document, base);

		if (transition == NULL) {
			return NULL;
		}

		base->addTransition(transition);
		return transition;
	}

	Transition* PrivateBaseContext::removeTransition(
		    string documentId, string transitionId) {

		NclDocument* document;
		TransitionBase* base;
		Transition* transition;

		document = getBaseDocument(documentId);
		if (document == NULL) {
			return NULL;
		}

		base = document->getTransitionBase();
		if (base == NULL) {
			return NULL;
		}

		transition = base->getTransition(transitionId);
		if (transition == NULL) {
			return NULL;
		}

		if (base->removeTransition(transition)) {
			return transition;

		} else {
			return NULL;
		}
	}

	TransitionBase* PrivateBaseContext::addTransitionBase(
		    string documentId, string xmlTransitionBase) {

		NclDocument* document;
		TransitionBase* base;

		document = getBaseDocument(documentId);
		if (document == NULL) {
			return NULL;
		}

		base = (TransitionBase*)compileEntity(
			    xmlTransitionBase, document, NULL);

		if (base == NULL) {
			return NULL;
		}

		document->setTransitionBase(base);
		return base;
	}

	TransitionBase* PrivateBaseContext::removeTransitionBase(
		    string documentId, string transitionBaseId) {

		NclDocument* document;
		TransitionBase* base;

		document = getBaseDocument(documentId);
		if (document == NULL) {
			return NULL;
		}

		base = document->getTransitionBase();
		if (base == NULL) {
			return NULL;
		}

		document->setTransitionBase(NULL);
		return base;
	}

	Connector* PrivateBaseContext::addConnector(
		    string documentId, string xmlConnector) {

		NclDocument* document;
		ConnectorBase* base;
		Connector* connector;

		document = getBaseDocument(documentId);
		if (document == NULL) {
			return NULL;
		}

		base = document->getConnectorBase();
		if (base == NULL) {
			return NULL;
		}

		connector = (Connector*)(compileEntity(
			     xmlConnector, document, (void*)base));

		if (connector == NULL) {
			return NULL;
		}

		base->addConnector(connector);
		return connector;
	}

	Connector* PrivateBaseContext::removeConnector(
		    string documentId, string connectorId) {

		NclDocument* document;
		ConnectorBase* base;
		Connector* connector;

		document = getBaseDocument(documentId);
		if (document == NULL) {
			return NULL;
		}

		base = document->getConnectorBase();
		if (base == NULL) {
			return NULL;
		}

		connector = base->getConnector(connectorId);
		if (connector == NULL) {
			return NULL;
		}

		if (base->removeConnector(connector)) {
			return connector;

		} else {
			return NULL;
		}
	}

	ConnectorBase* PrivateBaseContext::addConnectorBase(
		    string documentId, string xmlConnectorBase) {

		NclDocument* document;
		ConnectorBase* base;

		document = getBaseDocument(documentId);
		if (document == NULL) {
			return NULL;
		}

		base = (ConnectorBase*)compileEntity(xmlConnectorBase, document, NULL);
		if (base == NULL) {
			return NULL;
		}

		document->setConnectorBase(base);
		return base;
	}

	ConnectorBase* PrivateBaseContext::removeConnectorBase(
		    string documentId, string connectorBaseId) {

		NclDocument* document;
		ConnectorBase* base;

		document = getBaseDocument(documentId);
		if (document == NULL) {
			return NULL;
		}

		base = document->getConnectorBase();
		if (base == NULL) {
			return NULL;
		}

		document->setConnectorBase(NULL);
		return base;
	}

	GenericDescriptor* PrivateBaseContext::addDescriptor(
		    string documentId, string xmlDescriptor) {

		NclDocument* document;
		DescriptorBase* base;
		GenericDescriptor* descriptor;

		document = getBaseDocument(documentId);
		if (document == NULL) {
			return NULL;
		}

		base = document->getDescriptorBase();
		if (base == NULL) {
			return NULL;
		}

		descriptor = (GenericDescriptor*)(compileEntity(
			     xmlDescriptor, document, base));

		if (descriptor == NULL) {
			return NULL;
		}

		base->addDescriptor(descriptor);
		return descriptor;
	}

	GenericDescriptor* PrivateBaseContext::removeDescriptor(
		    string documentId, string descriptorId) {

		NclDocument* document;
		DescriptorBase* base;
		GenericDescriptor* descriptor;

		document = getBaseDocument(documentId);
		if (document == NULL) {
			return NULL;
		}

		base = document->getDescriptorBase();
		if (base == NULL) {
			return NULL;
		}

		descriptor = base->getDescriptor(descriptorId);
		if (descriptor == NULL) {
			return NULL;
		}

		if (base->removeDescriptor(descriptor)) {
			return descriptor;

		}

		return NULL;
	}

	DescriptorBase* PrivateBaseContext::addDescriptorBase(
		    string documentId, string xmlDescriptorBase) {

		NclDocument* document;
		DescriptorBase* base;

		document = getBaseDocument(documentId);
		if (document == NULL) {
			return NULL;
		}

		base = (DescriptorBase*)(compileEntity(
			    xmlDescriptorBase, document, NULL));

		if (base == NULL) {
			return NULL;
		}

		document->setDescriptorBase(base);
		return base;
	}

	DescriptorBase* PrivateBaseContext::removeDescriptorBase(
		    string documentId, string descriptorBaseId) {

		NclDocument* document;
		DescriptorBase* base;

		document = getBaseDocument(documentId);
		if (document == NULL) {
			return NULL;
		}

		base = document->getDescriptorBase();
		if (base == NULL) {
			return NULL;
		}

		document->setDescriptorBase(NULL);
		return base;
	}

	Base* PrivateBaseContext::getBase(NclDocument* document, string baseId) {
		Base* base;

		base = document->getConnectorBase();
		if (base->getId() != "" && base->getId() == baseId) {
			return base;
		}

		base = document->getRegionBase(baseId);
		if (base != NULL) {
			return base;
		}

		base = document->getRuleBase();
		if (base->getId() != "" && base->getId() == baseId) {
			return base;
		}

		base = document->getDescriptorBase();
		if (base->getId() != "" && base->getId() == baseId) {
			return base;
		}

		return NULL;
	}

	Base* PrivateBaseContext::addImportBase(
		    string documentId, string docBaseId, string xmlImportBase) {

		IDocumentConverter* compiler;
		NclDocument* document;
		Base* parentBase;
		void* importElement;
		string location, alias;
		NclDocument* importedDocument;
		Base* base = NULL;
		IDeviceLayout* deviceLayout;

		document = getBaseDocument(documentId);
		if (document == NULL) {
			return NULL;
		}

		parentBase = getBase(document, docBaseId);
		if (parentBase == NULL) {
			return NULL;
		}

		importElement = (void*)(compileEntity(
			    xmlImportBase, document, NULL));

		if (importElement == NULL) {
			return NULL;
		}

		if (layouts->count(document) == 0) {
			cout << "PrivateBaseContext::addImportBase Warning! Can't ";
			cout << "find layout for '" << location << "'" << endl;
			return NULL;
		}

		deviceLayout = (*layouts)[document];

#if HAVE_COMPSUPPORT
		compiler = ((ConverterCreator*)(cm->getObject(
				"NclDocumentConverter")))(this, deviceLayout);
#else
		compiler = new NclDocumentConverter(this, deviceLayout);
#endif

		location = compiler->getAttribute(importElement, "documentURI");
		if (location != "") {
			importedDocument = (NclDocument*)(addVisibleDocument(
					location, deviceLayout));

			if (importedDocument == NULL) {
				if (parentBase->instanceOf("ConnectorBase")) {
					base = importedDocument->getConnectorBase();

				} else if (parentBase->instanceOf("RegionBase")) {
					base = importedDocument->getRegionBase(docBaseId);

				} else if (parentBase->instanceOf("DescriptorBase")) {
					base = importedDocument->getDescriptorBase();

				} else if (parentBase->instanceOf("RuleBase")) {
					base = importedDocument->getRuleBase();
				}

				if (base != NULL) {
					try {
						alias = compiler->getAttribute(importElement, "alias");
						parentBase->addBase(base, alias, location);

					} catch (IllegalBaseTypeException* exc) {
						//do nothing
					}
				}
			}
		}

		delete compiler;

#if HAVE_COMPSUPPORT
		printTimeStamp();
		cm->releaseComponentFromObject("NclDocumentConverter");
#endif

		return base;
	}

	Base* PrivateBaseContext::removeImportBase(
		    string documentId, string docBaseId, string documentURI) {

		NclDocument* document;
		Base* parentBase;
		vector<Base*>* bases;
		vector<Base*>::iterator i;
		string location;

		document = getBaseDocument(documentId);
		if (document == NULL) {
			return NULL;
		}

		parentBase = getBase(document, docBaseId);
		if (parentBase == NULL) {
			return NULL;
		}

		bases = parentBase->getBases();
		if (bases != NULL) {
			i = bases->begin();
			while (i != bases->end()) {
				location = parentBase->getBaseLocation(*i);
				if (location != "" && location == documentURI) {
					parentBase->removeBase(*i);
					return *i;
				}
				++i;
			}
		}

		return NULL;
	}

	NclDocument* PrivateBaseContext::addImportedDocumentBase(
		    string documentId, string xmlImportedDocumentBase) {

		NclDocument* document;
		void* baseElement;

		document = getBaseDocument(documentId);
		if (document == NULL) {
			return NULL;
		}

		baseElement = (void*)(compileEntity(
			    xmlImportedDocumentBase, document, NULL));

		if (baseElement == NULL) {
			return NULL;
		}

		return document;
	}

	NclDocument* PrivateBaseContext::removeImportedDocumentBase(
		    string documentId, string importedDocumentBaseId) {

		NclDocument* document;

		document = getBaseDocument(documentId);
		if (document == NULL) {
			return NULL;
		}

		vector<NclDocument*>* documents;
		vector<NclDocument*>::iterator i;

		documents = document->getDocuments();
		if (documents != NULL) {
			i = documents->begin();
			while (i != documents->end()) {
				document->removeDocument(*i);
				documents = document->getDocuments();
				++i;
			}
		}

		return document;
	}

	NclDocument* PrivateBaseContext::addImportNCL(
		    string documentId, string xmlImportNCL) {

		IDocumentConverter* compiler;
		NclDocument* document;
		void* importElement;
		string location, alias;
		NclDocument* importedDocument = NULL;
		IDeviceLayout* deviceLayout;

		document = getBaseDocument(documentId);
		if (document == NULL) {
			cout << "PrivateBaseContext::addImportNCL Warning! ";
			cout << "Can't find document '" << documentId << "'";
			cout << ": returning NULL" << endl;
			return NULL;
		}

		importElement = (void*)(compileEntity(xmlImportNCL, document, NULL));

		if (importElement == NULL) {
			cout << "PrivateBaseContext::addImportNCL Warning! ";
			cout << "Can't compile importNCL '" << xmlImportNCL << "'";
			cout << ": returning NULL" << endl;
			return NULL;
		}

		if (layouts->count(document) == 0) {
			cout << "PrivateBaseContext::addImportNcl Warning! Can't ";
			cout << "find layout for '" << location << "'" << endl;
			return NULL;
		}

		deviceLayout = (*layouts)[document];

#if HAVE_COMPSUPPORT
		compiler = ((ConverterCreator*)(cm->getObject(
				"NclDocumentConverter")))(this, deviceLayout);
#else
		compiler = new NclDocumentConverter(this, deviceLayout);
#endif

		location = compiler->getAttribute(importElement, "documentURI");
		if (location != "") {
			importedDocument = (NclDocument*)addVisibleDocument(
					location, deviceLayout);

			if (importedDocument != NULL) {
				alias = compiler->getAttribute(importElement, "alias");
				document->addDocument(importedDocument, alias, location);
			}
		}

		delete compiler;

#if HAVE_COMPSUPPORT
		cm->releaseComponentFromObject("NclDocumentConverter");
#endif

		return importedDocument;
	}

	NclDocument* PrivateBaseContext::removeImportNCL(
		    string documentId, string documentURI) {

		NclDocument* document;

		document = getBaseDocument(documentId);
		if (document == NULL) {
			return NULL;
		}

		vector<NclDocument*>* documents;
		vector<NclDocument*>::iterator i;
		NclDocument* importedDocument = NULL;
		string location;

		documents = document->getDocuments();
		if (documents != NULL) {
			i = documents->begin();
			while (i != documents->end()) {
				importedDocument = *i;
				location = document->getDocumentLocation(*i);
				if (location != "" && location == documentURI) {
					document->removeDocument(importedDocument);
					return importedDocument;
				}

				++i;
			}
		}

		return NULL;
	}

	/*
	addTransition (baseId, documentId, xmlTransition)
	removeTransition (baseId, documentId, transitionId)
	addTransitionBase (baseId, documentId, xmlTransitionBase)
	removeTransitionBase (baseId, documentId, transitionBaseId)
	*/

	Node* PrivateBaseContext::addNode(
		    string documentId, string compositeId, string xmlNode) {

		NclDocument* document;
		Node* node;
		ContextNode* contextNode;

		document = getBaseDocument(documentId);
		if (document == NULL) {
			cout << "PrivateBaseContext::addNode Warning! cant add node: ";
			cout << "document '" << documentId << "' not found";
			cout << endl;
			return NULL;
		}

		node = document->getNode(compositeId);
		if (node == NULL || !(node->instanceOf("ContextNode"))) {
			cout << "PrivateBaseContext::addNode Warning! cant add node: ";
			cout << "parent composite '" << compositeId << "' not found";
			cout << endl;
			return NULL;
		}

		contextNode = (ContextNode*)node;

		try {
			cout << "PrivateBaseContext::addNode Trying to add node: '";
			cout << xmlNode << "'" << endl;
			node = (Node*)(compileEntity(xmlNode, document, node));

		} catch (exception* exc) {
			return NULL;
		}

		if (node == NULL) {
			return NULL;
		}

		try {
			contextNode->addNode(node);
			return node;

		} catch (IllegalNodeTypeException* e) {
			return NULL;
		}
	}

	InterfacePoint* PrivateBaseContext::addInterface(
		    string documentId, string nodeId, string xmlInterface) {

		NclDocument* document;
		Node* node;
		InterfacePoint* interfacePoint;

		document = getBaseDocument(documentId);
		if (document == NULL) {
			return NULL;
		}

		node = document->getNode(nodeId);
		if (node == NULL) {
			return NULL;
		}

		try {
			interfacePoint = (InterfacePoint*)(compileEntity(
				    xmlInterface, document, node));

		} catch (exception* ex) {
			return NULL;
		}

		if (interfacePoint == NULL) {
			return NULL;
		}

		if (interfacePoint->instanceOf("Anchor")) {
			node->addAnchor((Anchor*)interfacePoint);
			return interfacePoint;

		} else if (interfacePoint->instanceOf("SwitchPort")) {
			if (node->instanceOf("SwitchNode")) {
				((SwitchNode*)node)->addPort((SwitchPort*)interfacePoint);
				return interfacePoint;
			}

		} else if (interfacePoint->instanceOf("Port")) {
			if (node->instanceOf("ContextNode")) {
				((ContextNode*)node)->addPort((Port*)interfacePoint);
				return interfacePoint;
			}
		}

		return NULL;
	}

	Link* PrivateBaseContext::addLink(
		    string documentId, string compositeId, string xmlLink) {

		NclDocument* document;
		Node* node;
		ContextNode* contextNode;
		Link* link;

		document = getBaseDocument(documentId);
		if (document == NULL) {
			return NULL;
		}

		node = document->getNode(compositeId);
		if ((node == NULL) || !(node->instanceOf("ContextNode"))) {
			return NULL;
		}

		contextNode = (ContextNode*)node;

		try {
			link = (Link*)compileEntity(xmlLink, document, node);

		} catch (exception* exc) {
			return NULL;
		}

		if (link == NULL) {
			return NULL;
		}

		contextNode->addLink(link);
		return link;
	}

	PrivateBase* PrivateBaseContext::getPrivateBase() {
		return privateBase;
	}
}
}
}
}
}
