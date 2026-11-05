#include <bits/stdc++.h>
using namespace std;
long long scaled(string s){
    s.erase(s.find('.'),1);return stoll(s);
}
int main(){
    int n,m,k;cin>>n>>m>>k;long long mu=0,variance=0;
    string s;for(int i=0;i<n;i++){cin>>s;mu+=scaled(s);}
    for(int i=0;i<n;i++){cin>>s;long long x=scaled(s);variance+=x*x;}
    vector<long long> x(k);vector<string> phi(k);
    for(auto &a:x){cin>>s;a=scaled(s);}for(auto &a:phi)cin>>a;
    // z 与两查表点的中点比较；两边非负，平方后可精确比较。
    __int128 lhs=(__int128)2000*(1000LL*m-mu);lhs*=lhs;
    int chosen=k-1;
    for(int i=0;i+1<k;i++){
        __int128 mid=x[i]+x[i+1];
        if(lhs<=mid*mid*variance){chosen=i;break;}
    }
    cout<<phi[chosen]<<'\n';
}
