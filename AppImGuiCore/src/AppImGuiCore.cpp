#include <string>
#include <iostream>
#include <dlfcn.h>
#include <array>
#include <map>
#include <memory>
#include "src/IOCContainer.cpp"

typedef std::string (*double_ptr)();

class AppImGuiCore
{
private:
    /* data */

public:

    std::string nombres[2];
    AppImGuiCore(/* args */);
    ~AppImGuiCore();
class IAmAThing
{
public:
   virtual ~IAmAThing() { }
   virtual void TestThis() = 0;
};

class IAmTheOtherThing
{
public:
   virtual ~IAmTheOtherThing() { }
   virtual void TheOtherTest() = 0;
};

class TheThing: public IAmAThing
{
public:
   TheThing()
   {
   }
   void TestThis()
   {
      std::cout << "A Thing from TheThing" << std::endl;
   }
};

class TheOtherThing: public IAmTheOtherThing
{
   std::shared_ptr<IAmAThing> m_thing;
public:
   TheOtherThing(std::shared_ptr<IAmAThing> thing):m_thing(thing)
   {
   }
   void TheOtherTest()
   {
      m_thing->TestThis();
   }
};

class TheOtherThing2: public IAmTheOtherThing
{
   std::shared_ptr<IAmAThing> m_thing;
public:
   TheOtherThing2(std::shared_ptr<IAmAThing> thing):m_thing(thing)
   {
   }
   void TheOtherTest()
   {
      m_thing->TestThis();
      std::cout << "A Thing intern" << std::endl;
   }
};

};

// function export from the *.so


AppImGuiCore::AppImGuiCore(/* args */)
{
    char *error;
    double_ptr GetName;
    void *handle;
    // open the *.so
    handle = dlopen ("./libMCore.so", RTLD_LAZY);
    if (!handle) {
        fputs (dlerror(), stderr);
        exit(1);
    }
    // get the function address and make it ready for use
    GetName = (double_ptr)dlsym(handle, "Name");
    if ((error = dlerror()) != NULL)  {
        fputs(error, stderr);
        exit(1);
    }
    // call the function in *.so
    nombres[0] = (*GetName)();
    // remember to free the resource
    dlclose(handle);

    // open the *.so
    handle = dlopen ("./libMContracts.so", RTLD_LAZY);
    if (!handle) {
        fputs (dlerror(), stderr);
        exit(1);
    }
    // get the function address and make it ready for use
    GetName = (double_ptr)dlsym(handle, "Name");
    if ((error = dlerror()) != NULL)  {
        fputs(error, stderr);
        exit(1);
    }
    // call the function in *.so
    nombres[1] = (*GetName)();
    // remember to free the resource
    dlclose(handle);

    IOCContainer gContainer;
    gContainer.RegisterInstance<IAmAThing, TheThing>();
    gContainer.RegisterFactory<IAmTheOtherThing, TheOtherThing, IAmAThing>();
    gContainer.RegisterFactory<IAmTheOtherThing, TheOtherThing2, IAmAThing>();
    gContainer.GetObject<IAmTheOtherThing>()->TheOtherTest();

}

AppImGuiCore::~AppImGuiCore()
{
}
