#include "rxcpp/rx.hpp"
#include "logger/logger.hpp"
#include <vector>
#include <string>

using namespace std;
namespace rxop = rxcpp::operators;
void test_hello()
{
    auto iron_plate_observable = rxcpp::observable<>::range(1, 5);
    iron_plate_observable
    .filter(
        [](int iron_plate){
            return iron_plate % 2 == 0;  // 返回为false的iron_plate会被过滤
        }
    )
    .map(
        [](int iron_plate){
            // 将int转化为string，并添加前缀car_door
            std::string car_door = "car_door_" + std::to_string(iron_plate);
            return car_door;
        }
    )
    .subscribe(
        [](std::string iron_plate){
            logi("DEBUG", "on_next: {}", iron_plate);
        },
        [](){
            logi("DEBUG", "after work");
        }
    );
}

void test_map_pipeline()
{
    namespace rxop = rxcpp::operators;
    //
    // support range() | filter() | subscribe() syntax
    // '|' is spelled 'pipe'
    //
    auto iron_plate_observable = rxcpp::observable<>::range(1, 5);
    iron_plate_observable | 
    rxop::filter([](int iron_plate){
        return iron_plate % 2 == 0;
    }) |
    rxop::map([](int iron_plate){
        std::string car_door = "car_door_" + std::to_string(iron_plate);
        return car_door;
    }) |
    rxop::subscribe<std::string>(
        [](std::string car_door){
            SPDLOG_INFO("on_next: {}", car_door);
        },
        [](){
            SPDLOG_INFO("after work");
        }
    );
}
void test_concat_or_merge()
{
    auto ob1 = rxcpp::observable<>::range(1,3);
    auto ob2 = rxcpp::observable<>::range(4,6);

    ob1 | 
    rxop::concat(ob2) | 
    // rxop::merge(ob2) | 
    rxop::subscribe<int>(
        [](int val) { SPDLOG_INFO("on next:{}", val); },
        [](){ SPDLOG_INFO("on completed"); }
    );
}
void test_take()
{
    auto observable = rxcpp::observable<>::range(1,6);
    observable | 
    rxop::take(8) | 
    rxop::subscribe<int>(
        [](int val) { SPDLOG_INFO("on next:{}", val); },
        [](){ SPDLOG_INFO("on completed"); }
    );
}
void test_flatmap()
{
    vector<string> last_name = {"Zhang", "Li"};
    auto observable = rxcpp::observable<>::iterate(last_name);
    observable | 
    rxop::flat_map([](string last){
        // rxop::concat_map([](string last){
            vector<string> first_name = {"San", "Si", "Wu"};
            return rxcpp::observable<>::iterate(first_name);
        },
        [](string last, string first){
            return last + " " + first;
        }) | 
    rxop::subscribe<string>(
        [](string name) { SPDLOG_INFO("next: {}", name); },
        [](){ SPDLOG_INFO("on completed"); }
    );
}
void test_buffer()
{
    auto observable = rxcpp::observable<>::range(1,10);
    observable | 
    rxop::buffer(3) |
    rxop::subscribe<vector<int>>(
        [](vector<int> vals) { for(auto it: vals){ cout<<it<<","; } cout<<endl; },
        [](){ cout<<"on completed"; }
    );
}
void test_groupby()
{
    using namespace rxcpp;
    using namespace rxcpp::sources;
    using namespace rxcpp::subjects;
    using namespace rxcpp::util;
    struct Person
    {
        string name;
        string gender;
        int age;
    };

    subject<Person> person;
    // 通过gender字段对Person进行分组，分组后保留name和age字段
    auto agebygender = person.get_observable().group_by(
        [](Person& p){ return p.gender; },
        [](Person& p){ return std::pair<string, int>(p.name, p.age); }
    );
    auto result = agebygender.map(
        // group_by将observable转化为grouped_observable类型，为key：val的结构，通过map进行数据的转换
        // combine_latest将在所有数据完成聚合(count,last,min,max, max_x3,average)的结果时调用一次，将聚合结果
        // 构造为tuple对象返回，再通过merge操作，将键值唯一的tuple进行合并
        [](grouped_observable<string, pair<string, int>> gp){
            return gp.count().combine_latest(
                [=](int count, string last_name, int min, int max, int max_x3, double avg){
                    return make_tuple(gp.get_key(), count, last_name, min, max, max_x3, avg);
                },
                gp.map([](const pair<string, int>& p)->string{ return p.first; }).last(),
                gp.map([](const pair<string, int>& p)->int { return p.second; }).min(),
                gp.map([](const pair<string, int>& p)->int { return p.second; }).max(),
                // 下面自定义的聚合函数求的是最大值的3倍，和上一句的max()是有区别的
                gp.map([](const pair<string, int>& p)->int { return p.second; })
                    .accumulate(numeric_limits<int>::min(), // 可以看做下面匿名函数形参max_acc的初始值
                                [](int max_acc, int v){ return max(max_acc, v); }, // 循环迭代，求累积最大值
                                [](int max_acc){ return max_acc * 3; } // 当循环结束，最后的累积最大值再扩大3倍返回
                                ),
                gp.map([](const pair<string, int>& p)->double { return p.second; }).average()
            );
        }
    ).merge();
    // 打印结果
    result.subscribe(apply_to([](string gender, int count, string last_name, int min, int max, int max_x3, double avg){
        SPDLOG_INFO("gender={}, count={}, last_name={}, range=[{},{}], max*3={}, avg={}", gender, count, last_name, min, max, max_x3, avg);
    }));
    // 输入数据流
    observable<>::from(
        Person{"Tom", "Male", 32},
        Person{"Tim", "Male", 12},
        Person{"Stel", "Other", 42},
        Person{"Flor", "Female", 24},
        Person{"Fran", "Female", 97}
    ).subscribe(person.get_subscriber());
}

int main() {
    initLogger();
    test_hello();
    test_map_pipeline();
    test_concat_or_merge();
    test_take();
    test_flatmap();
    test_buffer();
    test_groupby();
    return 0;
}