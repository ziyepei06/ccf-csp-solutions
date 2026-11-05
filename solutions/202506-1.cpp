#include <bits/stdc++.h>
using namespace std;
int main() {
    int k;cin >> k;
    while(k--) {int u,s,n;cin >> u >> s >> n;int t=100*(n-u)/s;cout << t/10+1 << ' ' << t%10+1 << '\n';}
}
