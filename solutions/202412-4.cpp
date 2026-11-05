#include <bits/stdc++.h>
using namespace std;
int main(){
    int n;cin >> n;vector<int>a(n+1),k(n+1),p(n+2),d(n+1,-1);
    for(int i=1;i<=n;++i)cin >> a[i];for(int i=1;i<=n;++i)cin >> k[i];iota(p.begin(),p.end(),0);
    function<int(int)>find=[&](int x){int r=x;while(p[r]!=r)r=p[r];while(p[x]!=x){int y=p[x];p[x]=r;x=y;}return r;};
    queue<int>q;q.push(1);d[1]=0;
    while(!q.empty()){
        int u=q.front();q.pop();int r=min(1LL*n,u+1LL*k[u]);
        for(int j=find(u+1);j<=r;j=find(j)){
            int v=j-a[j];if(d[v]<0){d[v]=d[u]+1;q.push(v);}p[j]=find(j+1);
        }
    }
    cout << d[n] << '\n';
}
