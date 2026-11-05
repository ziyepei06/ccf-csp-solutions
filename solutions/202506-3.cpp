#include <bits/stdc++.h>
using namespace std;
using ull=unsigned long long;
ull bits(const string&s,int start,int len){ull x=0;while(len--)x=x*2+s[start++]-'0';return x;}
char digit(int v){if(!v)return ' ';if(v<=10)return '0'+v-1;if(v<=36)return 'A'+v-11;return '_';}
string fullName(ull x){string s(11,' ');for(int i=10;i>=0;--i){s[i]=digit(x%38);x/=38;}while(!s.empty()&&s.back()==' ')s.pop_back();return s;}
string shortName(ull x){
    string s(6,' ');for(int i=5;i>=3;--i){s[i]='A'+x%26;x/=26;}s[2]='0'+x%10;x/=10;
    int v=x%36;x/=36;s[1]=v<10?'0'+v:'A'+v-10;s[0]=digit(x);if(s[0]==' ')s.erase(s.begin());return s;
}
ull number(string s){while(s.size()<11)s+=' ';ull x=0;for(char c:s){int v=c==' '?0:isdigit(c)?c-'0'+1:isupper(c)?c-'A'+11:37;x=x*38+v;}return x;}
int main(){
    int n;cin >> n;unordered_map<ull,string>h12,h25;
    auto infer=[&](ull h,int b){auto &M=b==12?h12:h25;auto it=M.find(h);return it==M.end()?string("###"):"#"+it->second;};
    auto record=[&](const string&s){ull h=number(s)*47055833459ULL;h12[h>>(64-12)]=s;h25[h>>(64-25)]=s;};
    while(n--){
        string s,recv,send;cin >> s;bool directR=false,directS=false;int loc=0;
        if(s[0]=='0'){
            ull a=bits(s,1,28),b=bits(s,29,28);loc=bits(s,57,15);
            directR=a>=(1ULL<<25);directS=b>=(1ULL<<25);
            recv=directR?shortName(a-(1ULL<<25)):infer(a,25);send=directS?shortName(b-(1ULL<<25)):infer(b,25);
        }else{
            string plain=fullName(bits(s,1,58)),other=infer(bits(s,59,12),12);
            if(s[71]=='1'){recv=plain;send=other;directR=true;}else{recv=other;send=plain;directS=true;}
        }
        cout << recv << ' ' << send;if(loc)cout << ' ' << loc;cout << '\n';
        if(directR)record(recv);if(directS)record(send);
    }
}
