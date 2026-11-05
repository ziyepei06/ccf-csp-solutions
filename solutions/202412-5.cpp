#include <bits/stdc++.h>
using namespace std;
using ll=long long;
int main(){
    ios::sync_with_stdio(false);cin.tie(nullptr);
    int n;cin >> n;vector<ll>a(n),b(n);for(auto &x:a)cin >> x;for(auto &x:b)cin >> x;
    int q;cin >> q;
    while(q--){
        vector<ll>A=a,B=b;int k;cin >> k;while(k--){int i;ll x,y;cin >> i >> x >> y;A[i-1]=x;B[i-1]=y;}
        vector<int>left(n,-1),right(n,-1),stack;
        for(int i=0;i<n;++i){
            int last=-1;while(!stack.empty()&&A[stack.back()]<A[i]){last=stack.back();stack.pop_back();}
            left[i]=last;if(!stack.empty())right[stack.back()]=i;stack.push_back(i);
        }
        int root=stack[0];vector<int>order{root};
        for(size_t i=0;i<order.size();++i){int u=order[i];if(left[u]>=0)order.push_back(left[u]);if(right[u]>=0)order.push_back(right[u]);}
        vector<ll>sum=B,need(n),val(n);
        for(int i=n-1;i>=0;--i){int u=order[i];for(int v:{left[u],right[u]})if(v>=0)sum[u]+=sum[v];}
        for(int u:order){val[u]=max(need[u],A[u]);for(int v:{left[u],right[u]})if(v>=0)need[v]=max(need[u],A[u]-sum[v]);}
        ll ans=0;for(int i=1;i<n;++i)ans^=val[A[i-1]<A[i]?i-1:i];cout << ans << '\n';
    }
}
