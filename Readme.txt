Budowa instalatora luny:

a) linux

potrzeba mieć npm, cmake, g++, oraz statycznie zbudowane qt (http://192.168.1.67/w/tutorials/kompilacja_qt_na_linuksie/statyczna_kompilacja/) oraz icu (./configure --disable-shared --enable-static --with-data-package=static)

Należy zainstalować electron-packagera tak, aby mieć go w ścieżce:

sudo npm install -g electron-packager

Ponadto potrzebujemy moduł psutil do pythona

sudo pip install psutil # może też być pip2



