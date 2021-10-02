#ifndef Singleton_H
#define Singleton_H

// -= Singleton pattern =-
//
// Creates an instance of the class if one does not exist. 
// If an instance already exists it returns the existing object. 
// Basically an object orientated global variable that stay inside the rules of OO design.
//
// Class Declaration Example:
// ==========================
//
// class WiiManager : private SingletonClient  // this class can only ever have one instance
// { 
//   
//    public:
//		int GetSomething() { return 42; }
//
//	  private:
//
// }
//
//Definition code use example:
//============================
//
//  #include "Singleton.h"
//  ...
//	WiiManager& Wii ( Singleton<WiiManager>::GetInstanceByRef() ); // This line can go anywhere in your code
//  int Value( Wii.GetSomething() );  // or can be ... int Value = Wii.GetSomething() ;


template <class _template>
class Singleton : private _template
{
private:
    Singleton() {}
public:
    static _template& GetInstanceByRef()
    {
        static _template inst;
        return inst;
    }
    static _template* GetInstanceByPtr()
    {
        _template& inst = GetInstanceByRef();
        return &inst;
    }
};

// This class will help prevent assignment or copying of singletons.
class SingletonClient
{
private:
    SingletonClient( const SingletonClient& sc ) { } // Never called.
    SingletonClient operator=( const SingletonClient& sc ) {  return *this; }  // Never called
public:
    SingletonClient() {}
};


#endif 

