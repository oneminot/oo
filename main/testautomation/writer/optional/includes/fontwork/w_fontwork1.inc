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
'* short description : Test Format/Fontwork
'*
'************************************************************************
'*
' #1 tFontWork_1
' #1 tFontWork_2
' #1 tFontWork_3
' #1 tFontWork_4
' #1 tFontWork_5
' #1 tFontWork_6
'*
'\***********************************************************************

testcase tFontWork_1

    Dim i as Integer
    
    printLog Chr(13) + "- Test pre-defined shapes (upper/lower Semicircle,...) to the selected text object"
    '/// Test pre-defined shapes (upper/lower Semicircle,...) to the selected text object
    
    Call hNewDocument
    
    Call wZeichenobjektEinfuegen("TEXT", 30, 40, 60, 60)
    Call wTypeKeys "First Line<Return>"
    Call wTypeKeys "Second Line<Return>"
    Call wTypeKeys "Third Line<Return>"
    Kontext "DocumentWriter"
    Call wTypeKeys "<ESCAPE>"
    
    '/// format/fontwork
    FormatFontWork
    Kontext
    Kontext "Fontwork"
    
    '/// test Rotate
    Drehen.Click
    For i = 1 to 12
        kreisAuswahl.TypeKeys "<Right>"
        Sleep 3
    next i
    
    '/// test Upright
    Aufrecht.Click
    For i = 1 to 12
        kreisAuswahl.TypeKeys "<Right>"
        Sleep 3
    next i
    
    '/// test Slant Horizontal
    HorizontalKippen.Click
    For i = 1 to 12
        kreisAuswahl.TypeKeys "<Right>"
        Sleep 3
    next i
    
    '/// test Slant Vertical
    VertikalKippen.Click
    For i = 1 to 12
        kreisAuswahl.TypeKeys "<Right>"
        Sleep 3
    next i
    
    Fontwork.Close
    
    Call hCloseDocument

endcase

'-------------------------------------------------------------------------

testcase tFontWork_2

    Dim i , j as Integer
    
    printLog Chr(13) + "- Test Orientation"
    '/// Test Orientation
    
    Call hNewDocument
    
    Call wZeichenobjektEinfuegen("TEXT", 30, 40, 60, 60)
    Call wTypeKeys "Test"
    Kontext "DocumentWriter"
    Call wTypeKeys "<ESCAPE>"
    
    '/// format/fontwork
    FormatFontWork
    Kontext "Fontwork"
    
    '/// Test Orientation in Rotate
    Drehen.Click
    kreisAuswahl.TypeKeys "<Right>"
    Laufrichtgung.Click
    Sleep 2
    Laufrichtgung.Click
    
    '/// Test Orientation in Upright
    Aufrecht.Click
    kreisAuswahl.TypeKeys "<Right>"
    Laufrichtgung.Click
    Sleep 2
    Laufrichtgung.Click
    
    '/// Test Orientation in Horizontal
    HorizontalKippen.Click
    kreisAuswahl.TypeKeys "<Right>"
    Laufrichtgung.Click
    Sleep 2
    Laufrichtgung.Click
    
    '/// Test Orientation in Slant Vertical
    VertikalKippen.Click
    kreisAuswahl.TypeKeys "<Right>"
    Laufrichtgung.Click
    Sleep 2
    Laufrichtgung.Click
    
    Fontwork.Close
    
    Call hCloseDocument

endcase

'-------------------------------------------------------------------------

