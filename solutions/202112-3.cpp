#include <bits/stdc++.h>
using namespace std;
int main() {
    int w,s;string str;cin >> w >> s >> str;int k=s<0?0:1<<(s+1);
    vector<int> symbols;int mode=0;
    for(char c:str) {
        int next=isupper((unsigned char)c)?0:islower((unsigned char)c)?1:2;
        if(next!=mode) {
            if(next==0) {symbols.push_back(28);if(mode==1)symbols.push_back(28);}
            else symbols.push_back(next==1?27:28);
            mode=next;
        }
        symbols.push_back(mode==0?c-'A':mode==1?c-'a':c-'0');
    }
    if(symbols.size()%2)symbols.push_back(29);
    vector<int>d(1,0);
    for(size_t i=0;i<symbols.size();i+=2)d.push_back(30*symbols[i]+symbols[i+1]);
    while((d.size()+k)%w)d.push_back(900);d[0]=d.size();
    vector<int>g(1,1);int power=1;
    for(int i=1;i<=k;++i) {
        power=power*3%929;vector<int>h(g.size()+1);
        for(size_t j=0;j<g.size();++j) {
            h[j]=(h[j]+g[j])%929;
            h[j+1]=(h[j+1]-g[j]*power%929+929)%929;
        }
        g.swap(h);
    }
    vector<int>r=d;r.resize(d.size()+k);
    for(size_t i=0;i<d.size();++i) {
        int x=r[i];
        for(int j=0;j<=k;++j)r[i+j]=(r[i+j]-x*g[j]%929+929)%929;
    }
    for(int x:d)cout << x << '\n';
    for(size_t i=d.size();i<r.size();++i)cout << (929-r[i])%929 << '\n';
}
