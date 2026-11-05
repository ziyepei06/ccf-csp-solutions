#include <bits/stdc++.h>
using namespace std;
using ll=long long;const int P=998244353,S=10;
using Mat=array<array<int,S>,S>;using Small=array<array<int,3>,3>;
Mat mul(const Mat&a,const Mat&b){Mat c{};for(int i=0;i<S;++i)for(int k=0;k<S;++k)if(a[i][k])for(int j=0;j<S;++j)c[i][j]=(c[i][j]+1LL*a[i][k]*b[k][j])%P;return c;}
int advance(int state,char c){
    if(state==9)return 9;bool after=state>=3;string pat=after?"cspark":"ccf";int len=after?state-3:state;
    string s=pat.substr(0,len)+c;for(int k=min((int)s.size(),(int)pat.size());k>=0;--k)if(s.substr(s.size()-k)==pat.substr(0,k)){
        if(k==(int)pat.size())return after?9:3;return after?3+k:k;
    }return 0;
}
int main(){
    int n,m;cin >> n >> m;vector<int>a(m+2);for(int i=1;i<=m;++i)cin >> a[i];a[m+1]=n+1;
    vector<Mat>pw(31);for(int i=0;i<S;++i)for(char c='a';c<='z';++c)++pw[0][i][advance(i,c)];for(int k=1;k<31;++k)pw[k]=mul(pw[k-1],pw[k-1]);
    unordered_map<int,Small>cache;
    auto block=[&](int len){
        auto it=cache.find(len);if(it!=cache.end())return it->second;Small out{};int start[3]={0,3,9};
        for(int row=0;row<3;++row){array<int,S>v{};v[start[row]]=1;int x=len;
            for(int k=0;x;x>>=1,++k)if(x&1){array<int,S>w{};for(int i=0;i<S;++i)if(v[i])for(int j=0;j<S;++j)w[j]=(w[j]+1LL*v[i]*pw[k][i][j])%P;v=w;}
            for(int i=0;i<S;++i){int phase=i<3?0:i<9?1:2;out[row][phase]=(out[row][phase]+v[i])%P;}
        }cache.emplace(len,out);return out;
    };
    array<int,3>v{1,0,0};for(int i=1;i<=m+1;++i){Small T=block(a[i]-a[i-1]-1);array<int,3>w{};for(int j=0;j<3;++j)for(int k=0;k<3;++k)w[k]=(w[k]+1LL*v[j]*T[j][k])%P;v=w;}
    cout << v[2] << '\n';
}
