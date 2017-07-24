/// Не сделал IN .. TRY .. FINALLY .. END, т. к. creat создаёт сразу две сущности: файл и файловый дескриптор. Также иногда сразу возникает объект, который нужно уничтожить, например, в sh_multicat
/// Выбран C, т. к. libsh может пригодиться в программах на C. Поддержку исключений C++ не добавляю, т. к. мне это не надо
/// Используются пустые исключения, т. к. я не понимаю, зачем нужно что-то ещё. Если кто-нибудь убедит меня в нужности непустых исключений - буду рад
/// Вместо rethrow юзайте throw
/// libsh - это единственный способ программировать на C
/// Есть возможность задать _sh_out и _sh_err, т. к. это может пригодиться в mini. Предполагаем, что stdout и stderr можно юзать даже если они закрыты при старте программы, поэтому sh_init без всяких проверок инициализирует _sh_out и _sh_err в stdout и stderr. Но при этом допускается, что юзер может сам запортить окончательно stdout и stderr или просто может не захотеть куда-то что-то выводить. Поэтому оставлена возможность положить в _sh_out и _sh_err NULL. При старте всегда нужно вызывать sh_init, т. к. в эту функцию могут быть позже добавлены прочие инициализации. mini - это моя программа, возможно, сейчас не опубликованная. Инициализировать _sh_err статически stderr'ом нельзя (gcc не даёт), так что это сделано в sh_init. sh_init желательно вызывать совершенно первым, до проверки аргументов, т. к. sh_init проверяет argv[0]
/// Не предназначен для совсем старых систем, где, скажем, select не в том хедере, т. е. не может выбрать хедер из нескольких. винда тоже не подходит, там execve не там
/// Не обрабатывает EINTR по-особому, юзайте sigaction с рестартом (есть несколько счастливых исключений в libsh: select и др.)
/// Теперь вы будете проверять ошибки даже у write, close и т. д.
/// Проблемы с безопасностью: юзеру печатаются аргументы, которые могут быть секретны, особенно в SUID-программах
/// libsh - это просто хак, который предназначен для маленьких программ
  /// Of course, this is not user-friendly reports, не пригодна для больших проектов, так как не сообщает контекст, способ устранения ошибки
  /// Даже для серверного использования не подходит, сообщается лишь конкретный свалившийся вызов без контекста
  /// Не является истинно exception-safe, т. к. sh_x_close может не закрыть fd
  /// И идея пустых исключений кажется сомнительной для больших проектов
  /// libsh предназначен для простых синхронных однопоточных программ, в основном для UNIX-подобных систем
  /// libsh предназначен для консольных программ, которые просто пишут все ошибки в какой-нибудь fp
