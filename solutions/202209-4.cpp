#include <bits/stdc++.h>
using namespace std;
struct DSU {
    vector<int> p,sz,label;
    int make(int x) {int i=p.size();p.push_back(i);sz.push_back(1);label.push_back(x);return i;}
    int find(int x) {return p[x]==x?x:p[x]=find(p[x]);}
    int join(int a,int b,int x) {
        a=find(a);b=find(b);if(sz[a]<sz[b])swap(a,b);
        if(a!=b){p[b]=a;sz[a]+=sz[b];}label[a]=x;return a;
    }
};
int main() {
    int n,m,q;cin >> n >> m >> q;DSU d;vector<int>root(m+1);vector<long long>cnt(m+1);
    set<pair<long long,int>>rank;
    for(int i=0;i<=m;++i){root[i]=d.make(i);if(i)rank.insert({0,i});}cnt[0]=n;
    map<int,pair<int,int>> seg;seg[1]={n,root[0]};
    auto split=[&](int x) {
        auto it=seg.lower_bound(x);if(it!=seg.end()&&it->first==x)return it;
        auto old=prev(it);int r=old->second.first,c=old->second.second;
        old->second.first=x-1;return seg.emplace(x,make_pair(r,c)).first;
    };
    auto change=[&](int x,long long delta) {
        if(x)rank.erase({-cnt[x],x});cnt[x]+=delta;if(x)rank.insert({-cnt[x],x});
    };
    while(q--) {
        int op;cin >> op;
        if(op==1) {
            int l,r,x;cin >> l >> r >> x;
            auto R=r==n?seg.end():split(r+1),L=split(l);
            for(auto it=L;it!=R;++it) {
                int c=d.label[d.find(it->second.second)];change(c,-(it->second.first-it->first+1LL));
            }
            seg.erase(L,R);seg[l]={r,root[x]};change(x,r-l+1LL);
        } else if(op==2) {
            int x,w;cin >> x >> w;if(x==w)continue;
            change(w,cnt[x]);change(x,-cnt[x]);root[w]=d.join(root[x],root[w],w);root[x]=d.make(x);
        } else if(op==3) {
            int x,y;cin >> x >> y;if(x==y)continue;
            long long a=cnt[x],b=cnt[y];change(x,b-a);change(y,a-b);
            swap(root[x],root[y]);d.label[root[x]]=x;d.label[root[y]]=y;
        } else if(op==4) {int x;cin >> x;cout << cnt[x] << '\n';}
        else cout << (rank.begin()->first==0?0:rank.begin()->second) << '\n';
    }
}
