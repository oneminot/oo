/**************************************************************
 * 
 * Licensed to the Apache Software Foundation (ASF) under one
 * or more contributor license agreements.  See the NOTICE file
 * distributed with this work for additional information
 * regarding copyright ownership.  The ASF licenses this file
 * to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance
 * with the License.  You may obtain a copy of the License at
 * 
 *   http://www.apache.org/licenses/LICENSE-2.0
 * 
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 * KIND, either express or implied.  See the License for the
 * specific language governing permissions and limitations
 * under the License.
 * 
 *************************************************************/



#ifndef _SVDLAYER_HXX
#define _SVDLAYER_HXX

#include <bf_svtools/bf_solar.h>

#ifndef _STRING_HXX //autogen
#include <tools/string.hxx>
#endif

#ifndef _STREAM_HXX //autogen
#include <tools/stream.hxx>
#endif

#ifndef _SVDSOB_HXX //autogen
#include <bf_svx/svdsob.hxx>
#endif

#ifndef _SVDTYPES_HXX
#include <bf_svx/svdtypes.hxx> // fuer typedef SdrLayerID
#endif
namespace binfilter {

class SdrModel;
class SdrLayerSet;

class SdrLayer {
friend class SdrLayerSet;
friend class SdrLayerAdmin;
protected:
	String     aName;
	SdrModel*  pModel; // zum Broadcasten
	UINT16     nType;  // 0=Userdefined,1=Standardlayer
	SdrLayerID nID;
protected:
	SdrLayer(SdrLayerID nNewID, const String& rNewName)       { nID=nNewID; aName=rNewName; nType=0; pModel=NULL; }
	void SetID(SdrLayerID nNewID)                             { nID=nNewID; }
public:
	SdrLayer(): pModel(NULL),nType(0),nID(0)                  {}
	void          SetName(const String& rNewName);
	const String& GetName() const                             { return aName; }
	SdrLayerID    GetID() const                               { return nID; }
	void          SetModel(SdrModel* pNewModel)               { pModel=pNewModel; }
	SdrModel*     GetModel() const                            { return pModel; }
	// Einem SdrLayer kann man sagen dass er ein (der) Standardlayer sein soll.
	// Es wird dann laenderspeziefisch der passende Name gesetzt. SetName()
	// setzt das Flag "StandardLayer" ggf. zurueck auf "Userdefined".
	FASTBOOL      IsStandardLayer() const                     { return nType==1; }
	friend SvStream& operator>>(SvStream& rIn, SdrLayer& rLayer);
	friend SvStream& operator<<(SvStream& rOut, const SdrLayer& rLayer);
};

////////////////////////////////////////////////////////////////////////////////////////////////////

// Der Layerset merkt sich eine Menge von LayerID's.
// Wird die ID eines zum Set gehoerigen Layer geaendert, so gehoert
// dieser damit nichtmehr zum Set ...
// Beim Einblenden eines Layerset in einer View werden alle Member-
// Layer sichtbar und alle Excluded-Layer unsichtbat geschaltet.
// alle anderen Layer bleiben unberuehrt.
class SdrLayerSet {
friend class SdrLayerAdmin;
friend class SdrView;
protected:
	//SdrLayerAdmin& rAd; // Admin, um Layernamen herauszufinden, ...
	String    aName;
	SetOfByte aMember;
	SetOfByte aExclude;
	SdrModel* pModel; // zum Broadcasten
protected:
	// Broadcasting ueber's Model und setzen des Modified-Flags
public:
	SdrLayerSet(): pModel(NULL) {}
	SdrLayerSet(const String& rNewName): aName(rNewName), pModel(NULL) {}
	void            SetModel(SdrModel* pNewModel)          { pModel=pNewModel; }
//    void            AddAll()                               { aMember.SetAll(); }
//    void            ExcludeAll()                           { aExclude.SetAll(); }
	friend SvStream& operator>>(SvStream& rIn, SdrLayerSet& rSet);
	friend SvStream& operator<<(SvStream& rOut, const SdrLayerSet& rSet);
};

// Beim Aendern von Layerdaten muss man derzeit
// noch selbst das Modify-Flag am Model setzen.
#define SDRLAYER_MAXCOUNT 255
class SdrLayerAdmin {
friend class SdrView;
friend class SdrModel;
friend class SdrPage;
//friend class MyScr; // debug
protected:
	Container      aLayer;
	Container      aLSets;
	SdrLayerAdmin* pParent; // Der Admin der Seite kennt den Admin des Docs
	SdrModel*      pModel; // zum Broadcasten
	String         aControlLayerName;
protected:
	// Eine noch nicht verwendete LayerID raussuchen. Sind bereits alle
	// verbraucht, so gibt's 'ne 0. Wer sicher gehen will, muss vorher
	// GetLayerCount()<SDRLAYER_MAXCOUNT abfragen, denn sonst sind alle
	// vergeben.
	SdrLayerID           GetUniqueLayerID() const;
	// Broadcasting ueber's Model und setzen des Modified-Flags
	void                 Broadcast(FASTBOOL bLayerSet) const;
public:
	SdrLayerAdmin(SdrLayerAdmin* pNewParent=NULL);
	SdrLayerAdmin(const SdrLayerAdmin& rSrcLayerAdmin);
	~SdrLayerAdmin();
	SdrLayerAdmin*       GetParent() const                                           { return pParent; }
	void                 SetParent(SdrLayerAdmin* pNewParent)                        { pParent=pNewParent; }
	void                 SetModel(SdrModel* pNewModel);
	SdrModel*            GetModel() const                                            { return pModel; }
	void                 InsertLayer(SdrLayer* pLayer, USHORT nPos=0xFFFF)           { aLayer.Insert(pLayer,nPos); pLayer->SetModel(pModel); Broadcast(FALSE); }
	// Alle Layer loeschen
	void               ClearLayer();
	// Neuer Layer wird angelegt und eingefuegt
	SdrLayer*          NewLayer(const String& rName, USHORT nPos=0xFFFF);
	void               DeleteLayer(SdrLayer* pLayer)                                 { aLayer.Remove(pLayer); delete pLayer; Broadcast(FALSE); }
	// Neuer Layer, Name wird aus der Resource geholt