/// Привести кучу примеров, доказывающих необходимость libsh (копирование файла, fork/exec, pipe/fork)
/// libsh обычно не проверяет правильность аргументов, и не проверяет вообще ничего. Он проверяет лишь, вернула ли ошибку функция. В частности, он не проверяет, что аргументы не-NULL. Если они должны быть не-NULL, libsh этому верит
/// В типичном случае юзаются в основном FINALLY, а CATCH гораздо реже, и блок CATCH обычно пустой
/// libsh против "goto fail" (например, тут: http://nakedsecurity.sophos.com/2014/02/24/anatomy-of-a-goto-fail-apples-ssl-bug-explained-plus-an-unofficial-patch/ )
/// Манипуляции с тем fd (и fp), куда делается вывод ошибок, нужно делать осторожно
/// xx-функции - это как раз костыли вместо непустых исключений? А всякие write_close - костыли против нормального C++
/// Исключения сделаны пустыми, т. к. иначе одно исключение могло бы "перебить" другое
/// Вам нужно указать нужный feature test macro перед #include <libsh.h> (некий минимальный для получения sigsetjmp), _POSIX_C_SOURCE=1 подойдёт
/// sh_x_write возвращает ssize_t, а не size_t, хоть size_t был бы и логичнее. Я всегда возвращаю либо то же, либо void в x-функциях. Тем не менее, функции, не относящиеся к функциям с одним "x", могут возвращать size_t в таких ситуациях
/// C vs C++ statement
  /// Стандарт C и компиляторы C имеют право на существование как минимум потому, что C - это язык с context-free грамматикой, а потому его компиляция идёт быстрее. И если все части GNU/Linux на C переписать на C++, то Gentoo будет собираться дольше
  /// C++ - это монстр (см. аргументы Линуса), нужно юзать ограниченное подмножество C++
  /// В C нет деструкторов и исключений, а они там реально нужны, без них невозможно писать сколько-нибудь сложный exception-safe код (даже с libsh)
  /// Так что для тех нужд, где нужна бескомпромиссная производительность (достижимая в C, C++ и подобных языках) нужно использовать не C и не полноценный C++, а ограниченное подмножество C++ (либо использовать Rust, других языков с бескомпромиссной производительностью я не знаю [не считая ассемблера, а также Pascal, FORTRAN и тому подобных совершенно устаревших языков])
  /// Но существуют люди, не признающие C++. Если я скажу, что libsh только для C++, то они его сразу отбросят. Поэтому я делаю libsh для C. Т. е. для того, чтобы его можно было использовать в проектах на C
  /// Свои программы я, скорее всего, тоже буду писать на C. Пока это будет получаться (т. к. если можно писать на C, надо писать на C). Но как только реально понадобится C++ (например, понадобится написать какой-нибудь очень сложный exception-safe код), я воспользуюсь C++
  /// Если вам нужен libsh в программе на C++, просто используйте его, но знайте, что исключения libsh несовместимы с исключениями и деструкторами C++. Можно поменять несколько макросов и заставить libsh бросать исключения C++. Но настоящая сиплюсплюсизация libsh означает оборачивание файловых дескрипторов в объекты, закрывающие файл в деструкторе, и тому подобные вещи (а ещё семантику перемещения для этих обёрток и т. д. и т. п.)
  /// Ещё раз скажу: C++ лучше C. На C++ можно было бы написать libsh гораздо короче и гораздо короче его использовать. Но я отлично понимаю людей, отрицающих C++, и поэтому пишу libsh для них на C. Для того, чтобы libsh можно было юзать в проектах на C
  /// Я не стал делать #ifdef __cplusplus #define SH_THROW ..., т. к. это приведёт к несовместимости бинарников, собранных из разных языков
  /// Важный аргумент в пользу поддержки C в libsh: libdebian-installer на 2015-08-13 не совместим с C++

/// Под exception safety я везде подразумеваю basic exception safety из https://en.wikipedia.org/wiki/Exception_safety . Вся libsh является exception-safe в этом смысле, и главное её предназначение - писать такой exception-safe код. Но её можно использовать также и для написания strong exception-safe кода (из определения статьи на Википедии). Например, этот код я назову basic exception-safe:
/// int fd = sh_x_creat ("file", 0666);
/// SH_FTRY // fd
///   {
///     sh_repeat_write (fd, "hello\n", 6);
///   }
/// SH_FINALLY // fd
///   {
///     sh_x_close (fd);
///   }
/// SH_FEND; // fd
/// Но он не strong exception-safe, потому что может оставить недописанный file. А вот это strong exception-safe код:
/// int fd = sh_x_creat ("file", 0666);
/// SH_CTRY // file
///   {
///     SH_FTRY // fd
///       {
///         sh_repeat_write (fd, "hello\n", 6);
///       }
///     SH_FINALLY // fd
///       {
///         sh_x_close (fd);
///       }
///     SH_FEND; // fd
///   }
/// SH_CATCH // file
///   {
///     sh_x_unlink ("file");
///     SH_THROW;
///   }
/// SH_CEND; // file
/// Разумеется, код на libsh не является автоматически exception-safe, вам нужно самостоятельно потрудиться для этого :)
/// И вообще, нигде под exception-safe (и strong exception-safe) не подразумевается истинный exception-safe, т. к. всевозможные close и unlink всё равно могут фейлиться

/// Хорошая идея писать после SH_FTRY, SH_FINALLY, SH_FEND, SH_CTRY, SH_CATCH, SH_CEND в комментах тот объект, для которого написан этот блок. Ровно этот объект должен деструктуироваться в блоке SH_FINALLY/SH_CATCH. SH_FINALLY нужно использовать, если объект нужно деструктуировать в любом случае, SH_CATCH - в случае ошибки
  /// Можно привести пример с несколькими объектами, часть из которых нужно деструктуировать в любом случае, часть - в случае ошибки
