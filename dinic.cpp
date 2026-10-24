#ifndef PIA019_LIBRARY_DINIC_CPP
#define PIA019_LIBRARY_DINIC_CPP

#include <algorithm>
#include <cassert>
#include <limits>
#include <queue>
#include <utility>
#include <vector>

// Dinic 法: 時間 O(V^2 E)、空間 O(V + E)。ACL への依存なし。
// Cap は非負の容量を扱う整数型とし、最大流の合計が Cap に収まること。
// DFS は再帰なので、非常に深いグラフではスタック制限に注意。
// 使用例:
//   Dinic<> g(3);
//   int id = g.add_edge(0, 1, 5);
//   g.add_edge(1, 2, 3);
//   auto flow = g.max_flow(0, 2); // 3
//   auto edge = g.get_edge(id);  // {0, 1, 5, 3}
//   auto cut = g.min_cut(0);     // {true, true, false}
template <class Cap = long long> struct Dinic {
    struct Edge {
        int from, to;
        Cap cap, flow;
    };

    explicit Dinic(int n) : _n(n) {
        static_assert(std::numeric_limits<Cap>::is_integer,
                      "Cap must be an integer type");
        assert(n >= 0);
        graph.resize(n);
        level.resize(n);
        iter.resize(n);
    }

    // 有向辺を追加し、追加順の ID を返す。容量 0、自己ループ、多重辺も可。
    int add_edge(int from, int to, Cap cap) {
        assert(0 <= from && from < _n);
        assert(0 <= to && to < _n);
        assert(cap >= 0);
        int id = static_cast<int>(pos.size());
        int from_index = static_cast<int>(graph[from].size());
        int to_index = static_cast<int>(graph[to].size());
        pos.emplace_back(from, from_index);
        if (from == to) ++to_index;
        graph[from].push_back({to, to_index, cap});
        graph[to].push_back({from, from_index, 0});
        return id;
    }

    // 残余グラフを更新し、今回追加された流量を返す。
    // 再呼び出しは同じ s, t で行うこと。途中で辺を追加してもよい。
    Cap max_flow(int s, int t) {
        assert(0 <= s && s < _n);
        assert(0 <= t && t < _n);
        assert(s != t);
        Cap flow = 0;
        while (bfs(s, t)) {
            std::fill(iter.begin(), iter.end(), 0);
            while (true) {
                Cap pushed = dfs(s, t, std::numeric_limits<Cap>::max());
                if (pushed == 0) break;
                flow += pushed;
            }
        }
        return flow;
    }

    // 元の辺の容量と現在の流量を返す。内部の逆辺は公開しない。
    Edge get_edge(int id) const {
        assert(0 <= id && id < static_cast<int>(pos.size()));
        const auto& p = pos[id];
        const auto& e = graph[p.first][p.second];
        const auto& reverse = graph[e.to][e.rev];
        return {p.first, e.to, e.cap + reverse.cap, reverse.cap};
    }

    std::vector<Edge> edges() const {
        std::vector<Edge> result;
        result.reserve(pos.size());
        for (int id = 0; id < static_cast<int>(pos.size()); ++id) {
            result.push_back(get_edge(id));
        }
        return result;
    }

    // 残余グラフで s から到達できる頂点を返す (O(V + E))。
    // 最小カットとして使う場合は、max_flow(s, t) 完了後に呼ぶこと。
    std::vector<bool> min_cut(int s) const {
        assert(0 <= s && s < _n);
        std::vector<bool> reachable(_n, false);
        std::queue<int> q;
        reachable[s] = true;
        q.push(s);
        while (!q.empty()) {
            int v = q.front();
            q.pop();
            for (const auto& e : graph[v]) {
                if (e.cap == 0 || reachable[e.to]) continue;
                reachable[e.to] = true;
                q.push(e.to);
            }
        }
        return reachable;
    }

  private:
    struct ResidualEdge {
        int to, rev;
        Cap cap;
    };

    int _n;
    std::vector<std::vector<ResidualEdge>> graph;
    std::vector<std::pair<int, int>> pos;
    std::vector<int> level, iter;

    bool bfs(int s, int t) {
        std::fill(level.begin(), level.end(), -1);
        std::queue<int> q;
        level[s] = 0;
        q.push(s);
        while (!q.empty()) {
            int v = q.front();
            q.pop();
            for (const auto& e : graph[v]) {
                if (e.cap == 0 || level[e.to] != -1) continue;
                level[e.to] = level[v] + 1;
                q.push(e.to);
            }
        }
        return level[t] != -1;
    }

    Cap dfs(int v, int t, Cap limit) {
        if (v == t) return limit;
        for (int& i = iter[v]; i < static_cast<int>(graph[v].size()); ++i) {
            auto& e = graph[v][i];
            if (e.cap == 0 || level[e.to] != level[v] + 1) continue;
            Cap pushed = dfs(e.to, t, std::min(limit, e.cap));
            if (pushed == 0) continue;
            e.cap -= pushed;
            graph[e.to][e.rev].cap += pushed;
            return pushed;
        }
        return 0;
    }
};

#endif // PIA019_LIBRARY_DINIC_CPP
