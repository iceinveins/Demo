# <font  color='3d8c95'>动态链接、dlopen、符号决议、介入和重定向的时间顺序</font>
PLT过程链接表  
GOT全局偏移表  
动态观察：LD_DEBUG=all ./app1.elf 1> app1.log 2>&1  

## <font color="dc843f">程序启动时的动态链接阶段</font>

步骤顺序：  
1. 加载主程序及其依赖库  
动态链接器按DT_NEEDED加载所有依赖的共享库（包括LD_PRELOAD指定的库）。  
1. 重定向（Relocation）（部分）  
对已加载的库进行初步地址修正（如.data和代码中对全局变量的引用）。但延迟绑定（PLT/GOT）的符号此时可能未被解析。
1. 符号决议（Symbol Resolution）  
立即绑定：若库要求立即解析（如编译时标记为-z now），动态链接器会直接解析所有符号。
延迟绑定：默认情况下，函数符号的解析推迟到第一次调用时（通过PLT/GOT机制触发）。
1. 符号介入（Interposition）  
在符号解析时，动态链接器按优先级选择符号定义。优先级顺序通常为：  
主程序 > LD_PRELOAD库 > 后续加载的共享库（按依赖顺序）。
例如，主程序中定义的malloc会覆盖libc的版本。

## <font color="dc843f">运行时通过dlopen加载库</font>

步骤顺序：
1. 加载库及其依赖  
调用dlopen(filename, flags)加载目标库，并根据flags（如RTLD_NOW或RTLD_LAZY）决定解析时机。
2. 重定向（动态修正）与符号决议  
RTLD_NOW：立即解析所有符号并完成重定向。
RTLD_LAZY：延迟符号解析到首次使用时。
3. 符号介入的影响  
若使用RTLD_GLOBAL，新加载库的符号会加入全局符号表，可能覆盖后续符号解析。
若符号已在全局符号表中存在（如主程序或之前加载的库），则优先使用已有符号（介入生效）。
4. 作用域隔离  
默认使用RTLD_LOCAL时，新加载库的符号不会暴露给全局，仅自身及其依赖可见。
关键顺序总结


主程序启动：  
加载libc.so，其printf的GOT条目初始化为PLT桩代码。  
主程序定义了自己的printf，符号介入覆盖libc的版本。  
首次调用printf时，PLT触发解析，最终指向主程序的实现。  
运行时dlopen加载插件：  
```
void* handle = dlopen("plugin.so", RTLD_NOW | RTLD_GLOBAL);
```
立即解析plugin.so的所有符号。  
若plugin.so调用了printf，会使用主程序介入后的版本（全局符号表已存在）。  
若plugin.so导出新符号foo，且标记为全局，后续dlopen的库可能使用它。  

## <font color="dc843f">注意事项</font>
延迟绑定的代价：首次调用函数时有解析开销，但减少启动时间。  
介入的陷阱：全局符号覆盖可能导致意外行为（如LD_PRELOAD滥用）。  
可见性控制：通过RTLD_LOCAL隔离符号，避免污染全局命名空间。  