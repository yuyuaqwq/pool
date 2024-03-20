#include <cassert>
#include <iostream>
#include <vector>
#include <map>
#include <array>
#include <chrono>

#include <pool/memory_pool.hpp>
#include <pool/compact_memory_pool.hpp>
#include <pool/static_memory_pool.hpp>

int main() {
    constexpr int count = 100000000;
    constexpr int bytes = 32;
    constexpr int block_bytes = 32767;

    std::cout << "count:" << count << std::endl;
    std::cout << "bytes:" << bytes << std::endl;
    std::cout << "block_bytes:" << block_bytes << std::endl;

    pool::StaticMemoryPool<int> sp1;
    auto a = sp1.allocate();
    sp1.deallocate(a);
    pool::StaticMemoryPool<int> sp2;
    auto b = sp1.allocate();
    sp2.deallocate(b);
    assert(a == b);
    std::map<int, int, std::less<>, pool::StaticMemoryPool<std::pair<const int, int>>> map;
    for (int i = 0; i < 10000; i++) {
        map.insert({ i, i });
    }
    

    using AllocType = std::array<int32_t, bytes / 4>;
    std::vector<AllocType*> test_res(count);
    std::vector<uint32_t> test_res2(count);

    pool::StaticMemoryPool<AllocType, block_bytes> pool;

    std::chrono::steady_clock::time_point start, finish;
    std::chrono::milliseconds duration;

    std::cout << std::endl << "pool::StaticMemoryPool" << std::endl;

    start = std::chrono::steady_clock::now();
    for (int i = 0; i < 100000000; i++) {
        test_res[i] = pool.allocate();
        //auto ptr = pool.get_ptr(test_res[i]);
        //*ptr = i;
    }
    finish = std::chrono::steady_clock::now();
    duration = std::chrono::duration_cast<std::chrono::milliseconds>(finish - start);
    std::cout << "allocate: " << duration.count() << "ms" << std::endl;

    start = std::chrono::steady_clock::now();
    for (int i = 0; i < 100000000; i++) {
        pool.deallocate(test_res[i]);
    }
    finish = std::chrono::steady_clock::now();
    duration = std::chrono::duration_cast<std::chrono::milliseconds>(finish - start);
    std::cout << "deallocate: " << duration.count() << "ms" << std::endl;

    start = std::chrono::steady_clock::now();
    for (int i = 0; i < 100000000; i++) {
        test_res[i] = pool.allocate();
    }
    finish = std::chrono::steady_clock::now();
    duration = std::chrono::duration_cast<std::chrono::milliseconds>(finish - start);
    std::cout << "allocate: " << duration.count() << "ms" << std::endl;

    start = std::chrono::steady_clock::now();
    for (int i = 0; i < 100000000; i++) {
        pool.deallocate(test_res[i]);
    }
    finish = std::chrono::steady_clock::now();
    duration = std::chrono::duration_cast<std::chrono::milliseconds>(finish - start);
    std::cout << "deallocate: " << duration.count() << "ms" << std::endl;


    std::cout << std::endl << "std" << std::endl;

    start = std::chrono::steady_clock::now();
    for (int i = 0; i < 100000000; i++) {
        test_res[i] = new AllocType;
    }
    finish = std::chrono::steady_clock::now();
    duration = std::chrono::duration_cast<std::chrono::milliseconds>(finish - start);
    std::cout << "new: " << duration.count() << "ms" << std::endl;

    start = std::chrono::steady_clock::now();
    for (int i = 0; i < 100000000; i++) {
        delete(test_res[i]);
    }
    finish = std::chrono::steady_clock::now();
    duration = std::chrono::duration_cast<std::chrono::milliseconds>(finish - start);
    std::cout << "delete: " << duration.count() << "ms" << std::endl;

    start = std::chrono::steady_clock::now();
    for (int i = 0; i < 100000000; i++) {
        test_res[i] = new AllocType;
    }
    finish = std::chrono::steady_clock::now();
    duration = std::chrono::duration_cast<std::chrono::milliseconds>(finish - start);
    std::cout << "new: " << duration.count() << "ms" << std::endl;

    start = std::chrono::steady_clock::now();
    for (int i = 0; i < 100000000; i++) {
        delete(test_res[i]);
    }
    finish = std::chrono::steady_clock::now();
    duration = std::chrono::duration_cast<std::chrono::milliseconds>(finish - start);
    std::cout << "delete: " << duration.count() << "ms" << std::endl;


    std::system("pause");
}

