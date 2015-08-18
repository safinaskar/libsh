/// не написан генератор типа gengetopt, потому что никто не будет его использовать. использованы лямбды, т. к. по-другому никак нельзя сделать лаконичный парсер (отсюда вытекает c++)
/// sh_init должен быть уже вызван, sh_arg_parse берёт инфу из sh_program
/// написано на c++14, юзайте c++14 при компиляции
/// несколько форм запуска поддерживаются
/// вы должны явно инклудить arg.hpp
/// sh_arg_parse хочет std::string, а не const char *, чтоб удобнее было
/// опции после операндов не учитываются, т. к. этого требует POSIX 2013 и это может пригодиться в программах типа chroot, в отличие от GNU getopt
/// после --help нет \n, пишите несколько метаопций типа --version после --help, потом \n, а потом остальные опции. сами ставьте \n после тех хелпов, после которых нужно
/// после sh_arg_parse argv[0] потерян
/// нельзя свои алиасы для --help, нельзя отключить --help
/// пишет в sh_out/sh_err, проверяя, что они не NULL, так же, как и остальной libsh
/// меняет только *argvp (т. е. argv), но не **argvp и не ***argvp. сигнатуры такие, т. к. нельзя сконвертировать char ** в const char ** и т. д. тем не менее, лямбда может принимать в качестве аргумента char * или const char *, sh_arg_operand возвращает char *. это сделано, т. к. иногда может понадобиться именно char *
/// соответствует требованиям POSIX 2013 к парсеру аргументов
/// используется следующая терминолония: всё - аргумент, с чёрточки - опция, не с чёрточки - операнд, аргумент к опции - параметр
/// главное преимущество этой либы - краткость кода

#ifndef _SH_ARG_HPP
#define _SH_ARG_HPP

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ex.h"

#include <initializer_list>
#include <string>
#include <tuple>
#include <vector>

template <typename func_t> bool
_sh_arg_disj (const func_t &func)
{
  return false;
}

// Yeah, x, xs
// Any sufficiently complicated C or Fortran program contains an ad hoc, informally-specified, bug-ridden, slow implementation of half of Common Lisp -Philip Greenspun
template <typename func_t, typename t, typename... ts> bool
_sh_arg_disj (const func_t &func, const t &x, const ts &... xs)
{
  return func (x) || _sh_arg_disj (func, xs...);
}

enum sh_arg_kind {sh_arg_optional = 0, sh_arg_mandatory};

template <typename func_t> struct _sh_arg_opt
{
private:
  std::vector<char> shorts;
  std::vector<const char *> longs;
  enum sh_arg_kind kind;
  func_t func;
  const char *param_name;
  const char *help;

  _sh_arg_opt (const std::vector<char> &shorts, const std::vector<const char *> &longs, enum sh_arg_kind kind, const func_t &func, const char *param_name, const char *help) :
    shorts (shorts), longs (longs), kind (kind), func (func), param_name (param_name), help (help)
  {
    if (false)
      {
        // Type checking
        char s[] = "C string";
        func (s);
      }
  }

  template <typename func2_t> friend struct _sh_arg_opt<func2_t>
  sh_arg_make_opt (const std::initializer_list<char> &shorts, const std::initializer_list<const char *> &longs, enum sh_arg_kind kind, const func2_t &func, const char *param_name, const char *help);

  template <typename... func_ts> friend void
  _sh_arg_help (const _sh_arg_opt<func_ts> &... opts);

  template <typename... func_ts> friend void
  sh_arg_parse (char ***argvp, const std::string &header, const std::string &footer, const _sh_arg_opt<func_ts> &... opts);
};

template <typename func_t> struct _sh_arg_opt<func_t>
sh_arg_make_opt (const std::initializer_list<char> &shorts, const std::initializer_list<const char *> &longs, enum sh_arg_kind kind, const func_t &func, const char *param_name, const char *help)
{
  return _sh_arg_opt<func_t> (shorts, longs, kind, func, param_name, help);
}

// SOMEDAY: автоматически выбирать ширину, а не всегда 25
template <typename... func_ts> void
_sh_arg_help (const _sh_arg_opt<func_ts> &... opts)
{
  fprintf (sh_get_out (), "  %-25s   display this help and exit\n", "    --help");

  std::initializer_list<int> unused = {([](const auto &o)
    {
      std::string line;

      for (const char &name : o.shorts)
        {
          line += "-";
          line += name;

          if (&name != &o.shorts.back () || !o.longs.empty ())
            {
              line += ", ";
            }
        }

      if (o.shorts.empty ())
        {
          line = "    ";
        }

      for (const auto &name : o.longs)
        {
          line += "--";
          line += name;

          if (&name != &o.longs.back ())
            {
              line += ", ";
            }
        }

      if (o.param_name != NULL)
        {
          if (o.kind == sh_arg_mandatory)
            {
              if (o.longs.empty ())
                {
                  line += " ";
                  line += o.param_name;
                }
              else
                {
                  line += "=";
                  line += o.param_name;
                }
            }
          else
            {
              if (o.longs.empty ())
                {
                  line += "[";
                  line += o.param_name;
                  line += "]";
                }
              else
                {
                  line += "[=";
                  line += o.param_name;
                  line += "]";
                }
            }
        }

      fprintf (sh_get_out (), "  %-25s   %s\n", line.c_str (), o.help);
    }
  (opts), 0)...};
}

