/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile$
 *
 *  $Revision$
 *
 *  last change: $Author$ $Date$
 *
 *  The Contents of this file are made available subject to
 *  the terms of GNU Lesser General Public License Version 2.1.
 *
 *
 *    GNU Lesser General Public License Version 2.1
 *    =============================================
 *    Copyright 2005 by Sun Microsystems, Inc.
 *    901 San Antonio Road, Palo Alto, CA 94303, USA
 *
 *    This library is free software; you can redistribute it and/or
 *    modify it under the terms of the GNU Lesser General Public
 *    License version 2.1, as published by the Free Software Foundation.
 *
 *    This library is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *    Lesser General Public License for more details.
 *
 *    You should have received a copy of the GNU Lesser General Public
 *    License along with this library; if not, write to the Free Software
 *    Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 *    MA  02111-1307  USA
 *
 ************************************************************************/
#include "major.hxx"

#ifndef _RTL_USTRING_HXX_
#include <rtl/ustring.hxx>
#endif//_RTL_USTRING_HXX_

#ifndef _RTL_STRBUF_HXX_
#include <rtl/strbuf.hxx>
#endif//_RTL_STRBUF_HXX_

#ifndef _OSL_FILE_HXX_
#include <osl/file.hxx>
#endif//_OSL_FILE_HXX_

#ifndef _OSL_TIME_H_
#include <osl/time.h>
#endif//_OSL_TIME_H_

#ifndef _OSL_THREAD_H_
#include <osl/thread.h>
#endif//_OSL_THREAD_H_

#include <osl/diagnose.h>

#include <osl/time.h>

#include <algorithm>

namespace configmgr 
{
    namespace dbbe 
    {
        void printUString(const char *name, const rtl::OUString &aString)
        {
            using namespace std;
            using namespace rtl;
            
            OSL_ASSERT(name);
            cerr << name << "=\"" ;
            cerr << OUStringToOString(aString, 
                                      osl_getThreadTextEncoding()).getStr();
            cerr << "\"" << endl;
        }

        mangler::mangler(const char *Code):
            sep("::"),
            xcu(".xcu")
        {
            OSL_ASSERT(Code);
            code= Code;
        }

        mangler::~mangler()
        {
        }

        
        void mangler::demangle(const rtl::OUString &aUrl, const rtl::OUString &aBaseURL)
        {
            OSL_VERIFY(aUrl.getLength());
            OSL_VERIFY(aBaseURL.getLength());
            fileUrl= aUrl;
            OSL_VERIFY(aUrl.match(aBaseURL, 0));
            rtl::OUString aPath= aUrl.copy(aBaseURL.getLength() + 1, 
                                           aUrl.getLength() - aBaseURL.getLength() - 1);
            rtl::OString path= rtl::OUStringToOString(aPath,  osl_getThreadTextEncoding());

            sal_Int32 p= 0;
            rtl::OString nameSpace, layer, sublayer;
            
            for (int c= 0; c < strlen(code); c++)
            {
                switch (code[c])
                {
                    case 'n':
                        nameSpace= path.getToken(0, '/', p);
                        break;
                        
                    case 'l':
                        if (c == strlen(code) - 1)
                        {//   just eat everything
                            sal_Int32 dot= path.lastIndexOf('.');
                            OSL_VERIFY(dot > 0);
                            layer= path.copy(p, dot - p).replace('/', '.');
                        }
                        else
                        {//  keep eating up to the last slash
                            sal_Int32 last_slash= path.lastIndexOf('/');
                            OSL_VERIFY(last_slash > p);
                            layer= path.copy(p, last_slash - p).replace('/', '.');
                            p= last_slash + 1;
                        }
                        break;
                        
                    case 's':
                        if (c == strlen(code) - 1)
                        {//this is the last code -- eat .xcu
                            sublayer= path.copy(p, 
                                                path.getLength() - 
                                                xcu.getLength() - p);
                        }
                        else
                        {//this is not the last code -- go to /
                            sublayer= path.getToken(0, '/', p);
                        }
                        break;

                    default:
                        std::cerr << "Encountered illegal conversion char: " << code[c] << std::endl;
                        abort();
                        break;
                }
            }

            rtl::OStringBuffer keyBuf;
            keyBuf.append(nameSpace);
            keyBuf.append(sep);
            keyBuf.append(layer);
            keyBuf.append(sep);
            keyBuf.append(sublayer);
            key= keyBuf.makeStringAndClear();
            std::cerr << "key= \"" << key.getStr() << "\"\n";
            
        }

        const char* mangler::getKey(void) const
        {
            return key.getStr();
        }

        rtl::OString mangler::getNamespace(void) const
        {
            sal_uInt32 sepIndex= key.indexOf(sep);
            OSL_VERIFY(sepIndex > 1);
            return key.copy(0, sepIndex - 2);
        }
        
        rtl::OString mangler::getNamespaceKey(void) const
        {
            rtl::OString ret(getNamespace());
            ret+= sep;
            ret+= sep;
            return ret;
        }

