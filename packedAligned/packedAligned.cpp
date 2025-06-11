#include <cstddef>
#include <iostream>

#pragma pack(push, 1)  // 禁用内部填充
struct alignas(16) PackedAligned {  // 整个结构体 16 字节对齐
    char a;            // 1 字节
    int b;             // 4 字节
    double c;          // 8 字节
    
    // 手动添加尾部填充，使总大小成为 16 的倍数
    char padding[16 - (sizeof(a) + sizeof(b) + sizeof(c)) % 16];
};
#pragma pack(pop)  // 恢复默认对齐

static_assert(sizeof(PackedAligned) == 16, "Size must be 16 bytes");
static_assert(alignof(PackedAligned) == 16, "Alignment must be 16 bytes");

int main() {
    PackedAligned obj;
    
    // 验证成员偏移（应无内部填充）
    std::cout << "Offsets:\n"
              << "a: " << offsetof(PackedAligned, a) << "\n"  // 0
              << "b: " << offsetof(PackedAligned, b) << "\n"  // 1
              << "c: " << offsetof(PackedAligned, c) << "\n"; // 5
    
    // 验证地址对齐
    std::cout << "Address=" << std::hex << &obj << " aligned to 16: " 
              << (reinterpret_cast<uintptr_t>(&obj) % 16) << "\n"; // 0
}