testcase tFontWork_3

    Dim i , j as Integer
    
    printLog Chr(13) + "- Test Align , include Left , Center, Right and Autosize ."
    '/// Test Align , include Left , Center, Right and Autosize
    
    Call hNewDocument
    
    Call wZeichenobjektEinfuegen("TEXT", 30, 40, 60, 60)
    Call wTypeKeys "Test"
    Kontext "DocumentWriter"
    Call wTypeKeys "<ESCAPE>"
    
    '/// format/fontwork
    FormatFontWork
    Kontext "Fontwork"
    
    '/// Test Align in Rotate
    Drehen.Click
    kreisAuswahl.TypeKeys "<Right>"
    Linksbuendig.Click
    Sleep 2
    Zentriert.Click
    Sleep 2
    Rechtsbuendig.Click
    Sleep 2
    
    '/// Test Align in Upright
    Aufrecht.Click
    kreisAuswahl.TypeKeys "<Right>"
    Linksbuendig.Click
    Sleep 2
    Zentriert.Click
    Sleep 2
    Rechtsbuendig.Click
    Sleep 2
    
    '/// Test Align in Horizontal
    HorizontalKippen.Click
    kreisAuswahl.TypeKeys "<Right>"
    Linksbuendig.Click
    Sleep 2
    Zentriert.Click
    Sleep 2
    Rechtsbuendig.Click
    Sleep 2
    
    '/// Test Align in Slant Vertical
    VertikalKippen.Click
    kreisAuswahl.TypeKeys "<Right>"
    Linksbuendig.Click
    Sleep 2
    Zentriert.Click
    Sleep 2
    Rechtsbuendig.Click
    Sleep 2
    
    Fontwork.Close
    
    Call hCloseDocument

endcase

'-------------------------------------------------------------------------

testcase tFontWork_4

    Dim sDistance , sIndent as String
    
    sDistance = "1" + gSeperator + "00" + gMeasurementUnit
    sIndent   = "1" + gSeperator + "10" + gMeasurementUnit
    
    printLog Chr(13) + "- Test distance and indent"
    '/// Test distance and indent
    
    Call hNewDocument
    
    Call wZeichenobjektEinfuegen("TEXT", 30, 40, 60, 60)
    Call wTypeKeys "Test"
    Kontext "DocumentWriter"
    Call wTypeKeys "<ESCAPE>"
    
    '/// format/fontwork , set distance and indent
    FormatFontWork
    Kontext "Fontwork"
    Drehen.Click
    Sleep 4
    kreisAuswahl.TypeKeys "<Right>"
    Sleep 5
    Linksbuendig.Click
    Sleep 5
    Abstand.SetText   sDistance
    Sleep 5
    Einzug.SetText    sIndent
    Sleep 5
    Fontwork.Close
    Sleep 5
    
    FormatFontWork
    Kontext
    Kontext "Fontwork"
    Sleep 2
    if Left$(Abstand.GetText,4) <> Left$(sDistance,4) then Warnlog "Distance should be "+sDistance+"  but -> "+Abstand.GetText
    Sleep 2
    if Left$(Einzug.GetText,4) <> Left$(sIndent,4) then Warnlog "Indent should be "+sIndent+"  but -> "+Einzug.GetText
    Sleep 2
    Fontwork.Close
    
    Call hCloseDocument

endcase

'-------------------------------------------------------------------------

testcase tFontWork_5

    printLog Chr(13) + "- Test Contour - include contour and text contour"
    '/// Test Contour - include contour and text contour
    
    Call hNewDocument
    
    Call wZeichenobjektEinfuegen("TEXT", 30, 40, 60, 60)
    Call wTypeKeys "Test"
    Kontext "DocumentWriter"
    Call wTypeKeys "<ESCAPE>"
    
    '/// format/fontwork
    FormatFontWork
    Kontext
    Kontext "Fontwork"
    Drehen.Click
    kreisAuswahl.TypeKeys "<Right>"
    Kontur.Click
    Buchstabenumrandung.Click
    Sleep 2
    Kontur.Click
    Buchstabenumrandung.Click
    Fontwork.Close
    
    Call hCloseDocument

endcase

'-------------------------------------------------------------------------

