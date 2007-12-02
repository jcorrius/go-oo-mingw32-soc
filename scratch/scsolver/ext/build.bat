@echo off

set c=cl
set oooinst=C:\Program Files\OpenOffice.org 2.3
set ooosdk=C:\Program Files\OpenOffice.org_2.3_SDK
set sdklib=%ooosdk%\windows\lib
set cloption=/EHsc /Os /wd4675
set cxxinc=-IC:\dev\boost_1_34_1 -I..\..\source\inc -I.\obj -I"%ooosdk%\include" -I.
set cxxcpp=-DSCSOLVER_UNO_COMPONENT -DWNT -DWIN32 -DMSC -DCPPU_ENV=msci %cxxinc%

set srcfiles=..\..\source\numeric\cellfuncobj.cxx ..\..\source\numeric\diff.cxx ..\..\source\numeric\exception.cxx ..\..\source\numeric\funcobj.cxx ..\..\source\numeric\lpbase.cxx ..\..\source\numeric\lpmodel.cxx ..\..\source\numeric\lpsolve.cxx ..\..\source\numeric\matrix.cxx ..\..\source\numeric\nlpbase.cxx ..\..\source\numeric\nlpmodel.cxx ..\..\source\numeric\nlpqnewton.cxx ..\..\source\numeric\polyeqnsolver.cxx ..\..\source\service\service.cxx ..\..\source\tool\global.cxx ..\..\source\tool\timer.cxx ..\..\source\ui\basedlg.cxx ..\..\source\ui\baselistener.cxx ..\..\source\ui\dialog.cxx ..\..\source\ui\listener.cxx ..\..\source\ui\lpbuilder.cxx ..\..\source\ui\msgdlg.cxx ..\..\source\ui\nlpbuilder.cxx ..\..\source\ui\option.cxx ..\..\source\ui\optiondlg.cxx ..\..\source\ui\resmgr.cxx ..\..\source\ui\solvemodel.cxx ..\..\source\ui\unoglobal.cxx ..\..\source\ui\unohelper.cxx ..\..\source\ui\xcalc.cxx

set sdklibs="%sdklib%\isal.lib" "%sdklib%\icppu.lib" "%sdklib%\icppuhelper.lib"

cppumaker -BUCR "%oooinst%\program\types.rdb" -O.\obj
%c% %cloption% %cxxcpp% /Fo.\obj\ -c %srcfiles%
%c% .\obj\*.obj /LD /ML %sdklibs% .\lpsolve\lpsolve55\liblpsolve55.lib /Fescsolver.uno.dll /DEF:scsolver.def
regcomp -register -r .\scsolver.uno.rdb -c .\scsolver.uno.dll
zip -r scsolver.uno.zip Addons.xcu ProtocolHandler.xcu scsolver.uno.dll scsolver.uno.rdb description.xml license.txt META-INF translation