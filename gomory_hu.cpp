#ifndef PIA019_LIBRARY_GOMORY_HU_CPP
#define PIA019_LIBRARY_GOMORY_HU_CPP

#include "dinic.cpp"

// 無向グラフの Gomory-Hu 木 (Gusfield 法)。ACL への依存なし。
// 構築: 最大流 n-1 回 (各回グラフを再構築)、親更新 O(n^2)、
//       ダブリング O(n log n)。空間 O(m + n log n)。
// 問い合わせ: O(log n)。木の2頂点間の最小辺重みが最小カット値。
// Cap は非負の容量を扱う整数型とし、各最大流が Cap に収まること。
// Dinic の DFS は再帰なので、非常に深いグラフではスタック制限に注意。
// 使用例 (無向辺は1回だけ追加する):
//   GomoryHu<> g(3);
//   g.add_edge(0, 1, 5);
//   g.add_edge(1, 2, 3);
//   g.build();
//   auto value = g.min_cut_value(0, 2); // 3
//   auto tree = g.edges();             // 木の辺 (from, to, cap)
template <class Cap = long long> struct GomoryHu {
    struct Edge {
        int from, to;
        Cap cap;
    };

    explicit GomoryHu(int n) : _n(n), built(false) {
        static_assert(std::numeric_limits<Cap>::is_integer,
                      "Cap must be an integer type");
        assert(n >= 0);
    }

    // 無向辺を1本追加する。追加後は再度 build() が必要。
    void add_edge(int u, int v, Cap cap) {
        assert(0 <= u && u < _n);
        assert(0 <= v && v < _n);
        assert(cap >= 0);
        input.push_back({u, v, cap});
        built = false;
    }

    // 再実行可能。非連結グラフでは成分間を重み0の木辺で結ぶ。
    // 頂点数0・1の場合、木の辺は空になる。
    void build() {
        built = false;
        tree_edges.clear();
        std::vector<int> parent(_n, 0);
        std::vector<Cap> weight(_n, 0);
        for (int s = 1; s < _n; ++s) {
            int t = parent[s];
            Dinic<Cap> flow(_n);
            for (const auto& e : input) {
                if (e.from == e.to) continue;
                flow.add_edge(e.from, e.to, e.cap);
                flow.add_edge(e.to, e.from, e.cap);
            }
            Cap value = flow.max_flow(s, t);
            const auto side = flow.min_cut(s);
            // 処理済み頂点も付け替える。既存の辺重みは引き継ぐ。
            for (int v = 1; v < _n; ++v) {
                if (v != s && parent[v] == t && side[v]) parent[v] = s;
            }
            if (t != 0 && side[parent[t]]) {
                parent[s] = parent[t];
                parent[t] = s;
                weight[s] = weight[t];
                weight[t] = value;
            } else {
                weight[s] = value;
            }
        }

        std::vector<std::vector<std::pair<int, Cap>>> tree(_n);
        for (int v = 1; v < _n; ++v) {
            tree_edges.push_back({v, parent[v], weight[v]});
            tree[v].emplace_back(parent[v], weight[v]);
            tree[parent[v]].emplace_back(v, weight[v]);
        }
        int height = 1;
        while ((1LL << height) <= _n) ++height;
        up.assign(height, std::vector<int>(_n, 0));
        minimum.assign(height, std::vector<Cap>(_n, std::numeric_limits<Cap>::max()));
        depth.assign(_n, -1);
        if (_n > 0) {
            std::vector<int> order(1, 0);
            depth[0] = 0;
            for (int i = 0; i < static_cast<int>(order.size()); ++i) {
                int v = order[i];
                for (const auto& e : tree[v]) {
                    int to = e.first;
                    if (depth[to] != -1) continue;
                    depth[to] = depth[v] + 1;
                    up[0][to] = v;
                    minimum[0][to] = e.second;
                    order.push_back(to);
                }
            }
        }
        for (int k = 1; k < height; ++k) {
            for (int v = 0; v < _n; ++v) {
                int mid = up[k - 1][v];
                up[k][v] = up[k - 1][mid];
                minimum[k][v] = std::min(minimum[k - 1][v], minimum[k - 1][mid]);
            }
        }
        built = true;
    }

    // build() 後の木の辺を返す (O(n))。
    std::vector<Edge> edges() const {
        assert(built);
        return tree_edges;
    }

    // 異なる2頂点の最小カット値。build() 後に呼ぶこと。
    Cap min_cut_value(int u, int v) const {
        assert(built);
        assert(0 <= u && u < _n);
        assert(0 <= v && v < _n);
        assert(u != v);
        Cap answer = std::numeric_limits<Cap>::max();
        if (depth[u] < depth[v]) std::swap(u, v);
        int difference = depth[u] - depth[v];
        for (int k = 0; k < static_cast<int>(up.size()); ++k) {
            if ((difference >> k) & 1) {
                answer = std::min(answer, minimum[k][u]);
                u = up[k][u];
            }
        }
        if (u == v) return answer;
        for (int k = static_cast<int>(up.size()) - 1; k >= 0; --k) {
            if (up[k][u] == up[k][v]) continue;
            answer = std::min(answer, std::min(minimum[k][u], minimum[k][v]));
            u = up[k][u];
            v = up[k][v];
        }
        return std::min(answer, std::min(minimum[0][u], minimum[0][v]));
    }

  private:
    int _n;
    bool built;
    std::vector<Edge> input, tree_edges;
    std::vector<int> depth;
    std::vector<std::vector<int>> up;
    std::vector<std::vector<Cap>> minimum;
};

#endif // PIA019_LIBRARY_GOMORY_HU_CPP
