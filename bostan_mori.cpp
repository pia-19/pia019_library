#include <bits/stdc++.h>
#include <atcoder/all>
using namespace std;

using mint = atcoder::modint998244353;

mint bostan_mori(vector<mint> P, vector<mint> Q, long long n){
    while (n > 0){
        vector<mint> Q_neg(Q.size());
        for (int i = 0; i < (int)Q.size(); i++) Q_neg[i] = (i % 2) ? -Q[i] : Q[i];
        
        vector<mint> U = atcoder::convolution(P, Q_neg);
        vector<mint> V = atcoder::convolution(Q, Q_neg);
        
        P.clear();
        Q.clear();
        
        for (int i = n & 1; i < (int)U.size(); i += 2) P.push_back(U[i]);
        for (int i = 0; i < (int)V.size(); i += 2) Q.push_back(V[i]);
        
        n >>= 1;
    }
    
    return P[0] / Q[0];
}