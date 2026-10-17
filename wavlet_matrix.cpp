#include <bits/stdc++.h>
using namespace std;
typedef long long ll;

template <class T>
struct WaveletMatrix {
    int n, lg;
    vector<T> xs;                  // 座標圧縮の復元用
    vector<vector<int>> pref;      // pref[h][i] = [0,i) の 1 の個数
    vector<int> mid;               // 0 側の要素数

    WaveletMatrix(const vector<T>& a) {
        n = (int)a.size();

        // 座標圧縮
        xs = a;
        sort(xs.begin(), xs.end());
        xs.erase(unique(xs.begin(), xs.end()), xs.end());

        vector<int> cur(n);
        for (int i = 0; i < n; i++) {
            cur[i] = lower_bound(xs.begin(), xs.end(), a[i]) - xs.begin();
        }

        int m = xs.size();
        lg = 0;
        while ((1LL << lg) < max(1, m)) lg++;
        lg = max(lg, 1);

        pref.assign(lg, vector<int>(n + 1));
        mid.resize(lg);

        // 上位 bit から処理
        for (int h = lg - 1; h >= 0; h--) {
            vector<int> zero, one;
            zero.reserve(n);
            one.reserve(n);

            for (int i = 0; i < n; i++) {
                int bit = (cur[i] >> h) & 1;
                pref[h][i + 1] = pref[h][i] + bit;

                if (bit) one.push_back(cur[i]);
                else     zero.push_back(cur[i]);
            }

            mid[h] = zero.size();

            int p = 0;
            for (int x : zero) cur[p++] = x;
            for (int x : one)  cur[p++] = x;
        }
    }

private:
    // 圧縮後の値について kth smallest
    int kth_id(int l, int r, int k) const {
        assert(0 <= l && l < r && r <= n);
        assert(0 <= k && k < r - l);

        int res = 0;

        for (int h = lg - 1; h >= 0; h--) {
            int l1 = pref[h][l];
            int r1 = pref[h][r];

            int l0 = l - l1;
            int r0 = r - r1;

            int cnt0 = r0 - l0;

            if (k < cnt0) {
                l = l0;
                r = r0;
            } else {
                k -= cnt0;
                res |= 1 << h;

                l = mid[h] + l1;
                r = mid[h] + r1;
            }
        }

        return res;
    }

    // 圧縮後の値 id 未満の要素数
    int freq_id(int l, int r, int id) const {
        if (id <= 0) return 0;
        if (id >= (int)xs.size()) return r - l;

        int res = 0;

        for (int h = lg - 1; h >= 0; h--) {
            int l1 = pref[h][l];
            int r1 = pref[h][r];

            int l0 = l - l1;
            int r0 = r - r1;

            if ((id >> h) & 1) {
                // この bit が 0 のものは全部 id 未満
                res += r0 - l0;

                l = mid[h] + l1;
                r = mid[h] + r1;
            } else {
                l = l0;
                r = r0;
            }
        }

        return res;
    }

public:
    // A[l,r) の k 番目に小さい値 (0-indexed)
    T kth_smallest(int l, int r, int k) const {return xs[kth_id(l, r, k)];}

    // A[l,r) に x 未満の値が何個あるか
    int range_freq(int l, int r, T x) const {
        assert(0 <= l && l <= r && r <= n);
        int id = lower_bound(xs.begin(), xs.end(), x) - xs.begin();
        return freq_id(l, r, id);
    }

    // A[l,r) に [lower, upper) の値が何個あるか
    int range_freq(int l, int r, T lower, T upper) const {
        assert(0 <= l && l <= r && r <= n);
        return range_freq(l, r, upper) - range_freq(l, r, lower);
    }
};




int main(){
    ios::sync_with_stdio(false);
    cin.tie(nullptr);
    cout << fixed << setprecision(15);
    
    
    int n, q; cin >> n >> q;
    vector<int> a(n);
    for (int i = 0; i < n; i++) cin >> a[i];
    
    WaveletMatrix<int> WM(a);
    while (q--){
        int l, r, k; cin >> l >> r >> k;
        cout << WM.kth_smallest(l, r, k) << '\n';
    }
    
    return 0;
}