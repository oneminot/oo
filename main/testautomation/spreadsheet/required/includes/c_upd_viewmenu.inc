'encoding UTF-8  Do not remove or change this line!
'*************************************************************************
'
'  Licensed to the Apache Software Foundation (ASF) under one
'  or more contributor license agreements.  See the NOTICE file
'  distributed with this work for additional information
'  regarding copyright ownership.  The ASF licenses this file
'  to you under the Apache License, Version 2.0 (the
'  "License"); you may not use this file except in compliance
'  with the License.  You may obtain a copy of the License at
'  
'    http://www.apache.org/licenses/LICENSE-2.0
'  
'  Unless required by applicable law or agreed to in writing,
'  software distributed under the License is distributed on an
'  "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
'  KIND, either express or implied.  See the License for the
'  specific language governing permissions and limitations
'  under the License.
'
'*************************************************************************
'*
'* short description : Resource Test - View Menu
'*
'************************************************************************
'*
' #1 tViewPageBreakPreview
' #1 tViewToolbars
' #1 tViewFormularbar
' #1 tViewStatusbar
' #1 tViewInputMethodStatus
' #1 tViewColumnRowHeaders
' #1 tViewValueHighlighting
' #1 tViewDataSources
' #1 tViewNavigator
' #1 tViewFullScreen
' #1 tViewZoom
'*
'\***********************************************************************

sub c_upd_viewmenu

    Printlog Chr(13) + "--------- View Menu (c_upd_viewmenu.inc) ---------"
    
    call tViewPageBreakPreview
    call tViewToolbars
    call tViewFormularbar
    call tViewStatusbar
    call tViewInputMethodStatus
    call tViewColumnRowHeaders
    call tViewValueHighlighting
    call tViewDataSources
    call tViewNavigator
    call tViewFullScreen
    call tViewZoom

end sub

'-----------------------------------------------------------

testcase tViewPageBreakPreview
'///<u><b>View - Normal / Page Break Preview</b></u>

    '/// Opening new spreadsheet document for getting defined starting environment
    printlog " Opening new spreadsheet document for getting defined starting environment"
    call hNewDocument
    '/// Activate Page Break Preview by 'View – Page Break Preview'
    printlog " Activate Page Break Preview by 'View – Page Break Preview'"
    ViewPageBreakPreview
    '/// Switch back to normal view by 'View – Normal'
    printlog " Switch back to normal view by 'View – Normal'"
    ViewNormalView
    '/// Close starting document
    printlog " Close starting document"
    call hCloseDocument

endcase

'-----------------------------------------------------------

