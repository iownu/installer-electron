Budowa instalatora luny:
I. Przygotowania

a) przygotowania: linux

potrzeba mieć:
 * git
 * npm
 * cmake >= 2.8.13
 * g++ >= 4.8.3
 * scss lub sassc
 * statycznie zbudowane qt (http://192.168.1.67/w/tutorials/kompilacja_qt_na_linuksie/statyczna_kompilacja/)
 * statycznie zbudowane icu (./configure --disable-shared --enable-static --with-data-package=static)
 * fontconfig >= 2.8 może być statyczne, ale w sumie biblioteka powinna być na wszystkich linuksach. Niemniej, na amazon linuksie budowałem statyczną paczkę
 * python >=2.7 < 3 z zainstalowanym psutil (sudo pip2 install psutil)


b) przygotowania MacOS
 * git
 * npm
 * cmake >= 2.8.13
 * sassc (tudzież jakiś kompilator scss do css)

c) przygotowania Windows

* Trzeba zainstalować msys2, otworzyć i wykonać podstawowe aktualizacje (pacman -Syu, potem restart i jeszcze raz pacman -Syu)
* W konsoli msys2 zainstalować: git, cmake, make, mingw-w64-x86_64-gcc, mingw-w64-x86_64-qt5-static, mingw-w64_x86_64-sassc
* POZA Msys2 zainstalować: python2.7, nodejs oraz npm, najlepiej w domyślnych katalogach
* instalujemy psutil (w cmd.exe: C:\Python27\Scripts\pip.exe install psutil)
* instalujemy electron-packagera (C:\Program Files\nodejs\npm.exe install -g electron-packager)
* trzeba podać node do path (niestety; w przeciwnym razie electron-packager nie działa). Do ~/.bashrc dopisujemy export PATH="/c/Program Files/nodejs/:$PATH"

Pamiętamy, żeby w II. podać do cmake odpowiednie ścieżki do statycznego qt, czyli -DCMAKE_PREFIX_PATH=/mingw64/qt-static


II. Kompilacja wszystkiego

Należy zainstalować electron-packagera tak, aby mieć go w ścieżce:

sudo npm install -g electron-packager

klonujemy repo. robimy katalog do budowy, gdzieś obok najlepiej. Wywołujemy w nim komendę:

cmake ../ścieżka/do/repo -DCONSOLE_INSTALLER_EXE="/ścieżka/do/konsolowego/instalatora" -DCMAKE_BUILD_TYPE=Release

Jeżeli biblioteki mamy pobudowane w dziwnych ścieżkach, to przekazujemy je do cmake opcją -DCMAKE_PREFIX_PATH="ścieżka1;ścieżka2". Podobnie, jeżeli pythona, npm albo sassc wywołuje się w sposób nietypowy (np. inaczej niż "python2.7" na linuksie), to dodajemy opcje odpowiednio -DPYTHON_EXE="wywołanie pythona", -DNPM_EXE="wywołanie npm" -DSASS_EXE="wywołanie scss"

Jak wszystko się uda, to odpalamy make.

Podczas linkowania może wyjść brak różnych bibliotek, trzeba je wtedy zazwyczaj doinstalować/dokompilować.



III. Generowanie samej apki elektronowej (bez kompilacji)

po cmake jak w pkt. II. robimy:

make lunaInstallerApp

