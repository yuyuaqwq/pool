#ifndef MEMORY_POOL_MEMORY_POOL_HPP_

#include <vector>

namespace spoo {

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
            delete[] block.slot_array;
        }
    }

    CompactMemoryPool(const CompactMemoryPool&) = delete;
    void operator=(const CompactMemoryPool&) = delete;

    SlotPos allocate() {
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

    void deallocate(SlotPos& free_pos) {
        if (free_pos != kSlotPosInvalid) {
            auto [block_id, slot_id] = SplitId(free_pos);
            auto& slot = block_table_[block_id].slot_array[slot_id];
            slot.next = free_slot_;
            free_slot_ = free_pos;
            free_pos = kSlotPosInvalid;
        }
    }

    T* get_ptr(const SlotPos& pos) noexcept {
        auto [block_id, slot_id] = SplitId(pos);
        auto& alloc_slot = block_table_[block_id].slot_array[slot_id];
        return &alloc_slot.element;
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
        auto new_block = new Slot[kBlockSlotCount];
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

template <class T, size_t block_size = 4096>
class MemoryPool {
public:
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
    }
    ~MemoryPool() noexcept {
        for (auto& block : block_table_) {
            delete[] block.slot_array;
        }
    }

    MemoryPool(const MemoryPool&) = delete;
    void operator=(const MemoryPool&) = delete;

    T* allocate() {
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

    void deallocate(T* free_ptr) {
        if (free_ptr != nullptr) {
            auto free_slot = reinterpret_cast<Slot*>(free_ptr);
            free_slot->next = free_slot_;
            free_slot_ = free_slot;
        }
    }

private:
    void CreateBlock() {
        auto new_block_ = new Slot[kBlockSlotCount];
        current_slot_ = &new_block_[0];
        end_slot_ = &new_block_[kBlockSlotCount];
        block_table_.push_back(BlockInfo{ new_block_ });
    }

private:
    Slot* free_slot_;
    Slot* current_slot_;
    Slot* end_slot_;

    std::vector<BlockInfo> block_table_;
};


} // namespace spoo

#endif // MEMORY_POOL_MEMORY_POOL_HPP_