# <font color="3d8c95">示例代码解析</font>
my_class.hpp中声明了g_fun和MyClass，由这两个根据不同场景实例化全局变量  
app1.log对应执行LD_DEBUG=libs ./app1.elf 1> app1.log 2>&1的日志

LD_DEBUG显示的阶段顺序：  
relocation processing  
calling init  
initialize program   
transferring control  
calling fini  
## <font color="dc843f">app1.elf</font>
编译时直接链接
```
app1: app1.cpp my_class.hpp mylib1 mylib2
    g++ -L./ -lmylib1 -lmylib2 -o app1 app1.cpp
```
结果输出
```
Construct! 3 @ 0x7f078f5a2074
app1.cpp:6
Construct! 2 @ 0x7f078f5a2074
app1.cpp:6
Construct! 1 @ 0x7f078f5a2074
app1.cpp:6
Construct! 11 @ 0x404198
app1.cpp:6
----------
app1.cpp:6
----------
Destruct! 11 @ 0x404198
Destruct! 1 @ 0x7f078f5a2074
Destruct! 1 @ 0x7f078f5a2074
Destruct! 1 @ 0x7f078f5a2074
```

g_func在链接期间已经完成重定位（LD_DEBUG日志app1.log中不会有它），并且根据链接时优先顺序<font color="fed3a8">本编译单元 > -l链接库1 > -l链接库2</font>  
导致其中g_func的输出都为本编译单元的app1.cpp:6  
而如果将app1.cpp中的g_func注释后，调整-l顺序如下
```
g++ -L./ -lmylib2 -lmylib1 -lmylib3 -o app1 app1.cpp
```
则对应输出为-lmylib2中的my_lib2.cpp:7

