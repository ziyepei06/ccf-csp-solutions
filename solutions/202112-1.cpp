#include <bits/stdc++.h>
using namespace std;
int main() {
    int n; long long N; cin >> n >> N;
    vector<long long> a(n+2); a[n+1]=N;
    for (int i=1;i<=n;++i) cin >> a[i];
    long long ans=0;
    for (int i=0;i<=n;++i) ans+=i*(a[i+1]-a[i]);
    cout << ans << '\n';
}
