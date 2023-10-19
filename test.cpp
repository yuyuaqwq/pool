#include <iostream>
#include <map>
#include <set>
#include <vector>
#include <array>
#include <chrono>

#include <fpoo/memory_pool.hpp>

#include <rbt/set.hpp>

#include <vector>

int main() {
    //fpoo::CompactMemoryPool<rbt::set<uint64_t>::Node> cmp;

    std::vector<int> fake;

    decltype(operator<=>(std::declval<std::string>(), std::declval<std::string>())) aaa;

    rbt::set<uint64_t> set;
    uint64_t adawaaaa = 31321;
    set.insert(adawaaaa);
    set.insert(233);
    set.insert(999999);

    sizeof(set);

    std::map<int, int> aaaaa;
    std::set<int, std::less<int>, fpoo::MemoryPool<int>> aaaaaa;
    
    aaaaaa.insert(100);
    aaaaaa.insert(200);

    using AllocType = std::array<int32_t, 2>;


    std::cout << "size: " << sizeof(AllocType) << std::endl;

    
    std::vector<AllocType*> test_res(100000000);

    std::vector<AllocType*> test_res2(100000000);


    fpoo::MemoryPool<AllocType> pool;

    std::chrono::steady_clock::time_point start, finish;
    std::chrono::milliseconds duration;


    std::cout << "fpoo::MemoryPool" << std::endl;

    //sizeof(fpoo::MemoryPool<AllocType>::Slot);

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
        //*test_res2[i] = i;
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
        new AllocType;
    }

    finish = std::chrono::steady_clock::now();
    duration = std::chrono::duration_cast<std::chrono::milliseconds>(finish - start);
    std::cout << "new: " << duration.count() << "ms" << std::endl;

    //
    //auto pos2 = pool.allocate();
    //pool.deallocate(pos);
    //auto pos3 = pool.allocate();
    //auto pos4 = pool.allocate();


    std::cout << "Hello World!\n";
}