testcase tViewToolbars
'///<u><b>View - Toolbars</b></u>

    '/// Opening new spreadsheet document for getting defined starting environment
    printlog " Opening new spreadsheet document for getting defined starting environment"
    call hNewDocument
    '/// Switch on/off toolbar '3D-Settings' by 'View - Toolbars'
    printlog " Switch on/off toolbar '3D-Settings' by 'View - Toolbars'"
    ViewToolbarsThreeDSettings
    ViewToolbarsThreeDSettings
    '/// Switch on/off toolbar 'Align' by 'View - Toolbars'
    printlog " Switch on/off toolbar 'Align' by 'View - Toolbars'"
    ViewToolbarsAlign
    ViewToolbarsAlign
    '/// Switch on/off toolbar 'Drawing' by 'View - Toolbars'
    printlog " Switch on/off toolbar 'Drawing' by 'View - Toolbars'"
    ViewToolbarsDrawing
    ViewToolbarsDrawing
    '/// Switch on/off toolbar 'Drawing Objects Properties' by 'View - Toolbars'
    printlog " Switch on/off toolbar 'Drawing Objects Properties' by 'View - Toolbars'"
    ViewToolbarsDrawObjectbar
    ViewToolbarsDrawObjectbar
    '/// Switch on/off toolbar 'Fontwork' by 'View - Toolbars'
    printlog " Switch on/off toolbar 'Fontwork' by 'View - Toolbars'"
    ViewToolbarsFontwork
    ViewToolbarsFontwork
    '/// Switch on/off toolbar 'Form Controls' by 'View - Toolbars'
    printlog " Switch on/off toolbar 'Form Controls' by 'View - Toolbars'"
    ViewToolbarsFormControls
    ViewToolbarsFormControls
    '/// Switch on/off toolbar 'Form Design' by 'View - Toolbars'
    printlog " Switch on/off toolbar 'Form Design' by 'View - Toolbars'"
    ViewToolbarsFormDesign
    ViewToolbarsFormDesign
    '/// Switch on/off toolbar 'Form Navigation' by 'View - Toolbars'
    printlog " Switch on/off toolbar 'Form Navigation' by 'View - Toolbars'"
    ViewToolbarsFormNavigation
    ViewToolbarsFormNavigation
    '/// Switch on/off toolbar 'Formatting' by 'View - Toolbars'
    printlog " Switch on/off toolbar 'Formatting' by 'View - Toolbars'"
    ViewToolbarsFormatting
    ViewToolbarsFormatting
    '/// Switch on/off toolbar 'Insert' by 'View - Toolbars'
    printlog " Switch on/off toolbar 'Insert' by 'View - Toolbars'"
    ViewToolbarsInsert
    ViewToolbarsInsert
    '/// Switch on/off toolbar 'Insert Cells' by 'View - Toolbars'
    printlog " Switch on/off toolbar 'Insert Cells' by 'View - Toolbars'"
    ViewToolbarsInsertCell
    ViewToolbarsInsertCell
    '/// Switch on/off toolbar 'Media Playback' by 'View - Toolbars'
    printlog " Switch on/off toolbar 'Media Playback' by 'View - Toolbars'"
    ViewToolbarsMediaPlayback
    ViewToolbarsMediaPlayback
    '/// Switch on/off toolbar 'Picture' by 'View - Toolbars'
    printlog " Switch on/off toolbar 'Picture' by 'View - Toolbars'"
    ViewToolbarsPicture
    ViewToolbarsPicture
    '/// Switch on/off toolbar 'Standard' by 'View - Toolbars'
    printlog " Switch on/off toolbar 'Standard' by 'View - Toolbars'"
    ViewToolbarsStandard
    ViewToolbarsStandard
    '/// Switch on/off toolbar 'Text Formatting' by 'View - Toolbars'
    printlog " Switch on/off toolbar 'Text Formatting' by 'View - Toolbars'"
    ViewToolbarsTextFormatting
    ViewToolbarsTextFormatting
    '/// Switch on/off toolbar 'Tools' by 'View - Toolbars'
    printlog " Switch on/off toolbar 'Tools' by 'View - Toolbars'"
    ViewToolbarsTools
    ViewToolbarsTools
    '/// Switch on/off toolbar 'Hyperlink Bar' by 'View - Toolbars'
    printlog " Switch on/off toolbar 'Hyperlink Bar' by 'View - Toolbars'"
    ViewToolbarsInsertHyperlink
    ViewToolbarsInsertHyperlink
    '/// Switch on/off toolbar 'Formular Bar' by 'View - Toolbars'
    printlog " Switch on/off toolbar 'Formular Bar' by 'View - Toolbars'"
    ViewToolbarsFormulaBar
    ViewToolbarsFormulaBar
    '/// Open 'Toolbar Customize' - dialog by 'View – Toolbars – Customize' (will be tested in detail by tToolsCustomize)
    printlog " Open 'Toolbar Customize' - dialog by 'View – Toolbars – Customize' (will be tested in detail by tToolsCustomize)"
    ViewToolbarsConfigure
    '/// Close dialog with 'Cancel'
    Kontext
    Active.SetPage TabCustomizeMenu
    Kontext "TabCustomizeMenu"
    TabCustomizeMenu.Cancel
    '/// Reset toolbars by 'View - Toolbars -  Reset'
    printlog " Reset toolbars by 'View - Toolbars -  Reset'"
    Kontext "DocumentCalc"
    DocumentCalc.UseMenu 'Selecting Menubar
    hMenuSelectNr(3) 'Using third entry on Menubar (VIEW)
    hMenuSelectNr(3) 'Using third entry on menu (TOOLBARS)
    hMenuFindSelect(22, true, 22) 'Using last entry on menu (RESET)
'    ViewToolbarsReset
    '/// Close document
    printlog " Close starting document"
    call hCloseDocument
    
endcase

'-----------------------------------------------------------

testcase tViewFormularbar
'///<u><b>View - Formular Bar</b></u>
   
    '/// Open new Spreadsheet document
    printlog " Open new Spreadsheet document"
    call hNewDocument
    '/// Switch on/off toolbar 'Formular Bar' by 'View -  Formular Bar'
    printlog " Switch on/off toolbar 'Formular Bar' by 'View -  Formular Bar'"
    ViewToolbarsFormulaBar
    ViewToolbarsFormulaBar
    '/// Close new document
    printlog " Close document"
    call hCloseDocument

endcase

'-----------------------------------------------------------

testcase tViewStatusbar
'///<u><b>View - Status Bar</b></u>
   
    '/// Open new Spreadsheet document
    printlog " Open new Spreadsheet document"
    call hNewDocument
    '/// Switch on/off toolbar 'Status Bar' by 'View -  Status Bar'
    printlog " Switch on/off toolbar 'Status Bar' by 'View -  Status Bar'"
    ViewToolbarsStatusbar
    ViewToolbarsStatusbar
    '/// Close new document
    printlog " Close document"
    call hCloseDocument

endcase

'-----------------------------------------------------------

testcase tViewInputMethodStatus
'///<u><b>View - Input Method Status</b></u>
   
    '/// Open new Spreadsheet document
    printlog " Open new Spreadsheet document"
    call hNewDocument
    '/// If enabled (only on UNIX) switch on/off 'View – Input Method Status'
    printlog " If enabled (only on UNIX) switch on/off 'View – Input Method Status'"
    try 
        ViewInputMethodStatus
        ViewInputMethodStatus
    catch
        printlog "Input Method Status not available on this system"
    endcatch
    '/// Close new document
    printlog " Close document"
    call hCloseDocument

