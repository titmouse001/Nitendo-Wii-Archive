#ifndef Singleton_H
#define Singleton_H

//Example use: 
//	WiiManager& Wii ( Singleton<WiiManager>::GetInstanceByRef() );

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

// This class will prevent assignment or copying of singletons.
// example: 
//	class WiiManager : private SingletonClient { ... }

class SingletonClient
{
private:
    SingletonClient( const SingletonClient& sc ) { } // never called.
    SingletonClient operator=( const SingletonClient& sc ) {  return *this; }  // Never called
public:
    SingletonClient() {}
};




//template <typename T> 
//class Singleton
//{
//public:
//    static T* GetSingleton()
//    {
//        if (m_instance == NULL) 
//		{
//			m_instance = new T;
//		}
//        return m_instance;
//    };
//
//    static void Destroy()
//    {
//        delete m_instance;
//        m_instance = NULL;
//    };
//
//protected:
//    // prevent others from creating or destroying a Singleton
//    Singleton() {};
//    virtual ~Singleton() {};
//private:
//    Singleton(const Singleton& source) {};
//    static T* m_instance;
//};
//
//// static class member initialisation.
//template <typename T> T* Singleton<T>::m_instance = NULL;

#endif 

