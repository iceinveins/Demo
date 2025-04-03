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