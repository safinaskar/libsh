// Делаем "#if defined" на каждую используемую внешнюю функцию, кроме C89 (иногда делаем даже для C89 для единообразия)

//@ #ifndef _SH_ETC_H
//@ #define _SH_ETC_H
//@
//@ #ifdef __cplusplus
//@ extern "C"
//@ {
//@ #endif
//@
//@ #include <stdio.h>
//@
//@ #include "ex.h"
//@ #include "funcs.h"
//@

// Чтобы быть уверенным, что, например, stdio.h объявит ssize_t, нужный для sh_getdelim_no_delim
#define _POSIX_C_SOURCE 200809L

// Для strcasestr
#define _GNU_SOURCE 1

#include <errno.h>

#include "etc.h"

//@ /// ---- nointr ----

#if defined (SH_HAVE_select) //@
//@ /// Может не вычислять правильно оставшееся время
int //@
sh_select_nointr (int nfds, fd_set *SH_RESTRICT readfds, fd_set *SH_RESTRICT writefds, fd_set *SH_RESTRICT errorfds, struct timeval *SH_RESTRICT timeout)//@;
{
  int result;

  for (;;)
    {
      result = select (nfds, readfds, writefds, errorfds, timeout);

      if (result == -1 && errno == EINTR)
        ;
      else
        {
          break;
        }
    }

  return result;
}

int //@
sh_x_select (int nfds, fd_set *SH_RESTRICT readfds, fd_set *SH_RESTRICT writefds, fd_set *SH_RESTRICT errorfds, struct timeval *SH_RESTRICT timeout)//@;
{
  int result = sh_select_nointr (nfds, readfds, writefds, errorfds, timeout);

  if (result == -1)
    {
      sh_throw ("select");
    }

  return result;
}
#endif //@

#if defined (SH_HAVE_pselect) //@
//@ /// Не вычисляет правильно оставшееся время
int //@
sh_pselect_nointr (int nfds, fd_set *SH_RESTRICT readfds, fd_set *SH_RESTRICT writefds, fd_set *SH_RESTRICT errorfds, const struct timespec *SH_RESTRICT timeout, const sigset_t *SH_RESTRICT sigmask)//@;
{
  int result;

  for (;;)
    {
      result = pselect (nfds, readfds, writefds, errorfds, timeout, sigmask);

      if (result == -1 && errno == EINTR)
        ;
      else
        {
          break;
        }
    }

  return result;
}

int //@
sh_x_pselect (int nfds, fd_set *SH_RESTRICT readfds, fd_set *SH_RESTRICT writefds, fd_set *SH_RESTRICT errorfds, const struct timespec *SH_RESTRICT timeout, const sigset_t *SH_RESTRICT sigmask)//@;
{
  int result = sh_pselect_nointr (nfds, readfds, writefds, errorfds, timeout, sigmask);

  if (result == -1)
    {
      sh_throw ("pselect");
    }

  return result;
}
#endif //@

#if defined (SH_HAVE_poll) //@
//@ /// Не вычисляет правильно оставшееся время
int //@
sh_poll_nointr (struct pollfd fds[], nfds_t nfds, int timeout)//@;
{
  int result;

  for (;;)
    {
      result = poll (fds, nfds, timeout);

      if (result == -1 && errno == EINTR)
        ;
      else
        {
          break;
        }
    }

  return result;
}

int //@
sh_x_poll (struct pollfd fds[], nfds_t nfds, int timeout)//@;
{
  int result = sh_poll_nointr (fds, nfds, timeout);

  if (result == -1)
    {
      sh_throw ("poll");
    }

  return result;
}
#endif //@

#if defined (SH_HAVE_sleep) //@
#include <unistd.h>
void //@
sh_sleep_nointr (unsigned seconds)//@;
{
  while (seconds != 0)
    {
      seconds = sleep (seconds);
    }
}
#endif //@

#if defined (SH_HAVE_nanosleep) //@
//@ /// sh_nanosleep_nointr, sh_x_nanosleep, sh_clock_nanosleep_nointr, sh_x_clock_nanosleep не имеют последнего аргумента, это исключение из правил libsh
int //@
sh_nanosleep_nointr (const struct timespec *rqtp)//@;
{
  struct timespec rqt = *rqtp;
  int result;

  for (;;)
    {
      result = nanosleep (&rqt, &rqt);

      if (result == -1 && errno == EINTR)
        ;
      else
        {
          break;
        }
    }

  return result;
}

void //@
sh_x_nanosleep (const struct timespec *rqtp)//@;
{
  if (sh_nanosleep_nointr (rqtp) == -1)
    {
      sh_throw ("nanosleep");
    }
}
#endif //@

#if defined (SH_HAVE_clock_nanosleep) //@
int //@
sh_clock_nanosleep_nointr (clockid_t clock_id, int flags, const struct timespec *rqtp)//@;
{
  struct timespec rqt = *rqtp;
  int result;

  for (;;)
    {
      result = clock_nanosleep (clock_id, flags, &rqt, &rqt);

      if (result == EINTR)
        ;
      else
        {
          break;
        }
    }

  return result;
}

