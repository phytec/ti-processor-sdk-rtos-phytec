Module Module1
    Sub Main(ByVal CmdArgs() As String)
        Dim filename As String
        If CmdArgs.Length = 0 Then
            Console.WriteLine("Usage: bootimage filename.xml")
            Return
        Else
            filename = CmdArgs(0)
        End If

        Dim xDoc As MSXML.DOMDocument
        xDoc = New MSXML.DOMDocument
        xDoc.async = False
        ' load the xml file. It must have been previously created using ofd -x filename.
        If xDoc.load(filename) Then
            ' The document loaded successfully.
            Console.WriteLine("The document loaded")
            ProcessSections(xDoc, filename)
        Else
            ' The document failed to load.
            Console.WriteLine("The document failed to load")
        End If
        xDoc = Nothing
    End Sub

    Public Sub ProcessSections(ByRef xmlDoc As MSXML.DOMDocument, ByVal filename As String)
        Dim sectNodeList As MSXML.IXMLDOMNodeList
        Dim sectchildNodeList As MSXML.IXMLDOMNodeList
        Dim i As Integer
        Dim sectionName As String
        Dim sectionSize As String
        Dim sectionAddr As String
        Dim sectionFPtr As String
        ' List of all "section" nodes under "ti_coff" context
        sectNodeList = xmlDoc.selectNodes("ofd/object_file/ti_coff/section")
        Console.WriteLine("Total Number of Sections:  " & sectNodeList.length)
        ' Open output files for writing
        ' Open input COFF file (.out) for reading
        FileOpen(1, String.Concat(filename, ".vb.c"), OpenMode.Output, OpenAccess.Write)
        FileOpen(2, String.Concat(filename, ".vb.h"), OpenMode.Output, OpenAccess.Write)
        FileOpen(3, filename.Replace(".xml", ".out"), OpenMode.Binary, OpenAccess.Read)
        For i = 0 To (sectNodeList.length - 1)
            Console.WriteLine(sectNodeList.item(i).firstChild.text)
            ' Get section's name, length and address
            sectionName = sectNodeList.item(i).selectSingleNode("name").text
            sectionSize = sectNodeList.item(i).selectSingleNode("raw_data_size").text
            sectionAddr = sectNodeList.item(i).selectSingleNode("physical_addr").text
            ' Find pointer to section's raw data inside the COFF file
            sectionFPtr = sectNodeList.item(i).selectSingleNode("file_offsets/raw_data_ptr").text
            'Console.WriteLine(sectionName & ControlChars.Tab & sectionSize & ControlChars.Tab & sectionAddr)
            ' Determine if the Section is initialized:
            ' If section is non-zero length and there is data in the file ...
            If sectionSize <> "0x0" And sectionFPtr <> "0x0" Then
                ' look for tags which indicate that a section should not be copied:
                ' bss - means a section is not initialized
                ' copy - means that a section is not to be loaded on a target
                '        (examples: .cinit when linked under -cr, dwarf debug sections)
                ' dummy- legacy, typically not used
                ' noload - comes up in the context of complicated partial linking
                sectchildNodeList = sectNodeList.item(i).selectNodes("bss | copy | noload | dummy")
                ' if no such tags are defined, the list will be empty,
                ' meaning that the section should be copied
                If sectchildNodeList.length = 0 Then
                    ' Only initialized sections will pass through here.
                    Console.WriteLine("Section " & sectionName & " will be copied")
                    ' Val("&HFFFF")
                    ' Copy it's raw data from the COFF file into the .c file
                    ' and copy section's name and size into the .h file
                    Dim bytes(CInt(sectionSize.Replace("0x", "&H"))) As Byte
                    Dim sectionFPtrdec As Integer
                    sectionFPtrdec = CInt(sectionFPtr.Replace("0x", "&H"))
                    'Console.WriteLine("byte position " & sectionFPtrdec & " or " & sectionFPtr)
                    ' get the section from the .out file ... 
                    FileGet(3, bytes, sectionFPtrdec)
                    'Console.WriteLine(Hex(bytes(1)) & Hex(bytes(2)) & Hex(bytes(3)) & Hex(bytes(4)))
                    ' ... now write it to a C file
                    PrintCArray(sectionName, sectionSize, sectionAddr, bytes)
                End If
            End If
        Next
        ' close files and exit
        FileClose(1)
        FileClose(2)
        FileClose(3)
    End Sub

    Public Sub PrintCArray(ByVal sectionName As String, ByVal sectionSize As String, ByVal sectionAddr As String, ByRef bytes() As Byte)
        ' Change any non-alphanumeric characters in section name to "_"
        Dim sectionNameMod As String
        sectionNameMod = OnlyAlphaNumericChars(sectionName)
        'Console.WriteLine(sectionName & " ->" & sectionNameMod)
        ' Output section header to C file
        PrintLine(1, "/**********************************************************")
        PrintLine(1, String.Concat("**  ", sectionNameMod, "[", sectionSize, "]: paddr = ", sectionAddr))
        PrintLine(1, "**********************************************************/")
        PrintLine(1, String.Concat("const unsigned char ", sectionNameMod, "[", sectionSize, "] = {"))
        ' Output declaration to header file
        PrintLine(2, String.Concat("extern const unsigned char ", sectionNameMod, "[", sectionSize, "];"))
        ' Start a new line every 13 bytes (Comes to 80 chars/line)
        Dim i As Integer
        For i = 1 To CInt(sectionSize.Replace("0x", "&H"))
            Print(1, String.Concat("0x", Hex(bytes(i)).PadLeft(2, "0"c), ","))
            If (i Mod 13) = 0 Then
                PrintLine(1)
            End If
        Next i
        PrintLine(1, "}")
        PrintLine(1)
    End Sub

    ' Code from http://www.freevbcode.com/ShowCode.Asp?ID=1662

    Public Function OnlyAlphaNumericChars(ByVal OrigString As _
  String) As String
        '***********************************************************
        'INPUT:  Any String
        'OUTPUT: The Input String with all non-alphanumeric characters 
        '        removed
        'EXAMPLE Debug.Print OnlyAlphaNumericChars("Hello World!")
        'output = "HelloWorld")
        'NOTES:  Not optimized for speed and will run slow on long
        '        strings.  If you plan on using long strings, consider 
        '        using alternative method of appending to output string,
        '        such as the method at
        '        http://www.freevbcode.com/ShowCode.Asp?ID=154
        '***********************************************************
        Dim lLen As Long
        Dim sAns As String
        Dim lCtr As Long
        Dim sChar As String

        OrigString = Trim(OrigString)
        lLen = Len(OrigString)
        For lCtr = 1 To lLen
            sChar = Mid(OrigString, lCtr, 1)
            If IsAlphaNumeric(Mid(OrigString, lCtr, 1)) Then
                sAns = sAns & sChar
            Else
                'replace with '_'
                sAns = sAns & "_"
            End If
        Next
        OnlyAlphaNumericChars = sAns
    End Function

    Private Function IsAlphaNumeric(ByVal sChr As String) As Boolean
        IsAlphaNumeric = sChr Like "[0-9A-Za-z]"
    End Function
End Module
