SET username=xx
SET password=xx
SET hostname=cluster-srv0.logti.etsmtl.ca
SET srcdir=..\..\src\labo2
SET dstdir=/export/LOG710/home/xx/labo2

bin\plink.exe %hostname% -ssh -l %username% -pw %password% "rm -rf %dstdir%/*"
bin\pscp.exe -l %username% -pw %password% -r %srcdir%\* %hostname%:%dstdir%
REM bin\plink.exe %hostname% -ssh -l %username% -pw %password% "chmod 777 %dstdir%/make"
bin\plink.exe %hostname% -ssh -l %username% -pw %password% "find %dstdir% -type f -exec sh -c 'tr -d "\\r" < "{}" > "{}".new && mv "{}".new "{}"' -- {} \;"

REM PAUSE