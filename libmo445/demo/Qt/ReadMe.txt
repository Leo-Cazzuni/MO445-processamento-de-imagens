#guide to compile any Qt project

#0. open the terminal

#1. go to the project folder
cd some/path/to/ift/folder/demo/Qt/someProject

#2.generate makefile
qmake someProject.pro

#3. compile
make

#4. run
./someProject


**Commom Issues**

*qmake is not defnied
After downloading Qt, you can find qmake on /home/user/Qt/x.x/gcc_64/bin/qmake. So the second step in "Compile and Run"
section becomes

/home/user/Qt/x.x/gcc_64/bin/qmake Teste.pro

where x.x is the Qt version (e.g. 5.8)