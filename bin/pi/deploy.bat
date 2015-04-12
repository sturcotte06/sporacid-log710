SET username=xx
SET password=xx
SET hostname=xx
SET bindir=..\bin
SET srcdir=..\..\src\pi
SET dstdir=/home/%username%/pi

%bindir%\plink.exe %hostname% -ssh -l %username% -pw %password% "rm -rf %dstdir%/*"
%bindir%\pscp.exe -l %username% -pw %password% -r %srcdir%\* %hostname%:%dstdir%
%bindir%\plink.exe %hostname% -ssh -l %username% -pw %password% "find %dstdir% -type f -exec sh -c 'tr -d "\\r" < "{}" > "{}".new && mv "{}".new "{}"' -- {} \;"
%bindir%\plink.exe %hostname% -ssh -l %username% -pw %password% "cd %dstdir%; chmod 777 ./make; ./make;"