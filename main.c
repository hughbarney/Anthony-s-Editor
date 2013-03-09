/*
 * main.c              
 *
 * Anthony's Editor January 93
 *
 * Public Domain 1991, 1993 by Anthony Howe.  No warranty.
 */

#include <ctype.h>
#include <errno.h>
#include <string.h>
#include "header.h"

int
main(argc, argv)
int argc;
char **argv;
{
        int i;
        keymap_t *kp;
        char *ap, *config;

        /* Find basename. */
        prog_name = *argv;
        i = strlen(prog_name);
        while (0 <= i && prog_name[i] != '\\' && prog_name[i] != '/')
                --i;
        prog_name += i+1;

        /* Parse options. */
        config = CONFIG;
        for (--argc, ++argv; 0 < argc && **argv == '-'; --argc, ++argv) {
                ap = &argv[0][1];
                if (*ap == '-' && ap[1] == '\0') {
                        /* -- terminates options. */
                        --argc;
                        ++argv;
                        break;
                }
                while (*ap != '\0') {
                        switch (*ap++) {
                        case 'f':
                                /* -f <config_file>  or -f<config_file> */

                                if (*ap != '\0') {
                                        config = ap;
                                } else if (1 < argc) {
                                        config = *++argv;
                                        --argc;
                                } else {
                                        fatal(f_usage);
                                }
                                break;
                        default:
                                fatal(f_usage);
                        }
                        break;
                }
        }

        if (initscr() == NULL)
                fatal(f_initscr);
        if (initkey(config, &key_map) != INITKEY_OK)
                fatal(f_config);

        /* Determine if editor is modeless or not.
         * Define insert mode keys from the master table.
         */
        for (modeless = TRUE, kp = key_map; kp->code != K_ERROR; ++kp) {
                switch (kp->code) {
                case K_INSERT_ENTER:
                        modeless = FALSE;
                        break;
                case K_INSERT_EXIT:
                        kp->code = K_DISABLED;
                        key_mode[0].lhs = kp->lhs;
                        break;
                case K_STTY_ERASE:
                        key_mode[1].lhs = kp->lhs;
                        break;
                case K_LITERAL:
                        key_mode[2].lhs = kp->lhs;
                        break;
                case K_HELP_OFF:
                        textline = 0;
                        break;
                }
        }

        noecho();
        lineinput(FALSE);
        idlok(stdscr, TRUE);

        if (0 < argc) {
                (void) load(*argv);
                /* Save filename irregardless of load() success. */
                strcpy(filename, *argv);
                modified = FALSE;
        }
        if (!growgap(CHUNK))
                fatal(f_alloc);

        top();
        i = msgflag;
        help();
        msgflag = i;
        while (!done) {
                display();
                i = 0;
                input = getkey(key_map);
                while (table[i].key != 0 && input != table[i].key)
                        ++i;
                if (table[i].func != NULL)
                        (*table[i].func)();
                else if (modeless)
                        insert();
        }
        if (scrap != NULL)
                free(scrap);
        finikey(key_map);
        move(LINES-1, 0);
        refresh();
        endwin();
        putchar('\n');
        return (EXIT_OK);

}

#ifdef TERMIOS
#include <termios.h>

/*
 *      Set the desired input mode.
 *
 *      FALSE enables immediate character processing (disable line processing
 *      and signals for INTR, QUIT, and SUSP).  TRUE enables line processing
 *      and signals (disables immediate character processing).  In either
 *      case flow control (XON/XOFF) is still active.  
 *
 *      If the termios function calls fail, then fall back on using
 *      CURSES' cbreak()/nocbreak() functions; however signals will be
 *      still be in effect.
 */
void
lineinput(bf)
int bf;
{
        int error;
        struct termios term;
        error = tcgetattr(fileno(stdin), &term) < 0;
        if (!error) {
                if (bf)
                        term.c_lflag |= ISIG | ICANON;
                else
                        term.c_lflag &= ~(ISIG | ICANON);
                error = tcsetattr(fileno(stdin), TCSANOW, &term) < 0;
        }
        /* Fall back on CURSES functions that do almost what we need if
         * either tcgetattr() or tcsetattr() fail.
         */
        if (error) {
                if (bf)
                        nocbreak();
                else
                        cbreak();
        }
}

#endif /* TERMIOS */

void
fatal(m)
msg_t m;
{
        if (curscr != NULL) {
                move(LINES-1, 0);
                refresh();
                endwin();
                putchar('\n');
        }
        fprintf(stderr, m, prog_name);
        if (m == f_ok)
                exit(EXIT_OK);
        if (m == f_error)
                exit(EXIT_ERROR);
        if (m == f_usage)
                exit(EXIT_USAGE);
        exit(EXIT_FAIL);

}

