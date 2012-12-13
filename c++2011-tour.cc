// Public domain software by bert.hubert@netherlabs.nl 
#include <vector>
#include <unordered_map>
#include <stdlib.h>
#include <algorithm>
#include <list>
#include <iostream>
#include <string>	
#include <memory>

using namespace std;

// this class has deleted copy constructors and assignment operators
class NoCopy
{
  public:
    NoCopy(const NoCopy& rhs) = delete;
    NoCopy& operator=(const NoCopy& rhs) = delete;
    explicit NoCopy(int s) 
    {
      cout<<"Constructed "<<s<<", we are @"<<(void*)this << endl;
      d_s = new int(s);
    }
    
    NoCopy(NoCopy&& rhs) // this is a 'move' constructor which *kills* the donor
    {
      d_s = rhs.d_s;
      rhs.d_s = 0;
      cout << "Move constructed "<<*d_s<<", we are @" <<(void*)this<<", they were @"<<(void*)&rhs <<endl;
    }
    
    int getS() const
    {
      return *d_s;
    }
    
    ~NoCopy() 
    {
      if(d_s)
	cout<<"Being destroyed "<<*d_s <<", we were @"<<(void*)this<<endl;
      delete d_s;
    }
  private:
    int* d_s = 0;   // YEAH FINALLY - we can set defaults! just like that!
};


// shows how you can use initialized_lists yourself
void initList(const initializer_list<int>& args)
{
  cout << "initList: \n";
  for (auto& p : args ) cout << p << "\n";
}

int main()
{
  // generates a nice error message AT COMPILE TIME if this is not true
  static_assert(sizeof(long) == 8, "This code requires 64 bits (not really)");
  
  // -----------------------------------------------
  
  // this is a powerful new way to initialize a lot of things
  vector<int> v = {50, -10, 20, -30};
  cout << "Biggest: " << max({11, 12, 32, 2345}) << endl;
  // powered by 'variadic templates' 
  
  // -----------------------------------------------
  
  // char c{129};     // this generates an error, 129 does not fit in a signed char
  
  // -----------------------------------------------
  
  // sort our vector using a lambda function as comparison
  sort(v.begin(), v.end(), 
    [](int a, int b) 
    { 
      return abs(a)<abs(b); 
    }
  );
  
  // -----------------------------------------------
  
  // alternately, we can now define local functors, which was impossible in c++2003
  struct abscomp {
    bool operator()(int a, int b) const { return abs(a) < abs(b) ; }
  };
  sort(v.begin(), v.end(), abscomp());
  
  // -----------------------------------------------
  
  // this executes a lambda function, which returns a lambda function which we execute
  [](){return [](){};}()();    // yo dawg
  
  // -----------------------------------------------
  
  // this is is how to store a lambda function with a static type
  function<void(void)> fptr = [](){};
  fptr();
  
  // -----------------------------------------------
  
  // and this is convenient, but we can't put it in a container
  auto aptr = [](){};
  aptr();
  // otherwise auto rocks!
  
  // -----------------------------------------------
  
  int version;
  
  /* note the >> below - finally! I find that I miss the space after a decade+ tho ;-)
   * this is the new shorthand initializer way of filling a map with lambdas.
   * this map is unordered (and uses hashes internally), this can be faster */
  
  unordered_map<string, function<void(void)>> directory = { 
      {"hello",   [&](){ cout << "Hello, C++" << version <<endl; } },
      {"goodbye", [&](){ cout << "Goodbye, C++" << version << endl; } }
  };
  // note the [&] above, this means that all local variables are 'captured by reference'
  
  version = 2011;
  directory["hello"]();
  version = 2003;
  
  // this is the new 'walk a container' syntax
  for(auto m : directory) {
    cout << "Now executing " << m.first << endl;
    m.second(); // and execute
  }
  
  // there are some new algorithms, like all_of, any_of, none_of etc
  string lc("hello brave new world");
  cout << "Is string '"<<lc<<"' all lower case: " << 
    all_of(lc.begin(), lc.end(), [](char c) { return !isalpha(c) || islower(c); })
  << endl;
  
  
  // -----------------------------------------------
  
  // see the function above
  initList({1,2,3,4});
  
  // -----------------------------------------------
  
  NoCopy nc(1), bc(2);
  
  // this will error out, since we 'deleted' operator= and copy constructor:
  // bc=nc;    
  
  // -----------------------------------------------
  
  cout << R"(\n\\\n)" << endl;   // prints \n\\\n -> these are 'R'aw strings, delimiters are "( and )"

  // -----------------------------------------------

  cout << "Vector of 4 NoCopy instances - note 4 actual constructions and destructions" <<endl;
  {
    vector<NoCopy> ncvec;
    for(auto n : {1,2,3,4})           // because we can
      ncvec.emplace_back(NoCopy(n));  // look ma, no copies!
      
    for(auto& n : ncvec)
      cout << n.getS() << endl;
  }
  cout << "Done with vector"<<endl;
  
  // -----------------------------------------------
  
  unique_ptr<NoCopy> up(new NoCopy(1));
  unique_ptr<NoCopy> down;
  down = move(up);
  // up->getS(); // this would crash, 'up' no longer owns it!
  
  // -----------------------------------------------
  
  // this works much like shared_ptr, but with less overhead
  cout<<"vector of unique_ptr's of NoCopy instances, again only 4 constructions"<<endl;
  {
    vector<unique_ptr<NoCopy>> upncvec; 
    for(unsigned int n = 0; n < 4; ++n) {
      upncvec.push_back(unique_ptr<NoCopy>(new NoCopy(n)));
    }
    upncvec[3]->getS(); 			 // this is fine, gets a reference
    unique_ptr<NoCopy> loose = move(upncvec[3]); // now we are f*cked
    // upncvec[3]->getS();                       // crashes - only move if you know what you are doing
  }
  cout<<"and 4 destructions"<<endl;
  
  // -----------------------------------------------
  NoCopy A(123);
  cout << "A.getS(): "<< A.getS() << endl;
  NoCopy B(move(A));
  cout << "B.getS(): "<< B.getS() << endl;
  cout << "Will now crash: A.getS(): "; cout.flush(); cout << A.getS() << endl;
  
  // -----------------------------------------------
}
