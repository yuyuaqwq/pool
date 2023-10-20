#ifndef FPOO_MEMORY_POOL_HPP_
#define FPOO_MEMORY_POOL_HPP_

#include <vector>

namespace fpoo {

namespace detail {
consteval size_t bit_shift(size_t base) {
    size_t index = 0;
    while (base >>= 1) {
        ++index;
    }
    return index;
}

consteval size_t bit_fill(size_t bit_count) {
    size_t val = 0;
    for (size_t i = 0; i < bit_count; i++) {
        val <<= 1;
        val |= 1;
    }
    return val;
}
}

template <class T, size_t block_size = 4096>
class CompactMemoryPool {
public:
    using value_type = T;
    using size_type = uint32_t;
    using difference_type = uint32_t;
    using propagate_on_container_move_assignment = std::true_type;

    template <typename U> struct rebind {
        using other = CompactMemoryPool<U>;
    };

private:
    using SlotPos = uint32_t;
    using BlockId = uint32_t;
    using SlotId = uint32_t;

	union Slot {
        SlotPos next;
        T element;
    };
    struct BlockInfo {
        Slot* slot_array;
    };

    static constexpr size_t kBlockSlotCount = block_size / sizeof(Slot);
    static constexpr size_t kBlockShift = detail::bit_shift(kBlockSlotCount);
    static constexpr size_t kSlotMark = kBlockSlotCount - 1;
    static constexpr SlotPos kSlotPosInvalid = detail::bit_fill(sizeof(SlotPos));
  
public:
    CompactMemoryPool() noexcept {
        free_slot_ = kSlotPosInvalid;
        current_slot_ = kSlotPosInvalid;
        end_slot_ = kSlotPosInvalid;
    }
    ~CompactMemoryPool() noexcept {
        for (auto& block : block_table_) {
            operator delete(reinterpret_cast<void*>(block.slot_array));
        }
    }

    CompactMemoryPool(const CompactMemoryPool&) = delete;
    void operator=(const CompactMemoryPool&) = delete;

    [[nodiscard]] SlotPos allocate(std::size_t n = 1) {
        if (n > 1) {
            throw std::runtime_error("memory pool does not support allocating multiple elements.");
        }
        SlotPos res{};
        if (free_slot_ != kSlotPosInvalid) {
            res = free_slot_;
            auto [block_id, slot_id] = SplitId(res);
            auto& alloc_slot = block_table_[block_id].slot_array[slot_id];
            free_slot_ = alloc_slot.next;
            return res;
        }
        else {
            if (current_slot_ >= end_slot_) {
                CreateBlock();
            }
            return current_slot_++;
        }
    }

    void deallocate(SlotPos free_pos, std::size_t n = 1) {
        if (free_pos != kSlotPosInvalid) {
            auto [block_id, slot_id] = SplitId(free_pos);
            auto& slot = block_table_[block_id].slot_array[slot_id];
            slot.next = free_slot_;
            free_slot_ = free_pos;
        }
    }

    T* reference(SlotPos pos) {
        auto [block_id, slot_id] = SplitId(pos);
        auto& slot = block_table_[block_id].slot_array[slot_id];
        return &slot.element;
    }

    void dereference(T*) {

    }

private:
    std::tuple<BlockId, SlotId> SplitId(const SlotPos& slot_pos) const noexcept {
        BlockId block_id = slot_pos / kBlockSlotCount;
        SlotId slot_id = slot_pos % kBlockSlotCount;
        //BlockId block_id = slot_pos >> kBlockShift;
        //SlotId slot_id = slot_pos & kSlotMark;
        return std::tuple{ block_id, slot_id };
    }

    void CreateBlock() {
        auto new_block = reinterpret_cast<Slot*>(operator new(kBlockSlotCount * sizeof(Slot)));
        auto slot_pos = block_table_.size() * kBlockSlotCount;
        block_table_.push_back(BlockInfo{ new_block });

        current_slot_ = slot_pos;
        end_slot_ = slot_pos + kBlockSlotCount;
    }

private:
    std::vector<BlockInfo> block_table_;

