
#include "antinno.LibraryLoader.class.h"
#include "common/LimaCommon.h"
#include <fstream>
#include <boost/algorithm/string.hpp>
#include <boost/locale.hpp>
#ifdef WIN32
  #define WIN32_LEAN_AND_MEAN
  #include <Windows.h>
  #ifdef ERROR
    //#undef ERROR
  #endif
#endif

namespace Lima { namespace antinno {

#ifdef WIN32
class SystemMsg
{
public:
  SystemMsg(DWORD msgId) : _msgId(msgId), _lpMsgBuf(NULL)
  {
    DWORD msgBufLen = ::FormatMessage(
      FORMAT_MESSAGE_ALLOCATE_BUFFER | // max 64K bytes
      FORMAT_MESSAGE_FROM_SYSTEM |
      FORMAT_MESSAGE_IGNORE_INSERTS,
      NULL, // lpSource (optional)
      _msgId,
      MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
      (LPTSTR) &_lpMsgBuf,
      0, NULL );
    if (msgBufLen != 0)
      _s.assign(static_cast<::std::wstring::value_type const*>(_lpMsgBuf));
    else // traitement par défaut mais perfectible
      _s.clear();
  }
  ::std::string toUtf8String() const
  {
    return ::boost::locale::conv::utf_to_utf<char>(_s);
  }
  ~SystemMsg()
  {
    LocalFree(_lpMsgBuf);
  }
private:
  LPVOID _lpMsgBuf;
  DWORD _msgId;
  ::std::wstring _s;
};
#else
  #error no implementation for non-win32 systems
#endif

LibraryLoader::LibraryLoader()
{
}
void LibraryLoader::loadFromFile(::std::string const& filePath)
{
  ABSTRACTFACTORYPATTERNLOGINIT
  ::std::ifstream in(filePath);
  if (!in)
    throw ::std::exception((::std::string("Cannot open file (read mode): ") + filePath).data());
  ::std::string line;
  while (::std::getline(in, line))
  {  
    ::boost::algorithm::trim(line);
    if (line.size() > 1 && line[0] != '#') // skip comment lines beginning with "#"
    {
#ifdef WIN32
      ::std::string const path = line + ".dll";
#else
#error no implementation for non-win32 systems
#endif
#ifdef WIN32
      if (NULL != /*win32*/::LoadLibrary(path.c_str()))
#else
#error no implementation for non-win32 systems
#endif
      {
        ::std::cout << L"Plugin successfully loaded: " << path << ::std::endl;
        LDEBUG << "Plugin successfully loaded: " << path;
      }
      else
      {
#ifdef WIN32
        auto const msgId = ::GetLastError();
#else
#error no implementation for non-win32 systems
#endif
        ::std::cout << L"Plugin loading failed: " << line << " : (err windows " << msgId << ") " << SystemMsg(msgId).toUtf8String() << ::std::endl;
        LDEBUG << L"Plugin loading failed: " << line << " : (err windows " << msgId << ") " << SystemMsg(msgId).toUtf8String();
      }
    }
  }
}

}}
