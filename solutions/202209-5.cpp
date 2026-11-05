#include <bits/stdc++.h>
using namespace std;
using ll=long long;using i128=__int128_t;
struct Line {ll k,b;ll at(ll x)const{return k*x+b;}};
struct Hull {
    deque<Line>h;
    void add(Line c) {
        while(h.size()>=2) {
            auto a=h[h.size()-2],b=h.back();
            if((i128)(b.b-a.b)*(b.k-c.k)>=(i128)(c.b-b.b)*(a.k-b.k))h.pop_back();else break;
        }
        h.push_back(c);
    }
    ll get(ll x) {
        if(h.empty())return 4000000000000000000LL;
        while(h.size()>1&&h[0].at(x)>=h[1].at(x))h.pop_front();
        return h[0].at(x);
    }
};
int main() {
    int n;ll L;cin >> n >> L;vector<ll>S(n+1);vector<int>m(n+1),xs{0};
    for(int i=1,v;i<=n;++i){cin >> v;S[i]=S[i-1]+v;}
    for(int i=1;i<=n;++i){cin >> m[i];xs.push_back(m[i]);}
    sort(xs.begin(),xs.end());xs.erase(unique(xs.begin(),xs.end()),xs.end());
    vector<Hull>bit(xs.size()+1);
    auto put=[&](int pos,Line a){for(int j=pos;j<(int)bit.size();j+=j&-j)bit[j].add(a);};
    put(1,{0,L*L});ll ans=0;
    for(int i=1;i<=n;++i) {
        int r=lower_bound(xs.begin(),xs.end(),m[i])-xs.begin()+1;
        ll best=4000000000000000000LL;
        for(int j=r-1;j;j-=j&-j)best=min(best,bit[j].get(S[i]));
        ans=best+S[i]*S[i]-2*L*S[i];
        put(r,{-2*S[i],ans+(S[i]+L)*(S[i]+L)});
    }
    cout << ans << '\n';
}
