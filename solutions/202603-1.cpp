#include <bits/stdc++.h>
using namespace std;
int main(){
    int n,answer=0; cin>>n;
    while(n--){
        unsigned x; cin>>x; int ones=0,bits=0;
        for(;x;x>>=1){ ones+=x&1; ++bits; }
        answer+=(2*ones==bits);
    }
    cout<<answer<<'\n';
}
