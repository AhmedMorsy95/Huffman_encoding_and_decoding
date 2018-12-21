#include <iostream>
#include <map>
#include <fstream>
#include <queue>

using namespace std;

class node{
  public:
   node *lft,*rgt;
   int freq;
   char c;
   node(){;}
   node(int a,char b){
        freq = a;
        c = b;
        lft = rgt = NULL;
   }
};
bool is_leaf(node x){
  if(x.lft == NULL && x.rgt == NULL) return 1;
  return 0;
}

map<char,string>codes;

void traverse(node *root,string s){ /// to get code of every character
    if(root == NULL)return;
    if(is_leaf(*root)){
        codes[root->c] = s;
        cout<<root->c<<' '<<s<<endl;
        return;
    }
    traverse(root->lft,s+"0");
    traverse(root->rgt,s+"1");
}
struct CompareNode /// comparator for nodes in priority queue
{
  bool operator()(const node* a, const node* b) const
  {
     return a->freq > b->freq;
  }
};
void build_Huffman(string text){
   /// count frequency
   map<char,int>mp;
   for(int i=0;i<text.length();i++){
      mp[text[i]]++;
   }
   /// add to priority queue
   priority_queue<node*,vector<node*>,CompareNode>q;
   cout<<"frequency\n";
   for(auto i : mp){
      node *cur = new node(i.second,i.first);
      q.push(cur);
      cout<<i.first<<' '<<i.second<<endl;
   }
   while(q.size() > 1){
      node *first = q.top();
      q.pop();
      node *second = q.top();
      q.pop();
      node *parent = new node(first->freq+second->freq,'$');/// dummy character
      parent->lft = first;
      parent->rgt = second;
      q.push(parent);
   }
   cout<<"Codes\n";
   /// traveserse tree
   traverse(q.top(),"");

}
void compress_data_from_file(string filename){ /// file to open
  /// reading data
  ifstream in;
  in.open(filename);
  string text = "";
  if(in.is_open()){
    string line = "";
    while(getline(in,line)){
        if(text.length()) text += "\n";
        text += line;
    }
  }
  /// build tree
  build_Huffman(text);
  /// get code of every character
  string encoded = "";
  for(int i=0;i<text.length();i++){
    encoded += codes[text[i]];
  }
  cout<<encoded;
}
int main()
{
    /// call function  compress_data_from_file(filename)
    /// it will read data in filename
    compress_data_from_file("test.in");
    return 0;
}
