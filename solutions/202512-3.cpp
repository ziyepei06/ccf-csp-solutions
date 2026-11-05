#include <bits/stdc++.h>
using namespace std;
int main(){
    int Z;cin >> Z;vector<string>a(Z);for(auto &r:a)cin >> r;int k;cin >> k;vector<int>K(k);for(int &x:K)cin >> x;
    int orient=0;
    auto coord=[&](int x,int y){for(int j=0;j<orient;++j){int old=x;x=Z-1-y;y=old;}return pair<int,int>{x,y};};
    auto get=[&](int x,int y){auto [i,j]=coord(x,y);return a[i][j];};
    auto set=[&](int x,int y,char c){auto [i,j]=coord(x,y);a[i][j]=c;};
    for(int i=K[0]-1;i>=0;--i){
        int p=1+6*i,op=K[p];
        if(op==1){
            int u=K[p+1]-1,v=K[p+2]-1,L=K[p+3],turn=K[p+4]/90,r=K[p+5];
            orient=(orient+r)%4;vector<string>b(L,string(L,' '));for(int x=0;x<L;++x)for(int y=0;y<L;++y)b[x][y]=get(u+x,v+y);
            for(int x=0;x<L;++x)for(int y=0;y<L;++y){int X=x,Y=y;for(int t=0;t<turn;++t){int old=X;X=L-1-Y;Y=old;}set(u+X,v+Y,b[x][y]);}
        }else{
            int u=K[p+1]-1,d=K[p+2]-1,l=K[p+3]-1,r=K[p+4]-1,o=K[p+5];
            vector<string>b(d-u+1,string(r-l+1,' '));for(int x=u;x<=d;++x)for(int y=l;y<=r;++y)b[x-u][y-l]=get(x,y);
            for(int x=u;x<=d;++x)for(int y=l;y<=r;++y)set(x,y,b[o==1?d-x:x-u][o==-1?r-y:y-l]);
        }
    }
    int n=0,m=0;for(int x=0;x<Z;++x)for(int y=0;y<Z;++y)if(get(x,y)!='?'){n=max(n,x+1);m=max(m,y+1);}
    cout << n << ' ' << m << '\n';for(int x=0;x<n;++x){for(int y=0;y<m;++y)cout << get(x,y);cout << '\n';}
}
