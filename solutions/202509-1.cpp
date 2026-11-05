#include <bits/stdc++.h>
using namespace std;
long long read100() {
    string s;cin >> s;bool neg=!s.empty()&&s[0]=='-';if(neg)s=s.substr(1);
    auto p=s.find('.');string a=s.substr(0,p),b=p==string::npos?"":s.substr(p+1);
    while(b.size()<2)b+='0';long long v=stoll(a)*100+stoll(b);return neg?-v:v;
}
int main() {
    int n,a,m=0;cin >> n >> a;
    for(int i=0;i<n;++i) {long long x=read100(),y=read100();m+=x*x+y*y<=10000LL*a*a;}
    cout << fixed << setprecision(6) << 4.0*m/n << '\n';
}
