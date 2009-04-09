Attribute VB_Name = "TestLogMacros"
' { Declarations shared with MS Office API tests

Global Const FILE_EXT_DOC = "odt"
Global Const FILE_EXT_XLS = "ods"
Global Const FILE_EXT_PPT = "odp"
Global Const FILE_EXT_VIS = "odg"

Private theCalcApplication As Object
' Global vbExternalCall As Long

Global PARAGRAPH_END As String
Global Const DIR_SEPARATOR As String = "\"

' } Declarations shared with MS Office API tests

' BEGIN: *** { TEST API (PortableHelperAPITest)

Private HELPER_TEST_LOG As String

Private TestLog_FileNo As Integer
Private TestLog_LastTest As String
Private TestLog_LastTestResult As Boolean

Private TESTLOG_VERBOSE As Boolean

Function isOpenoffice() As Boolean
On Error GoTo notopenoffice
Dim a As Variant
a = CreateObject("com.sun.star.beans.PropertyValue")
isOpenoffice = True
Exit Function
notopenoffice:
isOpenoffice = False
End Function
Function TestAreEqual(x1 As Variant, x2 As Variant) As Boolean
    If x1 = x2 Then
        TestAreEqual = True
    Else
        TestAreEqual = False
    End If
End Function

Function TestNotEqual(x1 As Variant, x2 As Variant) As Boolean
    If Not (x1 = x2) Then
        TestNotEqual = True
    Else
        TestNotEqual = False
    End If
End Function

Sub TestLog_SetFileName(filename As String)
    Dim tmp As String
    tmp = HELPER_TEST_LOG
    HELPER_TEST_LOG = filename
    If Not (TestLog_FileNo = 0) Then
        TestLog_Comment "Closing log, future log output going to " + filename
        TestLog_Close
        TestLog_Comment "Reopening log, previous log output went to " + tmp
    End If
End Sub

Sub TestLog_Comment(comment As String)
    If (TestLog_FileNo = 0) Then
        TestLog_FileNo = FreeFile ' Establish free file handle
        If (HELPER_TEST_LOG = "") Then
            HELPER_TEST_LOG = DefaultLog
        End If
        Open HELPER_TEST_LOG For Output As #TestLog_FileNo
    End If
    Print #TestLog_FileNo, comment
End Sub
Function OpenofficeLog() As String
         Dim PathSettings As Object
         Dim WorkingDirectory As String
        PathSettings = createUnoService("com.sun.star.comp.framework.PathSettings")
        WorkingDirectory = PathSettings.Work
        OpenofficeLog = WorkingDirectory + "/" + "HelperAPI-test.log"
        End Function
Function DefaultLog() As String
If isOpenoffice Then
    DefaultLog = OpenofficeLog
Else
    DefaultLog = "c:\HelperApi-vba.log"
End If

End Function

Sub TestLog_BEGIN(testName As String)
    If Not (TestLog_LastTest = "") Then
        Print #TestLog_FileNo, TestLog_LastTest + " NOT COMPLETE"
    End If
    TestLog_LastTest = testName
    TestLog_LastTestResult = True
    
    Dim s As String
    If TESTLOG_VERBOSE Then
        s = " TEST START : " + testName
        TestLog_Comment s
    End If
End Sub

Sub TestLog_END(testResult As String, testName As String, Optional testComment As String)
    If (TestLog_LastTest = "") Then
        TestLog_Comment "TEST ERROR - no test begun: " + testName
    Else
        If Not (TestLog_LastTest = testName) Then
            TestLog_Comment "TEST ERROR - found test end: " + testName + " , expecting test end " + TestLog_LastTest
        Else
            Dim s As String
            s = " TEST " + testResult + " : " + testName
            If TESTLOG_VERBOSE And Not IsMissing(testComment) And Not (testComment = "") Then
                s = s + " (" + testComment + ")."
            End If
            TestLog_Comment s
        End If
    End If
    TestLog_LastTest = ""
End Sub

Sub TestLog_ITEM(testComment As String)
    If (TestLog_LastTest = "") Then
        TestLog_Comment "TEST ERROR - no test begun: " + testComment
    Else
        TestLog_Comment "  ITEM " + testComment
    End If
End Sub

Sub TestLog_FAIL(testName As String, Optional testComment As String)
    If IsMissing(testComment) Then
        TestLog_END "FAIL", testName
    Else
        TestLog_END "FAIL", testName, testComment
    End If
End Sub

Sub TestLog_OK(testName As String, Optional testComment As String)
    If IsMissing(testComment) Then
        TestLog_END "OK", testName
    Else
        TestLog_END "OK", testName, testComment
    End If
End Sub

 Sub TestLog_PartComment(status As String, Optional testComment As String)
     If IsMissing(testComment) Then
         testComment = status
     Else
         testComment = status + " (" + testComment + ")"
     End If
     TestLog_ITEM testComment
 End Sub

 Sub TestLog_PartFAIL(Optional testComment As String)
     TestLog_LastTestResult = False
     TestLog_PartComment "FAIL", testComment
 End Sub

 Sub TestLog_PartOK(Optional testComment As String)
     TestLog_PartComment "OK", testComment
 End Sub

Sub TestLog_ASSERTSetVerbose(verbose As Boolean)
    TESTLOG_VERBOSE = verbose
End Sub

Function TestLog_ASSERTGetVerbose() As Boolean
    TestLog_ASSERTGetVerbose = TESTLOG_VERBOSE
End Function

Sub TestLog_ASSERT(assertion As Boolean, Optional testId As String, Optional testComment As String)
    Dim testMsg As String
    testMsg = "Assertion "
    If assertion = True Then
        testMsg = testMsg + "OK"
    Else
        TestLog_LastTestResult = False
        testMsg = testMsg + "FAIL"
    End If
    If Not IsMissing(testId) Then
        testMsg = testMsg + " : " + testId
    End If
    If TESTLOG_VERBOSE And Not IsMissing(testComment) And Not (testComment = "") Then
        testMsg = testMsg + " (" + testComment + ")"
    End If
    If assertion = False Or TESTLOG_VERBOSE Then
        TestLog_ITEM testMsg
    End If
End Sub

Sub TestLog_SUMMARY(testName As String, Optional testComment As String)
    ' SBA does not preserve "optional" status of a parameter; it is evaluated then passed,
    ' even if the called routine (e.g. TestLog_OK) *also* has considers the parameter to be optional.
    If IsMissing(testComment) Or testComment = "" Then
        If TestLog_LastTestResult = True Then
         TestLog_OK testName
        Else
            TestLog_FAIL testName
        End If
    Else
        If TestLog_LastTestResult = True Then
         TestLog_OK testName, testComment
        Else
            TestLog_FAIL testName, testComment
        End If
    End If
End Sub

Sub TestLog_Close()
    Close #TestLog_FileNo
    TestLog_FileNo = 0
End Sub


' END *** } Test API
Rem  *****  MSO MACRO RUNTIME MODULE END  *****


