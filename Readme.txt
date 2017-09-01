Budowa instalatora luny:
I. Przygotowania

a) przygotowania: linux

potrzeba mieć:
 * npm
 * cmake >= 2.8.13
 * g++ >= 4.8.3
 * scss lub sassc
 * statycznie zbudowane qt (http://192.168.1.67/w/tutorials/kompilacja_qt_na_linuksie/statyczna_kompilacja/)
 * statycznie zbudowane icu (./configure --disable-shared --enable-static --with-data-package=static)
 * freetype >= 2.8 może być statyczne, ale w sumie biblioteka powinna być na wszystkich linuksach
 * python >=2.7 < 3 z zainstalowanym psutil (sudo pip2 install psutil)

b) przygotowania MacOS
 * npm
 * cmake >= 2.8.13
 * sassc (tudzież jakiś kompilator scss do css)

c) przygotowania Windows

TBA



II. Kompilacja wszystkiego

Należy zainstalować electron-packagera tak, aby mieć go w ścieżce:

sudo npm install -g electron-packager

klonujemy repo. robimy katalog do budowy, gdzieś obok najlepiej. Wywołujemy w nim komendę:

cmake ../ścieżka/do/repo -DCONSOLE_INSTALLER_EXE="/ścieżka/do/konsolowego/instalatora" -DCMAKE_BUILD_TYPE=Release

Jeżeli biblioteki mamy pobudowane w dziwnych ścieżkach, to przekazujemy je do cmake opcją -DCMAKE_PREFIX_PATH="ścieżka1/ścieżka2". Podobnie, jeżeli pythona albo sassc wywołuje się w sposób nietypowy (np. inaczej niż "python2.7" na linuksie), to dodajemy opcje odpowiednio -DPYTHON_EXE="wywołanie pythona" -DSASS_EXE="wywołanie scss"

Jak wszystko się uda, to odpalamy make.

Podczas linkowania może wyjść brak różnych bibliotek, trzeba je wtedy zazwyczaj doinstalować/dokompilować.



III. Generowanie samej apki elektronowej (bez kompilacji)

po cmake jak w pkt. II. robimy:

make lunaInstallerApp

