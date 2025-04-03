mylib1: my_class.hpp my_lib1.cpp
	g++ -fPIC -shared -o libmylib1.so my_lib1.cpp

mylib2: my_class.hpp my_lib2.cpp
	g++ -fPIC -shared -o libmylib2.so my_lib2.cpp

mylib3: my_class.hpp my_lib3.cpp
	g++ -fPIC -shared -o libmylib3.so my_lib3.cpp

app1: app1.cpp my_class.hpp mylib1 mylib2 mylib3
	g++ -L./ -lmylib1 -lmylib2 -lmylib3 -o app1.elf app1.cpp

app2: app2.cpp my_class.hpp
	g++ -ldl -o app2.elf app2.cpp

app3: app3.cpp my_class.hpp
	g++ -ldl -o app3.elf app3.cpp

app4: app4.cpp my_class.hpp
	g++ -ldl -o app4.elf app4.cpp

app5: app5.cpp my_class.hpp
	g++ -rdynamic -ldl -o app5.elf app5.cpp 
	g++ -fPIE -pie -rdynamic -ldl -o app5.elf app5.cpp 

all: app1 app2 app3 app4 app5

clean:
	rm -f *.elf lib*.so