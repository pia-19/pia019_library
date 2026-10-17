#include <bits/stdc++.h>
#include <atcoder/all>
using namespace std;
typedef long long ll;


vector<pair<ll, ll>> get_convex_hull(vector<pair<ll, ll>>& points){
    sort(points.begin(), points.end());
    points.erase(unique(begin(points), end(points)), end(points));
    int n = points.size();
    int k = 0; //size of convexfull
    if (n <= 2) return points;
    vector<pair<ll, ll>> ch;
    auto det = [&](pair<ll, ll> p, pair<ll, ll> q){
        return p.first * q.second - q.first * p.second;
    };
    
    auto sub = [&](pair<ll, ll> p, pair<ll, ll> q){
        return make_pair(p.first - q.first, p.second - q.second);
    };
    
    //反時計回り
    for (int i = 0; i < n; i++){
        while (k > 1){
            pair<ll, ll> v_cur = sub(ch[k - 1], ch[k - 2]);
            pair<ll, ll> v_new = sub(points[i], ch[k - 2]);
            if (det(v_cur, v_new) > 0) break;
            k--;
            ch.pop_back();
        }
        ch.push_back(points[i]);
        k++;
    }
    
    int t = ch.size();
    for (int i = n - 2; i >= 0; i--){
        while (k > t){
            pair<ll, ll> v_cur = sub(ch[k - 1], ch[k - 2]);
            pair<ll, ll> v_new = sub(points[i], ch[k - 2]);
            if (det(v_cur, v_new) > 0) break;
            k--;
            ch.pop_back();
        }
        ch.push_back(points[i]);
        k++;
    }
    ch.resize(k - 1);
    return ch;
}