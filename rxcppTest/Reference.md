https://blog.csdn.net/m0_60274660/article/details/149158228
https://zhuanlan.zhihu.com/p/598792670

# 从一个最简 Observable 开始
不用线程、不接网络，先看数据怎么“发出来”：

用 rxcpp::observable::just(1, 2, 3) 创建一个立即发出 1、2、3 的同步序列；
用 .subscribe([](int v) { std::cout })接收并打印 —— 这就是最基础的 Observer；
注意：subscribe() 调用后，数据立刻推送到 lambda，整个过程是同步阻塞的，没有后台线程。

# 让事件真正“异步”起来
响应式真正的价值在于解耦执行时机。RxCpp 提供调度器（Scheduler）控制在哪跑：

用 rxcpp::observe_on_event_loop() 或 rxcpp::synchronize_new_thread() 获取调度器；
链式调用 .observe_on(scheduler) 把后续操作切到目标线程；
例如：定时发数 —— rxcpp::observable::interval(std::chrono::seconds(1)) 默认在事件循环中每秒发一个 long 值，配合 take(5) 可限制次数。

1. concat:使用concat将多个observable合并为一个，concat操作符前面的ob1发射完所有数据后ob2再发射。 
2. merge:代码编写方式和concat一致，但是ob1和ob2的数据是交替发射的，就像车道合并，交替通行一样。
3. take:只发射observable的前N个数据，忽略剩余数据。如果take 大于总量，则最终为总量数量