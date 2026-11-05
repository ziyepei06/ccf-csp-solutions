#include <bits/stdc++.h>
using namespace std;
using ll=long long;const int MOD=998244353;
int local(int p,int k,int op){
    vector<ll>pow(3*k+1,1);for(int i=1;i<=3*k;++i)pow[i]=pow[i-1]*p%MOD;
    ll all=0;for(int c=0;c<=2*k;++c)for(int e=0;e<=2*k&&c+e<=3*k;++e)all=(all+pow[3*k-c-e])%MOD;
    if(!op)return all;
    ll f1=0,f3=0,f4=(2*k+1)*pow[k]%MOD,f5=0;
    for(int i=0;i<=2*k;++i)f1=(f1+pow[i])%MOD;
    for(int c=0;2*c<=3*k;++c)f3=(f3+pow[3*k-2*c])%MOD;
    for(int c=(k+1)/2;c<=3*k/2;++c)f5=(f5+pow[c])%MOD;
    // Return each component separately through the caller's mode.
    if(op==1)return f1;if(op==2)return f3;if(op==3)return f4;return f5;
}
int main(){
    ios::sync_with_stdio(false);cin.tie(nullptr);int op,n;cin >> op >> n;
    vector<int>lp(n+1),pk(n+1),primes;vector<array<int,5>>f(n+1);f[1].fill(1);
    for(int i=2;i<=n;++i){
        if(!lp[i]){lp[i]=i;pk[i]=i;primes.push_back(i);for(int j=0;j<(op?5:1);++j)f[i][j]=local(i,1,j);}
        for(int p:primes){
            if(1LL*p*i>n)break;int x=p*i;lp[x]=p;
            if(p==lp[i]){
                pk[x]=pk[i]*p;int rest=i/pk[i];
                if(rest==1){int k=0,v=x;while(v%p==0){v/=p;++k;}for(int j=0;j<(op?5:1);++j)f[x][j]=local(p,k,j);}
                else for(int j=0;j<(op?5:1);++j)f[x][j]=1LL*f[rest][j]*f[pk[x]][j]%MOD;
                break;
            }
            pk[x]=p;for(int j=0;j<(op?5:1);++j)f[x][j]=1LL*f[i][j]*f[p][j]%MOD;
        }
    }
    ll ans=0;for(int i=1;i<=n;++i){ll v=f[i][0];if(op)v=v-2LL*f[i][1]-f[i][2]-f[i][3]-2LL*f[i][4]+5LL*i;ans=(ans+v)%MOD;}
    cout << (ans+MOD)%MOD << '\n';
}
