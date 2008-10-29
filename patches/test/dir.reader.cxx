// Cutn'pasting this into msvbasicdir ( from ooobuild/patches/test/vba-export-dir.diff should work
// the reader class DirDumper ( at the end of this file ) is not complete, still some records to go :-(

// also _VBA_PROJECT_VDPI can be used to create a usable 
// ( and much smaller ) "_VBA_PROJECT" stream

// _VBA_PROJECT Stream Version Dependant Project Information
// _VBA_PROJECT Stream Version Dependant Project Information
class _VBA_PROJECT_VDPI
{
public:
sal_Int16 Reserved1;
sal_Int16 Version;
sal_Int8 Reserved2;
sal_Int16 Reserved3;
sal_Int8* PerformanceCache;
sal_Int32 PerformanceCacheSize;
_VBA_PROJECT_VDPI(): Reserved1( 0x61CC), Version( 0xFFFF ), Reserved2(0x0), Reserved3(0x0), PerformanceCache(0),  PerformanceCacheSize(0) {}
~_VBA_PROJECT_VDPI()
{
    if (  PerformanceCache )
       delete [] PerformanceCache;
    PerformanceCache = 0;
    PerformanceCacheSize = 0;
}
void read(){}
void write( SvStream* pStream )
{
    *pStream << Reserved1 << Version << Reserved2 << Reserved3;
    for( sal_Int32 i = 0; PerformanceCache && i < PerformanceCacheSize; ++i )
    {
        *pStream >> PerformanceCache[ i ];
    }
}
};

class ProjectSysKindRecord
{
public:
sal_Int16 Id;
sal_Int32 Size;
sal_Int32 SysKind;
ProjectSysKindRecord(): Id(0x1), Size(0x4), SysKind( 0x1 ) {}
void read( SvStream* pStream )
{
    *pStream >> Id >> Size >> SysKind;
}
};

class ProjectLcidRecord
{
public:
sal_Int16 Id;
sal_Int32 Size;
sal_Int32 Lcid;

ProjectLcidRecord() : Id( 0x2 ), Size( 0x4 ), Lcid( 0x409 ) {}
void read( SvStream* pStream )
{
    *pStream >> Id >> Size >> Lcid;
}
};

class ProjectLcidInvokeRecord
{
sal_Int16 Id;
sal_Int32 Size;
sal_Int32 LcidInvoke;
public:
ProjectLcidInvokeRecord() : Id( 0x14 ), Size( 0x4 ), LcidInvoke( 0x409 ) {}
void read( SvStream* pStream )
{
    *pStream >> Id >> Size >> LcidInvoke;
}
};

class ProjectCodePageRecord
{
sal_Int16 Id;
sal_Int32 Size;
sal_Int16 CodePage;
public:
// #FIXME get a better default for the CodePage
ProjectCodePageRecord() : Id( 0x03 ), Size( 0x2 ), CodePage( 0x0 ) {}
void read( SvStream* pStream )
{
    *pStream >> Id >> Size >> CodePage;
}
};
class ProjectNameRecord
{
public:
sal_Int16 Id;
sal_Int32 SizeOfProjectName;
sal_Int8* ProjectName;
ProjectNameRecord() : Id( 0x04 ), SizeOfProjectName( 0x0 ), ProjectName(0) {}
~ProjectNameRecord()
{
    delete [] ProjectName;
    ProjectName = 0;
}
void read( SvStream* pStream )
{
    *pStream >> Id >> SizeOfProjectName;
    if ( ProjectName )
        delete [] ProjectName;

    ProjectName = new sal_Int8[ SizeOfProjectName ];
    for( sal_Int32 nIndex = 0; nIndex < SizeOfProjectName; ++nIndex )
    {
        *pStream >> ProjectName[ nIndex ];
    }
}
};