/// См. Мастер Фу и десять тысяч строк кода Си, libsh - это среднее между C и shell. libsh обеспечивает манипулативити и что-то там ещё на той диаграмме с перлом :)
/// Примеры использования (во всех этих случаях shell слишком забаженный, тормознутый, не даёт низкоуровневых деталей и не мощный, а C слишком verbose). Найти ещё кучу примеров во всём моём софте
///   Простейшая система инициализации, которая всё делает сама, без использования скриптов из /etc/init.d . Ей приходится постоянно запускать процессы, делать cat /etc/hostname > /proc/hostname, echo comp > /proc/hostname и т. д.
///   Пусть надо найти определённый текст во всех файлах некоторого каталога, при чём искать только в тех, где первые 1000 байт валидные UTF-8 (чтоб сузить область поиска)
///   Надо запустить из моей программы gdb и подключить к ней же
/// http://habrahabr.ru/company/infopulse/blog/227529/ - Скотт Майерс (автор effc++): "Самая важная фича языка [C++] - это деструкторы"
///   RAII - великая вещь
///   Как минимум следующее нужно обязательно добавить в C: деструкторы и совместимые с ними исключения. Даже в systemd используются своего рода деструкторы
/// "Хуже обобщения одного примера может быть только обобщение вообще без примеров" - Боб Шейфлер и Джим Геттис, принципы построения X Window System. Поэтому я не буду вводить непустые исключения до тех пор, пока я не пойму, что они мне нужны
/// Можно обрабатывать ошибки и вручную. Например:
///   int fd = open (...);
///   if (fd == -1) switch (errno)
///     {
///       case ENOENT: ...
///       default: sh_x_throw (...);
///     }
/// Если писать ошибки в один файл/fd из разных потоков или процессов, вывод может перемешаться
/// Существуют такие ситуации: сгенерить файл b на основе a, удалить a, сгенерить c на основе b, удалить b. Записать это правильно с помощью libsh не получится, придётся удалять a в самом конце, хотя он перестал быть нужным раньше. Скорее всего, это фундаментальный exception design bug, он вроде как есть и в C++, и пофиксить его можно только если в языке есть возможность явно вызвать деструктор в середине блока без последующего автоматического вызова этого деструктора в конце блока (и такая возможность вроде есть в Rust)
/// libsh названа так, потому что приближает программирование на C к простоте программирования на shell. Функции-обёртки начинаются с x по аналогии с функциями вроде xmalloc из gnulib
/// Все функции, юзающие libsh-исключения, пихаются в libsh
/// safe_fork устроен именно так, так как: внешние ресурсы не надо деструктуировать дважды, а внутренние всё равно будут деструктуированы при exit (SOMEDAY: но ведь это не единственное, что можно сделать в такой ситуации?)

/// Каждый файл здесь include'ит всё, что ему надо, даже если это уже за'include'ено косвенно. Исключение: если мы инклудим funcs.h, то мы знаем, что он include'ит типы и константы и опираемся на это

//@ #ifndef _SH_EX_H
//@ #define _SH_EX_H
//@
//@ /// It is bad idea to use libsh in C++ (mixing libsh's exceptions with C++'s ones), but I provide this code anyway:
//@ #ifdef __cplusplus
//@ extern "C"
//@ {
//@ #endif
//@
//@ #cmakedefine SH_HAVE_sigsetjmp
//@
//@ #include <stdio.h>
//@ #include <stdlib.h>
//@ #include <stdarg.h>
//@ #include <setjmp.h>
//@

#include <string.h>
#include <errno.h>

#include "ex.h"

