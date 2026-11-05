#include <bits/stdc++.h>
using namespace std;
long long r;
long long F(long long t) {
    long long q=t/r, s=t%r;
    return r*q*(q-1)/2+q*s;
}
int main() {
    int n; long long N; cin >> n >> N; r=N/(n+1);
    vector<long long> a(n+2); a[n+1]=N;
    for (int i=1;i<=n;++i) cin >> a[i];
    long long ans=0;
    for (long long i=0;i<=n;++i) {
        long long l=a[i], h=a[i+1], t=clamp(i*r,l,h);
        ans+=i*(t-l)-(F(t)-F(l));
        ans+=F(h)-F(t)-i*(h-t);
    }
    cout << ans << '\n';
}