class ProjectDocStringRecord
{
public:
sal_Int16 Id;
sal_Int32 SizeOfDocString;
sal_Int8* DocString;
sal_Int16 Reserved;
sal_Int32 SizeOfDocStringUnicode;
sal_Int8* DocStringUnicode;

ProjectDocStringRecord() : Id( 0x5 ), SizeOfDocString( 0x0 ), DocString( 0 ), Reserved( 0x0 ), SizeOfDocStringUnicode( 0 ), DocStringUnicode( 0 ) {}

~ProjectDocStringRecord()
{
    delete [] DocString;
    delete [] DocStringUnicode;
    DocString = 0;
    DocStringUnicode = 0;
}
void read( SvStream* pStream )
{
    *pStream >> Id >> SizeOfDocString;

    if ( DocString )
        delete [] DocString;

    pStream->Read( DocString, SizeOfDocString );

    if ( SizeOfDocStringUnicode )
        delete [] DocStringUnicode;

    *pStream >> Reserved >> SizeOfDocStringUnicode;

    if ( DocStringUnicode )
        delete [] DocStringUnicode;

    DocStringUnicode =  new sal_Int8[ SizeOfDocStringUnicode ];

    pStream->Read( DocStringUnicode, SizeOfDocStringUnicode );
}

};

class ProjectHelpFilePath
{
public:
sal_Int16 Id;
sal_Int32 SizeOfHelpFile1;
sal_Int8* HelpFile1;
sal_Int16 Reserved;
sal_Int32 SizeOfHelpFile2;
sal_Int8* HelpFile2;

ProjectHelpFilePath() : Id( 0x06 ), SizeOfHelpFile1(0), HelpFile1(0), Reserved(0x0), SizeOfHelpFile2(0), HelpFile2(0) {}
~ProjectHelpFilePath()
{
    if ( HelpFile1 )
        delete [] HelpFile1;
    if ( HelpFile2 )
        delete [] HelpFile2;
    HelpFile1 = 0;
    HelpFile2 = 0;
}
void read( SvStream* pStream )
{
    *pStream >> Id >> SizeOfHelpFile1;

    if ( HelpFile1 )
        delete HelpFile1;

    HelpFile1 = new sal_Int8[ SizeOfHelpFile1 ];
    pStream->Read( HelpFile1, SizeOfHelpFile1 );

    *pStream >> Reserved >> SizeOfHelpFile2;

    if ( HelpFile2 )
        delete HelpFile2;

    HelpFile2 = new sal_Int8[ SizeOfHelpFile2 ];
    pStream->Read( HelpFile2, SizeOfHelpFile2 );
  
}
};

class ProjectHelpContextRecord
{
public:
sal_Int16 Id;
sal_Int32 Size;    
sal_Int32 HelpContext;    

ProjectHelpContextRecord() : Id( 0x7 ), Size( 0x4 ), HelpContext( 0 ) {}
void read( SvStream* pStream )
{
   *pStream >> Id >> Size >> HelpContext;
}

};

class ProjectLibFlagsRecord
{
sal_Int16 Id;
sal_Int32 Size;
sal_Int32 ProjectLibFlags;

public:
ProjectLibFlagsRecord() : Id( 0x8 ), Size( 0x4 ), ProjectLibFlags( 0x0 ) {}
void read( SvStream* pStream ) 
{
    *pStream >> Id >> Size >> ProjectLibFlags;
}
};

class ProjectVersionRecord
{
public:
sal_Int16 Id;
sal_Int32 Reserved;
sal_Int32 VersionMajor;
sal_Int16 VersionMinor;
ProjectVersionRecord() : Id( 0x9 ), Reserved( 0x4 ), VersionMajor( 0x1 ), VersionMinor( 0 ) {}
void read( SvStream* pStream )
{
    *pStream >> Id >> Reserved >> VersionMajor >> VersionMinor;
}
};

