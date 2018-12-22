#include <iostream>
#include <map>
#include <fstream>
#include <queue>
#include <stdlib.h>
#include <bitset>
#include <vector>
#include <string>
#include <algorithm>

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
void write_into_text_file(string code,string filename){
   fstream out;
   out.open(filename);
   out << codes.size()<<endl;
   for(auto i : codes){
       out<<i.first<<' '<<i.second<<endl;
   }
   out << code;
}
unsigned char int_to_char(int n){
  unsigned char c = 0;
  for(int i=0;i<8;i++){
     c <<= 1;
     if(n&(1<<i))
        c |= 1,cout<<1;
  }
  return c;
}
int char_to_int(char c){
    string x =bitset<8>(c).to_string();
    reverse(x.begin(),x.end());
    int n = 0;
    for(int i=0;i<x.length();i++){
       n <<= 1;
       n |= (x[i]-'0');
    }
    return n;
}
void write_into_binary_file(string code,string filename){
   /// open file
   ofstream OutFile;
   OutFile.open(filename, ios::out | ios::binary);
   /// number of unique characters
   int n = codes.size();
   OutFile << int_to_char(n);
   /// characters , used_bits_of_code x 2 BYTES , character_code
   for(auto i : codes){
       OutFile<<i.first;
       int counter = 0;
       unsigned char c=0;
       if(i.second.length() > 16){
          cout << "Symbol size exceeds 16 bits exiting...";
       }
       int used = (int)i.second.length();
       string mapping = i.second;
       while(mapping.length() < 16) mapping += "0";
       for(int j=0;j<mapping.length();j++){
            c<<=1;
            c|=(mapping[j]-'0');
            counter++;
            if(counter == 8){
               OutFile << c;
               c = 0;
               counter = 0;
            }
       }
       OutFile << int_to_char(used);
   }
   /// write the extra padding
    int all = ((int)code.length())%8;
    OutFile << int_to_char((8-all)%8);
   /// write the encoding
    unsigned char cur_byte = 0;
    int counter = 0;
    for(int i=0;i<code.length();i++){
          int cur_bit = code[i] - '0';
          cur_byte <<= 1;
          cur_byte |= cur_bit;
          counter++;
          if(counter == 8){
               OutFile << cur_byte;
               counter = 0;
               cur_byte = 0;
          }
    }
    if(counter){
        cur_byte <<= 8 - counter;
        OutFile << cur_byte;
    }
    OutFile.close();
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
  in.close();
  /// build tree
  build_Huffman(text);
  /// get code of every character
  string encoded = "";
  for(int i=0;i<text.length();i++){
    encoded += codes[text[i]];
  }
  cout<<"encoded\n";
  cout<<encoded;
  string tmp = "";
  for(int i=0;i<filename.length();i++){
    if(filename[i] == '.')break;
    tmp += filename[i];
  }
  tmp += ".bin";
  write_into_binary_file(encoded,tmp);
}

void printbinchar(char character)
{
   string x = bitset<8>(character).to_string();
   cout<<x;
}
string get_x_bits(char c1,char c2,int used_bits){
   string x = bitset<8>(c1).to_string() + bitset<8>(c2).to_string();
   return x.substr(0,used_bits);
}

void decode(string filename){
   ifstream in;
   in.open(filename,ios::in | ios::binary);
   vector<char>all;
   if(in.is_open()){
        char c;
        while(in.get(c)){
            all.push_back(c);
        }
   }
   in.close();
   int n = char_to_int(all[0]);
   int idx = 1;
   cout<<"\ndecoding...\n"<<"found "<<n<<" unique characters.\n"<<endl;
   map<string,char>reverse_code;
   for(int i=0;i<n;i++){
       /// symbol code how many bits used
       char symbol = all[idx++];
       char code1 = all[idx++];
       char code2 = all[idx++];
       int used_bits = char_to_int(all[idx++]);
       cout<<symbol<<' '<<used_bits<<' '<<get_x_bits(code1,code2,used_bits)<<endl;
       string cur_symbol_code = get_x_bits(code1,code2,used_bits);
       reverse_code[cur_symbol_code] = symbol;
   }
   int padding = char_to_int(all[idx++]);
   string code = "";
   while(idx < all.size()){
         code += bitset<8>(all[idx++]).to_string();
   }
   string tmp = "";
   for(int i=0;i + padding<code.size();i++){
      tmp += code[i];
      if(reverse_code.count(tmp)){
        cout<<reverse_code[tmp];
        tmp = "";
      }
   }

}

int main()
{
    /// call function  compress_data_from_file(filename)
    /// it will read data in filename
    compress_data_from_file("test.in");
    decode("test.bin");

    return 0;
}

/*
number of characters
character -> code  01



*/
