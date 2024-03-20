## pool

快速的池组件库


## memory_pool
- 常规内存池实现
- 不可直接用于STL容器

## compact_memory_pool
- 指针压缩内存池，实际返回uint32_t索引
	- 需要另行调用方法转换为指针
- 用于需要压缩指针占用空间的场景
- 如rbtree
	- 原始指针：8*3+1≈32
	- 压缩后：4*3+1≈16
- 不可直接用于STL容器

## static_memory_pool
- 可直接用于STL容器的内存池

## Performance
### Setup
```
System: Windows11 x64
CPU:11th Gen Intel(R) Core(TM) i5-11260H @ 2.60GHz   2.61 GHz
RAM:32GB
Compiler:MSVC 1939, Release, /O2
```

## Run

```
count:100000000
bytes:32
block_bytes:4096

pool::StaticMemoryPool
allocate: 1234ms
deallocate: 407ms
allocate: 511ms
deallocate: 347ms

std
new: 4197ms
delete: 4575ms
new: 5196ms
delete: 5255ms
```

```
count:100000000
bytes:32
block_bytes:32767

pool::StaticMemoryPool
allocate: 548ms
deallocate: 975ms
allocate: 433ms
deallocate: 348ms

std
new: 4041ms
delete: 4217ms
new: 4672ms
delete: 4828ms
```

## Reference

-   [MemoryPool](https://github.com/cacay/MemoryPool)
    -   相当精巧的内存池，参考了其无需初始化block的设计。

