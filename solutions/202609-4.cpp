#include <bits/stdc++.h>
using namespace std;
using ll=long long;using i128=__int128_t;
int main(){
    ios::sync_with_stdio(false);cin.tie(nullptr);
    int n,op;ll k;cin>>n>>k>>op;vector<ll>a(n);for(ll &x:a)cin>>x;sort(a.begin(),a.end());
    vector<array<pair<int,int>,2>> edge(n);
    auto interval=[&](i128 lo,i128 hi){
        if(lo>hi||lo>a.back()||hi<a.front())return make_pair(0,0);
        ll l=(ll)max<i128>(lo,a.front()),r=(ll)min<i128>(hi,a.back());
        return make_pair((int)(lower_bound(a.begin(),a.end(),l)-a.begin()),
                         (int)(upper_bound(a.begin(),a.end(),r)-a.begin()));
    };
    bool hasEdge=false;
    for(int i=0;i<n;i++){
        if(op){
            edge[i][0]=interval(a[i]/(k+1)+1,a[i]/k);
            edge[i][1]=interval((i128)a[i]*k,(i128)a[i]*(k+1)-1);
        }else{
            edge[i][0]=a[i]%k?make_pair(0,0):interval(a[i]/k,a[i]/k);
            edge[i][1]=interval((i128)a[i]*k,(i128)a[i]*k);
        }
        for(auto [l,r]:edge[i])hasEdge|=l<r;
    }
    vector<int> color(n,0);int groups=1;
    if(hasEdge){
        groups=2;vector<int> next(n+1);iota(next.begin(),next.end(),0);
        auto find=[&](int x){
            int r=x;while(next[r]!=r)r=next[r];
            while(next[x]!=x){int y=next[x];next[x]=r;x=y;}return r;
        };
        for(int root=0;root<n;root++)if(next[root]==root){
            next[root]=find(root+1);vector<int> todo{root};
            for(size_t at=0;at<todo.size();at++){
                int u=todo[at];
                for(auto [l,r]:edge[u])for(int v=find(l);v<r;v=find(v)){
                    color[v]=color[u]^1;todo.push_back(v);next[v]=find(v+1);
                }
            }
        }
        vector<int> pref(n+1);
        for(int i=0;i<n;i++)pref[i+1]=pref[i]+color[i];
        bool valid=true;
        for(int i=0;i<n;i++)for(auto [l,r]:edge[i]){
            int ones=pref[r]-pref[l];
            if(color[i]?ones>0:ones<r-l)valid=false;
        }
        if(!valid){
            groups=3;
            for(int i=0;i<n;i++){
                int depth=0;for(ll x=a[i];x>=k;x/=k)++depth;
                color[i]=depth%3;
            }
        }
    }
    cout<<groups<<'\n';
    for(int c=0;c<groups;c++){
        int count=0;for(int x:color)count+=x==c;cout<<count;
        for(int i=0;i<n;i++)if(color[i]==c)cout<<' '<<a[i];cout<<'\n';
    }
}
