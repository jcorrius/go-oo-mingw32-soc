/* As we can't any longer include all the langpacks we want in the
 * installer because it would take forever to build it (issue #87401),
 * let's use a small wrapper that first runs the base OOo installer
 * (which might be multi-lingual as before, but with just a small
 * number of langpacks included), and then the langpack installers for
 * the system default UI language, the user default UI language if
 * different, and any other UI languages present in the system.
 */

#define WIN32_LEAN_AND_MEAN
#define _WIN32_WINNT 0x0500
#undef WINVER
#define WINVER 0x0500

#include <windows.h>

#include <process.h>
#include <stdlib.h>
#include <string.h>
#include <wchar.h>

#define NELEMS(a) (sizeof (a) / sizeof (a[0]))

static wchar_t path[1000];
static LANGID system_ui_lang;
static LANGID user_ui_lang;

static void
try_langpack (LANGID langid)
{
  LCID lcid;
  wchar_t lang[10], ctry[10];
  wchar_t locale[22];
  wchar_t langpack_setup[1000];

  lcid = MAKELCID (langid, SORT_DEFAULT);

  if (GetLocaleInfoW (lcid, LOCALE_SISO639LANGNAME, lang, NELEMS (lang)) &&
      GetLocaleInfoW (lcid, LOCALE_SISO3166CTRYNAME, ctry, NELEMS (ctry)))
    {
      wcscpy (locale, lang);
      wcscat (locale, L"-");
      wcscat (locale, ctry);

      wcscpy (langpack_setup, path);
      wcscat (langpack_setup, L"langpack/");
      wcscat (langpack_setup, locale);
      wcscat (langpack_setup, L"/setup.exe");

      if (GetFileAttributesW (langpack_setup) != INVALID_FILE_ATTRIBUTES)
        _wspawnl (_P_WAIT, langpack_setup, langpack_setup, NULL);
    }

  /* Then try just the language */
  if (GetLocaleInfoW (lcid, LOCALE_SISO639LANGNAME, locale, NELEMS (locale)))
    {
      wcscpy (langpack_setup, path);
      wcscat (langpack_setup, L"langpack/");
      wcscat (langpack_setup, locale);
      wcscat (langpack_setup, L"/setup.exe");

      if (GetFileAttributesW (langpack_setup) != INVALID_FILE_ATTRIBUTES)
        _wspawnl (_P_WAIT, langpack_setup, langpack_setup, NULL);
    }
}

static BOOL CALLBACK
enum_ui_lang_proc (LPTSTR language, LONG_PTR lParam)
{
  long langid = strtol(language, NULL, 16);

  if (langid > 0xFFFF ||
      langid == system_ui_lang ||
      langid == user_ui_lang)
    return TRUE;

  try_langpack (langid);

  return TRUE;
}

int
main (int argc, char **argv)
{
  wchar_t base_setup[1000];
  wchar_t *last_slash, *last_backslash;
  int rc;

  if (!GetModuleFileNameW (NULL, path, 500))
    {
      MessageBox (NULL, "GetModuleFileNameW failed", "setup-wrapper", MB_OK);
      exit (1);
    }

  last_slash = wcsrchr (path, L'/');
  last_backslash = wcsrchr (path, L'\\');

  if (last_slash != NULL &&
      last_backslash != NULL &&
      last_backslash > last_slash)
    last_slash = last_backslash;
  else if (last_slash == NULL)
    last_slash = last_backslash;

  last_slash[1] = L'\0';

  wcscpy (base_setup, path);
  wcscat (base_setup, L"base/setup.exe");

  rc =_wspawnl (_P_WAIT, base_setup, base_setup, NULL);

  if (rc != 0)
    exit (1);

  system_ui_lang = GetSystemDefaultUILanguage ();
  try_langpack (system_ui_lang);

  user_ui_lang = GetUserDefaultUILanguage ();
  if (user_ui_lang != system_ui_lang)
    try_langpack (user_ui_lang);

  EnumUILanguages (enum_ui_lang_proc, 0, 0);

  return 0;
}
