#include<bits/stdc++.h>
using namespace std;

/*
beet-aizuさんの参考
size(sub)-部分木の大きさのこと
depth-深さ
vid-列にしたときに何番目になるかのid
inv-列の前からi番目はどれか
nxt-分解の先頭はどれか
par-親のidx
*/

struct HLD{
  private:
    //部分木のサイズを求める。大きいものを先頭に持ってくる
    void dfs_sz(int v){
        size[v] = 1;
        for (auto &nv : G[v]){
            if (nv == par[v]){
                continue;//?
            }
            par[nv] = v;
            depth[nv] = depth[v] + 1;
            dfs_sz(nv);
            size[v] += size[nv];
            if (size[nv] > size[G[v][0]]) swap(nv, G[v][0]); //先頭にサイズが大きいものを持ってくる
        }
    }
    
    //実際に分解する
    void dfs_hld(int v, int &idx){
        vid[v] = idx++;
        inv[vid[v]] = v;
        for (auto nv : G[v]){
            if (nv == par[v]) continue;
            nxt[nv] = (nv == G[v][0] ? nxt[v] : nv); //一番サイズが大きいものでないなら切る
            dfs_hld(nv, idx);
        }
    }
    
  public:
    vector<vector<int>> G;
    vector<int> size, depth, nxt, vid, inv, par;
    HLD(int n) : G(n), size(n, -1), depth(n), nxt(n), vid(n), inv(n), par(n, -1){}
    void add_edge(int u, int v){
        G[u].push_back(v);
        G[v].push_back(u);
    }
    
    //r(root)
    void build(int r = 0){
        int idx = 0;
        dfs_sz(r);
        nxt[r] = r;
        dfs_hld(r, idx);
    }
    
    int lca(int u, int v){
        while (true){
            if (vid[u] > vid[v]) swap(u, v);
            if (nxt[u] == nxt[v]) return u;
            v = par[nxt[v]];
        }
    }
    
    template<typename F>
    void for_each(int u, int v, const F& f){
        while(1){
            if (vid[u] > vid[v]) swap(u, v);
            f(max(vid[nxt[v]], vid[u]), vid[v] + 1);
            if (nxt[u] != nxt[v]) v = par[nxt[v]];
            else break;
        }
    }
    
    template<typename F>
    void for_each_edge(int u, int v, const F& f){
        while (1){
            if (vid[u] > vid[v]) swap(u, v);
            if (nxt[u] != nxt[v]){
                f(vid[nxt[v]], vid[v] + 1);
                v = par[nxt[v]];
            }
            else{
                if (u != v) f(vid[u] + 1, vid[v] + 1);
                break;
            }
        }
    }
};
