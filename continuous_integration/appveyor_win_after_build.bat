echo %1
dir
pushd c:\b
cmake --build . --config Release --target install
cmake --build . --config Release --target package
popd
dir C:\d\share\apps\lima\tests
pushd C:\d\share\apps\lima\tests
set PATH=c:\d\bin;%PATH%
set LIMA_CONF=C:\d\share\config\lima
set LIMA_RESOURCES=C:\d\share\apps\lima\resources
tva --language=eng test-eng.tokenizer.xml test-eng.simpleword.xml test-eng.idiom.xml test-eng.abbrev.xml test-eng.hyphen.xml test-eng.default.xml test-eng.se-PERSON.xml test-eng.se-DATE.xml test-eng.se-EMBEDED.xml test-eng.se-ORGANISATION.xml test-eng.se.xml test-eng.se-LOCATION.xml test-eng.disambiguated.xml test-eng.sa.xml 2>&1 | tee tva-eng.log
cat tva-eng.log
tva --language=fre test-fre.tokenizer.xml test-fre.simpleword.xml test-fre.hyphen.xml test-fre.idiom.xml test-fre.default.xml test-fre.se.xml test-fre.disambiguated.xml test-fre.sa.xml 2>&1 | tee tva-fre.log
cat tva-fre.log
popd
pushd C:\b
echo %1
copy lima*.exe %1
rem %APPVEYOR_BUILD_FOLDER%
popd


