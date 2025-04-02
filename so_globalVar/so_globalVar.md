# <font color="3d8c95">动态库与全局静态变量</font>
在C++大型项目中，当包含静态变量的公共模块被编译到多个动态库（so）中，并由同一程序通过dlopen加载时，会导致每个动态库拥有该静态变量的独立实例。程序退出时，这些实例的析构可能引发double free问题。以下是

# <font color="3d8c95">根本原因</font>
静态变量的重复实例化：每个动态库独立编译公共模块，导致各自包含静态变量的副本。

独立析构：动态库卸载时，各自实例的析构函数尝试释放同一资源，引发重复释放。

## <font color="dc843f">解决方案</font>
1. 将公共模块编译为独立共享库  
    步骤：  
    将包含静态变量的模块单独编译为共享库（如libcommon.so）。  
    其他动态库在编译时链接libcommon.so，而非直接包含模块源码。  
    确保所有动态库运行时正确加载同一libcommon.so（通过LD_LIBRARY_PATH或rpath）。  
    效果：所有动态库共享同一静态变量实例，确保仅一次初始化和析构。  

    示例：
    ```
    // common.h（声明单例接口）
    class Singleton {
    public:
        static Singleton& getInstance();
    private:
        Singleton();
        ~Singleton();
    };

    // common.cpp（实现并编译到libcommon.so）
    #include "common.h"
    Singleton& Singleton::getInstance() {
        static Singleton instance; // 唯一实例
        return instance;
    }
    ```
2. 控制符号可见性  
    步骤：  
    使用编译选项`-fvisibility=hidden`隐藏动态库中非必要符号。  
    显式导出需要共享的符号（通过`__attribute__((visibility("default")))`）。  
    效果：避免符号冲突，确保动态库引用主程序或其他库中的唯一实例。  

    示例：
    ```
    // 导出符号
    __attribute__((visibility("default"))) extern int global_var;
    ```

3. 使用动态分配与全局管理  
    步骤：  
    将静态变量改为指针（如static T* instance）。  
    在程序启动时显式初始化，退出时统一释放资源。  
    禁用动态库的自动析构（如通过dlclose时不卸载库）。  

    示例：
    ```
    // 公共模块
    static SomeClass* instance = nullptr;

    void initModule() {
        if (!instance) instance = new SomeClass();
    }

    void cleanupModule() {
        delete instance;
        instance = nullptr;
    }

    // 主程序
    int main() {
        initModule();
        // ... dlopen等操作
        cleanupModule();
        return 0;
    }
    ```
4. 避免依赖静态变量析构  
    策略：  
    使用智能指针（如std::shared_ptr）管理资源，确保无主时自动释放。  
    设计无需清理的静态变量（如无锁结构、纯数据）。  

    示例：
    ```
    static std::shared_ptr<Resource> resource = std::make_shared<Resource>();
    ```
    // 所有动态库共享引用计数，资源仅释放一次

## <font color="dc843f">关键原则</font>
共享实例：确保所有动态库使用同一静态变量实例，而非各自副本。

资源所有权集中管理：由主程序或独立库控制资源的生命周期。

符号可见性控制：避免符号冲突导致重复定义。


## <font color="dc843f">示例代码解析</font>
my_class.hpp中声明了g_fun和MyClass，由这两个根据不同场景实例化全局变量
1. app1  直接链接
    ```
    app1: app1.cpp my_class.hpp mylib1 mylib2
	    g++ -L./ -lmylib1 -lmylib2 -o app1 app1.cpp
    ```
    结果输出
    ```
    Construct! 3 @ 0x404194
    app1.cpp:6
    Construct! 2 @ 0x404194
    app1.cpp:6
    Construct! 1 @ 0x404194
    app1.cpp:6
    Construct! 11 @ 0x404194
    app1.cpp:6
    ----------
    app1.cpp:6
    ----------
    Destruct! 11 @ 0x404194
    Destruct! 11 @ 0x404194
    Destruct! 11 @ 0x404194
    Destruct! 11 @ 0x404194
    ```
    <font color="fed3a8">g_func</font>  
    其中g_func的输出都为app1.cpp:6 其根据编译时的链接顺序决定: 
    <font color="fed3a8">本编译单元>-l链接库1>-l链接库2</font>，即如果将app1.cpp中的g_func注释后，调整-l顺序如下
    ```
    g++ -L./ -lmylib2 -lmylib1 -lmylib3 -o app1 app1.cpp
    ```
    则对应输出为 my_lib2.cpp:7

    <font color="fed3a8">MyClass g_var</font>  
    当程序启动时，动态链接器（如 ld-linux.so）会解析可执行文件的依赖关系，并加载所有直接链接的动态库（递归加载其依赖的库）
    1. 动态链接器将动态库映射到进程的虚拟地址空间。
    2. 执行动态库的 重定位（relocation）和 符号解析（symbol resolution）。
    3. 调用动态库的 初始化函数（如 .init 段或构造函数属性标记的函数）。
   
    静态变量的初始化发生在动态库加载后的初始化阶段，即动态链接器调用库的初始化函数时。

    <font color="fed3a8">初始化顺序：</font>  
    同一编译单元内的静态变量按定义顺序初始化。  
    不同编译单元或不同动态库间的静态变量初始化顺序是**未定义**的

    析构时逆序，所以app1 > my_libX 且因为地址相同（<font color="fed3a8">符号介入机制：动态链接器按加载顺序将符号填入全局符号表。第一个加载的库中的符号会被后续库引用。</font>）导致 
    ```
    Destruct! 11 @ 0x404194
    Destruct! 11 @ 0x404194
    Destruct! 11 @ 0x404194
    Destruct! 11 @ 0x404194
    ```

2. app2 dlopen 全部NOW+LOCAL
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
    <font color="fed3a8">g_func</font>   
    <font color="fed3a8">MyClass g_var</font> 
    如果动态库通过 dlopen 显式加载（非 -l 直接链接）：  
    静态变量的初始化发生在 dlopen 调用时（若使用 RTLD_NOW）。  
    析构函数在 dlclose 调用时执行（若库的引用计数归零）。  
    所以先执行了app2.cpp:9然后根据代码中dlopen顺序执行

    RTLD_LOCAL下每个so都有自己的副本，所以地址都不一样，各管各的

3. app3 全部NOW+GLOBAL
    结果输出
    ```
    Construct! 33 @ 0x404194
    app3.cpp:9
    Construct! 1 @ 0x7fe753beb074
    my_lib1.cpp:7
    Construct! 2 @ 0x7fe753beb074
    my_lib1.cpp:7
    Construct! 3 @ 0x7fe753beb074
    my_lib1.cpp:7
    ----------
    app3.cpp:9
    ----------
    Destruct! 3 @ 0x7fe753beb074
    Destruct! 3 @ 0x7fe753beb074
    Destruct! 3 @ 0x7fe753beb074
    Destruct! 33 @ 0x404194
    ```
    <font color="fed3a8">g_func</font>  
    <font color="fed3a8">MyClass g_var</font>  
    有点类似于直接-l链接，所有符号都记录在一张全局符号表中
    但是注意！<font color="fed3a8">构造函数不会被导出为全局符号（默认隐藏），因此不参与动态符号表的重定向。</font>
    每个动态库的构造函数（__attribute__((constructor))）在加载时独立执行，但操作的变量已被重定向。
    析构函数（__attribute__((destructor))）在库卸载时调用，但操作的变量地址仍为介入后的地址，因此多次打印相同地址。


1. app4
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
    <font color="fed3a8">g_func</font> 
    <font color="fed3a8">MyClass g_var</font> 