//@ /// SH_NORETURN нужно ставить перед static, т. к. clang++ 3.5.0 понимает [[noreturn]] только там
//@ // http://www.open-std.org/jtc1/sc22/wg14/www/docs/n1453.htm
//@ #if _MSC_VER >= 1310
//@ // MS Visual Studio 2003/.NET Framework 1.1 or newer
//@ #define SH_NORETURN _declspec (noreturn)
//@ #elif __GNUC__ > 2 || (__GNUC__ == 2 && __GNUC_MINOR__ >= 5)
//@ // GCC 2.5 or newer
//@ #define SH_NORETURN __attribute__ ((noreturn))
//@ #elif __cplusplus >= 201103L
//@ // 201103 = the value specified in C++11 16.8
//@ #define SH_NORETURN [[noreturn]]
//@ #elif __STDC_VERSION__ >= 201112L
//@ // The value specified in C11 6.10.8
//@ #define SH_NORETURN _Noreturn
//@ #else
//@ #define SH_NORETURN // Nothing
//@ #endif
//@
//@ #if defined (_MSC_VER) // Supported in Visual Studio 2005, so I think just "defined (_MSC_VER)" will go
//@ #define SH_RESTRICT __restrict
//@ #elif defined (__GNUC__) // I hope __restrict available in all modern gcc versions
//@ #define SH_RESTRICT __restrict
//@ #elif __STDC_VERSION__ >= 199901L
//@ #define SH_RESTRICT restrict
//@ #else
//@ #define SH_RESTRICT // Nothing
//@ #endif
//@
//@ /// sh_bool может быть ABI-несовместим с C _Bool и C++ bool (а они могут быть ABI-несовместимы друг с другом), но C sh_bool ABI-совместим с C++ sh_bool, потому что это просто enum. Любое отличное от нуля значение означает sh_true
//@ typedef enum {sh_false = 0, sh_true = 1} sh_bool;
//@
//@ #ifdef SH_HAVE_sigsetjmp
//@ # define _SH_JMP_BUF sigjmp_buf
//@ # define _SH_SETJMP(env) sigsetjmp (env, 1)
//@ # define _SH_LONGJMP(env) siglongjmp (env, 1)
//@ #else
//@ # define _SH_JMP_BUF jmp_buf
//@ # define _SH_SETJMP(env) setjmp (env)
//@ # define _SH_LONGJMP(env) longjmp (env, 1)
//@ #endif
//@
//@ #define _SH_VOID_V(format, call) \
//@   va_list ap; \
//@   va_start (ap, format); \
//@   (call); \
//@   va_end (ap)
//@
//@ #define _SH_V(format, type, call) \
//@   va_list ap; \
//@   va_start (ap, format); \
//@   type result = (call); \
//@   va_end (ap); \
//@   \
//@   return result
//@
//@ #define _SH_STRTO(type, func, args) \
//@   int saved_errno = errno; \
//@   \
//@   errno = 0; \
//@   \
//@   type result = func args; \
//@   \
//@   if (errno != 0) \
//@     { \
//@       sh_throw (#func); \
//@     } \
//@   \
//@   errno = saved_errno; \
//@   \
//@   return result

//@ extern _SH_JMP_BUF *_sh_buf;
//@
_SH_JMP_BUF *_sh_buf = NULL;

//@ typedef /* noreturn */ void (*sh_terminate_t) (void);
//@

SH_NORETURN void //@
sh_exit_failure (void)//@;
{
  exit (EXIT_FAILURE);
}

//@ extern sh_terminate_t _sh_terminate;
//@
sh_terminate_t _sh_terminate = &sh_exit_failure;

sh_terminate_t //@
sh_get_terminate (void)//@;
{
  return _sh_terminate;
}

void //@
sh_set_terminate (sh_terminate_t terminate)//@;
{
  _sh_terminate = terminate;
}

// SOMEDAY: сделать SH_THROW не макросом, а просто функцией?

