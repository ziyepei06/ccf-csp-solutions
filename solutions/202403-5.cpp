#include <bits/stdc++.h>
using namespace std;
int main(){
    ios::sync_with_stdio(false);cin.tie(nullptr);
    int n,m;cin >> n >> m;vector<list<int>>ch(n+1);vector<long long>d(n+1);
    for(int i=2,p;i<=n;++i){cin >> p;ch[p].push_back(i);}for(int i=1;i<=n;++i)cin >> d[i];
    vector<int>in(n+1),out(n+1),depth(n+1),bit(n+2);int timer=0;
    vector<pair<int,bool>>st{{1,false}};depth[1]=1;
    while(!st.empty()){
        auto [u,exit]=st.back();st.pop_back();if(exit){out[u]=timer;continue;}
        in[u]=++timer;st.push_back({u,true});
        for(auto it=ch[u].rbegin();it!=ch[u].rend();++it){depth[*it]=depth[u]+1;st.push_back({*it,false});}
    }
    auto change=[&](int x,int v){for(;x<=n;x+=x&-x)bit[x]+=v;};
    while(m--){
        int op,x;cin >> op >> x;
        if(op==1){
            list<int>old;old.splice(old.end(),ch[x]);
            for(int y:old){d[x]+=d[y];ch[x].splice(ch[x].end(),ch[y]);change(in[y],-1);change(out[y]+1,1);}
            cout << ch[x].size() << ' ' << d[x] << '\n';
        }else{int ans=depth[x];for(int t=in[x];t;t-=t&-t)ans+=bit[t];cout << ans << '\n';}
    }
}
