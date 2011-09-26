/* Copyright (C) 2011 Kevin W. Hamlen
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 * MA  02110-1301, USA.
 *
 * The latest version of this program can be obtained from
 * http://songs.sourceforge.net.
 */

#if HAVE_CONFIG_H
#  include "config.h"
#else
#  include "vsconfig.h"
#endif

#include "sbdchk.h"
#include "chars.h"

#if HAVE_STDIO_H
#  include <stdio.h>
#endif

#define NEXTCHAR(f,c) if (((c)=ws_fgetc(f))==wc_EOF)

/* reporthyphens(<infile>,<outfile>)
 *   Read .log file <infile> that was generated by LaTeX when it processed
 *   the .tex file generated by a previous invocation of this program with
 *   the -x option. From the .log file, generate a human-readable warning
 *   message for any chord that breaks a word in a place that LaTeX does
 *   not recognize as a valid hyphenation point for that word.
 *   Return 0 on success or non-zero on failure.
 */
int
reporthyphens(fin,fout)
  FILE *fin;
  FILE *fout;
{
  int c;
  const WCHAR *idstring=ws_lit(IDSTRING);
  const WCHAR *idp;
  unsigned int line, col;
  WCHAR sbdword[MAXWORDLEN+1], *swp;
  WCHAR texword[MAXWORDLEN+1], *twp;
  WCHAR dispword[MAXWORDLEN+1], *dwp;

  NEXTCHAR(fin,c) return 0;
  for (;;)
  {
    /* Scan the input file until we reach a line of the form
     *   "SBDCHK: <line> <word>"
     * where <line> is a number indicating the line number within the original
     * .sbd file where <word> occurs. <word> will have a hyphen (-) in it
     * wherever there was a chord in the original .sbd file, and an asterisk
     * (*) wherever there was a ligature-chord macro. */
    for (idp=idstring; *idp; )
    {
      if (c == *idp) ++idp; else idp=idstring;
      NEXTCHAR(fin,c) return 0;
    }
    if (ws_fscanf3(fin, ws_lit(fmt_sbdline), &line, &col, &sbdword) < 2)
    {
      fprintf(stderr, "sbdchk: Identifier \"" IDSTRING "\" not followed by"
                      " a valid line number and word.\n");
      return -2;
    }

    /* There might be trailing whitespace so skip to the end of the line. */
    do
    {
      NEXTCHAR(fin,c) return -2;
    }
    while (c!=wc_newline);

    /* The next line will be an "underfull hbox" warning.  Skip it. */
    do
    {
      NEXTCHAR(fin,c) return -2;
    }
    while (c!=wc_newline);

    /* The third line in the sequence will contain LaTeX's hyphenation of the
     * word.  The line will look like "[] <fontname> <hyphenation>" so skip
     * the first two words to reach the <hyphenation> part, which is the only
     * part we want. */
    if (ws_fscanf1(fin, ws_lit(fmt_texline), &texword) < 1)
    {
      fprintf(stderr, "sbdchk:%u: unable to read TeX hyphenation:", line);
      ws_fputs(sbdword, stderr);
      fprintf(stderr, "\n");
      return -2;
    }

    /* Compare the original .sbd word to LaTeX's word to find anywhere where
     * the .sbd word has a hyphen or asterisk but the LaTeX word does not have
     * a hyphen.  Those are places where there's a chord at a place that LaTeX
     * doesn't recognize as a hyphenation point. */
    for (swp=sbdword, twp=texword, dwp=dispword; *swp && *twp; )
    {
      if (*swp == *twp)
      {
        *dwp++ = *swp++;
        ++twp;
      }
      else if ((*swp==wc_asterisk) && (*twp==wc_hyphen))
      {
        *dwp++ = wc_hyphen;
        ++swp;
        ++twp;
      }
      else if (*twp == wc_hyphen)
        ++twp;
      else
        break;
    }
    ws_strcpy(dwp,swp);
    if (*swp || *twp)
    {
      ws_fprintf2(fout, ws_lit("%u:%u: Questionable hyphenation \""),
                  line, col);
      ws_fputs(dispword, fout);
      ws_fputs(ws_lit("\" (TeX allows \""), fout);
      ws_fputs(texword, fout);
      ws_fputs(ws_lit("\")\n"), fout);
    }

    /* Now find any hyphen in the .sbd word that falls with one of the
     * sequences "ff", "ffl", "ffi", "fi", or "fl".  These are places where a
     * ligature chord macro should have been used but wasn't. */
    for (swp=sbdword; *swp; ++swp)
      if ((*swp==wc_hyphen) && (swp>sbdword) && (*(swp-1)==wc_lit('f')))
      {
        if ((*(swp+1)==wc_lit('i')) || (*(swp+1)==wc_lit('l')))
        {
          ws_fprintf1(fout, ws_lit("%u: Hyphenation \""), line);
          ws_fputs(dispword, fout);
          ws_fputs(ws_lit("\" breaks "), fout);
          if ((swp-2>=sbdword) && (*(swp-2)==wc_lit('f')))
            ws_fputc(wc_lit('f'), fout);
          ws_fputc(wc_lit('f'), fout);
          ws_fputc(*(swp+1), fout);
          ws_fputs(ws_lit(" ligature.\n"), fout);
        }
        else if (*(swp+1)==wc_lit('f'))
        {
          ws_fprintf1(fout, ws_lit("%u: Hyphenation \""), line);
          ws_fputs(dispword, fout);
          ws_fputs(ws_lit("\" breaks ff"), fout);
          if ((*(swp+2)==wc_lit('l')) || (*(swp+2)==wc_lit('i')))
            ws_fputc(*(swp+2), fout);
          ws_fputs(ws_lit(" ligature.\n"), fout);
        }
      }
  }
}