//@ /// Пишет в errno
void //@
sh_x_clock_nanosleep (clockid_t clock_id, int flags, const struct timespec *rqtp)//@;
{
  int result = sh_clock_nanosleep_nointr (clock_id, flags, rqtp);

  if (result != 0)
    {
      errno = result;
      sh_throw ("clock_nanosleep");
    }
}
#endif //@

//@ /// ---- accept ----

#if defined (SH_HAVE_accept) //@
#include <errno.h>

#include <sys/socket.h>

int //@
sh_repeat_accept (int socket, struct sockaddr *SH_RESTRICT address, socklen_t *SH_RESTRICT address_len)//@;
{
  for (;;)
    {
      int result = accept (socket, address, address_len);

      if (result != -1)
        {
          return result;
        }

      if (errno != EWOULDBLOCK && errno != ECONNABORTED && errno != EPROTO && errno != EINTR)
        {
          return -1;
        }
    }
}

int //@
sh_x_accept (int socket, struct sockaddr *SH_RESTRICT address, socklen_t *SH_RESTRICT address_len)//@;
{
  int result = sh_repeat_accept (socket, address, address_len);

  if (result == -1)
    {
      sh_throw ("accept");
    }

  return result;
}
#endif //@

//@ /// ---- external ----

//@ #if defined (SH_HAVE_execl) && defined (SH_HAVE_execle) && defined (SH_HAVE_execlp)
//@ # include <unistd.h>
//@ /// В случае успеха все exec-функции не возвращают, path/file вычисляется дважды
//@ # define sh_x_execl(path,  ...) (execl  (path, __VA_ARGS__), sh_throw ("execl: %s",  path))
//@ # define sh_x_execle(path, ...) (execle (path, __VA_ARGS__), sh_throw ("execle: %s", path))
//@ # define sh_x_execlp(file, ...) (execlp (file, __VA_ARGS__), sh_throw ("execlp: %s", file))
//@ #endif
//@

#if defined (SH_HAVE_fcntl) //@
//@ #include <fcntl.h>
int //@
_sh_after_fcntl (int result)//@;
{
  if (result == -1)
    {
      sh_throw ("fcntl");
    }

  return result;
}
//@ # define sh_x_fcntl(fildes, ...) _sh_after_fcntl (fcntl (fildes, __VA_ARGS__))
#endif //@

//@ /// ---- xx without x ----

#include <string.h>
//@ /// x_memchr нет, есть HAVE, то же для strchr, strstr, strcasestr
void * //@
sh_xx_memchr (const void *s, int c, size_t n)//@;
{
  void *result = memchr (s, c, n);

  if (result == NULL)
    {
      sh_throwx ("memchr: byte not found");
    }

  return result;
}

#include <string.h>
char * //@
sh_xx_strchr (const char *s, int c)//@;
{
  char *result = strchr (s, c);

  if (result == NULL)
    {
      sh_throwx ("strchr: character not found");
    }

  return result;
}

#include <string.h>
char * //@
sh_xx_strstr (const char *s1 /* haystack */, const char *s2 /* needle */)//@;
{
  char *result = strstr (s1, s2);

  if (result == NULL)
    {
      sh_throwx ("strstr: substring not found");
    }

  return result;
}

#if defined (SH_HAVE_strcasestr)
#include <string.h>
char * //@
sh_xx_strcasestr (const char *haystack, const char *needle)//@;
{
  char *result = strcasestr (haystack, needle);

  if (result == NULL)
    {
      sh_throwx ("strcasestr: substring not found");
    }

  return result;
}
#endif

//@ /// ---- xx ----

int //@
sh_xx_vsnprintf (char *SH_RESTRICT s, size_t n, const char *SH_RESTRICT format, va_list ap)//@;
{
  int result = sh_x_vsnprintf (s, n, format, ap);

  if ((unsigned) result >= n)
    {
      sh_throwx ("vsnprintf: truncated");
    }

  return result;
}

int //@
sh_xx_snprintf (char *SH_RESTRICT s, size_t n, const char *SH_RESTRICT format, ...)//@;
{
  _SH_V (format, int, sh_xx_vsnprintf (s, n, format, ap));
}

char //@
sh_xx_fgetc (FILE *stream)//@;
{
  int result = sh_x_fgetc (stream);

  if (result == EOF)
    {
      sh_throwx ("fgetc: end of file");
    }

  return (char) result;
}

char //@
sh_xx_getc (FILE *stream)//@;
{
  return sh_xx_fgetc (stream);
}

char //@
sh_xx_getchar (void)//@;
{
  return sh_xx_fgetc (stdin);
}

