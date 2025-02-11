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
#include "precompiled_automation.hxx"
#include <osl/mutex.hxx>

#include <vcl/window.hxx>
#include <vcl/vclevent.hxx>
#include <vcl/button.hxx>
#include <vcl/edit.hxx>
#include <vcl/spinfld.hxx>
#include <vcl/msgbox.hxx>
#include <vcl/toolbox.hxx>
#include <vcl/lstbox.hxx>
#include <vcl/sound.hxx>
#include <vcl/combobox.hxx>
#include <vcl/floatwin.hxx>
#include <basic/ttstrhlp.hxx>
#include "statemnt.hxx"
#include "retstrm.hxx"
#include "rcontrol.hxx"
#include "recorder.hxx"

#include <comphelper/uieventslogger.hxx>

MacroRecorder* MacroRecorder::pMacroRecorder = NULL;

MacroRecorder::MacroRecorder()
: pLastWin( NULL )
, pEditModify( NULL )
, pActionParent( NULL )
, aKeyUniqueID()
, pKeyWin( NULL )
, bKeyFollowFocus( sal_False )
, m_bRecord( sal_False )
, m_bLog( sal_False )
{
    aHookRefresh.SetTimeout( 500 );
    aHookRefresh.SetTimeoutHdl( LINK( this, MacroRecorder, HookRefreshHdl) );
    aHookRefresh.Start();
    aEventListenerHdl = LINK( this, MacroRecorder, EventListener );
    AddEventHooks();
}

MacroRecorder::~MacroRecorder()
{
    aHookRefresh.Stop();
    RemoveEventHooks();
}

void MacroRecorder::AddEventHooks()
{
    Window *pTopLevel = Application::GetFirstTopLevelWindow();
    while ( pTopLevel )
	{
        Window *pParent = pTopLevel;
        while ( pParent->GetParent() )
            pParent = pParent->GetParent();

        pParent->RemoveChildEventListener( aEventListenerHdl );     // might be instrumented already
        pParent->AddChildEventListener( aEventListenerHdl );

        pTopLevel = Application::GetNextTopLevelWindow( pTopLevel );
	}
}

void MacroRecorder::RemoveEventHooks()
{
    Window *pTopLevel = Application::GetFirstTopLevelWindow();
    while ( pTopLevel )
	{
        pTopLevel->RemoveChildEventListener( aEventListenerHdl );
		pTopLevel = Application::GetNextTopLevelWindow( pTopLevel );
	}
}

IMPL_LINK( MacroRecorder, HookRefreshHdl, void*, EMPTYARG )
{
    AddEventHooks();
    return 0;
}

void MacroRecorder::LogVCL( rtl::OString aParentID, sal_uInt16 nVCLWindowType, rtl::OString aID, String aMethod, sal_uInt16 nParam )
{
    ::comphelper::UiEventsLogger::logVcl( Id2Str( aParentID ), nVCLWindowType, Id2Str( aID ), aMethod, nParam );
}

void MacroRecorder::LogVCL( rtl::OString aParentID, sal_uInt16 nVCLWindowType, rtl::OString aID, String aMethod )
{
    ::comphelper::UiEventsLogger::logVcl( Id2Str( aParentID ), nVCLWindowType, Id2Str( aID ), aMethod );
}

Window* MacroRecorder::GetParentWithID( Window* pThis )
{
    Window *pOverlap = pThis->GetWindow( WINDOW_OVERLAP );
    while ( pOverlap != pThis && !pThis->GetUniqueOrHelpId().getLength() && pThis->GET_REAL_PARENT() )
        pThis = pThis->GET_REAL_PARENT();
    return pThis;
}

rtl::OString MacroRecorder::GetParentID( Window* pThis )
{
    if ( pThis->GetParent() )
        return pThis->GetParent()->GetUniqueOrHelpId();
    else
        return rtl::OString();
}

