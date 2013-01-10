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
#include "precompiled_sw.hxx"


#include <doc.hxx>
#include <node.hxx>
#include <rootfrm.hxx>
#include <editsh.hxx>
#include <viscrs.hxx>
#include <IMark.hxx>
#include <bookmrk.hxx>
#include <redline.hxx>
#include <mvsave.hxx>
#include <docary.hxx>
#include <unocrsr.hxx>
#include <swundo.hxx>
#include <hints.hxx>

/*
 * MACROS um ueber alle CrsrShells zu iterieren
 */
#define PCURSH ((SwCrsrShell*)_pStartShell)
#define FOREACHSHELL_START( pEShell ) \
    {\
		ViewShell *_pStartShell = pEShell; \
		do { \
			SwCrsrShell* pSwCrsrShell = dynamic_cast< SwCrsrShell* >(_pStartShell); \
			if( pSwCrsrShell ) \
			{

#define FOREACHSHELL_END( pEShell ) \
			} \
        } while((_pStartShell=(ViewShell*)_pStartShell->GetNext())!= pEShell ); \
	}

#define PCURCRSR (_pCurrCrsr)
#define FOREACHPAM_START(pSttCrsr) \
	{\
		SwPaM *_pStartCrsr = pSttCrsr, *_pCurrCrsr = pSttCrsr; \
		do {

#define FOREACHPAM_END() \
		} while( (_pCurrCrsr=(SwPaM *)_pCurrCrsr->GetNext()) != _pStartCrsr ); \
	}

namespace
{
    // find the relevant section in which the SwUnoCrsr may wander.
    // returns NULL if no restrictions apply
    static const SwStartNode* lcl_FindUnoCrsrSection( const SwNode& rNode )
    {
        const SwStartNode* pStartNode = rNode.StartOfSectionNode();
        while( ( pStartNode != NULL ) &&
               ( pStartNode->StartOfSectionNode() != pStartNode ) &&
               ( pStartNode->GetStartNodeType() == SwNormalStartNode ) )
            pStartNode = pStartNode->StartOfSectionNode();

        return pStartNode;
    }

    static inline bool lcl_PosCorrAbs(SwPosition & rPos,
        const SwPosition& rStart,
        const SwPosition& rEnd,
        const SwPosition& rNewPos)
    {
        if ((rStart <= rPos) && (rPos <= rEnd))
        {
            rPos = rNewPos;
            return true;
        }
        return false;
    };

    static inline bool lcl_PaMCorrAbs(SwPaM & rPam,
        const SwPosition& rStart,
        const SwPosition& rEnd,
        const SwPosition& rNewPos)
    {
        bool bRet = false;
        bRet |= lcl_PosCorrAbs(rPam.GetBound(true ), rStart, rEnd, rNewPos);
        bRet |= lcl_PosCorrAbs(rPam.GetBound(false), rStart, rEnd, rNewPos);
        return bRet;
    };

    static inline void lcl_PaMCorrRel1(SwPaM * pPam,
        SwNode const * const pOldNode,
        const SwPosition& rNewPos,
        const xub_StrLen nCntIdx)
    {
        for(int nb = 0; nb < 2; ++nb)
            if(&((pPam)->GetBound(sal_Bool(nb)).nNode.GetNode()) == pOldNode)
            {
                (pPam)->GetBound(sal_Bool(nb)).nNode = rNewPos.nNode;
                (pPam)->GetBound(sal_Bool(nb)).nContent.Assign(
                    const_cast<SwIndexReg*>(rNewPos.nContent.GetIdxReg()),
                    nCntIdx + (pPam)->GetBound(sal_Bool(nb)).nContent.GetIndex()); 
            }
    }
}