        const rtl::OUString& mangler::getFileUrl(void) const
        {
            return fileUrl;
        }

        repository::repository(Db &aDb, char* aDbPath):
            aDatabasePath(aDbPath),
            aDatabase(aDb)
        {
        }

        void repository::importFile(const rtl::OUString &aURL, const rtl::OUString &aBaseURL, 
                                    const char *code)
        {
            char *guessedCode= NULL;
            if (!code)
            {
                rtl::OUString aRelURL= aURL.copy(aBaseURL.getLength() + 1, //the slash
                                                 aURL.getLength() - aBaseURL.getLength() - 1);
                guessedCode= const_cast<char*>(guessCode(aRelURL));
            }
            else
            {
                guessedCode= const_cast<char*>(code);
            }
            
            OSL_VERIFY(guessedCode);
            mangler aMangler(guessedCode);            
            aMangler.demangle(aURL, aBaseURL);

            //make a record from this
            {
                Record aRecord;
                aRecord.setBlobFromFile(aURL);
                OSL_VERIFY(aRecord.putRecord(aDatabase, aMangler.getKey()));
            }
            
            //look for parents
            {
                Dbt Data;
                Dbt Key((void*)aMangler.getNamespaceKey().getStr(),
                        aMangler.getNamespaceKey().getLength());
                int ret= aDatabase.get(NULL, &Key, &Data, 0);
                
                int swap= 0;
                switch (ret)
                {
                    
                    case DB_NOTFOUND:
                    {
                        Record aRecord;
                        aRecord.touch();
                        std::vector<sal_Char*> sublayers;
                        sublayers.push_back(const_cast<sal_Char*>(aMangler.getKey()));
                        aRecord.setSubLayers(sublayers);
                        aRecord.putRecord(aDatabase, aMangler.getNamespaceKey());
                    }
                    break;
                    
                    case 0:
                    { //found
                        //FIXME: don't use marshal, etc
                        Record* pRecord= Record::getFromDbt(Data);
                        OSL_VERIFY(pRecord);
                        pRecord->unMarshal();
                        OSL_VERIFY(!aDatabase.get_byteswapped(&swap));
                        if (swap)
                            pRecord->bytesex(); 
                        std::vector<sal_Char*> sublayers= pRecord->listSubLayers();
                        std::vector<sal_Char*>::iterator it;
                        for (it= sublayers.begin();
                             it != sublayers.end();
                             it++)
                        {
                            if (!strcmp(*it, aMangler.getKey()))
                            {
                                break;
                            }
                        }                        
                        if (it == sublayers.end())
                        { 
                            sublayers.push_back(const_cast<sal_Char*>(aMangler.getKey()));
                            pRecord->setSubLayers(sublayers);
                            OSL_VERIFY(pRecord->putRecord(aDatabase, aMangler.getNamespaceKey()));
                        }
                    }
                    break;
                        
                    default:
                        //FIXME: error messages?
                        std::cerr << "Error when trying to import file \"";
                        std::cerr << rtl::OUStringToOString(aURL, osl_getThreadTextEncoding());
                        std::cerr << "\"!\n";
                        abort();
                        break;
                }
                OSL_VERIFY(!aDatabase.sync(0));
            }
        }

