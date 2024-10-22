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



// MARKER(update_precomp.py): autogen include statement, do not remove
#include "precompiled_svx.hxx"
#include <editeng/eeitem.hxx>

#include "svdibrow.hxx"

#ifndef _STDLIB_H
#include <stdlib.h>
#define _STDLIB_H
#endif

#include "svx/svditext.hxx"
#include <editeng/flditem.hxx>
#include <editeng/editdata.hxx>
#include <svx/svdpool.hxx>
#include <svx/svdattr.hxx>
#include <svx/svdattrx.hxx>
#include <svx/svdview.hxx>
#include <svx/xenum.hxx>
#include <svx/xlineit0.hxx>
#include <svx/xlnstwit.hxx>
#include <svx/xlnedwit.hxx>
#include <svx/xfillit0.hxx>
#include <svx/xflbmtit.hxx>
#include <svx/xtextit0.hxx>
#include <svx/xflbstit.hxx>
#include <svx/xflbtoxy.hxx>
#include <svx/xftshit.hxx>
#include <editeng/colritem.hxx>


#include "editeng/fontitem.hxx"
#include <editeng/fhgtitem.hxx>

#include <editeng/charscaleitem.hxx>
#include <svl/whiter.hxx>
#include <svl/flagitem.hxx>
#include <svl/ptitem.hxx>
#include <svl/rectitem.hxx>

#include <svl/rngitem.hxx>
#include <svx/sdrpaintwindow.hxx>

////////////////////////////////////////////////////////////////////////////////////////////////////

#define ITEMBROWSER_WHICHCOL_ID 1
#define ITEMBROWSER_STATECOL_ID 2
#define ITEMBROWSER_TYPECOL_ID  3
#define ITEMBROWSER_NAMECOL_ID  4
#define ITEMBROWSER_VALUECOL_ID 5

enum ItemType {
	ITEM_DONTKNOW, ITEM_BYTE, ITEM_INT16, ITEM_UINT16, ITEM_INT32, ITEM_UINT32,
	ITEM_ENUM, ITEM_BOOL, ITEM_FLAG, ITEM_STRING, ITEM_POINT, ITEM_RECT, ITEM_RANGE, ITEM_LRANGE,
	ITEM_FRACTION,
	ITEM_XCOLOR,
	ITEM_COLOR,
	ITEM_FONT, ITEM_FONTHEIGHT, ITEM_FONTWIDTH, ITEM_FIELD
};

class ImpItemListRow
{
public:
	XubString					aName;
	XubString					aValue;
	SfxItemState				eState;
	sal_uInt16						nWhichId;

	TypeId						pType;
	ItemType					eItemType;

	sal_Int32						nVal;
	sal_Int32						nMin;
	sal_Int32						nMax;

	sal_Bool						bComment;
	sal_Bool						bIsNum;
	sal_Bool						bCanNum;

public:
	ImpItemListRow()
	:	eState(SFX_ITEM_UNKNOWN),
		nWhichId(0),
		pType(NULL),
		eItemType(ITEM_DONTKNOW),
		nVal(0),
		nMin(0),
		nMax(0),
		bComment(sal_False),
		bIsNum(sal_False),
		bCanNum(sal_False)
	{}

	XubString GetItemTypeStr() const;
	sal_Bool operator==(const ImpItemListRow& rEntry) const;
	sal_Bool operator!=(const ImpItemListRow& rEntry) const { return !operator==(rEntry); }
};

XubString ImpItemListRow::GetItemTypeStr() const
{
	XubString aStr;

	switch(eItemType)
	{
		case ITEM_BYTE		: aStr.AppendAscii("Byte");		break;
		case ITEM_INT16		: aStr.AppendAscii("Int16");	break;
		case ITEM_UINT16	: aStr.AppendAscii("UInt16");	break;
		case ITEM_INT32		: aStr.AppendAscii("Int32");	break;
		case ITEM_UINT32	: aStr.AppendAscii("UInt32");	break;
		case ITEM_ENUM		: aStr.AppendAscii("Enum");		break;
		case ITEM_BOOL		: aStr.AppendAscii("Bool");		break;
		case ITEM_FLAG		: aStr.AppendAscii("Flag");		break;
		case ITEM_STRING	: aStr.AppendAscii("String");	break;
		case ITEM_POINT		: aStr.AppendAscii("Point");	break;
		case ITEM_RECT		: aStr.AppendAscii("Rectangle");break;
		case ITEM_RANGE		: aStr.AppendAscii("Range");	break;
		case ITEM_LRANGE	: aStr.AppendAscii("LRange");	break;
		case ITEM_FRACTION	: aStr.AppendAscii("Fraction");	break;
		case ITEM_XCOLOR	: aStr.AppendAscii("XColor");	break;
		case ITEM_COLOR		: aStr.AppendAscii("Color");	break;
		case ITEM_FONT		: aStr.AppendAscii("Font");		break;
		case ITEM_FONTHEIGHT:aStr.AppendAscii("FontHeight");break;
		case ITEM_FONTWIDTH	:aStr.AppendAscii("FontWidth");	break;
		case ITEM_FIELD		:aStr.AppendAscii("Field");		break;
		default: break;
	}

	return aStr;
}

