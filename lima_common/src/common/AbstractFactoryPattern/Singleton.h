// Copyright 2002-2016 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

#ifndef LIMA_MISC_SINGLETON_H
#define LIMA_MISC_SINGLETON_H
#include <memory>

namespace Lima
{

/**
  * Defines access method for the singleton pattern. To ensure object is a singleton,
  * it should have his only default constructor defined private.
  * @brief defines a singleton
  * @author Benoit Mathieu <mathieub@zoe.cea.fr>
  * @param Object Type of singleton Object
  */
template<typename Object>
class Singleton
{
public:
  Singleton() {}
  virtual ~Singleton() {}
  
  /**
    * @brief const singleton accessor
    */
  static const Object& single();
  static const Object* psingle();

  /**
    * @brief singleton accessor
    */
  static Object& changeable();
  static Object* pchangeable();


private:
  static std::unique_ptr< Object > s_instance;

  Singleton(const Singleton<Object>&) {}
};

template<typename Object>
std::unique_ptr< Object > Singleton<Object>::s_instance(new Object());

template<typename Object>
const Object& Singleton<Object>::single()
{
  if (s_instance==0)
  {
    s_instance=std::unique_ptr< Object >(new Object());
  }
  return *s_instance;
}

template<typename Object>
const Object* Singleton<Object>::psingle()
{
  if (s_instance==0)
  {
    s_instance=std::unique_ptr< Object >(new Object());
  }
  return s_instance.get();
}

template<typename Object>
Object& Singleton<Object>::changeable()
{
  if (s_instance==0)
  {
    s_instance=std::unique_ptr< Object >(new Object());
  }
  return *s_instance;
}

template<typename Object>
Object* Singleton<Object>::pchangeable()
{
  if (s_instance==0)
  {
    s_instance=std::unique_ptr< Object >(new Object());
  }
  return s_instance.get();
}

} // Lima

#endif
