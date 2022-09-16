#include <memory>
#include <map>
#include <functional>
#include <string>
class IOCContainer
{
public:
    class FactoryRoot
    {
    public:
        virtual ~FactoryRoot() {}
    };

    // todo: consider sorted vector
    std::map<std::string, std::shared_ptr<FactoryRoot>> m_factories;

    template <typename T>
    class CFactory : public FactoryRoot
    {
        std::function<std::shared_ptr<T>()> m_functor;

    public:
        ~CFactory() {}

        CFactory(std::function<std::shared_ptr<T>()> functor)
            : m_functor(functor)
        {
        }

        std::shared_ptr<T> GetObject()
        {
            return m_functor();
        }
    };

    template <typename T>
    std::shared_ptr<T> GetObject()
    {
        auto factoryBase = m_factories[typeid(T).name()];
        auto factory = std::static_pointer_cast<CFactory<T>>(factoryBase);
        return factory->GetObject();
    }
    // Most basic implementation - register a functor
    template <typename TInterface, typename... TS>
    void RegisterFunctor(std::function<std::shared_ptr<TInterface>(std::shared_ptr<TS>... ts)> functor)
    {
        m_factories[typeid(TInterface).name()] = std::make_shared<CFactory<TInterface>>([=]
                                                                                        { return functor(GetObject<TS>()...); });
    }

    // Register one instance of an object
    template <typename TInterface>
    void RegisterInstance(std::shared_ptr<TInterface> t)
    {
        m_factories[typeid(TInterface).name()] = std::make_shared<CFactory<TInterface>>([=]
                                                                                        { return t; });
    }

    // Supply a function pointer
    template <typename TInterface, typename... TS>
    void RegisterFunctor(std::shared_ptr<TInterface> (*functor)(std::shared_ptr<TS>... ts))
    {
        RegisterFunctor(std::function<std::shared_ptr<TInterface>(std::shared_ptr<TS>... ts)>(functor));
    }

    // A factory that will call the constructor, per instance required
    template <typename TInterface, typename TConcrete, typename... TArguments>
    void RegisterFactory()
    {
        RegisterFunctor(
            std::function<std::shared_ptr<TInterface>(std::shared_ptr<TArguments>... ts)>(
                [](std::shared_ptr<TArguments>... arguments) -> std::shared_ptr<TInterface>
                {
                    return std::make_shared<TConcrete>(std::forward<std::shared_ptr<TArguments>>(arguments)...);
                }));
    }

    // A factory that will return one instance for every request
    template <typename TInterface, typename TConcrete, typename... TArguments>
    void RegisterInstance()
    {
        RegisterInstance<TInterface>(std::make_shared<TConcrete>(GetObject<TArguments>()...));
    }
};