//@ // Правила написания SH_CTRY и тому подобных макросов. Весь блок SH_CTRY-SH_CATCH нельзя заключать в do-while, иначе break будет работать неправильно. В конце открывающего макроса нужно поставить if (1), чтобы содержимое блока SH_CTRY-SH_CATCH представляло собой в точности один оператор (statement). В начале закрывающего макроса нужно написать else с той же целью. В конце закрывающего макроса нужно поставить do-while, чтобы пользователь макроса поставил после него точку с запятой. Весь блок SH_CTRY-SH_CATCH-SH_CEND должен представлять собой один оператор (statement). Нигде нельзя использовать одну точку с запятой вместо составного оператора, например в else или do-while, т. к. g++ 4.9.2 с опцией -Wempty-body ругается на это
//@
//@ #define SH_THROW \
//@   do \
//@     { \
//@       if (_sh_buf == NULL) \
//@         { \
//@           (*_sh_terminate) (); \
//@           abort (); \
//@         } \
//@       else \
//@         { \
//@           _SH_LONGJMP (*_sh_buf); \
//@         } \
//@     } \
//@   while (0)
//@
//@ /// Из SH_CATCH можно делать break, continue, goto и return, из SH_CTRY, SH_FTRY, SH_FINALLY - нельзя
//@ /// После SH_CEND и SH_FEND нужно ставить ";", SH_CTRY и SH_FTRY можно использовать как единственный оператор в if
//@ /// Пример:
//@ ///   SH_CTRY
//@ ///     {
//@ ///       ...
//@ ///     }
//@ ///   SH_CATCH
//@ ///     {
//@ ///       ...
//@ ///     }
//@ ///   SH_CEND;
//@ /// После SH_CTRY, SH_CATCH, SH_FTRY, SH_FINALLY должен быть ровно один оператор, можно без фигурных скобок
//@
//@ /// В SH_CATCH не видны переменные из SH_CTRY (то же для SH_FINALLY), даже если писать без фигурных скобок. Это сделано специально. Если вы написали такой код:
//@ /// // Плохой код!
//@ /// SH_FTRY
//@ ///   {
//@ ///     int fd = sh_x_creat ("file", 0666);
//@ ///     sh_repeat_write (fd, "hello\n", 6);
//@ ///   }
//@ /// SH_FINALLY
//@ ///   {
//@ ///     sh_x_close (fd);
//@ ///   }
//@ /// SH_FEND;
//@ /// то он не скомлилируется, т. к. переменная fd не видна в SH_FINALLY. И правильно, т. к. sh_x_creat должен был быть снаружи SH_FTRY
//@
//@ #define SH_CTRY \
//@   if (1) \
//@     { \
//@       _SH_JMP_BUF *_sh_saved = _sh_buf; \
//@       _SH_JMP_BUF _sh_this_buf; \
//@       _sh_buf = &_sh_this_buf; \
//@       if (_SH_SETJMP (_sh_this_buf) == 0) \
//@         { \
//@           if (1)
//@
//@ #define SH_CATCH \
//@           else \
//@             { \
//@             } \
//@           _sh_buf = _sh_saved; \
//@         } \
//@       else \
//@         { \
//@           _sh_buf = _sh_saved; \
//@           if (1)
//@
//@ #define SH_CEND \
//@           else \
//@             { \
//@             } \
//@         } \
//@     } \
//@   else \
//@     do \
//@       { \
//@       } \
//@     while (0)
//@
//@ #define SH_FTRY \
//@   if (1) \
//@     { \
//@       sh_bool _sh_thrown; \
//@       SH_CTRY \
//@         { \
//@           if (1)
//@
//@ #define SH_FINALLY \
//@           else \
//@             { \
//@             } \
//@           _sh_thrown = sh_false; \
//@         } \
//@       SH_CATCH \
//@         { \
//@           _sh_thrown = sh_true; \
//@         } \
//@       SH_CEND; \
//@       if (1)
//@
//@ #define SH_FEND \
//@       else \
//@         { \
//@         } \
//@       if (_sh_thrown) \
//@         { \
//@           SH_THROW; \
//@         } \
//@     } \
//@   else \
//@     do \
//@       { \
//@       } \
//@     while (0)
//@

//@ /// sh_get_program/sh_set_program даёт portable way to manage argv[0]
const char *_sh_program = "N/A";

const char * //@
sh_get_program (void)//@;
{
  return _sh_program;
}

//@ /// May throw (and sh_init, too)
void //@
sh_set_program (const char *program)//@;
{
  if (program == NULL)
    {
      sh_throwx ("sh_set_program: NULL; it seems a NULL argv[0] was passed through an exec system call");
    }

  _sh_program = program;
}

FILE *_sh_out = NULL;
FILE *_sh_err = NULL;