size_t //@
sh_xx_fread (void *SH_RESTRICT ptr, size_t size, size_t nitems, FILE *SH_RESTRICT stream)//@;
{
  if (sh_x_fread (ptr, size, nitems, stream) != nitems)
    {
      sh_throwx ("fread: end of file");
    }

  return nitems;
}

#include <ctype.h>

#define _SH_XX_STRTO(str, func, type, args) \
  if (str[0] == '\0') \
    { \
      sh_throwx (#func ": empty string"); \
    } \
  \
  if (isspace (str[0])) \
    { \
      sh_throwx (#func ": white-space at start"); \
    } \
  \
  char *end; \
  type result = sh_x_ ## func args; \
  \
  if (end[0] != '\0') \
    { \
      sh_throwx (#func ": some garbage after number"); \
    } \
  \
  return result

#if defined (SH_HAVE_strtod) //@
double //@
sh_xx_strtod (const char *nptr)//@;
{
  _SH_XX_STRTO (nptr, strtod, double, (nptr, &end));
}
#endif //@

#if defined (SH_HAVE_strtof) //@
float //@
sh_xx_strtof (const char *nptr)//@;
{
  _SH_XX_STRTO (nptr, strtof, float, (nptr, &end));
}
#endif //@

#if defined (SH_HAVE_strtold) //@
long double //@
sh_xx_strtold (const char *nptr)//@;
{
  _SH_XX_STRTO (nptr, strtold, long double, (nptr, &end));
}
#endif //@

#if defined (SH_HAVE_strtoimax) //@
intmax_t //@
sh_xx_strtoimax (const char *nptr, int base)//@;
{
  _SH_XX_STRTO (nptr, strtoimax, intmax_t, (nptr, &end, base));
}
#endif //@

#if defined (SH_HAVE_strtoumax) //@
uintmax_t //@
sh_xx_strtoumax (const char *nptr, int base)//@;
{
  _SH_XX_STRTO (nptr, strtoumax, uintmax_t, (nptr, &end, base));
}
#endif //@

#if defined (SH_HAVE_strtol) //@
long //@
sh_xx_strtol (const char *str, int base)//@;
{
  _SH_XX_STRTO (str, strtol, long, (str, &end, base));
}
#endif //@

#if defined (SH_HAVE_strtoll) //@
long long //@
sh_xx_strtoll (const char *str, int base)//@;
{
  _SH_XX_STRTO (str, strtoll, long long, (str, &end, base));
}
#endif //@

#if defined (SH_HAVE_strtoul) //@
unsigned long //@
sh_xx_strtoul (const char *str, int base)//@;
{
  _SH_XX_STRTO (str, strtoul, unsigned long, (str, &end, base));
}
#endif //@

#if defined (SH_HAVE_strtoull) //@
unsigned long long //@
sh_xx_strtoull (const char *str, int base)//@;
{
  _SH_XX_STRTO (str, strtoull, unsigned long long, (str, &end, base));
}
#endif //@

//@ /// ---- Высокоуровневые функции ----

#define _SH_CONV(min, max, func) \
  if (x < min || x > max) \
    { \
      sh_throwx (func ": out of range"); \
    } \
  \
  return x

#include <limits.h>
#include <stdint.h>

int //@
sh_long2int (long x)//@;
{
  _SH_CONV (INT_MIN, INT_MAX, "sh_long2int");
}

size_t //@
sh_umax2size_t (uintmax_t x)//@;
{
  _SH_CONV (0, SIZE_MAX, "sh_umax2size_t");
}

// Размер буфера для чтения и записи. 1 MiB, т. к. это быстрее, чем, скажем, 16 KiB, проверено на опыте. В то же время значения больше 1 MiB могут привести к появлению бага 12309 ядра Linux
static const size_t rw_buf_size = 1024 * 1024;

#if defined (SH_HAVE_read) //@
#include <stdlib.h>

// SOMEDAY: возможно, выделять память по-другому, например, увеличивая её экспоненциально
size_t //@
sh_read_all (int fildes, void **bufptr)//@;
{
  size_t result = 0;

  *bufptr = sh_x_malloc (rw_buf_size);

  SH_CTRY
    {
      for (;;)
        {
          size_t read_returned = (size_t) sh_x_read (fildes, (char *) *bufptr + result, rw_buf_size);

          if (read_returned == 0)
            {
              break;
            }

          result += read_returned;

          *bufptr = sh_x_realloc (*bufptr, result + rw_buf_size);
        }
    }
  SH_CATCH
    {
      free (*bufptr);
      *bufptr = NULL;
      SH_THROW;
    }
  SH_CEND;

  return result;
}

size_t //@
sh_read_all_close (int fildes, void **bufptr)//@;
{
  size_t result;

  SH_FTRY
    {
      result = sh_read_all (fildes, bufptr);
    }
  SH_FINALLY
    {
      sh_x_close (fildes);
    }
  SH_FEND;

  return result;
}
#endif //@

#if defined (SH_HAVE_write) //@
//@ /// write вызывается как минимум один раз, поэтому делается проверка возможности чтения/записи
void //@
sh_repeat_write (int fildes, const void *buf, size_t nbyte)//@;
{
  for (;;)
    {
      ssize_t written = sh_x_write (fildes, buf, nbyte);

      nbyte -= written;

      if (nbyte == 0)
        {
          break;
        }

      buf = (const char *) buf + written;
    }
}
#endif //@

#if defined (SH_HAVE_getdelim) //@
/// libsh работает в основном с fd, но здесь (для семейства функций sh_getdelim_no_delim) я делаю исключение, т. к. иначе пришлось бы читать по одному символу
ssize_t //@
sh_getdelim_no_delim (char **SH_RESTRICT lineptr, size_t *SH_RESTRICT n, int delimiter, FILE *SH_RESTRICT stream)//@;
{
  ssize_t result = sh_x_getdelim (lineptr, n, delimiter, stream);

  if (result != -1 && (unsigned char) ((*lineptr)[result - 1]) == (unsigned char) delimiter)
    {
      (*lineptr)[result - 1] = '\0';
      --result;
    }

  return result;
}

//@ /// Будьте осторожны с '\0'-байтами внутри строк в sh_getdelim_one_shot, sh_getdelim_fclose, sh_getline_one_shot и sh_getline_fclose
char * //@
sh_getdelim_one_shot (int delimiter, FILE *stream)//@;
{
  char *line = NULL;
  size_t n = 0;

  if (sh_getdelim_no_delim (&line, &n, delimiter, stream) == -1)
    {
      sh_throwx ("getdelim: end of file");
    }

  return line;
}

char * //@
sh_getdelim_fclose (int delimiter, FILE *stream)//@;
{
  char *result;

  SH_FTRY
    {
      result = sh_getdelim_one_shot (delimiter, stream);
    }
  SH_FINALLY
    {
      sh_x_fclose (stream);
    }
  SH_FEND;

  return result;
}

ssize_t //@
sh_getline_no_delim (char **SH_RESTRICT lineptr, size_t *SH_RESTRICT n, FILE *SH_RESTRICT stream)//@;
{
  return sh_getdelim_no_delim (lineptr, n, '\n', stream);
}

char * //@
sh_getline_one_shot (FILE *stream)//@;
{
  return sh_getdelim_one_shot ('\n', stream);
}

char * //@
sh_getline_fclose (FILE *stream)//@;
{
  return sh_getdelim_fclose ('\n', stream);
}
#endif //@

#if defined (SH_HAVE_waitpid) //@
//@ /// запускать с реальным положительным pid и без WNOHANG
int //@
sh_waitpid_status (pid_t pid, int options)//@;
{
  int status;

  sh_x_waitpid (pid, &status, options);

  return status;
}
#endif //@

#if defined (SH_HAVE_fork) //@
//@ /// Делайте так же для каждой функции, копирующей поток (clone, pthread_create, fork, vfork и т. д.)
pid_t //@
sh_safe_fork (void)//@;
{
  pid_t result = sh_x_fork ();

  if (result == 0)
    {
      sh_reset ();
      sh_set_terminate (&sh_exit_failure);
    }

  return result;
}
#endif //@

#if defined (SH_HAVE_wait) && defined (SH_HAVE_strsignal) //@
#include <string.h>
int //@
sh_code (int status)//@;
{
  if (WIFEXITED (status))
    {
      return WEXITSTATUS (status);
    }
  else if (WIFSIGNALED (status))
    {
      // SOMEDAY: strsignal is not thread-safe
      sh_throwx ("child process killed by signal: %s", strsignal (WTERMSIG (status)));
    }
  else if (WIFSTOPPED (status))
    {
      sh_throwx ("child process stopped by signal: %s", strsignal (WSTOPSIG (status)));
    }
#ifdef WIFCONTINUED
  else if (WIFCONTINUED (status))
    {
      sh_throwx ("child process continued");
    }
#endif
  else
    {
      // POSIX 2013 edition:
      // There may be additional implementation-defined circumstances under which wait() or waitpid()
      // report status. This shall not occur unless the calling process or one of its child processes
      // explicitly makes use of a non-standard extension. In these cases the interpretation of the
      // reported status is implementation-defined.

      sh_throwx ("child process terminated using method not specified in POSIX 2013 edition");
    }
}

#include <stdlib.h>
void //@
sh_success (int status)//@;
{
  int code = sh_code (status);

  if (code != EXIT_SUCCESS)
    {
      sh_throwx ("child process terminated with exit code %d", code);
    }
}
#endif //@

#if defined (SH_HAVE_close) && defined (SH_HAVE_shutdown) && defined (SH_HAVE_poll) && defined (SH_HAVE_read) && defined (SH_HAVE_write) //@
#include <stdlib.h>

#include <sys/socket.h>
#include <unistd.h>
#include <poll.h>

//@ struct sh_multicat_t
//@ {
//@   int src;
//@   int dst;
//@   int flags;
//@ };

//@ /// При EOF fd всегда исключается из working set, т. е. sh_multicat не продолжает читать из него дальше. Есть ситуации, когда продолжение чтения возможно, например, если это терминал. Но я не могу придумать use case'а, когда такое чтение (и игнорирование EOF'а!) было бы нужно
//@ /// При EOF'е я не всегда закрываю input fd, т. к. это может быть терминал. И я не всегда закрываю output fd, т. к. в него просто можно писать дальше, например, сделать туда ещё один sh_multicat
//@ /// sh_wshutdownw и sh_done, т. к. netcat in mind. Но даже в самом гибком netcat'е я не могу придумать use case'ы на остальные комбинации, т. е. регекс sh_[rw]shutdown[rw]
//@ /// нет rshutdownr, т. к. EOF на сокете - это уже rshutdownr
//@ /// Вообще, эти действия придуманы вот для чего:
//@ /// * Для более краткой записи
//@ /// * Чтобы закрыть fd сразу же, как только он перестал быть нужен (чтобы можно было отмонтировать fs и т. д.)
//@ /// * Чтобы можно было реализовать корректный netcat, который посылает FIN сразу же после EOF'а на stdin и закрывает stdout сразу же после FIN
//@ /// * Если не закрывать сразу же, это может привести к deadlock'ам. Например:
//@ ///     sh_multicat ({{0, 3, sh_rclose | sh_wclose}, {4, 1, sh_rclose | sh_wclose}}); // Псевдокод, предположим, что 3 и 4 - это концы одного пайпа
//@ ///   Тогда, если бы не было sh_wclose, был бы deadlock
//@ static const int sh_rclose     = 1 << 0;
//@ static const int sh_wshutdownw = 1 << 1;
//@ static const int sh_wclose     = 1 << 2;
//@ static const int sh_done       = 1 << 3;

static sh_bool
multicat_done (const struct sh_multicat_t *what)
{
  sh_bool result = sh_true;

  // Сперва должны быть все shutdown'ы, а потом все close'ы, т. к. это может быть один fd

  if (what->flags & sh_wshutdownw)
    {
      SH_CTRY
        {
          sh_x_shutdown (what->dst, SHUT_WR);
        }
      SH_CATCH
        {
          result = sh_false;
        }
      SH_CEND;
    }

  if (what->flags & sh_rclose)
    {
      SH_CTRY
        {
          sh_x_close (what->src);
        }
      SH_CATCH
        {
          result = sh_false;
        }
      SH_CEND;
    }

  if (what->flags & sh_wclose)
    {
      SH_CTRY
        {
          sh_x_close (what->dst);
        }
      SH_CATCH
        {
          result = sh_false;
        }
      SH_CEND;
    }

  return result;
}

//@ /// Это НЕ параллельный cat, т. к. всё происходит в одном потоке и синхронно (но с мультиплексированием I/O)
//@ /// sh_multicat modifies 'what'
//@ /// size >= 0
//@ /// Любые fd могут быть одинаковыми с оговоркой: после закрывания fd его нельзя использовать, т. е. нельзя {{3, 3, sh_rclose | sh_wclose}}, нельзя {{3, 4, sh_rclose}, {3, 5, 0}}
//@ /// sh_multicat нужен в том числе для правильной реализации "cat file - | prog"
//@ /// sh_multicat применяет действия в любом случае
//@ /// sh_multicat всегда завершается в случае любой ошибки, т. к. нет контрюзекейса
//@ /// Данные могут потеряться, т. е. прочитали, но не записали
//@ /// sh_multicat делает poll только на чтение и ошибки, но не на запись. Т. е. write may block
//@ /// sh_multicat обычно не лочится в read и поэтому сразу же чувствует ошибки write
//@ /// sh_multicat позволяет избежать получения SIGPIPE в большинстве случаев. Но SIGPIPE всё же возможен, если пайп закрылся между poll и write
//@ /// Мой cat круче вашего. В моём случае в "cat | prog" cat сможет сдетектить завершение prog и сразу же завершиться
//@ /// Инициализируйте sh_multicat_t нулями перед передачей в sh_multicat, т. к. могут быть добавлены новые поля. Инициализировать инициализатором можно, т. к. порядок полей будет неизменным
// Не удалось доказать экспериментом, что системные вызовы Linux sendfile и splice быстрее обычного read/write
void //@
sh_multicat (struct sh_multicat_t what[], int size)//@;
{
  int working = size;

  SH_FTRY // what
    {
      struct pollfd *p = (struct pollfd *) sh_x_malloc (2 * size * sizeof (struct pollfd));

      SH_FTRY // p
        {
          // Делаем malloc, т. к. 1 MiB может не поместиться на стеке
          char *buf = (char *) sh_x_malloc (rw_buf_size);

          SH_FTRY // buf
            {
              for (int i = 0; i != size; ++i)
                {
                  p[2 * i].fd = what[i].src;
                  p[2 * i].events = POLLIN;
                  p[2 * i + 1].fd = what[i].dst;
                  p[2 * i + 1].events = 0;
                }

              while (working != 0)
                {
                  // Делаем poll, т. к. select может не работать с большими fd. Есть планы по включению ppoll в POSIX: http://austingroupbugs.net/view.php?id=881
                  sh_x_poll (p, 2 * size, -1);

                  for (int i = 0; i != size; ++i)
                    {
                      // Проверка на POLLHUP идёт раньше проверки на POLLERR, т. к. на GNU/Linux при получении RST on TCP выставляется POLLERR | POLLHUP, и мне хочется узнать о POLLHUP раньше, чтобы выдать более точное сообщение об ошибке

                      if (p[2 * i + 1].revents & POLLHUP)
                        {
                          sh_throwx ("sh_multicat: hang up on an output file descriptor (maybe RST on TCP, for example, peer rejected data we sent)");
                        }

                      if (p[2 * i + 1].revents & POLLERR)
                        {
                          sh_throwx ("sh_multicat: error on an output file descriptor (maybe broken pipe)");
                        }

                      if (p[2 * i + 1].revents & POLLNVAL)
                        {
                          sh_throwx ("sh_multicat: an output file descriptor is invalid");
                        }

                      // Input: unfortunately, if there is no writers to pipe, then this is POLLHUP. So, we assume POLLHUP == POLLIN. For the same reason, just in case, we assume POLLERR and POLLNVAL are equal to POLLIN
                      if (p[2 * i].revents & POLLIN || p[2 * i].revents & POLLERR || p[2 * i].revents & POLLHUP || p[2 * i].revents & POLLNVAL)
                        {
                          size_t got = sh_x_read (what[i].src, buf, rw_buf_size);

                          if (got == 0)
                            {
                              p[2 * i].fd = -1;
                              p[2 * i + 1].fd = -1;
                              --working;

                              if (!multicat_done (&(what[i])))
                                {
                                  what[i].src = -1;
                                  SH_THROW;
                                }

                              what[i].src = -1;

                              if (what[i].flags & sh_done)
                                {
                                  goto done;
                                }

                              continue;
                            }

                          sh_repeat_write (what[i].dst, buf, got);
                        }
                    }
                }

              done: ;
            }
          SH_FINALLY // buf
            {
              free (buf);
            }
          SH_FEND; // buf
        }
      SH_FINALLY // p
        {
          free (p);
        }
      SH_FEND; // p
    }
  SH_FINALLY // what
    {
      if (working != 0)
        {
          sh_bool result = sh_true;

          for (int i = 0; i != size; ++i)
            {
              if (what[i].src != -1)
                {
                  result = result && multicat_done (&(what[i]));
                }
            }

          if (!result)
            {
              SH_THROW;
            }
        }
    }
  SH_FEND; // what
}

//@ /// "cat a b > out" равен следующему коду (не exception-safe):
//@ /// sh_cat (sh_x_open ("b", O_RDONLY), sh_cat (sh_x_open ("a", O_RDONLY), sh_x_creat ("out", 0666), sh_rclose), sh_rclose | sh_wclose)
//@ /// (такой сжатый стиль - особенность C, см. например, i++ и ++i, strcpy)
//@ /// Или следующему exception-safe, который выглядит даже красивее:
//@ /// int out = sh_x_creat ("out", 0666);
//@ /// SH_FTRY
//@ ///   {
//@ ///     sh_cat (sh_x_open ("a", O_RDONLY), out, sh_rclose);
//@ ///     sh_cat (sh_x_open ("b", O_RDONLY), out, sh_rclose);
//@ ///   }
//@ /// SH_FINALLY
//@ ///   {
//@ ///     sh_close (out);
//@ ///   }
//@ /// SH_FEND;
int //@
sh_cat (int src, int dst, int flags)//@;
{
  struct sh_multicat_t what = {0};

  what.src = src;
  what.dst = dst;
  what.flags = flags;

  sh_multicat (&what, 1);

  return dst;
}
#endif //@

#if defined (SH_HAVE_socket) && defined (SH_HAVE_connect) && defined (SH_HAVE_close) && defined (SH_HAVE_getaddrinfo) //@
#include <sys/socket.h>
#include <unistd.h>
#include <netdb.h>

static int
tcp_connect_loop (const char *host, const char *protocol, struct addrinfo *res)
{
  for (; res != NULL; res = res->ai_next)
    {
      int result = socket (res->ai_family, res->ai_socktype, res->ai_protocol);

      if (result != -1)
        {
          if (connect (result, res->ai_addr, res->ai_addrlen) != -1)
            {
              return result;
            }

          close (result);
        }
    }

  // SOMEDAY: печатать больше инфы (то же для tcp_listen_loop)?
  sh_throwx ("sh_tcp_connect: [%s]:%s: all structs returned by getaddrinfo failed", host, protocol);
}

int //@
sh_tcp_connect (const char *host, const char *protocol, int family)//@;
{
  struct addrinfo hints = {0};

  hints.ai_family = family;
  hints.ai_socktype = SOCK_STREAM;

  struct addrinfo *res;

  sh_x_getaddrinfo (host, protocol, &hints, &res);

  // POSIX 2013 says that now 'res' list contains at least one item

  int result;

  SH_FTRY
    {
      result = tcp_connect_loop (host, protocol, res);
    }
  SH_FINALLY
    {
      freeaddrinfo (res);
    }
  SH_FEND;

  return result;
}
#endif //@

#if defined (SH_HAVE_socket) && defined (SH_HAVE_setsockopt) && defined (SH_HAVE_bind) && defined (SH_HAVE_close) && defined (SH_HAVE_getaddrinfo) && defined (SH_HAVE_listen) //@
#include <sys/socket.h>
#include <unistd.h>
#include <netdb.h>

static int
tcp_listen_loop (const char *protocol, struct addrinfo *res)
{
  for (; res != NULL; res = res->ai_next)
    {
      int result = socket (res->ai_family, res->ai_socktype, res->ai_protocol);

      if (result != -1)
        {
          const int on = 1;

          if (setsockopt (result, SOL_SOCKET, SO_REUSEADDR, &on, sizeof (on)) != -1)
            {
              if (bind (result, res->ai_addr, res->ai_addrlen) != -1)
                {
                  return result;
                }
            }

          close (result);
        }
    }

  sh_throwx ("sh_tcp_listen: *:%s: all structs returned by getaddrinfo failed", protocol);
}

int //@
sh_tcp_listen (const char *protocol, int family)//@;
{
  struct addrinfo hints = {0};

  hints.ai_flags = AI_PASSIVE;
  hints.ai_family = family;
  hints.ai_socktype = SOCK_STREAM;

  struct addrinfo *res;

  sh_x_getaddrinfo (NULL, protocol, &hints, &res);

  int result;

  SH_FTRY // res
    {
      result = tcp_listen_loop (protocol, res);

      SH_CTRY // result
        {
          sh_x_listen (result, SOMAXCONN);
        }
      SH_CATCH
        {
          sh_x_close (result);
          SH_THROW;
        }
      SH_CEND;
    }
  SH_FINALLY
    {
      freeaddrinfo (res);
    }
  SH_FEND;

  return result;
}
#endif //@

#if defined (SH_HAVE_accept) && defined (SH_HAVE_close) //@
int //@
sh_tcp_accept_close (int listen_fd)//@;
{
  int result;

  SH_FTRY // listen_fd
    {
      result = sh_x_accept (listen_fd, NULL, NULL);
    }
  SH_FINALLY
    {
      sh_x_close (listen_fd);
    }
  SH_FEND;

  return result;
}
#endif //@

#include <curl/curl.h> //@

void //@
sh_curl_wrapper (CURLcode errornum)//@;
{
  if (errornum != CURLE_OK)
    {
      sh_throwx ("libcurl: %s", curl_easy_strerror (errornum));
    }
}

#if defined (SH_HAVE_isatty) && defined (SH_HAVE_fileno) //@
#include <unistd.h>

static int
progress_callback (void *clientp, curl_off_t dltotal, curl_off_t dlnow, curl_off_t ultotal, curl_off_t ulnow)
{
  if (sh_get_err () != NULL && isatty (sh_x_fileno (sh_get_err ())))
    {
      if (dltotal == 0)
        {
          sh_x_fprintf (sh_get_err (), "\b\b\b\b    \b\b\b\b?%%");
        }
      else
        {
          sh_x_fprintf (sh_get_err (), "\b\b\b\b    \b\b\b\b%d%%", (int) floor (dlnow * 100. / dltotal));
        }

      sh_x_fflush (sh_get_err ());
    }

  return 0;
}

void //@
sh_curl (CURL *handle, const char *uri, FILE *fout)//@;
{
  if (sh_get_err () != NULL)
    {
      sh_x_fprintf (sh_get_err (), "Downloading %s\n", uri);
    }

  sh_curl_wrapper (curl_easy_setopt (handle, CURLOPT_URL, uri));
  sh_curl_wrapper (curl_easy_setopt (handle, CURLOPT_WRITEFUNCTION, NULL));
  sh_curl_wrapper (curl_easy_setopt (handle, CURLOPT_WRITEDATA, (void *) fout));
  sh_curl_wrapper (curl_easy_setopt (handle, CURLOPT_NOPROGRESS, 0L));
  sh_curl_wrapper (curl_easy_setopt (handle, CURLOPT_XFERINFOFUNCTION, &progress_callback));

  sh_curl_wrapper (curl_easy_perform (handle));

  if (sh_get_err () != NULL && isatty (sh_x_fileno (sh_get_err ())))
    {
      sh_x_fprintf (sh_get_err (), "\b\b\b\b    \b\b\b\b");
      sh_x_fflush (sh_get_err ());
    }

  {
    long response_code;

    sh_curl_wrapper (curl_easy_getinfo (handle, CURLINFO_RESPONSE_CODE, &response_code));

    if (response_code != 200)
      {
        sh_throwx ("sh_curl: response code is %ld (200 expected)", response_code);
      }
  }
}

void //@
sh_curl_fclose (CURL *handle, const char *uri, FILE *fout)//@;
{
  SH_FTRY
    {
      sh_curl (handle, uri, fout);
    }
  SH_FINALLY
    {
      sh_x_fclose (fout);
    }
  SH_FEND;
}
#endif //@

#if defined (SH_HAVE_pipe) && defined (SH_HAVE_fork) && defined (SH_HAVE_dup2) && defined (SH_HAVE_close) //@
//@ struct sh_redir
//@ {
//@   int parent;
//@   int child;
//@ };

//@ enum sh_pipe_direction {sh_to_child = 0, sh_from_child};

//@ struct sh_pipe
//@ {
//@   enum sh_pipe_direction direction;
//@   int *parent;
//@   int child;
//@   int _internal_pipe[2];
//@ };

// SOMEDAY: сделать функцию exception-safe
pid_t //@
sh_fork_redirs (const struct sh_redir redirs[], struct sh_pipe pipes[])//@;
{
  struct sh_redir no_redirs[] = {{-1}};
  struct sh_pipe no_pipes[] = {{0}};

  if (redirs == NULL)
    {
      redirs = no_redirs;
    }

  if (pipes == NULL)
    {
      pipes = no_pipes;
    }

  for (int i = 0; pipes[i].parent != NULL; ++i)
    {
      sh_x_pipe (pipes[i]._internal_pipe);
    }

  pid_t result = sh_safe_fork ();

  if (result == 0)
    {
      for (int i = 0; redirs[i].parent != -1; ++i)
        {
          sh_x_dup2 (redirs[i].parent, redirs[i].child);
          sh_x_close (redirs[i].parent);
        }

      for (int i = 0; pipes[i].parent != NULL; ++i)
        {
          if (pipes[i].direction == sh_to_child)
            {
              sh_x_dup2 (pipes[i]._internal_pipe[0], pipes[i].child);
            }
          else
            {
              sh_x_dup2 (pipes[i]._internal_pipe[1], pipes[i].child);
            }

          sh_x_close (pipes[i]._internal_pipe[0]);
          sh_x_close (pipes[i]._internal_pipe[1]);
        }
    }
  else
    {
      for (int i = 0; redirs[i].parent != -1; ++i)
        {
          sh_x_close (redirs[i].parent);
        }

      for (int i = 0; pipes[i].parent != NULL; ++i)
        {
          if (pipes[i].direction == sh_to_child)
            {
              sh_x_close (pipes[i]._internal_pipe[0]);
              *(pipes[i].parent) = pipes[i]._internal_pipe[1];
            }
          else
            {
              sh_x_close (pipes[i]._internal_pipe[1]);
              *(pipes[i].parent) = pipes[i]._internal_pipe[0];
            }
        }
    }

  return result;
}
#endif //@

#if defined (SH_HAVE_pipe) && defined (SH_HAVE_fork) && defined (SH_HAVE_dup2) && defined (SH_HAVE_close) && defined (SH_HAVE_execve) && defined (SH_HAVE_execvp) //@
pid_t //@
sh_spawnve (struct sh_redir redirs[], struct sh_pipe pipes[], const char *path, char *const argv[], char *const envp[])//@;
{
  pid_t result = sh_fork_redirs (redirs, pipes);

  if (result == 0)
    {
      sh_x_execve (path, argv, envp);
    }

  return result;
}

pid_t //@
sh_spawnvp (struct sh_redir redirs[], struct sh_pipe pipes[], const char *command, char *const argv[])//@;
{
  pid_t result = sh_fork_redirs (redirs, pipes);

  if (result == 0)
    {
      sh_x_execvp (command, argv);
    }

  return result;
}
#endif //@

#if defined (SH_HAVE_stat) //@
#include <sys/stat.h>

sh_bool //@
sh_test_d (const char *file)//@;
{
  struct stat buf;

  if (stat (file, &buf) == -1)
    {
      return sh_false;
    }

  return buf.st_mode & S_IFDIR;
}
#endif //@

#if defined (SH_HAVE_stat) && defined (SH_HAVE_mkdir) //@
void //@
sh_force_mkdir (const char *file, mode_t mode)//@;
{
  if (!sh_test_d (file))
    {
      sh_x_mkdir (file, mode);
    }
}
#endif //@

//@
//@ #ifdef __cplusplus
//@ }
//@ #endif
//@
//@ #endif // ! _SH_ETC_H
