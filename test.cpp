#include <iostream>
#include <map>
#include <set>
#include <array>
#include <chrono>

#include <spoo/MemoryPool.h>
#include <spoo/memory_pool.hpp>


int main() {
    std::map<int, int> aaaaa;
    std::set<int> aaaaaa;

    using AllocType = std::array<int32_t, 2>;

    std::cout << "size: " << sizeof(AllocType) << std::endl;

    
    std::vector<AllocType*> test_res(100000000);

    std::vector<AllocType*> test_res2(100000000);


    spoo::MemoryPool<AllocType> pool;

    std::chrono::steady_clock::time_point start, finish;
    std::chrono::milliseconds duration;
    std::cout << "spoo::MemoryPool" << std::endl;


    std::cout << "MemoryPool" << std::endl;


    MemoryPool<AllocType> pool2;

    start = std::chrono::steady_clock::now();

    for (int i = 0; i < 100000000; i++) {
        test_res2[i] = pool2.allocate();
        //*test_res2[i] = i;
    }

    finish = std::chrono::steady_clock::now();
    duration = std::chrono::duration_cast<std::chrono::milliseconds>(finish - start);
    std::cout << "allocate: " << duration.count() << "ms" << std::endl;

    start = std::chrono::steady_clock::now();

    for (int i = 0; i < 100000000; i++) {
        pool2.deallocate(test_res2[i]);
    }

    finish = std::chrono::steady_clock::now();
    duration = std::chrono::duration_cast<std::chrono::milliseconds>(finish - start);
    std::cout << "deallocate: " << duration.count() << "ms" << std::endl;



    start = std::chrono::steady_clock::now();

    for (int i = 0; i < 100000000; i++) {
        test_res2[i] = pool2.allocate();
        //*test_res2[i] = i;
    }

    finish = std::chrono::steady_clock::now();
    duration = std::chrono::duration_cast<std::chrono::milliseconds>(finish - start);
    std::cout << "allocate: " << duration.count() << "ms" << std::endl;

    start = std::chrono::steady_clock::now();

    for (int i = 0; i < 100000000; i++) {
        pool2.deallocate(test_res2[i]);
    }

    finish = std::chrono::steady_clock::now();
    duration = std::chrono::duration_cast<std::chrono::milliseconds>(finish - start);
    std::cout << "deallocate: " << duration.count() << "ms" << std::endl;





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

