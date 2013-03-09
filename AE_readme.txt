Anthony's Editor January 93
===========================

WHAT'S NEW since AE October 92

  o     Added .macro, .macro_define, and .literal support.
  o     Added .help_off command to disable initial help messages.
  o     .delete_left and .delete_right can now be undone.
  o     Resolve some of the 8-bit clean issues in getkey() and display().
  o     Dropped functionality where the AE_HUP file is saved in case of
        a write error on the primary filename, in favour of reporting
        the error and letting the user take action.
  o     Added validation of portable POSIX file name on write.
  o     The original filename provided on the command line is preserved;
        it is no longer replaced by the name used for the next file write.
  o     Fixed more portability issues concerning BSD vs. System V and
        K&R C vs. ANSI C.
  o     Compile macro -DPOSIX changed to -DTERMIOS since this macro
        name better describes what is being enabled.
  o     Changed sample configurations mode.rc and modeless.rc.

The original AE, in its obfuscated form, won "Best Utility" in the
1991 International Obfuscated C Code Contest.  Since that contest, AE
has been revised and extended in order to try new ideas and provide
better functionality, while still retaining a simple interface and
modualar design.

The distribution now composes of several files.  Below is a list of the
files in this distribution:

        readme          This file.
        ae.man          Manual reference and installation guide.
        copying         Public License similar, but not identical, to GNU's.

        makefile        Source makefile suitable for Make or AM.
        command.c       AE source.
        data.c              "
        display.c           "
        gap.c               "
        key.c               "
        main.c              "
        header.h            "
        key.h               "

        mode.rc         Sample configuration file modual style.
        modeless.rc     Sample configuration file modeless style (ansi keys).

AE merges two schools of thought by providing both VI style (modual)
and EMACS style (modeless) editing interfaces.  Users can configure the
command-key bindings and the help text for either style.

AE is simple enough that anyone can modify it to add features.  The
source is in the Public Domain, so people are free to modify and use
as they see fit.  

e.g.
        o  Tutorials on the Buffer Gap Scheme and/or editor design.
        o  A basis for an editor that can be built into a project.
        o  An editor for novice users.

AE has been know to compile on a wide variety of machines and compilers
like BSD and System V Unix with GNU C, PC mahcines with WatCom C or Turbo C,
and ATARI ST machines with Sozobon C.  Any machine that provides at least
K&R C and a CURSES library should have no trouble getting AE to compile.

See the INSTALLATION section of the manual on how to build AE.  Also
take note of the BUG section of the manual.

Anthony Howe
a...@mks.com

PS.     Available upon request is AM, Anthony's Make, winner of "Best
        Utility" in the 1992 International Obfuscated C Code Contest.
        Please specify either the entry or human-readable version.

        Also available upon request, Stdscr Curses for PC class
        machines.  For those people who cannot find a suitable Curses
        library to link AE with or use for other small projects.  This
        small Curses library supports only the standard full screen
        window, stdscr.

-- 
a...@mks.com                                                   Anthony C Howe
Mortice Kern Systems Inc. 35 King St. N., Waterloo, Ontario, Canada, N2J 6W9
"Nice legs.  For a human that is." - Worf (Q-pid) 