endcase

'-----------------------------------------------------------

testcase tViewColumnRowHeaders
'///<u><b>View - Column And Row Headers</b></u>
   
    '/// Open new Spreadsheet document
    printlog " Open new Spreadsheet document"
    call hNewDocument
    '/// Switch on/off  'View -  Column And Row Headers'
    printlog " Switch on/off  'View -  Column And Row Headers'"
    ViewColumnAndRowHeaders
    ViewColumnAndRowHeaders
    '/// Close new document
    printlog " Close document"
    call hCloseDocument

endcase

'-----------------------------------------------------------

testcase tViewValueHighlighting
'///<u><b>View - Value Highlighting</b></u>

    '/// Open new Spreadsheet document
    printlog " Open new Spreadsheet document"
    call hNewDocument
    '/// Switch on/off  'View -  Value Highlighting'
    printlog " Switch on/off  'View -  Value Highlighting'"
    ViewValueHightlighting
    ViewValueHightlighting
    '/// Close new document
    printlog " Close document"
    call hCloseDocument

endcase

'-----------------------------------------------------------

testcase tViewDataSources
'///<u><b>View - Data Sources</b></u>

    '/// Open new Spreadsheet document
    printlog " Open new Spreadsheet document"
    call hNewDocument
    '/// Open datasourcebeamer by  'View -  Data Sources'
    ViewCurrentDatabaseCalc
    '/// Close datasourcebeamer by  'View -  Data Sources'
    ViewCurrentDatabaseCalc
    '/// Close new document
    printlog " Close document"
    call hCloseDocument

endcase

'-----------------------------------------------------------

testcase tViewNavigator
'///<u><b>Edit - Navigator</b></u>

    '/// Open new Spreadsheet document
    printlog " Open new Spreadsheet document"
    call hNewDocument
    '/// Check if Navigator already exists. If not open Navigator by View-Navigator.
    printlog " Check if Navigator already exists. If not open Navigator by View-Navigator."
    Kontext "NavigatorCalc"
    if NavigatorCalc.exists then
        warnlog "Navigator is already enabled"
    else
        ViewNavigator
    end if
    Kontext "NavigatorCalc"
    call DialogTest ( NavigatorCalc )
    '/// Check functionality of button for content by clicking it twice
    printlog " Check functionality of button for content by clicking it twice"
    Inhalte.Click
    Inhalte.Click
    '/// Check functionality of toggle button by clicking it twice
    printlog " Check functionality of toggle button by clicking it twice"
    Umschalten.Click
    Umschalten.Click
    '/// Check functionality of button for scenarios by clicking it twice
    printlog " Check functionality of button for scenarios by clicking it twice"
    Szenarien.Click
    Szenarien.Click
    '/// Check that the dragmodus has 3 modes
    qaerrorlog "Dragmodus disabled because of unreliability"
'    printlog " Check that the dragmodus has 3 modes"
'    Dragmodus.OpenMenu
'    waitslot(1000)
'    if hMenuItemGetCount <> 3 then
'        warnlog "Not enough modes for dragmodus"
'    end if
'    hMenuSelectNr (0)
    '/// Close Navigator
    printlog " Close Navigator"
    Kontext "Navigator"
    Navigator.Close    'Because of #i78307 the navigatorwindow has to be closed by Navigator.close
    Kontext "NavigatorCalc"
    if NavigatorCalc.exists then
        ViewNavigator
        Warnlog "NavigatorCalc.Close doesn't close the navigator"
    end if
    '/// Close document
    printlog " Close document"
    hCloseDocument

endcase

'-----------------------------------------------------------

testcase tViewFullScreen
'///<u><b>View - Full Screen</b></u>

    '/// Open new Spreadsheet document
    printlog " Open new Spreadsheet document"
    call hNewDocument
    '/// Switch to full screen view by  'View -  Full Screen'
    ViewFullScreen
    '/// Switch back to normal view
    ViewFullScreen
    '/// Close new document
    printlog " Close document"
    call hCloseDocument

endcase

'-----------------------------------------------------------

testcase tViewZoom
'///<u><b>View - Zoom</b></u>

    '/// Open new Spreadsheet document
    printlog " Open new Spreadsheet document"
    call hNewDocument
    '/// Open 'Zoom' – dialog by 'View – Zoom'
    printlog " Open 'Zoom' – dialog by 'View – Zoom'"
    ViewZoom
    Kontext "Massstab"
    DialogTest ( Massstab )
    '/// Check 'Variable'
    printlog " Check 'Variable'"
    VergroesserungStufenlos.Check
    '/// Increase variablezoom by 1
    printlog " Increase variablezoom by 1"
    Stufenlos.More
    '/// Close dialog with 'Cancel'
    printlog " Close dialog with 'Cancel'"
    Massstab.Cancel
    '/// Close new document
    printlog " Close document"
    call hCloseDocument

endcase