[lib3 relocation processing阶段](app1.log#L7888)  
[binding MyClass析构函数符号到app1.elf](app1.log#L7900)  
[binding g_var变量符号到libmylib1.so](app1.log#L7935)  
[lib3 calling init阶段](app1.log#L8192)  
[binding MyClass构造函数符号到app1.elf](app1.log#L8540)  
[打印Construct! 3](app1.log#L8679)   
同理lib2 lib1

在链接阶段，app1.cpp中的MyClass构造析构函数以及g_var都已完成重定位0x404198

## <font color="dc843f">app2.elf</font>
```
dlopen("./libmylib1.so", RTLD_NOW | RTLD_LOCAL);
dlopen("./libmylib2.so", RTLD_NOW | RTLD_LOCAL);
dlopen("./libmylib3.so", RTLD_NOW | RTLD_LOCAL);
```
结果输出
```
Construct! 22 @ 0x404194
app2.cpp:9
Construct! 1 @ 0x7f0e941ed074
my_lib1.cpp:7
Construct! 2 @ 0x7f0e941e2074
my_lib2.cpp:7
Construct! 3 @ 0x7f0e941dd074
my_lib3.cpp:7
----------
app2.cpp:9
----------
Destruct! 3 @ 0x7f0e941dd074
Destruct! 2 @ 0x7f0e941e2074
Destruct! 1 @ 0x7f0e941ed074
Destruct! 22 @ 0x404194
```

[lib1 relocation processing阶段](app2.log#L6095)  
[binding MyClass析构函数符号到libmylib1.so](app2.log#L6119)  
[binding g_var变量符号到libmylib1.so](app2.log#L6165)  
[binding g_fun函数符号到libmylib1.so](app2.log#L6208)
[binding MyClass构造函数符号到libmylib1.so](app2.log#L6228)  
[lib1 calling init阶段](app2.log#L6253)  
[打印Construct! 1](app2.log#L6255)  
同理lib2 lib3

在链接阶段，app2.cpp中的MyClass构造析构函数以及g_var都已完成重定位0x404194
RTLD_LOCAL 阻止当前库的符号被后续库看到，所以每个后续的lib看不到前面的于是用自己的

## <font color="dc843f">app3.elf</font>
```
dlopen("./libmylib1.so", RTLD_NOW | RTLD_GLOBAL);
dlopen("./libmylib2.so", RTLD_NOW | RTLD_GLOBAL);
dlopen("./libmylib3.so", RTLD_NOW | RTLD_GLOBAL);
```
结果输出
```
Construct! 33 @ 0x404198
app3.cpp:9
Construct! 1 @ 0x7f0d57051074
my_lib1.cpp:7
Construct! 2 @ 0x7f0d57051074
my_lib1.cpp:7
Construct! 3 @ 0x7f0d57051074
my_lib1.cpp:7
----------
app3.cpp:9
----------
Destruct! 3 @ 0x7fe753beb074
Destruct! 3 @ 0x7fe753beb074
Destruct! 3 @ 0x7fe753beb074
Destruct! 33 @ 0x404194
```
[lib1 relocation processing阶段](app3.log#L6095)  
[binding MyClass析构函数符号到libmylib1.so](app3.log#L6119)  
[binding g_var变量符号到libmylib1.so](app3.log#L6165)  
[binding g_fun函数符号到libmylib1.so](app3.log#L6208)
[binding MyClass构造函数符号到libmylib1.so](app3.log#L6228)  
[lib1 calling init阶段](app3.log#L6253)  
[打印Construct! 1](app3.log#L6255)  
同理lib2 lib3

在链接阶段，app2.cpp中的MyClass构造析构函数以及g_var都已完成重定位0x404198  
以RTLD_GLOBAL加载时，其符号g_var会被添加到全局符号表。  
后续加载的库在解析g_var时，会优先使用全局符号表中已有的定义，导致所有库中的g_var指向同一地址。

## <font color="dc843f">app4.elf</font>
```
dlopen("./libmylib1.so", RTLD_NOW | RTLD_GLOBAL);
dlopen("./libmylib2.so", RTLD_NOW | RTLD_LOCAL);
dlopen("./libmylib3.so", RTLD_NOW | RTLD_GLOBAL);
```
结果输出
```
Construct! 44 @ 0x404198
app4.cpp:9
Construct! 1 @ 0x7fa6a1aaa074
my_lib1.cpp:7
Construct! 2 @ 0x7fa6a1aaa074
my_lib1.cpp:7
Construct! 3 @ 0x7fa6a1aaa074
my_lib1.cpp:7
----------
app4.cpp:9
----------
Destruct! 3 @ 0x7fa6a1aaa074
Destruct! 3 @ 0x7fa6a1aaa074
Destruct! 3 @ 0x7fa6a1aaa074
Destruct! 33 @ 0x404198
```
[lib1 relocation processing阶段](app4.log#L6095)  
[binding MyClass析构函数符号到libmylib1.so](app4.log#L6119)  
[binding g_var变量符号到libmylib1.so](app4.log#L6165)  
[binding g_fun函数符号到libmylib1.so](app4.log#L6208)
[binding MyClass构造函数符号到libmylib1.so](app4.log#L6228)  
[lib1 calling init阶段](app4.log#L6253)  
[打印Construct! 1](app4.log#L6255)  
[lib2 relocation processing阶段](app4.log#L6275)  
[binding MyClass析构函数符号到libmylib1.so](app4.log#L6303)   
[binding g_var变量符号到libmylib1.so](app4.log#L6351)  
[binding g_fun函数符号到libmylib1.so](app4.log#L6395)  
[binding MyClass构造函数符号到libmylib1.so](app4.log#L6415)  
[lib2 calling init阶段](app4.log#L6440)  
[打印Construct! 2](app4.log#L6442)  

在链接阶段，app2.cpp中的MyClass构造析构函数以及g_var都已完成重定位0x404198  
RTLD_LOCAL仅阻止当前库的符号被后续库看到，但不会阻止当前库使用全局符号表中已有的符号。因此，即使以RTLD_LOCAL加载，如果全局符号表中已有g_var，仍会复用该地址。

## <font color="dc843f">app5.elf</font>
```
dlopen("./libmylib1.so", RTLD_NOW | RTLD_GLOBAL);
dlopen("./libmylib2.so", RTLD_NOW | RTLD_GLOBAL);
dlopen("./libmylib3.so", RTLD_NOW | RTLD_GLOBAL);
且编译主程序app5.elf时添加-rdynamic将符号都导入到动态符号表中
```
结果输出
```
Construct! 55 @ 0x404194
app5.cpp:9
Construct! 1 @ 0x404194
app5.cpp:9
Construct! 2 @ 0x404194
app5.cpp:9
Construct! 3 @ 0x404194
app5.cpp:9
----------
app5.cpp:9
----------
Destruct! 3 @ 0x404194
Destruct! 3 @ 0x404194
Destruct! 3 @ 0x404194
Destruct! 3 @ 0x404194
```
因为主程序将g_var符号暴露,所以所有后续的动态库都重定向到同一个地址 0x404194

此外，如果编译主程序时再添加-fPIE -pie， 则其输出的地址明显变化
```
Construct! 55 @ 0x561c77b16194
app5.cpp:9
Construct! 1 @ 0x561c77b16194
app5.cpp:9
Construct! 2 @ 0x561c77b16194
app5.cpp:9
Construct! 3 @ 0x561c77b16194
app5.cpp:9
----------
app5.cpp:9
----------
Destruct! 3 @ 0x561c77b16194
Destruct! 3 @ 0x561c77b16194
Destruct! 3 @ 0x561c77b16194
Destruct! 3 @ 0x561c77b16194
```
运行时重定向