sal_Bool ImpItemListRow::operator==(const ImpItemListRow& rEntry) const
{
	return (aName.Equals(rEntry.aName)
		&& aValue.Equals(rEntry.aValue)
		&& eState==rEntry.eState
		&& nWhichId==rEntry.nWhichId
		&& bComment==rEntry.bComment
		&& bIsNum==rEntry.bIsNum
		&& bCanNum==rEntry.bCanNum
		&& pType==rEntry.pType
		&& eItemType==rEntry.eItemType
		&& nVal==rEntry.nVal
		&& nMin==rEntry.nMin
		&& nMax==rEntry.nMax);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

class ImpItemEdit: public Edit
{
	_SdrItemBrowserControl*		pBrowse;

public:
	ImpItemEdit(Window* pParent, _SdrItemBrowserControl* pBrowse_, WinBits nBits=0)
	:	Edit(pParent, nBits),
		pBrowse(pBrowse_)
	{}

	virtual ~ImpItemEdit();
	virtual void KeyInput(const KeyEvent& rEvt);
};

__EXPORT ImpItemEdit::~ImpItemEdit()
{
}

void __EXPORT ImpItemEdit::KeyInput(const KeyEvent& rKEvt)
{
	_SdrItemBrowserControl* pBrowseMerk = pBrowse;

	sal_uInt16 nKeyCode(rKEvt.GetKeyCode().GetCode() + rKEvt.GetKeyCode().GetModifier());

	if(nKeyCode == KEY_RETURN)
	{
		pBrowseMerk->EndChangeEntry();
		pBrowseMerk->GrabFocus();
	}
	else if(nKeyCode == KEY_ESCAPE)
	{
		pBrowseMerk->BrkChangeEntry();
		pBrowseMerk->GrabFocus();
	}
	else if(nKeyCode == KEY_UP || nKeyCode == KEY_DOWN)
	{
		pBrowseMerk->EndChangeEntry();
		pBrowseMerk->GrabFocus();
		pBrowseMerk->KeyInput(rKEvt);
	}
	else
		Edit::KeyInput(rKEvt);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

#define MYBROWSEMODE (BROWSER_THUMBDRAGGING|BROWSER_KEEPHIGHLIGHT|BROWSER_NO_HSCROLL|BROWSER_HIDECURSOR)

_SdrItemBrowserControl::_SdrItemBrowserControl(Window* pParent, WinBits nBits):
	BrowseBox(pParent,nBits,MYBROWSEMODE),
	aList(1024,16,16)
{
	ImpCtor();
}

__EXPORT _SdrItemBrowserControl::~_SdrItemBrowserControl()
{
	if(pEditControl)
		delete pEditControl;

	if(pAktChangeEntry)
		delete pAktChangeEntry;

	Clear();
}

void _SdrItemBrowserControl::ImpCtor()
{
	pEditControl = NULL;
	pAktChangeEntry = NULL;
	nLastWhichOfs = 0;
	nLastWhich = 0;
	nLastWhichOben = 0;  // not implemented yet
	nLastWhichUnten = 0; // not implemented yet
	bWhichesButNames = sal_False;
	bDontHideIneffectiveItems = sal_False;
	bDontSortItems = sal_False;
	bShowWhichIds = sal_False;
	bShowRealValues = sal_False;
	bShowWhichIds = sal_True;   // not implemented yet
	bShowRealValues = sal_True; // not implemented yet

	rtl_TextEncoding aTextEncoding = gsl_getSystemTextEncoding();

	InsertDataColumn(
		ITEMBROWSER_WHICHCOL_ID,
		String("Which", aTextEncoding),
		GetTextWidth(String(" Which ", aTextEncoding)) + 2);
	InsertDataColumn(
		ITEMBROWSER_STATECOL_ID,
		String("State", aTextEncoding),
		Max(GetTextWidth(String(" State ", aTextEncoding)) + 2 ,
			GetTextWidth(String("DontCare", aTextEncoding)) + 2));
	InsertDataColumn(
		ITEMBROWSER_TYPECOL_ID ,
		String("Type", aTextEncoding),
		GetTextWidth(String(" Type_ ", aTextEncoding)) + 2);
	InsertDataColumn(
		ITEMBROWSER_NAMECOL_ID ,
		String("Name", aTextEncoding),
		150);
	InsertDataColumn(
		ITEMBROWSER_VALUECOL_ID,
		String("Value", aTextEncoding),
		GetTextWidth(String("12345678901234567890", aTextEncoding)));
	SetDataRowHeight(
		GetTextHeight());

	long nWdt=GetColumnWidth(ITEMBROWSER_WHICHCOL_ID)+
			  GetColumnWidth(ITEMBROWSER_STATECOL_ID)+
			  GetColumnWidth(ITEMBROWSER_TYPECOL_ID )+
			  GetColumnWidth(ITEMBROWSER_NAMECOL_ID )+
			  GetColumnWidth(ITEMBROWSER_VALUECOL_ID);

	long nHgt=GetTitleHeight()+16*GetDataRowHeight();

	SetOutputSizePixel(Size(nWdt,nHgt));
}

void _SdrItemBrowserControl::Clear()
{
	sal_uIntPtr nAnz=aList.Count();
	for (sal_uIntPtr nNum=0; nNum<nAnz; nNum++) {
		delete ImpGetEntry(nNum);
	}
	aList.Clear();
	BrowseBox::Clear();
}

long __EXPORT _SdrItemBrowserControl::GetRowCount() const
{
	return aList.Count();
}

sal_Bool __EXPORT _SdrItemBrowserControl::SeekRow(long nRow)
{
	nAktPaintRow=nRow;
	return sal_True;
}

String _SdrItemBrowserControl::GetCellText(long _nRow, sal_uInt16 _nColId) const
{
	String sRet;
	if ( _nRow >= 0 && _nRow < (sal_Int32)aList.Count() )
	{
		ImpItemListRow* pEntry = ImpGetEntry(_nRow);
		if ( pEntry )
		{
			if ( pEntry->bComment )
			{
				if (_nColId == ITEMBROWSER_NAMECOL_ID)
					sRet = pEntry->aName;
			}
			else
			{
				rtl_TextEncoding aTextEncoding = gsl_getSystemTextEncoding();

				sRet = XubString("???", aTextEncoding);
				switch (_nColId)
				{
					case ITEMBROWSER_WHICHCOL_ID:
						sRet = UniString::CreateFromInt32(pEntry->nWhichId); break;
					case ITEMBROWSER_STATECOL_ID:
					{
						switch (pEntry->eState)
						{
							case SFX_ITEM_UNKNOWN : sRet=String("Uknown", aTextEncoding);   break;
							case SFX_ITEM_DISABLED: sRet=String("Disabled", aTextEncoding); break;
							case SFX_ITEM_DONTCARE: sRet=String("DontCare", aTextEncoding); break;
							case SFX_ITEM_SET     : sRet=String("Set", aTextEncoding);      break;
							case SFX_ITEM_DEFAULT : sRet=String("Default", aTextEncoding);  break;
						} // switch
					} break;
					case ITEMBROWSER_TYPECOL_ID: sRet = pEntry->GetItemTypeStr(); break;
					case ITEMBROWSER_NAMECOL_ID: sRet = pEntry->aName; break;
					case ITEMBROWSER_VALUECOL_ID: sRet = pEntry->aValue; break;
				} // switch
			}
		}
	}
	return sRet;
}

void __EXPORT _SdrItemBrowserControl::PaintField(OutputDevice& rDev, const Rectangle& rRect, sal_uInt16 nColumnId) const
{
	if (nAktPaintRow<0 || (sal_uIntPtr)nAktPaintRow>=aList.Count()) {
		return;
	}
	Rectangle aR(rRect);
	aR.Bottom()++;
	ImpItemListRow* pEntry=ImpGetEntry(nAktPaintRow);
	if (pEntry->bComment)
	{
		if (nColumnId==ITEMBROWSER_NAMECOL_ID)
		{
			rDev.SetLineColor();
			rDev.SetFillColor( Color( COL_LIGHTGRAY ) );
			aR.Left()=0;
			aR.Right()=rDev.GetOutputSize().Width();
			rDev.DrawRect(aR);
			rDev.DrawText(rRect.TopLeft(),pEntry->aName);
		}
	} else {
		rDev.SetClipRegion(aR);
		rDev.DrawText(aR.TopLeft(),GetCellText(nAktPaintRow,nColumnId));
		rDev.SetClipRegion();
	}
}

sal_uIntPtr _SdrItemBrowserControl::GetCurrentPos() const
{
	sal_uIntPtr nRet=CONTAINER_ENTRY_NOTFOUND;
	if (GetSelectRowCount()==1) {
		long nPos=((BrowseBox*)this)->FirstSelectedRow();
		if (nPos>=0 && (sal_uIntPtr)nPos<aList.Count()) {
			nRet=(sal_uIntPtr)nPos;
		}
	}
	return nRet;
}

sal_uInt16 _SdrItemBrowserControl::GetCurrentWhich() const
{
	sal_uInt16 nRet=0;
	sal_uIntPtr nPos=GetCurrentPos();
	if (nPos!=CONTAINER_ENTRY_NOTFOUND) {
		nRet=ImpGetEntry(nPos)->nWhichId;
	}
	return nRet;
}

void __EXPORT _SdrItemBrowserControl::DoubleClick(const BrowserMouseEvent&)
{
	sal_uIntPtr nPos=GetCurrentPos();
	if (nPos!=CONTAINER_ENTRY_NOTFOUND) {
		BegChangeEntry(nPos);
	}
}

void __EXPORT _SdrItemBrowserControl::KeyInput(const KeyEvent& rKEvt)
{
	sal_uInt16 nKeyCode=rKEvt.GetKeyCode().GetCode()+rKEvt.GetKeyCode().GetModifier();
	FASTBOOL bAusgewertet=sal_False;
	sal_uIntPtr nPos=GetCurrentPos();
	if (nPos!=CONTAINER_ENTRY_NOTFOUND) {
		if (nKeyCode==KEY_RETURN) {
			if (BegChangeEntry(nPos)) bAusgewertet=sal_True;
		} else if (nKeyCode==KEY_ESCAPE) {
			// ...
		} else if (rKEvt.GetKeyCode().GetModifier()==KEY_SHIFT+KEY_MOD1+KEY_MOD2) { // Strg
			if (nKeyCode==KEY_SHIFT+KEY_MOD1+KEY_MOD2+KEY_W) {
				bWhichesButNames=!bWhichesButNames;
				SetDirty();
			}
			if (nKeyCode==KEY_SHIFT+KEY_MOD1+KEY_MOD2+KEY_I) {
				bDontHideIneffectiveItems=!bDontHideIneffectiveItems;
				SetDirty();
			}
			if (nKeyCode==KEY_SHIFT+KEY_MOD1+KEY_MOD2+KEY_S) {
				bDontSortItems=!bDontSortItems;
				SetDirty();
			}
		}
	}
	if (!bAusgewertet) BrowseBox::KeyInput(rKEvt);
}

void _SdrItemBrowserControl::SetDirty()
{
	aSetDirtyHdl.Call(this);
}

Rectangle _SdrItemBrowserControl::GetFieldCharacterBounds(sal_Int32 /*_nRow*/,sal_Int32 /*_nColumnPos*/,sal_Int32 /*nIndex*/)
{
    // no accessibility implementation required
    return Rectangle();
}

sal_Int32 _SdrItemBrowserControl::GetFieldIndexAtPoint(sal_Int32 /*_nRow*/,sal_Int32 /*_nColumnPos*/,const Point& /*_rPoint*/)
{
    // no accessibility implementation required
    return -1;
}

void __EXPORT _SdrItemBrowserControl::Select()
{
	EndChangeEntry();
	BrowseBox::Select();
	ImpSaveWhich();
}

void _SdrItemBrowserControl::ImpSaveWhich()
{
	sal_uInt16 nWh=GetCurrentWhich();
	if (nWh!=0) {
		long nPos=GetCurrentPos();
		long nTop=GetTopRow();
		long nBtm=GetTopRow()+GetVisibleRows()+1;
		nLastWhich=nWh;
		nLastWhichOfs=nPos-nTop;
		if (nTop<0) nTop=0;
		if (nBtm>=(long)aList.Count()) nBtm=aList.Count()-1;
		nLastWhichOben=ImpGetEntry(nTop)->nWhichId;
		nLastWhichUnten=ImpGetEntry(nBtm)->nWhichId;
	}
}

void _SdrItemBrowserControl::ImpRestoreWhich()
{
	if (nLastWhich!=0) {
		FASTBOOL bFnd=sal_False;
		sal_uInt16 nBestMinWh=0,nBestMaxWh=0xFFFF;       // not implemented yet
		sal_uIntPtr nBestMinPos=0,nBestMaxPos=0xFFFFFFFF;  // not implemented yet
		sal_uIntPtr nAnz=aList.Count();
		sal_uIntPtr nNum;
		for (nNum=0; nNum<nAnz && !bFnd; nNum++) {
			ImpItemListRow* pEntry=ImpGetEntry(nNum);
			if (!pEntry->bComment) {
				sal_uInt16 nWh=pEntry->nWhichId;
				if (nWh==nLastWhich) bFnd=sal_True;
				else if (nWh<nLastWhich && nWh>nBestMinWh) nBestMinPos=nNum;
				else if (nWh>nLastWhich && nWh<nBestMaxWh) nBestMaxPos=nNum;
			}
		}
		if (bFnd) {
			long nPos=nNum-1;
			long nWhichOfs=nPos-GetTopRow();
			if (nWhichOfs!=nLastWhichOfs) {
				ScrollRows(nWhichOfs-nLastWhichOfs);
			}
			GoToRow(nPos);
		}
	}
}

FASTBOOL _SdrItemBrowserControl::BegChangeEntry(sal_uIntPtr nPos)
{
	BrkChangeEntry();
	FASTBOOL bRet=sal_False;
	ImpItemListRow* pEntry=ImpGetEntry(nPos);
	if (pEntry!=NULL && !pEntry->bComment) {
		SetMode(MYBROWSEMODE & ~BROWSER_KEEPHIGHLIGHT);
		pEditControl=new ImpItemEdit(&GetDataWindow(),this,0/*|WB_BORDER|WB_3DLOOK*/);
		Rectangle aRect(GetFieldRectPixel(nPos,ITEMBROWSER_VALUECOL_ID,sal_False));
		aRect.Left()+=2; // Kleiner Offset fuer's Edit, damit's pixelgenau stimmt
		aRect.Right()--;
		pEditControl->SetPosSizePixel(aRect.TopLeft(),aRect.GetSize());
		pEditControl->SetText(pEntry->aValue);
		pEditControl->SetBackground( Wallpaper(Color(COL_LIGHTGRAY)));
		Font aFont(pEditControl->GetFont());
		aFont.SetFillColor(Color(COL_LIGHTGRAY));
		pEditControl->SetFont(aFont);
		pEditControl->Show();
		pEditControl->GrabFocus();
		pEditControl->SetSelection(Selection(SELECTION_MIN,SELECTION_MAX));
		Window* pParent=GetParent();
		aWNamMerk=pParent->GetText();
		XubString aNeuNam(aWNamMerk);
		aNeuNam += sal_Unicode(' ');
		aNeuNam += pEntry->GetItemTypeStr();
		if (pEntry->bCanNum) {
			aNeuNam.AppendAscii(": ");
			aNeuNam += UniString::CreateFromInt32(pEntry->nMin);
			aNeuNam.AppendAscii("..");
			aNeuNam += UniString::CreateFromInt32(pEntry->nMax);
		}
		aNeuNam.AppendAscii(" - Type 'del' to reset to default.");
		pParent->SetText(aNeuNam);
		pAktChangeEntry=new ImpItemListRow(*pEntry);
		bRet=sal_True;
	}
	return bRet;
}

FASTBOOL _SdrItemBrowserControl::EndChangeEntry()
{
	FASTBOOL bRet=sal_False;
	if (pEditControl!=NULL) {
		aEntryChangedHdl.Call(this);
		delete pEditControl;
		pEditControl=NULL;
		delete pAktChangeEntry;
		pAktChangeEntry=NULL;
		Window* pParent=GetParent();
		pParent->SetText(aWNamMerk);
		SetMode(MYBROWSEMODE);
		bRet=sal_True;
	}
	return bRet;
}

void _SdrItemBrowserControl::BrkChangeEntry()
{
	if (pEditControl!=NULL) {
		delete pEditControl;
		pEditControl=NULL;
		delete pAktChangeEntry;
		pAktChangeEntry=NULL;
		Window* pParent=GetParent();
		pParent->SetText(aWNamMerk);
		SetMode(MYBROWSEMODE);
	}
}

void _SdrItemBrowserControl::ImpSetEntry(const ImpItemListRow& rEntry, sal_uIntPtr nEntryNum)
{
	ImpItemListRow* pAktEntry=ImpGetEntry(nEntryNum);
	if (pAktEntry==NULL) {
		aList.Insert(new ImpItemListRow(rEntry),CONTAINER_APPEND);
		RowInserted(nEntryNum);
	} else if (*pAktEntry!=rEntry) {
		FASTBOOL bStateDiff=rEntry.eState!=pAktEntry->eState;
		FASTBOOL bValueDiff=!rEntry.aValue.Equals(pAktEntry->aValue);
		FASTBOOL bAllDiff=sal_True;
		if (bStateDiff || bValueDiff) {
			// Checken, ob nur State und/oder Value geaendert
			ImpItemListRow aTest(rEntry);
			aTest.eState=pAktEntry->eState;
			aTest.aValue=pAktEntry->aValue;
			if (aTest==*pAktEntry) bAllDiff=sal_False;
		}
		*pAktEntry=rEntry;
		if (bAllDiff) {
			RowModified(nEntryNum);
		} else {
			if (bStateDiff) RowModified(nEntryNum,ITEMBROWSER_STATECOL_ID);
			if (bValueDiff) RowModified(nEntryNum,ITEMBROWSER_VALUECOL_ID);
		}
	}
}

FASTBOOL ImpGetItem(const SfxItemSet& rSet, sal_uInt16 nWhich, const SfxPoolItem*& rpItem)
{
	SfxItemState eState=rSet.GetItemState(nWhich,sal_True,&rpItem);
	if (eState==SFX_ITEM_DEFAULT) {
		rpItem=&rSet.Get(nWhich);
	}
	return (eState==SFX_ITEM_DEFAULT || eState==SFX_ITEM_SET) && rpItem!=NULL;
}

FASTBOOL IsItemIneffective(sal_uInt16 nWhich, const SfxItemSet* pSet, sal_uInt16& rIndent)
{
	rIndent=0;
	if (pSet==NULL) return sal_False;
	const SfxPoolItem* pItem=NULL;
	FASTBOOL bRet=sal_False;
	switch (nWhich) {
		case XATTR_LINEDASH         :
		case XATTR_LINEWIDTH        :
		case XATTR_LINECOLOR        :
		case XATTR_LINESTART        :
		case XATTR_LINEEND          :
		case XATTR_LINESTARTWIDTH   :
		case XATTR_LINEENDWIDTH     :
		case XATTR_LINESTARTCENTER  :
		case XATTR_LINEENDCENTER    :
		case XATTR_LINETRANSPARENCE : {
			rIndent=1;
			if (ImpGetItem(*pSet,XATTR_LINESTYLE,pItem)) {
				XLineStyle eLineStyle=((const XLineStyleItem*)pItem)->GetValue();
				if (eLineStyle==XLINE_NONE) return sal_True;
				if (eLineStyle!=XLINE_DASH && nWhich==XATTR_LINEDASH) return sal_True;
			}
			if (nWhich==XATTR_LINESTART || nWhich==XATTR_LINESTARTCENTER) {
				rIndent=2;
				if (ImpGetItem(*pSet,XATTR_LINESTARTWIDTH,pItem)) {
					sal_Int32 nWdt=((const XLineStartWidthItem*)pItem)->GetValue();
					if (nWdt==0) return sal_True;
				}
			}
			if (nWhich==XATTR_LINEEND || nWhich==XATTR_LINEENDCENTER) {
				rIndent=2;
				if (ImpGetItem(*pSet,XATTR_LINEENDWIDTH,pItem)) {
					sal_Int32 nWdt=((const XLineEndWidthItem*)pItem)->GetValue();
					if (nWdt==0) return sal_True;
				}
			}
		} break;
		case XATTR_FILLCOLOR           : /*nur bei Style=Color*/
		case XATTR_FILLGRADIENT        : /*nur bei Style=Gradient*/
		case XATTR_FILLHATCH           : /*nur bei Style=Hatch*/
		case XATTR_FILLTRANSPARENCE    : /*nur bei Style=Color*/
		case XATTR_GRADIENTSTEPCOUNT   : /*nur bei Style=Gradient*/
		case XATTR_FILLBACKGROUND	   : /*nur bei Style=Hatch*/
		{
			rIndent=1;
			if (ImpGetItem(*pSet,XATTR_FILLSTYLE,pItem)) {
				XFillStyle eFillStyle=((const XFillStyleItem*)pItem)->GetValue();
				if (eFillStyle==XFILL_NONE) return sal_True;
				// Transparenz z.Zt. nur fuer SolidFill
				if (eFillStyle!=XFILL_SOLID && (nWhich==XATTR_FILLCOLOR || nWhich==XATTR_FILLTRANSPARENCE)) return sal_True;
				if (eFillStyle!=XFILL_GRADIENT && (nWhich==XATTR_FILLGRADIENT || nWhich==XATTR_GRADIENTSTEPCOUNT)) return sal_True;
				if (eFillStyle!=XFILL_HATCH && (nWhich==XATTR_FILLHATCH || nWhich==XATTR_FILLBACKGROUND)) return sal_True;
			}
		} break;
		case XATTR_FILLBITMAP          :
		case XATTR_FILLBMP_TILE        :
		case XATTR_FILLBMP_POS         : /* z.Zt. nur wenn TILE=sal_True */
		case XATTR_FILLBMP_SIZEX       : /* nur wenn nicht Stretch */
		case XATTR_FILLBMP_SIZEY       : /* nur wenn nicht Stretch */
		case XATTR_FILLBMP_SIZELOG     : /* nur wenn SIZELOG=sal_False zum ruecksetzen auf sal_True (alt) -> doch noch in Gebrauch */
		case XATTR_FILLBMP_TILEOFFSETX : /* nur wenn TILE=sal_True */
		case XATTR_FILLBMP_TILEOFFSETY : /* nur wenn TILE=sal_True */
		case XATTR_FILLBMP_STRETCH     : /* nur wenn TILE=sal_False */
		case XATTR_FILLBMP_POSOFFSETX  : /* nur wenn TILE=sal_True*/
		case XATTR_FILLBMP_POSOFFSETY  : { /* nur wenn TILE=sal_True*/
			rIndent=1;
			if (ImpGetItem(*pSet,XATTR_FILLSTYLE,pItem)) {
				XFillStyle eFillStyle=((const XFillStyleItem*)pItem)->GetValue();
				if (eFillStyle!=XFILL_BITMAP) return sal_True;
			}
			if (nWhich==XATTR_FILLBITMAP || nWhich==XATTR_FILLBMP_TILE) {
				return sal_False; // immer anwaehlbar
			}
			FASTBOOL bTileTRUE=sal_False;
			FASTBOOL bTileFALSE=sal_False;
			FASTBOOL bStretchTRUE=sal_False;
			FASTBOOL bStretchFALSE=sal_False;
			if (ImpGetItem(*pSet,XATTR_FILLBMP_TILE,pItem)) {
				bTileTRUE=((const XFillBmpTileItem*)pItem)->GetValue();
				bTileFALSE=!bTileTRUE;
			}
			if (ImpGetItem(*pSet,XATTR_FILLBMP_STRETCH,pItem)) {
				bStretchTRUE=((const XFillBmpStretchItem*)pItem)->GetValue();
				bStretchFALSE=!bStretchTRUE;
			}
			// Stretch nicht anwaehlbar, wenn Tile=TRUE
			if (nWhich==XATTR_FILLBMP_STRETCH) return bTileTRUE;
			// und uebrig bleiben 7+1 Item (Unterattribute)
			rIndent=2;
			// Pos (enum) nicht anwaehlbar, wenn Tile=FALSE
			if (nWhich==XATTR_FILLBMP_POS) return bTileFALSE;
			// SizeXY nicht anwaehlbar bei Stretch=TRUE
			if (nWhich==XATTR_FILLBMP_SIZEX || nWhich==XATTR_FILLBMP_SIZEY) {
				return bTileFALSE && bStretchTRUE;
			}
			// 2 Items speziell fuer Tile
			if (nWhich==XATTR_FILLBMP_POSOFFSETX  || nWhich==XATTR_FILLBMP_POSOFFSETY) {
				return bTileFALSE;
			}
			// Noch 2 Items speziell fuer Tile die sich jedoch gegenseitig ausschliessen
			if (nWhich==XATTR_FILLBMP_TILEOFFSETX || nWhich==XATTR_FILLBMP_TILEOFFSETY) {
				if (bTileFALSE) return sal_True;
				sal_uInt16 nX=0,nY=0;
				FASTBOOL bX=sal_False,bY=sal_False;
				if (ImpGetItem(*pSet,XATTR_FILLBMP_TILEOFFSETX,pItem)) {
					nX=((const XFillBmpTileOffsetXItem*)pItem)->GetValue();
					bX=sal_True;
				}
				if (ImpGetItem(*pSet,XATTR_FILLBMP_TILEOFFSETY,pItem)) {
					nY=((const XFillBmpTileOffsetYItem*)pItem)->GetValue();
					bY=sal_True;
				}
				if (nWhich==XATTR_FILLBMP_TILEOFFSETX) {
					if (nX!=0 || !bX) return sal_False;
					if (nY!=0) return sal_True;
				} else {
					if (nY!=0 || !bY) return sal_False;
					if (nX!=0) return sal_True;
				}
			}
			// SizeLog nicht anwaehlbar bei Stretch=TRUE
			// und sonst auch nur wenn es auf SizeLog=sal_False gesetzt ist.
			// -> wohl doch noch in Gebrauch
			// (sal_True ist der statische PoolDefault)
			if (nWhich==XATTR_FILLBMP_SIZELOG) {
				if (bTileFALSE && bStretchTRUE) return sal_True;
			}
		} break;

		case XATTR_FORMTXTADJUST    :
		case XATTR_FORMTXTDISTANCE  :
		case XATTR_FORMTXTSTART     :
		case XATTR_FORMTXTMIRROR    :
		case XATTR_FORMTXTOUTLINE   :
		case XATTR_FORMTXTSHADOW    :
		case XATTR_FORMTXTSHDWCOLOR :
		case XATTR_FORMTXTSHDWXVAL  :
		case XATTR_FORMTXTSHDWYVAL  :
		case XATTR_FORMTXTSTDFORM   :
		case XATTR_FORMTXTHIDEFORM  :
		case XATTR_FORMTXTSHDWTRANSP: {
			rIndent=1;
			if (ImpGetItem(*pSet,XATTR_FORMTXTSTYLE,pItem)) {
				XFormTextStyle eStyle=((const XFormTextStyleItem*)pItem)->GetValue();
				if (eStyle==XFT_NONE) return sal_True;
			}
			if ((nWhich>=XATTR_FORMTXTSHDWCOLOR && nWhich<=XATTR_FORMTXTSHDWYVAL) || nWhich>=XATTR_FORMTXTSHDWTRANSP) {
				rIndent=2;
				if (ImpGetItem(*pSet,XATTR_FORMTXTSHADOW,pItem)) {
					XFormTextShadow eShadow=((const XFormTextShadowItem*)pItem)->GetValue();
					if (eShadow==XFTSHADOW_NONE) return sal_True;
				}
			}
		} break;

		case SDRATTR_SHADOWCOLOR       :
		case SDRATTR_SHADOWXDIST       :
		case SDRATTR_SHADOWYDIST       :
		case SDRATTR_SHADOWTRANSPARENCE:
		case SDRATTR_SHADOW3D          :
		case SDRATTR_SHADOWPERSP       : {
			rIndent=1;
			if (ImpGetItem(*pSet,SDRATTR_SHADOW,pItem)) {
				FASTBOOL bShadow=((const SdrShadowItem*)pItem)->GetValue();
				if (!bShadow) return sal_True;
			}
		} break;

		case SDRATTR_CAPTIONANGLE: {
			rIndent=1;
			if (ImpGetItem(*pSet,SDRATTR_CAPTIONFIXEDANGLE,pItem)) {
				FASTBOOL bFixed=((const SdrCaptionFixedAngleItem*)pItem)->GetValue();
				if (!bFixed) return sal_True;
			}
		} break;
		case SDRATTR_CAPTIONESCREL:
		case SDRATTR_CAPTIONESCABS: {
			rIndent=1;
			if (ImpGetItem(*pSet,SDRATTR_CAPTIONESCISREL,pItem)) {
				FASTBOOL bRel=((const SdrCaptionEscIsRelItem*)pItem)->GetValue();
				if (bRel && nWhich==SDRATTR_CAPTIONESCABS) return sal_True;
				if (!bRel && nWhich==SDRATTR_CAPTIONESCREL) return sal_True;
			}
		} break;
		case SDRATTR_CAPTIONLINELEN: {
			rIndent=1;
			if (ImpGetItem(*pSet,SDRATTR_CAPTIONFITLINELEN,pItem)) {
				FASTBOOL bFit=((const SdrCaptionFitLineLenItem*)pItem)->GetValue();
				if (bFit) return sal_True;
			}
		} break;

		case SDRATTR_TEXT_MINFRAMEHEIGHT:
		case SDRATTR_TEXT_MAXFRAMEHEIGHT: {
			rIndent=1;
			if (ImpGetItem(*pSet,SDRATTR_TEXT_AUTOGROWHEIGHT,pItem)) {
				FASTBOOL bAutoGrow=((const SdrTextAutoGrowHeightItem*)pItem)->GetValue();
				if (!bAutoGrow) return sal_True;
			}
		} break;
		case SDRATTR_TEXT_MINFRAMEWIDTH:
		case SDRATTR_TEXT_MAXFRAMEWIDTH: {
			rIndent=1;
			if (ImpGetItem(*pSet,SDRATTR_TEXT_AUTOGROWWIDTH,pItem)) {
				FASTBOOL bAutoGrow=((const SdrTextAutoGrowWidthItem*)pItem)->GetValue();
				if (!bAutoGrow) return sal_True;
			}
		} break;
		case SDRATTR_TEXT_VERTADJUST:
		case SDRATTR_TEXT_HORZADJUST: {
			if (ImpGetItem(*pSet,SDRATTR_TEXT_FITTOSIZE,pItem)) {
				SdrFitToSizeType eFit=((const SdrTextFitToSizeTypeItem*)pItem)->GetValue();
				if (eFit!=SDRTEXTFIT_NONE) return sal_True;
			}
		} break;

		case SDRATTR_TEXT_ANIDIRECTION  :
		case SDRATTR_TEXT_ANISTARTINSIDE:
		case SDRATTR_TEXT_ANISTOPINSIDE :
		case SDRATTR_TEXT_ANICOUNT      :
		case SDRATTR_TEXT_ANIDELAY      :
		case SDRATTR_TEXT_ANIAMOUNT     : {
			rIndent=1;
			if (ImpGetItem(*pSet,SDRATTR_TEXT_ANIKIND,pItem)) {
				SdrTextAniKind eAniKind=((const SdrTextAniKindItem*)pItem)->GetValue();
				if (eAniKind==SDRTEXTANI_NONE) return sal_True;
				if (eAniKind==SDRTEXTANI_BLINK && (nWhich==SDRATTR_TEXT_ANIDIRECTION || nWhich==SDRATTR_TEXT_ANIAMOUNT)) return sal_True;
				if (eAniKind==SDRTEXTANI_SLIDE && (nWhich==SDRATTR_TEXT_ANISTARTINSIDE || nWhich==SDRATTR_TEXT_ANISTOPINSIDE)) return sal_True;
			}
		} break;

		case SDRATTR_EDGELINEDELTAANZ: return sal_True;
		case SDRATTR_EDGELINE1DELTA:
		case SDRATTR_EDGELINE2DELTA:
		case SDRATTR_EDGELINE3DELTA: {
			if (ImpGetItem(*pSet,SDRATTR_EDGEKIND,pItem)) {
				SdrEdgeKind eKind=((const SdrEdgeKindItem*)pItem)->GetValue();
				if (eKind==SDREDGE_THREELINES) {
					if (nWhich>SDRATTR_EDGELINE2DELTA) return sal_True;
					else return sal_False;
				}
				if (eKind!=SDREDGE_ORTHOLINES && eKind!=SDREDGE_BEZIER) return sal_True;
			}
			if (ImpGetItem(*pSet,SDRATTR_EDGELINEDELTAANZ,pItem)) {
				sal_uInt16 nAnz=((const SdrEdgeLineDeltaAnzItem*)pItem)->GetValue();
				if (nAnz==0) return sal_True;
				if (nAnz==1 && nWhich>SDRATTR_EDGELINE1DELTA) return sal_True;
				if (nAnz==2 && nWhich>SDRATTR_EDGELINE2DELTA) return sal_True;
				if (nAnz==3 && nWhich>SDRATTR_EDGELINE3DELTA) return sal_True;
			}
		} break;

		case SDRATTR_CIRCSTARTANGLE:
		case SDRATTR_CIRCENDANGLE  : {
			rIndent=1;
			if (ImpGetItem(*pSet,SDRATTR_CIRCKIND,pItem)) {
				SdrCircKind eKind=((const SdrCircKindItem*)pItem)->GetValue();
				if (eKind==SDRCIRC_FULL) return sal_True;
			}
		} break;
	} // switch
	return bRet;
}

sal_uInt16 ImpSortWhich(sal_uInt16 nWhich)
{
	switch (nWhich) {
		// Line
		case XATTR_LINESTART            : nWhich=XATTR_LINETRANSPARENCE     ; break;
		case XATTR_LINEEND              : nWhich=XATTR_LINESTARTWIDTH       ; break;
		case XATTR_LINESTARTWIDTH       : nWhich=XATTR_LINESTART            ; break;
		case XATTR_LINEENDWIDTH         : nWhich=XATTR_LINESTARTCENTER      ; break;
		case XATTR_LINESTARTCENTER      : nWhich=XATTR_LINEENDWIDTH         ; break;
		case XATTR_LINEENDCENTER        : nWhich=XATTR_LINEEND              ; break;
		case XATTR_LINETRANSPARENCE     : nWhich=XATTR_LINEENDCENTER        ; break;

		// Fill
		case XATTR_FILLBMP_POS          : nWhich=XATTR_FILLBMP_STRETCH      ; break;
		case XATTR_FILLBMP_SIZEX        : nWhich=XATTR_FILLBMP_POS          ; break;
		case XATTR_FILLBMP_SIZEY        : nWhich=XATTR_FILLBMP_SIZEX        ; break;
		case XATTR_FILLBMP_SIZELOG      : nWhich=XATTR_FILLBMP_SIZEY        ; break;
		case XATTR_FILLBMP_TILEOFFSETX  : nWhich=XATTR_FILLBMP_SIZELOG      ; break;
		case XATTR_FILLBMP_TILEOFFSETY  : nWhich=XATTR_FILLBMP_TILEOFFSETX  ; break;
		case XATTR_FILLBMP_STRETCH      : nWhich=XATTR_FILLBMP_TILEOFFSETY  ; break;

		// Fontwork
		case XATTR_FORMTXTSHADOW        : nWhich=XATTR_FORMTXTSTDFORM       ; break;
		case XATTR_FORMTXTSHDWCOLOR     : nWhich=XATTR_FORMTXTHIDEFORM      ; break;
		case XATTR_FORMTXTSHDWXVAL      : nWhich=XATTR_FORMTXTSHADOW        ; break;
		case XATTR_FORMTXTSHDWYVAL      : nWhich=XATTR_FORMTXTSHDWCOLOR     ; break;
		case XATTR_FORMTXTSTDFORM       : nWhich=XATTR_FORMTXTSHDWXVAL      ; break;
		case XATTR_FORMTXTHIDEFORM      : nWhich=XATTR_FORMTXTSHDWYVAL      ; break;

		// Misc
		case SDRATTR_TEXT_MINFRAMEHEIGHT: nWhich=SDRATTR_TEXT_FITTOSIZE     ; break;
		case SDRATTR_TEXT_AUTOGROWHEIGHT: nWhich=SDRATTR_TEXT_LEFTDIST      ; break;
		case SDRATTR_TEXT_FITTOSIZE     : nWhich=SDRATTR_TEXT_RIGHTDIST     ; break;
		case SDRATTR_TEXT_LEFTDIST      : nWhich=SDRATTR_TEXT_UPPERDIST     ; break;
		case SDRATTR_TEXT_RIGHTDIST     : nWhich=SDRATTR_TEXT_LOWERDIST     ; break;
		case SDRATTR_TEXT_UPPERDIST     : nWhich=SDRATTR_TEXT_AUTOGROWWIDTH ; break;
		case SDRATTR_TEXT_LOWERDIST     : nWhich=SDRATTR_TEXT_MINFRAMEWIDTH ; break;
		case SDRATTR_TEXT_VERTADJUST    : nWhich=SDRATTR_TEXT_MAXFRAMEWIDTH ; break;
		case SDRATTR_TEXT_MAXFRAMEHEIGHT: nWhich=SDRATTR_TEXT_AUTOGROWHEIGHT; break;
		case SDRATTR_TEXT_MINFRAMEWIDTH : nWhich=SDRATTR_TEXT_MINFRAMEHEIGHT; break;
		case SDRATTR_TEXT_MAXFRAMEWIDTH : nWhich=SDRATTR_TEXT_MAXFRAMEHEIGHT; break;
		case SDRATTR_TEXT_AUTOGROWWIDTH : nWhich=SDRATTR_TEXT_HORZADJUST    ; break;
		case SDRATTR_TEXT_HORZADJUST    : nWhich=SDRATTR_TEXT_VERTADJUST    ; break;
	} // switch
	return nWhich;
}

#define INSERTCOMMENT(nStartId,nEndId,aStr) \
    { if (nWhich0<nStartId && nWhich>=nStartId && nWhich<=nEndId) aCommentStr=aStr; }

void _SdrItemBrowserControl::SetAttributes(const SfxItemSet* pSet, const SfxItemSet* p2ndSet)
{
	SetMode(MYBROWSEMODE & ~BROWSER_KEEPHIGHLIGHT);
	if (pSet!=NULL) {
		rtl_TextEncoding aTextEncoding = gsl_getSystemTextEncoding();
		sal_uInt16 nEntryNum=0;
		SfxWhichIter aIter(*pSet);
		const SfxItemPool* pPool=pSet->GetPool();
		sal_uInt16 nWhich0=0;
		sal_uInt16 nWhich=aIter.FirstWhich();
		while (nWhich!=0) {
			// Nun erstmal etwas umsortieren
			// Geht nur, solange keine InvalidItems, d.h. keine Luecken
			// an dieser Stelle im Set sind
			if (!bDontSortItems) nWhich=ImpSortWhich(nWhich);
			SfxItemState eState=pSet->GetItemState(nWhich);
			if (p2ndSet!=NULL) {
				SfxItemState e2ndState=p2ndSet->GetItemState(nWhich);
				if (eState==SFX_ITEM_DEFAULT) eState=SFX_ITEM_DISABLED;
				else if (e2ndState==SFX_ITEM_DEFAULT) eState=SFX_ITEM_DEFAULT;
			}
			if (eState!=SFX_ITEM_DISABLED) {
				const SfxPoolItem& rItem=pSet->Get(nWhich);
				sal_uInt16 nIndent=0;
				if (!HAS_BASE(SfxVoidItem,&rItem) && !HAS_BASE(SfxSetItem,&rItem) && (!IsItemIneffective(nWhich,pSet,nIndent) || bDontHideIneffectiveItems)) {
					XubString aCommentStr;

					INSERTCOMMENT(XATTR_LINE_FIRST,XATTR_LINE_LAST,String("L I N I E", aTextEncoding));
                    INSERTCOMMENT(XATTR_FILL_FIRST,XATTR_FILL_LAST,String("F L � C H E", aTextEncoding));
					INSERTCOMMENT(XATTR_TEXT_FIRST,XATTR_TEXT_LAST,String("F O N T W O R K", aTextEncoding));
					INSERTCOMMENT(SDRATTR_SHADOW_FIRST,SDRATTR_SHADOW_LAST,String("S C H A T T E N", aTextEncoding));
					INSERTCOMMENT(SDRATTR_CAPTION_FIRST,SDRATTR_CAPTION_LAST,String("L E G E N D E", aTextEncoding));
					INSERTCOMMENT(SDRATTR_MISC_FIRST,SDRATTR_MISC_LAST,String("V E R S C H I E D E N E S", aTextEncoding));
					INSERTCOMMENT(SDRATTR_EDGE_FIRST,SDRATTR_EDGE_LAST,String("V E R B I N D E R", aTextEncoding));
					INSERTCOMMENT(SDRATTR_MEASURE_FIRST,SDRATTR_MEASURE_LAST,String("B E M A S S U N G", aTextEncoding));
					INSERTCOMMENT(SDRATTR_CIRC_FIRST,SDRATTR_CIRC_LAST,String("K R E I S", aTextEncoding));
					INSERTCOMMENT(SDRATTR_NOTPERSIST_FIRST,SDRATTR_NOTPERSIST_LAST,String("N O T P E R S I S T", aTextEncoding));
					INSERTCOMMENT(SDRATTR_MOVEX,SDRATTR_VERTSHEARONE,String("Transformationen auf alle Objekte einzeln", aTextEncoding));
					INSERTCOMMENT(SDRATTR_RESIZEXALL,SDRATTR_VERTSHEARALL,String("Transformationen auf alle Objekte gemeinsam", aTextEncoding));
					INSERTCOMMENT(SDRATTR_TRANSFORMREF1X,SDRATTR_TRANSFORMREF2Y,String("View-Referenzpunkte", aTextEncoding));
					INSERTCOMMENT(SDRATTR_GRAF_FIRST,SDRATTR_GRAF_LAST,String("G R A F I K", aTextEncoding));
					INSERTCOMMENT(EE_ITEMS_START,EE_ITEMS_END,String("E D I T  E N G I N E", aTextEncoding));
					INSERTCOMMENT(EE_ITEMS_END+1,EE_ITEMS_END+1,String("... by Joe Merten, JME Engineering Berlin ...", aTextEncoding));

					if(aCommentStr.Len())
					{
						ImpItemListRow aEntry;
						aEntry.bComment=sal_True;
						aEntry.aName=aCommentStr;
						ImpSetEntry(aEntry,nEntryNum);
						nEntryNum++;
					}
					nWhich0=nWhich;
					ImpItemListRow aEntry;
					SdrItemPool::TakeItemName(nWhich, aEntry.aName);
					nIndent*=2;

					while(nIndent > 0)
					{
						aEntry.aName.Insert(sal_Unicode(' '), 0);
						nIndent--;
					}

					aEntry.eState=eState;
					aEntry.nWhichId=nWhich;
					if (!IsInvalidItem(&rItem)) {
						aEntry.pType=rItem.Type();
						aEntry.nMax=0x7FFFFFFF;
						aEntry.nMin=-aEntry.nMax;
						aEntry.nVal=-4711;
						if      (HAS_BASE(SfxByteItem     ,&rItem)) aEntry.eItemType=ITEM_BYTE;
						else if (HAS_BASE(SfxInt16Item    ,&rItem)) aEntry.eItemType=ITEM_INT16;
						else if (HAS_BASE(SfxUInt16Item   ,&rItem)) aEntry.eItemType=ITEM_UINT16;
						else if (HAS_BASE(SfxInt32Item    ,&rItem)) aEntry.eItemType=ITEM_INT32;
						else if (HAS_BASE(SfxUInt32Item   ,&rItem)) aEntry.eItemType=ITEM_UINT32;
						else if (HAS_BASE(SfxEnumItemInterface,&rItem)) aEntry.eItemType=ITEM_ENUM;
						else if (HAS_BASE(SfxBoolItem     ,&rItem)) aEntry.eItemType=ITEM_BOOL;
						else if (HAS_BASE(SfxFlagItem     ,&rItem)) aEntry.eItemType=ITEM_FLAG;
						else if (HAS_BASE(XColorItem      ,&rItem)) aEntry.eItemType=ITEM_XCOLOR;
						else if (HAS_BASE(SfxStringItem   ,&rItem)) aEntry.eItemType=ITEM_STRING;
						else if (HAS_BASE(SfxPointItem    ,&rItem)) aEntry.eItemType=ITEM_POINT;
						else if (HAS_BASE(SfxRectangleItem,&rItem)) aEntry.eItemType=ITEM_RECT;
						else if (HAS_BASE(SfxRangeItem    ,&rItem)) aEntry.eItemType=ITEM_RANGE;
						else if (HAS_BASE(SdrFractionItem ,&rItem)) aEntry.eItemType=ITEM_FRACTION;
						else if (HAS_BASE(SvxColorItem    ,&rItem)) aEntry.eItemType=ITEM_COLOR;
						else if (HAS_BASE(SvxFontItem     ,&rItem)) aEntry.eItemType=ITEM_FONT;
						else if (HAS_BASE(SvxFontHeightItem,&rItem))aEntry.eItemType=ITEM_FONTHEIGHT;
						else if (HAS_BASE(SvxCharScaleWidthItem,&rItem)) aEntry.eItemType=ITEM_FONTWIDTH;
						else if (HAS_BASE(SvxFieldItem    ,&rItem)) aEntry.eItemType=ITEM_FIELD;
						switch (aEntry.eItemType) {
							case ITEM_BYTE      : aEntry.bIsNum=sal_True;  aEntry.nVal=((SfxByteItem  &)rItem).GetValue(); aEntry.nMin=0;      aEntry.nMax=255;   break;
							case ITEM_INT16     : aEntry.bIsNum=sal_True;  aEntry.nVal=((SfxInt16Item &)rItem).GetValue(); aEntry.nMin=-32767; aEntry.nMax=32767; break;
							case ITEM_UINT16    : aEntry.bIsNum=sal_True;  aEntry.nVal=((SfxUInt16Item&)rItem).GetValue(); aEntry.nMin=0;      aEntry.nMax=65535; break;
							case ITEM_INT32     : aEntry.bIsNum=sal_True;  aEntry.nVal=((SfxInt32Item &)rItem).GetValue();                                        break;
							case ITEM_UINT32    : aEntry.bIsNum=sal_True;  aEntry.nVal=((SfxUInt32Item&)rItem).GetValue(); aEntry.nMin=0; /*aEntry.nMax=0xFF...*/;break;
							case ITEM_ENUM      : aEntry.bCanNum=sal_True; aEntry.nVal=((SfxEnumItemInterface&)rItem).GetEnumValue(); aEntry.nMin=0; aEntry.nMax=((SfxEnumItemInterface&)rItem).GetValueCount()-1; break;
							case ITEM_BOOL      : aEntry.bCanNum=sal_True; aEntry.nVal=((SfxBoolItem  &)rItem).GetValue(); aEntry.nMin=0; aEntry.nMax=1;          break;
							case ITEM_FLAG      : aEntry.bCanNum=sal_True; aEntry.nVal=((SfxFlagItem  &)rItem).GetValue(); aEntry.nMin=0; aEntry.nMax=0xFFFF;     break;
							case ITEM_FONTHEIGHT: aEntry.bCanNum=sal_True; aEntry.nVal=((SvxFontHeightItem&)rItem).GetHeight(); aEntry.nMin=0;                    break;
							case ITEM_FONTWIDTH : aEntry.bCanNum=sal_True; aEntry.nVal=((SvxCharScaleWidthItem&)rItem).GetValue();    aEntry.nMin=0; aEntry.nMax=0xFFFF;break;
							default: break;
						} // switch
						if (aEntry.bIsNum) aEntry.bCanNum=sal_True;
						FASTBOOL bGetPres=sal_True;
						if (bGetPres) {
							rItem.GetPresentation(SFX_ITEM_PRESENTATION_NAMELESS,
												  pPool->GetMetric(nWhich),
												  SFX_MAPUNIT_MM,aEntry.aValue);
							if (aEntry.bCanNum) {
								aEntry.aValue.InsertAscii(": ",0);
								aEntry.aValue.Insert(UniString::CreateFromInt32(aEntry.nVal),0);
							}
						} else {
							if (aEntry.eItemType==ITEM_BOOL) aEntry.aValue.AppendAscii(aEntry.nVal!=0 ? "True" : "False");
							else if (aEntry.bCanNum) aEntry.aValue = UniString::CreateFromInt32(aEntry.nVal);
							else if (aEntry.eItemType==ITEM_STRING) aEntry.aValue=((SfxStringItem&)rItem).GetValue();
							else if (aEntry.eItemType==ITEM_ENUM && nWhich!=EE_CHAR_WEIGHT) aEntry.aValue=((SfxEnumItemInterface&)rItem).GetValueTextByPos((sal_Bool)aEntry.nVal);
							else aEntry.aValue = String("GPF", aTextEncoding);
						}
					} else {
						aEntry.aValue = String("InvalidItem", aTextEncoding);
					}
					ImpSetEntry(aEntry,nEntryNum);
					nEntryNum++;
				}
			}
			nWhich=aIter.NextWhich();
		} // while

		if (aList.Count()>nEntryNum) { // evtl. noch zuviele Eintraege
			sal_uIntPtr nTooMuch=aList.Count()-nEntryNum;
			for (sal_uIntPtr nNum=0; nNum<nTooMuch; nNum++) {
				delete ImpGetEntry(nEntryNum);
				aList.Remove(nEntryNum);
			}
			RowRemoved(nEntryNum,nTooMuch);
		}
	} else {
		Clear(); // wenn pSet==NULL
	}
	ImpRestoreWhich();
	SetMode(MYBROWSEMODE);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

_SdrItemBrowserWindow::_SdrItemBrowserWindow(Window* pParent, WinBits nBits):
	FloatingWindow(pParent,nBits),
	aBrowse(this)
{
	SetOutputSizePixel(aBrowse.GetSizePixel());
	SetText(String("Joe's ItemBrowser", gsl_getSystemTextEncoding()));
	aBrowse.Show();
}

__EXPORT _SdrItemBrowserWindow::~_SdrItemBrowserWindow()
{
}

void __EXPORT _SdrItemBrowserWindow::Resize()
{
	aBrowse.SetSizePixel(GetOutputSizePixel());
}

void __EXPORT _SdrItemBrowserWindow::GetFocus()
{
	aBrowse.GrabFocus();
}

////////////////////////////////////////////////////////////////////////////////////////////////////

SdrItemBrowser::SdrItemBrowser(SdrView& rView):
	_SdrItemBrowserWindow(ImpGetViewWin(rView)),
	pView(&rView),
	bDirty(sal_False)
{
	aIdleTimer.SetTimeoutHdl(LINK(this,SdrItemBrowser,IdleHdl));
	GetBrowserControl().SetEntryChangedHdl(LINK(this,SdrItemBrowser,ChangedHdl));
	GetBrowserControl().SetSetDirtyHdl(LINK(this,SdrItemBrowser,SetDirtyHdl));
	SetDirty();
}

Window* SdrItemBrowser::ImpGetViewWin(SdrView& rView)
{
	const sal_uInt32 nWinCount(rView.PaintWindowCount());

	for(sal_uInt32 a(0L); a < nWinCount; a++)
	{
		SdrPaintWindow* pCandidate = rView.GetPaintWindow(a);

		if(OUTDEV_WINDOW == pCandidate->GetOutputDevice().GetOutDevType())
		{
			return (Window*)(&pCandidate->GetOutputDevice());
		}
	}

	return 0L;
}

void SdrItemBrowser::ForceParent()
{
	Window* pWin=ImpGetViewWin(*pView);
	if (pWin!=NULL) SetParent(pWin);
}

void SdrItemBrowser::SetDirty()
{
	if (!bDirty) {
		bDirty=sal_True;
		aIdleTimer.SetTimeout(1);
		aIdleTimer.Start();
	}
}

void SdrItemBrowser::Undirty()
{
	aIdleTimer.Stop();
	bDirty = sal_False;

//	SfxItemSet aSet(pView->GetAttributes());
	SfxItemSet aSet(pView->GetModel()->GetItemPool());
	pView->GetAttributes(aSet);

	if(pView->AreObjectsMarked())
	{
		// SfxItemSet a2ndSet(pView->GetAttributes(sal_True));
		SfxItemSet a2ndSet(pView->GetModel()->GetItemPool());
		pView->GetAttributes(a2ndSet, sal_True);

		SetAttributes(&aSet,&a2ndSet);
	}
	else
	{
		SetAttributes(&aSet);
	}
}

IMPL_LINK(SdrItemBrowser,IdleHdl,Timer*,EMPTYARG)
{
	Undirty();
	return 0;
}

IMPL_LINK(SdrItemBrowser,ChangedHdl,_SdrItemBrowserControl*,pBrowse)
{
	const ImpItemListRow* pEntry=pBrowse->GetAktChangeEntry();
	if (pEntry!=NULL)
	{
//		SfxItemSet aSet(pView->GetAttributes());
		SfxItemSet aSet(pView->GetModel()->GetItemPool());
		pView->GetAttributes(aSet);

		SfxItemSet aNewSet(*aSet.GetPool(),pEntry->nWhichId,pEntry->nWhichId);
		XubString aNewText(pBrowse->GetNewEntryValue());
		sal_Bool bDel( aNewText.EqualsAscii("del")
			|| aNewText.EqualsAscii("Del")
			|| aNewText.EqualsAscii("DEL")
			|| aNewText.EqualsAscii("default")
			|| aNewText.EqualsAscii("Default")
			|| aNewText.EqualsAscii("DEFAULT"));

		if (!bDel) {
			SfxPoolItem* pNewItem=aSet.Get(pEntry->nWhichId).Clone();
			long nLongVal=0;
			nLongVal = aNewText.ToInt32();
			if (pEntry->bCanNum) {
				if (nLongVal>pEntry->nMax) nLongVal=pEntry->nMax;
				if (nLongVal<pEntry->nMin) nLongVal=pEntry->nMin;
			}
			FASTBOOL bPair=sal_False;
			FASTBOOL bPairX=sal_True;
			FASTBOOL bPairY=sal_False;
			sal_uInt16 nSepLen=1;
			long nLongX = aNewText.ToInt32();
			long nLongY=0;
			xub_StrLen nPos = aNewText.Search(sal_Unicode('/'));
			if (nPos==STRING_NOTFOUND) nPos=aNewText.Search(sal_Unicode(':'));
			if (nPos==STRING_NOTFOUND) nPos=aNewText.Search(sal_Unicode(' '));
			if (nPos==STRING_NOTFOUND) { nPos=aNewText.SearchAscii(".."); if (nPos!=STRING_NOTFOUND) nSepLen=2; }
			if (nPos!=STRING_NOTFOUND) {
				bPair=sal_True;
				bPairX=nPos>0;
				XubString s(aNewText);
				s.Erase(0,nPos+nSepLen);
				bPairY = (sal_Bool)aNewText.Len();
				nLongY = s.ToInt32();
			}
			switch (pEntry->eItemType) {
				case ITEM_BYTE  : ((SfxByteItem  *)pNewItem)->SetValue((sal_uInt8  )nLongVal); break;
				case ITEM_INT16 : ((SfxInt16Item *)pNewItem)->SetValue((sal_Int16 )nLongVal); break;
				case ITEM_UINT16: ((SfxUInt16Item*)pNewItem)->SetValue((sal_uInt16)nLongVal); break;
				case ITEM_INT32: {
					if(HAS_BASE(SdrAngleItem, pNewItem))
					{
						aNewText.SearchAndReplace(sal_Unicode(','), sal_Unicode('.'));
						double nVal = aNewText.ToFloat();
						nLongVal = (long)(nVal * 100 + 0.5);
					}
					((SfxInt32Item *)pNewItem)->SetValue((sal_Int32)nLongVal);
				} break;
				case ITEM_UINT32: ((SfxUInt32Item*)pNewItem)->SetValue(aNewText.ToInt32()); break;
				case ITEM_ENUM  : ((SfxEnumItemInterface*)pNewItem)->SetEnumValue((sal_uInt16)nLongVal); break;
				case ITEM_BOOL: {
					aNewText.ToUpperAscii();
					if (aNewText.EqualsAscii("TRUE")) nLongVal=1;
					if (aNewText.EqualsAscii("JA")) nLongVal=1;
					if (aNewText.EqualsAscii("AN")) nLongVal=1;
					if (aNewText.EqualsAscii("EIN")) nLongVal=1;
					if (aNewText.EqualsAscii("ON")) nLongVal=1;
					if (aNewText.EqualsAscii("YES")) nLongVal=1;
					((SfxBoolItem*)pNewItem)->SetValue((sal_Bool)nLongVal);
				} break;
				case ITEM_FLAG  : ((SfxFlagItem  *)pNewItem)->SetValue((sal_uInt16)nLongVal); break;
				case ITEM_STRING: ((SfxStringItem*)pNewItem)->SetValue(aNewText); break;
				case ITEM_POINT : ((SfxPointItem*)pNewItem)->SetValue(Point(nLongX,nLongY)); break;
				case ITEM_RECT  : break;
				case ITEM_RANGE : {
					((SfxRangeItem*)pNewItem)->From()=(sal_uInt16)nLongX;
					((SfxRangeItem*)pNewItem)->From()=(sal_uInt16)nLongY;
				} break;
				case ITEM_LRANGE : {
				} break;
				case ITEM_FRACTION: {
					if (!bPairX) nLongX=1;
					if (!bPairY) nLongY=1;
					((SdrFractionItem*)pNewItem)->SetValue(Fraction(nLongX,nLongY));
				} break;
				case ITEM_XCOLOR: break;
				case ITEM_COLOR: break;
				case ITEM_FONT: {
                    ((SvxFontItem*)pNewItem)->SetFamily( FAMILY_DONTKNOW );
                    ((SvxFontItem*)pNewItem)->SetFamilyName(aNewText);
                    ((SvxFontItem*)pNewItem)->SetStyleName( String() );
				} break;
				case ITEM_FONTHEIGHT: {
					sal_uIntPtr nHgt=0;
					sal_uInt16 nProp=100;
					if (aNewText.Search(sal_Unicode('%'))!=STRING_NOTFOUND) {
						nProp=(sal_uInt16)nLongVal;
					} else {
						nHgt=nLongVal;
					}
					((SvxFontHeightItem*)pNewItem)->SetHeight(nHgt,nProp);
				} break;
				case ITEM_FONTWIDTH: {
					sal_uInt16 nProp=100;
					if (aNewText.Search(sal_Unicode('%'))!=STRING_NOTFOUND) {
						nProp=(sal_uInt16)nLongVal;
					}
					((SvxCharScaleWidthItem*)pNewItem)->SetValue(nProp);
				} break;
				case ITEM_FIELD: break;
				default: break;
			} // switch
			aNewSet.Put(*pNewItem);
			delete pNewItem;
		}
		pView->SetAttributes(aNewSet,bDel);
	}
	return 0;
}

IMPL_LINK(SdrItemBrowser,SetDirtyHdl,_SdrItemBrowserControl*,EMPTYARG)
{
	SetDirty();
	return 0;
}


