#include <bits/stdc++.h>
using namespace std;
using ll=long long;
ll power(ll a,ll e,int p){ll r=1;while(e){if(e&1)r=r*a%p;a=a*a%p;e>>=1;}return r;}
bool prime(int x){for(int d=2;1LL*d*d<=x;++d)if(x%d==0)return false;return true;}
int rankMod(vector<vector<int>>a,int m,int p){
    int r=0,n=a.size();
    for(int c=0;c<m&&r<n;++c){
        int z=r;while(z<n&&!a[z][c])++z;if(z==n)continue;swap(a[z],a[r]);
        ll inv=power(a[r][c],p-2,p);
        for(int j=c;j<m;++j)a[r][j]=a[r][j]*inv%p;
        for(int i=r+1;i<n;++i)if(a[i][c]){
            ll f=a[i][c];for(int j=c;j<m;++j)a[i][j]=(a[i][j]-f*a[r][j]%p+p)%p;
        }++r;
    }return r;
}
int main(){
    vector<int>ps;for(int p=1000000007;ps.size()<12;p-=2)if(prime(p))ps.push_back(p);
    int tests;cin >> tests;
    while(tests--){
        int m;cin >> m;map<string,int>id;vector<vector<int>>a;
        for(int j=0;j<m;++j){
            string s;cin >> s;int p=0;
            while(p<(int)s.size()){
                string name;while(p<(int)s.size()&&isalpha(s[p]))name+=s[p++];
                int v=0;while(p<(int)s.size()&&isdigit(s[p]))v=v*10+s[p++]-'0';
                if(!id.count(name)){id[name]=a.size();a.emplace_back(m);}
                a[id[name]][j]=v;
            }
        }
        bool yes=true;for(int p:ps)if(rankMod(a,m,p)==m){yes=false;break;}
        cout << (yes?'Y':'N') << '\n';
    }
}