// SOMEDAY: после бросания исключения argvp undefined, также бросание исключения может порождать утечки памяти и т. д., я не думал, как эти бросания взаимодействуют с лямбдами, то же для бросаний из лямбд
template <typename... func_ts> void
sh_arg_parse (char ***argvp, const std::string &header, const std::string &footer, const _sh_arg_opt<func_ts> &... opts)
{
#define _SH_ARG_ALL_OPTS help, opts...

  // Unfortunately, I cannot do "auto standard_opts = make_tuple (... [&standard_opts, ...](...){...} ...)"

  const auto help = sh_arg_make_opt ({}, {"help"}, sh_arg_optional, [header, footer, opts...](const char *)
    {
      if (sh_get_out () != NULL)
        {
          fprintf (sh_get_out (), "%s\n", header.c_str ());
          _sh_arg_help (opts...);
          fprintf (sh_get_out (), "\n%s", footer.c_str ());
        }

      exit (EXIT_SUCCESS);
    },
    NULL, "This is an unused string (due to bootstrapping problems)");

  ++argvp[0];

  for (;;)
    {
      if (argvp[0][0] == NULL)
        {
          break;
        }

      if (argvp[0][0][0] != '-')
        {
          break;
        }

      if (argvp[0][0][1] == '\0')
        {
          break;
        }
      else if (argvp[0][0][1] == '-')
        {
          if (argvp[0][0][2] == '\0')
            {
              ++argvp[0];
              break;
            }

          if (!_sh_arg_disj ([argvp](const auto &o)
            {
              for (const char *i : o.longs)
                {
                  if (strncmp (argvp[0][0] + 2, i, strlen (i)) == 0)
                    {
                      if (argvp[0][0][2 + strlen (i)] == '\0')
                        {
                          if (o.param_name == NULL)
                            {
                              o.func (NULL);
                              ++argvp[0];
                            }
                          else
                            {
                              if (o.kind == sh_arg_mandatory)
                                {
                                  if (argvp[0][1] == NULL)
                                    {
                                      sh_throwx ("option \"--%s\" requires an argument\nTry \"%s --help\" for more information.", i, sh_get_program ());
                                    }
                                  else
                                    {
                                      o.func (argvp[0][1]);
                                      argvp[0] += 2;
                                    }
                                }
                              else
                                {
                                  o.func (NULL);
                                  ++argvp[0];
                                }
                            }

                          return true;
                        }
                      else if (argvp[0][0][2 + strlen (i)] == '=')
                        {
                          if (o.param_name == NULL)
                            {
                              sh_throwx ("option \"--%s\" doesn't allow an argument\nTry \"%s --help\" for more information.", i, sh_get_program ());
                            }
                          else
                            {
                              o.func (argvp[0][0] + 2 + strlen (i) + 1);
                              ++argvp[0];
                            }

                          return true;
                        }
                    }
                }

              return false;
            },
            _SH_ARG_ALL_OPTS))
            {
              sh_throwx ("unrecognized option \"%s\"\nTry \"%s --help\" for more information.", argvp[0][0], sh_get_program ());
            }
        }
      else
        {
          for (const char *i = argvp[0][0] + 1; i[0] != '\0'; ++i)
            {
              if (!_sh_arg_disj ([argvp, &i](const auto &o)
                {
                  for (char j : o.shorts)
                    {
                      if (i[0] == j)
                        {
                          if (o.param_name == NULL)
                            {
                              o.func (NULL);
                              return true;
                            }
                          else
                            {
                              if (o.kind == sh_arg_mandatory)
                                {
                                  if (i[1] == '\0')
                                    {
                                      if (argvp[0][1] == NULL)
                                        {
                                          sh_throwx ("option \"-%c\" requires an argument\nTry \"%s --help\" for more information.", j, sh_get_program ());
                                        }
                                      else
                                        {
                                          o.func (argvp[0][1]);
                                          i = i + strlen (i) - 1;
                                          ++argvp[0];
                                          return true;
                                        }
                                    }
                                  else
                                    {
                                      o.func (i + 1);
                                      i = i + strlen (i) - 1;
                                      return true;
                                    }
                                }
                              else
                                {
                                  if (i[1] == '\0')
                                    {
                                      o.func (NULL);
                                      return true;
                                    }
                                  else
                                    {
                                      o.func (i + 1);
                                      i = i + strlen (i) - 1;
                                      return true;
                                    }
                                }
                            }
                        }
                    }

                  return false;
                },
                _SH_ARG_ALL_OPTS))
                {
                  sh_throwx ("unrecognized option \"-%c\"\nTry \"%s --help\" for more information.", i[0], sh_get_program ());
                }
            }

          ++argvp[0];
        }
    }

#undef _SH_ARG_ALL_OPTS
}

static char *
sh_arg_operand (char ***argvp)
{
  if (argvp[0][0] == NULL)
    {
      sh_throwx ("missing an operand\nTry \"%s --help\" for more information.", sh_get_program ());
    }

  char *result = argvp[0][0];

  ++argvp[0];

  return result;
}

static void
sh_arg_end (char *const *argv)
{
  if (argv[0] != NULL)
    {
      sh_throwx ("extra operand: %s\nTry \"%s --help\" for more information.", argv[0], sh_get_program ());
    }
}

#endif // ! _SH_ARG_HPP