    SlotPos free_slot_;

    SlotPos current_slot_;
    SlotPos end_slot_;
};

template< class T1, class T2 >
constexpr bool operator==(const CompactMemoryPool<T1>& lhs, const CompactMemoryPool<T2>& rhs) noexcept {
    return true;
}

template <class T, size_t block_size = 4096>
class MemoryPool {
public:
    static_assert(!std::is_const_v<T>, "The C++ Standard forbids containers of const elements "
        "because allocator<const T> is ill-formed.");

    using value_type = T;
    using size_type = size_t;
    using difference_type = ptrdiff_t;
    using propagate_on_container_move_assignment = std::true_type;
    using is_always_equal = std::true_type;

    template <typename U> struct rebind {
        using other =  MemoryPool<U>;
    };

private:
	union Slot {
        Slot* next;
        T element;
    };

    struct BlockInfo {
        Slot* slot_array;
    };

    static constexpr size_t kBlockSlotCount = block_size / sizeof(Slot);

public:
    MemoryPool() noexcept {
        free_slot_ = nullptr;
        current_slot_ = nullptr;
        end_slot_ = nullptr;

        current_block_ = nullptr;
    }
    ~MemoryPool() noexcept {
        if constexpr (sizeof(Slot) == sizeof(Slot*)) {
            while (current_block_) {
                auto next = current_block_->next;
                operator delete(reinterpret_cast<void*>(current_block_));
                current_block_ = next;
            }
        }
        else {
            for (auto& block : block_table_) {
                operator delete(reinterpret_cast<void*>(block.slot_array));
            }
        }
    }

    MemoryPool(MemoryPool&& other) noexcept {
        operator=(std::move(other));
    }
    void operator=(MemoryPool&& other) noexcept {
        block_table_ = std::move(other.block_table_);
        current_block_ = other.current_block_;
        free_slot_ = other.free_slot_;
        current_slot_ = other.current_slot_;
        end_slot_ = other.end_slot_;
        other.current_block_ = nullptr;
        other.free_slot_ = nullptr;
        other.current_slot_ = nullptr;
        other.end_slot_ = nullptr;
    }

    MemoryPool(const MemoryPool&) = delete;
    void operator=(const MemoryPool&) = delete;

    [[nodiscard]] T* allocate(std::size_t n = 1) {
        if (n > 1) {
            throw std::runtime_error("memory pool does not support allocating multiple elements.");
        }
        if (free_slot_ != nullptr) {
            auto res = free_slot_;
            free_slot_ = res->next;
            return &res->element;
        }
        else {
            if (current_slot_ >= end_slot_) {
                CreateBlock();
            }
            return &(current_slot_++)->element;
        }
    }

    void deallocate(T* free_ptr, std::size_t n = 1) {
        if (free_ptr != nullptr) {
            auto free_slot = reinterpret_cast<Slot*>(free_ptr);
            free_slot->next = free_slot_;
            free_slot_ = free_slot;
        }
    }

private:
    inline void CreateBlock() {
        auto new_block = reinterpret_cast<Slot*>(operator new(kBlockSlotCount * sizeof(Slot)));
        if constexpr (sizeof(Slot) == sizeof(Slot*)) {
            new_block[0].next = current_block_;
            current_block_ = new_block;

            current_slot_ = &new_block[1];
        }
        else {
            block_table_.push_back(BlockInfo{ new_block });

            current_slot_ = &new_block[0];
        }
        end_slot_ = &new_block[kBlockSlotCount];
    }

private:
    Slot* current_block_;
    std::vector<BlockInfo> block_table_;

    Slot* free_slot_;
    Slot* current_slot_;
    Slot* end_slot_;
};

template< class T1, class T2 >
constexpr bool operator==(const MemoryPool<T1>& lhs, const MemoryPool<T2>& rhs) noexcept {
    return true;
}

}  // namespace fpoo

#endif  // FPOO_MEMORY_POOL_HPP_