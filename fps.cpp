#include<bits/stdc++.h>
#include<complex>
using namespace std;
/*
一旦再帰関数を書いて理解する

*/

complex<double> w(double k, double n, bool inv){
    //2πk/nの複素数
    double arg = 2.0 * M_PI * k / n;
    if (inv) return {cos(arg), -sin(arg)};
    else return {cos(arg), sin(arg)};
     
}


//DFTの計算
void DFT(vector<complex<double>> &q, int n, bool inv = 0){
    if (n == 1) return;
    
    //偶数番目と奇数番目に分けて計算
    vector<complex<double>> even(n / 2), odd(n / 2);
	for (int i = 0;i < n / 2;i++) {
		//E[i]は説明のX'に対応
		even[i] = q[i] + q[i + n / 2];
  
		//O[i]は説明のX''に対応
		//W(i, N, inv) は、回転子のi乗
		//inv=0ではDFT、inv=1ではIDFT
		odd[i] = (q[i] - q[i + n / 2]) * w(i, n, inv);
	}
 
	//偶数行目と奇数行目のそれぞれについて、サイズが半分のFFTを計算
	//E,OにDFTの結果(説明のY',Y''に対応)がそのまま入る
	DFT(even, n / 2, inv);
	DFT(odd, n / 2, inv);

	for (int i = 0;i < n / 2;i++) {
		//偶数行目と奇数行目にそれぞれの結果を入れていく
		q[i * 2] = even[i];
		q[i * 2 + 1] = odd[i];
	}
    return;
}

void IDFT(vector<complex<double>>& q, int n){
    DFT(q, n, 1);
    for (int i = 0; i < n; i++) q[i] /= double(n);
}



int main(){
    vector<complex<double>> a = {1, 2, 3};
    vector<complex<double>> b = {2, 3, 4};
    int n = 8;
    a.resize(n);
    b.resize(n);
    vector<complex<double>> c(n);
    DFT(a, n, 0);
    DFT(b, n, 0);
    for (int i = 0; i < n; i++) c[i] = a[i] * b[i];
    IDFT(c, n);
    for (int i = 0; i < n; i++) cout << c[i] << (i == n - 1 ? '\n' : ' ');
    
    
    return 0;
}