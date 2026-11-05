#include <bits/stdc++.h>
using namespace std;
int main() {
    int n,d;cin >> n >> d;
    vector<vector<long long>> Q(n,vector<long long>(d)),K=Q,V=Q;
    for(auto *M:{&Q,&K,&V})for(auto &row:*M)for(auto &x:row)cin >> x;
    vector<long long> W(n);for(auto &x:W)cin >> x;
    vector<vector<long long>> B(d,vector<long long>(d));
    for(int t=0;t<n;++t)for(int i=0;i<d;++i)for(int j=0;j<d;++j)B[i][j]+=K[t][i]*V[t][j];
    for(int i=0;i<n;++i)for(int j=0;j<d;++j) {
        long long x=0;for(int t=0;t<d;++t)x+=Q[i][t]*B[t][j];
        cout << x*W[i] << (j+1==d?'\n':' ');
    }
}
