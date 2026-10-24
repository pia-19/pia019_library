#ifndef PIA019_LIBRARY_CONVEX_HULL_TRICK_CPP
#define PIA019_LIBRARY_CONVEX_HULL_TRICK_CPP

#include <cassert>
#include <cstddef>
#include <deque>


struct CHT {
    /*
    - min
    - 直線は傾き単調減少で追加
    */

    struct Linear {
        long long a, b;
        Linear(long long a = 0, long long b = 0) : a(a), b(b) {}
        long long operator()(long long x) const { return a * x + b; }
    };

    std::deque<Linear> ls;

    void add(long long a, long long b) {
        Linear l(a, b);

        assert(ls.empty() || ls.back().a >= l.a);

        // 同じ傾きなら切片が小さい方だけ残す
        if (!ls.empty() && ls.back().a == l.a) {
            if (ls.back().b <= l.b) return;
            ls.pop_back();
        }

        while (ls.size() >= 2) {
            const Linear& l1 = ls[ls.size() - 2];
            const Linear& l2 = ls[ls.size() - 1];

            // x12 >= x23 なら l2 は不要
            using i128 = __int128_t;
            if ((static_cast<i128>(l2.b) - l1.b) *
                    (static_cast<i128>(l2.a) - l.a) >=
                (static_cast<i128>(l.b) - l2.b) *
                    (static_cast<i128>(l1.a) - l2.a)) {
                ls.pop_back();
            }
            else break;
        }

        ls.push_back(l);
    }
    
    // query の x が単調増加の場合
    // long long query(long long x) {
    //     assert(!ls.empty());

    //     while (ls.size() >= 2 && ls[0](x) >= ls[1](x)) ls.pop_front();

    //     return ls[0](x);
    // }
    
    // query の x が一般の場合
    long long query(long long x) const {
        assert(!ls.empty());

        std::size_t l = 0, r = ls.size();
        while (r - l > 1) {
            const std::size_t m = (l + r) / 2;
            if (ls[m - 1](x) >= ls[m](x)) l = m;
            else r = m;
        }
        
        return ls[l](x);
    }
};



#endif // PIA019_LIBRARY_CONVEX_HULL_TRICK_CPP
