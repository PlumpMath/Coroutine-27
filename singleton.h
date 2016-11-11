#ifndef SINGLETON_H
#define SINGLETON_H
#include <memory>

/** Derive this class to apply singleton pattern easily and quickly. */
template<typename ClassType>
class Singleton
{
public:
	/** Get a raw pointer to the unique instance. */
	static ClassType* GetInstance();
	/** Delete the unique instance, using unique_ptr<Type>.reset() function. */
	static void DeleteSingleton();

protected:
	/** Disable copy construct. */
	Singleton();
	/** Disable copy construct. */
	virtual ~Singleton();

private:
	/** Disable be constructed from the outside. */
	Singleton(const Singleton&);
	/** Disable be destructed from the outside. */
	Singleton& operator=(const Singleton&);

	/** The unique instance. */
	static std::unique_ptr<ClassType> pInstance;
};


/** Put this macro inside class which want to apply singleton pattern. */
#define APPLY_SINGLETON(ClassType)								\
	friend class Singleton<ClassType>;\
protected:														\
	ClassType() = default;										\
	ClassType(const ClassType&) = delete;						\
	ClassType& operator=(const ClassType&) = delete;			

#define APPLY_SINGLETON_WITH_NO_DEFUALT_CONSTRUCTOR(ClassType)	\
	friend class Singleton<ClassType>;\
protected:														\
	ClassType();												\
	ClassType(const ClassType&) = delete;						\
	ClassType& operator=(const ClassType&) = delete;						

template<typename ClassType>
std::unique_ptr<ClassType> Singleton<ClassType>::pInstance;

template<typename ClassType>
ClassType* Singleton<ClassType>::GetInstance()
{
	if (!pInstance)
	{
		// error caused by accessing permission
		// pInstance = make_unique<ClassType>();
		pInstance = std::unique_ptr<ClassType>(new ClassType());
	}
	return pInstance.get();
}

template<typename ClassType>
void Singleton<ClassType>::DeleteSingleton()
{
	pInstance.reset();
}

template<typename ClassType>
Singleton<ClassType>::Singleton() {}

template<typename ClassType>
Singleton<ClassType>::~Singleton() {}

#endif //!SINGLETON_H