IMPL_LINK( MacroRecorder, EventListener, VclSimpleEvent*, pEvent )
{
    sal_Bool bSendData = sal_False;

    if ( pEvent->ISA( VclWindowEvent ) )
    {
        VclWindowEvent* pWinEvent = ( VclWindowEvent* ) pEvent;
        Window* pWin = pWinEvent->GetWindow();
        sal_uLong nEventID = pWinEvent->GetId();
#if OSL_DEBUG_LEVEL > 1
        if ( nEventID >= 1001 && nEventID != VCLEVENT_WINDOW_KEYUP )
            nEventID = pWinEvent->GetId();  // Just something to set a breakpoint
        else
            return 0;
#endif

// check for different action after collecting keys
        // send if there_is_something_to_send
        // and eather event_is_not_of_interest
        //     or ( new_window and new_window_is_interesting )    ( ignore interesting events to uninteresting window )

        if (  aKeyString.Len()
            && (  ( nEventID != VCLEVENT_WINDOW_KEYINPUT 
                 && nEventID != VCLEVENT_WINDOW_MOUSEMOVE
                 && nEventID != VCLEVENT_WINDOW_COMMAND
                 && nEventID != VCLEVENT_WINDOW_KEYUP )
                || ( pKeyWin != pWin
                    && ( pWin->GetType() == WINDOW_CONTROL || pWin->GetType() == WINDOW_WINDOW )
                   )
               )
           )
        {
			if ( m_bRecord )
			{
				// we cannot access pKeyWin since it might have dissapeared
				if ( bKeyFollowFocus )
					StatementList::pRet->GenReturn( RET_MacroRecorder, aKeyUniqueID, (sal_uInt16)M_TypeKeys, aKeyString, bKeyFollowFocus );
				else
					StatementList::pRet->GenReturn( RET_MacroRecorder, aKeyUniqueID, (sal_uInt16)M_TypeKeys, aKeyString );
#if OSL_DEBUG_LEVEL > 1
                StatementList::pRet->GenReturn( RET_MacroRecorder, aKeyUniqueID, (sal_uInt16)M_TypeKeys, String::CreateFromInt32( nEventID ) );
#endif
				bSendData = sal_True;
			}
			if ( m_bLog )
			{
//  HACK Too many KeyEvents generated              LogVCL( rtl::OString(), 0, aKeyUniqueID, CUniString("TypeKeys"), aKeyString.Len() );
			}
            // cleanup
            aKeyString.Erase();
            pKeyWin = NULL;
            bKeyFollowFocus = sal_False;
        }

        switch ( pWin->GetType() )
        {
			case WINDOW_TABPAGE:
				switch( nEventID )
				{
					case VCLEVENT_WINDOW_ACTIVATE:
						if ( m_bRecord )
						{
							StatementList::pRet->GenReturn( RET_MacroRecorder, rtl::OString(), (comm_USHORT)(M_SetPage|M_RET_NUM_CONTROL), Id2Str( pWin->GetUniqueOrHelpId() ) );
							bSendData = sal_True;
						}
						if ( m_bLog )
						{
                            LogVCL( rtl::OString(), pWin->GetType(), pWin->GetUniqueOrHelpId(), CUniString("SetPage") );
						}
						break;
				}
				break;
			case WINDOW_RADIOBUTTON:
			case WINDOW_IMAGERADIOBUTTON:
				switch( nEventID )
				{
                    case VCLEVENT_BUTTON_CLICK:  // VCLEVENT_RADIOBUTTON_TOGGLE
                        {
                            if ( ((RadioButton*)pWin)->IsChecked() )
                            {
								if ( m_bRecord )
								{
									StatementList::pRet->GenReturn( RET_MacroRecorder, pWin->GetUniqueOrHelpId(), (comm_USHORT)M_Check );
									bSendData = sal_True;
								}
								if ( m_bLog )
								{
                                    LogVCL( GetParentID( pWin ), pWin->GetType(), pWin->GetUniqueOrHelpId(), CUniString("Check") );
								}
                            }
                        }
						break;
				}
				break;
			case WINDOW_CHECKBOX:
			case WINDOW_TRISTATEBOX:
				switch( nEventID )
				{
                    case VCLEVENT_BUTTON_CLICK: //VCLEVENT_CHECKBOX_TOGGLE:
                        {
                            comm_USHORT nMethod;
                            String aMethod;
                            switch ( ((TriStateBox*)pWin)->GetState() )
                            {
                                case STATE_CHECK: nMethod = M_Check; aMethod = CUniString("Check"); break;
                                case STATE_NOCHECK: nMethod = M_UnCheck; aMethod = CUniString("UnCheck"); break;
                                case STATE_DONTKNOW: nMethod = M_TriState; aMethod = CUniString("TriState"); break;
                                default: nMethod = M_Check;
                                    DBG_ERROR( "Unknown state in TriStateBox::GetState()" );
                            }
							if ( m_bRecord )
							{
								StatementList::pRet->GenReturn( RET_MacroRecorder, pWin->GetUniqueOrHelpId(), nMethod );
								bSendData = sal_True;
							}
							if ( m_bLog )
							{
                                LogVCL( GetParentID( pWin ), pWin->GetType(), pWin->GetUniqueOrHelpId(), aMethod );
							}
                        }
                        break;
				}
				break;
			case WINDOW_EDIT:
			case WINDOW_MULTILINEEDIT:
				switch( nEventID )
				{
					case VCLEVENT_EDIT_MODIFY:
                        pEditModify = pWin;
                        aEditModifyString = ((Edit*)pWin)->GetText();
						break;
				}
				break;
			case WINDOW_MULTILISTBOX:
				switch( nEventID )
				{
                    case VCLEVENT_LISTBOX_SELECT:
                        Sound::Beep();
                }
			case WINDOW_LISTBOX:
				switch( nEventID )
				{
//                    case VCLEVENT_LISTBOX_DOUBLECLICK:
                    case VCLEVENT_LISTBOX_SELECT:
						if ( m_bRecord )
						{
							StatementList::pRet->GenReturn( RET_MacroRecorder, pWin->GetUniqueOrHelpId(), (comm_USHORT)M_Select, comm_ULONG( ((ListBox*)pWin)->GetSelectEntryPos() +1 ) );
							bSendData = sal_True;
						}
						if ( m_bLog )
						{
                            LogVCL( GetParentID( pWin ), pWin->GetType(), pWin->GetUniqueOrHelpId(), CUniString("Select"), ((ListBox*)pWin)->GetSelectEntryPos() );
						}
						break;
				}
				break;
			case WINDOW_COMBOBOX:
			case WINDOW_PATTERNBOX:
			case WINDOW_NUMERICBOX:
			case WINDOW_METRICBOX:
			case WINDOW_CURRENCYBOX:
			case WINDOW_DATEBOX:
			case WINDOW_TIMEBOX:
                switch( nEventID )
				{
					case VCLEVENT_EDIT_MODIFY:
                        pEditModify = pWin;
                        aEditModifyString = ((Edit*)pWin)->GetText();
						break;
                    case VCLEVENT_COMBOBOX_SELECT:
                        {
                            pEditModify = NULL;
                            aEditModifyString.Erase();

                            sal_uInt16 nPos = ((ComboBox*)pWin)->GetEntryPos(((ComboBox*)pWin)->GetText());
							if ( m_bRecord )
							{
								if ( nPos == COMBOBOX_ENTRY_NOTFOUND )
									Sound::Beep();
								else
								{
									StatementList::pRet->GenReturn( RET_MacroRecorder, pWin->GetUniqueOrHelpId(), (comm_USHORT)M_Select, (comm_ULONG) nPos+1 );
									bSendData = sal_True;
								}
							}
							if ( m_bLog )
							{
                                LogVCL( GetParentID( pWin ), pWin->GetType(), pWin->GetUniqueOrHelpId(), CUniString("Select"), nPos );
							}
                        }
				}
				break;
			case WINDOW_PUSHBUTTON:
			case WINDOW_OKBUTTON:
			case WINDOW_CANCELBUTTON:
			case WINDOW_IMAGEBUTTON:
            case WINDOW_MOREBUTTON:
            case WINDOW_HELPBUTTON:
				switch( nEventID )
				{
					case VCLEVENT_BUTTON_CLICK:
                        Window* pParent = pWin->GetParent();
                        sal_Bool bDone = sal_False;
                        if ( pParent->IsDialog() && !pWin->GetUniqueOrHelpId().getLength() )
                        {
                            switch ( pParent->GetType() )
                            {
                                case WINDOW_MESSBOX:
                                case WINDOW_INFOBOX:
                                case WINDOW_WARNINGBOX:
                                case WINDOW_ERRORBOX:
                                case WINDOW_QUERYBOX:
                                case WINDOW_BUTTONDIALOG:
                                    {
                                        comm_USHORT nMethod;
                                        String aMethod;
                                        ButtonDialog* pBD = (ButtonDialog*)pParent;

                                        // we have to find the current Button ID ourselves since it is not generated at this point :-(
                                        sal_uInt16 nCurrentButtonId = 0xffff;    // Some wild value to wak up people
				                        sal_uInt16 i;
				                        for ( i = 0; i < pBD->GetButtonCount() ; i++ )
				                        {
					                        if ( pBD->GetPushButton( pBD->GetButtonId(i) ) == pWin )
                                            {
                                                nCurrentButtonId = pBD->GetButtonId(i);
                                                break;
                                            }
                                        }

                                        switch ( nCurrentButtonId )
                                        {
                                            case BUTTONID_OK: nMethod = M_OK; aMethod = CUniString("OK"); break;
                                            case BUTTONID_CANCEL: nMethod = M_Cancel; aMethod = CUniString("Cancel"); break;
                                            case BUTTONID_YES: nMethod = M_Yes; aMethod = CUniString("Yes"); break;
                                            case BUTTONID_NO: nMethod = M_No; aMethod = CUniString("No"); break;
                                            case BUTTONID_RETRY: nMethod = M_Repeat; aMethod = CUniString("Repeat"); break;
                                            case BUTTONID_HELP: nMethod = M_Help; aMethod = CUniString("Help"); break;
                                            default: nMethod = M_Click; aMethod = CUniString("Click");
                                        }
										if ( m_bRecord )
										{
											if ( nMethod != M_Click )
												StatementList::pRet->GenReturn( RET_MacroRecorder, UID_ACTIVE, nMethod );
											else
												StatementList::pRet->GenReturn( RET_MacroRecorder, UID_ACTIVE, nMethod, (comm_ULONG)nCurrentButtonId );
											bSendData = sal_True;
										}
										if ( m_bLog )
										{
											if ( nMethod != M_Click )
                                                LogVCL( rtl::OString(), pWin->GetType(), pWin->GetUniqueOrHelpId(), aMethod );
                                            else
                                                LogVCL( rtl::OString(), pWin->GetType(), pWin->GetUniqueOrHelpId(), aMethod, nCurrentButtonId );
                                            bDone = sal_True;
										}
                                    }
                                    break;
                                default:
                                    {
                                        comm_USHORT nMethod;
                                        String aMethod;
                                        switch ( pWin->GetType() )
                                        {
			                                case WINDOW_OKBUTTON: nMethod = M_OK; aMethod = CUniString("OK"); break;
			                                case WINDOW_CANCELBUTTON: nMethod = M_Cancel; aMethod = CUniString("Cancel"); break;
                                            case WINDOW_HELPBUTTON: nMethod = M_Help; aMethod = CUniString("Help"); break;
                                            default: nMethod = M_Default;aMethod = CUniString("Unknown Button");
                                                DBG_ERROR( "Unknown Button" );
                                        }
										if ( m_bRecord )
										{
											StatementList::pRet->GenReturn( RET_MacroRecorder, pParent->GetUniqueOrHelpId(), nMethod );
											bSendData = sal_True;
										}
										if ( m_bLog )
										{
                                            LogVCL( GetParentID( pWin ), pWin->GetType(), pWin->GetUniqueOrHelpId(), aMethod );
                                            bDone = sal_True;
										}
                                    }
                                    break;
                            }
                        }
						if ( m_bRecord )
						{
							if ( !bSendData && pWin->GetUniqueOrHelpId().getLength() )
							{
								StatementList::pRet->GenReturn( RET_MacroRecorder, pWin->GetUniqueOrHelpId(), (comm_USHORT)M_Click );
								bSendData = sal_True;
							}
						}
                        if ( m_bLog )
                        {
                            if ( !bDone )
                                LogVCL( GetParentID( pWin ), pWin->GetType(), pWin->GetUniqueOrHelpId(), CUniString("Click") );
                        }
				}
				break;
/*			case C_MoreButton:
				switch( nEventID )
				{
					case M_IsOpen :
						pRet->GenReturn ( RET_Value, nUId, ((MoreButton*)pControl)->GetState());
						break;
					case M_Click :
						((MoreButton*)pControl)->Click();
						break;
					case M_Open :
						((MoreButton*)pControl)->SetState(sal_True);
						break;
					case M_Close :
						((MoreButton*)pControl)->SetState(sal_False);
						break;
					default:
						ReportError( nUId, GEN_RES_STR2c2( S_UNKNOWN_METHOD, MethodString(nMethodId), "MoreButton" ) );
						break;
				}
				break;*/
			case WINDOW_SPINFIELD:
			case WINDOW_PATTERNFIELD:
			case WINDOW_NUMERICFIELD:
			case WINDOW_METRICFIELD:
			case WINDOW_CURRENCYFIELD:
			case WINDOW_DATEFIELD:
			case WINDOW_TIMEFIELD:
				switch( nEventID )
				{
					case VCLEVENT_SPINFIELD_UP:
					case VCLEVENT_SPINFIELD_DOWN:
					case VCLEVENT_SPINFIELD_FIRST:
					case VCLEVENT_SPINFIELD_LAST:
                        {
                            pEditModify = NULL;
                            aEditModifyString.Erase();

                            comm_USHORT nMethod;
                            String aMethod;
                            switch ( nEventID )
                            {
                                case VCLEVENT_SPINFIELD_UP: nMethod = M_More; aMethod = CUniString("More"); break;
                                case VCLEVENT_SPINFIELD_DOWN: nMethod = M_Less; aMethod = CUniString("Less"); break;
                                case VCLEVENT_SPINFIELD_FIRST: nMethod = M_ToMin; aMethod = CUniString("ToMin"); break;
                                case VCLEVENT_SPINFIELD_LAST: nMethod = M_ToMax; aMethod = CUniString("ToMax"); break;
                                default: nMethod = M_ToMin; aMethod = CUniString("Unknown");
                                    DBG_ERROR( "Unknown EventID in Spinfield" );
                            }
							if ( m_bRecord )
							{
								StatementList::pRet->GenReturn( RET_MacroRecorder, pWin->GetUniqueOrHelpId(), nMethod );
								bSendData = sal_True;
							}
                            if ( m_bLog )
                            {
                                LogVCL( GetParentID( pWin ), pWin->GetType(), pWin->GetUniqueOrHelpId(), aMethod );
                            }
                        }
                        break;
					case VCLEVENT_EDIT_MODIFY:
                        pEditModify = pWin;
                        aEditModifyString = ((SpinField*)pWin)->GetText();
                        break;
				}
				break;

			case WINDOW_MENUBUTTON:
					switch( nEventID )
					{
					case VCLEVENT_BUTTON_CLICK:
						if ( m_bRecord )
						{
							StatementList::pRet->GenReturn( RET_MacroRecorder, pWin->GetUniqueOrHelpId(), (comm_USHORT)M_Click );
							bSendData = sal_True;
						}
                        if ( m_bLog )
                        {
                            LogVCL( GetParentID( pWin ), pWin->GetType(), pWin->GetUniqueOrHelpId(), CUniString("Click") );
                        }
						break;
/*      Keyevent or Timeout
					case M_Open :
						{
							MouseEvent aMEvnt;
							Point aPt( pControl->GetSizePixel().Width() / 2, pControl->GetSizePixel().Height() / 2 );
							aMEvnt = MouseEvent( aPt,1,MOUSE_SIMPLECLICK,MOUSE_LEFT );
							ImplMouseButtonDown( pControl, aMEvnt );

							sal_uLong nStart = Time::GetSystemTicks();
							sal_uLong nDelay = pControl->GetSettings().GetMouseSettings().GetActionDelay();
							while ( ( Time::GetSystemTicks() - nStart ) < nDelay + 100 )
								SafeReschedule();

							ImplMouseButtonUp  ( pControl, aMEvnt );
						}
						break;*/
                    }
				break;
			case WINDOW_TOOLBOX:
                {
				    ToolBox *pTB = ((ToolBox*)pWin);
				    switch( nEventID )
				    {
                        case VCLEVENT_TOOLBOX_SELECT:
                            {   // a Button has been clicked
                                // so this cannot be a tearoff or OpenMenu anymore
                                pActionParent = NULL;
                                // compare to 1 for floating ToolBoxes
								if ( m_bRecord )
								{
									if ( !pWin->GetUniqueOrHelpId().getLength() /* || pWin->GetUniqueOrHelpId().Matches( 1 ) */ )
										// generate direct Button access
										StatementList::pRet->GenReturn( RET_MacroRecorder, Str2Id( pTB->GetItemCommand( pTB->GetCurItemId() ) ), (comm_USHORT)(M_Click) );
									else
										// access via Toolbox
										StatementList::pRet->GenReturn( RET_MacroRecorder, pTB->GetUniqueOrHelpId(), (comm_USHORT)(M_Click|M_RET_NUM_CONTROL), Id2Str( pTB->GetHelpId( pTB->GetCurItemId() ) ) );
									bSendData = sal_True;
								}
/* not needed                               if ( m_bLog )
                                {
                                }*/
                            }
						    break;
                        case VCLEVENT_TOOLBOX_CLICK:  /// ATTENTION this is called during initialisation of toolbox. whoever 'invented' this
                            pActionParent = pTB;
                            break;
//                        case VCLEVENT_WINDOW_SHOW:        // not usable ATM. see above
//                            if ( pActionParent )
//                            {   // a new toolbox opens up, might be a tearoff
//                                if ( pActionParent != pWin )
//                                {   // it IS a tearoff not an undock
//                                    // compare to 1 for floating ToolBoxes
//									if ( m_bRecord )
//									{
//										if ( !pWin->GetSmartUniqueOrHelpId().HasAny() || pWin->GetSmartUniqueOrHelpId().Matches( 1 ) )
//											// generate direct Button access
//											StatementList::pRet->GenReturn( RET_MacroRecorder, rtl::OString( pActionParent->GetHelpId( pActionParent->GetCurItemId() ) ), (comm_USHORT)(M_TearOff) );
//										else
//											// access via Toolbox
//											StatementList::pRet->GenReturn( RET_MacroRecorder, pActionParent->GetSmartUniqueOrHelpId(), (comm_USHORT)(M_TearOff|M_RET_NUM_CONTROL), static_cast<comm_ULONG>(pActionParent->GetHelpId( pActionParent->GetCurItemId() )) ); // GetHelpId() sal_uLong != comm_ULONG on 64bit
//										bSendData = sal_True;
//									}
//                                    if ( m_bLog )
//                                    {
//                                        LogVCL( pActionParent->GetSmartUniqueOrHelpId(), pWin->GetType(), pActionParent->GetHelpId( pActionParent->GetCurItemId() ), CUniString("TearOff") );
//                                    }
//                                }
//                                pActionParent = NULL;
//                            }
//                            break;
                        case VCLEVENT_TOOLBOX_DEACTIVATE:
                            pActionParent = NULL;
                            break;
                    }
                }
                break;
/*					ToolBox *pTB = ((ToolBox*)pControl);
					if ( pTB->GetUniqueOrHelpId() != nUId )	// Also Button auf der ToolBox gefunden
					{
						if ( nParams == PARAM_NONE )
						{			// Wir f�lschen einen Parameter
							nParams = PARAM_USHORT_1;
							nNr1 = nUId;
						}
						else
							ReportError( nUId, GEN_RES_STR1( S_INTERNAL_ERROR, MethodString( nMethodId ) ) );
					}

#define FIND_HELP\
{\
	if( nParams == PARAM_USHORT_1 )\
		nLNr1 = nNr1;\
	for ( nNr1 = 0; nNr1 < pTB->GetItemCount() && nLNr1 != pTB->GetHelpId(pTB->GetItemId(nNr1)) ; nNr1++ ) {}\
	bBool1 = nLNr1 == pTB->GetHelpId(pTB->GetItemId(nNr1));\
	if ( !bBool1 )\
		ReportError( nUId, GEN_RES_STR1( S_HELPID_ON_TOOLBOX_NOT_FOUND, MethodString( nMethodId ) ) );\
	else\
	{\
		if ( !pTB->IsItemEnabled( pTB->GetItemId(nNr1) ) && nMethodId != _M_IsEnabled )\
		{\
			ReportError( nUId, GEN_RES_STR1( S_BUTTON_DISABLED_ON_TOOLBOX, MethodString( nMethodId ) ) );\
			bBool1 = sal_False;\
		}\
		else if ( !pTB->IsItemVisible( pTB->GetItemId(nNr1) ) )\
		{\
			ReportError( nUId, GEN_RES_STR1( S_BUTTON_HIDDEN_ON_TOOLBOX, MethodString( nMethodId ) ) );\
			bBool1 = sal_False;\
		}\
		else\
		{\
			if ( pTB->GetItemRect(pTB->GetItemId(nNr1)).IsEmpty() )\
			{\
				sal_uInt16 nLine = pTB->GetCurLine();\
				do\
				{\
					pTB->ShowLine( sal_False );\
					for ( int i = 1 ; i < 30 ; i++ )\
						SafeReschedule();\
				}\
				while ( pTB->GetCurLine() != nLine && pTB->GetItemRect(pTB->GetItemId(nNr1)).IsEmpty() );\
				pTB->Invalidate( pTB->GetScrollRect() );\
			}\
			if ( pTB->GetItemRect(pTB->GetItemId(nNr1)).IsEmpty() )\
			{\
				ReportError( nUId, GEN_RES_STR1( S_CANNOT_MAKE_BUTTON_VISIBLE_IN_TOOLBOX, MethodString( nMethodId ) ) );\
				bBool1 = sal_False;\
			}\
		}\
	}\
}\

					switch( nEventID )
					{
						case M_SetNextToolBox :
							if ( (nParams & PARAM_STR_1) )
								pTB->SetNextToolBox( aString1 );
							else
								pTB->SetNextToolBox( pTB->GetNextToolBox() );
							pTB->NextToolBox();
							break;
						case M_GetNextToolBox :
							pRet->GenReturn ( RET_Value, nUId, (String)pTB->GetNextToolBox());
							break;
						default:
							ReportError( nUId, GEN_RES_STR2c2( S_UNKNOWN_METHOD, MethodString(nMethodId), "ToolBox" ) );
							break;
					}
				}
				break;
*/
			case WINDOW_CONTROL:
			case WINDOW_WINDOW:
				switch( nEventID )
                {
                    case VCLEVENT_WINDOW_COMMAND:
                        break;
                    case VCLEVENT_WINDOW_KEYINPUT:
                        {
                            const KeyEvent *pKeyEvent = ((KeyEvent*)pWinEvent->GetData());
                            const KeyCode aKeyCode = pKeyEvent->GetKeyCode();
                            if ( pKeyEvent )
                            {
                                pKeyWin = pWin;
                                Window *pIdWin = GetParentWithID( pWin );
								if ( pIdWin != pWin )
                                   bKeyFollowFocus = sal_True;
                                aKeyUniqueID = pIdWin->GetUniqueOrHelpId();
		                        if ( m_bLog )
		                        {
//   HACK Too many KeyEvents generated                                 if ( aKeyString.Len() == 0 )
//   HACK Too many KeyEvents generated                                     LogVCL( rtl::OString(), 0, aKeyUniqueID, CUniString("TypeKeysStart") );
		                        }
						        if ( ( !aKeyCode.IsMod1() && !aKeyCode.IsMod2() ) &&
							          (( aKeyCode.GetGroup() == KEYGROUP_NUM)   ||
							           ( aKeyCode.GetGroup() == KEYGROUP_ALPHA) ||
							           ( aKeyCode.GetCode() == KEY_SPACE) ||
							           ( aKeyCode.GetCode() == KEY_ADD) ||
							           ( aKeyCode.GetCode() == KEY_SUBTRACT) ||
							           ( aKeyCode.GetCode() == KEY_MULTIPLY) ||
							           ( aKeyCode.GetCode() == KEY_DIVIDE) ||
							           ( aKeyCode.GetCode() == KEY_POINT) ||
							           ( aKeyCode.GetCode() == KEY_COMMA) ||
							           ( aKeyCode.GetCode() == KEY_EQUAL) ||
							           ( aKeyCode.GetCode() == 0) ) )
                                {
                                    DBG_ASSERT( pKeyEvent->GetCharCode(), "no charcode found" );
                                    aKeyString += pKeyEvent->GetCharCode();
                                }
                                else
                                {   // not a regular key, transfer KeyCode
                                    aKeyString += sal_Unicode(1);   // mask it
                                    // extra for '>' which is coded as <SHIFT GREATER>
							        if ( pKeyEvent->GetCharCode() == '>' )
                                        aKeyString += sal_Unicode( KEY_GREATER | (aKeyCode.GetAllModifier() & ~KEY_SHIFT) );
                                    else
                                        aKeyString += sal_Unicode( aKeyCode.GetCode() | aKeyCode.GetAllModifier() );
                                }
                            }
                        }
                        break;

                    case VCLEVENT_WINDOW_MOUSEMOVE:
                    case VCLEVENT_WINDOW_MOUSEBUTTONDOWN:
                    case VCLEVENT_WINDOW_MOUSEBUTTONUP:
                        {
                        }
                        break;


                }
                break;
  		    case WINDOW_DOCKINGWINDOW:
//                switch( nEventID )
				{
//					case 1 .. 0xffff:
    DBG_TRACE3( "TT_VCLMessage %u %u  %X",nEventID, pWin->GetType(), pWin );
//						sal_Bool bx = ((DockingWindow*)pWin)->IsFloatingMode();
//						break;
/*					case M_Dock :
						if ( ((DockingWindow*)pControl)->IsFloatingMode() )
							((DockingWindow*)pControl)->SetFloatingMode(sal_False);
						else
							ReportError( nUId, GEN_RES_STR1( S_ALLOWED_ONLY_IN_FLOATING_MODE, MethodString( nMethodId ) ) );
						break;
					case M_Undock :
						if ( !((DockingWindow*)pControl)->IsFloatingMode() )
							((DockingWindow*)pControl)->SetFloatingMode(sal_True);
						else
							ReportError( nUId, GEN_RES_STR1( S_ALLOWED_ONLY_IN_FLOATING_MODE, MethodString( nMethodId ) ) );
						break;
					case M_IsDocked :
						pRet->GenReturn ( RET_Value, nUId, (comm_BOOL) !((DockingWindow*)pControl)->IsFloatingMode());
						break;
					case M_Close:
							//nWindowWaitUId = nUId;
						DBG_ASSERT( nUId == pControl->GetUniqueOrHelpId(), "nUID != UniqueOrHelpId");
						SET_WINP_CLOSING(pControl);
						((DockingWindow*)pControl)->Close();
						break;
					case M_Size:
					case M_Move:
					case M_IsMax :
					case M_Minimize :
					case M_Maximize :
						if ( ((DockingWindow*)pControl)->IsFloatingMode() )
						{
							pControl = ((DockingWindow*)pControl)->GetFloatingWindow();
							goto FloatWin;
						}
						else
							ReportError( nUId, GEN_RES_STR1( S_ALLOWED_ONLY_IN_DOCKING_MODE, MethodString( nMethodId ) ) );
						break;
					case M_Help:		// Alles was unten weiterbehandelt werden soll
						goto MoreDialog;

					default:
						ReportError( nUId, GEN_RES_STR2c2( S_UNKNOWN_METHOD, MethodString(nMethodId), "DockingWindow" ) );
						break;*/
				}
				break;






			case WINDOW_FLOATINGWINDOW:
                {
    DBG_TRACE3( "TT_VCLMessage %u %u  %X",nEventID, pWin->GetType(), pWin );
//                    FloatingWindow *pFW = ((FloatingWindow*)pWin);
/*				    switch( nEventID )
				    {

// M_OpenMenu an einem ToolboxButton
                        case VCLEVENT_WINDOW_SHOW:
                            if ( pActionParent )
                            {   // a new FloatingWindow opens up, so we assume an OpenMenu
                                // compare to 1 for floating ToolBoxes
                                if ( ( pActionParent->GetUniqueOrHelpId() == 0 || pActionParent->GetUniqueOrHelpId() == 1 ) )
                                    // generate direct Button access
                                    StatementList::pRet->GenReturn( RET_MacroRecorder, pActionParent->GetHelpId( pActionParent->GetCurItemId() ), (comm_USHORT)(M_OpenMenu) );
                                else
                                    // access via Toolbox
                                    StatementList::pRet->GenReturn( RET_MacroRecorder, pActionParent->GetUniqueOrHelpId(), (comm_USHORT)(M_OpenMenu|M_RET_NUM_CONTROL), pActionParent->GetHelpId( pActionParent->GetCurItemId() ) );
                                bSendData = sal_True;
                            }
                            break;
  
                    }
  */              }
                break;
/*
					case M_AnimateMouse :
						AnimateMouse( pControl, MitteOben);
						break;
					case M_IsMax :
						pRet->GenReturn ( RET_Value, nUId, (comm_BOOL)!((FloatingWindow*)pControl)->IsRollUp());
						break;
					case M_Minimize :
						((FloatingWindow*)pControl)->RollUp();
						break;
					case M_Maximize :
						((FloatingWindow*)pControl)->RollDown();
						break;
					case M_Size:
					{
						if ( pControl->GetStyle() & WB_SIZEABLE )
						{
							pControl->SetSizePixel(Size(nNr1,nNr2));
							pControl->Resize();
						}
						else
							ReportError( nUId, GEN_RES_STR1( S_SIZE_NOT_CHANGEABLE, MethodString( nMethodId ) ) );
						break;
					}
					case M_Close:
						DBG_ASSERT( nUId == pControl->GetUniqueOrHelpId(), "nUID != UniqueOrHelpId");
						SET_WINP_CLOSING(pControl);
						((FloatingWindow*)pControl)->Close();
						break;
					case M_Help:		// Alles was unten weiterbehandelt werden soll
					case M_Move:
						goto MoreDialog;
					default:
						ReportError( nUId, GEN_RES_STR2c2( S_UNKNOWN_METHOD, MethodString(nMethodId), "FloatingWin" ) );
						break;
				}
				break;*/








/*
			case C_ModelessDlg:
			case C_Dlg:
			case C_TabDlg:
				MoreDialog:
				switch( nEventID )
				{

					// (Rect GetRect)

					case M_AnimateMouse :
						AnimateMouse( pControl, MitteOben);
						break;
					case M_Close:
						DBG_ASSERT( nUId == pControl->GetUniqueOrHelpId(), "nUID != UniqueOrHelpId");
						SET_WINP_CLOSING(pControl);
						((SystemWindow*)pControl)->Close();
						break;
					case M_Move:
					{
						pControl->SetPosPixel(Point(nNr1,nNr2));
						break;
					}
					default:
						ReportError( nUId, GEN_RES_STR2c2( S_UNKNOWN_METHOD, MethodString(nMethodId), "Dialog" ) );
						break;
				}
				break;*/













/*
			case C_WorkWin:
				switch( nEventID )
				{
					case M_Close:
						DBG_ASSERT( nUId == pControl->GetUniqueOrHelpId(), "nUID != UniqueOrHelpId");
						SET_WINP_CLOSING(pControl);
						((WorkWindow*)pControl)->Close();
						break;
					case M_Size:
					case M_Move:
						goto FloatWin;
						break;
					case M_Help:		// Alles was unten weiterbehandelt werden soll
						goto MoreDialog;
					default:
						ReportError( nUId, GEN_RES_STR2c2( S_UNKNOWN_METHOD, MethodString(nMethodId), "WorkWindow" ) );
						break;
				}
				break;
  */


/*			case C_TabPage:
				switch( nEventID )
				{
				}
				break;*/













/*
			case C_MessBox:
			case C_InfoBox:
			case C_WarningBox:
			case C_ErrorBox:
			case C_QueryBox:
				{
					sal_Bool bDone = sal_True;
					MessBox* pMB = (MessBox*)pControl;
					switch( nEventID )
					{
						case M_GetCheckBoxText:
							pRet->GenReturn ( RET_Value, nUId, pMB->GetCheckBoxText() );
							break;
						case M_IsChecked :
							pRet->GenReturn ( RET_Value, nUId, comm_BOOL( pMB->GetCheckBoxState() == STATE_CHECK) );
							break;
						case M_GetState :
							pRet->GenReturn ( RET_Value, nUId, comm_ULONG( pMB->GetCheckBoxState() ));
							break;
						case M_Check :
							pMB->SetCheckBoxState( STATE_CHECK );
							break;
						case M_UnCheck :
							pMB->SetCheckBoxState( STATE_NOCHECK );
							break;
						case M_GetText :
							pRet->GenReturn ( RET_Value, nUId, pMB->GetMessText());
							break;

						default:
							bDone = sal_False;
							break;
					}
					if ( bDone )
						break;	// break the case here else continue at C_ButtonDialog
				}
			case C_ButtonDialog:
				{
					ButtonDialog* pBD = (ButtonDialog*)pControl;
#if OSL_DEBUG_LEVEL > 1
					m_pDbgWin->AddText( "Working MessBox: " );
					if (pControl->IsVisible())
						m_pDbgWin->AddText("*(Visible)\n");
					else
						m_pDbgWin->AddText("*(nicht Visible)\n");
#endif
					switch( nEventID )
					{
						case M_GetText :
							pRet->GenReturn ( RET_Value, nUId, pControl->GetText());
							break;
						case M_Click:
							if ( nParams & PARAM_USHORT_1 )
							{
								if ( pBD->GetPushButton( nNr1 ) )
								{
									if ( nNr1 != BUTTONID_HELP )
									{
										SET_WINP_CLOSING(pControl);
									}
									pBD->GetPushButton( nNr1 )->Click();
								}
								else
									ReportError( nUId, GEN_RES_STR2( S_NO_DEFAULT_BUTTON, UniString::CreateFromInt32( nNr1 ), MethodString( nMethodId ) ) );
							}
							else
								ReportError( nUId, GEN_RES_STR1( S_BUTTONID_REQUIRED, MethodString( nMethodId ) ) );
							break;
						case M_GetButtonCount :
							pRet->GenReturn ( RET_Value, nUId, comm_ULONG(pBD->GetButtonCount()));
							break;
						case M_GetButtonId :
							if ( ValueOK(nUId, MethodString( nMethodId ),nNr1,pBD->GetButtonCount()) )
								pRet->GenReturn ( RET_Value, nUId, comm_ULONG(pBD->GetButtonId(nNr1-1)));
							break;
						default:
							ReportError( nUId, GEN_RES_STR2c2( S_UNKNOWN_METHOD, MethodString(nMethodId), "MessageBox" ) );
							break;
					}
					break;


                 */


        }


		switch( nEventID )
		{
			case VCLEVENT_CONTROL_LOSEFOCUS:
                if ( pEditModify == pWin )
                {
					if ( m_bRecord )
					{
						StatementList::pRet->GenReturn( RET_MacroRecorder, pWin->GetUniqueOrHelpId(), M_SetText, aEditModifyString );
						bSendData = sal_True;
					}
                    if ( m_bLog )
                    {
                        LogVCL( GetParentID( pWin ), pWin->GetType(), pWin->GetUniqueOrHelpId(), CUniString("Modify") );
                    }
                    pEditModify = NULL;
                    aEditModifyString.Erase();  //could be somewhat lengthy
                }
				break;
        }

        pLastWin = pWin;

    }  // if
    else if ( pEvent->ISA( VclMenuEvent ) )
    {
//        VclMenuEvent* pMenuEvent = ( VclMenuEvent* ) pEvent;
    }


    if ( bSendData )
        new StatementFlow( NULL, F_EndCommandBlock );	// Kommando zum Senden erzeugen und in que eintragen

    return 0;
}



static ::osl::Mutex * getRecorderMutex()
{
    static ::osl::Mutex * pMutex  = NULL;
    if(pMutex==NULL)
    {
        ::osl::MutexGuard aGuard(::osl::Mutex::getGlobalMutex());
        if(pMutex==NULL)
            pMutex = new ::osl::Mutex();
    }
    return pMutex;
}


void MacroRecorder::CheckDelete()
{
	::osl::MutexGuard aGuard( getRecorderMutex() );
    if ( !m_bRecord && !m_bLog )
    {
        pMacroRecorder = NULL;
        delete this;
    }
}


MacroRecorder* MacroRecorder::GetMacroRecorder()
{
	::osl::MutexGuard aGuard( getRecorderMutex() );
    if ( !pMacroRecorder )
        pMacroRecorder = new MacroRecorder;

    return pMacroRecorder;
}

sal_Bool MacroRecorder::HasMacroRecorder()
{
	::osl::MutexGuard aGuard( getRecorderMutex() );
    return pMacroRecorder != NULL;
}

