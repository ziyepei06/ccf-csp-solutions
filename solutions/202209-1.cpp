#include <bits/stdc++.h>
using namespace std;
int main() {
    int n; long long m; cin >> n >> m;
    for (int i=0,a;i<n;++i) {
        cin >> a; cout << m%a << (i+1==n?'\n':' '); m/=a;
    }
}
