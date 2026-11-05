#include <bits/stdc++.h>
using namespace std;
int main(){
    int n;cin>>n;vector<int> normal(n),bank(n);
    for(int i=0;i<n;i++){
        string s;cin>>s;auto dot=s.find('.');
        int a=stoi(s.substr(0,dot)),b=s[dot+1]-'0';
        normal[i]=a+(b>=5);
        bank[i]=a+(b>5||(b==5&&(a&1)));
    }
    for(auto &row:{normal,bank}){
        for(int i=0;i<n;i++)cout<<row[i]<<(i+1==n?'\n':' ');
    }
}