void PaMCorrAbs( const SwPaM& rRange,
				const SwPosition& rNewPos )
{
    SwPosition const aStart( *rRange.Start() );
    SwPosition const aEnd( *rRange.End() );
    SwPosition const aNewPos( rNewPos );
    SwDoc *const pDoc = aStart.nNode.GetNode().GetDoc();
    SwCrsrShell *const pShell = pDoc->GetEditShell();

	if( pShell )
	{
		FOREACHSHELL_START( pShell )
			SwPaM *_pStkCrsr = PCURSH->GetStkCrsr();
			if( _pStkCrsr )
			do {
                lcl_PaMCorrAbs( *_pStkCrsr, aStart, aEnd, aNewPos );
			} while ( (_pStkCrsr != 0 ) &&
				((_pStkCrsr=(SwPaM *)_pStkCrsr->GetNext()) != PCURSH->GetStkCrsr()) );

			FOREACHPAM_START( PCURSH->_GetCrsr() )
                lcl_PaMCorrAbs( *PCURCRSR, aStart, aEnd, aNewPos );
			FOREACHPAM_END()

			if( PCURSH->IsTableMode() )
                lcl_PaMCorrAbs( *PCURSH->GetTblCrs(), aStart, aEnd, aNewPos );

		FOREACHSHELL_END( pShell )
	}
	{
        SwUnoCrsrTbl& rTbl = const_cast<SwUnoCrsrTbl&>(pDoc->GetUnoCrsrTbl());

		for( sal_uInt16 n = 0; n < rTbl.Count(); ++n )
		{
            SwUnoCrsr *const pUnoCursor = rTbl[ n ];

            bool bChange = false; // has the UNO cursor been corrected?

            // determine whether the UNO cursor will leave it's designated
            // section
            bool const bLeaveSection =
                pUnoCursor->IsRemainInSection() &&
                ( lcl_FindUnoCrsrSection( aNewPos.nNode.GetNode() ) !=
                  lcl_FindUnoCrsrSection(
                      pUnoCursor->GetPoint()->nNode.GetNode() ) );

			FOREACHPAM_START( pUnoCursor )
                bChange |= lcl_PaMCorrAbs( *PCURCRSR, aStart, aEnd, aNewPos );
			FOREACHPAM_END()

            SwUnoTableCrsr *const pUnoTblCrsr =
                dynamic_cast<SwUnoTableCrsr *>(rTbl[ n ]);
			if( pUnoTblCrsr )
			{
				FOREACHPAM_START( &pUnoTblCrsr->GetSelRing() )
                    bChange |=
                        lcl_PaMCorrAbs( *PCURCRSR, aStart, aEnd, aNewPos );
				FOREACHPAM_END()
			}

            // if a UNO cursor leaves its designated section, we must inform
            // (and invalidate) said cursor
            if (bChange && bLeaveSection)
            {
                // the UNO cursor has left its section. We need to notify it!
                SwMsgPoolItem aHint( RES_UNOCURSOR_LEAVES_SECTION );
                pUnoCursor->ModifyNotification( &aHint, NULL );
            }
		}
	}
}

void SwDoc::CorrAbs(const SwNodeIndex& rOldNode,
    const SwPosition& rNewPos,
    const xub_StrLen nOffset,
    sal_Bool bMoveCrsr)
{
    SwCntntNode *const pCntntNode( rOldNode.GetNode().GetCntntNode() );
    SwPaM const aPam(rOldNode, 0,
                     rOldNode, (pCntntNode) ? pCntntNode->Len() : 0);
    SwPosition aNewPos(rNewPos);
    aNewPos.nContent += nOffset;

    getIDocumentMarkAccess()->correctMarksAbsolute(rOldNode, rNewPos, nOffset);
    {   // fix redlines
        SwRedlineTbl& rTbl = *pRedlineTbl;
        for (sal_uInt16 n = 0; n < rTbl.Count(); )
        {
            // is on position ??
            SwRedline *const pRedline( rTbl[ n ] );
            bool const bChanged =
                lcl_PaMCorrAbs(*pRedline, *aPam.Start(), *aPam.End(), aNewPos);
            // clean up empty redlines: docredln.cxx asserts these as invalid
            if (bChanged && (*pRedline->GetPoint() == *pRedline->GetMark())
                         && (pRedline->GetContentIdx() == NULL))
            {
                rTbl.DeleteAndDestroy(n);
            }
            else
            {
                ++n;
            }
        }
    }

    if(bMoveCrsr)
    {
        ::PaMCorrAbs(aPam, aNewPos);
    }
}

void SwDoc::CorrAbs(const SwPaM& rRange,
    const SwPosition& rNewPos,
    sal_Bool bMoveCrsr)
{
    SwPosition aStart(*rRange.Start());
    SwPosition aEnd(*rRange.End());
    SwPosition aNewPos(rNewPos);

    _DelBookmarks(aStart.nNode, aEnd.nNode, NULL,
        &aStart.nContent, &aEnd.nContent);
    if(bMoveCrsr)
        ::PaMCorrAbs(rRange, rNewPos);
}

