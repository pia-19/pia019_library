/**
author  : pia019
created : 2026/10/17 17:14:17
library : https://github.com/pia-19/pia019_library
**/


#include <bits/stdc++.h>
#include <atcoder/all>
using namespace std;

using mint = atcoder::modint998244353;


struct FPS : vector<mint> {
    using vector<mint>::vector;
    using vector<mint>::operator=;

    FPS(const vector<mint>& v) : vector<mint>(v) {}
    FPS(vector<mint>&& v) : vector<mint>(std::move(v)) {}

    // n 次未満に切り詰める
    FPS pre(int n) const {
        return FPS(begin(), begin() + min<int>(size(), n));
    }

    // +=
    FPS& operator+=(const FPS& r) {
        if (size() < r.size()) resize(r.size());
        for (int i = 0; i < (int)r.size(); i++) {
            (*this)[i] += r[i];
        }
        return *this;
    }

    // -=
    FPS& operator-=(const FPS& r) {
        if (size() < r.size()) resize(r.size());
        for (int i = 0; i < (int)r.size(); i++) {
            (*this)[i] -= r[i];
        }
        return *this;
    }

    // *= scalar
    FPS& operator*=(mint c) {
        for (auto& x : *this) x *= c;
        return *this;
    }

    // /= scalar
    FPS& operator/=(mint c) {return *this *= c.inv();}

    // *= FPS
    FPS& operator*=(const FPS& r) {
        auto v = atcoder::convolution(
            vector<mint>(begin(), end()),
            vector<mint>(r.begin(), r.end())
        );
        assign(v.begin(), v.end());
        return *this;
    }



    // 微分
    FPS diff() const {
        int n = size();
        if (n <= 1) return {};

        FPS res(n - 1);
        for (int i = 1; i < n; i++) res[i - 1] = (*this)[i] * i;
        
        return res;
    }

    // 積分
    FPS integral() const {
        int n = size();

        FPS res(n + 1);
        for (int i = 0; i < n; i++) res[i + 1] = (*this)[i] / (i + 1);
        return res;
    }

    // 逆元
    // F * G = 1 (mod x^n)
    // 前提: F[0] != 0
    FPS inv(int n) const {
        assert(!empty() && (*this)[0] != 0);

        FPS g{(*this)[0].inv()};

        for (int m = 1; m < n; m <<= 1) {
            int len = min(2 * m, n);

            FPS f = pre(len);

            // g <- g(2 - fg)
            FPS fg = f * g;
            fg.resize(len);

            for (auto& x : fg) x = -x;
            fg[0] += 2;

            g *= fg;
            g.resize(len);
        }

        g.resize(n);
        return g;
    }
    
    
    // 末尾の 0 を削除
    void shrink() {
        while (!empty() && back() == 0) pop_back();
    }

    // 係数を逆順にする
    FPS rev() const {
        FPS res = *this;
        reverse(res.begin(), res.end());
        return res;
    }

    // 多項式の商
    // F / G
    // 前提: G != 0
    FPS div(const FPS& g) const {
        assert(!g.empty());

        FPS f = *this;
        FPS h = g;
        f.shrink();
        h.shrink();

        assert(!h.empty());

        if (f.size() < h.size()) return {};

        int n = f.size() - h.size() + 1;

        // 係数を反転すると
        // rev(Q) = rev(F) / rev(G) mod x^n
        FPS q = f.rev().pre(n) * h.rev().inv(n);
        q.resize(n);
        q = q.rev();
        q.shrink();

        return q;
    }

    // 多項式の余り
    // F % G
    FPS mod(const FPS& g) const {
        FPS f = *this;
        FPS h = g;
        f.shrink();
        h.shrink();

        assert(!h.empty());

        if (f.size() < h.size()) return f;

        FPS q = f.div(h);
        FPS r = f - q * h;

        // deg(r) < deg(h)
        r.resize(min<int>(r.size(), h.size() - 1));
        r.shrink();

        return r;
    }
    
    friend FPS operator+(FPS l, const FPS& r) {return l += r;}
    friend FPS operator-(FPS l, const FPS& r) {return l -= r;}
    friend FPS operator*(FPS l, const FPS& r) {return l *= r;}
    friend FPS operator*(FPS l, mint r) {return l *= r;}
    friend FPS operator*(mint l, FPS r) {return r *= l;}
    friend FPS operator/(FPS l, mint r) {return l /= r;}
    friend FPS operator/(const FPS& f, const FPS& g) {return f.div(g);}
    friend FPS operator%(const FPS& f, const FPS& g) {return f.mod(g);}
    

    // log F
    // 前提: F[0] = 1
    FPS log(int n) const {
        assert(!empty() && (*this)[0] == 1);

        FPS res = diff() * inv(n);
        res.resize(max(0, n - 1));

        res = res.integral();
        res.resize(n);

        return res;
    }

    // exp F
    // 前提: F[0] = 0
    FPS exp(int n) const {
        assert(empty() || (*this)[0] == 0);

        FPS g{1};

        for (int m = 1; m < n; m <<= 1) {
            int len = min(2 * m, n);

            FPS f = pre(len);
            f.resize(len);

            FPS lg = g.log(len);

            // g <- g * (1 + f - log(g))
            FPS q = f - lg;
            q.resize(len);
            q[0] += 1;

            g *= q;
            g.resize(len);
        }

        g.resize(n);
        return g;
    }

    // F^k
    // k >= 0
    FPS pow(long long k, int n) const {
        FPS res(n);

        if (n == 0) return res;

        if (k == 0) {
            res[0] = 1;
            return res;
        }

        // 最初の非零項
        int p = 0;
        while (p < (int)size() && (*this)[p] == 0) p++;

        if (p == (int)size()) return res;

        // x^(p*k) 以降しか非零にならない
        if ((__int128)p * k >= n) return res;

        int shift = p * k;
        int len = n - shift;

        mint c = (*this)[p];

        FPS f;
        for (int i = p; i < (int)size() && i - p < len; i++) {
            f.push_back((*this)[i] / c);
        }
        f.resize(len);

        // f[0] = 1 なので
        // F^k = x^(pk) c^k exp(k log f)
        FPS g = f.log(len);
        g *= mint(k);
        g = g.exp(len);
        g *= c.pow(k);

        for (int i = 0; i < len; i++) res[i + shift] = g[i];

        return res;
    }
};