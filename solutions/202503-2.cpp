#include <bits/stdc++.h>
using namespace std;
int main() {
    int n,m;cin >> n >> m;vector<long long>a(m+1),dp(n+1);
    for(int i=1;i<=m;++i)cin >> a[i];
    for(int s=1;s<=n;++s)for(int i=1;i<=min(s,m);++i)dp[s]=max(dp[s],dp[s-i]+a[i]);
    cout << dp[n] << '\n';
}