void SwDoc::CorrAbs(const SwNodeIndex& rStartNode,
     const SwNodeIndex& rEndNode,
     const SwPosition& rNewPos,
     sal_Bool bMoveCrsr)
{
    _DelBookmarks(rStartNode, rEndNode);

    if(bMoveCrsr)
    {
        SwCntntNode *const pCntntNode( rEndNode.GetNode().GetCntntNode() );
        SwPaM const aPam(rStartNode, 0,
                         rEndNode, (pCntntNode) ? pCntntNode->Len() : 0);
        ::PaMCorrAbs(aPam, rNewPos);
    }
}





void PaMCorrRel( const SwNodeIndex &rOldNode,
				 const SwPosition &rNewPos,
				 const xub_StrLen nOffset )
{
	const SwNode* pOldNode = &rOldNode.GetNode();
	SwPosition aNewPos( rNewPos );
	const SwDoc* pDoc = pOldNode->GetDoc();

	xub_StrLen nCntIdx = rNewPos.nContent.GetIndex() + nOffset;

	SwCrsrShell* pShell = pDoc->GetEditShell();
	if( pShell )
	{
		FOREACHSHELL_START( pShell )
			SwPaM *_pStkCrsr = PCURSH->GetStkCrsr();
			if( _pStkCrsr )
			do {
				lcl_PaMCorrRel1( _pStkCrsr, pOldNode, aNewPos, nCntIdx );
			} while ( (_pStkCrsr != 0 ) &&
				((_pStkCrsr=(SwPaM *)_pStkCrsr->GetNext()) != PCURSH->GetStkCrsr()) );

			FOREACHPAM_START( PCURSH->_GetCrsr() )
				lcl_PaMCorrRel1( PCURCRSR, pOldNode, aNewPos, nCntIdx);
			FOREACHPAM_END()

			if( PCURSH->IsTableMode() )
				lcl_PaMCorrRel1( PCURSH->GetTblCrs(), pOldNode, aNewPos, nCntIdx );

		FOREACHSHELL_END( pShell )
	}
	{
		SwUnoCrsrTbl& rTbl = (SwUnoCrsrTbl&)pDoc->GetUnoCrsrTbl();
		for( sal_uInt16 n = 0; n < rTbl.Count(); ++n )
		{
			FOREACHPAM_START( rTbl[ n ] )
				lcl_PaMCorrRel1( PCURCRSR, pOldNode, aNewPos, nCntIdx );
			FOREACHPAM_END()

            SwUnoTableCrsr* pUnoTblCrsr =
                dynamic_cast<SwUnoTableCrsr*>(rTbl[ n ]);
			if( pUnoTblCrsr )
			{
				FOREACHPAM_START( &pUnoTblCrsr->GetSelRing() )
					lcl_PaMCorrRel1( PCURCRSR, pOldNode, aNewPos, nCntIdx );
				FOREACHPAM_END()
			}
		}
	}
}

void SwDoc::CorrRel(const SwNodeIndex& rOldNode,
    const SwPosition& rNewPos,
    const xub_StrLen nOffset,
    sal_Bool bMoveCrsr)
{
    getIDocumentMarkAccess()->correctMarksRelative(rOldNode, rNewPos, nOffset);

    { // dann die Redlines korrigieren
        SwRedlineTbl& rTbl = *pRedlineTbl;
        SwPosition aNewPos(rNewPos);
        for( sal_uInt16 n = 0; n < rTbl.Count(); ++n )
        {
            // liegt auf der Position ??
            lcl_PaMCorrRel1( rTbl[ n ], &rOldNode.GetNode(), aNewPos, aNewPos.nContent.GetIndex() + nOffset );
        }
    }

    if(bMoveCrsr)
        ::PaMCorrRel(rOldNode, rNewPos, nOffset);
}


SwEditShell* SwDoc::GetEditShell( ViewShell** ppSh ) const
{
	// Layout und OLE-Shells sollten vorhanden sein!
	if( pCurrentView )
	{
		ViewShell *pSh = pCurrentView, *pVSh = pSh;
		if( ppSh )
			*ppSh = pSh;

		// wir suchen uns eine EditShell, falls diese existiert
		do {
			SwEditShell* pSwEditShell = dynamic_cast< SwEditShell* >(pSh);
			if( pSwEditShell )
				return pSwEditShell;

		} while( pVSh != ( pSh = (ViewShell*)pSh->GetNext() ));
	}
	else if( ppSh )
		*ppSh = 0;	//swmod 071029//swmod 071225

	return 0;
}

::sw::IShellCursorSupplier * SwDoc::GetIShellCursorSupplier()
{
    return GetEditShell(0);
}
