rem auto-find makecert.exe util provided that vcvarsXX.bat was executed
"%WindowsSdkVerBinPath%%VSCMD_ARG_TGT_ARCH%\makecert.exe" -r -pe -ss PrivateCertStore -n CN=Windows(TEST) -eku 1.3.6.1.5.5.7.3.3 Test.cer

rem CertMgr /add Test.cer /s /r localMachine root
rem certmgr -add Test.cer -s -r localMachine TRUSTEDPUBLISHER