	// Iterieren ueber alle Layer
	USHORT             GetLayerCount() const                                         { return USHORT(aLayer.Count()); }
	SdrLayer*          GetLayer(USHORT i)                                            { return (SdrLayer*)(aLayer.GetObject(i)); }
	const SdrLayer*    GetLayer(USHORT i) const                                      { return (SdrLayer*)(aLayer.GetObject(i)); }


	SdrLayer*          GetLayer(const String& rName, FASTBOOL bInherited)            { return (SdrLayer*)(((const SdrLayerAdmin*)this)->GetLayer(rName,bInherited)); }
	const SdrLayer*    GetLayer(const String& rName, FASTBOOL bInherited) const;
		  SdrLayerID   GetLayerID(const String& rName, FASTBOOL bInherited) const;
		  SdrLayer*    GetLayerPerID(USHORT nID)                                     { return (SdrLayer*)(((const SdrLayerAdmin*)this)->GetLayerPerID(nID)); }
	const SdrLayer*    GetLayerPerID(USHORT nID) const;

	void               InsertLayerSet(SdrLayerSet* pSet, USHORT nPos=0xFFFF)         { aLSets.Insert(pSet,nPos); pSet->SetModel(pModel); Broadcast(TRUE); }
	// Alle LayerSets loeschen
	void               ClearLayerSets();
	// Neuer Layerset wird angelegt und eingefuegt
	void               DeleteLayerSet(SdrLayerSet* pSet)                             { aLSets.Remove(pSet); delete pSet; Broadcast(TRUE); }
	// Iterieren ueber alle LayerSets
	USHORT             GetLayerSetCount() const                                      { return USHORT(aLSets.Count()); }
	SdrLayerSet*       GetLayerSet(USHORT i)                                         { return (SdrLayerSet*)(aLSets.GetObject(i)); }
	const SdrLayerSet* GetLayerSet(USHORT i) const                                   { return (SdrLayerSet*)(aLSets.GetObject(i)); }


	void     	       SetControlLayerName(const String& rNewName) { aControlLayerName=rNewName; }
	const String& 	   GetControlLayerName() const                 { return aControlLayerName; }
};

/*
Anmerkung zu den Layer - Gemischt symbolisch/ID-basierendes Interface
	Einen neuen Layer macht man sich mit:
	  pLayerAdmin->NewLayer("Der neue Layer");
	Der Layer wird dann automatisch an das Ende der Liste angehaengt.
	Entsprechdes gilt fuer Layersets gleichermassen.
	Das Interface am SdrLayerSet basiert auf LayerID's. Die App muss sich
	dafuer am SdrLayerAdmin eine ID abholen:
		SdrLayerID nLayerID=pLayerAdmin->GetLayerID("Der neue Layer");
	Wird der Layer nicht gefunden, so liefert die Methode SDRLAYER_NOTFOUND
	zurueck. Die Methoden mit ID-Interface fangen diesen Wert jedoch i.d.R
	sinnvoll ab.
	Hat man nicht nur den Namen, sondern gar einen SdrLayer*, so kann man
	sich die ID natuerlich wesentlich schneller direkt vom Layer abholen.
bInherited:
	TRUE: Wird der Layer/LayerSet nicht gefunden, so wird im Parent-LayerAdmin
		  nachgesehen, ob es dort einen entsprechende Definition gibt.
	FALSE: Es wird nur dieser LayerAdmin durchsucht.
	Jeder LayerAdmin einer Seite hat einen Parent-LayerAdmin, n�mlich den des
	Model. Das Model selbst hat keinen Parent.
*/

}//end of namespace binfilter
#endif //_SVDLAYER_HXX

