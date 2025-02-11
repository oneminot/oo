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




#ifdef _MSC_VER
#pragma hdrstop
#endif

#define _SVX_PARAITEM_HXX
#define _SVX_TEXTITEM_HXX

#ifndef _SFX_PROGRESS_HXX //autogen
#include <bf_sfx2/progress.hxx>
#endif
#ifndef _SWWAIT_HXX
#include <swwait.hxx>
#endif

#ifndef _HORIORNT_HXX
#include <horiornt.hxx>
#endif

#ifndef _FESH_HXX
#include <fesh.hxx>
#endif
#ifndef _DOC_HXX
#include <doc.hxx>
#endif
#ifndef _ROOTFRM_HXX
#include <rootfrm.hxx>
#endif
#ifndef _PAGEFRM_HXX
#include <pagefrm.hxx>
#endif
#ifndef _VIEWIMP_HXX
#include <viewimp.hxx>
#endif
#ifndef _FRMTOOL_HXX
#include <frmtool.hxx>
#endif
#ifndef _VIEWOPT_HXX
#include <viewopt.hxx>
#endif
#ifndef _DVIEW_HXX
#include <dview.hxx>
#endif
#ifndef _SWREGION_HXX
#include <swregion.hxx>
#endif
#ifndef _HINTS_HXX
#include <hints.hxx>
#endif
#ifndef _TXTFRM_HXX
#include <txtfrm.hxx>
#endif
#ifndef _LAYACT_HXX
#include <layact.hxx>
#endif
#ifndef _MDIEXP_HXX
#include <mdiexp.hxx>
#endif
#ifndef _FNTCACHE_HXX
#include <fntcache.hxx>
#endif
#ifndef _DOCSH_HXX
#include <docsh.hxx>
#endif
#ifndef _PAGEDESC_HXX
#include <pagedesc.hxx>
#endif
#ifndef _ACCESSIBILITYOPTIONS_HXX
#include <accessibilityoptions.hxx>
#endif
#ifndef _STATSTR_HRC
#include <statstr.hrc>
#endif
#include <shellres.hxx>
namespace binfilter {

BOOL ViewShell::bLstAct = FALSE;
ShellResource *ViewShell::pShellRes = 0;
Window *ViewShell::pCareWindow = 0;

FASTBOOL bInSizeNotify = FALSE;

/*N*/ DBG_NAME(LayoutIdle)

/*N*/ TYPEINIT0(ViewShell);

/******************************************************************************
|*
|*	ViewShell::ImplEndAction()
|*
|*	Letzte Aenderung	MA 04. Sep. 96
|*
******************************************************************************/

/*N*/ void ViewShell::ImplEndAction( const BOOL bIdleEnd )
/*N*/ {
/*N*/ 	//Fuer den Drucker gibt es hier nichts zu tun.
/*N*/ 	if ( !GetWin() || IsPreView() )
/*N*/ 	{
/*?*/ 		bPaintWorks = TRUE;
/*?*/ 		UISizeNotify();
/*?*/ 		return;
/*N*/ 	}
/*N*/
/*N*/ 	// #94195# remember when the handles need refresh at end of method
/*N*/ 	sal_Bool bRefreshMarker(sal_False);
/*N*/
/*N*/ 	bInEndAction = TRUE;
/*N*/
/*N*/ 	//Laeuft hiermit das EndAction der Letzten Shell im Ring?
/*N*/ 	ViewShell::bLstAct = TRUE;
/*N*/ 	ViewShell *pSh = (ViewShell*)this->GetNext();
/*N*/ 	while ( pSh != this )
/*?*/ 	{	if ( pSh->ActionPend() )
/*?*/ 		{	ViewShell::bLstAct = FALSE;
/*?*/ 			pSh = this;
/*?*/ 		}
/*?*/ 		else
/*?*/ 			pSh = (ViewShell*)pSh->GetNext();
/*?*/ 	}
/*N*/
/*N*/ 	SET_CURR_SHELL( this );
/*N*/ 	if ( Imp()->HasDrawView() && !Imp()->GetDrawView()->IsMarkHdlHidden() )
/*?*/ 		Imp()->StartAction();
/*N*/
/*N*/ 	if ( Imp()->GetRegion() && Imp()->GetRegion()->GetOrigin() != VisArea() )
/*N*/ 		Imp()->DelRegions();
/*N*/
/*N*/ 	const FASTBOOL bExtraData = ::binfilter::IsExtraData( GetDoc() );
/*N*/
/*N*/ 	if ( !bIdleEnd )
/*N*/ 	{
/*N*/ 		if ( Imp()->IsNextScroll() && !bExtraData )
/*N*/ 			Imp()->SetScroll();
/*N*/ 		else
/*N*/ 		{
/*N*/ 			if ( bExtraData )
/*?*/ 				Imp()->bScroll = FALSE;
/*N*/ 			Imp()->SetNextScroll();
/*N*/ 			Imp()->ResetScroll();
/*N*/ 		}
/*N*/ 		SwLayAction aAction( GetLayout(), Imp() );
/*N*/ 		aAction.SetComplete( FALSE );
/*N*/ 		if ( nLockPaint )
/*?*/ 			aAction.SetPaint( FALSE );
/*N*/ 		aAction.SetInputType( INPUT_KEYBOARD );
/*N*/ 		aAction.Action();
/*N*/ 		Imp()->SetScroll();
/*N*/ 	}
/*N*/
/*N*/ 	//Wenn wir selbst keine Paints erzeugen, so warten wir auf das Paint
/*N*/ 	//vom System. Dann ist das Clipping korrekt gesetzt; Beispiel: verschieben
/*N*/ 	//eines DrawObjektes.
/*N*/ 	if ( Imp()->GetRegion() 	|| Imp()->GetScrollRects() ||
/*N*/ 		 aInvalidRect.HasArea() || bExtraData )
/*N*/ 	{
/*?*/ 		if ( !nLockPaint )
/*?*/ 		{
/*?*/ 			FASTBOOL bPaintsFromSystem = aInvalidRect.HasArea();
/*?*/ 			GetWin()->Update();
/*?*/ 			if ( aInvalidRect.HasArea() )
/*?*/ 			{
/*?*/ 				if ( bPaintsFromSystem )
/*?*/ 					Imp()->AddPaintRect( aInvalidRect );
/*?*/
/*?*/ 				// AW 22.09.99: tell DrawView that drawing order will be rearranged
/*?*/ 				// to give it a chance to react with proper IAO updates
/*?*/ 				if (HasDrawView())
/*?*/ 				{
/*?*/ 				DBG_BF_ASSERT(0, "STRIP"); //STRIP001 	GetDrawView()->ForceInvalidateMarkHandles();
/*?*/ 				}
/*?*/
/*?*/ 				ResetInvalidRect();
/*?*/ 				bPaintsFromSystem = TRUE;
/*?*/ 			}
/*?*/ 			bPaintWorks = TRUE;
/*?*/
/*?*/ 			SwRegionRects *pRegion = Imp()->GetRegion();
/*?*/
/*?*/ 			//JP 27.11.97: wer die Selection hided, muss sie aber auch
/*?*/ 			//				wieder Showen. Sonst gibt es Paintfehler!
/*?*/ 			//	z.B.: addional Mode, Seite vertikal hab zu sehen, in der
/*?*/ 			// Mitte eine Selektion und mit einem anderen Cursor an linken
/*?*/ 			// rechten Rand springen. Ohne ShowCrsr verschwindet die
/*?*/ 			// Selektion
/*?*/ 			BOOL bShowCrsr = (pRegion || Imp()->GetScrollRects()) &&
/*?*/ 								IsA( TYPE(SwCrsrShell) );
/*?*/ 			if( bShowCrsr )
/*?*/ 				{DBG_BF_ASSERT(0, "STRIP");} //STRIP001 ((SwCrsrShell*)this)->HideCrsrs();
/*?*/
/*?*/ 			Scroll();
/*?*/ 			if ( bPaintsFromSystem && Imp()->pScrolledArea )
					{DBG_BF_ASSERT(0, "STRIP");} //STRIP001 /*?*/ 				Imp()->FlushScrolledArea();
/*?*/
/*?*/ 			if ( pRegion )
					{DBG_BF_ASSERT(0, "STRIP");} //STRIP001 /*?*/ 			{
/*?*/ 			if( bShowCrsr )
/*?*/ 				((SwCrsrShell*)this)->ShowCrsrs( TRUE );
/*?*/ 		}
/*?*/ 		else
/*?*/ 		{
/*?*/ 			Imp()->DelRegions();
/*?*/ 			bPaintWorks =  TRUE;
/*?*/ 		}
/*N*/ 	}
/*N*/ 	else
/*N*/ 		bPaintWorks = TRUE;
/*N*/
/*N*/ 	bInEndAction = FALSE;
/*N*/ 	ViewShell::bLstAct = FALSE;
/*N*/ 	Imp()->EndAction();
/*N*/
/*N*/
/*N*/ 	//Damit sich die automatischen Scrollbars auch richtig anordnen k�nnen
/*N*/ 	//muessen wir die Aktion hier kuenstlich beenden (EndAction loesst ein
/*N*/ 	//Notify aus, und das muss Start-/EndAction rufen um die  Scrollbars
/*N*/ 	//klarzubekommen.
/*N*/ 	--nStartAction;
/*N*/ 	UISizeNotify();
/*N*/ 	++nStartAction;
/*N*/
/*N*/ #ifdef DBG_UTIL
/*N*/ 	// No Scroll starts the timer to repair the scrolled area automatically
/*N*/ 	if( GetViewOptions()->IsTest8() )
/*N*/ #endif
/*?*/ 	if ( Imp()->IsScrolled() )
/*?*/ 		Imp()->RestartScrollTimer();
/*N*/
/*N*/ 	// #94195# refresh handles when they were hard removed for display change
/*N*/ 	if(bRefreshMarker && HasDrawView())
/*N*/ 	{
/*?*/ 		GetDrawView()->AdjustMarkHdl(FALSE);
/*N*/ 	}
/*N*/
/*N*/ #ifdef ACCESSIBLE_LAYOUT
/*N*/ #endif
/*N*/ }

/******************************************************************************
|*
|*	ViewShell::ImplStartAction()
|*
|*	Ersterstellung		MA 25. Jul. 94
|*	Letzte Aenderung	MA 25. Jul. 94
|*
******************************************************************************/

/*N*/ void ViewShell::ImplStartAction()
/*N*/ {
/*N*/ 	bPaintWorks = FALSE;
/*N*/ 	Imp()->StartAction();
/*N*/ }

/******************************************************************************
|*
|*	ViewShell::AddPaintRect()
|*
|*	Ersterstellung		MA ??
|*	Letzte Aenderung	MA 09. Feb. 97
|*
******************************************************************************/

/*N*/ BOOL ViewShell::AddPaintRect( const SwRect & rRect )
/*N*/ {
/*N*/ 	BOOL bRet = FALSE;
/*N*/ 	ViewShell *pSh = this;
/*N*/ 	do
/*N*/ 	{
/*N*/ 		if ( pSh->IsPreView() && pSh->GetWin() )
/*N*/ //			pSh->GetWin()->Invalidate();
/*?*/           DBG_BF_ASSERT(0, "STRIP");//::binfilter::RepaintPagePreview( pSh, rRect );
/*N*/ 		else
/*N*/ 			bRet |= pSh->Imp()->AddPaintRect( rRect );
/*N*/ 		pSh = (ViewShell*)pSh->GetNext();
/*N*/
/*N*/ 	} while ( pSh != this );
/*N*/ 	return bRet;
/*N*/ }

/******************************************************************************
|*
|*	ViewShell::InvalidateWindows()
|*
|*	Ersterstellung		MA ??
|*	Letzte Aenderung	MA 09. Feb. 97
|*
******************************************************************************/

/*N*/ void ViewShell::InvalidateWindows( const SwRect &rRect )
/*N*/ {
/*N*/ 	if ( !Imp()->IsCalcLayoutProgress() )
/*N*/ 	{
/*N*/ 		ViewShell *pSh = this;
/*N*/ 		do
/*N*/ 		{
/*N*/ 			if ( pSh->GetWin() )
/*N*/ 			{
/*N*/ 				if ( pSh->IsPreView() )
/*N*/ //					pSh->GetWin()->Invalidate();
/*?*/                   DBG_BF_ASSERT(0, "STRIP");//::binfilter::RepaintPagePreview( pSh, rRect );
/*N*/ 				else if ( pSh->VisArea().IsOver( rRect ) )
/*N*/ 					pSh->GetWin()->Invalidate( rRect.SVRect() );
/*N*/ 			}
/*N*/ 			pSh = (ViewShell*)pSh->GetNext();
/*N*/
/*N*/ 		} while ( pSh != this );
/*N*/ 	}
/*N*/ }

/******************************************************************************
|*
|*	ViewShell::MakeVisible()
|*
|*	Ersterstellung		MA ??
|*	Letzte Aenderung	AMA 10. Okt. 95
|*
******************************************************************************/

/*N*/ void ViewShell::MakeVisible( const SwRect &rRect )
/*N*/ {
/*N*/   if ( !VisArea().IsInside( rRect ) || /*IsScrollMDI( this, rRect ) ||*/ GetCareWin(*this) )
/*N*/ 	{
/*N*/ 		if ( !IsViewLocked() )
/*N*/ 		{
/*N*/ 			if( pWin )
/*N*/ 			{
/*N*/ 				const SwFrm* pRoot = GetDoc()->GetRootFrm();
/*N*/ 				int nLoopCnt = 3;
/*N*/ 				long nOldH;
/*N*/ 				do{
/*N*/ 					nOldH = pRoot->Frm().Height();
/*N*/ 					StartAction();
/*N*/                   DBG_BF_ASSERT(0, "STRIP");//ScrollMDI( this, rRect, USHRT_MAX, USHRT_MAX );
/*N*/ 					EndAction();
/*N*/ 				} while( nOldH != pRoot->Frm().Height() && nLoopCnt-- );
/*N*/ 			}
/*N*/ #ifdef DBG_UTIL
/*N*/ 			else
/*N*/ 			{
/*N*/ 				//MA: 04. Nov. 94, braucht doch keiner oder??
/*N*/ 				ASSERT( !this, "MakeVisible fuer Drucker wird doch gebraucht?" );
/*N*/ 			}
/*N*/
/*N*/ #endif
/*N*/ 		}
/*N*/ 	}
/*N*/ }

/******************************************************************************
|*
|*	ViewShell::CareChildWindow()
|*
|*	Ersterstellung		AMA 10. Okt. 95
|*	Letzte Aenderung	AMA 10. Okt. 95
|*
******************************************************************************/


/******************************************************************************
|*
|*	ViewShell::GetPagePos()
|*
|*	Ersterstellung		MA ??
|*	Letzte Aenderung	MA 04. Aug. 93
|*
******************************************************************************/


/*************************************************************************
|*
|*	  ViewShell::LayoutIdle()
|*
|*	  Ersterstellung	MA 26. May. 92
|*	  Letzte Aenderung	OG 19. Mar. 96
|*
*************************************************************************/

/*N*/ void ViewShell::LayoutIdle()
/*N*/ {
/*N*/ #ifdef TCOVER
/*N*/ 	//fuer TCV-Version: Ende der Startphase des Programmes
/*N*/ 	TCovCall::Idle();
/*N*/ #endif
/*N*/ 	if( !pOpt->IsIdle() || !GetWin() ||
/*N*/ 		( Imp()->HasDrawView() && Imp()->GetDrawView()->IsDragObj() ) )
/*N*/ 		return;
/*N*/
/*N*/ 	//Kein Idle wenn gerade gedruckt wird.
/*N*/ 	ViewShell *pSh = this;
/*N*/ 	do
/*N*/ 	{	if ( !pSh->GetWin() )
/*N*/ 			return;
/*N*/ 		pSh = (ViewShell*)pSh->GetNext();
/*N*/
/*N*/ 	} while ( pSh != this );
/*N*/
/*N*/ 	SET_CURR_SHELL( this );
/*N*/
/*N*/ #ifdef DBG_UTIL
/*N*/ 	// Wenn Test5 gedrueckt ist, wird der IdleFormatierer abgeknipst.
/*N*/ 	if( pOpt->IsTest5() )
/*N*/ 		return;
/*N*/ #endif
/*N*/
/*N*/ 	{
/*N*/ 		DBG_PROFSTART( LayoutIdle );
/*N*/
/*N*/ 		//Cache vorbereiten und restaurieren, damit er nicht versaut wird.
/*N*/ 		SwSaveSetLRUOfst aSave( *SwTxtFrm::GetTxtCache(),
/*N*/ 							 SwTxtFrm::GetTxtCache()->GetCurMax() - 50 );
/*N*/ 		SwLayIdle aIdle( GetLayout(), Imp() );
/*N*/ 		DBG_PROFSTOP( LayoutIdle );
/*N*/ 	}
/*N*/ }

// Absatzabstaende koennen wahlweise addiert oder maximiert werden


/******************************************************************************
|*
|*	ViewShell::Reformat
|*
|*	Ersterstellung		BP ???
|*	Letzte Aenderung	MA 13. Feb. 98
|*
******************************************************************************/

/*N*/ void ViewShell::Reformat()
/*N*/ {
/*N*/ 	// Wir gehen auf Nummer sicher:
/*N*/ 	// Wir muessen die alten Fontinformationen wegschmeissen,
/*N*/ 	// wenn die Druckeraufloesung oder der Zoomfaktor sich aendert.
/*N*/ 	// Init() und Reformat() sind die sichersten Stellen.
/*N*/ #ifdef FNTMET
/*N*/ 	aFntMetList.Flush();
/*N*/ #else
/*N*/ 	pFntCache->Flush( );
/*N*/ #endif
/*N*/
/*N*/     if( GetLayout()->IsCallbackActionEnabled() )
/*N*/     {
/*N*/
/*N*/         StartAction();
/*N*/         GetLayout()->InvalidateAllCntnt();
/*N*/         EndAction();
/*N*/     }
/*N*/ }

/******************************************************************************
|*
|*	ViewShell::CalcLayout()
|*					Vollstaendige Formatierung von Layout und Inhalt.
|*
|*	Ersterstellung		MA 31. Jan. 94
|*	Letzte Aenderung	MA 08. Oct. 96
|*
******************************************************************************/

/*N*/ void ViewShell::CalcLayout()
/*N*/ {
/*N*/ 	SET_CURR_SHELL( this );
/*N*/
/*N*/ 	//Cache vorbereiten und restaurieren, damit er nicht versaut wird.
/*N*/ 	SwSaveSetLRUOfst aSaveLRU( *SwTxtFrm::GetTxtCache(),
/*N*/ 						  		SwTxtFrm::GetTxtCache()->GetCurMax() - 50 );
/*N*/
/*N*/ 	//Progress einschalten wenn noch keiner Lauft.
/*N*/ 	const BOOL bEndProgress = SfxProgress::GetActiveProgress( GetDoc()->GetDocShell() ) == 0;
/*N*/ 	if ( bEndProgress )
/*N*/ 	{
/*N*/ 		USHORT nEndPage = GetLayout()->GetPageNum();
/*N*/ 		nEndPage += nEndPage * 10 / 100;
/*N*/ 		::binfilter::StartProgress( STR_STATSTR_REFORMAT, 0, nEndPage, GetDoc()->GetDocShell() );
/*N*/ 	}
/*N*/
/*N*/ 	SwLayAction aAction( GetLayout(), Imp() );
/*N*/ 	aAction.SetPaint( FALSE );
/*N*/ 	aAction.SetStatBar( TRUE );
/*N*/ 	aAction.SetCalcLayout( TRUE );
/*N*/ 	aAction.SetReschedule( TRUE );
/*N*/ 	GetDoc()->LockExpFlds();
/*N*/ 	aAction.Action();
/*N*/ 	GetDoc()->UnlockExpFlds();
/*N*/
/*N*/ 	//Das SetNewFldLst() am Doc wurde unterbunden und muss nachgeholt
/*N*/ 	//werden (siehe flowfrm.cxx, txtfld.cxx)
/*N*/ 	if ( aAction.IsExpFlds() )
/*N*/ 	{
/*N*/ 		aAction.Reset();
/*N*/ 		aAction.SetPaint( FALSE );
/*N*/ 		aAction.SetStatBar( TRUE );
/*N*/ 		aAction.SetReschedule( TRUE );
/*N*/
/*N*/ 		SwDocPosUpdate aMsgHnt( 0 );
/*N*/ 		GetDoc()->UpdatePageFlds( &aMsgHnt );
/*N*/ 		GetDoc()->UpdateExpFlds();
/*N*/
/*N*/ 		aAction.Action();
/*N*/ 	}
/*N*/
/*N*/ 	if ( VisArea().HasArea() )
/*N*/ 		InvalidateWindows( VisArea() );
/*N*/ 	if ( bEndProgress )
/*N*/ 		::binfilter::EndProgress( GetDoc()->GetDocShell() );
/*N*/ }

/******************************************************************************
|*
|*	ViewShell::SetFirstVisPageInvalid()
|*
|*	Ersterstellung		MA 19. May. 94
|*	Letzte Aenderung	MA 19. May. 94
|*
******************************************************************************/

/*N*/ void ViewShell::SetFirstVisPageInvalid()
/*N*/ {
/*N*/ 	ViewShell *pSh = this;
/*N*/ 	do
/*N*/ 	{	pSh->Imp()->SetFirstVisPageInvalid();
/*N*/ 		pSh = (ViewShell*)pSh->GetNext();
/*N*/
/*N*/ 	} while ( pSh != this );
/*N*/ }

/******************************************************************************
|*
|*	ViewShell::SizeChgNotify()
|*
|*	Ersterstellung		MA ??
|*	Letzte Aenderung	MA 17. Sep. 96
|*
******************************************************************************/

/*N*/ void ViewShell::SizeChgNotify(const Size &rSize)
/*N*/ {
/*N*/ 	if ( !pWin )
/*N*/ 		bDocSizeChgd = TRUE;
/*N*/ 	else if( ActionPend() || Imp()->IsCalcLayoutProgress() || bPaintInProgress )
/*N*/ 	{
/*N*/ 		bDocSizeChgd = TRUE;
/*N*/
/*N*/ 		if ( !Imp()->IsCalcLayoutProgress() && ISA( SwCrsrShell ) )
/*N*/ 		{
/*N*/ 			const SwFrm *pCnt = ((SwCrsrShell*)this)->GetCurrFrm( FALSE );
/*N*/ 			const SwPageFrm *pPage;
/*N*/ 			if ( pCnt && 0 != (pPage = pCnt->FindPageFrm()) )
/*N*/ 			{
/*N*/ 				USHORT nVirtNum = pPage->GetVirtPageNum();
/*N*/ 		 		const SvxNumberType& rNum = pPage->GetPageDesc()->GetNumType();
/*N*/ 				String sDisplay = rNum.GetNumStr( nVirtNum );
/*N*/               DBG_BF_ASSERT(0, "STRIP");//PageNumNotify( this, pCnt->GetPhyPageNum(), nVirtNum, sDisplay );
/*N*/ 			}
/*N*/ 		}
/*N*/ 	}
/*N*/ 	else
/*N*/ 	{
/*N*/ 		bDocSizeChgd = FALSE;
/*N*/       DBG_BF_ASSERT(0, "STRIP");//::binfilter::SizeNotify( this, GetLayout()->Frm().SSize() );
/*N*/ 	}
/*N*/ }

/*************************************************************************
|*
|* 	  ViewShell::GetLayout()
|*
|*	  Ersterstellung	OK 26. May. 92
|*	  Letzte Aenderung	MA 16. Sep. 93
|*
*************************************************************************/

/*N*/ SwRootFrm *ViewShell::GetLayout() const
/*N*/ {
/*N*/ 	return GetDoc()->GetRootFrm();
/*N*/ }
/*N*/
/*N*/ SfxPrinter* ViewShell::GetPrt( BOOL bCreate ) const
/*N*/ {
/*N*/     return GetDoc()->GetPrt( bCreate );
/*N*/ }


/*N*/ OutputDevice& ViewShell::GetRefDev() const
/*N*/ {
/*N*/     OutputDevice* pTmpOut = 0;
/*N*/     if ( GetWin() && IsBrowseMode() &&
/*N*/          ! GetViewOptions()->IsPrtFormat() )
/*N*/         pTmpOut = GetWin();
/*N*/     else if ( 0 != mpTmpRef )
/*N*/         pTmpOut = mpTmpRef;
/*N*/     else
/*N*/         pTmpOut = &GetDoc()->GetRefDev();
/*N*/
/*N*/     return *pTmpOut;
/*N*/ }

/*N*/ void ViewShell::DrawSelChanged(SdrView*)
/*N*/ {
/*N*/ }


/******************************************************************************
|*
|*	ViewShell::UISizeNotify()
|*
|*	Ersterstellung		MA 14. Jan. 97
|*	Letzte Aenderung	MA 14. Jan. 97
|*
******************************************************************************/


/*N*/ void ViewShell::UISizeNotify()
/*N*/ {
/*N*/ 	if ( bDocSizeChgd )
/*N*/ 	{
/*N*/ 		bDocSizeChgd = FALSE;
/*N*/ 		FASTBOOL bOld = bInSizeNotify;
/*N*/ 		bInSizeNotify = TRUE;
/*N*/       DBG_BF_ASSERT(0, "STRIP");//::binfilter::SizeNotify( this, GetLayout()->Frm().SSize() );
/*N*/ 		bInSizeNotify = bOld;
/*N*/ 	}
/*N*/ }


/*N*/ BOOL ViewShell::IsBrowseMode() const
/*N*/ {
/*N*/ 	return GetDoc()->IsBrowseMode();
/*N*/ }

ShellResource* ViewShell::GetShellRes()
{
    if ( !pShellRes )
        pShellRes = new ShellResource();
    return pShellRes;
}
}