class ProjectConstantsRecord
{
sal_Int16 Id;
sal_Int32 SizeOfConstants;
sal_Int8* Constants;
sal_Int16 Reserved;
sal_Int32 SizeOfConstantsUnicode;
sal_Int8* ConstantsUnicode;
public:
ProjectConstantsRecord() : Id( 0xC ), SizeOfConstants( 0 ), Constants( 0 ), Reserved( 0x3C ), SizeOfConstantsUnicode( 0 ), ConstantsUnicode(0) {}

~ProjectConstantsRecord()
{
    delete [] Constants;
    Constants = 0;
    delete [] ConstantsUnicode;
    ConstantsUnicode = 0;
}

void read( SvStream* pStream )
{
   *pStream >> Id >> SizeOfConstants;
    if ( Constants )
        delete [] Constants;
    Constants = new sal_Int8[ SizeOfConstants ];
    pStream->Read( Constants, SizeOfConstants );

    if ( ConstantsUnicode )
        delete [] ConstantsUnicode;
    ConstantsUnicode = new sal_Int8[ SizeOfConstantsUnicode ];
    pStream->Read( ConstantsUnicode, SizeOfConstantsUnicode );
}

};

class ReferenceNameRecord
{
public:
sal_Int16 Id;
sal_Int32 SizeOfName;
sal_Int8* Name;
sal_Int16 Reserved;
sal_Int32 SizeOfNameUnicode;
sal_Int8* NameUnicode;

ReferenceNameRecord() : Id( 0x16 ), SizeOfName( 0 ), Name( 0 ), Reserved( 0x3E ), SizeOfNameUnicode( 0 ), NameUnicode( 0 ) {}
~ReferenceNameRecord()
{
    delete [] Name;
    Name = 0;
    delete [] NameUnicode;
    NameUnicode = 0;
}

void read( SvStream* pStream )
{
    *pStream >> Id >> SizeOfName;
 
    if ( Name )
        delete [] Name;

    pStream->Read( Name, SizeOfName );

    *pStream >> Reserved >> SizeOfNameUnicode;

    if ( NameUnicode )
        delete [] Name;

    pStream->Read( NameUnicode, SizeOfNameUnicode );
}

};

// Baseclass for ReferenceControlRecord, ReferenceRegisteredRecord, ReferenceProjectRecord
class BaseReferenceRecord 
{
public:
virtual ~BaseReferenceRecord(){}
virtual void read( SvStream* pStream ) = 0;
};

class ReferenceControlRecord : public BaseReferenceRecord
{
public:
sal_Int16 Id;
sal_Int32 SizeTwiddled;
sal_Int32 SizeOfLibidTwiddled;
sal_Int8* LibidTwiddled;
sal_Int32 Reserved1;
sal_Int16 Reserved2;
ReferenceNameRecord* NameRecordExtended;// Optional
sal_Int16 Reserved3;
sal_Int32 SizeExtended;
sal_Int32 SizeOfLibidExtended;
sal_Int8* LibidExtended;
sal_Int32 Reserved4;
sal_Int16 Reserved5;
sal_Int8  OriginalTypeLib[ 16 ];
sal_Int32 Cookie;

ReferenceControlRecord() : Id( 0x2F ), SizeTwiddled( 0 ), SizeOfLibidTwiddled( 0 ), LibidTwiddled( 0 ), Reserved1( 0 ), Reserved2( 0 ), NameRecordExtended( 0 ), Reserved3( 0x30 ), SizeExtended( 0 ), SizeOfLibidExtended( 0 ), LibidExtended( 0 ), Reserved4( 0 ), Reserved5( 0 ), Cookie( 0 )
{
    for( int i = 0; i < 16; ++i )
        OriginalTypeLib[ i ] = 0;
}

~ReferenceControlRecord()
{
    delete LibidTwiddled;
    delete NameRecordExtended;
    delete [] LibidExtended;
    LibidTwiddled = 0;
    NameRecordExtended = 0;
    LibidExtended = 0;
}

void read( SvStream* pStream )
{
    *pStream >> Id >> SizeTwiddled >> SizeOfLibidTwiddled;
    pStream->Read( LibidTwiddled,  SizeOfLibidTwiddled );

    *pStream >> Reserved1 >> Reserved2;

    long nPos = pStream->Tell();
    // peek at the id for optional NameRecord
    sal_Int16 nTmpId;
    *pStream >> nTmpId;
    if ( nTmpId == 0x30 )
    {
        Reserved3 = 0x30;
    }
    else
    {
        pStream->Seek( nPos );
        NameRecordExtended = new ReferenceNameRecord();
        NameRecordExtended->read( pStream );
    }
}

};