#ifdef va_dcl
void
msg(va_alist)
va_dcl
{
        long num;
        char *f, *m;
        va_list args;

        va_start(args);
        for (f = va_arg(args, char *), m = msgline; *f != '\0'; ) {
                if (*f == '%') {
                        switch (*++f) {
                        case 's':
                                (void) strcpy(m, va_arg(args, char *));
                                break;
                        case 'l':
                                if (*++f != 'd') {
                                        (void) strcpy(m, "UNSUPPORTED");
                                        break;
                                }
                                num = va_arg(args, long);
                                /* fall */
                        case 'd':
                                if (f[-1] == '%')
                                        num = (long) va_arg(args, int);
                                sprintf(m, "%ld", num);
                                break;
                        }
                        m += strlen(m);
                        ++f;
                } else {
                        *m++ = *f++;
                }
        }
        *m = '\0';
        va_end(args);
        msgflag = TRUE;
}

#else /* not va_dcl */
#ifdef __STDC__
void
msg(msg_t m, ...)
#else
void
msg(m)
msg_t m;
#endif /* __STDC__ */
{
        va_list args;
        va_start(args, m);
        (void) vsprintf(msgline, m, args);
        va_end(args);
        msgflag = TRUE;
}

#endif /* va_dcl */

/*
 *      Convert a string to lower case.  Return the string pointer.
 */
char *
strlwr(str)
char *str;
{
        register char *s;
        for (s = str; *s != '\0'; ++s)
                if (isupper(*s))
                        *s = tolower(*s);
        return (str);

}

/*
 *      Make a duplicate of a string.  Return a pointer to an allocated
 *      copy of the string, or NULL if malloc() failed.
 */
char *
strdup(str)
const char *str;
{
        char *new;
        if ((new = (char*) malloc(strlen(str)+1)) != NULL)
                (void) strcpy(new, str);
        return (new);

}      

/*
 *
 */
char *
pathname(path, file)
char *path, *file;
{
        char *buf;
        size_t plen, flen;
        plen = path == NULL ? 0 : strlen(path);
        flen = file == NULL ? 0 : strlen(file);
        buf = (char*) malloc(plen + flen + 2);
        if (buf == NULL)
                return (NULL);
        (void) strcpy(buf, path);
        buf[plen] = '/';
        (void) strcpy(&buf[plen+1], file);
        return (buf);

}

/*
 * Open resource file.
 */
FILE *
openrc(fn)
char *fn;
{
        FILE *fp;
        char *ptr, *buf;

        if ((fp = fopen(fn, "r")) == NULL) {
                if ((ptr = getenv("HOME")) == NULL)
                        return (NULL);
                if ((buf = pathname(ptr, fn)) == NULL)
                        return (NULL);
                if ((fp = fopen(buf, "r")) == NULL) {
                        for (ptr = buf; *ptr != '\0'; ++ptr)
                                if (*ptr == '/')
                                        *ptr = '\\';
                        fp = fopen(buf, "r");
                }
                free(buf);
        }
        return (fp);

}

/*
 *      Get an arbitrarily long block of text from a file.
 *      The read is terminated when the given key-word is read;
 *      the keyword is discarded.  The buffer that is passed
 *      back in ptr will be NULL terminated.  If an error occurs,
 *      the contents of ptr will be undefined.
 */
int
getblock(fp, word, ptr)
FILE *fp;
char *word, **ptr;
{
        int ch;
        char *buf, *new;
        size_t wlen, blen, slen = 0;

        *ptr = NULL;
        if ((buf = (char *) malloc(blen = BUFSIZ)) == NULL)
                return (GETBLOCK_ALLOC);

        if (word == NULL)
                word = "";
        wlen = strlen(word);

        while ((ch = fgetc(fp)) != EOF) {
                buf[slen++] = ch;
                /* Check for terminating keyword. */
                if (wlen <= slen && strncmp(word, buf+slen-wlen, wlen) == 0) {
                        buf[slen-wlen] = '\0';
                        break;
                }
                if (blen <= slen) {
                        blen += BUFSIZ;
                        if ((new = (char*) realloc(buf, blen)) == NULL) {
                                free(buf);
                                return (GETBLOCK_ALLOC);
                        }
                        buf = new;
                }
                if (ch == '\0') {
                        *ptr = buf;
                        return (GETBLOCK_BINARY);
                }
        }

        if (ferror(fp)) {
                free(buf);
                return (GETBLOCK_ERROR);
        }
        if (slen == 0) {
                free(buf);
                return (GETBLOCK_EOF);
        }

        buf[slen++] = '\0';
        if ((new = (char *) realloc(buf, slen)) == NULL)
                new = buf;
        *ptr = new;
        return (GETBLOCK_OK);
}

