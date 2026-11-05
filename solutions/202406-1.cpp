#include <bits/stdc++.h>
using namespace std;
int main() {
    int n,m,p,q;cin >> n >> m >> p >> q;
    for(int k=0,x;k<n*m;++k) {cin >> x;cout << x << ((k+1)%q?' ':'\n');}
}