class ReferenceRecord
{
public:
// NameRecord is Optional
ReferenceNameRecord* NameRecord;
BaseReferenceRecord*  aReferenceRecord;
ReferenceRecord(): NameRecord(0), aReferenceRecord(0) {}
~ReferenceRecord()
{
    if ( NameRecord )
        delete NameRecord;       
    if ( aReferenceRecord )
        delete aReferenceRecord;       
}

// false return would mean failed to read Record e.g. end of array encountered
// Note: this read routine will make sure the stream is pointing to where it was the 
// method was called ) 

bool read( SvStream* pStream )
{
    bool bResult = true;
    long nPos = pStream->Tell();
    // Peek at the ID 
    sal_Int16 Id;
    *pStream >> Id;
    pStream->Seek( nPos ); // place back before Id
    if ( Id == 0x16 ) // Optional NameRecord
    {
        NameRecord = new ReferenceNameRecord();
        NameRecord->read( pStream ); 
    }

    nPos = pStream->Tell(); // peek at next
    *pStream >> Id;
    pStream->Seek( nPos ); // place back before Id

    switch( Id )
    {
        case 0x0:
            bResult = false;
            break;
/* #FIXME haven't got as far as coding the following records yet :-( oh hackweek
why aren't you longer :-(((
        case 0x0D:
            aReferenceRecord = new ReferenceRegisteredRecord();
            break; 
        case 0x0E:
            aReferenceRecord = new ReferenceProjectRecord();
            break; 
        case 0x2F:
        case 0x33:
            aReferenceRecord = new ReferenceControlRecord();
            break; 
*/
        default:
            bResult = false;
            OSL_TRACE("Big fat error, unknown ID 0x%x", Id);
            break;
        aReferenceRecord->read( pStream );
    } 
    return bResult;
}

};

class DirDumper
{
public:
ProjectSysKindRecord mSysKindRec;
ProjectLcidRecord mLcidRec;
ProjectLcidInvokeRecord mLcidInvokeRec;
ProjectCodePageRecord mCodePageRec;
ProjectNameRecord mProjectNameRec;
ProjectDocStringRecord mDocStringRec;
ProjectHelpFilePath mHelpFileRec;
ProjectHelpContextRecord mHelpContextRec;
ProjectLibFlagsRecord mLibFlagsRec;
ProjectVersionRecord mVersionRec;
std::vector< ReferenceRecord > ReferenceArray;

DirDumper() {}

void read( SvStream* pStream )
{
    readProjectInformation( pStream );
    readProjectReferenceInformation( pStream );
}

void readProjectReferenceInformation( SvStream* pStream )
{
    ReferenceRecord aRef;
    while( aRef.read( pStream ) )
        ReferenceArray.push_back( aRef );
}

void readProjectInformation( SvStream* pStream )
{
    mSysKindRec.read( pStream );
    mLcidRec.read( pStream ); 
    mLcidInvokeRec.read( pStream );
    mCodePageRec.read( pStream );
    mProjectNameRec.read( pStream );
    mDocStringRec.read( pStream );
    mHelpFileRec.read( pStream );
    mHelpContextRec.read( pStream );
    mLibFlagsRec.read( pStream );
    mVersionRec.read( pStream ); 
}

};
