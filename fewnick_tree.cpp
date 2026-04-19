#include <cassert>
#include <vector>

template <class T> struct fenwick_tree {
  public:
    fenwick_tree() : _n(0) {}
    fenwick_tree(int n) : _n(n), data(n) {}

    void add(int p, T x) {
        assert(0 <= p && p < _n);
        p++;
        while (p <= _n) {
            data[p - 1] += x;
            p += p & -p;
        }
    }

    T sum(int l, int r) {
        assert(0 <= l && l <= r && r <= _n);
        return sum(r) - sum(l);
    }
    
    // a_1 + a_2 + ... + a_x >= w となるような
    //最小の x を求める(ただし a_i >= 0)
    int lower_bound(T w) {
        if (w <= 0) {
            return 0;
        }
        else {
            int x = 0, r = 1;
            while (r < _n) r = r << 1;
            for (int len = r; len > 0; len >>= 1) { // 長さlenは1段下るごとに半分に
                if (x + len < _n && data[x + len] < w) { // 採用するとき
                    w -= data[x + len];
                    x += len;
                }
            }
            return x;
        }
    }

  private:
    int _n;
    std::vector<T> data;

    T sum(int r) {
        T s = 0;
        while (r > 0) {
            s += data[r - 1];
            r -= r & -r;
        }
        return s;
    }
};