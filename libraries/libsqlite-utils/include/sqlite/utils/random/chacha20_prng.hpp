#pragma once

#include <array>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <mutex>
#include <random>

// Legacy source: random.c (SRS S3.2, sqlite::utils::random). Ports the
// RFC-7539 ChaCha20-block-function PRNG verbatim (bit for bit -- same
// algorithm, same 64-byte output block, same "16 bytes of counter, refill
// the block once exhausted" scheme), since this is the actual observable
// contract of sqlite3_randomness(): deterministic given a seed, and callers
// (e.g. random ROWID generation) depend on its output distribution, not on
// any particular entropy source.
//
// What differs from random.c is only the entropy *source* used to seed the
// generator: the legacy code seeds from sqlite3OsRandomness(), the VFS
// layer's OS-randomness hook, which lives in sqlite-backend-os (SRS S3.3.1)
// and is out of scope for this phase. Here, std::random_device seeds the
// generator instead; swapping in a VFS-backed source later is a one-line
// change to seed_from_system_entropy() and does not affect anything
// downstream of it.
namespace sqlite::utils::random {

class chacha20_prng {
public:
    // Fills buf[0, n) with pseudo-random bytes, initializing (seeding) the
    // generator on first use. Thread-safe: legacy random.c serializes all
    // callers through a single static generator instance and a mutex
    // (SQLITE_MUTEX_STATIC_PRNG); this class does the same with std::mutex,
    // since the full pluggable mutex dispatch (sqlite::utils::mutex) is an
    // interface-only stub in this phase (SRS S3.2's mutex note) with its real
    // bodies deferred to sqlite-backend-os.
    void randomness(int n, void* buf) {
        auto* out = static_cast<unsigned char*>(buf);
        std::lock_guard<std::mutex> lock(mutex_);
        if (n <= 0 || buf == nullptr) {
            state_.s[0] = 0;
            return;
        }
        if (state_.s[0] == 0) seed_from_system_entropy();

        while (true) {
            if (n <= state_.n) {
                std::memcpy(out, &state_.out[state_.n - n], static_cast<std::size_t>(n));
                state_.n -= static_cast<std::uint8_t>(n);
                break;
            }
            if (state_.n > 0) {
                std::memcpy(out, state_.out.data(), state_.n);
                n -= state_.n;
                out += state_.n;
            }
            ++state_.s[12];
            chacha_block(reinterpret_cast<std::uint32_t*>(state_.out.data()), state_.s.data());
            state_.n = 64;
        }
    }

    // Testing hooks mirroring sqlite3PrngSaveState/RestoreState: sqlite's own
    // test_control interface uses these to make PRNG-dependent tests
    // deterministic, and this library's own tests (chacha20_prng_test.hpp)
    // rely on them for the same reason.
    struct saved_state {
        std::array<std::uint32_t, 16> s;
    };

    [[nodiscard]] saved_state save_state() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return {state_.s};
    }

    void restore_state(const saved_state& saved) {
        std::lock_guard<std::mutex> lock(mutex_);
        state_.s = saved.s;
        state_.n = 0;
    }

    // Loads a specific 16-word state directly (bypassing system entropy), so
    // tests can drive the generator to a known state and check its output
    // deterministically.
    void reseed(const std::array<std::uint32_t, 16>& raw) {
        std::lock_guard<std::mutex> lock(mutex_);
        state_.s = raw;
        state_.n = 0;
    }

    // The RFC-7539 ChaCha20 block function, exposed so tests can check it
    // against the RFC's published test vector independently of entropy-source
    // seeding (chacha20_prng_test.hpp).
    static void chacha_block(std::uint32_t* out, const std::uint32_t* in) {
        std::uint32_t x[16];
        std::memcpy(x, in, 64);
        auto qr = [](std::uint32_t& a, std::uint32_t& b, std::uint32_t& c, std::uint32_t& d) {
            a += b; d ^= a; d = rotl(d, 16);
            c += d; b ^= c; b = rotl(b, 12);
            a += b; d ^= a; d = rotl(d, 8);
            c += d; b ^= c; b = rotl(b, 7);
        };
        for (int i = 0; i < 10; ++i) {
            qr(x[0], x[4], x[8], x[12]);
            qr(x[1], x[5], x[9], x[13]);
            qr(x[2], x[6], x[10], x[14]);
            qr(x[3], x[7], x[11], x[15]);
            qr(x[0], x[5], x[10], x[15]);
            qr(x[1], x[6], x[11], x[12]);
            qr(x[2], x[7], x[8], x[13]);
            qr(x[3], x[4], x[9], x[14]);
        }
        for (int i = 0; i < 16; ++i) out[i] = x[i] + in[i];
    }

private:
    struct prng_state {
        std::array<std::uint32_t, 16> s{};   // 64 bytes of chacha20 state
        std::array<std::uint8_t, 64> out{};  // Output bytes
        std::uint8_t n = 0;                  // Output bytes remaining
    };

    static constexpr std::uint32_t rotl(std::uint32_t a, int b) noexcept {
        return (a << b) | (a >> (32 - b));
    }

    void seed_from_system_entropy() {
        static constexpr std::uint32_t chacha20_init[4] = {
            0x61707865, 0x3320646e, 0x79622d32, 0x6b206574
        };
        std::memcpy(state_.s.data(), chacha20_init, 16);

        std::random_device entropy;
        for (std::size_t i = 4; i < 15; ++i) state_.s[i] = entropy();
        state_.s[15] = state_.s[12];
        state_.s[12] = 0;
        state_.n = 0;
    }

    mutable std::mutex mutex_;
    prng_state state_;
};

} // namespace sqlite::utils::random