        void repository::mergeDatabase(const char *path)
        {
            //steps to merge a database
            //
            // find all the keys in new database,
            // for_each(key)
            //   if (key is child)
            //      put in database
            //      if (parent node is in database)
            //        update parent to point to child
            //      else
            //        bring in parent from new database
            std::cerr << "Impliment me!\n";
            OSL_VERIFY(0);
        }

        
        const char* repository::guessCode(const rtl::OUString& aRelUrl) const
        {
            
            using namespace rtl;
            
            static const char* res=      "nsl";
            static const char* data=     "nl";
            static const char* modules=  "nls";

            const rtl::OUString resString(RTL_CONSTASCII_USTRINGPARAM("res"));
            const rtl::OUString dataString(RTL_CONSTASCII_USTRINGPARAM("data"));
            const rtl::OUString modulesString(RTL_CONSTASCII_USTRINGPARAM("modules"));
            
//            printUString("resString", resString);
//            printUString("dataString", dataString);
//            printUString("modulesString", modulesString);

            OSL_VERIFY(aRelUrl.getLength());
            sal_Int32 slash= aRelUrl.indexOf('/');
            OSL_VERIFY(slash > 0);
            OUString aSubUrl= aRelUrl.copy(0, slash);
            OSL_VERIFY(aSubUrl.getLength());
            printUString("aSubUrl" ,aSubUrl);
            

            if (aSubUrl.match(resString))
            {
//                std::cerr << "res\n";
                return res;
            }
            if (aSubUrl.match(dataString))
            {
//                std::cerr << "data\n";
                return data;
            }
            if (aSubUrl.match(modulesString))
            {
//                std::cerr << "modules\n"                ;
                return modules;
            }
//            std::cerr << "nothing!\n";
            return NULL;
        }

        
        void repository::importPathInnerLoop(const rtl::OUString &aUrl, 
                                             const rtl::OUString &aBaseUrl, 
                                             const char *code)
        {
            using namespace osl;

            //DEBUG
            
            //static int recursecount= 0;
            //if (recursecount > 10)
            //    abort();
            //else
            //    recursecount++;
            

            printUString("aUrl", aUrl);
            printUString("aBaseUrl", aBaseUrl);
            //END DEBUG
            
            OSL_ASSERT(aUrl.getLength());
            OSL_ASSERT(aBaseUrl.getLength());
            
            Directory aDir(aUrl);
            OSL_VERIFY(aDir.open() == FileBase::E_None);
            DirectoryItem aDirItem;
            FileStatus fs(FileStatusMask_Type | FileStatusMask_FileURL);
            while (aDir.getNextItem(aDirItem) == FileBase::E_None)
            {
                if (aDirItem.getFileStatus(fs) == FileBase::E_None)
                {
                    switch (fs.getFileType())
                    {
                        case FileStatus::Directory:
                        {
                            std::cerr << "Would recurse into ";
                            printUString("", fs.getFileURL());
                            importPathInnerLoop(fs.getFileURL(), aBaseUrl, code);
                        }
                        break;
                        
                        case FileStatus::Regular:
                        {
                            std::cerr << "Would import file ";
                            importFile(fs.getFileURL(), aBaseUrl, code);
                            printUString("", fs.getFileURL());
                        }
                        break;
                        
                        default:
                            abort();
                            break;
                    }
                }
                else
                {
                    abort();
                }
            }
            
#if 0

            int recurseCount= 0;

            Directory::RC ret= Directory::E_None;
            while (ret == Directory::E_None)
            {
                ret = aDir.getNextItem(aDirItem); 
                FileStatus aFileStatus(FileStatusMask_Type || FileStatusMask_FileURL 
                                       || FileStatusMask_FileName);
                OSL_VERIFY(DirectoryItem::E_None == aDirItem.getFileStatus(aFileStatus));
                switch(aFileStatus.getFileType())
                {
                    case FileStatus::Directory:
                    {
                        rtl::OUString anItemURL= aFileStatus.getFileURL();
                        if (anItemURL.getLength() == 0)
                        {
                            OSL_ASSERT(aFileStatus.getFileName().getLength());
                            anItemURL= aUrl + aFileStatus.getFileName();
                        }
                        printUString("anItemURL", anItemURL);
                        if (anItemURL.getLength() == 0)
                            break;
                        OSL_ASSERT(anItemURL.getLength());
                        importPathInnerLoop(anItemURL,
                                            aBaseUrl,
                                            code);
                    }
                        break;
                        
                    case FileStatus::Regular:
                        std::cerr << "Regular File\n";
                        importFile(aFileStatus.getFileURL(), aBaseUrl, code);
                        break;
                        
                    default:
                        std::cerr << "none of the above\n";
                        OSL_VERIFY(0); //should never happen
                }
            }
            OSL_VERIFY(ret == Directory::E_NOENT);
#endif
            aDir.close();
        }


        void repository::importPath(const char *path, const char *code)
        {
            using namespace osl;
            using namespace rtl;

            const OUString sep(RTL_CONSTASCII_USTRINGPARAM("/"));
            OUString DbURL;

            const OUString aDbPath(aDatabasePath, strlen(aDatabasePath), osl_getThreadTextEncoding());
            
            
            OSL_VERIFY(FileBase::getFileURLFromSystemPath(OUString(aDatabasePath, 
                                                                   strlen(aDatabasePath), 
                                                                   osl_getThreadTextEncoding()),
                                                          DbURL) == FileBase::E_None);
            
            sal_uInt32 slash= DbURL.lastIndexOf(sep);
            OSL_VERIFY(slash > 0);

            OUString baseURL= DbURL.copy(0, slash);
            OUString aURL;
            OSL_VERIFY(FileBase::getFileURLFromSystemPath(
                           OUString(path, strlen(path), osl_getThreadTextEncoding()),
                           aURL) == FileBase::E_None);
            OSL_ASSERT(aURL.getLength());

            std::cerr << "from importPath:\n";
            printUString("aURL", aURL);
            printUString("baseURL", baseURL);
            std::cerr << "\n";
                        
            DirectoryItem aDirItem;
            OSL_VERIFY(DirectoryItem::E_None == DirectoryItem::get(aURL, aDirItem));
            FileStatus aFileStatus(FileStatusMask_Type);
            OSL_VERIFY(DirectoryItem::E_None == aDirItem.getFileStatus(aFileStatus));
            switch(aFileStatus.getFileType())
                {
                    case FileStatus::Directory:
                        importPathInnerLoop(aURL, baseURL, code);
                        break;
                        
                    case FileStatus::Regular:
                        importFile(aURL, baseURL, code);
                        break;
                        
                    default:
                        abort();
                }
        }
    };
};
