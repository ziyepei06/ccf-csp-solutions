#include <bits/stdc++.h>
using namespace std;
int main(){
    int s;cin >> s;string hex,line;while(cin >> line)hex+=line;
    vector<int>a;for(int i=0;i<2*s;i+=2)a.push_back(stoi(hex.substr(i,2),nullptr,16));
    int p=0,N=0,shift=0;
    while(true){int x=a[p++];N|=(x&127)<<shift;if(!(x&128))break;shift+=7;}
    vector<int>out;out.reserve(N);
    while(p<s){
        int tag=a[p++],type=tag&3,len,off;
        if(type==0){
            len=tag>>2;
            if(len<60)++len;
            else{int bytes=len-59;len=0;for(int j=0;j<bytes;++j)len|=a[p++]<<(8*j);++len;}
            while(len--)out.push_back(a[p++]);
        }else{
            if(type==1){len=((tag>>2)&7)+4;off=((tag>>5)<<8)|a[p++];}
            else{len=(tag>>2)+1;off=a[p]|(a[p+1]<<8);p+=2;}
            while(len--){int x=out[out.size()-off];out.push_back(x);}
        }
    }
    const char *digits="0123456789abcdef";
    for(size_t i=0;i<out.size();++i){cout << digits[out[i]>>4] << digits[out[i]&15];if(i%8==7||i+1==out.size())cout << '\n';}
}
