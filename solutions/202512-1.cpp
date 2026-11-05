#include <bits/stdc++.h>
using namespace std;
int main() {
    int n,m;cin >> n >> m;vector<int>a(n+1),h(m);vector<vector<int>>S(m);
    for(int i=1;i<=n;++i)cin >> a[i];
    for(int i=0;i<m;++i) {int k;cin >> k;S[i].resize(k);for(int &x:S[i]) {cin >> x;h[i]^=a[x];}}
    for(int i=0;i<m;++i) {
        int k,g=0;cin >> k;vector<int>T(k);for(int &x:T) {cin >> x;g^=a[x];}
        cout << (((S[i]==T)==(h[i]==g))?"correct":"wrong") << '\n';
    }
}
