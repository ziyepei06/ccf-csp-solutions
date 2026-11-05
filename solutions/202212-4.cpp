#include <bits/stdc++.h>
using namespace std;
using ll=long long;
struct Bag {
    map<ll,int>a;ll sum=0;
    ll cost(map<ll,int>::iterator it) {
        if(it==a.end()||it->second>=2||a.size()<2)return 0;
        ll d=2000000000LL;
        if(it!=a.begin())d=min(d,it->first-prev(it)->first);
        auto nx=next(it);if(nx!=a.end())d=min(d,nx->first-it->first);
        return d*d;
    }
    void insert(ll x,int cnt) {
        auto it=a.lower_bound(x);
        if(it!=a.end()&&it->first==x) {sum-=cost(it);it->second+=cnt;sum+=cost(it);return;}
        auto pr=it==a.begin()?a.end():prev(it);
        sum-=cost(pr)+cost(it);auto mid=a.emplace(x,cnt).first;
        sum+=cost(pr)+cost(mid)+cost(it);
    }
};
int main() {
    int n;cin >> n;vector<int>p(n+1);vector<ll>a(n+1),ans(n+1);
    for(int i=2;i<=n;++i)cin >> p[i];for(int i=1;i<=n;++i)cin >> a[i];
    vector<unique_ptr<Bag>>b(n+1);for(int i=1;i<=n;++i)b[i]=make_unique<Bag>();
    for(int i=n;i>=1;--i) {
        b[i]->insert(a[i],1);ans[i]=b[i]->sum;
        if(p[i]) {
            int j=p[i];if(b[i]->a.size()>b[j]->a.size())swap(b[i],b[j]);
            for(auto [x,c]:b[i]->a)b[j]->insert(x,c);b[i].reset();
        }
    }
    for(int i=1;i<=n;++i)cout << ans[i] << '\n';
}
