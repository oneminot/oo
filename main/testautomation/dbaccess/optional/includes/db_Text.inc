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
'* short description : test for a text database
'*
'\***********************************************************************

sub db_Text(sFilename as string)

    call tCountCharacterSet(sFilename)
    call tOpenUTF8File(sFilename)

end sub
'-------------------------------------------------------------------------
testcase tCountCharacterSet(sFilename as string)

	'/// open the text database create in the bas file    
    call fOpendatabase(sFileName)

	'/// open the properties dialog of the open database
    printlog "open the properties dialog of the open database"            
    Kontext "DATABASE"    
        Database.UseMenu    
        hMenuSelectNr(2)
        hMenuSelectNr(15)
        hMenuSelectNr(1)

    '/// switch to the Text Tabpage    
    printlog "switch to the Text Tabpage"    
    Kontext "DatabaseProperties"        
        TabControl.setPage TabTextBase
    
    '/// check if there are 54 character sets
    printlog "check if there are 54 character sets"   
    Kontext "TabTextBase"
        dim i as integer
        i = CharSet.getItemCount()
        if i <> 54 then
            warnlog "The count of the charsets is wrong. It have to be 54 but it is:" + i
        else
            printlog "The count of the charsets is correct.There are 54 character sets."
        endif       
    '/// close the properties dialog with OK
    printlog "close the properties dialog with OK"    
    Kontext "DatabaseProperties"
        DatabaseProperties.OK
    
    '/// close the database
    printlog "close the database"    
    call fCloseDatabase()

endcase
'-------------------------------------------------------------------------
testcase tOpenUTF8File(sFilename as string)
    
    '/// copy the file TesttoolPath + dbaccess/optional/input/text_datasource/text_database.txt" to your local wotk directory
    printlog "copy the file TesttoolPath + dbaccess/optional/input/text_datasource/text_database.txt to your local wotk directory"
    app.FileCopy gTesttoolPath + ConvertPath("dbaccess/optional/input/text_datasource/text_database.txt"),gOfficePath + ConvertPath("user/work/text_database.txt")

    '/// open the text database create in the bas file
    call fOpendatabase(sFileName)
    
    '/// open the properties dialog of the open database
    printlog "open the properties dialog of the open database"
    Kontext "DATABASE"    
        Database.UseMenu    
        hMenuSelectNr(2)
        hMenuSelectNr(15)
        hMenuSelectNr(1)

    '/// switch to the Text Tabpage
    printlog "switch to the Text Tabpage"
    Kontext "DatabaseProperties"        
        TabControl.setPage TabTextBase
   
    '/// set UTF-8 as character set
    printlog "set UTF-8 as character set"    
    Kontext "TabTextBase"
        dim iCount as integer
        dim i as integer
        iCount = CharSet.getItemCount()
        for i = 1 to iCount 
            CharSet.select(i)            
            if (Instr(CharSet.getSelText(),"(UTF-8)") <> 0 ) then
                i = iCount
            endif
        next

    '/// close the properties dialog with OK
    printlog "close the properties dialog with OK"    
    Kontext "DatabaseProperties"
        DatabaseProperties.OK
    
    '/// save and close the database
    call fCloseDatabase(true)
    
    '/// open the text database again
    call fOpendatabase(sFileName)    
        
    '/// open the table text_database
    printlog "open the table text_Database"    
    call fOpenTable("text_database")
    
    '/// check if the first row contains the string öäü
    printlog "check if the first row contains the string öäü"
    kontext "TableView"
        TableView.TypeKeys("<MOD1 C>",true)
        if ( getClipboard = "1;öäü") then
        	printlog "The table is load with an UTF-8 locale -> OK"
        else
        	warnlog "The table is not load ed with an UTF-8 locale -> FAILED"
		endif
    
    '/// close the table
    printlog "close the table"    
    call fCloseTableView
    
    '/// close the database
    printlog "close the database"
    call fCloseDatabase()

endcase
