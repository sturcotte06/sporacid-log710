SET username=xx
SET password=xx
SET hostname=cluster-srv0.logti.etsmtl.ca
SET srcdir=..\..\src\labo3
SET dstdir=/export/LOG710/home/%username%/labo3

bin\plink.exe %hostname% -ssh -l %username% -pw %password% "rm -rf %dstdir%/*"
bin\pscp.exe -l %username% -pw %password% -r %srcdir%\* %hostname%:%dstdir%
bin\plink.exe %hostname% -ssh -l %username% -pw %password% "find %dstdir% -type f -exec sh -c 'tr -d "\\r" < "{}" > "{}".new && mv "{}".new "{}"' -- {} \;"
bin\plink.exe %hostname% -ssh -l %username% -pw %password% "cd %dstdir%; chmod 777 ./make; ./make;"