FILE * //@
sh_get_out (void)//@;
{
  return _sh_out;
}

void //@
sh_set_out (FILE *out)//@;
{
  _sh_out = out;
}

FILE * //@
sh_get_err (void)//@;
{
  return _sh_err;
}

void //@
sh_set_err (FILE *err)//@;
{
  _sh_err = err;
}

//@ /// sh_warn нужен, например, для такой ситуации:
//@ /// {
//@ ///   sh_warn (...);
//@ ///   clean-up, который может изменить errno
//@ ///   SH_THROW;
//@ /// }

void //@
sh_default_vwarnx (const char *format, va_list ap)//@;
{
  if (sh_get_out () != NULL)
    {
      fflush (sh_get_out ());
    }

  if (sh_get_err () != NULL)
    {
      fprintf (sh_get_err (), "%s: ", sh_get_program ());

      if (format == NULL)
        {
          fprintf (sh_get_err (), "error\n");
        }
      else
        {
          vfprintf (sh_get_err (), format, ap);
          fprintf (sh_get_err (), "\n");
        }

      // POSIX гарантирует, что stderr не является fully buffered. Тем не менее мы fflush'им _err здесь и в других подобных местах, т. к. _err не обязательно stderr
      fflush (sh_get_err ());
    }
}

void //@
sh_default_vwarn (const char *format, va_list ap)//@;
{
  int saved_errno = errno;

  if (sh_get_out () != NULL)
    {
      fflush (sh_get_out ());
    }

  if (sh_get_err () != NULL)
    {
      fprintf (sh_get_err (), "%s: ", sh_get_program ());

      if (format != NULL)
        {
          vfprintf (sh_get_err (), format, ap);
          fprintf (sh_get_err (), ": ");
        }

      // SOMEDAY: strerror is not thread-safe
      fprintf (sh_get_err (), "%s\n", strerror (saved_errno));
      fflush (sh_get_err ());
    }
}

//@ typedef void (*sh_vwarn_t) (const char *format, va_list ap);

sh_vwarn_t _sh_vwarnx = &sh_default_vwarnx;
sh_vwarn_t _sh_vwarn = &sh_default_vwarn;

sh_vwarn_t //@
sh_get_vwarnx (void)//@;
{
  return _sh_vwarnx;
}

sh_vwarn_t //@
sh_get_vwarn (void)//@;
{
  return _sh_vwarn;
}

void //@
sh_set_vwarnx (sh_vwarn_t f)//@;
{
  _sh_vwarnx = f;
}

void //@
sh_set_vwarn (sh_vwarn_t f)//@;
{
  _sh_vwarn = f;
}

void //@
sh_vwarnx (const char *format, va_list ap)//@;
{
  (*_sh_vwarnx) (format, ap);
}

void //@
sh_vwarn (const char *format, va_list ap)//@;
{
  (*_sh_vwarn) (format, ap);
}

SH_NORETURN void //@
sh_vthrowx (const char *format, va_list ap)//@;
{
  sh_vwarnx (format, ap);
  SH_THROW;
}

SH_NORETURN void //@
sh_vthrow (const char *format, va_list ap)//@;
{
  sh_vwarn (format, ap);
  SH_THROW;
}

void //@
sh_warnx (const char *format, ...)//@;
{
  _SH_VOID_V (format, sh_vwarnx (format, ap));
}

void //@
sh_warn (const char *format, ...)//@;
{
  _SH_VOID_V (format, sh_vwarn (format, ap));
}

SH_NORETURN void //@
sh_throwx (const char *format, ...)//@;
{
  _SH_VOID_V (format, sh_vthrowx (format, ap));
}

SH_NORETURN void //@
sh_throw (const char *format, ...)//@;
{
  _SH_VOID_V (format, sh_vthrow (format, ap));
}

void //@
sh_reset (void)//@;
{
  _sh_buf = NULL;
}

void //@
sh_init (const char *program)//@;
{
  sh_set_out (stdout);
  sh_set_err (stderr);
  sh_set_program (program); // This call should be last, because it may throw
}

//@
//@ #ifdef __cplusplus
//@ }
//@ #endif
//@
//@ #endif // ! _SH_EX_H
