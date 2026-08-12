#pragma once

#include <cstddef>
#include <cstdint>
#include <iterator>
#include <string_view>
#include <vector>

// Legacy source: hash.c, hash.h (SRS S3.2, sqlite::utils::hash). A generic,
// SQL-unaware, ASCII-case-insensitive string-keyed hash table storing
// non-owning pointers, used throughout the legacy engine (schema symbol
// tables, function/collation registries, etc). Ported with the same
// algorithm (Knuth multiplicative string hash over the 0xdf-masked octets,
// doubly-linked element list for stable iteration order, rehash once
// count>=5 && count>2*bucket_count) rather than replaced with
// std::unordered_map, so callers relying on this exact growth/iteration
// behavior see no change. As in the legacy code, the key is stored
// by reference (std::string_view), not copied: the caller must keep the
// key's backing storage alive for as long as the entry exists.
namespace sqlite::utils::hash {

// hash.c's strHash(): only the low 7 bits (0xdf mask over ASCII) of each
// octet are hashed, which is what makes lookups case-insensitive for ASCII
// letters (the mask clears the one bit that distinguishes upper/lower case).
inline constexpr std::uint32_t string_hash(std::string_view key) noexcept {
    std::uint32_t h = 0;
    for (unsigned char c : key) {
        h += static_cast<std::uint32_t>(0xdfu & c);
        h *= 0x9e3779b1u;
    }
    return h;
}

// hash.c's findElementWithHash() confirms a hash match with
// sqlite3StrICmp(), an ASCII case-insensitive comparison -- string_hash()
// alone only makes same-case-modulo-letter-case keys collide into the same
// bucket, it does not by itself make lookup case-insensitive.
inline bool ascii_case_insensitive_equal(std::string_view a, std::string_view b) noexcept {
    if (a.size() != b.size()) return false;
    for (std::size_t i = 0; i < a.size(); ++i) {
        if ((0xdfu & static_cast<unsigned char>(a[i])) != (0xdfu & static_cast<unsigned char>(b[i]))) {
            return false;
        }
    }
    return true;
}

template<class T>
class hash_table {
public:
    struct element {
        std::string_view key;
        T* data = nullptr;
        std::uint32_t hash = 0;
        element* next = nullptr;
        element* prev = nullptr;
    };

    class iterator {
    public:
        using iterator_category = std::forward_iterator_tag;
        using value_type = element;
        using difference_type = std::ptrdiff_t;
        using pointer = element*;
        using reference = element&;

        iterator() = default;
        explicit iterator(element* e) : current_(e) {}

        reference operator*() const { return *current_; }
        pointer operator->() const { return current_; }
        iterator& operator++() { current_ = current_->next; return *this; }
        iterator operator++(int) { iterator tmp = *this; ++(*this); return tmp; }
        bool operator==(const iterator&) const = default;

    private:
        element* current_ = nullptr;
    };

    hash_table() = default;
    hash_table(const hash_table&) = delete;
    hash_table& operator=(const hash_table&) = delete;
    hash_table(hash_table&&) = default;
    hash_table& operator=(hash_table&&) = default;
    ~hash_table() { clear(); }

    [[nodiscard]] unsigned int count() const noexcept { return count_; }
    [[nodiscard]] bool empty() const noexcept { return count_ == 0; }

    iterator begin() const { return iterator(first_); }
    iterator end() const { return iterator(nullptr); }

    // Returns nullptr if no entry with this key exists.
    [[nodiscard]] T* find(std::string_view key) const {
        return find_element(key).data;
    }

    void clear() {
        element* e = first_;
        first_ = nullptr;
        buckets_.clear();
        while (e != nullptr) {
            element* next = e->next;
            delete e;
            e = next;
        }
        count_ = 0;
    }

    // Mirrors sqlite3HashInsert(): data==nullptr removes the entry (if any)
    // and returns the data that was removed. Otherwise inserts or replaces,
    // returning whatever data the key previously mapped to (or nullptr).
    T* insert(std::string_view key, T* data) {
        std::uint32_t h = string_hash(key);
        found found_entry = find_with_hash(key, h);
        if (found_entry.entry != nullptr) {
            T* old_data = found_entry.entry->data;
            if (data == nullptr) {
                remove(found_entry.entry);
            } else {
                found_entry.entry->data = data;
                found_entry.entry->key = key;
            }
            return old_data;
        }
        if (data == nullptr) return nullptr;

        auto* new_elem = new element{key, data, h, nullptr, nullptr};
        ++count_;
        if (count_ >= 5 && count_ > 2 * buckets_.size()) {
            rehash(count_ * 3);
        }
        link(new_elem, buckets_.empty() ? nullptr : &buckets_[new_elem->hash % buckets_.size()]);
        return nullptr;
    }

private:
    struct bucket {
        unsigned int count = 0;
        element* chain = nullptr;
    };
    struct found {
        element* entry = nullptr;
    };

    found find_with_hash(std::string_view key, std::uint32_t h) const {
        element* elem;
        unsigned int remaining;
        if (!buckets_.empty()) {
            const bucket& b = buckets_[h % buckets_.size()];
            elem = b.chain;
            remaining = b.count;
        } else {
            elem = first_;
            remaining = count_;
        }
        while (remaining > 0) {
            if (elem->hash == h && ascii_case_insensitive_equal(elem->key, key)) return {elem};
            elem = elem->next;
            --remaining;
        }
        return {nullptr};
    }

    element find_element(std::string_view key) const {
        found f = find_with_hash(key, string_hash(key));
        return f.entry != nullptr ? *f.entry : element{};
    }

    void link(element* new_elem, bucket* target_bucket) {
        element* head = nullptr;
        if (target_bucket != nullptr) {
            head = target_bucket->count > 0 ? target_bucket->chain : nullptr;
            ++target_bucket->count;
            target_bucket->chain = new_elem;
        }
        if (head != nullptr) {
            new_elem->next = head;
            new_elem->prev = head->prev;
            if (head->prev != nullptr) head->prev->next = new_elem;
            else first_ = new_elem;
            head->prev = new_elem;
        } else {
            new_elem->next = first_;
            if (first_ != nullptr) first_->prev = new_elem;
            new_elem->prev = nullptr;
            first_ = new_elem;
        }
    }

    void remove(element* elem) {
        if (elem->prev != nullptr) elem->prev->next = elem->next;
        else first_ = elem->next;
        if (elem->next != nullptr) elem->next->prev = elem->prev;

        if (!buckets_.empty()) {
            bucket& b = buckets_[elem->hash % buckets_.size()];
            if (b.chain == elem) b.chain = elem->next;
            --b.count;
        }
        delete elem;
        --count_;
        if (count_ == 0) clear();
    }

    bool rehash(std::size_t new_size) {
        if (new_size == buckets_.size()) return false;
        std::vector<bucket> new_buckets(new_size);
        buckets_.swap(new_buckets);
        element* elem = first_;
        first_ = nullptr;
        while (elem != nullptr) {
            element* next = elem->next;
            elem->next = elem->prev = nullptr;
            link(elem, &buckets_[elem->hash % buckets_.size()]);
            elem = next;
        }
        return true;
    }

    unsigned int count_ = 0;
    element* first_ = nullptr;
    std::vector<bucket> buckets_;
};

} // namespace sqlite::utils::hash