testcase tFontWork_6

    Dim sDistanceX , sDistanceY                    as String
    Dim sAngle     , SPercent                      as String
    Dim iVerticalShadowColor , iSlantedShadowColor as Integer
    
    Select case iSprache
        case 01   : sAngle = "140" + gSeperator + "0 degrees"
        case 03   : sAngle = "140" + gSeperator + "0 graus"
        case 31   : sAngle = "140" + gSeperator + "0graden"
        case 33   : sAngle = "140" + gSeperator + "0 degrés"
        case 34   : sAngle = "140" + gSeperator + "0Grados"
        case 39   : sAngle = "140" + gSeperator + "0gradi"
        case 46   : sAngle = "140" + gSeperator + "0 grader"
        case 49   : sAngle = "140" + gSeperator + "0 Grad"
        case 55   : sAngle = "140" + gSeperator + "0 graus"
        case 81   : sAngle = "140" + gSeperator + "0度"
        case 82   : sAngle = "140" + gSeperator + "0 도"
        case 86   : sAngle = "140" + gSeperator + "0度"
        case 88   : sAngle = "140" + gSeperator + "0度"
        case else : QAErrorLog "Now, the test does not support for the language " +iSprache
                 Goto endsub
    end select
    
    sDistanceX = "1" + gSeperator + "00" + gMeasurementUnit
    sDistanceY = "1" + gSeperator + "50" + gMeasurementUnit
    SPercent   = "150%"
    iVerticalShadowColor = 3
    iSlantedShadowColor  = 4
    
    printLog Chr(13) + "- Test shadow - include vertical and slanted"
    '/// Test shadow - include vertical and slanted
    
    Call hNewDocument
    
    Call wZeichenobjektEinfuegen("TEXT", 30, 40, 60, 60)
    Call wTypeKeys "Test"
    Kontext "DocumentWriter"
    Call wTypeKeys "<ESCAPE>"
    
    '/// format/fontwork
    FormatFontWork
    Kontext "Fontwork"
    Drehen.Click
    Sleep 2
    kreisAuswahl.TypeKeys "<Right>"
    Sleep 2
    Zentriert.Click                        ' center
    Sleep 2
    
    'vertical shadow
    Senkrecht.Click
    Sleep 2
    AbstandX.SetText sDistanceX
    Sleep 2
    AbstandY.SetText sDistanceY
    Sleep 2
    Schattenfarbe.Select iVerticalShadowColor
    Sleep 2
    FontWork.Close
    Sleep 2
    
    FormatFontWork
    Kontext "Fontwork"
    Sleep 2
    if AbstandX.GetText            <> sDistanceX           then Warnlog "Distance X should be "+sDistanceX+"  but -> "+AbstandX.GetText
    Sleep 2
    if AbstandY.GetText            <> sDistanceY           then Warnlog "Distance Y should be "+sDistanceY+"  but -> "+AbstandY.GetText
    Sleep 2
    if Schattenfarbe.GetSelIndex   <> iVerticalShadowColor then Warnlog "Vertical shadow color is wrong."
    Sleep 2
    'Slanted shadow
    Kippen.Click
    Sleep 2
    
    printLog "-" + AbstandX.GetText
    AbstandX.SetText sAngle
    Sleep 2
    AbstandY.SetText SPercent
    Sleep 2
    Schattenfarbe.Select iSlantedShadowColor
    Sleep 2
    Fontwork.Close
    Sleep 2
    
    FormatFontWork
    Kontext "Fontwork"
    Sleep 2
    if AbstandX.GetText <> sAngle then Warnlog "Distance X should be "+sAngle+"  but -> "+AbstandX.GetText
    Sleep 2
    if Left$(AbstandY.GetText,3) <> Left$(SPercent,3) then Warnlog "Distance Y should be "+SPercent+"  but -> "+AbstandY.GetText
    Sleep 2
    if Schattenfarbe.GetSelIndex <> iSlantedShadowColor then Warnlog "Slanted shadow color is wrong."
    Sleep 2
    Fontwork.Close
    
    Call hCloseDocument

endcase

'-------------------------------------------------------------------------
